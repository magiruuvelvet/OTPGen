//
// Java HEAP True Garbage Collection for C++ objects
//

// see http://www.swig.org/Doc3.0/Java.html

%{
#include <stdexcept>
#include "jni.h"

/**
 *  A stash area embedded in each allocation to hold java handles
 */
struct Jalloc {
  jbyteArray jba;
  jobject ref;
};

static JavaVM *cached_jvm = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
  cached_jvm = jvm;
  return JNI_VERSION_1_2;
}

static JNIEnv * JNU_GetEnv() {
  JNIEnv *env;
  jint rc = cached_jvm->GetEnv((void **)&env, JNI_VERSION_1_2);
  if (rc == JNI_EDETACHED)
    throw std::runtime_error("current thread not attached");
  if (rc == JNI_EVERSION)
    throw std::runtime_error("jni version not supported");
  return env;
}

void * operator new(size_t t) {
  if (cached_jvm != 0) {
    JNIEnv *env = JNU_GetEnv();
    jbyteArray jba = env->NewByteArray((int) t + sizeof(Jalloc));
    if (env->ExceptionOccurred())
      throw std::bad_alloc();
    void *jbuffer = static_cast<void *>(env->GetByteArrayElements(jba, 0));
    if (env->ExceptionOccurred())
      throw std::bad_alloc();
    Jalloc *pJalloc = static_cast<Jalloc *>(jbuffer);
    pJalloc->jba = jba;
    /* Assign a global reference so byte array will persist until delete'ed */
    pJalloc->ref = env->NewGlobalRef(jba);
    if (env->ExceptionOccurred())
      throw std::bad_alloc();
    return static_cast<void *>(static_cast<char *>(jbuffer) + sizeof(Jalloc));
  }
  else { /* JNI_OnLoad not called, use malloc and mark as special */
    Jalloc *pJalloc = static_cast<Jalloc *>(malloc((int) t + sizeof(Jalloc)));
    if (!pJalloc)
      throw std::bad_alloc();
    pJalloc->ref = 0;
    return static_cast<void *>(
        static_cast<char *>(static_cast<void *>(pJalloc)) + sizeof(Jalloc));
  }
}

void operator delete(void *v) {
  if (v != 0) {
    void *buffer = static_cast<void *>( static_cast<char *>(v) - sizeof(Jalloc));
    Jalloc *pJalloc = static_cast<Jalloc *>(buffer);
    if (pJalloc->ref) {
      JNIEnv *env = JNU_GetEnv();
      env->DeleteGlobalRef(pJalloc->ref);
      env->ReleaseByteArrayElements(pJalloc->jba, static_cast<jbyte *>(buffer), 0);
    }
    else {
      free(buffer);
    }
  }
}
%}
