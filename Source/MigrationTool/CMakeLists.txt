###############################################################################
## Migration Tool
###############################################################################
#
# contains old code to migrate the old format to the new SQLite-based format
#

include(SetCppStandard)

# Migration Tool
file(GLOB_RECURSE SourceListMigrationTool
    "${PROJECT_SOURCE_DIR}/Libs/cereal/*.hpp"

    "*.cpp"
    "*.hpp"
)

# MigrationTool
add_executable("MigrationTool" ${SourceListMigrationTool})
SetCppStandard("MigrationTool" 17)
set_target_properties("MigrationTool" PROPERTIES PREFIX "")
set_target_properties("MigrationTool" PROPERTIES OUTPUT_NAME "otpgen-migratedb")

# crypto++ (must match with core)
if (BUNDLED_CRYPTOPP)
    target_include_directories("MigrationTool" PRIVATE "${CRYPTOPP_INCLUDEDIR}")
    target_link_libraries("MigrationTool" cryptopp-static)
else()
    find_package(PkgConfig)

    pkg_check_modules(CRYPTOPP REQUIRED libcryptopp)
    target_include_directories("MigrationTool" PRIVATE "${CRYPTOPP_INCLUDEDIR}")
    target_link_libraries("MigrationTool" ${CRYPTOPP_LDFLAGS})
endif()

# Link to core and shared
target_link_libraries("MigrationTool" "CoreLib" "SharedLib")

# Android toolchain specific extensions
if (BUILD_ANDROID AND BUNDLED_CRYPTOPP)
    # cpufeatures required for crypto++
    target_include_directories(cryptopp-static PRIVATE "$ENV{ANDROID_NDK_ROOT}/sources/android/cpufeatures")
endif()

target_include_directories("MigrationTool" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/cereal")

target_include_directories("MigrationTool" PRIVATE "${PROJECT_SOURCE_DIR}/Source/MigrationTool")
target_include_directories("MigrationTool" PRIVATE "${PROJECT_SOURCE_DIR}/Source/Shared")
