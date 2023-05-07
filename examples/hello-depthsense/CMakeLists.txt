#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(ExamplesHelloDepthSense)

find_package(OpenCV REQUIRED)

add_executable(ds-hello-depthsense ds-hello-depthsense.cpp)

set_property(TARGET ds-hello-depthsense PROPERTY CXX_STANDARD 11)

target_link_libraries(ds-hello-depthsense ${DEPENDENCIES} ${OpenCV_LIBS})

set_target_properties (ds-hello-depthsense PROPERTIES FOLDER "Examples")

install(TARGETS ds-hello-depthsense RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
