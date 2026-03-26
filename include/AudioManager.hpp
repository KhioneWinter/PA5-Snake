#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

#include "miniaudio.h"
#include <string>
#include <map>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool init();
    void playSound(const std::string& name);
    void playMusic(const std::string& name, bool loop = true);
    void stopAll();

private:
    ma_engine engine;
    std::map<std::string, ma_sound> sounds;
    bool initialized = false;
};

#endif