#include "TransitionTrigger.h"
#include "Player.h"
#include "Game.h"
#include "StageState.h"
#include "InputManager.h"
#include "Camera.h"
#include "GameData.h" 
#include "EndState.h"

TransitionTrigger::TransitionTrigger(GameObject &associated, std::string targetStageId)
    : Component(associated), targetStageId(targetStageId) {}

void TransitionTrigger::Update(float dt) {}
void TransitionTrigger::Render() {
    // RENDERIZAR HITBOX
    SDL_Rect rect;
    rect.x = associated.box.x - Camera::pos.x;
    rect.y = associated.box.y - Camera::pos.y;
    rect.w = associated.box.w;
    rect.h = associated.box.h;

    SDL_Renderer* renderer = Game::GetInstance().GetRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Verde
    SDL_RenderDrawRect(renderer, &rect);
}

void TransitionTrigger::NotifyCollision(GameObject& other) {
    Player* interactingPlayer = other.GetComponent<Player>();
    
    if (interactingPlayer != nullptr) {
        InputManager& input = InputManager::GetInstance();
        
        bool pressedSpace = input.KeyPress(SPACE_KEY);
        
        float mouseWorldX = input.GetMouseX() + Camera::pos.x;
        float mouseWorldY = input.GetMouseY() + Camera::pos.y;
        
        bool clickedDoor = input.MousePress(LEFT_MOUSE_BUTTON) && 
                           (mouseWorldX >= associated.box.x && mouseWorldX <= associated.box.x + associated.box.w &&
                            mouseWorldY >= associated.box.y && mouseWorldY <= associated.box.y + associated.box.h);

        if (pressedSpace || clickedDoor) {
            StageState* currentStage = static_cast<StageState*>(&Game::GetInstance().GetCurrentState());
            if (currentStage != nullptr) {
                currentStage->TransitionTo(targetStageId);
            }
        }
    }
}
