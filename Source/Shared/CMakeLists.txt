###############################################################################
## Shared library
###############################################################################

include(SetCppStandard)

file(GLOB_RECURSE SourceListShared
    "*.cpp"
    "*.hpp"
)

add_library("SharedLib" STATIC ${SourceListShared})
SetCppStandard("SharedLib" 17)
target_link_libraries("SharedLib" "CoreLib")

target_include_directories("SharedLib" PRIVATE "${PROJECT_SOURCE_DIR}/Source/Shared")
set(SHAREDLIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Source/Shared" PARENT_SCOPE)
