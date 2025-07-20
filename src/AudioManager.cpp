#include "AudioManager.h"
#include "HardwareConfig.h"
#include "UIManager.h"
#include "RadioData.h"
#include "lvgl.h"
#include "ui.h"
#include "debug_config.h"

//##################################################################################################
// Arduino Audio Tools Integration
// Note: No separate audio task needed - Arduino Audio Tools handles threading internally

//##################################################################################################
// AudioManager Class

AudioManager::AudioManager() {
    playing = false;
    volume = 100;
    current_host = nullptr;
    encoded_stream = nullptr;
    copier = nullptr;
    audio_task_handle = nullptr;
    task_running = false;
    lazy_initialized = false;
    pending_start = false;
    pending_url = "";
}

AudioManager::~AudioManager() {
    // Ensure proper cleanup on destruction
    stop();
    
    if (encoded_stream) {
        delete encoded_stream;
        encoded_stream = nullptr;
    }
    
#if AUDIO_DEBUG
    DEBUG_PRINTLN("[AUDIO] AudioManager destroyed and cleaned up");
#endif
}

void AudioManager::begin() {
    // Defer heavy initialization until first playback to save memory
#if AUDIO_DEBUG
    DEBUG_PRINTLN("[AUDIO] AudioManager ready (lazy initialization)");
#endif
}

// Dedicated audio task running on separate core for smooth playbook
void AudioManager::audioTaskFunction(void* parameter) {
    AudioManager* audioMgr = static_cast<AudioManager*>(parameter);
    
    if (!audioMgr) {
        vTaskDelete(nullptr);
        return;
    }
    
#if AUDIO_DEBUG
    DEBUG_PRINTLN("[AUDIO] Audio task started on core " + String(xPortGetCoreID()));
#endif
    
    while (audioMgr->task_running) {
        // Check if we have valid resources before processing
        if (audioMgr->playing && audioMgr->copier && audioMgr->encoded_stream) {
            try {
                // Process audio pipeline - non-blocking
                size_t bytes_processed = audioMgr->copier->copy();
                
                if (bytes_processed == 0) {
                    // No data processed, minimal delay to prevent buffer starvation
                    vTaskDelay(pdMS_TO_TICKS(1));
                } else {
                    // Successfully processed data, yield briefly
                    taskYIELD();
                }
                
                // Periodic yield to display without long delays
                static int yield_counter = 0;
                if (++yield_counter >= 50) {
                    yield_counter = 0;
                    vTaskDelay(pdMS_TO_TICKS(1)); // Minimal display yield
                }
                
            } catch (...) {
#if AUDIO_DEBUG
                DEBUG_PRINTLN("[AUDIO] Exception in audio processing, stopping");
#endif
                audioMgr->playing = false;
                break;
            }
        } else {
            // Not playing or missing resources, longer delay to save CPU
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        // Yield to other tasks regularly
        taskYIELD();
    }
    
#if AUDIO_DEBUG
    DEBUG_PRINTLN("[AUDIO] Audio task terminated");
#endif
    
    // Clean up task handle before exit
    audioMgr->audio_task_handle = nullptr;
    vTaskDelete(nullptr);
}

void AudioManager::loop() {
    // Handle deferred connection requests from UI callbacks
    if (pending_start && !pending_url.isEmpty()) {
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] Processing deferred connection request");
#endif
        pending_start = false;
        _internal_connecttohost(pending_url.c_str());
        pending_url = "";
    }
    
    // Start audio task if not already running and we have active playback
    if (playing && !task_running && encoded_stream && copier) {
        task_running = true;
        
        // Create audio task with stable priority settings
        BaseType_t result = xTaskCreatePinnedToCore(
            audioTaskFunction, 
            "AudioTask", 
            4096,      // Larger stack size (4KB)
            this, 
            2,         // Lower priority to avoid system conflicts
            &audio_task_handle, 
            1          // Pin to core 1 (separate from UI on core 0)
        );
        
        if (result != pdPASS) {
#if AUDIO_DEBUG
            DEBUG_PRINTLN("[AUDIO] ERROR: Failed to create audio task");
#endif
            task_running = false;
            audio_task_handle = nullptr;
        }
#if AUDIO_DEBUG
        else {
            DEBUG_PRINTLN("[AUDIO] Audio task created successfully");
        }
#endif
    }
    
    // Update playback info if necessary
    if (playing && current_host) {
        // Update playback info to show current playback status
        strncpy(g_playbackInfo.Album, "", sizeof(g_playbackInfo.Album) - 1);
        g_playbackInfo.Album[sizeof(g_playbackInfo.Album) - 1] = '\0';
        strncpy(g_playbackInfo.Title, "Playing...", sizeof(g_playbackInfo.Title) - 1);
        g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
        strncpy(g_playbackInfo.Artist, "", sizeof(g_playbackInfo.Artist) - 1);
        g_playbackInfo.Artist[sizeof(g_playbackInfo.Artist) - 1] = '\0';
        // AlarmTitle is left blank (only used when radio started from alarm)
    }
}

bool AudioManager::connecttohost(const char* host) {
    // Defer actual connection to main loop to avoid task creation in UI callback
    pending_start = true;
    pending_url = String(host);
    
#if AUDIO_DEBUG
    DEBUG_PRINTF("[AUDIO] Deferred connection request: %s\n", host);
#endif
    
    return true;
}

// Internal method for actual connection (called from main loop)
bool AudioManager::_internal_connecttohost(const char* host) {
    using namespace audio_tools;
    
    // Declare audio parameters at function scope for later I2S configuration
    int detected_sample_rate = 44100;     // Default fallback
    int detected_channels = 2;             // Default stereo
    int detected_bits_per_sample = 16;     // Default 16-bit
    
    // Stop current playback if running
    if (playing) {
        stop();
    }
    
    // Lazy initialization: Only initialize Arduino Audio Tools when needed
    if (!lazy_initialized) {
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] Performing lazy initialization of Arduino Audio Tools");
#endif

#if HEAP_DEBUG
        DEBUG_PRINTF("[HEAP] Before audio init - Free heap: %d, PSRAM: %d\n", 
                     ESP.getFreeHeap(), ESP.getFreePsram());
#endif
        
        // Note: PSRAM allocation will be handled through buffer size reduction and manual allocation
        
        // Ensure any existing I2S is properly cleaned up first
        i2s.end();
        
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] Configuring I2S with explicit resource management");
#endif
        
        // Configure I2S output stream with dynamic parameters and PSRAM allocation
        auto i2s_config = i2s.defaultConfig(TX_MODE);
        i2s_config.pin_bck = I2S_BCLK;
        i2s_config.pin_ws = I2S_LRCK;
        i2s_config.pin_data = I2S_DOUT;
        i2s_config.sample_rate = detected_sample_rate;      // Dynamic sample rate
        i2s_config.bits_per_sample = detected_bits_per_sample; // Dynamic bits per sample  
        i2s_config.channels = detected_channels;            // Dynamic channel count
        
        // Reduced buffer sizes to minimize heap pressure and force PSRAM usage
        i2s_config.buffer_size = 512;   // Smaller buffer size (512 bytes vs 1024)
        i2s_config.buffer_count = 6;    // More buffers for smoother playback (6 vs 4)
        i2s_config.use_apll = false;    // Disable APLL for stable timing
        i2s_config.auto_clear = true;
        i2s_config.port_no = 0;
        
#if AUDIO_DEBUG
        DEBUG_PRINTF("[AUDIO] Configuring I2S: %d Hz, %d-bit, %d channels\n", 
                     detected_sample_rate, detected_bits_per_sample, detected_channels);
#endif
        
        if (!i2s.begin(i2s_config)) {
#if AUDIO_DEBUG
            DEBUG_PRINTLN("[AUDIO] ERROR: Failed to initialize I2S stream");
#endif
            return false;
        }
        
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] I2S stream initialized successfully");
#endif
        
        // Setup VolumeStream
        volume_stream.begin();
        volume_stream.setOutput(i2s);
        volume_stream.setVolume((float)volume / 100.0);
        
        // Create EncodedAudioStream
        encoded_stream = new EncodedAudioStream(&volume_stream, &decoder);
        encoded_stream->begin();
        
#if HEAP_DEBUG
        DEBUG_PRINTF("[HEAP] After audio init - Free heap: %d, PSRAM: %d\n", 
                     ESP.getFreeHeap(), ESP.getFreePsram());
#endif
        
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] Lazy initialization completed");
#endif
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
    
#if AUDIO_DEBUG
    DEBUG_PRINTF("[AUDIO] Connecting to: %s\n", host);
#endif
    
    // Configure URLStream for HTTP streaming
    if (!url.begin(host, "audio/mp3")) {
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] ERROR: Failed to connect to URL stream");
#endif
        strncpy(g_playbackInfo.Title, "Connection Failed", sizeof(g_playbackInfo.Title) - 1);
        g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
        return false;
    }
    
    // Wait briefly for stream info and try to detect sample rate
    vTaskDelay(pdMS_TO_TICKS(200));
    
    // Get audio info from stream if available
    audio_tools::AudioInfo stream_info = url.audioInfo();
    
    if (stream_info.sample_rate > 0 && stream_info.sample_rate <= 48000) {
        detected_sample_rate = stream_info.sample_rate;
#if AUDIO_DEBUG
        DEBUG_PRINTF("[AUDIO] Detected stream sample rate: %d Hz\n", detected_sample_rate);
#endif
    } else {
#if AUDIO_DEBUG
        DEBUG_PRINTF("[AUDIO] Could not detect sample rate, using default: %d Hz\n", detected_sample_rate);
#endif
    }
    
    if (stream_info.channels > 0 && stream_info.channels <= 2) {
        detected_channels = stream_info.channels;
#if AUDIO_DEBUG
        DEBUG_PRINTF("[AUDIO] Detected stream channels: %d\n", detected_channels);
#endif
    } else {
#if AUDIO_DEBUG
        DEBUG_PRINTF("[AUDIO] Could not detect channels, using default: %d\n", detected_channels);
#endif
    }
    
    if (stream_info.bits_per_sample > 0 && stream_info.bits_per_sample <= 32) {
        detected_bits_per_sample = stream_info.bits_per_sample;
#if AUDIO_DEBUG
        DEBUG_PRINTF("[AUDIO] Detected stream bits per sample: %d\n", detected_bits_per_sample);
#endif
    } else {
#if AUDIO_DEBUG
        DEBUG_PRINTF("[AUDIO] Could not detect bits per sample, using default: %d\n", detected_bits_per_sample);
#endif
    }
    
    // Clean up any existing copier to prevent memory leak
    if (copier) {
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] Cleaning up existing StreamCopy");
#endif
        delete copier;
        copier = nullptr;
    }
    
#if HEAP_DEBUG
    DEBUG_PRINTF("[HEAP] Before StreamCopy - Free heap: %d, PSRAM: %d\n", 
                 ESP.getFreeHeap(), ESP.getFreePsram());
#endif
    
    // Create new StreamCopy to copy from URLStream to EncodedAudioStream
    copier = new StreamCopy(*encoded_stream, url);
    
#if HEAP_DEBUG
    DEBUG_PRINTF("[HEAP] After StreamCopy - Free heap: %d, PSRAM: %d\n", 
                 ESP.getFreeHeap(), ESP.getFreePsram());
#endif
    
#if AUDIO_DEBUG
    DEBUG_PRINTLN("[AUDIO] StreamCopy created successfully");
#endif
    
    // Start playback - set flag first before creating task
    playing = true;
    strncpy(g_playbackInfo.Title, "Playing...", sizeof(g_playbackInfo.Title) - 1);
    g_playbackInfo.Title[sizeof(g_playbackInfo.Title) - 1] = '\0';
    
    // Note: Audio task creation is handled in loop() method via deferred execution pattern
    // This avoids immediate task creation in UI callback context which can cause crashes
    
#if AUDIO_DEBUG
    DEBUG_PRINTLN("[AUDIO] Playback started successfully");
#endif
    
    return true;
}

void AudioManager::stop() {
    if (playing) {
        playing = false;
        
        // Stop the audio pipeline
        if (copier) {
            delete copier;
            copier = nullptr;
        }
        url.end();
        
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] Playback stopped");
#endif
    }
    
    // Stop audio task
    if (task_running) {
        task_running = false;
        
        // Wait for task to terminate (up to 1 second)
        if (audio_task_handle) {
            int timeout = 100; // 1 second timeout
            while (audio_task_handle && timeout > 0) {
                vTaskDelay(pdMS_TO_TICKS(10));
                timeout--;
            }
            
            // Force termination if still running
            if (audio_task_handle) {
#if AUDIO_DEBUG
                DEBUG_PRINTLN("[AUDIO] Force terminating audio task");
#endif
                vTaskDelete(audio_task_handle);
                audio_task_handle = nullptr;
            }
        }
        
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] Audio task stopped");
#endif
    }
    
    // Cleanup encoded_stream (critical - prevents freeze on second play)
    if (encoded_stream) {
#if AUDIO_DEBUG
        DEBUG_PRINTLN("[AUDIO] Cleaning up encoded_stream");
#endif
        delete encoded_stream;
        encoded_stream = nullptr;
    }
    
    // Explicitly cleanup I2S resources
    i2s.end();
    volume_stream.end();
    
#if AUDIO_DEBUG
    DEBUG_PRINTLN("[AUDIO] All audio resources cleaned up");
#endif
    
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
    // Update VolumeStream volume
    volume_stream.setVolume((float)volume / 100.0);
    
#if AUDIO_DEBUG
    DEBUG_PRINTF("[AUDIO] Volume set to %d%%\n", volume);
#endif
}

uint8_t AudioManager::getVolume() {
    return volume;
}

bool AudioManager::isPlaying() {
    return playing && url.available();
}
