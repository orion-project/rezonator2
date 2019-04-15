#! /bin/bash
#
# Build application manual.
#

# Check if Sphinx is in PATH
SPHINX_VER="$(sphinx-build --version)"
if [[ -z "${SPHINX_VER}" ]]; then
  echo
  echo "ERROR: sphinx-build command was not found. Make sure you have Sphinx installed"
  exit
else
  echo
  echo "${SPHINX_VER}"
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${SCRIPT_DIR}/..

sphinx-build -b html ./help ./out/help
