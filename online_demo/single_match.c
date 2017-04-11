#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "urllibhandler.h"
#include "common.h"

#define MAX_PARA_LEN 128
#define URLLIB_SHM_KEY 263785
#define URLLIB_MAX_CNT 150000000
const char * pConf = "../conf/licence.conf";

int main(int argc, char * argv[])
{
    if(argc != 2){
        LOG("Usage %s url\n",argv[0]);
        return -1;
    }

    int iErrCode = 0;
    if(!UrlLibInit(URLLIB_SHM_KEY,URLLIB_MAX_CNT,pConf,&iErrCode))
    {
        LOG("UrlLibInit Failed\tErrcode:%#x\n",iErrCode);
        return -2;
    }

    unsigned int uiUrlType = 0;
    unsigned int uiEvilType = 0;
    unsigned int uiLevel = 0;
    char szParameter[MAX_PARA_LEN] = {0};
    int iRet = UrlLibDetect(argv[1],&uiUrlType, &uiEvilType, &uiLevel,szParameter);
    if(iRet != 0x00 && iRet != 0x52){
        LOG("url:%s\tUrlLibDetect url failed! iRet:%#x\n",argv[1],iRet);
    } else {
        LOG("url:%s\tuiUrlType:%u\tuiEvilType:%u\tuiLevel:%u\tparameter:%s\tiRet:%#x\n",argv[1],uiUrlType,uiEvilType,uiLevel,szParameter,iRet);
    }
    return 0;
}
