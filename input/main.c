#include <stdio.h>
#include "libavformat/avformat.h"
int main()
{

    AVFormatContext *fmt_ctx = NULL;
    int type = 2;
    int err;
    // 提示，要把 juren-30s.mp4 文件放到 Debug 目录下才能找到。
    char filename[] = "juren-30s.mp4";

    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
      printf("error code %d \n",AVERROR(ENOMEM));
      return 1;
    }

    if(1==type){

        if((err = avformat_open_input(&fmt_ctx, filename,NULL,NULL)) < 0){
            printf("error code %d \n",err);
        }else{
            printf("open success \n");
            printf("filename - %s \n",fmt_ctx->url);
            printf("duration - %I64d \n",fmt_ctx->duration);
            printf("nb_streams - %u \n",fmt_ctx->nb_streams);
            for( int i=0; i < fmt_ctx->nb_streams; i++ ){
                 printf("stream codec_type - %d \n",fmt_ctx->streams[i]->codecpar->codec_type);
            }
            printf("iformat name - %s \n",fmt_ctx->iformat->name);
            printf("iformat long name - %s \n",fmt_ctx->iformat->long_name);
       }

    }

    if(2==type){
        //设置探测大小
        AVDictionary *format_opts = NULL;
        av_dict_set(&format_opts, "probesize", "32", 0);

        if((err = avformat_open_input(&fmt_ctx, filename,NULL,&format_opts)) < 0){
            printf("error code %d \n",err);
        }else{
            avformat_find_stream_info(fmt_ctx,NULL);
            printf("open success \n");
            printf("filename - %s \n",fmt_ctx->url);
            printf("duration - %I64d \n",fmt_ctx->duration);
            printf("nb_streams - %u \n",fmt_ctx->nb_streams);
            for( int i=0; i < fmt_ctx->nb_streams; i++ ){
                 printf("stream codec_type - %d \n",fmt_ctx->streams[i]->codecpar->codec_type);
            }
            printf("iformat name - %s \n",fmt_ctx->iformat->name);
            printf("iformat long name - %s \n",fmt_ctx->iformat->long_name);
       }
       av_dict_free(&format_opts);
    }

    return 0;
}
