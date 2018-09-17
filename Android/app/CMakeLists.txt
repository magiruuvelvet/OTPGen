cmake_minimum_required(VERSION 3.8)

# Autogenerate JNI bindings using Swig
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
    message(WARNING "WARNING: Building on non-Android system!")
    find_package(JNI REQUIRED)
    message(STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message(STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
    set(HOST_BUILD ON)
endif()

# Configure libotpgen
set(BUNDLED_ZLIB ON CACHE BOOLEAN "" FORCE)
set(BUNDLED_CRYPTOPP ON CACHE BOOLEAN "" FORCE)
set(WITH_QR_CODES ON CACHE BOOLEAN "" FORCE)
set(DISABLE_GUI ON CACHE BOOLEAN "" FORCE)
set(DISABLE_CLI ON CACHE BOOLEAN "" FORCE)
set(BUILD_ANDROID ON CACHE BOOLEAN "" FORCE)

# Add libotpgen target
set(CMAKE_SOURCE_DIR_BACKUP "${CMAKE_SOURCE_DIR}")
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../..")
add_subdirectory("${CMAKE_SOURCE_DIR}" ${CMAKE_CURRENT_BINARY_DIR}/libotpgen)
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR_BACKUP}")

# Include directory of libotpgen (required for Swig)
include_directories("${LIBOTPGEN_INCLUDE_DIR}")
include_directories("${LIBOTPGEN_QRCODESUPPORT_INCLUDE_DIR}")

#
# Generate libotpgen JNI bindings
#

# Set Swig interface file and Java package name
set(SWIG_I_FILE "${CMAKE_SOURCE_DIR}/src/main/cpp/libotpgen.i")
set(JAVA_GEN_PACKAGE "com.magiruuvelvet.libotpgen")
string(REPLACE "." "/" JAVA_GEN_SUBDIR ${JAVA_GEN_PACKAGE})
set_source_files_properties("${CMAKE_SOURCE_DIR}/src/main/cpp/libotpgen.i" PROPERTIES CPLUSPLUS ON)
set_source_files_properties("${CMAKE_SOURCE_DIR}/src/main/cpp/libotpgen.i" PROPERTIES SWIG_FLAGS "")

# Output directory of generated Java package
set(JAVA_GEN_DIR ${PROJECT_SOURCE_DIR}/src/main/java/${JAVA_GEN_SUBDIR})
set_property(SOURCE "${CMAKE_SOURCE_DIR}/src/main/cpp/libotpgen.i" PROPERTY SWIG_MODULE_NAME ${JAVA_GEN_PACKAGE})

# Swig command flags
set(CMAKE_SWIG_FLAGS -java -package ${JAVA_GEN_PACKAGE})
set(CMAKE_SWIG_OUTDIR ${JAVA_GEN_DIR})

# Swig JNI binding library
SWIG_ADD_LIBRARY(libotpgen-jni-bindings
    LANGUAGE c++
    SOURCES "${CMAKE_SOURCE_DIR}/src/main/cpp/libotpgen.i"
)
SWIG_LINK_LIBRARIES(libotpgen-jni-bindings CoreLib QRCodeSupportLib)

if (HOST_BUILD)
    message(STATUS "Finalizing host build configuration.")
    target_include_directories(libotpgen-jni-bindings PRIVATE "${JNI_INCLUDE_DIRS}")
    SWIG_LINK_LIBRARIES(libotpgen-jni-bindings ${JNI_LIBRARIES})
endif()
