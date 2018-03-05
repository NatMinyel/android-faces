#include <jni.h>

JNIEXPORT jint JNICALL
Java_com_nathnael_faces_FullscreenActivity_getFaces(JNIEnv *env, jobject instance, jobject b,
                                                    jstring name_) {
    const char *name = (*env)->GetStringUTFChars(env, name_, 0);

    // TODO

    (*env)->ReleaseStringUTFChars(env, name_, name);
}