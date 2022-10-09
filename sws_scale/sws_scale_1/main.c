#include <stdio.h>
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/bprint.h"
#include "libavutil/pixdesc.h"

int save_rgb_to_file(AVFrame *frame, int num);

int main()
{
    int ret = 0; int err;
    int read_end = 0;
    int frame_num = 0;

    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    //打开输入文件
    char filename[] = "juren-30s.mp4";
    struct SwsContext* img_convert_ctx = NULL;
    AVFormatContext *fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
        printf("error code %d \n",AVERROR(ENOMEM));
        return ENOMEM;
    }
    if((err = avformat_open_input(&fmt_ctx, filename,NULL,NULL)) < 0){
        printf("can not open file %d \n",err);
        return err;
    }

    //打开解码器
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

    int sws_flags = SWS_BICUBIC;
    AVFrame* result_frame = av_frame_alloc();
    //定义 AVFrame 的格式，宽高。
    result_frame->format = AV_PIX_FMT_BGRA;
    result_frame->width = 200;
    result_frame->height = 100;

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
            //读取完文件，这时候 pkt 的 data 跟 size 应该是 null
            avcodec_send_packet(avctx, NULL);
        }else {
            if( 0 != ret){
                printf("read error code %d \n",ret);
                return ENOMEM;
            }else{
                retry:
                if (avcodec_send_packet(avctx, pkt) == AVERROR(EAGAIN)) {
                    printf("Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
                    //这里可以考虑休眠 0.1 秒，返回 EAGAIN 通常是 ffmpeg 的内部 api 有bug
                    goto retry;
                }
                //释放 pkt 里面的编码数据
                av_packet_unref(pkt);
            }

        }

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
                 * */

                //跳出 第二层 for，文件已经解码完毕。
                read_end = 1;
                break;
            }else if( ret >= 0 ){
                if( NULL == img_convert_ctx ){
                    img_convert_ctx = sws_getCachedContext(img_convert_ctx,
                                                           frame->width, frame->height, frame->format,
                                                           result_frame->width, result_frame->height, result_frame->format,
                                                           sws_flags, NULL, NULL, NULL);
                    if (NULL == img_convert_ctx) {
                        av_log(NULL, AV_LOG_FATAL, "no memory 1\n");
                        return ENOMEM;
                    }
                }

                //分配 frame 里面的 buffer 内存。
                ret = av_frame_get_buffer(result_frame, 1);
                if( 0 != ret ){
                    av_log(NULL, AV_LOG_FATAL, "no memory 2\n");
                    return ENOMEM;
                }

                sws_scale(img_convert_ctx,
                          (const uint8_t * const *)frame->data, frame->linesize, 0, frame->height,
                          result_frame->data, result_frame->linesize);

                save_rgb_to_file(result_frame,frame_num);

                //减少引用，释放内存。
                av_frame_unref(frame);
                av_frame_unref(result_frame);

                frame_num++;
                //只处理 10 张图片
                if( frame_num > 10 ){
                    return 666;
                }
            }else{
                av_log(NULL, AV_LOG_FATAL, "other fail \n");
                return ret;
            }
        }
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);

    sws_freeContext(img_convert_ctx);
    img_convert_ctx = NULL;

    //关闭编码器，解码器。
    avcodec_close(avctx);

    //释放容器内存。
    avformat_free_context(fmt_ctx);
    printf("done \n");
    return 0;
}

int save_rgb_to_file(AVFrame *frame, int num){
    //拼接文件名
    char pic_name[200] = {0};
    sprintf(pic_name,"./rgba_8888_%d.yuv",num);

    //写入文件
    FILE *fp = NULL;
    fp = fopen(pic_name, "wb+");
    fwrite(frame->data[0] , 1, frame->linesize[0] * frame->height, fp);
    fclose(fp);
    return 0;
}



