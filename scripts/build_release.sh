#! /bin/bash

echo "Build release version of the application."

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. ${SCRIPT_DIR}/helpers.sh
cd ${SCRIPT_DIR}/..

create_dir_if_none "out"
cd out


print_header "Building..."
if [ -d BuildDir ]; then rm -rdf BuildDir; fi
mkdir BuildDir
cd BuildDir
qmake -config release ../../rezonator.pro
make
exit_if_fail

print_done
