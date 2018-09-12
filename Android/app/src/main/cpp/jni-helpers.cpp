#include "jni-helpers.hpp"

// convert jstring to UTF-8 encoded std::string
// libotpgen only works with ASCII or UTF-8 and nothing else
const std::string JniHelpers::jstring2string(JNIEnv *env, jstring jStr)
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
