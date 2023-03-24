#include <stdio.h>
#include "libavutil/avutil.h"
int main()
{
    AVRational new_tb = {0,0};
   unsigned int pts_num = 10;
   unsigned int pts_den = 100;

    if (av_reduce(&new_tb.num, &new_tb.den, pts_num, pts_den, INT_MAX)) {
            if (new_tb.num != pts_num){
                av_log(NULL, AV_LOG_DEBUG,
                       "st:0 removing common factor %d from timebase\n",
                       pts_num / new_tb.num);
            }

         else {
           av_log(NULL, AV_LOG_WARNING, "st:0 has too large timebase, reducing\n");
        }
    }

    return 0;
}
