// in ubuntu x86_64, now can only record in 44100 rate
#include "record_pcm.h"

namespace audio {

RecordPcm::RecordPcm(const char *device_name, const char *file_name) : _device_name(device_name), _file_name(file_name) {
    // _init();
}

void RecordPcm::_init() {
    if (_status) {
        printf("不能重复打开设备\n");
        return;
    }
    _status = true;
    int err;
    /*打开音频采集卡硬件，并判断硬件是否打开成功，若打开失败则打印出错误提示*/
    if ((err = snd_pcm_open(&_capture_handle, _device_name.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        printf("无法打开音频设备: %s (%s)\n", _device_name.c_str(), snd_strerror(err));
        return;
    }
    //printf("音频接口打开成功.\n");

    /*创建一个保存PCM数据的文件*/
    if ((_pcm_data_file = fopen(_file_name.c_str(), "wb")) == NULL) {
        printf("无法创建%s音频文件.\n", _file_name.c_str());
        return;
    }
    //printf("用于录制的音频文件已打开.\n");

    /*分配硬件参数结构对象，并判断是否分配成功*/
    if ((err = snd_pcm_hw_params_malloc(&_hw_params)) < 0) {
        printf("无法分配硬件参数结构 (%s)\n", snd_strerror(err));
        return;
    }
    //printf("硬件参数结构已分配成功.\n");

    /*按照默认设置对硬件对象进行设置，并判断是否设置成功*/
    if ((err = snd_pcm_hw_params_any(_capture_handle, _hw_params)) < 0) {
        printf("无法初始化硬件参数结构 (%s)\n", snd_strerror(err));
        return;
    }
    //printf("硬件参数结构初始化成功.\n");

    /*
        设置数据为交叉模式，并判断是否设置成功
        interleaved/non interleaved:交叉/非交叉模式。
        表示在多声道数据传输的过程中是采样交叉的模式还是非交叉的模式。
        对多声道数据，如果采样交叉模式，使用一块buffer即可，其中各声道的数据交叉传输；
        如果使用非交叉模式，需要为各声道分别分配一个buffer，各声道数据分别传输。
    */
    if ((err = snd_pcm_hw_params_set_access(_capture_handle, _hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        printf("无法设置访问类型(%s)\n", snd_strerror(err));
        return;
    }
    //printf("访问类型设置成功.\n");

    /*设置数据编码格式，并判断是否设置成功*/
    if ((err = snd_pcm_hw_params_set_format(_capture_handle, _hw_params, format)) < 0) {
        printf("无法设置格式 (%s)\n", snd_strerror(err));
        return;
    }
    //fprintf(stdout, "PCM数据格式设置成功.\n");

    /*设置采样频率，并判断是否设置成功*/
    if ((err = snd_pcm_hw_params_set_rate_near(_capture_handle, _hw_params, &_rate, 0)) < 0) {
        printf("无法设置采样率(%s)\n", snd_strerror(err));
        return;
    }
    //printf("采样率设置成功\n");

    /*设置声道，并判断是否设置成功*/
    if ((err = snd_pcm_hw_params_set_channels(_capture_handle, _hw_params, 2)) < 0) {
        printf("无法设置声道数(%s)\n", snd_strerror(err));
        return;
    }
    //printf("声道数设置成功.\n");

    /*将配置写入驱动程序中，并判断是否配置成功*/
    if ((err = snd_pcm_hw_params(_capture_handle, _hw_params)) < 0) {
        printf("无法向驱动程序设置参数(%s)\n", snd_strerror(err));
        return;
    }
    //printf("参数设置成功.\n");

    /*使采集卡处于空闲状态*/
    snd_pcm_hw_params_free(_hw_params);

    /*准备音频接口,并判断是否准备好*/
    if ((err = snd_pcm_prepare(_capture_handle)) < 0) {
        printf("无法使用音频接口 (%s)\n", snd_strerror(err));
        return;
    }
    //printf("音频接口准备好.\n");

    /*配置一个数据缓冲区用来缓冲数据*/
    _single_piece_len = 128 * snd_pcm_format_width(format) / 8 * 2;
    _buffer = static_cast<char *>(malloc(_single_piece_len));
    _tot_buffer = static_cast<char *>(malloc(_single_piece_len * MAX_PIECES));
    //printf("缓冲区分配成功.\n");
}

void RecordPcm::start_record() {
    _destory();
    _init();
    /*开始采集音频pcm数据*/
    //printf("开始采集数据, 输入stop停止, exit退出\n");
    int err;
    memset(_buffer, 0, _single_piece_len);
    memset(_tot_buffer, 0, _single_piece_len * MAX_PIECES);
    _real_len = 0;
    while (_real_len / _single_piece_len < MAX_PIECES -2) {
        /*从声卡设备读取一帧音频数据*/
        if ((err = snd_pcm_readi(_capture_handle, _buffer, _buffer_frames)) != _buffer_frames) {
            if (err == - EPIPE) {
                fprintf(stderr, "overrun occurred\n");
                snd_pcm_prepare(_capture_handle);
            } else if (err < 0) {
                printf("从音频接口读取失败(%s)\n", snd_strerror(err));
                return;
            }
        }
        /*写数据到文件*/
        fwrite(_buffer, (_buffer_frames * 2), sizeof(short), _pcm_data_file);

        memcpy(&_tot_buffer[_real_len], _buffer, _single_piece_len);

        _real_len += _single_piece_len;

        if (_stop_record == 1) {
            //printf("停止采集: %d\n", _real_len);
            _stop_record = 2;
            break;
        }
    }

    if (_stop_record == 0) {
        _real_len -= _single_piece_len;
        // printf("停止采集2: %d\n", _real_len);
    }

    return ;//_real_len;
}

void RecordPcm::stop_record() {
    _stop_record = 1;
}

bool RecordPcm::is_stop() {
    return _stop_record == 2;
}

char *RecordPcm::get_record() const {
    return _tot_buffer;
}

int RecordPcm::get_length() const {
    return _real_len;
}

RecordPcm::~RecordPcm() {
    _destory();
}

void RecordPcm::_destory() {
    if (_status) {
        // printf("关闭音频设备\n");
        /*释放数据缓冲区*/
        free(_buffer);
        free(_tot_buffer);

        /*关闭音频采集卡硬件*/
        snd_pcm_close(_capture_handle);

        /*关闭文件流*/
        fclose(_pcm_data_file);
        _status = false;
    }
}

}