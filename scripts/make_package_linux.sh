#! /bin/bash
#
# Create redistributable AppImage package.
#

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${SCRIPT_DIR}/..

# Create out dir if none
if [ ! -d out ]; then mkdir out; fi
cd out

# Download linuxdeplyqt if none
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
  echo
  echo "Downloading ${LINUXDEPLOYQT}..."
  wget -c ${LINUXDEPLOYQT_URL}
  chmod a+x ${LINUXDEPLOYQT}
  if [ "${?}" != "0" ]; then exit 1; fi
fi

# Create AppDir structure
echo
echo "Creating AppDir structure..."
if [ -d AppDir ]; then rm -rdf AppDir; fi
mkdir -p AppDir/usr/bin
mkdir -p AppDir/usr/lib
mkdir -p AppDir/usr/share/applications
mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps
cp ../bin/rezonator AppDir/usr/bin
cp -r ../bin/examples AppDir/usr/bin
cp -r ../bin/test_files AppDir/usr/bin
cp ../release/rezonator.desktop AppDir/usr/share/applications
cp ../img/icon/main_2_256.png AppDir/usr/share/icons/hicolor/256x256/apps/rezonator.png
if [ "${?}" != "0" ]; then exit 1; fi

# Run linuxdeplyqt on the AppDir
echo
echo "Creating AppImage..."
# qmake must be in PATH, we can try to extract its path from RPATH of app binary
RPATH="$(objdump -x ../bin/rezonator | grep RPATH | sed -e 's/^\s*RPATH\s*//')"
PATH=${RPATH}/../bin:${PATH}
./${LINUXDEPLOYQT} AppDir/usr/share/applications/rezonator.desktop -appimage -no-translations
if [ "${?}" != "0" ]; then exit 1; fi

# Rename resulting file to contain version
if [ -f ../release/version.txt ]; then
  VERSION="$(cat ../release/version.txt)"
  cp rezonator-x86_64.AppImage rezonator-${VERSION}-x86_64.AppImage
else
  echo
  echo "Warning: Unknown release version."
  echo "Run release/make_version.py script to generate version number."
fi

