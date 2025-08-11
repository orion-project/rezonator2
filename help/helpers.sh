#! /bin/bash
#
# Helper functionality for reZonator scripts
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


print_header "Check platform..."
if [ "$(uname)" == "Darwin" ]; then IS_MACOS=1 ; else IS_LINUX=1 ; fi
if [ ${IS_LINUX} ]; then echo "Platform is Linux" ; fi
if [ ${IS_MACOS} ]; then echo "Platform is MacOS" ; fi


print_header "Check if Qt is in PATH..."
QMAKE_VER="$(qmake -v)"
if [[ -z ${QMAKE_VER} ]]; then
  echo
  echo -e "${COLOR_FAIL}ERROR: Qt is not found in PATH.${COLOR_RESET}"
  echo "Find Qt installation and update your PATH like:"
  if [ ${IS_LINUX} ]; then
    echo -e "${COLOR_BOLD}export PATH=/home/user/Qt/5.15.2/gcc_64/bin:\$PATH${COLOR_RESET}"
  else
    echo -e "${COLOR_BOLD}export PATH=/Users/user/Qt/5.15.2/clang_64/bin:\$PATH${COLOR_RESET}"
  fi
  echo "and run this script again."
  echo
  exit
fi
QMAKE_PATH="$(which qmake)"
QMAKE_DIR="$(dirname ${QMAKE_PATH})"
echo "${QMAKE_VER}"
echo "QMake dir: ${QMAKE_DIR}"


exit_if_fail() {
  if [ "${?}" != "0" ]; then exit 1; fi
}


create_dir_if_none() {
  print_header "Create '$1' dir if none..."
  if [ ! -d $1 ]; then
    mkdir $1
  else
    echo "Already there"
  fi
  exit_if_fail
}


print_done() {
  echo
  echo -e "${COLOR_OKGREEN}Done${COLOR_RESET}"
  echo
}
