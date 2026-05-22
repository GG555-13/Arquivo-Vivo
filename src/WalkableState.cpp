#include "WalkableState.h"

#include "Camera.h"
#include "Game.h"
#include "InputManager.h"
#include "MindPlaceState.h"

WalkableState::WalkableState() : State()
{
}

WalkableState::~WalkableState()
{
}

void WalkableState::Update(float dt)
{
    InputManager &input = InputManager::GetInstance();

    if (input.QuitRequested())
    {
        quitRequested = true;
    }

    if (ShouldCloseOnEscape() && input.KeyPress(ESCAPE_KEY))
    {
        popRequested = true;
    }

    if (CanOpenMindPlace() && input.KeyPress(MIND_PLACE_KEY))
    {
        Game::GetInstance().Push(new MindPlaceState());
        return;
    }

    Camera::Update(dt);
    UpdateWalkable(dt);
    UpdateArray(dt);
}

void WalkableState::UpdateWalkable(float dt)
{
    (void)dt;
}

bool WalkableState::CanOpenMindPlace() const
{
    return true;
}

bool WalkableState::ShouldCloseOnEscape() const
{
    return true;
}