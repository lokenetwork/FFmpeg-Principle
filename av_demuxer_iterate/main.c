#include <stdio.h>
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
int main(){
    AVCodec *encode = avcodec_find_encoder(AV_CODEC_ID_H264);
    AVCodecContext* enc_ctx = avcodec_alloc_context3(encode);
    const AVOption *opt = NULL;

    while (opt = av_opt_next(enc_ctx, opt)) {
        printf("common opt_name is %s \n", opt->name);
    }

    while (opt = av_opt_next(enc_ctx->priv_data, opt)) {
        printf("private opt_name is %s \n", opt->name);
    }
    avcodec_free_context(&enc_ctx);
    return 0;
}
