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

echo
echo "Checking Assistant app..."
HELP_TOOL_PATH="$(which ${HELP_TOOL})"
HELP_TOOL_DIR="$(dirname ${HELP_TOOL_PATH})"
if [ "$(uname)" == "Darwin" ]; then
  ASSISTANT_SOURCE=${HELP_TOOL_DIR}/Assistant.app/Contents/MacOS/Assistant
  ASSISTANT_TARGET=${BIN_DIR}/rezonator.app/Contents/MacOS/Assistant
else
  ASSISTANT_SOURCE=${HELP_TOOL_DIR}/assistant
  ASSISTANT_TARGET=${BIN_DIR}/assistant
fi
echo "Assistant path is ${ASSISTANT_SOURCE}"
if [ ! -f ${ASSISTANT_TARGET} ]; then
  echo "Copy Assistant app to bin dir..."
  cp -T ${ASSISTANT_SOURCE} ${ASSISTANT_TARGET}
fi
if [ "${?}" != "0" ]; then exit 1; fi

echo
echo "Running Assistant..."
${ASSISTANT_SOURCE} -collectionFile ${BIN_DIR}/rezonator.qhc -style fusion
