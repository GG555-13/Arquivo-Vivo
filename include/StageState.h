#ifndef STAGESTATE_H
#define STAGESTATE_H

#include "State.h"
#include "Music.h"
#include "TileSet.h"
#include "Timer.h"
#include "StageConfig.h"

class StageState : public State {
public:
    StageState(std::string stageId = "colonial_mansion");
    ~StageState();

    void LoadAssets();
    int walkableState;
    void UpdateWalkable(float dt);
    void Update(float dt);
    void Render();

    void Start();
    void Pause();
    void Resume();
    void LoadBackgroundLayers(const std::vector<BackgroundLayerConfig>& layers);
    void LoadStage(const StageConfig& config);
    void TransitionTo(std::string targetStageId);

private:
    Music music;
    TileSet* tileSet;
    Timer endGameTimer;
    bool endGameStarted;
    bool playerWon; 
    std::string currentStageId;
    float maxStageWidth;
    Music backgroundMusic;
};

#endif