#include <stdio.h>
#include "libavutil/avutil.h"
#include "libavutil/avstring.h"

int main()
{
    int ret = 0;
    char* src_str = av_strdup("FFmpeg-Principle");
    char* dst_str = av_malloc(1024);
    const char* ptr = NULL;
    ret = av_strstart(src_str,"FFmpeg",&ptr);
    printf("ret is %d , ptr is %s \n",ret,ptr);

    ret = av_stristart(src_str,"ffmpeg",&ptr);
    printf("ret is %d , ptr is %s \n",ret,ptr);

    ptr = av_strnstr(src_str,"Princ",20);
    printf("ptr is %s \n",ptr);

    ret = av_strlcpy(dst_str,src_str,8);
    printf("ret is %d , dst_str is %s \n",ret,dst_str);

    ret = av_strlcat(dst_str,src_str,100);
    printf("ret is %d , dst_str is %s \n",ret,dst_str);

    ret = av_strlcatf(dst_str,512," is %d",666);
    printf("ret is %d , dst_str is %s \n",ret,dst_str);

    ret = av_strnlen(dst_str,1024);
    printf("ret is %d , dst_str is %s \n",ret,dst_str);

    const char* dst_str_2 =  av_asprintf("FFmpeg-Pinciple %s","book");
    printf("dst_str_2 is %s \n",dst_str_2);

    /*
    const char *p2 = av_get_token(&dst_str_2, "-");
    */

    char* token_str;
    char* saveptr;
    while( (token_str = av_strtok(src_str,"-",&saveptr)) ) {
        //第二次调用的时候， src_str 必须为 NULL
        src_str = NULL;
        printf("ret is %d ,token_str is %s \n",ret,token_str);
    }

    char digit_1 = '2';
    ret = av_isdigit(digit_1);
    printf("ret is %d \n", ret);
    char digit_2 = 'r';
    ret = av_isdigit(digit_2);
    printf("ret is %d \n", ret);


    //清空一下 dst_str.
    // memset(dst_str,0,1024);


    av_freep(&src_str);
    av_freep(&dst_str);
    av_freep(&dst_str_2);
    return 0;
}
