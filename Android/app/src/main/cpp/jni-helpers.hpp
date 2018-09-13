#ifndef JNI_HELPERS_HPP
#define JNI_HELPERS_HPP

#include <jni.h>
#include <string>

class JniHelpers final
{
    JniHelpers() = delete;

public:

    // convert jstring into UTF-8 encoded std::string
    static const std::string jstring2string(JNIEnv *env, jstring jStr);

    // create java exception and throw it
    static jint throwException(JNIEnv *env, const std::string &message);
};

// use C function linkage (jni symbol names)
// verify with "nm libotpgen-jni-bindings.so | grep Java" on problems
#define JNI_C_LINKAGE extern "C"

// function naming helper
#define JNI_FUNCTION(Class, Name) \
    Java_com_magiruuvelvet_otpgen_##Class##_##Name

// useful export macros
#define JSTRING JNIEXPORT jstring JNICALL

#endif // JNI_HELPERS_HPP
