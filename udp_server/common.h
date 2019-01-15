#ifndef _COMMON_H_
#define _COMMON_H_

#define LOG(format,...) \
    do{\
        fprintf(stderr,"[ %s/%s,%d ]\t" format "\n",\
            __FILE__,__func__,__LINE__,##__VA_ARGS__);\
    }while(0)

#define SETWORKTHREAD(threadNum) \
    do{\
        char szThreadNum[MAX_THREAD_NUM];\
        snprintf(szThreadNum,MAX_THREAD_NUM,"%d",(unsigned int)threadNum);\
        setenv("UV_THREADPOOL_SIZE",szThreadNum,1);\
    }while(0)

#endif //_COMMON_H_
