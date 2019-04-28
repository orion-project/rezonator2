#! /bin/bash
#
# Build application manual.
#

# Check if Qt is in PATH
HELP_TOOL=qcollectiongenerator
HELP_TOOL_VER="$(${HELP_TOOL} -v)"
if [[ -z "${HELP_TOOL_VER}" ]]; then
  echo
  echo "ERROR: ${HELP_TOOL} is not found in PATH."
  echo "Find Qt installation and update your PATH like:"
  echo 'export PATH=/home/user/Qt/5.10.0/gcc_64/bin:$PATH'
  echo 'and run this script again.'
  exit
else
  echo
  echo "${HELP_TOOL_VER}"
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${SCRIPT_DIR}/..

SOURCE_DIR=./help
TARGET_DIR=./out/help

if [ "$(uname)" == "Darwin" ]; then
  BIN_DIR=./bin/rezonator.app/Contents/MacOS
else
  BIN_DIR=./bin
fi

build_help_files() {
  echo
  echo "Building html files..."
  python3 -m sphinx -b qthelp ${SOURCE_DIR} ${TARGET_DIR}
  if [ "${?}" != "0" ]; then exit 1; fi

  echo
  echo "Building qt-help files..."
  cp ${SOURCE_DIR}/rezonator.qhcp ${TARGET_DIR}
  ${HELP_TOOL} ${TARGET_DIR}/rezonator.qhcp
  if [ "${?}" != "0" ]; then exit 1; fi

  echo
  echo "Move built help files to bin dir..."
  mv ${TARGET_DIR}/rezonator.qhc ${BIN_DIR}
  mv ${TARGET_DIR}/rezonator.qch ${BIN_DIR}
  if [ "${?}" != "0" ]; then exit 1; fi
}


prepare_assistant() {
  echo
  echo "Checking Assistant app..."
  HELP_TOOL_PATH="$(which ${HELP_TOOL})"
  HELP_TOOL_DIR="$(dirname ${HELP_TOOL_PATH})"
  if [ "$(uname)" == "Darwin" ]; then
    ASSISTANT_SOURCE=${HELP_TOOL_DIR}/Assistant.app/Contents/MacOS/Assistant
  else
    ASSISTANT_SOURCE=${HELP_TOOL_DIR}/assistant
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
  
  echo "Modify Asisstant's RPATH to make it runnable on dev system outside of Qt Creator..."
  if [ "$(uname)" == "Darwin" ]; then
    # There are two LC_RPATH sections in rezonator exe and only one has absolute path
    # example of line: "         path /Users/user/Qt/5.10.0/clang_64/lib (offset 12)"
    # the first `sed` removes the "path" word, the second one removes "(offset 12)" part
    REZONATOR_RPATH="$(otool -l ${BIN_DIR}/rezonator | grep 'path /' | sed -e 's/path//' | sed -e 's/(.*)//')"
    REZONATOR_RPATH=${REZONATOR_RPATH// /} # <-- remove all spaces
    echo "RPATH=${REZONATOR_RPATH}"
    install_name_tool -add_rpath ${REZONATOR_RPATH} ${ASSISTANT_TARGET}
  else
    echo "TODO"
  fi 
  if [ "${?}" != "0" ]; then exit 1; fi 
}


run_assistant() {
  echo
  echo "Running Assistant..."
  ${ASSISTANT_TARGET} -collectionFile ${BIN_DIR}/rezonator.qhc -style fusion
}


build_help_files

prepare_assistant

run_assistant
