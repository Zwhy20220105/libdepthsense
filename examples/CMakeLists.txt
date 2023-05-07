#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(DepthsenseExamples)

# Save the command line compile commands in the build output
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if(ANDROID_NDK_TOOLCHAIN_INCLUDED)
    set(DEPENDENCIES depthsense log)
else()
    #set(DEPENDENCIES depthsense pthread udev)
    set(DEPENDENCIES depthsense pthread)
endif()

add_subdirectory(hello-depthsense)
add_subdirectory(multicam)
add_subdirectory(error-handling)
add_subdirectory(force-reset-camera)

if(ANDROID_NDK_TOOLCHAIN_INCLUDED OR LINUX_ARM64_V8A OR LINUX_ARMEABI_V7A)
    option(BUILD_CV_EXAMPLES "Build OpenCV examples" OFF)
    option(BUILD_PCL_EXAMPLES "Build PCL examples" OFF)
else()
    if (BUILD_CV_EXAMPLES) 
        add_subdirectory(opencv-example)
        add_subdirectory(stereo-capture)
        add_subdirectory(vte-example)
    endif()
    if (BUILD_PCL_EXAMPLES)
        add_subdirectory(pcl-pointcloud)
    endif()
endif()
