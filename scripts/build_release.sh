#! /bin/bash

echo "Build release version of the application."

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. ${SCRIPT_DIR}/helpers.sh
cd ${SCRIPT_DIR}/..

create_dir_if_none "out"
cd out


print_header "Building..."

START_TIME=`date +%s`

if [ -d BuildDir ]; then rm -rdf BuildDir; fi
mkdir BuildDir
cd BuildDir
qmake -config release ../../rezonator.pro
make

END_TIME=`date +%s`
echo
echo "Finished in `expr $END_TIME - $START_TIME` seconds"
echo
