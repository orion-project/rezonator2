#! /bin/bash
#
# Build application manual.
#

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${SCRIPT_DIR}/..

python3 -m sphinx -b html ./help ./out/help
