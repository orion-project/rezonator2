#! /bin/bash
#
# Build application manual.
#

# Check if Qt is in PATH
HELP_TOOL=qcollectiongenerator
HELP_TOOL_VER="$(${HELP_TOOL} -v)"
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
BIN_DIR=./bin

if [ "$(uname)" == "Darwin" ]; then
  HELP_TOOL_PATH="$(which ${HELP_TOOL})"
  HELP_TOOL_DIR="$(dirname ${HELP_TOOL_PATH})"
  ASSISTANT=${HELP_TOOL_DIR}/Assistant.app/Contents/MacOS/Assistant
else
  ASSISTANT=assistant
fi

python3 -m sphinx -b qthelp ${SOURCE_DIR} ${TARGET_DIR}

cp ${SOURCE_DIR}/rezonator.qhcp ${TARGET_DIR}

${HELP_TOOL} ${TARGET_DIR}/rezonator.qhcp

mv ${TARGET_DIR}/rezonator.qhc ${BIN_DIR}
mv ${TARGET_DIR}/rezonator.qch ${BIN_DIR}

${ASSISTANT} -collectionFile ${BIN_DIR}/rezonator.qhc -style fusion
