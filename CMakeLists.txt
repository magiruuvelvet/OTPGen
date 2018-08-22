cmake_minimum_required(VERSION 3.8)

project(OTPGen)

# Project configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release)
# Use project folders in IDE's
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Folder where generated files go
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/Generated)
include_directories(${GENERATED_DIR})

# Handle configuration types
option(CMAKE_BUILD_TYPE "Set the CMake configuration type to use" "Debug")
message(STATUS "Using configuration type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

# Enable (strict) C++17 features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Release mode flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -rpath -fPIC")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -rpath -fPIC")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unused-command-line-argument")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wno-unused-command-line-argument")

# Debug mode flags
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Building with development commands.")
    add_definitions(-DOTPGEN_DEBUG)
endif()

# Generic flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas")

# Add module paths
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}"
    "${CMAKE_SOURCE_DIR}/CMake/Modules")     # Meta and Helper Modules

# Qt deprecated warnings
add_definitions(-DQT_DEPRECATED_WARNINGS)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

# Find runtime dependencies
find_package(PkgConfig)

message(STATUS "Finding Qt...")
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(Gcrypt REQUIRED)
include_directories(${GCRYPT_INCLUDE_DIR})

pkg_check_modules(CRYPTOPP REQUIRED libcrypto++)
include_directories("${CRYPTOPP_INCLUDEDIR}")

# Qt automoc
message(STATUS "Enabling Qt automoc...")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Handle Qt qrc files automatically
message(STATUS "Enabling Qt autorcc...")
set(CMAKE_AUTORCC ON)

# Generate source list
file(GLOB_RECURSE SourceList
    "${CMAKE_SOURCE_DIR}/main.cpp"

    "${CMAKE_SOURCE_DIR}/Source/Core/*.cpp"    "${CMAKE_SOURCE_DIR}/Source/Core/*.hpp"
    "${CMAKE_SOURCE_DIR}/Source/Core/*.c"      "${CMAKE_SOURCE_DIR}/Source/Core/*.h"

    "${CMAKE_SOURCE_DIR}/Source/Config/*.cpp"    "${CMAKE_SOURCE_DIR}/Source/Config/*.hpp"
    "${CMAKE_SOURCE_DIR}/Source/Config/*.c"      "${CMAKE_SOURCE_DIR}/Source/Config/*.h"

    # bundled dependencies

    "${CMAKE_SOURCE_DIR}/Libs/libbaseencode/*.c"
    "${CMAKE_SOURCE_DIR}/Libs/libbaseencode/*.h"

    "${CMAKE_SOURCE_DIR}/Libs/libcotp/*.c"
    "${CMAKE_SOURCE_DIR}/Libs/libcotp/*.h"

    "${CMAKE_SOURCE_DIR}/Libs/cereal/*.hpp"
)

file(GLOB_RECURSE GuiSourceList
    "${CMAKE_SOURCE_DIR}/EmbeddedAssets.qrc"

    "${CMAKE_SOURCE_DIR}/Source/Gui/*.cpp"    "${CMAKE_SOURCE_DIR}/Source/Gui/*.hpp"
    "${CMAKE_SOURCE_DIR}/Source/Gui/*.c"      "${CMAKE_SOURCE_DIR}/Source/Gui/*.h"
)

file(GLOB_RECURSE CliSourceList
    "${CMAKE_SOURCE_DIR}/Source/Cli/*.cpp"    "${CMAKE_SOURCE_DIR}/Source/Cli/*.hpp"
    "${CMAKE_SOURCE_DIR}/Source/Cli/*.c"      "${CMAKE_SOURCE_DIR}/Source/Cli/*.h"
)

# Add source directory to include paths
include_directories("${CMAKE_SOURCE_DIR}/Source")
include_directories("${CMAKE_SOURCE_DIR}/Libs")
include_directories("${CMAKE_SOURCE_DIR}/Libs/libbaseencode")
include_directories("${CMAKE_SOURCE_DIR}/Libs/libcotp")
include_directories("${CMAKE_SOURCE_DIR}/Libs/cereal")

set(MODE "GUI" CACHE STRING "Application mode (GUI, CLI)")
if (MODE STREQUAL "GUI")
    message(STATUS "Building GUI...")
    add_definitions(-DOTPGEN_GUI)
elseif (MODE STREQUAL "CLI")
    message(STATUS "Building CLI...")
    add_definitions(-DOTPGEN_CLI)
else()
    message(FATAL_ERROR "Unknown application mode!")
endif()

if (MODE STREQUAL "GUI")
    # Configure QtKeychain
    message(STATUS "Configuring QtKeychain...")
    set(BUILD_TEST_APPLICATION OFF CACHE BOOLEAN "")
    set(BUILD_TRANSLATIONS OFF CACHE BOOLEAN "")
    set(QTKEYCHAIN_STATIC ON CACHE BOOLEAN "")
    add_subdirectory("${CMAKE_SOURCE_DIR}/Libs/qtkeychain")
    include_directories("${CMAKE_CURRENT_BINARY_DIR}/Libs/qtkeychain")
    link_directories("${CMAKE_CURRENT_BINARY_DIR}/Libs/qtkeychain")
    message(STATUS "Configured QtKeychain.")
endif()

# Target
if (MODE STREQUAL "GUI")
    add_executable(${PROJECT_NAME} ${SourceList} ${GuiSourceList})
    target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        ${GCRYPT_LIBRARIES}
        ${CRYPTOPP_LDFLAGS}
        qt5keychain
    )
elseif(MODE STREQUAL "CLI")
    add_executable("${PROJECT_NAME}Cli" ${SourceList} ${CliSourceList})
    target_link_libraries("${PROJECT_NAME}Cli"
        ${GCRYPT_LIBRARIES}
        ${CRYPTOPP_LDFLAGS}
    )
endif()

# Install rules

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OTPGen
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        RENAME otpgen)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/Desktop/applications
                  ${CMAKE_SOURCE_DIR}/Desktop/icons
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share)
