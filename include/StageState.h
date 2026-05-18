#ifndef STAGESTATE_H
#define STAGESTATE_H

#include "State.h"
#include "Music.h"
#include "TileSet.h"
#include "Timer.h"

class StageState : public State {
public:
    StageState();
    ~StageState();

    void LoadAssets();
    void Update(float dt);
    void Render();

    void Start();
    void Pause();
    void Resume();

private:
    Music music;
    TileSet* tileSet;
    Timer endGameTimer;
    bool endGameStarted;
    bool playerWon; 
};

#endif