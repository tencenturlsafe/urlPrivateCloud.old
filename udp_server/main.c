#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "threadHandle.h"

int main(int argc, char * argv[]) {
    if (argc != 2) {
        LOG("Usage:%s configFile",argv[0]);
        return -1;
    }
    
    if (!startThread(argv[1])) {
        LOG("startThread failed");
        return -2;
    }
    return 0;
}