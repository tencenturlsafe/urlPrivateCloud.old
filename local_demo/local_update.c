#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "common.h"
#include "urllibhandler.h"

#define UPDATE_FREQ_TIME 300
#define URLLIB_SHM_KEY 263700
#define URLLIB_MAX_CNT 150000000
const char * pDataDir = "../data/";
const char * pLogFile = "../log/local_update.log";
#define MAX_BUF_LEN 512
static FILE * fpw = NULL;

//写日志
void UPDATELOG(const char * fmt,...) {
    if (NULL == fpw) {
        fpw = fopen(pLogFile,"a+");
        if(NULL == fpw)
            return;
    }   

    time_t curTime = time(NULL);
    struct tm stTm = *localtime(&curTime);
    char szLogInfo[MAX_BUF_LEN];
    strftime(szLogInfo, MAX_BUF_LEN, "%Y%m%d %H:%M:%S", &stTm);
    strcat(szLogInfo,"\t");

    va_list ap; 
    va_start(ap,fmt);
    vsnprintf(szLogInfo + strlen(szLogInfo), MAX_BUF_LEN - 2, fmt, ap);
    va_end(ap);
    strcat(szLogInfo,"\n");

    fputs(szLogInfo, fpw);
    fflush(fpw);
} 

int main(int argc, char * argv[])
{
    if (argc != 2) {
        LOG("Usage:%s configFile",argv[0]);
        return -1;
    }

    int iErrCode = 0;
    if (!UrlLibInit(URLLIB_SHM_KEY, URLLIB_MAX_CNT, argv[1], &iErrCode)) {
        LOG("UrlLibInit Failed\tErrCode:%#x", iErrCode);
        return -3;
    }


    while(true)
    {
        int iErrCode = 0;
        unsigned int uiInsertDataCnt = 0;
        if(UrlLibLoad(pDataDir,&uiInsertDataCnt,&iErrCode,false))
            UPDATELOG("UrlLibLoad Success\tLoad Cnt : %u", uiInsertDataCnt);
        else 
            UPDATELOG("UrlLibLoad falied\tErrCode: %#x",iErrCode);
        sleep(UPDATE_FREQ_TIME);
    }

    fclose(fpw);
    return 0;
}
