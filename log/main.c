#include <stdio.h>
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/bprint.h"

void av_log_loken_callback(void *avcl, int level, const char *fmt, va_list vl){
    AVBPrint bPrint;
    av_bprint_init(&bPrint,1024,10240);
    av_vbprintf(&bPrint, fmt, vl);
    printf("log: %s",bPrint.str);
};

int main()
{
    av_log_set_callback(av_log_loken_callback);
    av_log(NULL, AV_LOG_INFO, "FFmpeg Principle %s %d \n","666",333);
    return 0;
}
