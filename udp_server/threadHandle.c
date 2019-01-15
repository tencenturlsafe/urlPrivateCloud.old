#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uv.h"
#include "common.h"
#include "memoryPool.h"
#include "ParseConf.h"
#include "UrlQueryProto.h"
#include "urllibhandler.h"

static void alloc_buffer(uv_handle_t * handle, size_t size, uv_buf_t * buf) {
    char * base = (char *)calloc(1,size);
    if(!base) {
        *buf = uv_buf_init(NULL,0);
    } else {
        *buf = uv_buf_init(base,size);
    }
}

//完成发送后回调处理
static void after_send(uv_udp_send_t * req,int status){
    struct context * pContext = (struct context * )req->data;
    contextFree(pContext);
}

//socket可写回调
static void afterMatchHandle(uv_work_t * req,int status){
    struct context * pContext = req->data;
    uv_buf_t buf = uv_buf_init(pContext->szBuf,pContext->bufLen);
    (pContext->send_req).data = (void *)pContext;
    
    uv_udp_send(&(pContext->send_req), pContext->handle,&buf,1,(const struct sockaddr *)&(pContext->client_addr),after_send);
}

//执行业务逻辑处理
static void matchHandle(uv_work_t * req){
    struct context * pContext = (struct context *)req->data;
    stRequest request;
    register int iRet = decodeReq((const unsigned char *)pContext->szBuf, pContext->bufLen, &request);
    if(0 != iRet){
        stResponse responseDefault;
        memset(&responseDefault, 0, sizeof (stResponse));
        responseDefault.uiSeq = request.uiSeq;
        responseDefault.reCode = iRet;
        pContext->bufLen = encodeResp(&responseDefault, (unsigned char *)pContext->szBuf, MAX_BUF_LEN);
    } else {
        stResponse response;
        memset(&response, 0, sizeof (stResponse));
        response.uiSeq = request.uiSeq;
        memcpy(response.reservedBuffer,request.reservedBuffer,MAX_RESERVED_LEN);

        register int iRet = UrlLibDetect(request.sourceUrl, &(response.uiUrlType), &(response.uiEvilType), &(response.uiLevel), response.parameter);
        if (iRet == 0x00 || iRet == 0x52) {
            response.reCode = 0;
        } else {
            response.reCode = 1;
        }
        pContext->bufLen = encodeResp(&response, (unsigned char *)pContext->szBuf, MAX_BUF_LEN);
    }
}

//socket可读回调
static void recv_cb(uv_udp_t * handle,ssize_t nread,const uv_buf_t * buf,const struct sockaddr * addr,unsigned flags){
    if(nread <= 0){
        free(buf->base);
        return;
    } else {
        struct context * pContext = contextGet((const uv_udp_t *)handle);
        if(pContext == NULL){
            free(buf->base);
            return;
        }
        
        pContext->bufLen = nread > pContext->bufLen ? pContext->bufLen : nread;
        memcpy(pContext->szBuf ,buf->base, pContext->bufLen);
        memcpy((void *)&pContext->client_addr,(const void *)addr,sizeof(struct sockaddr));
        
        free(buf->base);
        
        (pContext->work_req).data = (void *)pContext;
        uv_queue_work(handle->loop,&(pContext->work_req),matchHandle,afterMatchHandle);
    }
}

static void threadHandle(void * arg){
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0",*((short *)arg),&addr);
    
    //uv_loop_t * loop = uv_default_loop();
    uv_loop_t * loop = uv_loop_new();
    if(loop == NULL){
        LOG("uv_loop_new failed!");
        return;
    }
    uv_udp_t udp_sv;
    
    int iRet = uv_udp_init(loop,&udp_sv);
    if(iRet){
        LOG("uv_udp_init failed! %s",uv_strerror(iRet));
        return;
    }
    
    iRet = uv_udp_bind(&udp_sv,(const struct sockaddr *)&addr,UV_UDP_REUSEADDR);
    if(iRet){
        LOG("uv_udp_bind failed! %s",uv_strerror(iRet));
        return;
    }
    
    iRet = memoryPoolInit(&udp_sv);
    if(iRet){
        LOG("memoryPoolInit failed! ErrCode:%d",iRet);
        return;
    }
    
    iRet = uv_udp_recv_start(&udp_sv,alloc_buffer,recv_cb);
    if(iRet){
        LOG("uv_udp_recv_start failed! %s",uv_strerror(iRet));
        return;
    }
   
    uv_run(loop,UV_RUN_DEFAULT);
    uv_loop_delete(loop);
}

bool startThread(const char * szConfFile){
    static stConf confInfo;
    if (!AnalyzeConf(szConfFile,&confInfo)) {
        LOG("Error Config");
        return false;
    }
     
    if (1 == confInfo.uiType) {
        int iErrCode = 0;
        if (!UrlLibInit(confInfo.uiShmKey, confInfo.uiSize, confInfo.strLicence, &iErrCode)) {
            LOG("UrlLibInit Failed! iErrCode:%#x", iErrCode);
            return false;
        }
    } else if (2 == confInfo.uiType) {
        int iErrCode = 0;
        if (!UrlLibInitByMb(confInfo.uiShmKey, confInfo.uiSize, confInfo.strLicence, &iErrCode)) {
            LOG("UrlLibInitByMb Failed! iErrCode:%#x", iErrCode);
            return false;
        }
    } else {
        LOG("Error initType value! (1 for UrlLibInit and 2 for UrlLibInitByMb)");
        return false;
    }
    
    //设置工作线程数
    SETWORKTHREAD(confInfo.threadNum * 2);

    uv_thread_t thread_id[MAX_THREAD_NUM];
    unsigned int uiThread = 0;
    for (; uiThread < confInfo.threadNum && uiThread < MAX_THREAD_NUM; ++uiThread) {
        int iRet = uv_thread_create(&thread_id[uiThread],threadHandle,&confInfo.port[uiThread]);
        if(iRet){
            LOG("uv_thread_create %u failed! %s",uiThread,uv_strerror(iRet));
        }
    }
    
    for(unsigned int uiNum = 0;uiNum < uiThread;++uiNum){
        int iRet = uv_thread_join(&thread_id[uiNum]);
        if(iRet){
            LOG("uv_thread_join %u failed! %s",uiNum,uv_strerror(iRet));
        }
    }
    
    return true;
}
