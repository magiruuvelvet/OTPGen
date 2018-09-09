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

# Core library support (libcotp)
file(GLOB_RECURSE SourceListCoreSupport
    "${CMAKE_SOURCE_DIR}/Libs/libbaseencode/*.c"
    "${CMAKE_SOURCE_DIR}/Libs/libbaseencode/*.h"

    "${CMAKE_SOURCE_DIR}/Libs/libcotp/*.c"
    "${CMAKE_SOURCE_DIR}/Libs/libcotp/*.h"
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

# CoreLibSupport
add_library("CoreLibSupport" STATIC ${SourceListCoreSupport})
SetCppStandard("CoreLibSupport" 17)
target_link_libraries("CoreLibSupport" ${GCRYPT_LIBRARIES})

# QRCodeSupportLib
if (WITH_QR_CODES)
    add_library("QRCodeSupportLib" STATIC ${SourceListQRCodeSupport})
    SetCppStandard("QRCodeSupportLib" 17)
    target_link_libraries("QRCodeSupportLib" libzxing)

    target_include_directories("QRCodeSupportLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/core/src")
    target_include_directories("QRCodeSupportLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader")
    target_include_directories("QRCodeSupportLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/QRCodeGenerator")
endif()

# CoreLib
add_library("CoreLib" STATIC ${SourceListCore})
SetCppStandard("CoreLib" 17)
target_link_libraries("CoreLib" "CoreLibSupport")

if (WITH_QR_CODES)
    target_link_libraries("CoreLib" "QRCodeSupportLib")
    target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/core/src")
    target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zxing-cpp/imagereader")
    target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/QRCodeGenerator")
endif()

# Dependencies
find_package(PkgConfig)

find_package(Gcrypt REQUIRED)
target_include_directories("CoreLib" PRIVATE ${GCRYPT_INCLUDE_DIR})
target_include_directories("CoreLibSupport" PRIVATE ${GCRYPT_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
target_include_directories("CoreLib" PRIVATE "${ZLIB_INCLUDE_DIRS}")

pkg_check_modules(CRYPTOPP REQUIRED libcrypto++)
target_include_directories("CoreLib" PRIVATE "${CRYPTOPP_INCLUDEDIR}")

target_link_libraries("CoreLib"
    ${GCRYPT_LIBRARIES}
    ${CRYPTOPP_LDFLAGS}
    ${ZLIB_LIBRARIES}
)

target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/cereal")
target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/libbaseencode")
target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/libcotp")

target_include_directories("CoreLibSupport" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/libbaseencode/libbaseencode")

target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Source/Core")
set(CORELIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Source/Core" PARENT_SCOPE)
