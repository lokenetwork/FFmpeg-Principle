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

    if( 1==type ){
        AVPacket *pkt = av_packet_alloc();
        int ret = 0;
        ret = av_read_frame(fmt_ctx, pkt);
        if (ret < 0) {
            printf("read fail \n");
            //省略错误处理。
            return ret;
        }else{
            printf("read success \n");
            printf("stream 0 type : %d \n",fmt_ctx->streams[0]->codecpar->codec_type);
            printf("stream 1 type : %d \n",fmt_ctx->streams[1]->codecpar->codec_type);
            printf("stream_index : %d \n",pkt->stream_index);
            printf("duration : %I64d ,time_base : %d/%d \n",pkt->duration,
                   fmt_ctx->streams[0]->time_base.num,fmt_ctx->streams[0]->time_base.den);
            printf("size : %d \n",pkt->size);
            printf("pos : %I64d \n",pkt->pos);
            printf("data : %x %x %x %x %x %x %x \n",
                   pkt->data[0],pkt->data[1],pkt->data[2],pkt->data[3],pkt->data[4],
                   pkt->data[5],pkt->data[6]);
        }
        av_packet_free(&pkt);
    }

    if( 2==type ){

        AVPacket *pkt = av_packet_alloc();

        int ret = 0, i;
        for(i = 0; i < 100 ;i++){
            ret = av_read_frame(fmt_ctx, pkt);
            if (ret < 0) {
                printf("read fail \n");
                //省略错误处理。
                return ret;
            }else{
                printf("read success \n");
                printf("stream 0 type : %d \n",fmt_ctx->streams[0]->codecpar->codec_type);
                printf("stream 1 type : %d \n",fmt_ctx->streams[1]->codecpar->codec_type);
                printf("stream_index : %d \n",pkt->stream_index);
                printf("duration : %I64d ,time_base : %d/%d \n",pkt->duration,
                       fmt_ctx->streams[1]->time_base.num,fmt_ctx->streams[0]->time_base.den);
                printf("size : %d \n",pkt->size);
                printf("pos : %I64d \n",pkt->pos);
                printf("data : %x %x %x %x %x %x %x \n",
                       pkt->data[0],pkt->data[1],pkt->data[2],pkt->data[3],pkt->data[4],
                       pkt->data[5],pkt->data[6]);

                av_packet_unref(pkt);
            }
        }

        av_packet_free(&pkt);

    }


    return 0;
}
