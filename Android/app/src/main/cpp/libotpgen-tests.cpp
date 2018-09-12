#include "jni-helpers.hpp"

#include <Tokens.hpp>
#include <OTPGen.hpp>

JNI_C_LINKAGE {

//
// Instrumented Unit Tests
//  don't use outside of tests, unstable API
//

#define TESTNAME(name) JNI_FUNCTION(DeviceUnitTests, name)

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
        JniHelpers::jstring2string(env, secret),
        digits,
        period,
        OTPToken::SHA1);
    return env->NewStringUTF(token.c_str());
}

JSTRING TESTNAME(testHotpGeneration)(JNIEnv* env, jobject,
    jstring secret, jint digits, jint counter, jlong time)
{
    const auto token = OTPGen::computeHOTP(
        JniHelpers::jstring2string(env, secret),
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
        JniHelpers::jstring2string(env, secret));
    return env->NewStringUTF(token.c_str());
}

}
