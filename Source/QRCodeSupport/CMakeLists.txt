###############################################################################
## QR Code Support Library
###############################################################################

include(SetCppStandard)

# QR Code Support - depends on libzxing
if (WITH_QR_CODES)
    file(GLOB_RECURSE SourceListQRCodeSupport
        "${PROJECT_SOURCE_DIR}/Source/QRCodeSupport/*.cpp"
        "${PROJECT_SOURCE_DIR}/Source/QRCodeSupport/*.hpp"

        "${PROJECT_SOURCE_DIR}/Libs/zxing-cpp/imagereader/*.cpp"
        "${PROJECT_SOURCE_DIR}/Libs/zxing-cpp/imagereader/*.h"

        "${PROJECT_SOURCE_DIR}/Libs/QRCodeGenerator/*.cpp"
        "${PROJECT_SOURCE_DIR}/Libs/QRCodeGenerator/*.hpp"
    )

    message(STATUS "   -> Configuring zxing-cpp...")
    add_subdirectory("${PROJECT_SOURCE_DIR}/Libs/zxing-cpp" "${CMAKE_CURRENT_BINARY_DIR}/libzxing" EXCLUDE_FROM_ALL)
    message(STATUS "   -> Configured zxing-cpp.")
endif()

# QRCodeSupportLib
add_library("QRCodeSupportLib" SHARED ${SourceListQRCodeSupport})
SetCppStandard("QRCodeSupportLib" 17)
target_link_libraries("QRCodeSupportLib" libzxing)
set_target_properties("QRCodeSupportLib" PROPERTIES PREFIX "")
set_target_properties("QRCodeSupportLib" PROPERTIES OUTPUT_NAME "libotpgen-qrcodesupport")

target_include_directories("QRCodeSupportLib" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/zxing-cpp/core/src")
target_include_directories("QRCodeSupportLib" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/zxing-cpp/imagereader")
target_include_directories("QRCodeSupportLib" PRIVATE "${PROJECT_SOURCE_DIR}/Libs/QRCodeGenerator")

set(QRCODESUPPORTLIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Source/QRCodeSupport" PARENT_SCOPE)
