#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(ExamplesOpencvTest)

find_package(OpenCV REQUIRED)

add_executable(ds-opencv-example ds-opencv-example.cpp)

set_property(TARGET ds-opencv-example PROPERTY CXX_STANDARD 11)

target_include_directories(ds-opencv-example PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(ds-opencv-example ${DEPENDENCIES} ${OpenCV_LIBS})

set_target_properties (ds-opencv-example PROPERTIES FOLDER "Examples")

install(TARGETS ds-opencv-example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
