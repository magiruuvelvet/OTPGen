###############################################################################
## QR Code Support Library
###############################################################################

include(SetCppStandard)

# QR Code Support - depends on libzxing
if (WITH_QR_CODES)
    file(GLOB_RECURSE SourceListQRCodeSupport
        "${CMAKE_SOURCE_DIR}/Source/QRCodeSupport/*.cpp"
        "${CMAKE_SOURCE_DIR}/Source/QRCodeSupport/*.hpp"

        "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader/*.cpp"
        "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader/*.h"

        "${CMAKE_SOURCE_DIR}/Libs/QRCodeGenerator/*.cpp"
        "${CMAKE_SOURCE_DIR}/Libs/QRCodeGenerator/*.hpp"
    )

    message(STATUS "   -> Configuring zxing-cpp...")
    add_subdirectory("${CMAKE_SOURCE_DIR}/Libs/zxing-cpp" "${CMAKE_CURRENT_BINARY_DIR}/libzxing")
    message(STATUS "   -> Configured zxing-cpp.")
endif()

# QRCodeSupportLib
add_library("QRCodeSupportLib" SHARED ${SourceListQRCodeSupport})
SetCppStandard("QRCodeSupportLib" 17)
target_link_libraries("QRCodeSupportLib" libzxing)
set_target_properties("QRCodeSupportLib" PROPERTIES PREFIX "")
set_target_properties("QRCodeSupportLib" PROPERTIES OUTPUT_NAME "libotpgen-qrcodesupport")

target_include_directories("QRCodeSupportLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/core/src")
target_include_directories("QRCodeSupportLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader")
target_include_directories("QRCodeSupportLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/QRCodeGenerator")

set(QRCODESUPPORTLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Source/QRCodeSupport" PARENT_SCOPE)
