#ifndef MUSIC_H
#define MUSIC_H

#include <string>
#include "SDL_include.h"
#include <memory>

class Music
{
public:
    Music();
    explicit Music(const std::string &file);
    ~Music();

    void Open(std::string file);
    void Play(int loops = -1) const;
    void Stop(int msToStop = 1500) const;
    bool IsOpen() const;

private:
    std::shared_ptr<Mix_Music> music;
};

#endif
