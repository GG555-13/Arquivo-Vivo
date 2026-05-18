#include "Resources.h"
#include "Game.h"
#include <iostream>

std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> Resources::imageTable;
std::unordered_map<std::string, std::shared_ptr<Mix_Music>> Resources::musicTable;
std::unordered_map<std::string, std::shared_ptr<Mix_Chunk>> Resources::soundTable;
std::unordered_map<std::string, std::shared_ptr<TTF_Font>> Resources::fontTable;

std::shared_ptr<SDL_Texture> Resources::GetImage(std::string file)
{

    if (imageTable.find(file) != imageTable.end())
    {
        return imageTable[file];
    }

    SDL_Texture *texture = IMG_LoadTexture(Game::GetInstance().GetRenderer(), file.c_str());
    if (texture == nullptr)
    {
        std::cout << "Erro ao carregar imagem: " << file << " " << IMG_GetError() << std::endl;
        return nullptr;
    }

    std::shared_ptr<SDL_Texture> ptr(texture, [](SDL_Texture *t)
                                     { SDL_DestroyTexture(t); });

    imageTable[file] = ptr;
    return ptr;
}

void Resources::ClearImages()
{
    for (auto it = imageTable.begin(); it != imageTable.end();)
    {
        if (it->second.use_count() == 1)
        {
            it = imageTable.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::shared_ptr<Mix_Music> Resources::GetMusic(std::string file)
{
    if (musicTable.find(file) != musicTable.end())
    {
        return musicTable[file];
    }

    Mix_Music *music = Mix_LoadMUS(file.c_str());
    if (music == nullptr)
    {
        std::cout << "Erro ao carregar musica: " << file << " " << Mix_GetError() << std::endl;
        return nullptr;
    }

    std::shared_ptr<Mix_Music> ptr(music, [](Mix_Music *m)
                                   { Mix_FreeMusic(m); });

    musicTable[file] = ptr;
    return ptr;
}

void Resources::ClearMusics()
{
    for (auto it = musicTable.begin(); it != musicTable.end();)
    {
        if (it->second.use_count() == 1)
        {
            it = musicTable.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::shared_ptr<Mix_Chunk> Resources::GetSound(std::string file)
{
    if (soundTable.find(file) != soundTable.end())
    {
        return soundTable[file];
    }

    Mix_Chunk *chunk = Mix_LoadWAV(file.c_str());
    if (chunk == nullptr)
    {
        std::cout << "Erro ao carregar som: " << file << " " << Mix_GetError() << std::endl;
        return nullptr;
    }

    std::shared_ptr<Mix_Chunk> ptr(chunk, [](Mix_Chunk *s)
                                   { Mix_FreeChunk(s); });

    soundTable[file] = ptr;
    return ptr;
}

void Resources::ClearSounds()
{
    for (auto it = soundTable.begin(); it != soundTable.end();)
    {
        if (it->second.use_count() == 1)
        {
            it = soundTable.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::shared_ptr<TTF_Font> Resources::GetFont(std::string file, int fontSize) {
    std::string key = file + std::to_string(fontSize);

    if (fontTable.find(key) != fontTable.end()) {
        return fontTable[key];
    }

    TTF_Font* font = TTF_OpenFont(file.c_str(), fontSize);
    if (font == nullptr) {
        std::cout << "Erro ao carregar fonte: " << file << " " << TTF_GetError() << std::endl;
        return nullptr;
    }

    std::shared_ptr<TTF_Font> ptr(font, [](TTF_Font* f) {
        TTF_CloseFont(f);
    });

    fontTable[key] = ptr;
    return ptr;
}

void Resources::ClearFonts() {
    for (auto it = fontTable.begin(); it != fontTable.end(); ) {
        if (it->second.use_count() == 1) {
            it = fontTable.erase(it);
        } else {
            ++it;
        }
    }
}