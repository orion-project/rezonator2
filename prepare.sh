#! /bin/bash

PROJ_ROOT=${PWD}
PROJ_BIN=${PWD}/bin

echo "-----------------------------------------------------------------"
echo "Prepare muparser"

MUPARSER=${PROJ_ROOT}/libs/muparser
MUPARSER_LIB=libmuparser.so.2

echo "------------------------"
echo "Clone repo"

if [ -d ${MUPARSER} ]; then
    rm -rf ${MUPARSER}
fi
mkdir ${MUPARSER}
git clone https://github.com/beltoforion/muparser ${MUPARSER}
cd ${MUPARSER}
git checkout v2.2.5

echo "------------------------"
echo "Build"

./configure
make

echo "------------------------"
echo "Copy to bin"

if [ -f ${PROJ_BIN}/${MUPARSER_LIB} ]; then
    rm ${PROJ_BIN}/${MUPARSER_LIB}
fi
ln -s ${MUPARSER}/lib/${MUPARSER_LIB} ${PROJ_BIN}/${MUPARSER_LIB}

echo "-----------------------------------------------------------------"
echo "Marking executable files"

cd ${PROJ_BIN}
chmod +x rezonator.sh
chmod +x rezonator_test.sh
chmod +x rezonator_test_nogui.sh
