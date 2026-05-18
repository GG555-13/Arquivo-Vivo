#include "PlayerController.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Character.h"
#include "Camera.h"
#include "Vec2.h"
#include "SpriteRenderer.h"
#include "Game.h"
#include "State.h"

PlayerController::PlayerController(GameObject &associated) : Component(associated) {}

void PlayerController::Start() {}

void PlayerController::Update(float dt)
{
    Character *character = associated.GetComponent<Character>();
    if (!character) return;

    InputManager &input = InputManager::GetInstance();

    // Mouse
    if (input.MousePress(LEFT_MOUSE_BUTTON))
    {

        float mouseWorldX = input.GetMouseX() + Camera::pos.x;
        float mouseWorldY = input.GetMouseY() + Camera::pos.y;
        
        // determina até aonde o protagonista anda, verificar o valor e mander constante nos cenário 
        // ter certeza que tem o mesmo valor que em character.cpp 
        float minY = 700.0f; 
        float maxY = 850.0f; 
        
        if (mouseWorldY < minY) mouseWorldY = minY;
        if (mouseWorldY > maxY) mouseWorldY = maxY;

        if (input.GetMouseX() > 1150) mouseWorldX = 1926.0f; 
        if (input.GetMouseX() < 50) mouseWorldX = 0.0f;

        character->Issue(Character::Command(Character::Command::MOVE_TARGET, mouseWorldX, mouseWorldY));

        std::shared_ptr<GameObject> existingMarker = character->GetTargetIndicator().lock();
        
        if (!existingMarker)
        {
            GameObject* clickFeedback = new GameObject();
            SpriteRenderer* sprite = new SpriteRenderer(*clickFeedback, "recursos/img/Bullet.png");
            clickFeedback->AddComponent(sprite);
            
            clickFeedback->box.w = sprite->GetWidth();
            clickFeedback->box.h = sprite->GetHeight();
            clickFeedback->box.x = mouseWorldX - clickFeedback->box.w / 2.0f;
            clickFeedback->box.y = mouseWorldY - clickFeedback->box.h / 2.0f;
            
            std::weak_ptr<GameObject> targetPtr = Game::GetInstance().GetCurrentState().AddObject(clickFeedback);
            character->SetTargetIndicator(targetPtr);
        }
        else
        {
            existingMarker->box.x = mouseWorldX - existingMarker->box.w / 2.0f;
            existingMarker->box.y = mouseWorldY - existingMarker->box.h / 2.0f;
        }
    }

    //Keyboard
    if (input.IsKeyDown(SDLK_a) || input.IsKeyDown(LEFT_ARROW_KEY))
    {
        character->Issue(Character::Command(Character::Command::MOVE_DIR, -1.0f, 0.0f));
    }
    else if (input.IsKeyDown(SDLK_d) || input.IsKeyDown(RIGHT_ARROW_KEY))
    {
        character->Issue(Character::Command(Character::Command::MOVE_DIR, 1.0f, 0.0f));
    }
}

void PlayerController::Render() {}