cmake_minimum_required(VERSION 3.8)

project(OTPGen)

# Project configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release)
# Use project folders in IDE's
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Meta and Helper Modules
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}"
    "${PROJECT_SOURCE_DIR}/CMake/Modules")

# Detect operating system
include(DetectOS)

# set C++ standard macro
include(SetCppStandard)

# Folder where generated files go
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/Generated)
include_directories(${GENERATED_DIR})

# Handle configuration types
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the CMake configuration type to use")
message(STATUS "Using configuration type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

# Position Independent Code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Shared Libraries Configuration
set(CMAKE_INSTALL_RPATH "${ORIGIN}")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

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

# Android build
set(BUILD_ANDROID OFF CACHE BOOLEAN "Optimize build for Android")
if (BUILD_ANDROID)
    message(STATUS "NDK root: $ENV{ANDROID_NDK_ROOT}")
endif()

# Generic flags
if (NOT BUILD_ANDROID)
    # Android toolchain doesn't support the native arch flag
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-switch")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas -Wno-switch")

# Install Configuration
include(GNUInstallDirs)

# Unit testing
set(UNIT_TESTING OFF CACHE BOOLEAN "Build unit tests")
if (UNIT_TESTING)
    message(STATUS "Building with unit tests.")
endif()

# Build with GUI support?
set(DISABLE_GUI OFF CACHE BOOLEAN "Build without GUI support")
if (DISABLE_GUI)
    message(STATUS "Building without GUI...")
endif()
if (BUILD_ANDROID)
    set(DISABLE_GUI ON CACHE BOOLEAN "" FORCE)
endif()

# Build the CLI?
set(DISABLE_CLI OFF CACHE BOOLEAN "Build without the CLI")
if (DISABLE_CLI)
    message(STATUS "Building without CLI...")
endif()
if (BUILD_ANDROID)
    set(DISABLE_CLI ON CACHE BOOLEAN "" FORCE)
endif()

# Build the migration tool?
set(BUILD_MIGRATION_TOOL OFF CACHE BOOLEAN "Build the migration tool to upgrade your existing database to the new SQLite-based format")
if (BUILD_MIGRATION_TOOL)
    message(STATUS "Building the migration tool...")
endif()

# Use QR Code feature?
set(WITH_QR_CODES ON CACHE BOOLEAN "Enable support for QR codes")
if (WITH_QR_CODES)
    message(STATUS "Building with QR code support.")
    add_definitions(-DOTPGEN_WITH_QR_CODES)
endif()

#######################################################################################################################
# Targets
#######################################################################################################################

# Determine if this project is the parent or not.
# Used to export some variables to the parent scope if any.
get_directory_property(hasParent PARENT_DIRECTORY)

# Set target destination for built targets
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib-static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Core library
message(STATUS "==> Configuring target \"Core\"...")
add_subdirectory("${PROJECT_SOURCE_DIR}/Source/Core")
include_directories("${CORELIB_INCLUDE_DIR}")
if (hasParent)
    set(LIBOTPGEN_INCLUDE_DIR "${CORELIB_INCLUDE_DIR}" PARENT_SCOPE)
endif()

# Migration Tool
if (BUILD_MIGRATION_TOOL)
    add_subdirectory("${PROJECT_SOURCE_DIR}/Source/MigrationTool")
endif()

# QR Code Support library
if (WITH_QR_CODES)
    message(STATUS "==> Configuring target \"QRCodeSupportLib\"...")
    add_subdirectory("${PROJECT_SOURCE_DIR}/Source/QRCodeSupport" "${CMAKE_CURRENT_BINARY_DIR}/qrcodesupport")
    include_directories("${QRCODESUPPORTLIB_INCLUDE_DIR}")
    if (hasParent)
        set(LIBOTPGEN_QRCODESUPPORT_INCLUDE_DIR "${QRCODESUPPORTLIB_INCLUDE_DIR}" PARENT_SCOPE)
    endif()
endif()

# Shared library
message(STATUS "==> Configuring target \"Shared\"...")
add_subdirectory("${PROJECT_SOURCE_DIR}/Source/Shared")
include_directories("${SHAREDLIB_INCLUDE_DIR}")

# CLI
if (NOT DISABLE_CLI)
    message(STATUS "==> Configuring target \"CLI\"...")
    add_subdirectory("${PROJECT_SOURCE_DIR}/Source/Cli")
endif()

# GUI
if (NOT DISABLE_GUI)
    message(STATUS "==> Configuring target \"GUI\"...")
    add_subdirectory("${PROJECT_SOURCE_DIR}/Source/Gui")
endif()

# Unit test sources
if (UNIT_TESTING)
    add_subdirectory("${PROJECT_SOURCE_DIR}/Tests")
endif()

#######################################################################################################################
# Install rules
#######################################################################################################################

# install GUI and desktop files
if (NOT DISABLE_GUI)
    install(FILES ${CMAKE_BINARY_DIR}/bin/otpgen
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/Desktop/applications
                      ${PROJECT_SOURCE_DIR}/Desktop/icons
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share)
endif()

# install libraries
install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PATTERN *
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# header files for libraries
install(DIRECTORY ${CORELIB_INCLUDE_DIR}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/otpgen
        PATTERN *.hpp
        PATTERN *.cpp EXCLUDE
        PATTERN *.txt EXCLUDE
        PATTERN Internal EXCLUDE
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

if (WITH_QR_CODES)
    install(DIRECTORY ${QRCODESUPPORTLIB_INCLUDE_DIR}/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/otpgen
            PATTERN *.hpp
            PATTERN *.cpp EXCLUDE
            PATTERN *.txt EXCLUDE
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
endif()

# install CLI
install(FILES ${CMAKE_BINARY_DIR}/bin/otpgen-cli
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# install migration tool
if (BUILD_MIGRATION_TOOL)
    install(FILES ${CMAKE_BINARY_DIR}/bin/otpgen-migratedb
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()
