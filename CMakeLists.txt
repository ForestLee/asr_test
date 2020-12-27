cmake_minimum_required (VERSION 3.13)
project (asr)

set(CMAKE_C_STANDARD 99)
set(PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(CURL REQUIRED)
include_directories(
        ${CURL_INCLUDE_DIR}
        include
        baidu_asr/inc
        ./
)

add_executable(asr
        test.cpp
        record_pcm.cpp
        baidu_asr/src/asr_process.cpp
        baidu_asr/src/common.cpp
        baidu_asr/src/token.cpp
        )
target_link_libraries(asr asound pthread ${CURL_LIBRARY})
