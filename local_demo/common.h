#ifndef _COMMON_H_
#define _COMMON_H_

#define LOG(format,...) \
    do{\
        printf("[ %s/%s,%d ]\t" format "\n",\
            __FILE__,__func__,__LINE__,##__VA_ARGS__);\
    }while(0)

#endif //_COMMON_H_
