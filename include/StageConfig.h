#ifndef STAGECONFIG_H
#define STAGECONFIG_H

#include <string>
#include <vector>
#include "Vec2.h"
#include "FadeTypes.h"

struct BackgroundLayerConfig {
    std::string file;
    float scaleX, scaleY;
    float parallaxX, parallaxY;
    float baseWidth; 
    bool isSky;      
};

struct FadeTransitionConfig {
    bool fadeOut = false;
    float fadeOutDuration = 1.0f;
    FadeColor fadeOutColor = FadeColor::Black;

    bool fadeIn = false;
    float fadeInDuration = 1.0f;
    FadeColor fadeInColor = FadeColor::Black;
};

struct TriggerConfig {
    float x, y;
    float width, height;
    std::string targetStageId;
    float targetSpawnX = -1.0f;
    float targetSpawnY = -1.0f;
    FadeTransitionConfig fade;
    std::string conditionFlag = "";
};

struct NPCConfig {
    std::string id;
    float x, y;
    std::string spriteFile;
    int frameRows = 1, frameCols = 1;
    float scale = 1.0f;
    float renderOffsetY = 0.0f;
    bool flipHorizontal = false;
    float interactRadius = 100.0f;
    
    std::string dialogueJson; 
    std::string conditionFlag; 
};

struct PropConfig {
    std::string id;
    float x, y;
    std::string spriteFile;
    float scale = 1.0f;
    float interactRadius = 100.0f;
    float markerOffsetY = 0.0f;
    bool renderBehindPlayer = false;
    
    std::string interactDialogueJson; 
    std::string unlockFlag;           
    std::string conditionFlag;        
    std::string targetState; 
};

struct StageConfig {
    std::string stageId;
    std::string musicFile;
    std::vector<BackgroundLayerConfig> layers;
    Vec2 playerSpawn;
    
    std::vector<TriggerConfig> triggers; 
    std::vector<NPCConfig> npcs;   
    std::vector<PropConfig> props; 
};

#endif
