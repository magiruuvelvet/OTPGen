###############################################################################
## Unit tests
###############################################################################

include(SetCppStandard)

file(GLOB_RECURSE SourceListUnitTests
    "${CMAKE_SOURCE_DIR}/Libs/bandit/*.h"
    "${CMAKE_SOURCE_DIR}/Libs/bandit/*.hpp"

    "main.cpp"
    "*.hpp"
)

set(TARGET_NAME "${PROJECT_NAME}Tests")

add_executable("${TARGET_NAME}" ${SourceListUnitTests})
SetCppStandard("${TARGET_NAME}" 17)
target_link_libraries("${TARGET_NAME}" "CoreLib")

target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/bandit")
