#ifndef CHARACTER_H
#define CHARACTER_H

#include "Component.h"
#include "Vec2.h"
#include <string>
#include <queue>
#include <memory>

class GameObject;

class Character : public Component
{
public:
    class Command
    {
    public:
        enum CommandType
        {
            MOVE_TARGET, 
            MOVE_DIR     
        };
        CommandType type;
        Vec2 pos;

        Command(CommandType type, float x, float y) : type(type), pos(x, y) {}
    };

    Character(GameObject &associated, const std::string &spritePath);
    ~Character();

    void Start() override;
    void Update(float dt) override;
    void Issue(Command task);
    bool IsFlipped() const;
    void SetFlip(bool flipState);
    void Render() override;
    void NotifyCollision(GameObject& other) override; 
    
    Vec2 GetPosition() const;
    static Character *player;
    void SetTargetIndicator(std::weak_ptr<GameObject> indicator);
    std::weak_ptr<GameObject> GetTargetIndicator() const;

private:
    std::queue<Command> taskQueue;
    std::string currentAnim; 
    Vec2 speed;
    float linearSpeed;
    bool isMoving;
    bool flipped;
    bool isMovingToTarget; 
    Vec2 targetPos;
    std::weak_ptr<GameObject> targetIndicator;
};

#endif