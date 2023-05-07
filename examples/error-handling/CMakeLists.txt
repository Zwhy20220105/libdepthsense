#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(ExamplesErrorHandling)

find_package(OpenCV REQUIRED)

add_executable(ds-error-handling ds-error-handling.cpp)

set_property(TARGET ds-error-handling PROPERTY CXX_STANDARD 11)

target_link_libraries(ds-error-handling ${DEPENDENCIES}  ${OpenCV_LIBS})

set_target_properties (ds-error-handling PROPERTIES FOLDER "Examples")

install(TARGETS ds-error-handling RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
