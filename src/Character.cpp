#include "Character.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Game.h"
#include "Collider.h"
#include "Camera.h"

Character::Character(GameObject &associated)
    : Component(associated), linearSpeed(1050.0f), isMoving(false), flipped(false), isMovingToTarget(false) 
{                                       // padrão 450
    currentAnim = "idle";
    associated.AddComponent(new Collider(associated));
}

Character::~Character()
{
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
            OnDirectionInput();
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
            OnArrivedAtTarget();
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

    ConstrainPosition();

    if (speed.x < 0) flipped = true;
    else if (speed.x > 0) flipped = false;

    UpdateAnimation();

    SpriteRenderer *sr = associated.GetComponent<SpriteRenderer>();
    if (sr) sr->SetFlip(flipped ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
}

void Character::UpdateAnimation()
{
    std::string expectedAnim = isMoving ? "walking" : "idle";
    if (expectedAnim != currentAnim)
    {
        currentAnim = expectedAnim;
        Animator *animator = associated.GetComponent<Animator>();
        if (animator) animator->SetAnimation(currentAnim);
    }
}

void Character::ConstrainPosition()
{
    if (!boundsEnabled) return;

    Vec2 center = associated.box.Center();

    // MANTER AS COORDENADAS Y dentro da imagem do fundo
    if (floorMinY > 0.0f && center.y < floorMinY) center.y = floorMinY;
    if (floorMaxY > 0.0f && center.y > floorMaxY) center.y = floorMaxY;

    // MANTER AS COORDENADAS X dentro da imagem do fundo
    if (boundMinX > 0.0f && center.x < boundMinX) center.x = boundMinX;
    if (boundMaxX > 0.0f && center.x > boundMaxX) center.x = boundMaxX;

    associated.box.SetCenter(center);
}

void Character::OnDirectionInput()
{
}

void Character::OnArrivedAtTarget()
{
}

void Character::SetBounds(float minY, float maxY, float minX, float maxX)
{
    floorMinY = minY;
    floorMaxY = maxY;
    boundMinX = minX;
    boundMaxX = maxX;
}

void Character::EnableBounds(bool enable)
{
    boundsEnabled = enable;
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

bool Character::IsFlipped() const
{
    return flipped;
}

Vec2 Character::GetPosition() const
{
    return associated.box.Center();
}