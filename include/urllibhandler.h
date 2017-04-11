#ifndef _URL_LIB_HANDLER_H
#define _URL_LIB_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    URL_TYPE_GRAY = 1,
    URL_TYPE_BLACK = 2,
    URL_TYPE_WHITE_NO_QQ = 3,
    URL_TYPE_WHITE_QQ = 4
} E_URL_TYPE;

typedef enum
{
    URL_LEVEL_URL = 1,   // mail.qq.com/cgi-bin/frame_html?sid=Uch_hMabTYPCAT1q&r=354da9218e017a8b87a272529853a507
    URL_LEVEL_CGI = 2,   // mail.qq.com/cgi-bin/frame_html
    URL_LEVEL_PATH = 3,  // mail.qq.com/cgi-bin
    URL_LEVEL_HOST = 4,  // mail.qq.com
    URL_LEVEL_DOMAIN = 5 // qq.com
} E_URL_LEVEL;


 /**
  * @brief Init UrlLib by Number of Records
  * @details
  *
  * @param uiShmKey shared memory's key to identify url lib
  * @param uiHashTableMaxCnt urllib's cnt (minimum count is 20*500)
  * @param szConfFile licence conf name
  * @param iErrCode error code if init failed
  * 
  * @return true: successfully init. false: fail init.
  */
bool UrlLibInit(uint32_t uiShmKey, uint32_t uiHashTableMaxCnt,const char * szConfFile,int *iErrCode);

 /**
  * @brief Init UrlLib by Memory size
  * @details
  *
  * @param uiShmKey shared memory's key to identify url lib
  * @param uiMbSize shared memory size (unit:MB minimum:1)
  * @param szConfFile licence conf name
  * @param iErrCode error code if init failed
  *
  * @return true: successfully init. false: fail init.
  */
bool UrlLibInitByMb(uint32_t uiShmKey, uint32_t uiMbSize,const char * szConfFile,int *iErrCode);

 /**
  * @brief Upgrade Url lib once.
  * @details communicate with server, do one upgrade process
  *
  * @param szTmpFilePath: Temp file name with path to store during update process, file will be removed after use.
  * @param uiInsertDataCnt : Data count inserted during the update process
  * @param iErrorCode:  if UrlLibUpgrade fail, the code is for tencent to find the reason
  * @param uiNextTime: if iErrCode equal 0,it should be sleep uiNextTime and then upgrade
  * 
  * @return true: successfully upgrade. false: fail upgrade.
  */
bool UrlLibUpgrade(const char * szTmpFilePath,unsigned int *uiInsertDataCnt,int *iErrCode,unsigned int * uiNextTime);

 /**
  * @brief  Check Url's Status with Eviltype
  * @details
  *
  * @param szUrl url for check
  * @param uiUrlType: urltype of url, refer E_URL_TYPE above
  * @param uiEviltype: Url's eviltype when url is black, refer docs
  * @param uiLevel: Url's evil level when url is black, Refer E_URL_LEVEL above
  * @param szParameter: if black url take this parameter, will return unknown
  * 
  * @return 0x0a：licence.conf expired
  *          0x51: the library is not updated more than seven days
  *          0x52: the url status is unknown
  *          0x00: detect successful    
  */
int UrlLibDetect(const char * szUrl,unsigned int * uiUrlType,unsigned int * uiEvilType, unsigned int * uiLevel,char * szParameter);


 /**
  * @brief Clear whole UrlLib
  * @details All data (both tencent's data and user define data)
  *         will be cleared when this api is invoked
  */
void UrlLibClear();

 /**
  * @brief Used to load offline file (dowbloaded from join.urlsec.qq.com) into Urllib
  * @details
  *
  * @param szFileName File's Location to be load
  * @param uiLoadCnt Sucessed Load Count of the File
  * @param iErrCode Error Code if UrlLibLoad failed
  * @param bClearBeforeLoad Flag of data reset
  *
  * @return true: successfully load. false: fail load.
  */
bool UrlLibLoad(const char * szFileDir,unsigned int *uiLoadCnt,int *iErrCode ,bool bClearBeforeLoad);

 /**
  * @brief Conversion EvilType to EvilClass
  * @details
  *
  * @param evilType value
  * @param correspond evilClass value
  *
  * @return 0x00: successfully conversion.
  *          0x61: error uiEvilType.
  *          0x62: not load conv file.
  */
int convType2Class(unsigned int uiEvilType,unsigned int * uiEvilClass);

/**
 * @brief Show UrlLib memory use
 * @details
 * @return percentage of memory used for diagnose purpose
 */
int UrlLibShowUse();

 /**
  * @brief Insert User Define Elements into UrlLib
  * @details Elements will be formated in tencent manners and inserted intto tencent url lib.
  *          User-defined data will be stored in hashtable same as tencent’s
  *          User-defined data will be deleted when UrlLibDelete executed.
  *          User-defined data will only be matched with UrlLibDetectUserDefine API
  *
  * @param szUrl url to be inserted
  * @param int uiLevel: Ref E_URL_LEVEL above
  * @param int uiUrlType: Ref E_URL_TYPE above
  * 
  * @return  0x00: insert success
  *         0x31: bad szUrl
  *         0x32: bad uiLevel
  *         0x33: bad uiUrlType
  *         0x34: memory full
  */
int InsertUserDefineEle(const char * szUrl, unsigned int uiLevel, unsigned int uiUrlType);

 /**
  * @brief Delete User Define Elements from UrlLib
  * @details
  *
  * @param szUrl url to delete
  * @param int uiLevel: Ref E_URL_LEVEL above
  *
  * @return   0x00: delete success
  *          0x41: bad szUrl
  *          0x42: bad uiLevel
  *          0x43: deleted fail, the elements is already deleted
  */
int DeleteUserDefineEle(const char * szUrl, unsigned int uiLevel);

 /**
  * @brief Check Url's Status with Eviltype
  * @details
  *
  * @param szUrl: url to check
  * @param int uiLevel: Url's evil level when url is black, Refer E_URL_LEVEL above
  *
  * @return UrlType Ref E_URL_TYPE above
  */
unsigned int UrlLibDetectUserDefine(const char * szUrl, unsigned int * uiLevel);

#endif
