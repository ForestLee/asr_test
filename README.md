# asr_test
录音，转成rate 16000，单声道，16bit的音频pcm数据，发送到baidu asr识别

# 降噪处理
TBD， can reference to
https://www.cnblogs.com/cpuimage/p/8905965.html
https://blog.csdn.net/aa98865646/article/details/105435426

# 笔记
arecord -l

arecord -c 2 -D hw:1,0 -r 44100 -f S16_LE  ./2.wav

aplay 2.wav

arecord -c 2 -D hw:1,0 -r 44100 -f S16_LE -t raw ./2.pcm

aplay -t raw -c 2 -f S16_LE -r 44100 2.pcm 

