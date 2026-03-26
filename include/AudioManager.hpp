#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

#include "miniaudio.h"
#include <string>
#include <map>
#include <iostream>
#include <string>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool init();
    void playSound(const std::string& name);
    void playMusic(const std::string& name, bool loop = true);
    void stopMusic(const std::string& name);

    // paths to sounds
    std::string loseSoundPath = "assets/lose.wav";
    std::string eatFoodSoundPath = "assets/eat-fruit.wav";
    std::string powerupSoundPath = "assets/power-up.wav";
    std::string selectSoundPath = "assets/select.wav";

    std::string backgroundMusicPath = "assets/background-music.mp3";


private:
    ma_engine engine;
    std::map<std::string, ma_sound> sounds;
    bool initialized = false;
};

#endif