#include <jni.h>
#include <string>
#include <unistd.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
    #include <libavcodec/jni.h>

    JNIEXPORT jstring JNICALL
    Java_com_example_hello_FirstActivity_ffmpegVersion(JNIEnv *env, jobject  /* this */) {
        unsigned ver = avformat_version();
        char info[40000] = {0};
        sprintf(info, "avformat_version %u:", ver);
        return env->NewStringUTF(info);
    }
}
