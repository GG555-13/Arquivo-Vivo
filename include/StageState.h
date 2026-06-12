#ifndef STAGESTATE_H
#define STAGESTATE_H

#include "WalkableState.h"
#include "Music.h"
#include "TileSet.h"
#include "Timer.h"
#include "StageConfig.h"


class StageState : public WalkableState {
public:
    StageState(std::string stageId = "colonial_mansion");
    ~StageState();

    void LoadAssets();
    void Update(float dt);
    void Render();

    void Start();
    void Pause();
    void Resume();
    
    void LoadBackgroundLayers(const std::vector<BackgroundLayerConfig>& layers);
    void LoadStage(const StageConfig& config);
    void TransitionTo(std::string targetStageId);

protected:
    void UpdateWalkable(float dt) override;

private:
    TileSet* tileSet;
    Timer endGameTimer;
    bool endGameStarted;
    bool playerWon; 
    
    std::string currentStageId;
    float maxStageWidth;
    Music backgroundMusic;
};

#endif