#ifndef STAGESTATE_H
#define STAGESTATE_H

#include "WalkableState.h"
#include "Music.h"
#include "TileSet.h"
#include "Timer.h"
#include "StageConfig.h"
#include "Text.h"


class StageState : public WalkableState {
public:
    StageState(std::string stageId = "colonial_mansion", float overrideSpawnX = -1.0f, float overrideSpawnY = -1.0f);
    ~StageState();

    void LoadAssets();
    void Update(float dt);
    void Render();

    void Start();
    void Pause();
    void Resume();
    
    void LoadBackgroundLayers(const std::vector<BackgroundLayerConfig>& layers);
    void LoadStage(const StageConfig& config);
    void TransitionTo(std::string targetStageId, float spawnX = -1.0f, float spawnY = -1.0f,
                      const FadeTransitionConfig& fadeConfig = {});

protected:
    void UpdateWalkable(float dt) override;

private:
    void PerformTransitionTo(std::string targetStageId, float spawnX, float spawnY);

    TileSet* tileSet;
    Timer endGameTimer;
    bool endGameStarted;
    bool playerWon;
    GameObject* debugPosText; 
    
    std::string currentStageId;
    float maxStageWidth;
    Music backgroundMusic;
    float overrideSpawnX;
    float overrideSpawnY;
};

#endif