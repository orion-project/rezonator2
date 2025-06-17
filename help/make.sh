#! /bin/bash

echo "Build application manual."


SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. ${SCRIPT_DIR}/../scripts/helpers.sh
cd ${SCRIPT_DIR}/..


HELP_TOOL=qcollectiongenerator
${HELP_TOOL} -v
exit_if_fail

SOURCE_DIR=./help
TARGET_DIR=./out/help

if [ ${IS_MACOS} ]; then
  BIN_DIR=./bin/rezonator.app/Contents/MacOS
else
  BIN_DIR=./bin
fi


build_help_files() {
  print_header "Building html files..."
  python3 -m sphinx -b qthelp ${SOURCE_DIR} ${TARGET_DIR}
  exit_if_fail

  print_header "Building qt-help files..."
  cp ${SOURCE_DIR}/rezonator.qhcp ${TARGET_DIR}
  ${HELP_TOOL} ${TARGET_DIR}/rezonator.qhcp
  exit_if_fail

  print_header "Copy built help files to bin dir..."
  cp ${TARGET_DIR}/rezonator.qch ${BIN_DIR}
  cp ${TARGET_DIR}/rezonator.qhc ${BIN_DIR}
  exit_if_fail
}


prepare_assistant() {
  print_header "Checking Assistant app..."
  if [ ${IS_MACOS} ]; then
    ASSISTANT_SOURCE=${QMAKE_DIR}/Assistant.app/Contents/MacOS/Assistant
  else
    ASSISTANT_SOURCE=${QMAKE_DIR}/assistant
  fi
  echo "Source Assistant path is ${ASSISTANT_SOURCE}"

  ASSISTANT_TARGET=${BIN_DIR}/assistant
  echo "Target Assistant path is ${ASSISTANT_TARGET}"
  if [ -f ${ASSISTANT_TARGET} ]; then
    echo "Already there"
    return
  fi

  echo "Copy Assistant app to bin dir..."
  cp ${ASSISTANT_SOURCE} ${ASSISTANT_TARGET}

  echo "Modify Assistant's RPATH to make it runnable on dev system outside of Qt Creator..."
  if [ ${IS_MACOS} ]; then
    # There are two LC_RPATH sections in rezonator exe and only one has absolute path
    # example of line: "         path /Users/user/Qt/5.10.0/clang_64/lib (offset 12)"
    # the first `sed` removes the "path" word, the second one removes "(offset 12)" part
    REZONATOR_RPATH="$(otool -l ${BIN_DIR}/rezonator | grep 'path /' | sed -e 's/path//' | sed -e 's/(.*)//')"
    REZONATOR_RPATH=${REZONATOR_RPATH// /} # <-- remove all spaces
    echo "RPATH=${REZONATOR_RPATH}"
    install_name_tool -add_rpath ${REZONATOR_RPATH} ${ASSISTANT_TARGET}
  else
    # Example of line: "  RPATH                /home/user/Qt/5.10.0/gcc_64/lib"
    REZONATOR_RPATH="$(objdump -x ${BIN_DIR}/rezonator | grep RPATH | sed -e 's/^\s*RPATH\s*//')"
    echo "RPATH=${REZONATOR_RPATH}"
    if [[ -z "$REZONATOR_RPATH" ]]; then
      REZONATOR_RPATH="$(objdump -x ${BIN_DIR}/rezonator | grep RUNPATH | sed -e 's/^\s*RUNPATH\s*//')"
      echo "RUNPATH=${REZONATOR_RPATH}"
    fi
    if [[ -z "$REZONATOR_RPATH" ]]; then
      echo "Can not extract RPATH from rezonator exe, unable to run Assistant" 
    else
      # sudo apt install patchelf
      # Even with `--force-rpath` it sets not `RPATH` but `RUNPATH`, but it's works anyway
      patchelf --force-rpath --set-rpath "${REZONATOR_RPATH}" ${ASSISTANT_TARGET}
    fi
  fi
  exit_if_fail
}


run_assistant() {
  print_header "Running Assistant..."
  ${ASSISTANT_TARGET} -collectionFile ${BIN_DIR}/rezonator.qhc -style fusion
}


build_help_files

prepare_assistant

run_assistant

print_done
