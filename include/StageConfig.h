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
};

struct StageConfig {
    std::string stageId;
    std::string musicFile;
    std::vector<BackgroundLayerConfig> layers;
    Vec2 playerSpawn;
    
    std::vector<TriggerConfig> triggers; 
};

#endif