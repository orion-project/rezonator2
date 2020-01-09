#! /bin/bash

EXE="svgcleaner"

if [ ! -f ${EXE} ]; then
  echo "${EXE} executable not found, download it from here:"
  echo "https://github.com/RazrFalcon/svgcleaner/releases/download/v0.9.5/svgcleaner_linux_x86_64_0.9.5.tar.gz"
  exit
fi

IN_FILE=$1

if [ ! -f ${IN_FILE} ]; then
  echo "Input file ${IN_FILE} not found"
  exit
fi

IN_FILE_DIR=`dirname "$IN_FILE"`
IN_FILE_NAME=`basename "$IN_FILE"`
IN_FILE_EXT="${IN_FILE_NAME##*.}"
IN_FILE_NAME="${IN_FILE_NAME%.*}"

OUT_FILE="${IN_FILE_DIR}/${IN_FILE_NAME}_clean.${IN_FILE_EXT}"

./${EXE} \
  --coordinates-precision=2 \
  --properties-precision=2 \
  --transforms-precision=2 \
  --paths-coordinates-precision=2 \
  --indent=0 \
  ${IN_FILE} \
  ${OUT_FILE}

echo "Saved to ${OUT_FILE}"
