cmake_minimum_required(VERSION 3.13)

project(fStretch)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
)

set(CMAKE_INSTALL_MESSAGE "NEVER")
set(CMAKE_OSX_ARCHITECTURES "x86_64")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(IS_WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(IS_LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(IS_MACOSX TRUE)
endif()

find_package(Maya REQUIRED)

add_subdirectory(src)

configure_file(fStretch.mod.template ${PROJECT_BINARY_DIR}/fStretch.mod)
install(FILES ${PROJECT_BINARY_DIR}/fStretch.mod
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)

if(IS_WINDOWS)
# Simple .bat file that is useful when working with the build.
# Will set the location of the plugin's .mod file and launch the Maya
# that was used for the build (if it was found).
# This is just one way of setting things up and is not a requirement.
    configure_file(run_maya.bat.template ${PROJECT_BINARY_DIR}/run_maya.bat)
endif()
