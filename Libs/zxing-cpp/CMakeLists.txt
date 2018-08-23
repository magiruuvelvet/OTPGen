cmake_minimum_required(VERSION 3.0)

project(zxing)

# Adjust CMake's module path.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/FindModules)

# Suppress MSVC CRT warnings.
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(/Za)
    add_definitions(/MP) # multi-core builds
endif()

include(source_files.cmake)

if(WIN32)
    include_directories(core/lib/win32)
    set(CMAKE_DEBUG_POSTFIX -debug)
endif()

include_directories(core/src)
add_library(libzxing ${LIBZXING_FILES})
set_target_properties(libzxing PROPERTIES PREFIX "")

find_package(Iconv)
if(ICONV_FOUND)
    include_directories(${ICONV_INCLUDE_DIR})
    target_link_libraries(libzxing ${ICONV_LIBRARIES})
else()
    add_definitions(-DNO_ICONV=1)
endif()

configure_file(cmake/zxing-config.cmake.in zxing-config.cmake @ONLY)
