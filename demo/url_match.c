#include "urllibhandler.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#define MAX_PARA_LEN 128
#define URLLIB_SHM_KEY 263700
#define URLLIB_MAX_CNT 120000000
const char * pConfFile = "./licence.conf";

int main(int argc, char * argv[])
{
    if(argc != 2){
        printf("Usage %s url\n",argv[0]);
        return -1;
    }

    int iErrCode = 0;
    if(!UrlLibInit(URLLIB_SHM_KEY,URLLIB_MAX_CNT,pConfFile,&iErrCode))
    {
        printf("UrlLibInit Failed. Errcode:%#x\n",iErrCode);
        return -2;
    }
    
    unsigned int uiUrlType = 0;
    unsigned int uiEvilType = 0;
    unsigned int uiLevel = 0;
    char szParameter[MAX_PARA_LEN] = {0};
    int iRet = UrlLibDetect(argv[1],&uiUrlType, &uiEvilType, &uiLevel,szParameter);
    if(iRet != 0x00 && iRet != 0x52){
        printf("url:%s\tUrlLibDetect url failed! iRet:%#x\n",argv[1],iRet);
    } else {
        printf("url:%s\tuiUrlType:%u\tuiEvilType:%u\tuiLevel:%u\tiRet:%#x\n",argv[1],uiUrlType,uiEvilType,uiLevel,iRet);
    }
    return 0;
}
