#include <stdio.h>
#include "libavutil/avutil.h"
#include "libavutil/fifo.h"

typedef struct MyData {
    int a;
    int b;
    int c;
} MyData;

int main()
{
    int wirte_space, read_space;

    AVFifoBuffer *list = av_fifo_alloc(sizeof(MyData) * 10);
    read_space = av_fifo_size(list);
    wirte_space = av_fifo_space(list);
    av_log(NULL, AV_LOG_INFO, "read_space: %d,wirte_space: %d \n",read_space,wirte_space);

    av_fifo_grow(list,sizeof(MyData) * 5);
    read_space = av_fifo_size(list);
    wirte_space = av_fifo_space(list);
    av_log(NULL, AV_LOG_INFO, "read_space: %d,wirte_space: %d \n",read_space,wirte_space);

    MyData myData;
    for(int i=0; i<5 ; i++){
        myData.a = i;
        myData.b = i;
        myData.c = i;
        av_fifo_generic_write(list, &myData, sizeof(myData), NULL);
    }
    read_space = av_fifo_size(list);
    wirte_space = av_fifo_space(list);
    av_log(NULL, AV_LOG_INFO, "read_space: %d,wirte_space: %d \n",read_space,wirte_space);


    MyData myData2;
    av_fifo_generic_read(list, &myData2, sizeof(myData2), NULL);
    av_log(NULL, AV_LOG_INFO, "myData2: %d, %d, %d \n",myData2.a,myData2.b,myData2.c);
    av_fifo_generic_read(list, &myData2, sizeof(myData2), NULL);
    av_log(NULL, AV_LOG_INFO, "myData2: %d, %d, %d \n",myData2.a,myData2.b,myData2.c);
    read_space = av_fifo_size(list);
    wirte_space = av_fifo_space(list);
    av_log(NULL, AV_LOG_INFO, "read_space: %d,wirte_space: %d \n",read_space,wirte_space);

    av_fifo_freep(&list);

    return 0;
}
