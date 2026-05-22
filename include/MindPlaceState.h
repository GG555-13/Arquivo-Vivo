#ifndef MINDPLACESTATE_H
#define MINDPLACESTATE_H

#include "State.h"

class MindPlaceState : public State
{
public:
    MindPlaceState();
    ~MindPlaceState();

    void LoadAssets();
    void Update(float dt);
    void Render();

    void Start();
    void Pause();
    void Resume();
};

#endif