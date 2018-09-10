###############################################################################
## Core Library
###############################################################################

include(SetCppStandard)

# Core library
file(GLOB_RECURSE SourceListCore
    "${CMAKE_SOURCE_DIR}/Libs/cereal/*.hpp"

    "*.cpp"
    "*.hpp"
)

# QR Code Support - depends on libzxing
if (WITH_QR_CODES)
    file(GLOB_RECURSE SourceListQRCodeSupport
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
if (WITH_QR_CODES)
    add_library("QRCodeSupportLib" SHARED ${SourceListQRCodeSupport})
    SetCppStandard("QRCodeSupportLib" 17)
    target_link_libraries("QRCodeSupportLib" libzxing)
    set_target_properties("QRCodeSupportLib" PROPERTIES PREFIX "")
    set_target_properties("QRCodeSupportLib" PROPERTIES OUTPUT_NAME "libotpgen-qrcodesupport")

    target_include_directories("QRCodeSupportLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/core/src")
    target_include_directories("QRCodeSupportLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader")
    target_include_directories("QRCodeSupportLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/QRCodeGenerator")
endif()

# CoreLib
add_library("CoreLib" SHARED ${SourceListCore})
SetCppStandard("CoreLib" 17)
set_target_properties("CoreLib" PROPERTIES PREFIX "")
set_target_properties("CoreLib" PROPERTIES OUTPUT_NAME "libotpgen")

if (WITH_QR_CODES)
    target_link_libraries("CoreLib" "QRCodeSupportLib")
    target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/core/src")
    target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader")
    target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/QRCodeGenerator")
endif()

# Dependencies
find_package(PkgConfig)

# crypto++
pkg_check_modules(CRYPTOPP REQUIRED libcrypto++)
target_include_directories("CoreLib" PRIVATE "${CRYPTOPP_INCLUDEDIR}")

target_link_libraries("CoreLib"
    ${CRYPTOPP_LDFLAGS}
)

target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/cereal")

target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Source/Core")
set(CORELIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Source/Core" PARENT_SCOPE)
