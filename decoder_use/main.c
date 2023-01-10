#include <stdio.h>
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

int main()
{
    AVFormatContext *fmt_ctx = NULL;
    int type = 1;
    int ret = 0;

    int err;
    //提示，要把 juren-30s.mp4 文件放到 Debug 目录下才能找到。
    char filename[] = "juren-30s.mp4";

    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
      printf("error code %d \n",AVERROR(ENOMEM));
      return ENOMEM;
    }

    if((err = avformat_open_input(&fmt_ctx, filename,NULL,NULL)) < 0){
        printf("can not open file %d \n",err);
        return err;
    }

    if( 1 == type ){

        //本文只初始化 视频解码器。音频解码器不管
        AVCodecContext *avctx = avcodec_alloc_context3(NULL);
        ret = avcodec_parameters_to_context(avctx, fmt_ctx->streams[0]->codecpar);
        if (ret < 0){
            printf("error code %d \n",ret);
            return ret;
        }

        AVCodec *codec = avcodec_find_decoder(avctx->codec_id);
        if ((ret = avcodec_open2(avctx, codec, NULL)) < 0) {
            printf("open codec faile %d \n",ret);
            return ret;
        }

        AVPacket *pkt = av_packet_alloc();
        AVFrame *frame = av_frame_alloc();
        int frame_num = 0;
        int read_end = 0;
        for(;;){
            if( 1 == read_end ){
                break;
            }

            ret = av_read_frame(fmt_ctx, pkt);
            //跳过不处理音频包
            if( 1 == pkt->stream_index ){
                av_packet_unref(pkt);
                continue;
            }
            if ( AVERROR_EOF == ret) {
                //已经读完文件
                //读取完文件，这时候 pkt 的 data 跟 size 应该是 null
                //冲刷解码器
                avcodec_send_packet(avctx, pkt);
                //释放 pkt 里面的编码数据
                av_packet_unref(pkt);

                //循环不断从解码器读数据，直到没有数据可读。
                for(;;){
                    //读取 AVFrame
                    ret = avcodec_receive_frame(avctx, frame);
                    /* 释放 frame 里面的YUV数据，
                     * 由于 avcodec_receive_frame 函数里面会调用 av_frame_unref，所以下面的代码可以注释。
                     * 所以我们不需要 手动 unref 这个 AVFrame
                     * */
                    //av_frame_unref(frame);

                    if( AVERROR(EAGAIN) == ret ){
                        //提示 EAGAIN 代表 解码器 需要 更多的 AVPacket
                        //跳出 第一层 for，让 解码器拿到更多的 AVPacket
                        break;
                    }else if( AVERROR_EOF == ret ){
                        /* 提示 AVERROR_EOF 代表之前已经往 解码器发送了一个 data 跟 size 都是 NULL 的 AVPacket
                         * 发送 NULL 的 AVPacket 是提示解码器把所有的缓存帧全都刷出来。
                         * 通常只有在 读完输入文件才会发送 NULL 的 AVPacket，或者需要用现有的解码器解码另一个的视频流才会这么干。
                         *
                         * */

                        //跳出 第二层 for，文件已经解码完毕。
                        read_end = 1;
                        break;
                    }else if( ret >= 0 ){

                        //解码出一帧 YUV 数据，打印一些信息。
                        printf("decode success ----\n");
                        printf("width : %d , height : %d \n",frame->width,frame->height);
                        printf("pts : %I64d , duration : %I64d \n",frame->pts,frame->pkt_duration);
                        printf("format : %d \n",frame->format);
                        printf("key_frame : %d \n",frame->key_frame);
                        printf("AVPictureType : %d \n",frame->pict_type);
                        int num = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, 1920, 1080, 1);
                        printf("num : %d , \n",num);


                    }else{
                        printf("other fail \n");
                        return ret;
                    }
                }

            }else if( 0 == ret){
                retry:
                if (avcodec_send_packet(avctx, pkt) == AVERROR(EAGAIN)) {
                    printf("Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
                    //这里可以考虑休眠 0.1 秒，返回 EAGAIN 通常是 ffmpeg 的内部 api 有bug
                    goto retry;
                } else {
                    //释放 pkt 里面的编码数据
                    av_packet_unref(pkt);

                    //循环不断从解码器读数据，直到没有数据可读。
                    for(;;){
                        //读取 AVFrame
                        ret = avcodec_receive_frame(avctx, frame);
                        /* 释放 frame 里面的YUV数据，
                         * 由于 avcodec_receive_frame 函数里面会调用 av_frame_unref，所以下面的代码可以注释。
                         * 所以我们不需要 手动 unref 这个 AVFrame
                         * */
                        //av_frame_unref(frame);

                        if( AVERROR(EAGAIN) == ret ){
                            //提示 EAGAIN 代表 解码器 需要 更多的 AVPacket
                            //跳出 第一层 for，让 解码器拿到更多的 AVPacket
                            break;
                        }else if( AVERROR_EOF == ret ){
                            /* 提示 AVERROR_EOF 代表之前已经往 解码器发送了一个 data 跟 size 都是 NULL 的 AVPacket
                             * 发送 NULL 的 AVPacket 是提示解码器把所有的缓存帧全都刷出来。
                             * 通常只有在 读完输入文件才会发送 NULL 的 AVPacket，或者需要用现有的解码器解码另一个的视频流才会这么干。
                             *
                             * */

                            //跳出 第二层 for，文件已经解码完毕。
                            read_end = 1;
                            break;
                        }else if( ret >= 0 ){

                            //解码出一帧 YUV 数据，打印一些信息。
                            printf("decode success ----\n");
                            printf("width : %d , height : %d \n",frame->width,frame->height);
                            printf("pts : %I64d , duration : %I64d \n",frame->pts,frame->pkt_duration);
                            printf("format : %d \n",frame->format);
                            printf("key_frame : %d \n",frame->key_frame);
                            printf("AVPictureType : %d \n",frame->pict_type);
                            int num = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, 1920, 1080, 1);
                            printf("num : %d , \n",num);

                            //打印 yuv
                            //printf(" Y size : %d \n",frame->linesize[0]);
                            //printf(" U size : %d \n",frame->linesize[1]);
                            //printf(" V size : %d \n",frame->linesize[2]);

                            /*
                            //修改 yuv
                            for(int tt=0; tt < 5 ;tt++){
                                frame->data[0][tt] = 0xFF;
                            }
                            //拼接文件名
                            char yuv_pic_name[200] = "./yuv420p_";
                            char frame_num_str[10];
                            itoa(frame_num, frame_num_str, 10);
                            strcat(yuv_pic_name,frame_num_str);
                            //写入文件
                            FILE *fp = NULL;
                            fp = fopen(yuv_pic_name, "w+");
                            fwrite(frame->data[0] , 1, frame->linesize[0] * frame->height, fp);
                            fwrite(frame->data[1] , 1, frame->linesize[1] * frame->height/2, fp);
                            fwrite(frame->data[2] , 1, frame->linesize[2] * frame->height/2, fp);
                            fclose(fp);

                            frame_num++;
                            if( frame_num > 10 ){
                                return 99;
                            }
                            */

                        }else{
                            printf("other fail \n");
                            return ret;
                        }
                    }
                }

            }
        }

        av_frame_free(&frame);
        av_packet_free(&pkt);

        //关闭解码器。
        avcodec_close(avctx);

    }


    return 0;
}
