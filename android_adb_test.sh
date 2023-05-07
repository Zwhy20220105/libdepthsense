#!/usr/bin/env bash
SCRIPT_PATH="`dirname \"$0\"`"

adb push ${SCRIPT_PATH}/build-android-arm64-v8a/examples/hello-depthsense/ds-hello-depthsense /data/local
adb push ${SCRIPT_PATH}/build-android-arm64-v8a/examples/multicam/ds-multicam /data/local
adb push ${SCRIPT_PATH}/build-android-arm64-v8a/examples/error-handling/ds-error-handling /data/local

if test -f "${SCRIPT_PATH}/lib/arm64-v8a/libdepthsense.so"; then
    adb push ${SCRIPT_PATH}/lib/arm64-v8a/libdepthsense.so /data/local
fi

TEST_SCRIPT=/tmp/andriod_uvc_test.sh
echo "env LD_LIBRARY_PATH=. ./ds-hello-depthsense" > ${TEST_SCRIPT}
chmod +x ${TEST_SCRIPT}
adb push ${TEST_SCRIPT} /data/local

TEST_SCRIPT=/tmp/andriod_uvc_multicam_test.sh
echo "env LD_LIBRARY_PATH=. ./ds-multicam" > ${TEST_SCRIPT}
chmod +x ${TEST_SCRIPT}
adb push ${TEST_SCRIPT} /data/local

TEST_SCRIPT=/tmp/andriod_error-handling_test.sh
echo "env LD_LIBRARY_PATH=. ./ds-error-handling" > ${TEST_SCRIPT}
chmod +x ${TEST_SCRIPT}
adb push ${TEST_SCRIPT} /data/local



