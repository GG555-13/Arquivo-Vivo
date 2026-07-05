#include "Animator.h"
#include "GameObject.h"
#include "SpriteRenderer.h"

Animator::Animator(GameObject &associated) : Component(associated), currentAnimation("") { 
    frameStart = 0;
    frameEnd = 0;
    frameTime = 0;
    currentFrame = 0;
    timeElapsed = 0;
}

void Animator::AddAnimation(const std::string &name, const Animation &anim)
{
    if (animations.find(name) == animations.end())
    {
        animations.emplace(name, anim);
    }
}

void Animator::SetAnimation(const std::string &name)
{
    if (name != currentAnimation) {
        auto it = animations.find(name);
        if (it != animations.end())
        {
            const auto &anim = it->second;
            frameStart = anim.frameStart;
            frameEnd = anim.frameEnd;
            frameTime = anim.frameTime;

            currentFrame = frameStart;
            timeElapsed = 0;
            currentAnimation = name;

            SpriteRenderer *sprite = associated.GetComponent<SpriteRenderer>();
            if (sprite)
            {
                sprite->SetFrame(currentFrame, anim.flip); 
            }
        }
    }
}

void Animator::Update(float dt)
{
    if (frameTime == 0) {
        return;
    }
    timeElapsed += dt;
    if (timeElapsed > frameTime) {
        timeElapsed -= frameTime;
        currentFrame++;
        if (currentFrame > frameEnd) {
            currentFrame = frameStart;
        }

        SpriteRenderer *sprite = associated.GetComponent<SpriteRenderer>();
        if (sprite) {
            auto it = animations.find(currentAnimation);
            if(it != animations.end()) {
                sprite->SetFrame(currentFrame, it->second.flip); 
            } else {
                 sprite->SetFrame(currentFrame); 
            }
        }
    }
}

void Animator::Render()
{
}