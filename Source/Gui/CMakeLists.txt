###############################################################################
## GUI
###############################################################################

include(SetCppStandard)

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

# Graphical User Interface - depends on Qt
file(GLOB_RECURSE SourceListGui
    "*.cpp"
    "*.hpp"
)

set(TARGET_NAME "${PROJECT_NAME}")

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

add_library("GuiLibSupport" STATIC ${SourceListGuiSupport})
SetCppStandard("GuiLibSupport" 17)
target_link_libraries("GuiLibSupport"
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
)

# Configure QtKeychain
message(STATUS "=== Configuring QtKeychain...")
set(BUILD_TEST_APPLICATION OFF CACHE BOOLEAN "")
set(BUILD_TRANSLATIONS OFF CACHE BOOLEAN "")
set(QTKEYCHAIN_STATIC ON CACHE BOOLEAN "")
set(LIBSECRET_SUPPORT ON CACHE BOOLEAN "")
add_subdirectory("${CMAKE_SOURCE_DIR}/Libs/qtkeychain/qtkeychain" "${CMAKE_CURRENT_BINARY_DIR}/qt5keychain")
message(STATUS "=== Configured QtKeychain.")

qt5_add_resources(RCC_SOURCES "${CMAKE_SOURCE_DIR}/GuiAssets/EmbeddedAssets.qrc")
add_custom_target(GenerateEmbeddedAssets DEPENDS ${RCC_SOURCES})

add_executable("${TARGET_NAME}" ${SourceListGui} ${RCC_SOURCES})
SetCppStandard("${TARGET_NAME}" 17)
target_link_libraries("${TARGET_NAME}"
    "CoreLib" "GuiLibSupport" "SharedLib"
    qt5keychain
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
)
add_dependencies("${TARGET_NAME}" GenerateEmbeddedAssets)

target_include_directories("GuiLibSupport" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/qtsingleapplication")
target_include_directories("GuiLibSupport" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/qtlockedfile")

target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/qtkeychain")
target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Source/Gui/qt5keychain")
target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/qtsingleapplication")
