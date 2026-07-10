#ifndef TUTORIALENDINTERLUDESTATE_H
#define TUTORIALENDINTERLUDESTATE_H

#include "State.h"
#include "Timer.h"

class TutorialEndInterludeState : public State
{
public:
    TutorialEndInterludeState();
    ~TutorialEndInterludeState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;

private:
    Timer holdTimer;
    bool fadeOutStarted = false;

    void CleanupTutorialData();
};

#endif
