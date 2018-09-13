#include "jni-helpers.hpp"

#include <Tokens.hpp>
#include <OTPGen.hpp>

namespace {
    jstring validateTokenResultAndReturn(JNIEnv *env, const OTPGenErrorCode &err, const std::string &token)
    {
        if (err == OTPGenErrorCode::Valid && !token.empty())
        {
            return env->NewStringUTF(token.c_str());
        }
        else
        {
            switch (err)
            {
                case OTPGenErrorCode::InvalidBase32Input:
                    JniHelpers::throwException(env, "OTPGen: Invalid base-32 input!");
                    return nullptr;

                case OTPGenErrorCode::InvalidAlgorithm:
                    JniHelpers::throwException(env, "OTPGen: Invalid algorithm!");
                    return nullptr;

                case OTPGenErrorCode::InvalidDigits:
                    JniHelpers::throwException(env, "OTPGen: Invalid digit count!");
                    return nullptr;

                case OTPGenErrorCode::InvalidPeriod:
                    JniHelpers::throwException(env, "OTPGen: Invalid period!");
                    return nullptr;
            }
        }
    }
}

JNI_C_LINKAGE {

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeTOTP)(JNIEnv *env, jobject,
    jstring secret, jint digits, jint period, jint algo)
{
    auto err = OTPGenErrorCode::Valid;
    const auto token = OTPGen::computeTOTP(
        JniHelpers::jstring2string(env, secret),
        digits, period, static_cast<OTPToken::ShaAlgorithm>(algo),
        &err
    );
    return validateTokenResultAndReturn(env, err, token);
}

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeTOTP_1AtTime)(JNIEnv *env, jobject,
    jstring secret, jint digits, jint period, jint algo, jlong time)
{
    auto err = OTPGenErrorCode::Valid;
    const auto token = OTPGen::computeTOTP(
        time,
        JniHelpers::jstring2string(env, secret),
        digits, period, static_cast<OTPToken::ShaAlgorithm>(algo),
        &err
    );
    return validateTokenResultAndReturn(env, err, token);
}

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeHOTP)(JNIEnv *env, jobject,
    jstring secret, jint digits, jint counter, jint algo)
{
    auto err = OTPGenErrorCode::Valid;
    const auto token = OTPGen::computeHOTP(
        JniHelpers::jstring2string(env, secret),
        counter, digits, static_cast<OTPToken::ShaAlgorithm>(algo),
        &err
    );
    return validateTokenResultAndReturn(env, err, token);
}

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeSteam)(JNIEnv *env, jobject,
    jstring secret)
{
    auto err = OTPGenErrorCode::Valid;
    const auto token = OTPGen::computeSteam(
        JniHelpers::jstring2string(env, secret),
        &err
    );
    return validateTokenResultAndReturn(env, err, token);
}

JSTRING JNI_FUNCTION(OTPGen, 00024Companion__1computeSteam_1AtTime)(JNIEnv *env, jobject,
    jstring secret, jlong time)
{
    auto err = OTPGenErrorCode::Valid;
    const auto token = OTPGen::computeSteam(
        time,
        JniHelpers::jstring2string(env, secret),
        &err
    );
    return validateTokenResultAndReturn(env, err, token);
}

}
