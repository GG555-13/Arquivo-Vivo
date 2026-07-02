#include "StageManager.h"
#include "Interactable.h"
#include "DialogueBox.h"
#include <iostream>

std::unordered_map<std::string, StageConfig> StageManager::stages;

void StageManager::Initialize() {
    // EXTERIOR MANSION
    StageConfig mansionExterior;
    mansionExterior.stageId = "colonial_mansion";
    mansionExterior.musicFile = "recursos/audio/Intro.mp3";
    mansionExterior.playerSpawn = Vec2(200.0f, 850.0f);
    
    mansionExterior.layers.push_back({"recursos/img/NightSky.png", 1.0f, 1.0f, 0.3f, 0.3f, 0.0f, true});
    
    // mansionExterior.layers.push_back({"recursos/img/NightSky.png", 1.0f, 1.0f, 0.3f, 0.3f, 0.0f, true});
    // mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_01.png", 0.35f, 0.35f, 1.0f, 1.0f, 0.0f, false});
    // mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_02.png", 0.35f, 0.35f, 1.0f, 1.0f, 7856.0f, false});
    // mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_03.png", 0.35f, 0.35f, 1.0f, 1.0f, 15712.0f, false});
    // mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_04.png", 0.35f, 0.35f, 1.0f, 1.0f, 23568.0f, false});

    // mansionExterior.layers.push_back({"recursos/img/Cenario1_tentativa2_parte1.png", 0.35f, 0.35f, 1.0f, 1.0f, 0.0f, false});
    // mansionExterior.layers.push_back({"recursos/img/Cenario1_tentativa2_parte2.png", 0.35f, 0.35f, 1.0f, 1.0f, 10475.0f, false});
    // mansionExterior.layers.push_back({"recursos/img/Cenario1_tentativa2_parte3.png", 0.35f, 0.35f, 1.0f, 1.0f, 20950.0f, false});

    mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_parte1.png", 0.42f, 0.42f, 1.0f, 1.0f, 0.0f, false});
    mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_parte2.png", 0.42f, 0.42f, 1.0f, 1.0f, 10475.0f, false});
    mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_parte3.png", 0.42f, 0.42f, 1.0f, 1.0f, 20950.0f, false});

    // DOOR TRIGGER
    mansionExterior.triggers.push_back({9645.0f, 600.0f, 150.0f, 300.0f, "mansion_interior", 185.0f, 850.0f});
    mansionExterior.triggers.push_back({10721.0f, 600.0f, 150.0f, 300.0f, "residential_area", 185.0f, 850.0f});
    // 66 offset to character position on screen?
    
    stages["colonial_mansion"] = mansionExterior;

     

    // INTERIOR MANSION
    StageConfig mansionInterior;
    mansionInterior.stageId = "mansion_interior";
    mansionInterior.musicFile = "recursos/audio/endStateLose.ogg"; 
    mansionInterior.playerSpawn = Vec2(185.0f, 850.0f); 
    
    mansionInterior.layers.push_back({"recursos/img/delegacia_interior.png", 0.42f, 0.42f, 1.0f, 1.0f, 0.0f, false});

    mansionInterior.triggers.push_back({115.0f, 600.0f, 150.0f, 300.0f, "colonial_mansion", 9645.0f, 850.0f});
    
    mansionInterior.triggers.push_back({1288.0f, 600.0f, 200.0f, 260.0f, "WIN_GAME"});
    
    stages["mansion_interior"] = mansionInterior;

    // area residencial
    StageConfig residentialArea;
    residentialArea.stageId = "residential_area";
    residentialArea.musicFile = "recursos/audio/Intro.mp3";
    residentialArea.playerSpawn = Vec2(200.0f, 850.0f);

    residentialArea.layers.push_back({"recursos/img/residencial_externo.png", 0.42f, 0.42f, 1.0f, 1.0f, 0.0f, false});

    residentialArea.triggers.push_back({2821.0f, 600.0f, 150.0f, 300.0f, "colonial_mansion", 10721.0f, 850.0f});

    stages["residential_area"] = residentialArea;

}

StageConfig StageManager::GetStage(const std::string& stageId) {
    if (stages.find(stageId) != stages.end()) {
        return stages[stageId];
    }
    std::cerr << "ERROR: Stage ID '" << stageId << "' not found!\n";
    return StageConfig(); 
}