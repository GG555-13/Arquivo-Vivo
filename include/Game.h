#ifndef GAME_H
#define GAME_H

#include <string>
#include <stack>
#include <memory> 
#include "SDL_include.h"
#include "State.h"
#include "FadeTypes.h"

class Game
{
public:
    ~Game();
    void Run();
    SDL_Renderer *GetRenderer() const;
    State& GetCurrentState() const; 

    static Game &GetInstance(const std::string &title, int width, int height);
    static Game &GetInstance();
    float GetDeltaTime() const;

    void Push(State* state);

    void SetPendingFadeIn(const PendingFadeIn& pf);
    PendingFadeIn ConsumePendingFadeIn();

    int GetWidth() const;
    int GetHeight() const;

private:
    Game(const std::string &title, int width, int height);
    void CalculateDeltaTime();

    static Game *instance;
    SDL_Window *window;
    SDL_Renderer *renderer;

    State* storedState;
    std::stack<std::unique_ptr<State>> stateStack;
    PendingFadeIn pendingFadeIn;

    int frameStart;
    float dt;
    int width;
    int height;
    std::string title;
};

#endif