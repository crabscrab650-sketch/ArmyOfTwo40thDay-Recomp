# armyoftwo40threcomp - Army of Two: The 40th Day PC Recompilation
# Based on ReXGlue SDK v0.10.0+

cmake_minimum_required(VERSION 3.25)
project(armyoftwo40threcomp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(REXSDK_VERSION "0.10.0.8-dev.g1406e1b")

if(WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|x86_64)$")
    set(REXSDK_PLATFORM "win-amd64")
elseif(WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "^ARM64$")
    message(FATAL_ERROR "win-arm64 ReXGlue SDK release is not published")
elseif(APPLE AND CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
    set(REXSDK_PLATFORM "mac-arm64")
elseif(APPLE)
    set(REXSDK_PLATFORM "mac-amd64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(REXSDK_PLATFORM "linux-arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
    set(REXSDK_PLATFORM "linux-amd64")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND}
            "-DREXSDK_VERSION='${REXSDK_VERSION}'"
            "-DREXSDK_PLATFORM=${REXSDK_PLATFORM}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch-rexglue-sdk.cmake")

set(REXSDK_VERSION "")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/generated/rexglue.cmake")
    include(generated/rexglue.cmake)
endif()

set(ARMYOFTWO40THRECOMP_SOURCES
    src/main.cpp
)

if(WIN32)
    add_executable(armyoftwo40threcomp WIN32 ${ARMYOFTWO40THRECOMP_SOURCES})
else()
    add_executable(armyoftwo40threcomp ${ARMYOFTWO40THRECOMP_SOURCES})
endif()

if(COMMAND rexglue_setup_target)
    rexglue_setup_target(armyoftwo40threcomp GPU_PLUGINS xenos)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/settings/gamecontrollerdb.txt")
    add_custom_command(TARGET armyoftwo40threcomp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/settings/gamecontrollerdb.txt"
                "$<TARGET_FILE_DIR:armyoftwo40threcomp>/gamecontrollerdb.txt"
        COMMENT "Copying gamecontrollerdb.txt next to armyoftwo40threcomp.exe")
endif()
