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

# zlib
set(BUNDLED_ZLIB OFF CACHE BOOLEAN "Use the bundled zlib.")
set(BUNDLED_ZLIB_ASM686 OFF CACHE BOOLEAN "Use optimized x86-32 asm.")
set(BUNDLED_ZLIB_AMD64 OFF CACHE BOOLEAN "Use optimized x86-64 asm.")
if (BUNDLED_ZLIB)
    message(STATUS "   -> Configuring bundled zlib...")
    if (BUNDLED_ZLIB_ASM686)
        set(ASM686 ON CACHE BOOLEAN "" FORCE)
    endif()
    if (BUNDLED_ZLIB_AMD64)
        set(AMD64 ON CACHE BOOLEAN "" FORCE)
    endif()
    set(BUILD_SHARED_LIBS OFF CACHE BOOLEAN "" FORCE)
    set(SKIP_INSTALL_ALL ON CACHE BOOLEAN "" FORCE)
    add_subdirectory("${CMAKE_SOURCE_DIR}/Libs/zlib" "${CMAKE_CURRENT_BINARY_DIR}/zlib")
    target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/zlib")
    message(STATUS "   -> Configured bundled zlib.")
else()
    message(STATUS "Using shared zlib.")
    find_package(ZLIB REQUIRED)
    target_include_directories("CoreLib" PRIVATE "${ZLIB_INCLUDE_DIRS}")
endif()

# crypto++
pkg_check_modules(CRYPTOPP REQUIRED libcrypto++)
target_include_directories("CoreLib" PRIVATE "${CRYPTOPP_INCLUDEDIR}")

target_link_libraries("CoreLib"
    ${CRYPTOPP_LDFLAGS}
)

if (BUNDLED_ZLIB)
    target_link_libraries("CoreLib" zlibstatic)
else()
    target_link_libraries("CoreLib" ${ZLIB_LIBRARIES})
endif()

target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Libs/cereal")

target_include_directories("CoreLib" PRIVATE "${CMAKE_SOURCE_DIR}/Source/Core")
set(CORELIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Source/Core" PARENT_SCOPE)
