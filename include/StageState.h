#ifndef STAGESTATE_H
#define STAGESTATE_H

#include "WalkableState.h"
#include "Music.h"
#include "TileSet.h"
#include "Timer.h"

class StageState : public WalkableState {
public:
    StageState();
    ~StageState();

    void LoadAssets();
    void Render();

    void Start();
    void Pause();
    void Resume();

protected:
    void UpdateWalkable(float dt) override;

private:
    Music music;
    TileSet* tileSet;
    Timer endGameTimer;
    bool endGameStarted;
    bool playerWon; 
};

#endif