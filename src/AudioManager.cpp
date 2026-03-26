#include "AudioManager.hpp"
#include <iostream>

AudioManager::AudioManager() : initialized(false) {}

AudioManager::~AudioManager() {
    if (initialized) {
        // uninitialize sounds first
        for (auto& pair : sounds) {
            ma_sound_uninit(&pair.second);
        }
        ma_engine_uninit(&engine);
    }
}

bool AudioManager::init() {
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cout << "AUDIO ERROR: Could not start engine! Code: " << result << std::endl;
        return false;
    }
    std::cout << "Audio Engine Started Successfully." << std::endl;
    initialized = true;
    return true;
}

void AudioManager::playSound(const std::string& filePath) {
    // simple one play
    ma_engine_play_sound(&engine, filePath.c_str(), NULL);
}

void AudioManager::playMusic(const std::string& filePath, bool loop) {

    if (sounds.find(filePath) == sounds.end()) {
        ma_sound_init_from_file(&engine, filePath.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, &sounds[filePath]);
        if (result != MA_SUCCESS) {
            std::cout << "AUDIO ERROR: Could not load file: " << filePath << " | Error code: " << result << std::endl;
            return;
        }

    }
    ma_sound_set_looping(&sounds[filePath], loop);
    ma_sound_start(&sounds[filePath]);
}

void AudioManager::stopMusic(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        ma_sound_stop(&it->second); // stops the miniaudio sound object
    }
}