#! /bin/bash

echo "Create redistributable AppImage package."


SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. ${SCRIPT_DIR}/helpers.sh
cd ${SCRIPT_DIR}/..

create_dir_if_none "out"
cd out


download_linuxdeployqt() {
  print_header "Download linuxdeplyqt if none..."
  # Download linuxdeplyqt if none (https://github.com/probonopd/linuxdeployqt)
  # NOTE: It've broken compatibility with newer OS versions forsing to stick at Ubuntu 14 LTS.
  # See discussion here: https://github.com/probonopd/linuxdeployqt/issues/340
  # But I have nor a machine running Trusty or a wish to stick at Qt 5.5
  # (the last supported for Trusty) so have to use a more relaxed 5th version of the tool.
  #LINUXDEPLOYQT=linuxdeployqt-continuous-x86_64.AppImage
  #LINUXDEPLOYQT_URL=https://github.com/probonopd/linuxdeployqt/releases/download/continuous/${LINUXDEPLOYQT}
  LINUXDEPLOYQT=linuxdeployqt-5-x86_64.AppImage
  LINUXDEPLOYQT_URL=https://github.com/probonopd/linuxdeployqt/releases/download/5/${LINUXDEPLOYQT}
  if [ ! -f ${LINUXDEPLOYQT} ]; then
    echo "Downloading ${LINUXDEPLOYQT}..."
    wget -c ${LINUXDEPLOYQT_URL}
    chmod a+x ${LINUXDEPLOYQT}
    exit_if_fail
  else
    echo "Already there"
  fi
}


create_app_dir() {
  print_header "Creating AppDir structure..."
  if [ -d AppDir ]; then rm -rdf AppDir; fi
  mkdir -p AppDir/usr/bin
  mkdir -p AppDir/usr/lib
  mkdir -p AppDir/usr/share/applications
  mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps
  cp ../bin/rezonator AppDir/usr/bin
  cp ${QMAKE_DIR}/assistant AppDir/usr/bin
  cp ../bin/rezonator.qch AppDir/usr/bin
  cp ../bin/rezonator.qhc AppDir/usr/bin
  cp -r ../bin/examples AppDir/usr/bin
  cp -r ../bin/test_files AppDir/usr/bin
  cp ../release/rezonator.desktop AppDir/usr/share/applications
  cp ../img/icon/main_2_256.png AppDir/usr/share/icons/hicolor/256x256/apps/rezonator.png
  exit_if_fail
}


run_linuxdeplyqt() {
  print_header "Creating AppImage..."
  # qmake must be in PATH for `linuxdeployqt`

  echo
  echo "Bundle assistant..."
  ./${LINUXDEPLOYQT} AppDir/usr/bin/assistant \
    -no-translations \
    -no-copy-copyright-files \
    -exclude-libs=libqsqlmysql,libqsqlodbc,libqsqlpsql

  echo
  echo "Bundle rezonator..."
  ./${LINUXDEPLOYQT} AppDir/usr/share/applications/rezonator.desktop \
    -appimage \
    -no-translations \
    -no-copy-copyright-files \
    -exclude-libs=libqsqlmysql,libqsqlodbc,libqsqlpsql
  exit_if_fail
}


rename_to_version() {
  # Rename resulting file to contain version
  if [ -f ../release/version.txt ]; then
    VERSION="$(cat ../release/version.txt)"
    cp rezonator-x86_64.AppImage rezonator-${VERSION}-x86_64.AppImage
  else
    echo
    echo -e "${COLOR_WARNING}Warning: Unknown release version.${COLOR_RESET}"
    echo "Run 'release/make_version.py' script to generate version number."
    exit 1
  fi
}


download_linuxdeployqt

create_app_dir

run_linuxdeplyqt

rename_to_version

print_done
