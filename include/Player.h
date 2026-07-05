#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include <memory>

class GameObject;

class Player : public Character
{
public:
    Player(GameObject &associated);
    ~Player();

    static Player *player;

    void SetTargetIndicator(std::weak_ptr<GameObject> indicator);
    std::weak_ptr<GameObject> GetTargetIndicator() const;

protected:
    void SetupSprite() override;
    void ConstrainPosition() override;
    void OnDirectionInput() override;
    void OnArrivedAtTarget() override;

private:
    std::weak_ptr<GameObject> targetIndicator;
};

#endif
