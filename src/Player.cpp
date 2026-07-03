#include "Player.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Camera.h"

Player *Player::player = nullptr;

Player::Player(GameObject &associated)
    : Character(associated)
{
    if (player == nullptr)
    {
        player = this;
    }

    // Default bounds matching the old hardcoded values
    boundsEnabled = true;
    floorMinY = 700.0f;
    floorMaxY = 850.0f;
    boundMinX = 0.0f;
    // boundMaxX is set dynamically from Camera::stageWidth in ConstrainPosition

    SetupSprite();
}

Player::~Player()
{
    if (player == this)
    {
        player = nullptr;
    }
}

void Player::SetupSprite()
{
    SpriteRenderer* sr = new SpriteRenderer(associated, "recursos/img/Protagonista.png", 3, 2);
    sr->SetUseSourceFrameOffset(false);
    sr->SetScale(0.3f, 0.3f);
    sr->SetRenderOffsetY(-177);
    associated.AddComponent(sr);

    Animator *animator = new Animator(associated);
    animator->AddAnimation("idle", Animation(4, 4, 0));
    animator->AddAnimation("walking", Animation(0, 3, 0.1f));
    animator->SetAnimation("idle");
    associated.AddComponent(animator);

    associated.box.w = sr->GetWidth();
    associated.box.h = sr->GetHeight();
}

void Player::ConstrainPosition()
{
    if (!boundsEnabled) return;

    Vec2 center = associated.box.Center();

    if (center.y < floorMinY) center.y = floorMinY;
    if (center.y > floorMaxY) center.y = floorMaxY;

    if (center.x < boundMinX) center.x = boundMinX;
    if (center.x > Camera::stageWidth) center.x = Camera::stageWidth;

    associated.box.SetCenter(center);
}

void Player::OnDirectionInput()
{
    if (std::shared_ptr<GameObject> ind = targetIndicator.lock()) {
        ind->box.x = -10000; 
        ind->box.y = -10000;
    }
}

void Player::OnArrivedAtTarget()
{
    if (std::shared_ptr<GameObject> ind = targetIndicator.lock()) {
        ind->box.x = -10000;
        ind->box.y = -10000;
    }
}

void Player::SetTargetIndicator(std::weak_ptr<GameObject> indicator)
{
    targetIndicator = indicator;
}

std::weak_ptr<GameObject> Player::GetTargetIndicator() const
{
    return targetIndicator;
}
