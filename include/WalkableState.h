#ifndef WALKABLESTATE_H
#define WALKABLESTATE_H

#include "State.h"

class WalkableState : public State
{
public:
    WalkableState();
    virtual ~WalkableState();

    void Update(float dt) override;

protected:
    virtual void UpdateWalkable(float dt);
    virtual bool CanOpenMindPlace() const;
    virtual bool ShouldCloseOnEscape() const;
};

#endif