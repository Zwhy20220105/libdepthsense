#!/usr/bin/env bash
set -e

SCRIPT_PATH="`dirname \"$0\"`"

# tool chain params to build for android
_ANDDROID_CMAKE=cmake
_ANDROID_NDK=$ANDROID_NDK

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -h|--help)
    usage
    exit 0
    ;;
    -n|--ndk_path)
    _ANDROID_NDK="$2"
    shift
    shift
    ;;
    *)
    POSITIONAL+=("$1")
    shift
    ;;
esac
done

RED='\033[0;31m'
GREEN='\033[0;32m'
BROWN='\033[0;33m'
DARK_GRAY='\033[0;90m'
NO_COLOR='\033[0m'

##### linux host
echo "Build for linux host"
mkdir -p build-x86_64
pushd build-x86_64
cmake .. && make -j8
popd

# build and test pass on ndk 21.2.6472646
if [ -d "${_ANDROID_NDK}" ]; then
    BUILD_ARCH="armeabi-v7a"
    echo "Bulil for ${BUILD_ARCH}"
    ##### android armeabi-armv7
    mkdir -p build-android-${BUILD_ARCH}
    pushd build-android-${BUILD_ARCH}
    $_ANDDROID_CMAKE -DCMAKE_TOOLCHAIN_FILE=$_ANDROID_NDK/build/cmake/android.toolchain.cmake -DUSE_ANDROID_BACKEND=0 -DANDROID_ABI="${BUILD_ARCH}" -DANDROID_ARM_NEON=ON -DANDROID_PLATFORM=android-21 ..
    make -j8
    popd

    BUILD_ARCH="arm64-v8a"
    echo "Bulil for ${BUILD_ARCH}"
    ##### android armeabi-armv8a
    mkdir -p build-android-${BUILD_ARCH}
    pushd build-android-${BUILD_ARCH}
    $_ANDDROID_CMAKE -DCMAKE_TOOLCHAIN_FILE=$_ANDROID_NDK/build/cmake/android.toolchain.cmake -DUSE_ANDROID_BACKEND=0 -DANDROID_ABI="${BUILD_ARCH}" -DANDROID_ARM_NEON=ON -DANDROID_PLATFORM=android-21 ..
    make -j8
    popd
else
    echo -e "${BROWN}No android ndk found, not build for android${NO_COLOR}"
fi

if type aarch64-linux-gnu-gcc > /dev/null; then
    BUILD_ARCH="arm64-v8a"
    echo "Bulil for linux ${BUILD_ARCH}"
    ##### linux armeabi-armv8a
    mkdir -p build-linux-${BUILD_ARCH}
    mkdir -p build-linux-${BUILD_ARCH}/Cmake
    cp ./CMake/aarch64-linux-gnu-c.toolchain.cmake build-linux-${BUILD_ARCH}/Cmake
    pushd build-linux-${BUILD_ARCH}
    cmake .. -DCMAKE_TOOLCHAIN_FILE=./CMake/aarch64-linux-gnu-c.toolchain.cmake
    make -j8
    popd
else
    echo -e "${BROWN}No (aarch64-linux-gnu-gcc) was found, skip building linux arm64-v8a ${NO_COLOR}"
fi

if type arm-linux-gnueabihf-gcc > /dev/null; then
    BUILD_ARCH="armeabi-v7a"
    echo "Bulil for linux ${BUILD_ARCH}"
    ##### linux armeabi-v7a
    mkdir -p build-linux-${BUILD_ARCH}
    mkdir -p build-linux-${BUILD_ARCH}/Cmake
    cp ./CMake/arm-linux-gnueabihf-c.toolchain.cmake build-linux-${BUILD_ARCH}/Cmake
    pushd build-linux-${BUILD_ARCH}
    cmake .. -DCMAKE_TOOLCHAIN_FILE=./CMake/arm-linux-gnueabihf-c.toolchain.cmake
    make -j8
    popd
else
    echo -e "${BROWN}No (arm-linux-gnueabihf-gcc) was found, skip building linux armeabi-v7a ${NO_COLOR}"
fi