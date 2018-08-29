###############################################################################
## CLI
###############################################################################

include(SetCppStandard)

file(GLOB_RECURSE SourceListCli
    "*.cpp"
    "*.hpp"
)

file(GLOB_RECURSE SourceListCliDeps
    "${CMAKE_SOURCE_DIR}/Libs/PlatformFolders/sago/*.cpp"
    "${CMAKE_SOURCE_DIR}/Libs/PlatformFolders/sago/*.h"
)

set(TARGET_NAME "${PROJECT_NAME}Cli")

add_library("CliDependencies" STATIC ${SourceListCliDeps})

add_executable("${TARGET_NAME}" ${SourceListCli})
SetCppStandard("${TARGET_NAME}" 17)
target_link_libraries("${TARGET_NAME}" "CoreLib" "SharedLib" "CliDependencies")

target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/PlatformFolders")

target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/Source/Cli")
