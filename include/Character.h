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

    Character(GameObject &associated);
    virtual ~Character();

    void Start() override;
    void Update(float dt) override;
    void Issue(Command task);
    bool IsFlipped() const;
    void SetFlip(bool flipState);
    void Render() override;
    void NotifyCollision(GameObject& other) override; 
    
    Vec2 GetPosition() const;

    // Bounds (configurable by subclasses)
    void SetBounds(float minY, float maxY, float minX, float maxX);
    void EnableBounds(bool enable);

protected:
    // Subclasses must implement their own sprite/animation setup
    virtual void SetupSprite() = 0;
    virtual void UpdateAnimation();
    virtual void ConstrainPosition();
    virtual void OnDirectionInput();
    virtual void OnArrivedAtTarget();

    std::queue<Command> taskQueue;
    std::string currentAnim; 
    Vec2 speed;
    float linearSpeed;
    bool isMoving;
    bool flipped;
    bool isMovingToTarget; 
    Vec2 targetPos;

    // Bounds configuration (default: none)
    float floorMinY = 0.0f;
    float floorMaxY = 0.0f;
    float boundMinX = 0.0f;
    float boundMaxX = 0.0f;
    bool boundsEnabled = false;
};

#endif