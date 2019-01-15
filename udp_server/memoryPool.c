#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "uv.h"
#include "memoryPool.h"
#include "common.h"

int memoryPoolInit(uv_udp_t * handle){
    struct poolHead * pHead = (struct poolHead *)malloc(sizeof(struct poolHead));
    if(pHead == NULL){
        LOG("malloc(struct poolHead) failed!");
        return -1;
    }
    
    int iRet = uv_udp_init(handle->loop,&pHead->handle);
    if(iRet){
        free(pHead);
        LOG("uv_udp_init failed! %s",uv_strerror(iRet));
        return -2;
    }
    
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0",0,&addr);
    iRet = uv_udp_bind(&pHead->handle,(const struct sockaddr *)&addr,UV_UDP_REUSEADDR);
    if(iRet){
        free(pHead);
        LOG("uv_udp_bind failed! %s",uv_strerror(iRet));
        return -3;
    }
    
    pHead->pContext = (struct context *)calloc(MAX_CLIENT_NUM,sizeof(struct context));
    if(pHead->pContext == NULL){
        free(pHead);
        LOG("calloc(struct context) failed!");
        return -4;
    }
    for(unsigned int uiNum = 0;uiNum < MAX_CLIENT_NUM;++uiNum){
        (pHead->pContext + uiNum)->bufLen = MAX_BUF_LEN;
        (pHead->pContext + uiNum)->handle = &pHead->handle;
    }
    
    pHead->uiSeq = 0;
    handle->data = (void *)pHead;
    return 0;
}

int memoryPoolDestroy(uv_udp_t * handle){
    if(handle->data == NULL){
        LOG("handle->data == NULL !");
        return -1;
    }
    struct poolHead * pHead = (struct poolHead *)(handle->data);
    if(pHead->pContext == NULL){
        free(pHead);
        handle->data = NULL;
        LOG("pHead->pContext == NULL !");
        return -2;
    } else {
        free(pHead->pContext);
        pHead->pContext = NULL;

        free(pHead);
        handle->data = NULL;
        return 0;
    }
}

struct context * contextGet(const uv_udp_t * handle){
    if(handle->data == NULL){
        return NULL;
    }
    struct poolHead * pHead = (struct poolHead *)(handle->data);
    if(pHead->uiSeq == MAX_CLIENT_NUM){
        pHead->uiSeq = 0;
    }
    struct context * pContext = pHead->pContext + pHead->uiSeq;
        
    if(!pContext->isUsed){
        pContext->isUsed = true;
        ++pHead->uiSeq;
        return pContext;
    } else {
        return NULL;
    }
}

void contextFree(struct context * pContext){
    if(pContext != NULL){
        memset(pContext->szBuf,0,MAX_BUF_LEN);
        pContext->bufLen = MAX_BUF_LEN;
        memset(&(pContext->client_addr),0,sizeof(struct sockaddr));
        (pContext->work_req).data = NULL; 
        (pContext->send_req).data = NULL; 
        pContext->isUsed = false;
    }
}
