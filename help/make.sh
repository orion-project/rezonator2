#! /bin/bash
#
# Build application manual.
#

# Check if Qt is in PATH
HELP_TOOL_VER="$(qcollectiongenerator -v)"
if [[ -z "${HELP_TOOL_VER}" ]]; then
  echo
  echo "ERROR: qmake is not found in PATH."
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

python3 -m sphinx -b qthelp ${SOURCE_DIR} ${TARGET_DIR}

qcollectiongenerator ${TARGET_DIR}/rezonator2.qhcp

assistant -collectionFile ${TARGET_DIR}/rezonator2.qhc
