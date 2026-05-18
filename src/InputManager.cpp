#include "InputManager.h"

InputManager::InputManager() : quitRequested(false), updateCounter(0), mouseX(0), mouseY(0)
{
    for (int i = 0; i < 6; ++i)
    {
        mouseState[i] = false;
        mouseUpdate[i] = 0;
    }
}

InputManager &InputManager::GetInstance()
{
    static InputManager instance;
    return instance;
}

void InputManager::Update()
{
    SDL_Event event;

    quitRequested = false;
    updateCounter++;

    SDL_GetMouseState(&mouseX, &mouseY);

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            quitRequested = true;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            mouseState[event.button.button] = true;
            mouseUpdate[event.button.button] = updateCounter;
        }

        if (event.type == SDL_MOUSEBUTTONUP)
        {
            mouseState[event.button.button] = false;
            mouseUpdate[event.button.button] = updateCounter;
        }

        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.repeat == 0)
            {
                keyState[event.key.keysym.sym] = true;
                keyUpdate[event.key.keysym.sym] = updateCounter;
            }
        }

        if (event.type == SDL_KEYUP)
        {
            keyState[event.key.keysym.sym] = false;
            keyUpdate[event.key.keysym.sym] = updateCounter;
        }
    }
}

bool InputManager::KeyPress(int key)
{
    return keyState.count(key) && keyState.at(key) && keyUpdate.at(key) == updateCounter;
}

bool InputManager::KeyRelease(int key)
{
    return keyState.count(key) && !keyState.at(key) && keyUpdate.at(key) == updateCounter;
}

bool InputManager::IsKeyDown(int key)
{
    return keyState.count(key) && keyState.at(key);
}

bool InputManager::MousePress(int button)
{
    return mouseState[button] && mouseUpdate[button] == updateCounter;
}

bool InputManager::MouseRelease(int button)
{
    return !mouseState[button] && mouseUpdate[button] == updateCounter;
}

bool InputManager::IsMouseDown(int button)
{
    return mouseState[button];
}

int InputManager::GetMouseX() const
{
    return mouseX;
}

int InputManager::GetMouseY() const
{
    return mouseY;
}

bool InputManager::QuitRequested() const
{
    return quitRequested;
}