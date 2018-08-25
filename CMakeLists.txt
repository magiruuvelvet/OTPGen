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

# Install Configuration
include(GNUInstallDirs)

# Application Mode Flag
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

# Use QR Code feature?
set(WITH_QR_CODES ON CACHE BOOLEAN "Enable support for QR codes")
if (WITH_QR_CODES)
    message(STATUS "Building with QR code support.")
    add_definitions(-DOTPGEN_WITH_QR_CODES)
endif()

# Find runtime dependencies
find_package(PkgConfig)

if (MODE STREQUAL "GUI")
    message(STATUS "Finding Qt...")
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Network REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Xml REQUIRED)

    # Qt deprecated warnings
    add_definitions(-DQT_DEPRECATED_WARNINGS)
    add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

    # Qt automoc
    message(STATUS "Enabling Qt automoc...")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    set(CMAKE_AUTOMOC ON)

    # Handle Qt qrc files automatically
    message(STATUS "Enabling Qt autorcc...")
    set(CMAKE_AUTORCC ON)
endif()

find_package(Gcrypt REQUIRED)
include_directories(${GCRYPT_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
include_directories("${ZLIB_INCLUDE_DIRS}")

pkg_check_modules(CRYPTOPP REQUIRED libcrypto++)
include_directories("${CRYPTOPP_INCLUDEDIR}")

#######################################################################################################################
# Generate source lists
#######################################################################################################################

# Core library
file(GLOB_RECURSE SourceListCore
    "${CMAKE_SOURCE_DIR}/Source/Core/*.cpp"    "${CMAKE_SOURCE_DIR}/Source/Core/*.hpp"
    "${CMAKE_SOURCE_DIR}/Source/Core/*.c"      "${CMAKE_SOURCE_DIR}/Source/Core/*.h"

    "${CMAKE_SOURCE_DIR}/Libs/cereal/*.hpp"
)

# Core library support (libcotp)
file(GLOB_RECURSE SourceListCoreSupport
    "${CMAKE_SOURCE_DIR}/Libs/libbaseencode/*.c"
    "${CMAKE_SOURCE_DIR}/Libs/libbaseencode/*.h"

    "${CMAKE_SOURCE_DIR}/Libs/libcotp/*.c"
    "${CMAKE_SOURCE_DIR}/Libs/libcotp/*.h"
)

# App launcher and config
file(GLOB_RECURSE SourceListApp
    "${CMAKE_SOURCE_DIR}/main.cpp"

    "${CMAKE_SOURCE_DIR}/Source/Config/*.cpp"    "${CMAKE_SOURCE_DIR}/Source/Config/*.hpp"
    "${CMAKE_SOURCE_DIR}/Source/Config/*.c"      "${CMAKE_SOURCE_DIR}/Source/Config/*.h"
)

# QR Code Support - depends on libzxing
if (WITH_QR_CODES)
    file(GLOB_RECURSE SourceListQRCodeSupport
        "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader/*.cpp"
        "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader/*.h"

        "${CMAKE_SOURCE_DIR}/Libs/QRCodeGenerator/*.cpp"
        "${CMAKE_SOURCE_DIR}/Libs/QRCodeGenerator/*.hpp"
    )
endif()

# Graphical User Interface - depends on Qt
file(GLOB_RECURSE SourceListGui
    "${CMAKE_SOURCE_DIR}/Source/Gui/*.cpp"    "${CMAKE_SOURCE_DIR}/Source/Gui/*.hpp"
    "${CMAKE_SOURCE_DIR}/Source/Gui/*.c"      "${CMAKE_SOURCE_DIR}/Source/Gui/*.h"
)

# GUI Support Library
file(GLOB_RECURSE SourceListGuiSupport
    "${CMAKE_SOURCE_DIR}/Libs/qtsingleapplication/*.cpp"
    "${CMAKE_SOURCE_DIR}/Libs/qtsingleapplication/*.h"

    "${CMAKE_SOURCE_DIR}/Libs/qtlockedfile/qtlockedfile.cpp"
    "${CMAKE_SOURCE_DIR}/Libs/qtlockedfile/qtlockedfile.h"
)

# Platform-specific sources for the GUI
if (WIN32)
    file(GLOB_RECURSE SourceListGuiPlatformSupport
        "${CMAKE_SOURCE_DIR}/Libs/qtlockedfile/qtlockedfile_win.cpp"
    )
else()
    file(GLOB_RECURSE SourceListGuiPlatformSupport
        "${CMAKE_SOURCE_DIR}/Libs/qtlockedfile/qtlockedfile_unix.cpp"
    )
endif()
set(SourceListGuiSupport ${SourceListGuiSupport} ${SourceListGuiPlatformSupport})

# Cli mode sources
file(GLOB_RECURSE SourceListCli
    "${CMAKE_SOURCE_DIR}/Source/Cli/*.cpp"    "${CMAKE_SOURCE_DIR}/Source/Cli/*.hpp"
    "${CMAKE_SOURCE_DIR}/Source/Cli/*.c"      "${CMAKE_SOURCE_DIR}/Source/Cli/*.h"
)

# Add source directory to include paths
include_directories("${CMAKE_SOURCE_DIR}/Source")
include_directories("${CMAKE_SOURCE_DIR}/Libs")
include_directories("${CMAKE_SOURCE_DIR}/Libs/libbaseencode")
include_directories("${CMAKE_SOURCE_DIR}/Libs/libcotp")
include_directories("${CMAKE_SOURCE_DIR}/Libs/cereal")
include_directories("${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader")
include_directories("${CMAKE_SOURCE_DIR}/Libs/qtsingleapplication")
include_directories("${CMAKE_SOURCE_DIR}/Libs/qtlockedfile")

if (MODE STREQUAL "GUI")
    # Configure QtKeychain
    message(STATUS "=== Configuring QtKeychain...")
    set(BUILD_TEST_APPLICATION OFF CACHE BOOLEAN "")
    set(BUILD_TRANSLATIONS OFF CACHE BOOLEAN "")
    set(QTKEYCHAIN_STATIC ON CACHE BOOLEAN "")
    set(LIBSECRET_SUPPORT ON CACHE BOOLEAN "")
    add_subdirectory("${CMAKE_SOURCE_DIR}/Libs/qtkeychain")
    include_directories("${CMAKE_CURRENT_BINARY_DIR}/Libs/qtkeychain")
    link_directories("${CMAKE_CURRENT_BINARY_DIR}/Libs/qtkeychain")
    message(STATUS "=== Configured QtKeychain.")
endif()

if (WITH_QR_CODES)
    message(STATUS "=== Configuring zxing-cpp...")
    add_subdirectory("${CMAKE_SOURCE_DIR}/Libs/zxing-cpp")
    include_directories("${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/core/src")
    link_directories("${CMAKE_CURRENT_BINARY_DIR}/Libs/zxing-cpp")
    message(STATUS "=== Configured zxing-cpp.")
endif()

#######################################################################################################################
# Targets
#######################################################################################################################

# Core Library Support
add_library(CoreLibSupport ${SourceListCoreSupport})

# Core library
add_library(CoreLib ${SourceListCore})
target_link_libraries(CoreLib CoreLibSupport
    ${GCRYPT_LIBRARIES}
    ${CRYPTOPP_LDFLAGS}
    ${ZLIB_LIBRARIES}
)
if (WITH_QR_CODES)
    target_link_libraries(CoreLib
        QRCodeSupportLib
        libzxing
    )
endif()

# QR Code Support
if (WITH_QR_CODES)
    add_library(QRCodeSupportLib ${SourceListQRCodeSupport})
endif()

if (MODE STREQUAL "GUI")
    add_library(GuiLibSupport STATIC ${SourceListGuiSupport})
    target_link_libraries(GuiLibSupport
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
    )

    qt5_add_resources(RCC_SOURCES "${CMAKE_SOURCE_DIR}/EmbeddedAssets.qrc")
    add_library(GuiLib STATIC ${SourceListGui} ${RCC_SOURCES})
    target_link_libraries(GuiLib
        CoreLib
        GuiLibSupport
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Xml
    )

    if (WITH_QR_CODES)
        target_link_libraries(GuiLib
            QRCodeSupportLib
            libzxing
        )
    endif()
endif()

if (MODE STREQUAL "GUI")
    add_executable(${PROJECT_NAME} ${SourceListApp})
    target_link_libraries(${PROJECT_NAME}
        CoreLib
        GuiLib
        ${RCC_SOURCES}
        qt5keychain
    )
elseif(MODE STREQUAL "CLI")
    add_executable("${PROJECT_NAME}Cli" ${SourceListApp} ${SourceListCli})
    target_link_libraries("${PROJECT_NAME}Cli"
        CoreLib
    )
endif()

# Link additional external dependencies
if (MODE STREQUAL "GUI")
    target_link_libraries(${PROJECT_NAME}
        ${GCRYPT_LIBRARIES}
        ${CRYPTOPP_LDFLAGS}
        ${ZLIB_LIBRARIES}
    )
else()
    target_link_libraries("${PROJECT_NAME}Cli"
        ${GCRYPT_LIBRARIES}
        ${CRYPTOPP_LDFLAGS}
        ${ZLIB_LIBRARIES}
    )
endif()

# Link to bundled ZXing library when QR Code support was enabled
if (WITH_QR_CODES)
    if (MODE STREQUAL "GUI")
        target_link_libraries(${PROJECT_NAME} QRCodeSupportLib libzxing)
    else()
        target_link_libraries("${PROJECT_NAME}Cli" QRCodeSupportLib libzxing)
    endif()
endif()


#######################################################################################################################
# Install rules
#######################################################################################################################

if (MODE STREQUAL "GUI")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OTPGen
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
            RENAME otpgen)

    install(DIRECTORY ${CMAKE_SOURCE_DIR}/Desktop/applications
                      ${CMAKE_SOURCE_DIR}/Desktop/icons
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share)
elseif(MODE STREQUAL "CLI")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OTPGenCli
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
            RENAME otpgen-cli)
endif()
