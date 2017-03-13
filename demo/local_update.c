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
 
    while(1)
    {
        unsigned int uiInsertDataCnt = 0;
        if(UrlLibLoad("./load_dir",&uiInsertDataCnt,&iErrCode,false))
        {
            printf("UrlLibLoad Success, Load Cnt : %u \n", uiInsertDataCnt);
        }else {
            printf("UrlLibLoad falied, ErrCode: %#x \n",iErrCode);
        }
        sleep(UPDATE_TIME);
    }
    return 0;
}
