#include "urllibhandler.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define UPDATE_TIME 60
#define URLLIB_SHM_KEY 263700
#define URLLIB_MAX_CNT 120000000
const char * pConfFile = "./licence.conf";


int main()
{
    //读取配置文件相关信息,进行初始化
    int iErrCode = 0;
    if(!UrlLibInit(URLLIB_SHM_KEY,URLLIB_MAX_CNT,pConfFile,&iErrCode))
    {
    	printf("UrlLibInit Failed. ErrCode:%#x\n",iErrCode);
        return -1;
    }
 
    // 该文件所在的磁盘需要保留8G+的空间
    unsigned int uiCur = time(0);
    char szDownLoadFilePath[128] = {0};
    snprintf(szDownLoadFilePath, 128, ".TMP_LOG.DAT.%u", uiCur);
    unsigned int uiInsertDataCnt = 0;
    unsigned int uiNextTime = 0;
    while(1)
    {
        if(!UrlLibUpgrade(szDownLoadFilePath, &uiInsertDataCnt, &iErrCode,&uiNextTime))
        {
            printf("UrlLibUpgrade Failed, ErrCode: %#x\n", iErrCode);
            //请求频率过快,建议下次请求时间
            if(iErrCode == 0){
                sleep(uiNextTime);
                continue;
            }else{
                sleep(UPDATE_TIME);
                continue;
            }
        }
        printf("UrlLibUpgrade Success, Load Cnt:%u, iErrCode: %#x\n",uiInsertDataCnt,iErrCode);
        sleep(UPDATE_TIME);
   }
   return 0;
}
