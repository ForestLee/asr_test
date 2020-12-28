/*
 进行音频采集，采集pcm数据并直接保存pcm数据
 音频参数： 
	 声道数：		2
	 采样位数：	16bit、LE格式
	 采样频率：	44100Hz

	 check record device name: arecord -l
	 record pcm, like:arecord -c 2 -D hw:1,0 -r 44100 -f S16_LE -t raw ./2.pcm
	 play pcm: aplay 2.wav
               aplay -t raw -c 2 -f S16_LE -r 44100 2.pcm
               aplay -t raw -c 1 -f S16_LE -r 16000 2_16k.pcm

*/
#include <stdio.h>
#include <stdlib.h>
#include "alsa/asoundlib.h"
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <pthread.h>
#include "record_pcm.h"
#include "asr_process.h"

using namespace audio;

//FILE *pcm_data_file = nullptr;
//int run_flag = 0;
//void exit_sighandler(int sig) {
//    run_flag = 1;
//}
//
//// record pcm, like:arecord -c 2 -D hw:1,0 -r 44100 -f S16_LE -t raw ./2.pcm
//int record_pcm(const char *device_name, const char *file_name) {
//    int i;
//    int err;
//    char *buffer;
//    int buffer_frames = 128;
//    unsigned int rate = 16000;// 常用的采样频率: 44100Hz 、16000HZ、8000HZ、48000HZ、22050HZ
//    snd_pcm_t *capture_handle;// 一个指向PCM设备的句柄
//    snd_pcm_hw_params_t *hw_params; //此结构包含有关硬件的信息，可用于指定PCM流的配置
//
//    /*注册信号捕获退出接口*/
//    signal(2, exit_sighandler);
//
//    /*PCM的采样格式在pcm.h文件里有定义*/
//    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE; // 采样位数：16bit、LE格式
//
//    /*打开音频采集卡硬件，并判断硬件是否打开成功，若打开失败则打印出错误提示*/
//    if ((err = snd_pcm_open(&capture_handle, device_name, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
//        printf("无法打开音频设备: %s (%s)\n", device_name, snd_strerror(err));
//        exit(1);
//    }
//    printf("音频接口打开成功.\n");
//
//    /*创建一个保存PCM数据的文件*/
//    if ((pcm_data_file = fopen(file_name, "wb")) == NULL) {
//        printf("无法创建%s音频文件.\n", file_name);
//        exit(1);
//    }
//    printf("用于录制的音频文件已打开.\n");
//
//    /*分配硬件参数结构对象，并判断是否分配成功*/
//    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
//        printf("无法分配硬件参数结构 (%s)\n", snd_strerror(err));
//        exit(1);
//    }
//    printf("硬件参数结构已分配成功.\n");
//
//    /*按照默认设置对硬件对象进行设置，并判断是否设置成功*/
//    if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
//        printf("无法初始化硬件参数结构 (%s)\n", snd_strerror(err));
//        exit(1);
//    }
//    printf("硬件参数结构初始化成功.\n");
//
//    /*
//        设置数据为交叉模式，并判断是否设置成功
//        interleaved/non interleaved:交叉/非交叉模式。
//        表示在多声道数据传输的过程中是采样交叉的模式还是非交叉的模式。
//        对多声道数据，如果采样交叉模式，使用一块buffer即可，其中各声道的数据交叉传输；
//        如果使用非交叉模式，需要为各声道分别分配一个buffer，各声道数据分别传输。
//    */
//    if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
//        printf("无法设置访问类型(%s)\n", snd_strerror(err));
//        exit(1);
//    }
//    printf("访问类型设置成功.\n");
//
//    /*设置数据编码格式，并判断是否设置成功*/
//    if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, format)) < 0) {
//        printf("无法设置格式 (%s)\n", snd_strerror(err));
//        exit(1);
//    }
//    fprintf(stdout, "PCM数据格式设置成功.\n");
//
//    /*设置采样频率，并判断是否设置成功*/
//    if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0)) < 0) {
//        printf("无法设置采样率(%s)\n", snd_strerror(err));
//        exit(1);
//    }
//    printf("采样率设置成功\n");
//
//    /*设置声道，并判断是否设置成功*/
//    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, 2)) < 0) {
//        printf("无法设置声道数(%s)\n", snd_strerror(err));
//        exit(1);
//    }
//    printf("声道数设置成功.\n");
//
//    /*将配置写入驱动程序中，并判断是否配置成功*/
//    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
//        printf("无法向驱动程序设置参数(%s)\n", snd_strerror(err));
//        exit(1);
//    }
//    printf("参数设置成功.\n");
//
//    /*使采集卡处于空闲状态*/
//    snd_pcm_hw_params_free(hw_params);
//
//    /*准备音频接口,并判断是否准备好*/
//    if ((err = snd_pcm_prepare(capture_handle)) < 0) {
//        printf("无法使用音频接口 (%s)\n", snd_strerror(err));
//        exit(1);
//    }
//    printf("音频接口准备好.\n");
//
//    /*配置一个数据缓冲区用来缓冲数据*/
//    buffer = static_cast<char *>(malloc(128 * snd_pcm_format_width(format) / 8 * 2));
//    printf("缓冲区分配成功.\n");
//
//    /*开始采集音频pcm数据*/
//    printf("开始采集数据...\n");
//    while (1) {
//        /*从声卡设备读取一帧音频数据*/
//        if ((err = snd_pcm_readi(capture_handle, buffer, buffer_frames)) != buffer_frames) {
//            printf("从音频接口读取失败(%s)\n", snd_strerror(err));
//            exit(1);
//        }
//        /*写数据到文件*/
//        fwrite(buffer, (buffer_frames * 2), sizeof(short), pcm_data_file);
//
//        if (run_flag) {
//            printf("停止采集.\n");
//            break;
//        }
//    }
//
//    /*释放数据缓冲区*/
//    free(buffer);
//
//    /*关闭音频采集卡硬件*/
//    snd_pcm_close(capture_handle);
//
//    /*关闭文件流*/
//    fclose(pcm_data_file);
//    return 0;
//}

//// convert wav file to pcm file, just remove 44 bytes header from wav, then save to pcm file
//// ref: https://www.codeproject.com/Articles/501521/How-to-convert-between-most-audio-formats-in-NET
//int wav_to_pcm_remove_header(const char *input_wav_file, const char *output_pcm_file) {
//    size_t result;
//    char *buf;
//    FILE *fp1 = fopen(input_wav_file/*"/home/forest/asr/2.wav"*/, "rb");//wav文件打开，打开读权限
//    FILE *fp2 = fopen(output_pcm_file/*"/home/forest/asr/2.pcm"*/, "wb");//pcm文件创建，给予写权限
//    fseek(fp1, 0, SEEK_END);//文件指针从0挪到尾部
//    long filesize;
//    filesize = ftell(fp1);//ftell求文件指针相对于0的便宜字节数，就求出了文件字节数
//
//    if (fp1 == nullptr || fp2 == nullptr)//判断两个文件是否打开
//    {
//        printf("file open filed!!\n");
//        return 0;
//    }
//
//    rewind(fp1);//还原指针位置
//    fseek(fp1, 44, SEEK_SET);//wav文件的指针从头向后移动44字节
//    buf = (char *) malloc(sizeof(char) * filesize);//开辟空间给缓存数组
//
//    if (buf == nullptr) {
//        printf("memory  error\n");
//        return 0;
//    }
//
//    result = fread(buf, 1, (filesize - 44), fp1);//每次读一个字节到buf，同时求读的次数
//    if (result != filesize - 44)//判断读的次数和文件大小是否一致
//    {
//        printf("reing error!!\n");
//        return 0;
//    }
//    fwrite(buf, 1, (filesize - 44), fp2);//写到pcm文件中
//    fclose(fp1);//关闭文件指针
//    fclose(fp2);
//    free(buf);//释放buf
//    return 0;
//}

//// down sample, rate from 44100 to 16000, 2-byte(16bit)-data
//int down_sample(short int in_buffer[],
//                short int out_buffer[],
//                int input_sample_rate,
//                int output_sample_rate,
//                int input_len) {
//    double ratio = (double) input_sample_rate / output_sample_rate;
//    int out_sample = 0;
//    while (true) {
//        int in_buffer_index = (int) (out_sample++ * ratio);
//        if (in_buffer_index < input_len)
//            out_buffer[out_sample] = in_buffer[in_buffer_index];
//        else
//            break;
//    }
//
//    printf("down sample %d\n", out_sample - 1);
//    return out_sample - 1;
//}

// down sample, rate from 44100 to 16000, 2-byte(16bit)-data
// 2 channel to 1 channel
int down_sample_mono(short int in_buffer[],
                short int out_buffer[],
                int input_sample_rate,
                int output_sample_rate,
                int input_len) {
    double ratio = (double) input_sample_rate / output_sample_rate;
    int out_sample = 0;
    while (true) {
        int in_buffer_index = (int) (out_sample++ * ratio);
        if (in_buffer_index < input_len)
            out_buffer[out_sample] = in_buffer[in_buffer_index];
        else
            break;
    }

    int len = (out_sample - 1) / 2;
    for (int i = 0; i < len; i++)
    {
        out_buffer[i] = (static_cast<int32_t>(out_buffer[2 * i]) + out_buffer[2 * i + 1]) >> 1;
    }

    //printf("down sample %d\n", len);
    return len;
}
//
//void down_sample_pcm_file(const char *input_pcm_file, const char *output_pcm_file) {
//    size_t result;
//    short int *in_buf;
//    short int *out_buf;
//    FILE *fp1 = fopen(input_pcm_file/*"/home/forest/asr/2.pcm"*/, "rb");//wav文件打开，打开读权限
//    FILE *fp2 = fopen(output_pcm_file/*"/home/forest/asr/2_16k.pcm"*/, "wb");//pcm文件创建，给予写权限
//    fseek(fp1, 0, SEEK_END);//文件指针从0挪到尾部
//    long filesize;
//    filesize = ftell(fp1) / sizeof(short int);//ftell求文件指针相对于0的便宜字节数，就求出了文件字节数
//    rewind(fp1);//还原指针位置
//    if (fp1 == nullptr || fp2 == nullptr)//判断两个文件是否打开
//    {
//        printf("file open filed!!\n");
//        return;
//    }
//
//    in_buf = (short int *) malloc(sizeof(short int) * (filesize / sizeof(short int)));//开辟空间给缓存数组
//    if (in_buf == nullptr) {
//        printf("memory  error\n");
//        return;
//    } else {
//        memset(in_buf, 0, filesize);
//    }
//
//    out_buf = (short int *) malloc(sizeof(short int) * (filesize / sizeof(short int)));//开辟空间给缓存数组
//    if (out_buf == nullptr) {
//        printf("memory  error\n");
//        return;
//    } else {
//        memset(out_buf, 0, filesize);
//    }
//
//    // result = fread(in_buf, 1, filesize / sizeof(float), fp1);//每次读一个字节到in_buf，同时求读的次数
//    result = fread(in_buf, sizeof(short int), filesize / sizeof(short int), fp1);
//    if (result != filesize / sizeof(short int))//判断读的次数和文件大小是否一致
//    {
//        printf("reading error!!\n");
//        return;
//    }
//
//    int out_len = down_sample_mono(in_buf, out_buf, 44100, 16000, filesize);
//
//    fwrite(out_buf, sizeof(short int), out_len, fp2);//写到pcm文件中
//    fclose(fp1);//关闭文件指针
//    fclose(fp2);
//    free(in_buf);//释放in_buf
//    free(out_buf);//释放out_buf
//}


//void down_sample_pcm_buffer_to_file(short int in_buf[], const char *output_pcm_file, int filesize) {
//    short int *out_buf;
//    FILE *fp2 = fopen(output_pcm_file/*"/home/forest/asr/2_16k.pcm"*/, "wb");//pcm文件创建，给予写权限
//    if (fp2 == nullptr)//判断两个文件是否打开
//    {
//        printf("file open filed!!\n");
//        return;
//    }
//
//    out_buf = (short int *) malloc(sizeof(short int) * (filesize / sizeof(short int)));//开辟空间给缓存数组
//    if (out_buf == nullptr) {
//        printf("memory  error\n");
//        return;
//    } else {
//        memset(out_buf, 0, filesize);
//    }
//
//    int out_len = down_sample_mono(in_buf, out_buf, 44100, 16000, filesize);
//
//    fwrite(out_buf, sizeof(short int), out_len, fp2);//写到pcm文件中
//    fclose(fp2);
//    free(out_buf);//释放out_buf
//}

std::shared_ptr<audio::RecordPcm> g_record = std::make_shared<audio::RecordPcm>("hw:1,0", "/home/forest/asr/2.pcm");

void *record_task(void *ptr) {
    g_record->start_record();
}

int main(int argc, char *argv[]) {
//    const char *device_name = argv[1];
//    const char *save_file = argv[2];
    // record_pcm(device_name, save_file);
    // wav_to_pcm_remove_header("/home/forest/asr/2.wav", "/home/forest/asr/2.pcm");
    // down_sample_pcm_file("/home/forest/asr/2.pcm", "/home/forest/asr/2_16k.pcm");

    printf("输入start开始录音, stop停止, exit退出\n");
    pthread_t id;
    bool run = true;
    short int *buffer = nullptr;
    short int *out_buf = nullptr; // (short int *) malloc(sizeof(short int) * (filesize / sizeof(short int)));
    AsrProcess asr_process;
    while (run) {
        std::string input;

        std::cout << "\n>";
        std::getline(std::cin, input);

        if (input == "start") {
            int ret = pthread_create(&id, nullptr, record_task, nullptr);
        } else if (input == "stop") {
            g_record->stop_record();
            while(!g_record->is_stop());
            int len = g_record->get_length();
            //printf("record %d bytes\n", len);
            buffer = (short int *)g_record->get_record();
            // down_sample_pcm_buffer_to_file(buffer, "/home/forest/asr/2_16k.pcm", len / sizeof(short int));
            out_buf = (short int *) malloc(len / sizeof(short int));
            int out_len = down_sample_mono(buffer, out_buf, 44100, 16000, len / sizeof(short int));

            char *char_buffer = (char *)out_buf;
            asr_process.run(char_buffer, out_len);
            free(out_buf);
            out_buf = nullptr;
        } else if (input == "exit") {
            break;
        }
    }

    pthread_join(id, nullptr);
    return 0;
}