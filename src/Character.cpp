#include "Character.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Game.h"
#include "Collider.h"
#include "Camera.h"

Character *Character::player = nullptr;

Character::Character(GameObject &associated, const std::string &spritePath)
    : Component(associated), linearSpeed(450.0f), isMoving(false), flipped(false), isMovingToTarget(false) 
{
    SpriteRenderer* sr = new SpriteRenderer(associated, spritePath, 3, 2);
    sr->SetUseSourceFrameOffset(false);
    // sr->SetScale(3.8f, 3.8f);
    sr->SetScale(0.3f, 0.3f);

    associated.AddComponent(sr);

    Animator *animator = new Animator(associated);

    // animator->AddAnimation("idle", Animation(9, 9, 0));
    // animator->AddAnimation("walking", Animation(1, 8, 0.1f));
    // animator->SetAnimation("idle");
    // associated.AddComponent(animator);

    animator->AddAnimation("idle", Animation(4, 4, 0));
    animator->AddAnimation("walking", Animation(0, 3, 0.1f));
    animator->SetAnimation("idle");
    associated.AddComponent(animator);
    
    currentAnim = "idle"; 

    if (player == nullptr)
    {
        player = this;
    }
    
    associated.box.w = associated.GetComponent<SpriteRenderer>()->GetWidth();
    associated.box.h = associated.GetComponent<SpriteRenderer>()->GetHeight();
    associated.AddComponent(new Collider(associated));
}

Character::~Character()
{
    if (player == this)
    {
        player = nullptr;
    }
}

void Character::Start()
{
}

void Character::Update(float dt)
{
    isMoving = false;
    speed = Vec2(0, 0);

    while (!taskQueue.empty())
    {
        Command currentTask = taskQueue.front();
        
        if (currentTask.type == Command::MOVE_TARGET)
        {
            isMovingToTarget = true;
            targetPos = currentTask.pos;
        }
        else if (currentTask.type == Command::MOVE_DIR)
        {
            isMovingToTarget = false; 
            
            if (std::shared_ptr<GameObject> ind = targetIndicator.lock()) {
                ind->box.x = -10000; 
                ind->box.y = -10000;
            }

            speed.x += currentTask.pos.x * linearSpeed;
            if (currentTask.pos.x != 0) isMoving = true;
        }
        taskQueue.pop();
    }

    if (isMovingToTarget)
    {
        Vec2 currentPos = GetPosition();
        float distance = currentPos.Distance(targetPos);
        
        if (distance <= (linearSpeed * dt) + 2.0f) 
        {
            associated.box.x = targetPos.x - associated.box.w / 2;
            associated.box.y = targetPos.y - associated.box.h / 2;
            isMovingToTarget = false;

            if (std::shared_ptr<GameObject> ind = targetIndicator.lock()) {
                ind->box.x = -10000;
                ind->box.y = -10000;
            }
        }
        else
        {
            isMoving = true;
            Vec2 direction = (targetPos - currentPos).Normalized();
            speed = direction * linearSpeed;
            associated.box.x += speed.x * dt;
            associated.box.y += speed.y * dt;
        }
    }
    else
    {
        associated.box.x += speed.x * dt;
        associated.box.y += speed.y * dt;
    }

    if (speed.x < 0) flipped = true;
    else if (speed.x > 0) flipped = false;

    // ter certeza que os valores são os mesmos no PlayerController 
    float floorMinY = 700.0f;
    float floorMaxY = 850.0f;

    Vec2 center = associated.box.Center();

    // MANTER AS COORDENADAS Y dentro da imagem do fundo
    if (center.y < floorMinY) center.y = floorMinY;
    if (center.y > floorMaxY) center.y = floorMaxY;

    // MANTER AS COORDENADAS X dentro da imagem do fundo
    if (center.x < 0.0f) center.x = 0.0f;
    if (center.x > Camera::stageWidth) center.x = Camera::stageWidth; 

    associated.box.SetCenter(center);

    if (speed.x < 0) flipped = true;
    else if (speed.x > 0) flipped = false;

    std::string expectedAnim = isMoving ? "walking" : "idle";
    if (expectedAnim != currentAnim)
    {
        currentAnim = expectedAnim;
        Animator *animator = associated.GetComponent<Animator>();
        if (animator) animator->SetAnimation(currentAnim);
    }

    SpriteRenderer *sr = associated.GetComponent<SpriteRenderer>();
    if (sr) sr->SetFlip(flipped ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
}

void Character::SetTargetIndicator(std::weak_ptr<GameObject> indicator)
{
    targetIndicator = indicator;
}

std::weak_ptr<GameObject> Character::GetTargetIndicator() const
{
    return targetIndicator;
}

void Character::NotifyCollision(GameObject & other)
{
    
}

void Character::Render()
{
}

void Character::Issue(Command task)
{
    taskQueue.push(task);
}

void Character::SetFlip(bool flipState)
{
    flipped = flipState;
}

Vec2 Character::GetPosition() const
{
    return associated.box.Center();
}