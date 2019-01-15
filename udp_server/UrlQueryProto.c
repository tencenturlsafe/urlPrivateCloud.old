#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include "UrlQueryProto.h"

#define TAG_SIZE 1
#define LEN_SIZE 4

#define STX 0x02
#define ETX 0x03

static const size_t requestLen = sizeof(stRequest);
static const size_t responseLen = sizeof(stResponse);

enum tag_type{
    REQ_SEQ_TYPE = 1,
    REQ_URL_TYPE,
    REQ_BUFFER_TYPE,
    RESP_CODE_TYPE,
    RESP_SEQ_TYPE,
    RESP_URL_TYPE,
    RESP_EVIL_TYPE,
    RESP_LEVEL_TYPE,
    RESP_PARAM_TYPE,
    RESP_BUFFER_TYPE
};

#define COPY_ELEM(pBuf,iLeft,iOffset,pElem,iElemLen) \
    do{\
        if((pBuf) == NULL || (iLeft) == NULL || (iOffset) == NULL || (pElem) == NULL || (*(iLeft)) < (iElemLen))\
            return -1;\
        memcpy(pBuf,pElem,iElemLen);\
        (*(iLeft)) =(*(iLeft)) - (iElemLen);\
        (*(iOffset)) =(*(iOffset)) + (iElemLen);\
    }while(0)

int encodeReq(const stRequest * ptrRequest,unsigned char * szBuffer, int bufferLen) {
    if (ptrRequest == NULL || szBuffer == NULL || bufferLen <= 0){
        return -1;
    }

    memset(szBuffer,0,bufferLen);
    int offset = 0;
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){STX},TAG_SIZE);
    
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){REQ_SEQ_TYPE},TAG_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(LEN_SIZE)},LEN_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(ptrRequest->uiSeq)},LEN_SIZE);
 
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){REQ_URL_TYPE},TAG_SIZE);
    unsigned int uiUrlLen = strlen(ptrRequest->sourceUrl);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(uiUrlLen)},LEN_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,(void *)ptrRequest->sourceUrl,uiUrlLen);

    register unsigned int uiReservedLen = *(unsigned int *) (ptrRequest->reservedBuffer);
    if (uiReservedLen != 0) {
        COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){REQ_BUFFER_TYPE},TAG_SIZE);
        COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(uiReservedLen)},LEN_SIZE);
        COPY_ELEM(szBuffer + offset,&bufferLen,&offset,(void *)(ptrRequest->reservedBuffer + LEN_SIZE),uiReservedLen);
    }
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){ETX},TAG_SIZE);
    return offset;
}

int decodeReq(const unsigned char * szBuffer, unsigned int bufferLen, stRequest * ptrRequest) {
    if (szBuffer == NULL || ptrRequest == NULL || bufferLen < 2){
        return 0x01;
    } else if (szBuffer[0] != STX || szBuffer[bufferLen-1] != ETX) {
        return 0x02;
    } else  if (bufferLen == 2) {
        return 0x03;
    }
    
    memset(ptrRequest, 0, requestLen);
    const unsigned char * tempPtr = szBuffer + 1;
    bufferLen -= 2;
    register unsigned int handleLen = 0;
    while (handleLen < bufferLen) {
        register unsigned char ucTag = tempPtr[0];
        tempPtr = tempPtr + TAG_SIZE;
        unsigned int valueLen = ntohl(*(unsigned int *) (tempPtr));
        tempPtr = tempPtr + LEN_SIZE;
        switch (ucTag) {
            case REQ_SEQ_TYPE:
                ptrRequest->uiSeq = ntohl(*(unsigned int *) (tempPtr));
                break;
            case REQ_URL_TYPE:
                if(valueLen >= MAX_URL_LEN)
                    return 0x04;
                memcpy(ptrRequest->sourceUrl,(const char *)tempPtr, valueLen);
                ptrRequest->sourceUrl[valueLen] = '\0';
                break;
            case REQ_BUFFER_TYPE:
                if((valueLen + LEN_SIZE) > MAX_RESERVED_LEN)
                    return 0x05;
                memcpy(ptrRequest->reservedBuffer,&valueLen,LEN_SIZE);
                memcpy(ptrRequest->reservedBuffer + LEN_SIZE, (const char *)tempPtr, valueLen);
                break;
            default:
                break;
        }
        tempPtr = tempPtr + valueLen;
        handleLen = handleLen + TAG_SIZE + LEN_SIZE +valueLen;
    }
    return 0x00;
}

int encodeResp(const stResponse * ptrResponse,unsigned char * szBuffer,int bufferLen) {
    if (ptrResponse == NULL || szBuffer == NULL || bufferLen <= 0){
        return -1;
    }

    memset(szBuffer,0,bufferLen);
    int offset = 0;
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){STX},TAG_SIZE);
    
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){RESP_CODE_TYPE},TAG_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(TAG_SIZE)},LEN_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,(void *)&ptrResponse->reCode,TAG_SIZE);

    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){RESP_SEQ_TYPE},TAG_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(LEN_SIZE)},LEN_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(ptrResponse->uiSeq)},LEN_SIZE);
    
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){RESP_URL_TYPE},TAG_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(LEN_SIZE)},LEN_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(ptrResponse->uiUrlType)},LEN_SIZE);
    
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){RESP_EVIL_TYPE},TAG_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(LEN_SIZE)},LEN_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(ptrResponse->uiEvilType)},LEN_SIZE);
    
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){RESP_LEVEL_TYPE},TAG_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(LEN_SIZE)},LEN_SIZE);
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(ptrResponse->uiLevel)},LEN_SIZE);

    if (ptrResponse->parameter[0] != '\0') {
        COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){RESP_PARAM_TYPE},TAG_SIZE);
        register unsigned int uiParamLen = strlen(ptrResponse->parameter);
        COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(uiParamLen)},LEN_SIZE);
        COPY_ELEM(szBuffer + offset,&bufferLen,&offset,(void *)ptrResponse->parameter,uiParamLen);
    }
    
    register unsigned int uiReservedLen = *(unsigned int *) (ptrResponse->reservedBuffer);
    if (uiReservedLen != 0) {
        COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){RESP_BUFFER_TYPE},TAG_SIZE);
        COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned int){htonl(uiReservedLen)},LEN_SIZE);
        COPY_ELEM(szBuffer + offset,&bufferLen,&offset,(void *)(ptrResponse->reservedBuffer + LEN_SIZE),uiReservedLen);
    }
    COPY_ELEM(szBuffer + offset,&bufferLen,&offset,&(unsigned char){ETX},TAG_SIZE);
    return offset;
}

int decodeResp(const unsigned char * szBuffer, unsigned int bufferLen,stResponse * ptrResponse) {
   if(szBuffer == NULL || ptrResponse == NULL || bufferLen < 2){
        return 0x01;
   }else if (szBuffer[0] != STX || szBuffer[bufferLen-1] != ETX) {
       return 0x02;
   } else if (bufferLen == 2) {
       return 0x03;
   }
   
    memset(ptrResponse,0,responseLen);
    const unsigned char * tempPtr = szBuffer + 1;
    register unsigned int handleLen = 0;
    bufferLen -= 2;
    while(handleLen < bufferLen){
        register unsigned char ucTag = tempPtr[0];
        tempPtr = tempPtr + TAG_SIZE;
        unsigned int valueLen = ntohl(*(unsigned int *) (tempPtr));
        tempPtr = tempPtr + LEN_SIZE;
        switch(ucTag){
            case RESP_CODE_TYPE:
                ptrResponse->reCode = ntohl(*(unsigned char *)(tempPtr));
                break;
            case RESP_SEQ_TYPE:
                ptrResponse->uiSeq = ntohl(*(unsigned int *)(tempPtr));
                break;
            case RESP_URL_TYPE:
                ptrResponse->uiUrlType = ntohl(*(unsigned int *)(tempPtr));
                break;
            case RESP_EVIL_TYPE:
                ptrResponse->uiEvilType = ntohl(*(unsigned int *)(tempPtr));
                break;
            case RESP_LEVEL_TYPE:
                ptrResponse->uiLevel = ntohl(*(unsigned int *)(tempPtr));
                break;
            case RESP_PARAM_TYPE:
                if(valueLen >= MAX_PARAM_LEN)
                    return 0x04;
                memcpy(ptrResponse->parameter,(const char *)tempPtr,valueLen);
                ptrResponse->parameter[valueLen] = '\0';
                break;
            case RESP_BUFFER_TYPE:
                if((valueLen + LEN_SIZE) > MAX_RESERVED_LEN)
                    return 0x05;
                memcpy(ptrResponse->reservedBuffer,&valueLen,LEN_SIZE);
                memcpy(ptrResponse->reservedBuffer + LEN_SIZE, (const char *)tempPtr, valueLen);
                break;
        }
        tempPtr = tempPtr + valueLen;
        handleLen = handleLen + TAG_SIZE + LEN_SIZE +valueLen;
    }
    return 0x00;
}
