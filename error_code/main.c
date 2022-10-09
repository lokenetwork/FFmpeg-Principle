#include <stdio.h>
#include "libavutil/avutil.h"

int main()
{
    printf("AVERROR_BUG  is 0x%X \n",AVERROR_BUG);
    printf("AVERROR_EXIT is 0x%X \n",AVERROR_EXIT);
    printf("AVERROR_EOF  is 0x%X \n",AVERROR_EOF);

    printf("AVERROR_EOF  is %s \n",av_err2str(AVERROR_EOF));

    char str[AV_ERROR_MAX_STRING_SIZE] = {0};
    av_strerror(AVERROR_BUG,str,AV_ERROR_MAX_STRING_SIZE);
    printf("AVERROR_EOF  is %s \n",str);

    return 0;
}
