#ifndef _UEL_QUERY_PROTO_
#define _UEL_QUERY_PROTO_

#define MAX_URL_LEN 1280
#define MAX_RESERVED_LEN 512
#define MAX_PARAM_LEN 128

#pragma pack(4)
 /**
  * @brief request packet structure
  * @details
  *
  * @value  uiSeq: 请求包的序列号
  * @value  sourceUrl: 待匹配的url
  * @value  reservedBuffer: 预留字段，用于透传数据
  * 
  */
typedef struct{
    unsigned int uiSeq;
    char sourceUrl[MAX_URL_LEN];
    char reservedBuffer[MAX_RESERVED_LEN];
}stRequest;

 /**
  * @brief response packet structure
  * @details
  *
  * @value  uiSeq: 回复包的序列号
  * @value  uiUrlType: 参考urllibhanler.h的E_URL_TYPE
  * @value  uiEvilType: 标识的恶意类型，可调用urllibhanler.h的convType2Class()转换为EvilClass
  * @value  uiLevel: 参考urllibhanler.h的E_URL_LEVEL
  * @value  reCode: 返回码, 0x00: server成功处理; 0x01: server处理失败 
  * @value  parameter: 继续访问标识(若url为黑，带此参数，在有效时间内判未知)
  * @value  reservedBuffer: 预留字段，用于透传数据回传
  * 
  */
typedef struct{
    unsigned int uiSeq;
    unsigned int uiUrlType;
    unsigned int uiEvilType;
    unsigned int uiLevel;
    unsigned char reCode;
    char parameter[MAX_PARAM_LEN];
    char reservedBuffer[MAX_RESERVED_LEN];
}stResponse;
#pragma pack()

 /**
  * @brief 请求包编码（client使用）
  * @details
  *
  * @value  ptrRequest: 请求包结构体指针
  * @value  szBuffer: 存储编码数据的buffer
  * @value  bufferLen: buffer的可用大小
  * 
  * @return -1: 编码失败（传入参数有误）
  *         其他值：编码成功 （编码使用buf长度）
  * 
  */
int encodeReq(const stRequest * ptrRequest,unsigned char * szBuffer,int bufferLen);

 /**
  * @brief 请求包解码（server使用）
  * @details
  *
  * @value  szBuffer: 待解码的buffer
  * @value  bufferLen: buffer的长度
  * @value  ptrRequest: 存储解码数据的请求包
  * 
  * @return 0x00: 解码成功
  *          0x01：传入参数有误
  *          0x02：错误包
  *          0x03：空包
  *          0x04：url数据格式错误
  *          0x05：透传数据格式错误
  * 
  */
int decodeReq(const unsigned char * szBuffer, unsigned int bufferLen,stRequest * ptrRequest);

 /**
  * @brief 回复包编码（server使用）
  * @details
  *
  * @value  ptrResponse: 回复包结构体指针
  * @value  szBuffer: 存储编码数据的buffer
  * @value  bufferLen: buffer的可用大小
  * 
  * @return -1: 编码失败（传入参数有误）
  *         其他值：编码成功 （编码使用buf长度）
  * 
  */
int encodeResp(const stResponse * ptrResponse,unsigned char * szBuffer,int bufferLen);

 /**
  * @brief 回复包解码（client使用）
  * @details
  *
  * @value  szBuffer: 待解码的buffer
  * @value  bufferLen: buffer的长度
  * @value  ptrResponse: 存储解码数据的回复包
  * 
  * @return 0x00: 解码成功
  *          0x01：传入参数有误
  *          0x02：错误包
  *          0x03：空包
  *          0x04：parameter数据格式错误
  *          0x05：透传数据格式错误
  * 
  */
int decodeResp(const unsigned char * szBuffer, unsigned int bufferLen,stResponse * ptrResponse);

#endif //_UEL_QUERY_PROTO_
