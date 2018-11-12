###############################################################################
## GUI
###############################################################################

include(SetCppStandard)

# GUI Support Library
file(GLOB_RECURSE SourceListGuiSupport
    "${PROJECT_SOURCE_DIR}/Libs/qtsingleapplication/*.cpp"
    "${PROJECT_SOURCE_DIR}/Libs/qtsingleapplication/*.h"

    "${PROJECT_SOURCE_DIR}/Libs/qtlockedfile/qtlockedfile.cpp"
    "${PROJECT_SOURCE_DIR}/Libs/qtlockedfile/qtlockedfile.h"
)

# Platform-specific sources for the GUI
if (WIN32)
    file(GLOB_RECURSE SourceListGuiPlatformSupport
        "${PROJECT_SOURCE_DIR}/Libs/qtlockedfile/qtlockedfile_win.cpp"
    )
else()
    file(GLOB_RECURSE SourceListGuiPlatformSupport
        "${PROJECT_SOURCE_DIR}/Libs/qtlockedfile/qtlockedfile_unix.cpp"
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
set(DISABLE_QTKEYCHAIN OFF CACHE BOOLEAN "Build without Qt Keychain support")
if (NOT DISABLE_QTKEYCHAIN)
    message(STATUS "Building with Qt Keychain support.")
    message(STATUS "   -> Configuring QtKeychain...")
    add_definitions(-DQTKEYCHAIN_SUPPORT)
    set(BUILD_TEST_APPLICATION OFF CACHE BOOLEAN "" FORCE)
    set(BUILD_TRANSLATIONS OFF CACHE BOOLEAN "" FORCE)
    set(QTKEYCHAIN_STATIC OFF CACHE BOOLEAN "" FORCE)
    set(LIBSECRET_SUPPORT ON CACHE BOOLEAN "" FORCE)
    add_subdirectory("${PROJECT_SOURCE_DIR}/Libs/qtkeychain/qtkeychain" "${CMAKE_CURRENT_BINARY_DIR}/qt5keychain" EXCLUDE_FROM_ALL)
    set_target_properties(qt5keychain PROPERTIES PREFIX "")
    set_target_properties(qt5keychain PROPERTIES OUTPUT_NAME "libotpgen-keychainsupport")
    message(STATUS "   -> Configured QtKeychain.")
else()
    message(STATUS "Building without Qt Keychain support.")
endif()

# zlib
set(BUNDLED_ZLIB OFF CACHE BOOLEAN "Use the bundled zlib.")
set(BUNDLED_ZLIB_ASM686 OFF CACHE BOOLEAN "Use optimized x86-32 asm.")
set(BUNDLED_ZLIB_AMD64 OFF CACHE BOOLEAN "Use optimized x86-64 asm.")
if (BUNDLED_ZLIB)
    message(STATUS "Building with bundled zlib")
    message(STATUS "   -> Configuring bundled zlib...")
    if (BUNDLED_ZLIB_ASM686)
        set(ASM686 ON CACHE BOOLEAN "" FORCE)
    endif()
    if (BUNDLED_ZLIB_AMD64)
        set(AMD64 ON CACHE BOOLEAN "" FORCE)
    endif()
    set(BUILD_SHARED_LIBS OFF CACHE BOOLEAN "" FORCE)
    set(SKIP_INSTALL_ALL ON CACHE BOOLEAN "" FORCE)
    add_subdirectory("${PROJECT_SOURCE_DIR}/Libs/zlib" "${CMAKE_CURRENT_BINARY_DIR}/zlib" EXCLUDE_FROM_ALL)
    message(STATUS "   -> Configured bundled zlib.")
else()
    message(STATUS "Using shared zlib.")
    find_package(ZLIB REQUIRED)
endif()

qt5_add_resources(RCC_SOURCES "${PROJECT_SOURCE_DIR}/GuiAssets/EmbeddedAssets.qrc")
add_custom_target(GenerateEmbeddedAssets DEPENDS ${RCC_SOURCES})

add_executable("${TARGET_NAME}" ${SourceListGui} ${RCC_SOURCES})
SetCppStandard("${TARGET_NAME}" 17)
target_link_libraries("${TARGET_NAME}"
    "CoreLib" "GuiLibSupport" "SharedLib"
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Xml
)
set_target_properties("${TARGET_NAME}" PROPERTIES PREFIX "")
set_target_properties("${TARGET_NAME}" PROPERTIES OUTPUT_NAME "otpgen")

if (NOT DISABLE_QTKEYCHAIN)
    target_link_libraries("${TARGET_NAME}" qt5keychain)
endif()

if (BUNDLED_ZLIB)
    target_link_libraries("${TARGET_NAME}" zlibstatic)
    target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/zlib")
else()
    target_link_libraries("${TARGET_NAME}" ${ZLIB_LIBRARIES})
    target_include_directories("${TARGET_NAME}" PRIVATE "${ZLIB_INCLUDE_DIRS}")
endif()

# QR Code Support
if (WITH_QR_CODES)
    target_link_libraries("${TARGET_NAME}" "QRCodeSupportLib")
    target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Source/QRCodeSupport")
endif()

add_dependencies("${TARGET_NAME}" GenerateEmbeddedAssets)

target_include_directories("GuiLibSupport" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/qtsingleapplication")
target_include_directories("GuiLibSupport" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/qtlockedfile")

if (NOT DISABLE_QTKEYCHAIN)
    target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/qtkeychain")
endif()
target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Source/Gui/qt5keychain")
target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/qtsingleapplication")
