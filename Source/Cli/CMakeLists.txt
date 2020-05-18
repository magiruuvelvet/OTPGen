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

set(TARGET_NAME "${PROJECT_NAME}Cli")

add_library("CliDependencies" STATIC ${SourceListCliDeps})

add_executable("${TARGET_NAME}" ${SourceListCli})
SetCppStandard("${TARGET_NAME}" 17)
target_link_libraries("${TARGET_NAME}" "CoreLib" "SharedLib" "CliDependencies")
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
target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Source/Cli")
target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Source/Shared")
