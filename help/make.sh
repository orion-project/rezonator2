#! /bin/bash
#
# Build application manual.
#

# https://misc.flogisoft.com/bash/tip_colors_and_formatting
COLOR_HEADER=$"\033[95m"
COLOR_OKBLUE=$"\033[94m"
COLOR_OKGREEN=$"\033[92m"
COLOR_WARNING=$"\033[93m"
COLOR_FAIL=$"\033[91m"
COLOR_BOLD=$"\033[1m"
COLOR_UNDERLINE=$"\033[4m"
COLOR_RESET=$"\033[0m"

print_header() {
  echo
  echo -e "${COLOR_HEADER}$1${COLOR_RESET}"
}

# Check if Qt is in PATH
HELP_TOOL=qcollectiongenerator
HELP_TOOL_VER="$(${HELP_TOOL} -v)"
if [[ -z "${HELP_TOOL_VER}" ]]; then
  echo
  echo -e "${COLOR_FAIL}ERROR: ${HELP_TOOL} is not found in PATH.${COLOR_RESET}"
  echo "Find Qt installation and update your PATH like:"
  echo -e "${COLOR_BOLD}export PATH=/home/user/Qt/5.10.0/gcc_64/bin:\$PATH${COLOR_RESET}"
  echo "and run this script again."
  echo
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
  print_header "Building html files..."
  python3 -m sphinx -b qthelp ${SOURCE_DIR} ${TARGET_DIR}
  if [ "${?}" != "0" ]; then exit 1; fi

  print_header "Building qt-help files..."
  cp ${SOURCE_DIR}/rezonator.qhcp ${TARGET_DIR}
  ${HELP_TOOL} ${TARGET_DIR}/rezonator.qhcp
  if [ "${?}" != "0" ]; then exit 1; fi

  print_header "Move built help files to bin dir..."
  mv ${TARGET_DIR}/rezonator.qhc ${BIN_DIR}
  mv ${TARGET_DIR}/rezonator.qch ${BIN_DIR}
  if [ "${?}" != "0" ]; then exit 1; fi
}


prepare_assistant() {
  print_header "Checking Assistant app..."
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
    # Example of line: "  RPATH                /home/user/Qt/5.10.0/gcc_64/lib"
    REZONATOR_RPATH="$(objdump -x ${BIN_DIR}/rezonator | grep RPATH | sed -e 's/^\s*RPATH\s*//')"
    echo "RPATH=${REZONATOR_RPATH}"
    # sudo apt install patchelf
    # Even with `--force-rpath` it sets not `RPATH` but `RUNPATH`, but it's works anyway
    patchelf --force-rpath --set-rpath "${REZONATOR_RPATH}" ${BIN_DIR}/assistant
  fi
  if [ "${?}" != "0" ]; then exit 1; fi
}


run_assistant() {
  print_header "Running Assistant..."
  ${ASSISTANT_TARGET} -collectionFile ${BIN_DIR}/rezonator.qhc -style fusion
}


build_help_files

prepare_assistant

run_assistant

echo
echo -e "${COLOR_OKGREEN}Done${COLOR_RESET}"
echo
