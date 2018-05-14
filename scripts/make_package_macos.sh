#! /bin/bash
#
# Create redistributable dmg package.
#

# Check if Qt is in PATH
if [[ -z "$(qmake -v)" ]]; then
  echo
  echo "ERROR: Qt is not found in PATH."
  echo "Find Qt installation and update your PATH like:"
  echo 'PATH=/Users/user/Qt/5.10.0/clang_64/bin:$PATH'
  echo 'and run this script again.'
  exit
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${SCRIPT_DIR}/..

# Create out dir if none
if [ ! -d out ]; then mkdir out; fi
cd out

if [ -d AppDir ]; then rm -rdf AppDir; fi
mkdir AppDir
cd AppDir
cp -r ../../bin/rezonator.app .
macdeployqt rezonator.app -dmg
if [ "${?}" != "0" ]; then exit 1; fi

# Rename resulting file to contain version
if [ -f ../../release/version.txt ]; then
  VERSION="$(cat ../../release/version.txt)"
  cp rezonator.dmg rezonator-${VERSION}.dmg
else
  echo
  echo "Warning: Unknown release version."
  echo "Run release/make_version.py script to generate version number."
fi