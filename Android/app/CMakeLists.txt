cmake_minimum_required(VERSION 3.8)

# Configure libotpgen
set(BUNDLED_ZLIB ON CACHE BOOLEAN "" FORCE)
set(BUNDLED_CRYPTOPP ON CACHE BOOLEAN "" FORCE)
set(WITH_QR_CODES OFF CACHE BOOLEAN "" FORCE)
set(DISABLE_GUI ON CACHE BOOLEAN "" FORCE)
set(DISABLE_CLI ON CACHE BOOLEAN "" FORCE)
set(BUILD_ANDROID ON CACHE BOOLEAN "" FORCE)

# Add libotpgen target
set(CMAKE_SOURCE_DIR_BACKUP "${CMAKE_SOURCE_DIR}")
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../..")
add_subdirectory("${CMAKE_SOURCE_DIR}" ${CMAKE_CURRENT_BINARY_DIR}/libotpgen)
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR_BACKUP}")

# Create libotpgen bindings
add_library(libotpgen-jni-bindings SHARED
    "${CMAKE_SOURCE_DIR}/src/main/cpp/libotpgen-bindings.cpp"
)
set_target_properties(libotpgen-jni-bindings PROPERTIES PREFIX "")
set_target_properties(libotpgen-jni-bindings PROPERTIES OUTPUT_NAME "libotpgen-jni-bindings")
target_link_libraries(libotpgen-jni-bindings CoreLib)
include_directories("${LIBOTPGEN_INCLUDE_DIR}")
