#! /bin/bash

function exit_if_error() {
 if [ "${?}" != "0" ]; then exit 1; fi
}

function remove_dir_if_exists() {
  if [ -d $1 ]; then rm -rdf $1; fi
}

#-----------------------------------------------------------------------

function usage() {
  echo
  echo "Use options:"
  echo "-b Rebuild project in release mode"
  echo "-a Make standalone AppImage package"
  exit 1
}

while getopts "ba" opt_name; do
  case "$opt_name" in
    b) DO="true" DO_BUILD="true";;
    a) DO="true" DO_APPIMAGE="true";;
    *) usage;;
  esac
done

if [[ -z "${DO}" ]]; then
  echo "Nothing to be done."
  usage
fi

#-----------------------------------------------------------------------

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${SCRIPT_DIR}/..

# Create out dir if none
if [ ! -d out ]; then
  mkdir out
fi
cd out

#-----------------------------------------------------------------------

if [[ "${DO_BUILD}" == "true" ]]; then
  echo
  echo "Building release..."

  # Check if qmake in path
  QMAKE_VER="$(qmake -v)"
  if [[ -z "${QMAKE_VER}" ]]; then
    echo
    echo "ERROR: qmake is not found in PATH."
    echo "Find Qt installation and update your PATH like:"
    echo 'PATH=/home/user/Qt/5.10.0/gcc_64/bin:$PATH'
    echo 'and run this script again.'
    exit
  else
    echo
    echo "${QMAKE_VER}"
  fi

  # Update version info
  echo
  echo "Updating version number..."
  python ../release/make_version.py
  
  # Build release
  echo
  echo "Building..."
  remove_dir_if_exists BuildDir
  mkdir BuildDir
  cd BuildDir
  qmake -config release ../../rezonator.pro
  make
  exit_if_error

  echo
  echo "Building done"

  cd ..
fi

#-----------------------------------------------------------------------

if [[ "${DO_APPIMAGE}" == "true" ]]; then
  echo
  echo "Making AppImage..."

  # Download linuxdeplyqt if none
  # https://github.com/probonopd/linuxdeployqt
  LINUXDEPLOYQT=linuxdeployqt-continuous-x86_64.AppImage
  if [ ! -f ${LINUXDEPLOYQT} ]; then
    echo
    echo "Downloading ${LINUXDEPLOYQT}..."
    wget -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/${LINUXDEPLOYQT}
    chmod a+x ${LINUXDEPLOYQT}
    exit_if_error
  fi

  # Create AppDir structure
  echo
  echo "Creating AppDir structure..."
  remove_dir_if_exists AppDir
  mkdir -p AppDir/usr/bin
  mkdir -p AppDir/usr/lib
  mkdir -p AppDir/usr/share/applications
  mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps
  cp ../bin/rezonator AppDir/usr/bin
  cp ../release/rezonator.desktop AppDir/usr/share/applications
  cp ../img/icon/icon_main_2_256.png AppDir/usr/share/icons/hicolor/256x256/apps/rezonator.png
  exit_if_error

  # Run linuxdeplyqt on the AppDir
  echo
  echo "Creating AppImage..."
  # qmake must be in PATH, we can try to extract its path from RPATH of app binary
  RPATH="$(objdump -x ../bin/rezonator | grep RPATH | sed -e 's/^\s*RPATH\s*//')"
  PATH=${RPATH}/../bin:${PATH}
  ./${LINUXDEPLOYQT} AppDir/usr/share/applications/rezonator.desktop -appimage -no-translations
  exit_if_error

  # Rename resulting file to contain version
  if [ -f ../release/version.txt ]; then
    VERSION="$(cat ../release/version.txt)"
    cp rezonator-x86_64.AppImage rezonator-${VERSION}-x86_64.AppImage
  else
    echo
    echo "Warning: Unknown release version."
    echo "Run ../release/make_version.py script to generate version number."
  fi

  echo
  echo "AppImage created"
fi
