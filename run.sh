#!/usr/bin/env bash

quit() {
  echo "Failure in $1"
  exit 1
}

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

# 文件位置
PROJECT=ammon
WORK_DIR=$(pwd)

if [[ "$WORK_DIR" != */${PROJECT} ]]; then
  echo "Should be executed in ${PROJECT}"
  exit 1
fi

BUILD=$WORK_DIR/build
EXE=$BUILD/ammon
# 目标文件
TARGET_SOURCE="$1"
shift
AMMON_DEBUG=off
TYPE_OPTION=-1

# -d -a
while getopts ":adt:" opt; do
  case ${opt} in
    a)
      rm "$BUILD" -rf
      ;;
    d)
      AMMON_DEBUG=on
      ;;
    t)
      TYPE_OPTION="$OPTARG"
      ;;
    *) ;;
  esac
done

# build
cmake -S . -B "$BUILD" -G Ninja -DAMMON_DEBUG:STRING="$AMMON_DEBUG" ||
  quit "Configure Stage"
bear --append -- cmake --build "$BUILD" -- -j "$(nproc)" || quit "Build Stage"

# run
"$EXE" "$TARGET_SOURCE" -t "$TYPE_OPTION" --
