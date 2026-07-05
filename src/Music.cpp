#include "Music.h"
#include <iostream>
#include "Resources.h"

Music::Music() : music(nullptr) {}

Music::Music(const std::string &file) : music(nullptr)
{
    Open(file);
}

Music::~Music() {
    Stop(0);
}

void Music:: Open (std::string file) {
    music = Resources::GetMusic(file);
}

void Music::Play(int loops) const
{
    if (music)
    {
        Mix_PlayMusic(music.get(), loops);
    }
}

void Music::Stop(int msToStop) const
{
    Mix_FadeOutMusic(msToStop);
}

bool Music::IsOpen() const { return music != nullptr; }
