#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "urllibhandler.h"
#include "common.h"

#define MAX_PARA_LEN 128
#define MAX_BUF_LEN 1024
#define URLLIB_SHM_KEY 263700
#define URLLIB_MAX_CNT 150000000
const char * pConfFile = "../conf/licence.conf";

char * left_trim(char * str) {
    if (str == NULL)
        return str;
    for (; *str != '\0' && isspace(*str); ++str);
    return str;
}

char * both_trim(char * str) {
    if (str == NULL)
        return str;
    char * szOutput = left_trim(str);
    char * ptr;
    for (ptr = szOutput + strlen(szOutput) - 1; ptr >= szOutput && isspace(*ptr); --ptr);
    *(++ptr) = '\0';
    return szOutput;
}

int main(int argc, char * argv[])
{
    if(argc != 3){
        LOG("Usage %s urlFile saveFile\n",argv[0]);
        return -1;
    }

    FILE * fpr = fopen(argv[1], "rb");
    if(fpr == NULL){
        LOG("fopen %s failed!\n",argv[1]);
        return -2;
    }

    FILE * fpw = fopen(argv[2],"wb");
    if(fpw == NULL){
        LOG("fopen %s failed!\n",argv[2]);
        return -3;
    }

    int iErrCode = 0;
    if(!UrlLibInit(URLLIB_SHM_KEY,URLLIB_MAX_CNT,pConfFile,&iErrCode))
    {
        LOG("UrlLibInit Failed\tErrcode:%#x\n",iErrCode);
        return -4;
    }

    char buf[MAX_BUF_LEN];
    while (!feof(fpr)) {
        memset(buf,0,sizeof(buf));
        if (fgets(buf, sizeof (buf), fpr) != NULL) {
            char ch = left_trim(buf)[0];
            if (isblank(ch) || ch == '\n')
                continue;
            char * ptrUrl = both_trim(buf);
            unsigned int uiUrlType = 0;
            unsigned int uiEvilType = 0;
            unsigned int uiLevel = 0;
            char szParameter[MAX_PARA_LEN] = {0};
            char resultData[2048] = {0};
            int iRet = UrlLibDetect(ptrUrl,&uiUrlType, &uiEvilType, &uiLevel,szParameter);
            if(iRet != 0x00 && iRet != 0x52){
                snprintf(resultData,sizeof(resultData),"%s\tUrlLibDetect url failed! iRet:%#x\n",ptrUrl,iRet);
                fputs(resultData,fpw);
            } else {
                snprintf(resultData,sizeof(resultData),"%s\tUrlType: %d\tEvilType: %d\tiLevel: %d\n",ptrUrl,uiUrlType, uiEvilType, uiLevel);
                fputs(resultData,fpw);
            }
        }
    }

    return 0;
}
