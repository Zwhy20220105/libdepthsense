#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(ExamplesMultiCam)

add_executable(ds-multicam ds-multicam.cpp)

set_property(TARGET ds-multicam PROPERTY CXX_STANDARD 11)

target_link_libraries(ds-multicam ${DEPENDENCIES})

set_target_properties (ds-multicam PROPERTIES FOLDER "Examples")

install(TARGETS ds-multicam RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
