#! /bin/bash

echo "Create redistributable dmg package."


SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. ${SCRIPT_DIR}/helpers.sh
cd ${SCRIPT_DIR}/..

create_dir_if_none "out"
cd out


populate_assistant() {
  print_header "Populate assistant bundle..."
  if [ -d Assistant.app ]; then rm -rf Assistant.app ; fi
  DEPLOY_TOOL_PATH="$(which macdeployqt)"
  QT_DIR="$(dirname ${DEPLOY_TOOL_PATH})"
  cp -r ${QT_DIR}/Assistant.app .
  exit_if_fail

  macdeployqt Assistant.app
  exit_if_fail

  ASSISTANT_EXE=./Assistant.app/Contents/MacOS/Assistant

  # `macdeployqt` (at least in Qt 5.10) doesn't add this RPATH to assistant exe, 
  # and as a result when packed to rezonator.app bundle it can't find libraries
  install_name_tool -add_rpath @loader_path/../Frameworks ${ASSISTANT_EXE}
  install_name_tool -delete_rpath @loader_path/../../../../lib ${ASSISTANT_EXE}
  exit_if_fail
}


populate_rezonator() {
  print_header "Populate application bundle..."
  if [ -d rezonator.app ]; then rm -rf rezonator.app ; fi
  cp -r ../bin/rezonator.app .

  macdeployqt rezonator.app
  exit_if_fail

  ASSISTANT_DIR=./Assistant.app/Contents
  REZONATOR_DIR=./rezonator.app/Contents

  echo "Add additional rezonator files..."
  cp -r ../bin/examples ${REZONATOR_DIR}/MacOS
  cp -r ../bin/test_files ${REZONATOR_DIR}/MacOS

  echo "Add Assistant and related libs..."
  cp ${ASSISTANT_DIR}/MacOS/Assistant ${REZONATOR_DIR}/MacOS/assistant
  cp -R ${ASSISTANT_DIR}/Frameworks/QtHelp.framework ${REZONATOR_DIR}/Frameworks
  cp -R ${ASSISTANT_DIR}/Frameworks/QtNetwork.framework ${REZONATOR_DIR}/Frameworks
  cp -R ${ASSISTANT_DIR}/Frameworks/QtSql.framework ${REZONATOR_DIR}/Frameworks
  mkdir ${REZONATOR_DIR}/PlugIns/sqldrivers
  cp ${ASSISTANT_DIR}/PlugIns/sqldrivers/libqsqlite.dylib ${REZONATOR_DIR}/PlugIns/sqldrivers
  cp -r ${ASSISTANT_DIR}/PlugIns/bearer ${REZONATOR_DIR}/PlugIns
  exit_if_fail
}


pack_to_dmg() {
  print_header "Pack application bundle to dmg..."
  if [ -f tmp.dmg ]; then rm tmp.dmg ; fi
  if [ -f rezonator.dmg ]; then rm rezonator.dmg ; fi
  
  hdiutil create tmp.dmg -ov -volname "reZonator" -fs HFS+ -srcfolder "rezonator.app" 
  hdiutil convert tmp.dmg -format UDZO -o rezonator.dmg
  rm tmp.dmg
  exit_if_fail
}


rename_to_version() {
  print_header "Rename resulting file to contain version..."
  if [ -f ../release/version.txt ]; then
    VERSION="$(cat ../release/version.txt)"
    RELEASE_DMG=rezonator-${VERSION}.dmg
    if [ -f ${RELEASE_DMG} ]; then rm ${RELEASE_DMG} ; fi
    mv rezonator.dmg ${RELEASE_DMG}
  else
    echo
    echo -e "${COLOR_WARNING}Warning: Unknown release version.${COLOR_RESET}"
    echo "Run release/make_version.py script to generate version number."
    exit 1
  fi
}


populate_assistant

populate_rezonator

rm -rf Assistant.app

pack_to_dmg

rename_to_version

print_done
