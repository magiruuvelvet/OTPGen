###############################################################################
## CLI
###############################################################################

include(SetCppStandard)

file(GLOB_RECURSE SourceListCli
    "*.cpp"
    "*.hpp"
)

file(GLOB_RECURSE SourceListCliDeps
    "${PROJECT_SOURCE_DIR}/Libs/PlatformFolders/sago/*.cpp"
    "${PROJECT_SOURCE_DIR}/Libs/PlatformFolders/sago/*.h"
)

file(GLOB_RECURSE SourceListBoost
    "${PROJECT_SOURCE_DIR}/Libs/boost/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/Libs/boost/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/Libs/boost/src/*.h"
)

# define the boost fs no deprecated api macro globally
add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)

set(TARGET_NAME "${PROJECT_NAME}Cli")

add_library("CliDependencies" STATIC ${SourceListCliDeps})
add_library("BoostLib" STATIC ${SourceListBoost})

add_executable("${TARGET_NAME}" ${SourceListCli})
SetCppStandard("${TARGET_NAME}" 17)
target_link_libraries("${TARGET_NAME}" "CoreLib" "SharedLib" "CliDependencies" "BoostLib")
set_target_properties("${TARGET_NAME}" PROPERTIES PREFIX "")
set_target_properties("${TARGET_NAME}" PROPERTIES OUTPUT_NAME "otpgen-cli")

# pthread is required on Linux
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries("${TARGET_NAME}" -lpthread)
endif()

# QR Code Support
if (WITH_QR_CODES)
    target_link_libraries("${TARGET_NAME}" "QRCodeSupportLib")
    target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Source/QRCodeSupport")
endif()

target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/PlatformFolders")
target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/boost")

# add boost headers to global scope to avoid system conflicts
include_directories("${PROJECT_SOURCE_DIR}/Libs/boost")

target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Source/Cli")
target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Source/Shared")
