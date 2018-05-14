#! /bin/bash
#
# Download and build required third party libs.
#

PROJ_ROOT=${PWD}
PROJ_BIN=${PWD}/bin
LIBS_DIR=${PROJ_ROOT}/libs

if [[ "$OSTYPE" == "linux-gnu" ]]; then
  PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  PLATFORM="macosx"
fi

function prepare_lua() {
  echo "-----------------------------------------------------------------"
  echo "Prepare Lua"

  LUA_DIR=lua-5.3.4
  LUA_PACKAGE=lua-5.3.4.tar.gz
  LUA_URL=http://www.lua.org/ftp/${LUA_PACKAGE}

  if [ -f ${LIBS_DIR}/${LUA_PACKAGE} ]; then
    rm ${LIBS_DIR}/${LUA_PACKAGE}
  fi
  if [ -d ${LIBS_DIR}/${LUA_DIR} ]; then
    rm -rfd ${LIBS_DIR}/${LUA_DIR}
  fi

  echo "------------------------"
  echo "Download sources"

  cd ${LIBS_DIR}
  if [ "$PLATFORM" == "macosx" ]; then
    curl -R -O $LUA_URL
  elif [ "$PLATFORM" == "linux" ]; then
    wget $LUA_URL
  else
    echo "Uknown how to download for platform $PLATFORM"
    exit -1
  fi

  echo "------------------------"
  echo "Unpack sources"

  tar zxf ${LUA_PACKAGE}
  rm ${LUA_PACKAGE}
  cd ${LUA_DIR}

  echo "------------------------"
  echo "Build"

  if [ "$PLATFORM" == "macosx" ]; then
    # We have to set lower target os version than it actually is
    # to avoid a lot of linker warnings when linking lib to application:
    # object file (.../liblua.a(lgc.o)) was built for newer OSX version (10.12) than being linked (10.10)
    # https://stackoverflow.com/questions/43216273/object-file-was-built-for-newer-osx-version-than-being-linked
    make MYCFLAGS="-mmacosx-version-min=10.10" MYLDFLAGS="-mmacosx-version-min=10.10" macosx test
  elif [ "$PLATFORM" == "linux" ]; then
    # I've only tested it on Ubuntu and additional dep is required 
    # which seems not to be installed by default
    sudo apt-get install libreadline-dev
    make linux test
  fi
}

prepare_lua
