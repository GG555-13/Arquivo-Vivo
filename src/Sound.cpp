#include "Sound.h"
#include <iostream>
#include "Resources.h"

Sound::Sound() : chunk(nullptr), channel(-1)
{
}

Sound::Sound(const std::string &file) : chunk(nullptr), channel(-1)
{
    Open(file);
}

Sound::~Sound()
{

}

void Sound::Open(const std::string &file)
{
    chunk = Resources::GetSound(file);
    if (!chunk)
    {
        std::cerr << "Falha ao carregar o som (Resources): " << file << std::endl;
    }
}

void Sound::Play(int times)
{
    if (chunk != nullptr)
    {
        channel = Mix_PlayChannel(-1, chunk.get(), times - 1);
    }
}

void Sound::Stop()
{

    if (chunk != nullptr && channel != -1)
    {
        Mix_HaltChannel(channel);
    }
}

bool Sound::IsOpen() const
{
    return chunk != nullptr;
}