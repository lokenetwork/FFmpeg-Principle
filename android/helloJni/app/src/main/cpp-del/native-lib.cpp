#include <jni.h>
#include <string>
#include <unistd.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_hellojni_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
    #include <libavcodec/jni.h>

    JNIEXPORT jstring JNICALL
    Java_com_example_hellojni_MainActivity_ffmpegInfo(JNIEnv *env, jobject  /* this */) {
        unsigned ver = avformat_version();
        char info[40000] = {0};
        sprintf(info, "avformat_version %u:", ver);
        return env->NewStringUTF(info);
    }
}
