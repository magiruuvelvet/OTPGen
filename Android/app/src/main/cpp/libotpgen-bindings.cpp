#include <jni.h>
#include <string>

#include <Tokens.hpp>
#include <OTPGen.hpp>

// C bindings for C++ OOP library
// https://stackoverflow.com/a/2045860

// convert jstring to UTF-8 encoded std::string
// libotpgen only works with ASCII or UTF-8 and nothing else
const std::string jstring2string(JNIEnv *env, jstring jStr)
{
    if (!jStr)
    {
        return {};
    }

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = static_cast<jbyteArray>(env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8")));

    std::size_t length = static_cast<std::size_t>(env->GetArrayLength(stringJbytes));
    jbyte *pBytes = env->GetByteArrayElements(stringJbytes, nullptr);

    std::string ret = std::string(reinterpret_cast<char*>(pBytes), length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

// use C function linkage (jni symbol names)
// verify with "nm libotpgen-jni-bindings.so | grep Java" on problems
extern "C" {

#define JSTRING JNIEXPORT jstring JNICALL

JSTRING Java_com_magiruuvelvet_otpgen_MainActivity_stringFromJNI(JNIEnv *env, jobject)
{
    std::string hello = "Welcome to OTPGen";
    return env->NewStringUTF(hello.c_str());
}



//
// Instrumented Unit Tests
//  don't use outside of tests, unstable API
//

#define TESTNAME(name) Java_com_magiruuvelvet_otpgen_DeviceUnitTests_##name

JSTRING TESTNAME(stringFromJNI)(JNIEnv *env, jobject)
{
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JSTRING TESTNAME(testTotpGeneration)(JNIEnv* env, jobject,
    jstring secret, jint digits, jint period, jlong time)
{
    const auto token = OTPGen::computeTOTP(
        time,
        jstring2string(env, secret),
        digits,
        period,
        OTPToken::SHA1);
    return env->NewStringUTF(token.c_str());
}

JSTRING TESTNAME(testHotpGeneration)(JNIEnv* env, jobject,
    jstring secret, jint digits, jint counter, jlong time)
{
    const auto token = OTPGen::computeHOTP(
        jstring2string(env, secret),
        counter,
        digits,
        OTPToken::SHA1);
    return env->NewStringUTF(token.c_str());
}

JSTRING TESTNAME(testSteamGeneration)(JNIEnv* env, jobject,
    jstring secret, jlong time)
{
    const auto token = OTPGen::computeSteam(
        time,
        jstring2string(env, secret));
    return env->NewStringUTF(token.c_str());
}

} // extern C
