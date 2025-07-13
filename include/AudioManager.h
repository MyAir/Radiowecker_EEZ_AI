#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include <Arduino.h>
#include "AudioFileSourceHTTPStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

void audio_task(void *pvParameters);

class AudioManager {
public:
    AudioManager();
    void begin();
    void loop();
    bool connecttohost(const char* host);
    void stop();
    void setVolume(uint8_t vol);
    uint8_t getVolume();
    bool isPlaying();

private:
    AudioGeneratorMP3 *mp3;
    AudioFileSourceHTTPStream *file;
    AudioFileSourceBuffer *buff;
    AudioOutputI2S *out;
    TaskHandle_t audioTaskHandle = NULL;
    const char* current_host = nullptr;
    uint8_t volume = 100;
    bool playing = false;

    friend void audio_task(void *pvParameters);
};

extern AudioManager audioManager;

#endif // _AUDIO_MANAGER_H
