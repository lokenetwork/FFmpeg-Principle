#include <stdio.h>
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libavutil/bprint.h"

int save_yuv_to_file(AVFrame *frame, int num);

AVFrame* get_frame_from_jpeg_or_png_file(const char *filename,AVRational *logo_tb,AVRational *logo_fr);

int main()
{

    int ret = 0; int err;

    //打开输入文件
    char filename[] = "juren-30s.mp4";
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

    AVFilterGraph *filter_graph = NULL;
    AVFilterContext *mainsrc_ctx,*logo_ctx,*resultsink_ctx = NULL;


    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    AVFrame *result_frame = av_frame_alloc();
    AVRational logo_tb = {0};
    AVRational logo_fr = {0};
    AVFrame *logo_frame = get_frame_from_jpeg_or_png_file("logo.jpg",&logo_tb,&logo_fr);
    if( NULL == logo_frame ){
        printf("logo.jpg not exist\n");
        return 888;
    }

    int64_t logo_next_pts = 0,main_next_pts = 0;

    int read_end = 0;
    int frame_num = 0;
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
                 *
                 * */

                //跳出 第二层 for，文件已经解码完毕。
                read_end = 1;
                break;
            }else if( ret >= 0 ){
                //这两个变量在本文里没有用的，只是要传进去。
                AVFilterInOut *inputs, *cur, *outputs;

                if( NULL == filter_graph ){
                    //初始化滤镜容器
                    filter_graph = avfilter_graph_alloc();
                    if (!filter_graph) {
                        printf("Error: allocate filter graph failed\n");
                        return -1;
                    }

                    // 因为 filter 的输入是 AVFrame ，所以 filter 的时间基就是 AVFrame 的时间基
                    AVRational tb = fmt_ctx->streams[0]->time_base;
                    AVRational fr = av_guess_frame_rate(fmt_ctx, fmt_ctx->streams[0], NULL);
                    AVRational logo_sar = logo_frame->sample_aspect_ratio;
                    AVRational sar = frame->sample_aspect_ratio;
                    AVBPrint args;
                    av_bprint_init(&args, 0, AV_BPRINT_SIZE_AUTOMATIC);
                    av_bprintf(&args,
                               "buffer=video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d:frame_rate=%d/%d[main];"
                               "buffer=video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d:frame_rate=%d/%d[logo];"
                               "[main][logo]overlay=x=10:y=10[result];"
                               "[result]format=yuv420p[result_2];"
                               "[result_2]buffersink",
                               frame->width, frame->height, frame->format, tb.num,tb.den,sar.num, sar.den,fr.num, fr.den,
                               logo_frame->width, logo_frame->height, logo_frame->format, logo_tb.num,logo_tb.den ,
                               logo_sar.num,logo_sar.den, logo_fr.num, logo_fr.den);


                    ret = avfilter_graph_parse2(filter_graph, args.str, &inputs, &outputs);
                    if (ret < 0) {
                        printf("Cannot configure graph\n");
                        return ret;
                    }

                    //正式打开滤镜
                    ret = avfilter_graph_config(filter_graph, NULL);
                    if (ret < 0) {
                        printf("Cannot configure graph\n");
                        return ret;
                    }

                    //根据 名字 找到 AVFilterContext
                    mainsrc_ctx = avfilter_graph_get_filter(filter_graph, "Parsed_buffer_0");
                    logo_ctx = avfilter_graph_get_filter(filter_graph, "Parsed_buffer_1");
                    resultsink_ctx = avfilter_graph_get_filter(filter_graph, "Parsed_buffersink_4");

                    ret = av_buffersrc_add_frame_flags(logo_ctx, logo_frame,AV_BUFFERSRC_FLAG_PUSH);
                    if(ret < 0){
                        printf("Error: av_buffersrc_add_frame failed\n");
                        return ret;
                    }
                    //因为 logo 只有一帧，发完，就需要关闭 buffer 滤镜
                    logo_next_pts = logo_frame->pts + logo_frame->pkt_duration;
                    ret = av_buffersrc_close(logo_ctx, logo_next_pts, AV_BUFFERSRC_FLAG_PUSH);

                    /* 发 null 也可以，但是不太正规。可能某些场景下会有问题
                    ret = av_buffersrc_add_frame(logo_ctx, NULL);
                    if(ret < 0){
                        printf("Error: av_buffersrc_add_frame failed\n");
                        return ret;
                    }*/

                }

                if( frame_num <= 10 ){
                    ret = av_buffersrc_add_frame_flags(mainsrc_ctx, frame,AV_BUFFERSRC_FLAG_PUSH);
                    if(ret < 0){
                        printf("Error: av_buffersrc_add_frame failed\n");
                        return ret;
                    }
                }
                ret = av_buffersink_get_frame_flags(resultsink_ctx, result_frame,AV_BUFFERSINK_FLAG_NO_REQUEST);
                if( ret == AVERROR_EOF ){
                    //没有更多的 AVFrame
                    printf("no more avframe output \n");
                }else if( ret == AVERROR(EAGAIN) ){
                    //需要输入更多的 AVFrame
                    printf("need more avframe input \n");
                }else if( ret >= 0 ){
                    //保存进去文件。
                    printf("save_yuv_to_file success %d \n",frame_num);
                    save_yuv_to_file(result_frame,frame_num);
                }

                logo_next_pts = frame->pts + frame->pkt_duration;
                frame_num++;
                //只保存 10 张图片
                if( frame_num > 10 ){
                    ret = av_buffersrc_close(mainsrc_ctx, logo_next_pts, AV_BUFFERSRC_FLAG_PUSH);
                }

            }else{
                printf("other fail \n");
                return ret;
            }
        }


    }

    av_frame_free(&frame);
    av_frame_free(&logo_frame);
    av_frame_free(&result_frame);
    av_packet_free(&pkt);

    //关闭编码器，解码器。
    avcodec_close(avctx);

    //关闭输入文件
    avformat_close_input(&fmt_ctx);
    printf("done \n");

    //释放滤镜。
    avfilter_graph_free(&filter_graph);

    return 0;
}

int save_yuv_to_file(AVFrame *frame, int num){
    //拼接文件名
    AVBPrint f_name;
    av_bprint_init(&f_name, 0, AV_BPRINT_SIZE_AUTOMATIC);
    av_bprintf(&f_name,"./yuv420p_%d.yuv",num);

    //写入文件
    FILE *fp = NULL;
    fopen_s(&fp, f_name.str, "wb+");
    fwrite(frame->data[0] , 1, frame->linesize[0] * frame->height, fp);
    fwrite(frame->data[1] , 1, frame->linesize[1] * frame->height/2, fp);
    fwrite(frame->data[2] , 1, frame->linesize[2] * frame->height/2, fp);
    fclose(fp);
    return 0;
}


AVFrame* get_frame_from_jpeg_or_png_file(const char *filename,AVRational *logo_tb,AVRational *logo_fr){
    int ret = 0;

    AVDictionary *format_opts = NULL;
    av_dict_set(&format_opts, "probesize", "5000000", 0);
    AVFormatContext *format_ctx = NULL;
    if ((ret = avformat_open_input(&format_ctx, filename, NULL, &format_opts)) != 0) {
        printf("Error: avformat_open_input failed \n");
        return NULL;
    }
    avformat_find_stream_info(format_ctx,NULL);

    *logo_tb = format_ctx->streams[0]->time_base;
    *logo_fr = av_guess_frame_rate(format_ctx, format_ctx->streams[0], NULL);

    //打开解码器
    AVCodecContext *av_codec_ctx = avcodec_alloc_context3(NULL);
    ret = avcodec_parameters_to_context(av_codec_ctx, format_ctx->streams[0]->codecpar);
    if (ret < 0){
        printf("error code %d \n",ret);
        avformat_close_input(&format_ctx);
        return NULL;
    }

    AVDictionary *codec_opts = NULL;
    av_dict_set(&codec_opts, "sub_text_format", "ass", 0);
    AVCodec *codec = avcodec_find_decoder(av_codec_ctx->codec_id);
    if(!codec){
        printf("codec not support %d \n",ret);
        return NULL;
    }
    if ((ret = avcodec_open2(av_codec_ctx, codec, NULL)) < 0) {
        printf("open codec faile %d \n",ret);
        return NULL;
    }

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    ret = av_read_frame(format_ctx, &pkt);
    ret = avcodec_send_packet(av_codec_ctx, &pkt);
    AVFrame *frame_2 = av_frame_alloc();
    ret = avcodec_receive_frame(av_codec_ctx, frame_2);

    //省略错误处理

    av_dict_free(&format_opts);
    av_dict_free(&codec_opts);

    return frame_2;
}

