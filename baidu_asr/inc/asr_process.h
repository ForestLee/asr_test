//
// Created by forest on 2020/12/27.
//

#ifndef ASR_BAIDU_ASR_ASR_PROCESS_H_
#define ASR_BAIDU_ASR_ASR_PROCESS_H_

#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <curl/curl.h>
#include <stdlib.h>

#include "common.h"
#include "token.h"


namespace audio {
struct asr_config {
    char api_key[40]; // 填写网页上申请的appkey 如 $apiKey="g8eBUMSokVB1BHGmgxxxxxx"
    char secret_key[40]; // 填写网页上申请的APP SECRET 如 $secretKey="94dc99566550d87f8fa8ece112xxxxx"
    FILE *file;  // 需要识别的本地文件
    char format[4];
    int rate;
    int dev_pid;
    //int lm_id;//测试自训练平台需要打开此注释
    char cuid[20];
    char scope[20];
    char url[100];
};



class AsrProcess {
    private:
    asr_config config;
    char token[MAX_TOKEN_SIZE];

    public:
    AsrProcess();

    static RETURN_CODE fill_config(asr_config *config);

    RETURN_CODE run(char *buffer, int content_len);

    RETURN_CODE run_asr(struct asr_config *config, const char *token, char *buffer, int content_len);


};

}
#endif //ASR_BAIDU_ASR_ASR_PROCESS_H_
