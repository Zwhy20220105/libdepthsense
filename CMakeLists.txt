#  minimum required cmake version: 3.6.0
cmake_minimum_required(VERSION 3.6.0)
project(libdepthsense LANGUAGES CXX C)

include(CMake/lds_options.cmake)

include_directories(./include)
if(ANDROID_NDK_TOOLCHAIN_INCLUDED)
    set(ANDROID_STL "c++_static")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fPIC -pedantic -g -D_DEFAULT_SOURCE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pedantic -g -Wno-missing-field-initializers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch -Wno-multichar")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
    # Android
    if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
        link_directories(./lib/android-armeabi-v7a)
    elseif(${ANDROID_ABI} STREQUAL "arm64-v8a")
        link_directories(./lib/android-arm64-v8a)
    else()
        message(FATAL_ERROR "Can not find lib for ABI ${ANDROID_ABI}")
    endif()
elseif (LINUX_ARM64_V8A)
    link_directories(./lib/linux-arm64-v8a)
elseif (LINUX_ARMEABI_V7A)
    link_directories(./lib/linux-armeabi-v7a)
else()
    # Linux
    link_directories(./lib/x86_64)
endif()

set(CMAKE_INSTALL_BINDIR install/bin)

add_subdirectory(examples)

