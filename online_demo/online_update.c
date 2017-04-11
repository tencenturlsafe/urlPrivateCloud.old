#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "common.h"
#include "urllibhandler.h"

#define UPDATE_FREQ_TIME 60
#define MAX_BUF_LEN 1024
#define URLLIB_SHM_KEY 263785
#define URLLIB_MAX_CNT 150000000
const char * pLogFile = "../log/online_update.log";
static FILE * fpw = NULL;

//写系统日志
void UPDATELOG(const char * fmt,...) {
    if(NULL == fpw){
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
    if(argc != 2){
        LOG("Usage:%s configFile",argv[0]);
        return -1; 
    }


    int iErrCode = 0;
    if (!UrlLibInit(URLLIB_SHM_KEY,URLLIB_MAX_CNT,argv[1], &iErrCode)) {
        LOG("UrlLibInit Failed!\tErrCode:%#x", iErrCode);
        return -3;
    }

    // 该文件所在的磁盘需要保留8G+的空间
    while(true)
    {
        unsigned int uiCur = time(0);
        char szDownLoadFilePath[128] = {0};
        snprintf(szDownLoadFilePath, 128, ".TMP_LOG.DAT.%u", uiCur);
        unsigned int uiInsertDataCnt = 0;
        unsigned int uiNextTime = 0;
        if(!UrlLibUpgrade(szDownLoadFilePath, &uiInsertDataCnt, &iErrCode,&uiNextTime))
        {
            UPDATELOG("UrlLibUpgrade Failed\tErrCode: %#x\n", iErrCode);
            //请求频率过快,建议下次请求时间
            if(iErrCode == 0x14){
                sleep(uiNextTime);
                continue;
            }else{
                sleep(UPDATE_FREQ_TIME);
                continue;
            }
        }
        UPDATELOG("UrlLibUpgrade Success\tLoad Cnt:%u\n",uiInsertDataCnt);
        sleep(UPDATE_FREQ_TIME);
    }
    return 0;
}
