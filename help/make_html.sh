#! /bin/bash

echo "Build application manual (HTML version)."

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. ${SCRIPT_DIR}/../scripts/helpers.sh
cd ${SCRIPT_DIR}/..

SOURCE_DIR=${SCRIPT_DIR}
TARGET_DIR=${SCRIPT_DIR}/../out/help_html

print_header "Building html files..."
python3 -m sphinx -b html ${SOURCE_DIR} ${TARGET_DIR}
exit_if_fail

print_done
