#!/usr/bin/env bash

quit() {
  echo "Failure in $1"
  exit 1
}

# 文件位置
PROJECT=ammon
WORK_DIR=$(pwd)
BUILD=$WORK_DIR/build

if [[ "$WORK_DIR" != */${PROJECT} ]]; then
  echo "Should be executed in ${PROJECT}"
  exit 1
fi

# 目标文件
TARGET_SOURCE="$1"
shift

AMMON_DEBUG=off

# -d -a
while getopts ":ad" opt; do
  case ${opt} in
    a)
      rm "$BUILD" -rf
      ;;
    d)
      AMMON_DEBUG=on
      ;;
    *) ;;
  esac
done

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

# build
# -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B "$BUILD" -DAMMON_DEBUG:STRING="$AMMON_DEBUG" ||
  quit "Configure Stage"
bear --append -- cmake --build "$BUILD" -- -j "$(nproc)" || quit "Build Stage"

# run
"$BUILD"/ammon "$TARGET_SOURCE" -- -I/usr/lib/gcc/x86_64-pc-linux-gnu/"$(gcc -dumpversion)"/include/
