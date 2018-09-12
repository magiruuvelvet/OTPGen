###############################################################################
## Unit tests
###############################################################################

include(SetCppStandard)

file(GLOB_RECURSE SourceListUnitTests
    "${PROJECT_SOURCE_DIR}/Libs/bandit/*.h"
    "${PROJECT_SOURCE_DIR}/Libs/bandit/*.hpp"

    "main.cpp"
    "*.hpp"
)

set(TARGET_NAME "${PROJECT_NAME}Tests")

add_executable("${TARGET_NAME}" ${SourceListUnitTests})
SetCppStandard("${TARGET_NAME}" 17)
target_link_libraries("${TARGET_NAME}" "CoreLib")

# QR Code Support
if (WITH_QR_CODES)
    target_link_libraries("${TARGET_NAME}" "QRCodeSupportLib")
    target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Source/QRCodeSupport")
endif()

target_include_directories("${TARGET_NAME}" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/bandit")
