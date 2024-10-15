#include <stdio.h>
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
int main()
{

    AVFormatContext *fmt_ctx = NULL;
    int type = 1;
    int ret = 0;
    int frame_num = 0;
    int read_end = 0;
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();


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

    //本文只初始化 音频解码器。视频解码器不管
    AVCodecContext *avctx = avcodec_alloc_context3(NULL);
    //把容器记录的编码参数赋值给 编码器上下文。
    ret = avcodec_parameters_to_context(avctx, fmt_ctx->streams[1]->codecpar);
    if (ret < 0){
        printf("error code %d \n",ret);
        return ret;
    }

    AVCodec *codec = avcodec_find_decoder(avctx->codec_id);
    if ((ret = avcodec_open2(avctx, codec, NULL)) < 0) {
        printf("open codec faile %d \n",ret);
        return ret;
    }

    const char* format_name = NULL;
    struct SwrContext *swr_ctx = NULL;
    uint8_t *out = NULL;
    int out_count;
    int out_size;
    int out_nb_samples;
    int tgt_fmt = AV_SAMPLE_FMT_S64;
    int tgt_freq = 44100;
    int tgt_channels;

    for(;;){
        if( 1 == read_end ){
            break;
        }

        ret = av_read_frame(fmt_ctx, pkt);
        //跳过不处理音频包
        if( 0 == pkt->stream_index ){
            av_packet_unref(pkt);
            continue;
        }
        if ( AVERROR_EOF == ret) {
            //读取完文件，这时候 pkt 的 data 跟 size 应该是 null
            avcodec_send_packet(avctx, pkt);
        }else if( 0 == ret){
retry:
            if (avcodec_send_packet(avctx, pkt) == AVERROR(EAGAIN)) {
                printf("Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
                //这里可以考虑休眠 0.1 秒，返回 EAGAIN 通常是 ffmpeg 的内部 api 有bug
                goto retry;
            } else {
                //释放 pkt 里面的内存数据
                av_packet_unref(pkt);

                //循环不断从解码器读数据，直到没有数据可读。
                for(;;){
                    //读取 AVFrame
                    ret = avcodec_receive_frame(avctx, frame);
                    /* 释放 frame 里面的YUV数据，
                     * 由于 avcodec_receive_frame 函数里面会调用 av_frame_unref，所以下面的代码可以注释。
                     * 所以我们不需要 手动 unref 这个 AVFrame，当然你 unref 多一次也不会出错。
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
                        //解码出一帧 音频PCM 数据，打印一些信息。
                        if( frame_num == 0){
                            format_name = av_get_sample_fmt_name(frame->format);
                            printf("origin sample_format:%s, sample_rate:%d .\n",format_name,frame->sample_rate);
                            format_name = av_get_sample_fmt_name(tgt_fmt);
                            printf("target sample_format:%s, sample_rate:%d .\n",format_name,tgt_freq);
                        }

                        if( !swr_ctx ){
                            swr_ctx = swr_alloc_set_opts(NULL,
                                                         frame->channel_layout, tgt_fmt, tgt_freq,
                                                         frame->channel_layout, frame->format, frame->sample_rate,
                                                         0, NULL);
                            if (!swr_ctx || swr_init(swr_ctx) < 0) {
                                av_log(NULL, AV_LOG_ERROR, "Cannot create sample rate converter \n");
                                swr_free(&swr_ctx);
                                return -1;
                            }
                        }

                        //不改变声道布局
                        tgt_channels = frame->channels;
                        out_count = (int64_t)frame->nb_samples * tgt_freq / frame->sample_rate + 256;
                        out_size  = av_samples_get_buffer_size(NULL, tgt_channels, out_count, tgt_fmt, 1);
                        out = av_malloc(out_size);
                        //注意，因为 音频可能有超过 9 声道的数据，所以要用 extended_data;
                        const uint8_t **in = (const uint8_t **)frame->extended_data;

                        out_nb_samples = swr_convert(swr_ctx, &out, out_count, in, frame->nb_samples);
                        if( out_nb_samples < 0 ){
                            av_log(NULL, AV_LOG_ERROR, "converte fail \n");
                        }else{
                            printf("out_count:%d, out_nb_samples:%d, frame->nb_samples:%d \n", out_count, out_nb_samples,frame->nb_samples);
                        }

                        //可以把 out 的内存直接丢给播放器播放。
                        av_freep(&out);

                        frame_num++;
                        if( frame_num > 10 ){
                            out = av_malloc(out_size);
                            do {
                                //把剩下的样本数都刷出来。
                                out_nb_samples = swr_convert(swr_ctx, &out, out_count, NULL, 0);
                                printf("flush out_count:%d, out_nb_samples:%d  \n", out_count, out_nb_samples);
                            }
                            while(out_nb_samples);
                            av_freep(&out);

                            return 99;
                        }
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

    return 0;
}
