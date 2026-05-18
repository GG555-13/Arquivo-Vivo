#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <SDL_include.h>
#include "StageState.h"
#include "Resources.h"
#include "InputManager.h"
#include <cstdlib>
#include <ctime>

Game *Game::instance = nullptr;

Game &Game::GetInstance(const std::string &title, int width, int height)
{
    if (instance == nullptr)
    {
        instance = new Game(title, width, height);
    }
    return *instance;
}

Game &Game::GetInstance()
{
    if (instance == nullptr)
    {
        throw std::runtime_error("não inicializado");
    }
    return *instance;
}

Game::Game(const std::string &title_, int width_, int height_)
    : window(nullptr), renderer(nullptr), storedState(nullptr), width(width_), height(height_), frameStart(0), dt(0), title(title_)
{
    if (instance != nullptr)
    {
        throw std::runtime_error("Apenas uma instância");
    }
    instance = this;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
    {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        std::exit(1);
    }

    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
    {
        std::cerr << "IMG_Init warning: " << IMG_GetError() << std::endl;
    }

    int mixFlags = MIX_INIT_OGG | MIX_INIT_MP3;
    if ((Mix_Init(mixFlags) & mixFlags) != mixFlags)
    {
        std::cerr << "Mix_Init warning: " << std::endl;
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
    {
        std::cerr << "Mix_OpenAudio : " << Mix_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        std::exit(1);
    }
    Mix_AllocateChannels(32);

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow : " << SDL_GetError() << std::endl;
        Mix_CloseAudio();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        std::exit(1);
    }

    if (TTF_Init() != 0) 
    {
    std::cerr << "TTF_Init error: " << TTF_GetError() << std::endl;
    std::exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer : " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        std::exit(1);
    }

    storedState = nullptr;

    srand(time(NULL));
}

Game::~Game()
{
    if (storedState != nullptr)
    {
        delete storedState;
    }
    while (!stateStack.empty())
    {
        stateStack.pop();
    }

    Resources::ClearImages();
    Resources::ClearSounds();
    Resources::ClearMusics();

    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    instance = nullptr;
}

State &Game::GetCurrentState() const
{
    return *stateStack.top();
}

void Game::Push(State *state)
{
    storedState = state;
}

void Game::Run()
{
    if (storedState != nullptr)
    {
        stateStack.emplace(storedState);
        stateStack.top()->Start();
        storedState = nullptr;
    }

    while (!stateStack.empty() && !stateStack.top()->QuitRequested())
    {
        if (stateStack.top()->PopRequested())
        {
            stateStack.pop();
            Resources::ClearImages();
            Resources::ClearSounds();
            Resources::ClearMusics();

            if (!stateStack.empty())
            {
                stateStack.top()->Resume();
            }
        }

        if (storedState != nullptr)
        {
            if (!stateStack.empty())
            {
                stateStack.top()->Pause();
            }
            stateStack.emplace(storedState);
            stateStack.top()->Start();
            storedState = nullptr;
        }

        if (stateStack.empty())
        {
            break;
        }

        CalculateDeltaTime();
        InputManager::GetInstance().Update();

        stateStack.top()->Update(GetDeltaTime());

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        stateStack.top()->Render();

        SDL_RenderPresent(renderer);
        SDL_Delay(33);
    }
}
void Game::CalculateDeltaTime()
{
    Uint32 currentTime = SDL_GetTicks();
    dt = (currentTime - frameStart) / 1000.0f;
    frameStart = currentTime;
}

float Game::GetDeltaTime() const
{
    return dt;
}

SDL_Renderer *Game::GetRenderer() const
{
    return renderer;
}

int Game::GetWidth() const
{
    return width;
}

int Game::GetHeight() const
{
    return height;
}