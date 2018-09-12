#include "jni-helpers.hpp"

#include <Tokens.hpp>
#include <OTPGen.hpp>

JNI_C_LINKAGE {

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeTOTP)(JNIEnv *env, jobject,
    jstring secret, jint digits, jint period, jint algo)
{
    const auto token = OTPGen::computeTOTP(
        JniHelpers::jstring2string(env, secret),
        digits, period, static_cast<OTPToken::ShaAlgorithm>(algo)
    );
    return env->NewStringUTF(token.c_str());
}

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeTOTP_1AtTime)(JNIEnv *env, jobject,
    jstring secret, jint digits, jint period, jint algo, jlong time)
{
    const auto token = OTPGen::computeTOTP(
        time,
        JniHelpers::jstring2string(env, secret),
        digits, period, static_cast<OTPToken::ShaAlgorithm>(algo)
    );
    return env->NewStringUTF(token.c_str());
}

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeHOTP)(JNIEnv *env, jobject,
    jstring secret, jint digits, jint counter, jint algo)
{
    const auto token = OTPGen::computeHOTP(
        JniHelpers::jstring2string(env, secret),
        counter, digits, static_cast<OTPToken::ShaAlgorithm>(algo)
    );
    return env->NewStringUTF(token.c_str());
}

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeSteam)(JNIEnv *env, jobject,
    jstring secret)
{
    const auto token = OTPGen::computeSteam(
        JniHelpers::jstring2string(env, secret)
    );
    return env->NewStringUTF(token.c_str());
}

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeSteam_1AtTime)(JNIEnv *env, jobject,
    jstring secret, jlong time)
{
    const auto token = OTPGen::computeSteam(
        time,
        JniHelpers::jstring2string(env, secret)
    );
    return env->NewStringUTF(token.c_str());
}

}
