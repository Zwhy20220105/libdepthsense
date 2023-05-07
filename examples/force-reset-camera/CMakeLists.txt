#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(ExamplesForceResetCamera)

add_executable(ds-force-reset-camera ds-force-reset-camera.cpp)

set_property(TARGET ds-force-reset-camera PROPERTY CXX_STANDARD 11)

target_link_libraries(ds-force-reset-camera ${DEPENDENCIES})

set_target_properties (ds-force-reset-camera PROPERTIES FOLDER "Examples")

install(TARGETS ds-force-reset-camera RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
