#ifndef STAGECONFIG_H
#define STAGECONFIG_H

#include <string>
#include <vector>
#include "Vec2.h"

struct BackgroundLayerConfig {
    std::string file;
    float scaleX, scaleY;
    float parallaxX, parallaxY;
    float baseWidth; 
    bool isSky;      
};

struct TriggerConfig {
    float x, y;
    float width, height;
    std::string targetStageId;
};

struct StageConfig {
    std::string stageId;
    std::string musicFile;
    std::vector<BackgroundLayerConfig> layers;
    Vec2 playerSpawn;
    
    std::vector<TriggerConfig> triggers; 
};

#endif