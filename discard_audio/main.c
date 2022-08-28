#include <stdio.h>
#include "libavformat/avformat.h"
int main()
{

    AVFormatContext *fmt_ctx = NULL;
    int type = 1;

    int err;
    //提示，要把 juren-30s.mp4 文件放到 Debug 目录下才能找到。
    char filename[] = "juren-30s.mp4";

    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
      printf("error code %d \n",AVERROR(ENOMEM));
      return ENOMEM;
    }

    if((err = avformat_open_input(&fmt_ctx, filename,NULL,NULL)) < 0){
        printf("error code %d \n",err);
        return err;
    }
    printf("stream 0 type : %d \n",fmt_ctx->streams[0]->codecpar->codec_type);
    printf("stream 1 type : %d \n",fmt_ctx->streams[1]->codecpar->codec_type);

    fmt_ctx->streams[1]->discard = AVDISCARD_ALL;

    AVPacket *pkt = av_packet_alloc();
    int ret = 0, i;
    for(i = 0; i < 100 ;i++){
        ret = av_read_frame(fmt_ctx, pkt);
        if (ret < 0) {
            printf("read fail \n");
            //省略错误处理。
            return ret;
        }else{
            printf("stream_index : %d \n",pkt->stream_index);
            av_packet_unref(pkt);
        }
    }
    av_packet_free(&pkt);
    return 0;
}
