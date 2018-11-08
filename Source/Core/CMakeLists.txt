###############################################################################
## Core Library
###############################################################################

include(SetCppStandard)

# Core library
file(GLOB_RECURSE SourceListCore
    "${PROJECT_SOURCE_DIR}/Libs/cereal/*.hpp"

    "*.cpp"
    "*.hpp"
)

# CoreLib
add_library("CoreLib" SHARED ${SourceListCore})
SetCppStandard("CoreLib" 17)
set_target_properties("CoreLib" PROPERTIES PREFIX "")
set_target_properties("CoreLib" PROPERTIES OUTPUT_NAME "libotpgen")

# crypto++
set(BUNDLED_CRYPTOPP OFF CACHE BOOLEAN "Use the bundled crypto++ library.")
if (BUNDLED_CRYPTOPP)
    message(STATUS "Building with bundled crypto++")
    message(STATUS "   -> Configuring bundled crypto++ library...")
    add_subdirectory("${PROJECT_SOURCE_DIR}/Libs/cryptopp" "${CMAKE_CURRENT_BINARY_DIR}/cryptopp" EXCLUDE_FROM_ALL)
    target_include_directories("CoreLib" PRIVATE "${CRYPTOPP_INCLUDEDIR}")
    target_link_libraries("CoreLib" cryptopp-static)
    message(STATUS "   -> Configured bundled crypto++ library.")
else()
    find_package(PkgConfig)

    pkg_check_modules(CRYPTOPP REQUIRED libcrypto++)
    target_include_directories("CoreLib" PRIVATE "${CRYPTOPP_INCLUDEDIR}")
    target_link_libraries("CoreLib" ${CRYPTOPP_LDFLAGS})
endif()

# sqlite3
# (must be bundled because custom features are enabled)
# (not all operating systems have those in their binary releases)
# see "sqlite3/sqlite/sqlite-config.h" for a list of enabled features
message(STATUS "   -> Configuring bundled SQLite3 library...")
add_library(sqlite3_custom STATIC "${PROJECT_SOURCE_DIR}/Libs/sqlite3/sqlite/sqlite3.c")
target_link_libraries("CoreLib" sqlite3_custom)
target_include_directories("CoreLib" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/sqlite3")
message(STATUS "   -> Configured bundled SQLite3 library.")

# Android toolchain specific extensions
if (BUILD_ANDROID AND BUNDLED_CRYPTOPP)
    # cpufeatures required for crypto++
    target_include_directories(cryptopp-static PRIVATE "$ENV{ANDROID_NDK_ROOT}/sources/android/cpufeatures")
endif()

target_include_directories("CoreLib" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/cereal")

target_include_directories("CoreLib" PRIVATE "${PROJECT_SOURCE_DIR}/Source/Core")
set(CORELIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Source/Core" PARENT_SCOPE)
