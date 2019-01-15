#ifndef _MEMORY_POOL_
#define _MEMORY_POOL_

#define MAX_CLIENT_NUM 10000 
#define MAX_BUF_LEN 2048
#pragma pack(4)
struct context{
    char  szBuf[MAX_BUF_LEN];
    size_t bufLen;
    struct sockaddr client_addr;
    uv_work_t work_req;
    uv_udp_send_t send_req;
    uv_udp_t * handle;
    bool isUsed;
};

struct poolHead{
    struct context * pContext;
    uv_udp_t handle;
    unsigned int uiSeq;
};
#pragma pack()

int memoryPoolInit(uv_udp_t * handle);

int memoryPoolDestroy(uv_udp_t * handle);

struct context * contextGet(const uv_udp_t * handle);
    
void contextFree(struct context * pContext);

#endif //_MEMORY_POOL_
