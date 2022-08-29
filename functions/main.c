#include <stdio.h>
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"

int main()
{
    char* name;
    int type = 1;
    if( 2 == type ){
        name = malloc(100);
        strcpy(name,"loken-ffmpeg");
        printf("data : %s \n",name);
        free(name);
    }else{
        name = av_strdup("loken-ffmpeg");
        printf("data : %s \n",name);
        av_freep(&name);
    }

    char* err_str = av_err2str(AVERROR_EOF);
    printf("data : %s \n",err_str);

    return 0;
}
