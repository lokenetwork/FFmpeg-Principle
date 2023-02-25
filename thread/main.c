#include <stdio.h>
#include "libavutil/thread.h"

int sum = 0;
//互斥锁
AVMutex add_mutex;
//add_end 等于1 就结束加法任务
int add_end = 0;

void* add_num(void *argv){
    for(;;){
        if( add_end )
            break;
        ff_mutex_lock(&add_mutex);
        sum += 1;
        ff_mutex_unlock(&add_mutex);
    }
};

int main()
{
    int ret, i, j;
    pthread_t thread_add_1, thread_add_2;
    //初始化互斥锁
    ff_mutex_init(&add_mutex,NULL);

    if ((ret = pthread_create(&thread_add_1, NULL, add_num, NULL))) {
        av_log(NULL, AV_LOG_ERROR, "pthread_create failed.\n");
        return AVERROR(ret);
    }
    if ((ret = pthread_create(&thread_add_2, NULL, add_num, NULL))) {
        av_log(NULL, AV_LOG_ERROR, "pthread_create failed.\n");
        return AVERROR(ret);
    }

    //1 秒后结束加法
    av_usleep(1000 * 1000 * 2);
    add_end = 1;
    pthread_join(thread_add_1, NULL);
    pthread_join(thread_add_2, NULL);
    av_log(NULL, AV_LOG_INFO, "sum is %d.\n",sum);

    //销毁互斥锁
    ff_mutex_destroy(&add_mutex);

    /*----线程间传递数据的方式如下----*/
    //采用 av_thread_message_queue_send，av_thread_message_queue_recv 函数即可，后面再补充。

    return 0;
}
