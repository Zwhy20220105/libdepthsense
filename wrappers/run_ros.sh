#!/bin/bash

SCRIPT_PATH="`dirname \"$0\"`"
cd ${SCRIPT_PATH}/

source ros/devel/setup.bash

roslaunch depthsense_publisher depthsense_publisher.launch