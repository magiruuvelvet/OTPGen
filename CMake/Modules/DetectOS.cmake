#
# Module: DetectOS
#
# Description:
# Checks on which operating system we are building.
# Sets up CMake variables `OS_...` accordingly.
#
# Author:
# マギルゥーベルベット (magiruuvelvet)
#


# CMake does not distinguish Linux nor *BSD from other Unices.
string(REGEX MATCH "Linux" OS_LINUX ${CMAKE_SYSTEM_NAME})
string(REGEX MATCH "BSD" OS_BSD ${CMAKE_SYSTEM_NAME})
string(REGEX MATCH "Darwin" OS_MACOS ${CMAKE_SYSTEM_NAME})
string(REGEX MATCH "Emscripten" OS_WASM ${CMAKE_SYSTEM_NAME})

# Check if it's Windows
if (WIN32)
    set(OS_WINDOWS TRUE BOOL INTERNAL)
endif()

#
# Set MACROS and CMake variables and log results
#

# Linux
if (OS_LINUX)
    add_definitions(-DOS_LINUX)
    set(OS_LINUX TRUE BOOL INTERNAL)

# *BSD
elseif (OS_BSD)
    add_definitions(-DOS_BSD)
    set(OS_BSD TRUE BOOL INTERNAL)

# macOS
elseif (OS_MACOS)
    add_definitions(-DOS_MACOS)
    set(OS_MACOS TRUE BOOL INTERNAL)

# WebAssembly / emscripten
elseif (OS_WASM)
    add_definitions(-DOS_WASM)
    set(OS_WASM TRUE BOOL INTERNAL)

# OS not recognized
else()
    message(WARNING "Operating system not recognized!")
endif()

# Set system name as CMake string
set(OS_SYSTEMNAME "${CMAKE_SYSTEM_NAME}")

# Add system name as macro
add_definitions(-DOS_SYSTEMNAME="${OS_SYSTEMNAME}")

message(STATUS "Host operating system: ${OS_SYSTEMNAME}")
