#! /bin/bash

PROJ_ROOT=${PWD}
PROJ_BIN=${PWD}/bin
LIBS_DIR=${PROJ_ROOT}/libs

function prepare_lua() {
  echo "-----------------------------------------------------------------"
  echo "Prepare Lua"

  LUA_DIR=lua-5.3.4
  LUA_PACKAGE=lua-5.3.4.tar.gz

  if [ -f ${LIBS_DIR}/${LUA_PACKAGE} ]; then
    rm ${LIBS_DIR}/${LUA_PACKAGE}
  fi
  if [ -d ${LIBS_DIR}/${LUA_DIR} ]; then
    rm -rfd ${LIBS_DIR}/${LUA_DIR}
  fi

  echo "------------------------"
  echo "Download sources"

  cd ${LIBS_DIR}
  wget http://www.lua.org/ftp/${LUA_PACKAGE}

  echo "------------------------"
  echo "Unpack sources"

  tar zxf ${LUA_PACKAGE}
  rm ${LUA_PACKAGE}
  cd ${LUA_DIR}

  echo "------------------------"
  echo "Build"

  # sudo apt-get install libreadline-dev
  make linux test
}

function mark_executables() {
  echo "-----------------------------------------------------------------"
  echo "Marking executable files"

  cd ${PROJ_BIN}
  chmod +x rezonator.sh
  chmod +x rezonator_test.sh
  chmod +x rezonator_test_nogui.sh
}

prepare_lua
mark_executables
