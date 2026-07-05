#include "PlayerController.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Player.h"
#include "Camera.h"
#include "Vec2.h"
#include "SpriteRenderer.h"
#include "Game.h"
#include "DialogueBox.h" 

PlayerController::PlayerController(GameObject &associated) : Component(associated) {}

void PlayerController::Start() {}

void PlayerController::Update(float dt)
{
    (void)dt;

    Player *character = associated.GetComponent<Player>();
    if (!character) return;

    if (DialogueBox::isPlaying) 
    {
        return; 
    }

    InputManager &input = InputManager::GetInstance();
    State &currentState = Game::GetInstance().GetCurrentState();
    const Vec2 playerPos = character->GetPosition();

    if (input.KeyPress(SPACE_KEY))
    {
        if (currentState.ActivateActorInteractable(playerPos))
        {
            return;
        }
    }

    // Mouse
    if (input.MousePress(LEFT_MOUSE_BUTTON))
    {
        float mouseWorldX = input.GetMouseX() + Camera::pos.x;
        float mouseWorldY = input.GetMouseY() + Camera::pos.y;
        Vec2 mouseWorldPoint(mouseWorldX, mouseWorldY);

        if (currentState.ActivateInteractableAtPoint(mouseWorldPoint, playerPos))
        {
            return;
        }
        
        // determina até aonde o protagonista anda, verificar o valor e mander constante nos cenário 
        // ter certeza que tem o mesmo valor que em character.cpp 
        float minY = 700.0f; 
        float maxY = 850.0f; 
        
        if (mouseWorldY < minY) mouseWorldY = minY;
        if (mouseWorldY > maxY) mouseWorldY = maxY;

        if (input.GetMouseX() > 1150) mouseWorldX = Camera::stageWidth; 
        if (input.GetMouseX() < 50) mouseWorldX = 0.0f;

        character->Issue(Character::Command(Character::Command::MOVE_TARGET, mouseWorldX, mouseWorldY));

        std::shared_ptr<GameObject> existingMarker = character->GetTargetIndicator().lock();
        
        if (!existingMarker)
        {
            GameObject* clickFeedback = new GameObject();
            SpriteRenderer* sprite = new SpriteRenderer(*clickFeedback, "recursos/img/interactible_indicator.png");
            sprite->SetScale(0.1f, 0.1f);
            sprite->SetUseSourceFrameOffset(false);
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