#ifndef SOUND_H
#define SOUND_H

#include <string>
#include "SDL_include.h"
#include <memory>

class Sound {
public:
    Sound();
    explicit Sound(const std::string& file);
    ~Sound();

    void Open(const std::string& file);
    void Play(int times = 1);
    void Stop();
    bool IsOpen() const;

private:
    std::shared_ptr<Mix_Chunk> chunk;
    int channel;
};

#endif 