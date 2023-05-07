#!/usr/bin/env bash
set -e

SCRIPT_PATH="`dirname \"$0\"`"
cd ${SCRIPT_PATH}/..

X86_64_LIB=x86_64
ANDROID_ARMEABI_V7A_LIB=android-armeabi-v7a
ANDROID_ARM64_V8A_LIB=android-arm64-v8a
LINUX_ARMEABI_V7A_LIB=linux-armeabi-v7a
LINUX_ARM64_V8A_LIB=linux-arm64-v8a

# prepare library for wrappers
function preparelib(){
	TARGET_DIR=build-${1}
	LIB_DIR=${1}
	if [ -d "./${TARGET_DIR}" ]; then
		if test -f "./${TARGET_DIR}/libdepthsense.a"; then
			mkdir -p tmp
			find ./${TARGET_DIR} -name "*.a" -exec cp {} tmp/ \;
			cd tmp
			find . -name "*.a" -exec ar -x {} \;
			rm -rf *.a
			ar -qcs libdepthsense.a *.o
			cd ..
			mkdir -p lib/${LIB_DIR}
			cp ./tmp/libdepthsense.a lib/${LIB_DIR}/
			rm -rf tmp
		fi
		echo "${LIB_DIR} library is ready."
	fi
}

function testlib(){
	if test -f "./lib/${1}/libdepthsense.a"; then
		echo "Found ${1} lib."
	else
		echo "Not found ${1} lib."
	fi
}

# prepare lib
preparelib ${X86_64_LIB}
preparelib ${ANDROID_ARMEABI_V7A_LIB}
preparelib ${ANDROID_ARM64_V8A_LIB}
preparelib ${LINUX_ARMEABI_V7A_LIB}
preparelib ${LINUX_ARM64_V8A_LIB}
testlib ${X86_64_LIB}
testlib ${ANDROID_ARMEABI_V7A_LIB}
testlib ${ANDROID_ARM64_V8A_LIB}
testlib ${LINUX_ARMEABI_V7A_LIB}
testlib ${LINUX_ARM64_V8A_LIB}

cd wrappers/ros/src
catkin_init_workspace

cd ..
catkin_make

