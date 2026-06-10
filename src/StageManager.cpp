#include "StageManager.h"
#include <iostream>

std::unordered_map<std::string, StageConfig> StageManager::stages;

void StageManager::Initialize() {
    // EXTERIOR MANSION
    StageConfig mansionExterior;
    mansionExterior.stageId = "colonial_mansion";
    mansionExterior.musicFile = "recursos/audio/Intro.mp3";
    mansionExterior.playerSpawn = Vec2(200.0f, 700.0f);
    
    mansionExterior.layers.push_back({"recursos/img/NightSky.png", 1.0f, 1.0f, 0.3f, 0.3f, 2756.0f, true});
    mansionExterior.layers.push_back({"recursos/img/Mansion.jpg", 0.7f, 0.7f, 1.0f, 1.0f, 1929.0f, false});
    
    // DOOR TRIGGER
    mansionExterior.triggers.push_back({1310.0f, 600.0f, 150.0f, 300.0f, "mansion_interior"});
    
    stages["colonial_mansion"] = mansionExterior;

    

    // INTERIOR MANSION
    StageConfig mansionInterior;
    mansionInterior.stageId = "mansion_interior";
    mansionInterior.musicFile = "recursos/audio/endStateLose.ogg"; 
    mansionInterior.playerSpawn = Vec2(100.0f, 600.0f); 
    
    mansionInterior.layers.push_back({"recursos/img/InsideMansion.png", 1.0f, 1.0f, 1.0f, 1.0f, 1200.0f, false});
    
    mansionInterior.triggers.push_back({1288.0f, 400.0f, 200.0f, 260.0f, "WIN_GAME"});
    
    stages["mansion_interior"] = mansionInterior;
}

StageConfig StageManager::GetStage(const std::string& stageId) {
    if (stages.find(stageId) != stages.end()) {
        return stages[stageId];
    }
    std::cerr << "ERROR: Stage ID '" << stageId << "' not found!\n";
    return StageConfig(); 
}