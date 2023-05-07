# 指定所需的最低CMake版本为3.1.0
cmake_minimum_required(VERSION 3.1.0)

# 设置项目名称为"DepthsenseExamples"
project(DepthsenseExamples)

# 保存编译命令到构建输出中
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# 如果在Android NDK工具链中，则使用depthsense和log库
# 否则，使用depthsense和pthread库
if(ANDROID_NDK_TOOLCHAIN_INCLUDED)
    set(DEPENDENCIES depthsense log)
else()
    set(DEPENDENCIES depthsense pthread)
endif()

# 添加项目子目录
add_subdirectory(hello-depthsense)
add_subdirectory(multicam)
add_subdirectory(error-handling)
add_subdirectory(force-reset-camera)
add_subdirectory(opencv-example)

# 如果在Android或ARM Linux平台上，则不构建OpenCV和PCL示例
if(ANDROID_NDK_TOOLCHAIN_INCLUDED OR LINUX_ARM64_V8A OR LINUX_ARMEABI_V7A)
    option(BUILD_CV_EXAMPLES "Build OpenCV examples" OFF)
    option(BUILD_PCL_EXAMPLES "Build PCL examples" OFF)
else()
    # 如果选择构建OpenCV示例，则添加子目录
    if (BUILD_CV_EXAMPLES)
        add_subdirectory(opencv-example)
        add_subdirectory(stereo-capture)
        add_subdirectory(vte-example)
    endif()
    # 如果选择构建PCL示例，则添加子目录
    if (BUILD_PCL_EXAMPLES)
        add_subdirectory(pcl-pointcloud)
    endif()
endif()
