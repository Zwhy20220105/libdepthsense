#!/usr/bin/env bash

SCRIPT_PATH="`dirname \"$0\"`"
cd ${SCRIPT_PATH}/

rm -f ros/src/CMakeLists.txt
rm -rf ros/build/
rm -rf ros/devel/