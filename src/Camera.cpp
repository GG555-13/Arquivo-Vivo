#include "Camera.h"
#include "InputManager.h"
#include "Game.h"
#include "GameObject.h" 

Vec2 Camera::pos(0, 0);
Vec2 Camera::speed(0, 0);
float Camera::stageWidth = 1926.0f;
GameObject *Camera::focus = nullptr;

void Camera::Follow(GameObject *newFocus)
{
    focus = newFocus;
}

void Camera::Unfollow()
{
    focus = nullptr;
}

void Camera::Update(float dt)
{
    if (focus != nullptr)
    {
        pos.x = focus->box.Center().x - (1200.0f / 2.0f);
        pos.y = 0.0f; 

        //configurar a dimensão do fundo
        float windowWidth = 1200.0f; 
        float maxCameraX = stageWidth - windowWidth;

        if (pos.x < 0) 
        {
            pos.x = 0;
        }
        if (pos.x > maxCameraX) 
        {
            pos.x = maxCameraX;
        }
    }
    else
    {
        const float cameraSpeed = 300.0f;

        if (InputManager::GetInstance().IsKeyDown(LEFT_ARROW_KEY))
        {
            pos.x -= cameraSpeed * dt;
        }
        if (InputManager::GetInstance().IsKeyDown(RIGHT_ARROW_KEY))
        {
            pos.x += cameraSpeed * dt;
        }
        if (InputManager::GetInstance().IsKeyDown(UP_ARROW_KEY))
        {
            pos.y -= cameraSpeed * dt;
        }
        if (InputManager::GetInstance().IsKeyDown(DOWN_ARROW_KEY))
        {
            pos.y += cameraSpeed * dt;
        }
    }
}