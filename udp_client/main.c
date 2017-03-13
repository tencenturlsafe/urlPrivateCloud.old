#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <arpa/inet.h>
#include "UrlQueryProto.h"

#define MAX_IP_LEN 16
#define MAX_BUF_SIZE 2048

char serverIp[MAX_IP_LEN] = "10.191.4.178";
unsigned short serverPort = 30008;

int main(int argc, char * argv[]) {
    if (argc != 2) {
        printf("Usage:%s\tUrl\n", argv[0]);
        return -1;
    }

    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 == fd) {
        perror("call socket");
        return -2;
    }

    struct sockaddr_in serverAddr;
    socklen_t addr_server_len = sizeof (serverAddr);
    memset(&serverAddr, 0, addr_server_len);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);

    //设置请求包并编码到buffer
    stRequest request;
    memset(&request, 0, sizeof (request));
    srand((unsigned)time(NULL));
    //若采用异步调用，seq为包的唯一标识
    request.uiSeq = rand() % UINT_MAX;
    snprintf(request.sourceUrl, MAX_URL_LEN, "%s", argv[1]);
    //reservedBuffer为预留字段，存储二进制流，前四字节为二进制流的长度
    char reservedTest[] = "reserved test!";
    unsigned int reservedLen = strlen(reservedTest);
    memcpy(request.reservedBuffer,&reservedLen,4);
    memcpy(request.reservedBuffer + 4,reservedTest,reservedLen);
    
    unsigned char szBuffer[MAX_BUF_SIZE] = {0};
    int length = encodeReq(&request, szBuffer, MAX_BUF_SIZE);
    if(length == -1){
        printf("encodeReq error");
        return -3;
    }
    int iRet = sendto(fd, szBuffer, length, 0, (struct sockaddr *) &serverAddr, (socklen_t) addr_server_len);
    if(iRet < 0){
        perror("sendto");
        return -4;
    }
    
    //接收请求包并解码
    struct sockaddr_in clientAddr;
    socklen_t addr_client_len = sizeof (clientAddr);
    memset(&clientAddr, 0, addr_client_len);
    iRet = recvfrom(fd, szBuffer, MAX_BUF_SIZE, 0, (struct sockaddr *) &clientAddr, (socklen_t *) & addr_client_len);
    if (iRet < 0){
        perror("recvfrom");
        return -1;
    }

    stResponse response;
    if (decodeResp(szBuffer, iRet, &response) != 0x00){
        printf("decodeResp error!\n");
        return -1;
    }

    if(response.reCode != 0){
        printf("server handle the url failed!");
    } else {
        unsigned int uiReservedLen = *(unsigned int *) (response.reservedBuffer);
        printf("uiSeq:%u\tuiUrlType:%u\tuiEvilType:%u\tuiLevel:%u\tparameter:%s\tuiReservedLen:%u\treservedBuffer:%s\n",
                response.uiSeq,response.uiUrlType,response.uiEvilType,response.uiLevel,response.parameter,uiReservedLen,response.reservedBuffer +4);
    }
    return 0;
}
