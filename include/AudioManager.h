#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include <Arduino.h>
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
// Resolve STACK_SIZE macro collision with EEZ-Flow
#ifdef STACK_SIZE
#undef STACK_SIZE
#endif

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    void begin();
    void loop();
    bool connecttohost(const char* host);
    void stop();
    void setVolume(uint8_t vol);
    uint8_t getVolume();
    bool isPlaying();

private:
    // Arduino Audio Tools components
    audio_tools::URLStream url;
    audio_tools::MP3DecoderHelix decoder;
    audio_tools::I2SStream i2s;
    audio_tools::VolumeStream volume_stream;
    audio_tools::EncodedAudioStream* encoded_stream;
    audio_tools::StreamCopy* copier;
    
    // FreeRTOS task for audio processing
    TaskHandle_t audio_task_handle;
    static void audioTaskFunction(void* parameter);
    bool task_running;
    
    // Lazy initialization state
    bool lazy_initialized;
    
    // Deferred execution to avoid task creation in UI callbacks
    bool pending_start;
    String pending_url;
    bool _internal_connecttohost(const char* host);

    // Legacy members (kept for compatibility)
    const char* current_host = nullptr;
    uint8_t volume = 100;
    bool playing = false;
};

extern AudioManager audioManager;

#endif // _AUDIO_MANAGER_H
