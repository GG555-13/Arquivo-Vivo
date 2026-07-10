#include "StageManager.h"
#include "Interactable.h"
#include "DialogueBox.h"
#include <iostream>

std::unordered_map<std::string, StageConfig> StageManager::stages;

void StageManager::Initialize() {
    // ==========================================
    // 1. EXTERIOR MANSÃO (colonial_mansion)
    // ==========================================
    StageConfig mansionExterior;
    mansionExterior.stageId = "colonial_mansion";
    mansionExterior.musicFile = "recursos/audio/Intro.mp3";
    mansionExterior.playerSpawn = Vec2(200.0f, 850.0f);
    
    mansionExterior.layers.push_back({"recursos/img/NightSky.png", 1.0f, 1.0f, 0.3f, 0.3f, 0.0f, true});
    mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_parte1.png", 0.42f, 0.42f, 1.0f, 1.0f, 0.0f, false});
    mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_parte2.png", 0.42f, 0.42f, 1.0f, 1.0f, 10475.0f, false});
    mansionExterior.layers.push_back({"recursos/img/cidadearquivovivo_parte3.png", 0.42f, 0.42f, 1.0f, 1.0f, 20950.0f, false});

    PropConfig taxiForeground;
    taxiForeground.id = "taxi_foreground";
    taxiForeground.x = 9975.0f;
    taxiForeground.y = 290.0f;
    taxiForeground.spriteFile = "recursos/img/taxiseparado.png";
    taxiForeground.scale = 0.445f;
    mansionExterior.props.push_back(taxiForeground);

    mansionExterior.triggers.push_back({9645.0f, 600.0f, 150.0f, 300.0f, "mansion_interior", 185.0f, 850.0f});
    mansionExterior.triggers.push_back({10721.0f, 600.0f, 150.0f, 300.0f, "residential_area", 185.0f, 850.0f});
    
    stages["colonial_mansion"] = mansionExterior;


    // ==========================================
    // 2. INTERIOR MANSÃO / DELEGACIA (mansion_interior)
    // ==========================================
    StageConfig mansionInterior;
    mansionInterior.stageId = "mansion_interior";
    mansionInterior.musicFile = "recursos/audio/endStateLose.ogg"; 
    mansionInterior.playerSpawn = Vec2(185.0f, 850.0f); 
    
    mansionInterior.layers.push_back({"recursos/img/delegacia_interior.png", 0.42f, 0.42f, 1.0f, 1.0f, 0.0f, false});

    NPCConfig boss;
    boss.id = "chefe_policia"; 
    boss.x = 1310.0f;
    boss.y = 508.0f;
    boss.renderOffsetY = -562.0f;
    boss.spriteFile = "recursos/img/Chefe.png"; 
    boss.frameRows = 1;
    boss.frameCols = 1;
    boss.scale = 0.35f;
    boss.flipHorizontal = false;
    boss.interactRadius = 430.0f;
    boss.dialogueJson = "recursos/dialogos/dia1_chefe_intro_joca.json"; 
    mansionInterior.npcs.push_back(boss);

    PropConfig clueBoard;
    clueBoard.id = "quadro_pistas";
    clueBoard.x = 2400.0f;
    clueBoard.y = 750.0f;
    clueBoard.spriteFile = "";
    clueBoard.interactRadius = 250.0f;
    clueBoard.markerOffsetY = -110.0f;
    clueBoard.conditionFlag = "tutorial_board_unlocked"; 
    clueBoard.targetState = "ClueBoardState";
    mansionInterior.props.push_back(clueBoard);

    mansionInterior.triggers.push_back({115.0f, 600.0f, 150.0f, 300.0f, "colonial_mansion", 9645.0f, 850.0f});
    mansionInterior.triggers.push_back({1288.0f, 600.0f, 200.0f, 260.0f, "WIN_GAME", -1.0f, -1.0f, {true, 2.0f, FadeColor::Black, true, 1.0f, FadeColor::Black}});
    
    stages["mansion_interior"] = mansionInterior;

    


    // ==========================================
    // 3. ÁREA RESIDENCIAL (residential_area)
    // ==========================================
    StageConfig residentialArea;
    residentialArea.stageId = "residential_area";
    residentialArea.musicFile = "recursos/audio/Intro.mp3";
    residentialArea.playerSpawn = Vec2(200.0f, 850.0f);

    residentialArea.layers.push_back({"recursos/img/residencial_externo.png", 0.42f, 0.42f, 1.0f, 1.0f, 0.0f, false});

    residentialArea.triggers.push_back({2821.0f, 600.0f, 150.0f, 300.0f, "colonial_mansion", 10721.0f, 850.0f});
    residentialArea.triggers.push_back({1655.0f, 600.0f, 150.0f, 300.0f, "missing_family", 200.0f, 850.0f});
    
    stages["residential_area"] = residentialArea;


    // ==========================================
    // 4. CASA DA FAMÍLIA DESAPARECIDA (missing_family)
    // ==========================================
    StageConfig missingFamily;
    missingFamily.stageId = "missing_family";
    missingFamily.musicFile = "recursos/audio/Intro.mp3";
    missingFamily.playerSpawn = Vec2(200.0f, 850.0f);

    missingFamily.layers.push_back({"recursos/img/casa_familia_sumida.png", 0.42f, 0.42f, 1.0f, 1.0f, 0.0f, false});
    
    missingFamily.triggers.push_back({85.0f, 600.0f, 150.0f, 300.0f, "residential_area", 1655.0f, 850.0f});

    stages["missing_family"] = missingFamily;


}

StageConfig StageManager::GetStage(const std::string& stageId) {
    if (stages.find(stageId) != stages.end()) {
        return stages[stageId];
    }
    std::cerr << "ERROR: Stage ID '" << stageId << "' not found!\n";
    return StageConfig(); 
}
