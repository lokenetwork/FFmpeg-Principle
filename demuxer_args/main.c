#include <stdio.h>
#include "libavformat/avformat.h"
int main()
{

    AVFormatContext *fmt_ctx = NULL;
    int err;
    char filename[] = "juren-30s.mp4";

    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
      av_log(NULL, AV_LOG_ERROR, "error code %d \n",AVERROR(ENOMEM));
      return 1;
    }

    AVDictionary* format_opts = NULL;
    AVDictionaryEntry *t;
    av_dict_set(&format_opts, "formatprobesize", "10485760", AV_DICT_MATCH_CASE);
    av_dict_set(&format_opts, "export_all", "1", AV_DICT_MATCH_CASE);
    av_dict_set(&format_opts, "export_666", "1", AV_DICT_MATCH_CASE);

    //获取字典里的第一个属性。
    if ((t = av_dict_get(format_opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
          av_log(NULL, AV_LOG_INFO, "Option key: %s , value %s \n", t->key, t->value);
    }
    if((err = avformat_open_input(&fmt_ctx, filename,NULL,&format_opts)) < 0){
        av_log(NULL, AV_LOG_ERROR, "error code %d \n",err);
    }else{
        av_log(NULL, AV_LOG_INFO, "open success \n");
        av_log(NULL, AV_LOG_INFO, "duration: %I64d \n",fmt_ctx->duration);
   }
    //再次，获取字典里的第一个属性。
    if ((t = av_dict_get(format_opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
          av_log(NULL, AV_LOG_INFO, "Option key: %s , value %s \n", t->key, t->value);
    }

   return 0;
}
