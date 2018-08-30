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

file(GLOB_RECURSE SourceListBoost
    "${CMAKE_SOURCE_DIR}/Libs/boost/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/Libs/boost/src/*.hpp"
    "${CMAKE_SOURCE_DIR}/Libs/boost/src/*.h"
)

# define the boost fs no deprecated api macro globally
add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)

set(TARGET_NAME "${PROJECT_NAME}Cli")

add_library("CliDependencies" STATIC ${SourceListCliDeps})
add_library("BoostLib" STATIC ${SourceListBoost})

add_executable("${TARGET_NAME}" ${SourceListCli})
SetCppStandard("${TARGET_NAME}" 17)
target_link_libraries("${TARGET_NAME}" "CoreLib" "SharedLib" "CliDependencies" "BoostLib")

target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/PlatformFolders")
target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/boost")

# add boost headers to global scope to avoid system conflicts
include_directories("${CMAKE_SOURCE_DIR}/Libs/boost")

target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/Source/Cli")
