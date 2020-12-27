/*
 进行音频采集，采集pcm数据并直接保存pcm数据
 音频参数：
	 声道数：		2
	 采样位数：	16bit、LE格式
	 采样频率：	44100Hz

 "arecord -l" to find device name
*/

#ifndef RECORD_PCM_H_
#define RECORD_PCM_H_

#include <stdio.h>
#include <stdlib.h>
#include "alsa/asoundlib.h"
#include <signal.h>
#include<iostream>
#include <unistd.h>

namespace audio {

class RecordPcm {
    private:
    bool _status = false;
    std::string _device_name;
    const std::string _file_name = "/home/forest/asr/2.pcm";
    FILE *_pcm_data_file = nullptr;
    int _stop_record = 0;
    char *_buffer;
    int _buffer_frames = 128;
    unsigned int _rate = 16000;// 常用的采样频率: 44100Hz 、16000HZ、8000HZ、48000HZ、22050HZ
    snd_pcm_t *_capture_handle;// 一个指向PCM设备的句柄
    snd_pcm_hw_params_t *_hw_params; //此结构包含有关硬件的信息，可用于指定PCM流的配置
    /*PCM的采样格式在pcm.h文件里有定义*/
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE; // 采样位数：16bit、LE格式
    char *_tot_buffer;
    int _single_piece_len = 0;
    int _real_len = 0;
    const int MAX_PIECES = 100000;

    public:
    RecordPcm(const char *device_name = "hw:1,0", const char *file_name = "/home/forest/asr/2.pcm");

    void start_record();

    void stop_record();

    char *get_record() const;

    int get_length() const;

    bool is_stop();
    ~RecordPcm();

    private:
    void _init();
    void _destory();
};

}
#endif  // RECORD_PCM_H_
