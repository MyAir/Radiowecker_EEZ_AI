#include "AudioManager.h"
#include "HardwareConfig.h"
#include "UIManager.h"
#include "RadioData.h"
#include "lvgl.h"
#include "ui.h"

//##################################################################################################
// Audio Task

void audio_task(void *pvParameters) {
    AudioManager* am = (AudioManager*)pvParameters;
    while (true) {
        if (am->mp3 && am->mp3->isRunning()) {
            if (!am->mp3->loop()) {
                am->mp3->stop();
                am->playing = false;
                // Update playback info to show stopped
                strncpy(g_playbackInfo.Title, "Stopped", sizeof(g_playbackInfo.Title) - 1);
                g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
                strncpy(g_playbackInfo.Artist, "", sizeof(g_playbackInfo.Artist) - 1);
                g_playbackInfo.Artist[sizeof(g_playbackInfo.Artist) - 1] = '\0';
                strncpy(g_playbackInfo.Album, "", sizeof(g_playbackInfo.Album) - 1);
                g_playbackInfo.Album[sizeof(g_playbackInfo.Album) - 1] = '\0';
                // AlarmTitle is left blank (only used when radio started from alarm)
            }
        }
        vTaskDelay(10);
    }
}

//##################################################################################################
// AudioManager Class

AudioManager::AudioManager() {
    mp3 = nullptr;
    file = nullptr;
    buff = nullptr;
    out = nullptr;
    playing = false;
}

void AudioManager::begin() {
    // Initialize ESP8266Audio components
    out = new AudioOutputI2S();
    // Configure I2S pins using macros from HardwareConfig.h
    out->SetPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
    out->SetGain((float)volume / 100.0);
    
    mp3 = new AudioGeneratorMP3();
    
    xTaskCreatePinnedToCore(
        audio_task,          /* Task function. */
        "AudioTask",         /* name of task. */
        8192,                /* Stack size of task */
        this,                /* parameter of the task */
        1,                   /* priority of the task */
        &audioTaskHandle,    /* Task handle to keep track of created task */
        0);                  /* pin task to core 0 */
}

void AudioManager::loop() {
    // The main audio processing is in the audio_task now.
    // This function can be used for other checks if needed.
}

bool AudioManager::connecttohost(const char* host) {
    // Stop current playback if running
    if (playing && mp3 && mp3->isRunning()) {
        mp3->stop();
    }
    
    // Clean up existing stream components
    if (file) {
        file->close();
        delete file;
    }
    if (buff) {
        delete buff;
    }
    
    current_host = host;
    // Update playback info to show connecting status
    strncpy(g_playbackInfo.Album, "", sizeof(g_playbackInfo.Album) - 1);
    g_playbackInfo.Album[sizeof(g_playbackInfo.Album) - 1] = '\0';
    strncpy(g_playbackInfo.Title, "Connecting...", sizeof(g_playbackInfo.Title) - 1);
    g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
    strncpy(g_playbackInfo.Artist, "", sizeof(g_playbackInfo.Artist) - 1);
    g_playbackInfo.Artist[sizeof(g_playbackInfo.Artist) - 1] = '\0';
    // AlarmTitle is left blank (only used when radio started from alarm)
    
    // Create new stream components
    file = new AudioFileSourceHTTPStream(host);
    buff = new AudioFileSourceBuffer(file, 2048);
    
    // Start playback
    if (mp3->begin(buff, out)) {
        playing = true;
        strncpy(g_playbackInfo.Title, "Playing...", sizeof(g_playbackInfo.Title) - 1);
        g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
        return true;
    } else {
        strncpy(g_playbackInfo.Title, "Connection Failed", sizeof(g_playbackInfo.Title) - 1);
        g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
        playing = false;
        return false;
    }
}

void AudioManager::stop() {
    if (playing && mp3 && mp3->isRunning()) {
        mp3->stop();
        playing = false;
    }
    
    // Clean up stream components
    if (file) {
        file->close();
        delete file;
        file = nullptr;
    }
    if (buff) {
        delete buff;
        buff = nullptr;
    }
    
    current_host = nullptr;
    // Update playback info to show stopped status
    strncpy(g_playbackInfo.Album, "", sizeof(g_playbackInfo.Album) - 1);
    g_playbackInfo.Album[sizeof(g_playbackInfo.Album) - 1] = '\0';
    strncpy(g_playbackInfo.Title, "Stopped", sizeof(g_playbackInfo.Title) - 1);
    g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
    strncpy(g_playbackInfo.Artist, "", sizeof(g_playbackInfo.Artist) - 1);
    g_playbackInfo.Artist[sizeof(g_playbackInfo.Artist) - 1] = '\0';
    // AlarmTitle is left blank (only used when radio started from alarm)
}

void AudioManager::setVolume(uint8_t vol) {
    volume = vol;
    if (out) {
        out->SetGain((float)volume / 100.0);
    }
}

uint8_t AudioManager::getVolume() {
    return volume;
}

bool AudioManager::isPlaying() {
    return playing && mp3 && mp3->isRunning();
}
