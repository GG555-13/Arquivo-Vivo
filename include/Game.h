#ifndef GAME_H
#define GAME_H

#include <string>
#include <stack>
#include <memory> 
#include "SDL_include.h"
#include "State.h"

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

    int frameStart;
    float dt;
    int width;
    int height;
    std::string title;
};

#endif