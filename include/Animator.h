#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Component.h"
#include "Animation.h"
#include <string>
#include <unordered_map>

class Animator : public Component
{
public:
    explicit Animator(GameObject &associated);

    void AddAnimation(const std::string &name, const Animation &anim);
    void SetAnimation(const std::string &name);

    void Update(float dt) override;
    void Render() override;

private:
    std::unordered_map<std::string, Animation> animations;
    int frameStart;
    int frameEnd;
    float frameTime;
    int currentFrame;
    float timeElapsed;
    std::string currentAnimation;
};

#endif