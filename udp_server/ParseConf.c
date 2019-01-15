#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "ParseConf.h"

#define MAX_BUF_LEN 256

static char * del_left_trim(char * str) {
    if (str == NULL)
        return str;
    for (; *str != '\0' && isspace(*str); ++str);
    return str;
}

static char * del_both_trim(char * str) {
    if (str == NULL)
        return str;
    char * szOutput = del_left_trim(str);
    char * ptr;
    for (ptr = szOutput + strlen(szOutput) - 1; ptr >= szOutput && isspace(*ptr); --ptr);
    *(++ptr) = '\0';
    return szOutput;
}

bool AnalyzeConf(const char * szConfFile,stConf * pConf) {
    FILE * fpr = fopen(szConfFile, "rb");
    if (fpr == NULL) {
        LOG("open %s failed!", szConfFile);
        return false;
    }
    memset(pConf, 0, sizeof (stConf));
    char buf[MAX_BUF_LEN];
    while (!feof(fpr)) {
        memset(buf, 0, MAX_BUF_LEN);
        if (fgets(buf, MAX_BUF_LEN, fpr) != NULL) {
            char ch = del_left_trim(buf)[0];
            if (ch == '#' || ch == '\0')
                continue;
            char * key = del_both_trim(strtok(buf, "="));
            if (strcasecmp(key, "Type") == 0) {
                char * value = del_both_trim(strtok(NULL, "="));
                pConf->uiType = strtoul(value, NULL, 10);
            } else if (strcasecmp(key, "ShmKey") == 0) {
                char * value = del_both_trim(strtok(NULL, "="));
                pConf->uiShmKey = strtoul(value, NULL, 10);
            } else if (strcasecmp(key, "Size") == 0) {
                char * value = del_both_trim(strtok(NULL, "="));
                pConf->uiSize = strtoul(value, NULL, 10);
            } else if (strcasecmp(key, "Licence") == 0) {
                char * value = del_both_trim(strtok(NULL, "="));
                snprintf(pConf->strLicence,MAX_FILE_LEN,"%s",value);
            } else if (strcasecmp(key, "Port") == 0) {
                char * value = del_both_trim(strtok(NULL, "="));
                unsigned short uiPortCnt = 0;
                char * pPort = del_both_trim(strtok(value, ","));
                while (pPort !=NULL) {
                    pConf->port[uiPortCnt] = (short)atoi(pPort);
                    ++uiPortCnt;
                    pPort = del_both_trim(strtok(NULL, ","));
                }
                
            } else if (strcasecmp(key, "ThreadNum") == 0) {
                char * value = del_both_trim(strtok(NULL, "="));
                pConf->threadNum = (short)atoi(value);
            }
        }
    }
    fclose(fpr);
    if (pConf->uiType == 0 || pConf->uiShmKey == 0 || pConf->uiSize == 0 || 
            pConf->strLicence[0] == '\0' || pConf->threadNum == 0){
        LOG("config %s lack item!", szConfFile);
        return false;
    }
    for(unsigned short uNum = 0;uNum < pConf->threadNum; ++uNum){
        if(pConf->port[uNum] == 0){
            LOG("%s ThreadNum and Port not Correspond!", szConfFile);
            return false;
        }
    }
    return true;
}
