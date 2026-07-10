#include "StageState.h"
#include "SpriteRenderer.h"
#include "Game.h"
#include "TileSet.h"
#include "TileMap.h"
#include "Camera.h"
#include "Player.h"
#include "NPC.h"
#include "PlayerController.h"
#include "Collision.h"
#include "Collider.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include "GameData.h"
#include "EndState.h"
#include "StageManager.h"
#include "TransitionTrigger.h"
#include "Resources.h"
#include "InputManager.h"
#include "Interactable.h"
#include "DialogueBox.h"
#include "Inventory.h"
#include "ClueBoardState.h"
#include "Interactable.h"
#include "ObtainedItemCardPresenter.h"
#include "TutorialEndInterludeState.h"

StageState::StageState(std::string stageId, float spawnX, float spawnY) : WalkableState(), 
                                                                          currentStageId(stageId), 
                                                                          overrideSpawnX(spawnX), 
                                                                          overrideSpawnY(spawnY),
                                                                          itemNotifications(new ObtainedItemCardPresenter(*this)) {}

void StageState::Start() {
    LoadAssets();
    StageConfig config = StageManager::GetStage(currentStageId);
    LoadStage(config);
    started = true;
    StartArray();

    auto pf = Game::GetInstance().ConsumePendingFadeIn();
    if (pf.active) {
        screenFade.FadeIn(pf.duration, pf.color);
    }
}

void StageState::LoadBackgroundLayers(const std::vector<BackgroundLayerConfig>& layers) {
    this->maxStageWidth = 0.0f;
    for (const auto& layer : layers) {
        GameObject* layerGo = new GameObject();
        SpriteRenderer* layerSprite = new SpriteRenderer(*layerGo, layer.file);
        layerSprite->SetScale(layer.scaleX, layer.scaleY);
        layerSprite->SetParallax(Vec2(layer.parallaxX, layer.parallaxY));
        layerGo->AddComponent(layerSprite);
        // Compensa offsets
        float spriteWidth = layerSprite->GetWidth();
        float spriteHeight = layerSprite->GetHeight();
        float centeringOffsetX = (spriteWidth / layer.scaleX) * (1.0f - layer.scaleX) / 2.0f;
        float centeringOffsetY = (spriteHeight / layer.scaleY) * (1.0f - layer.scaleY) / 2.0f;
        layerGo->box.x = layer.baseWidth * layer.scaleX - centeringOffsetX;
        layerGo->box.y = 900.0f - spriteHeight - centeringOffsetY;
        if (!layer.isSky) {
            float layerRightEdge = layer.baseWidth * layer.scaleX + spriteWidth;
            if (layerRightEdge > this->maxStageWidth)
                this->maxStageWidth = layerRightEdge;
        }
        AddObject(layerGo);
    }
    Camera::stageWidth = this->maxStageWidth;
}

void StageState::LoadStage(const StageConfig& config) {
    Resources::ClearImages();
    Resources::ClearMusics();
    tutorialClueBoardInteractable = nullptr;
    tutorialIntroPending = config.stageId == "mansion_interior" &&
                           GameData::GetTutorialStep() == TutorialStep::TalkToBoss;
    tutorialIntroTriggered = false;
    tutorialIntroTimer.Restart();
    LoadBackgroundLayers(config.layers);

    if (!config.musicFile.empty()) {
        backgroundMusic.Open(config.musicFile);
        backgroundMusic.Play(-1);
    }

    GameObject* playerGo = new GameObject();
    playerGo->box.x = (overrideSpawnX >= 0.0f) ? overrideSpawnX : config.playerSpawn.x;
    playerGo->box.y = (overrideSpawnY >= 0.0f) ? overrideSpawnY : config.playerSpawn.y;
    playerGo->AddComponent(new Player(*playerGo));
    playerGo->AddComponent(new PlayerController(*playerGo));

    float sx = (overrideSpawnX >= 0.0f) ? overrideSpawnX : config.playerSpawn.x;
    float sy = (overrideSpawnY >= 0.0f) ? overrideSpawnY : config.playerSpawn.y;
    playerGo->box.SetCenter(Vec2(sx, sy));

    AddObject(playerGo);
    Camera::Follow(playerGo);
    Camera::Update(0.0f);

    if (config.stageId == "mansion_interior") {
        GameObject* foregroundDesk = new GameObject();
        SpriteRenderer* deskSprite = new SpriteRenderer(*foregroundDesk, "recursos/img/mesadelegaciaseparada.png");
        deskSprite->SetScale(0.141f, 0.19f);
        deskSprite->SetUseSourceFrameOffset(false);
        foregroundDesk->AddComponent(deskSprite);
        foregroundDesk->box.x = 1228.0f;
        foregroundDesk->box.y = 630.0f;
        AddObject(foregroundDesk);
    }

    // Debug: player position display
    debugPosText = new GameObject();
    Text* posText = new Text(*debugPosText,
        "recursos/font/neodgm.ttf", 16, Text::SOLID,
        "Pos: (0, 0)",
        {255, 255, 0, 255});  // yellow
    debugPosText->AddComponent(posText);
    AddObject(debugPosText);

    for (const auto& triggerData : config.triggers) {
        GameObject* triggerGo = new GameObject();
        triggerGo->box.x = triggerData.x;
        triggerGo->box.y = triggerData.y;
        triggerGo->box.w = triggerData.width;
        triggerGo->box.h = triggerData.height;
        // triggerGo->AddComponent(new Collider(*triggerGo));
        // triggerGo->AddComponent(new TransitionTrigger(*triggerGo, triggerData.targetStageId));

        // triggerGo->AddComponent(new Interactable(*triggerGo,
        //                                          Interactable::SPACE_OR_CLICK,
        //                                          Interactable::REQUIRE_INSIDE_AREA,
        //                                          0.0f,
        //                                          [targetId = triggerData.targetStageId]() {
        //                                             auto* stage = static_cast<StageState*>(&Game::GetInstance().GetCurrentState());
        //                                             if (stage) stage->TransitionTo(targetId);
        //                                          },
        //                                          -28.0f
        // ));

        triggerGo->AddComponent(new Interactable(*triggerGo,
                                                 Interactable::SPACE_OR_CLICK,
                                                 Interactable::REQUIRE_NEAR,
                                                 std::max(triggerData.width, triggerData.height) * 0.7f,
                                                 [targetId = triggerData.targetStageId,
                                                  sx = triggerData.targetSpawnX,
                                                  sy = triggerData.targetSpawnY,
                                                  fadeCfg = triggerData.fade]() {
                                                     auto* stage = static_cast<StageState*>(&Game::GetInstance().GetCurrentState());
                                                     if (stage) stage->TransitionTo(targetId, sx, sy, fadeCfg);
                                                 }
        ));

        AddObject(triggerGo);
    }

    // Clue board interactable — only inside the mansion
    if (config.stageId == "mansion_interior") {
        GameObject* clueBoardGo = new GameObject();
        // SpriteRenderer* boardSprite = new SpriteRenderer(*clueBoardGo, "recursos/img/quadro_pistas.png");
        // boardSprite->SetScale(1.2f, 1.2f);
        // clueBoardGo->AddComponent(boardSprite);
        clueBoardGo->box.SetCenter(Vec2(2400.0f, 750.0f));

        tutorialClueBoardInteractable = new Interactable(
            *clueBoardGo,
            Interactable::SPACE_OR_CLICK,
            Interactable::REQUIRE_NEAR,
            250.0f,
            []() {
                Game::GetInstance().Push(new ClueBoardState());
            },
            -110.0f
        );
        tutorialClueBoardInteractable->SetEnabled(
            GameData::GetTutorialStep() != TutorialStep::TalkToBoss
        );
        clueBoardGo->AddComponent(tutorialClueBoardInteractable);
        AddObject(clueBoardGo);
    }

    GameObject* npcGO = new GameObject();

    if (config.stageId == "mansion_interior") {
        npcGO->box.x = 1150.0f;
        npcGO->box.y = 750.0f;
    } else {
        npcGO->box.x = sx + 200.0f;
        npcGO->box.y = 750.0f;
    }

    npcGO->AddComponent(new NPC(*npcGO, "recursos/img/NPC.png", 3, 4));

    const bool isTutorialBoss = config.stageId == "mansion_interior";
    npcGO->AddComponent(new Interactable(*npcGO, Interactable::SPACE_ONLY, Interactable::REQUIRE_NEAR, 100.0f, [this, isTutorialBoss]() {
        if (DialogueBox::isPlaying) return; 

        const TutorialStep step = GameData::GetTutorialStep();
        if (isTutorialBoss && step != TutorialStep::TalkToBoss && step != TutorialStep::SolveWhisper) {
            return;
        }

        if (isTutorialBoss && step == TutorialStep::TalkToBoss) {
            StartInitialBossDialogue();
        }
        else if (isTutorialBoss && step == TutorialStep::SolveWhisper) {
            StartPostWhisperBossDialogue();
        }
    }));

    AddObject(npcGO);

}


void StageState::PerformTransitionTo(std::string targetStageId, float spawnX, float spawnY) {
    popRequested = true;

    if (targetStageId == "WIN_GAME") {
        GameData::playerVictory = true;
        Game::GetInstance().Push(new EndState());
    }
    else if (targetStageId == "LOSE_GAME") {
        GameData::playerVictory = false;
        Game::GetInstance().Push(new EndState());
    }
    else {
        Game::GetInstance().Push(new StageState(targetStageId, spawnX, spawnY));
    }
}

void StageState::TransitionTo(std::string targetStageId, float spawnX, float spawnY,
                               const FadeTransitionConfig& fadeConfig) {
    if (screenFade.IsActive()) return;  // block double-trigger while fading

    if (fadeConfig.fadeIn) {
        Game::GetInstance().SetPendingFadeIn({true, fadeConfig.fadeInDuration, fadeConfig.fadeInColor});
    }

    if (fadeConfig.fadeOut) {
        screenFade.FadeOut(fadeConfig.fadeOutDuration, fadeConfig.fadeOutColor, [=]() {
            PerformTransitionTo(targetStageId, spawnX, spawnY);
        });
    } else {
        PerformTransitionTo(targetStageId, spawnX, spawnY);
    }
}

void StageState::StartInitialBossDialogue() {
    if (DialogueBox::isPlaying || GameData::GetTutorialStep() != TutorialStep::TalkToBoss) return;

    tutorialIntroPending = false;
    tutorialIntroTriggered = true;

    GameObject* dialogueController = new GameObject();
    dialogueController->AddComponent(new DialogueBox(
        *dialogueController,
        "recursos/dialogos/clue01.json",
        [this]() {
            Inventory::Add("tutorial_notebook_1");
            Inventory::Add("tutorial_notebook_2");
            Inventory::Add("cafe");
            Inventory::Add("bread_de_queijo");
            Inventory::Add("relato_1");
            Inventory::Add("relato_2");
            Inventory::Add("relato_3");
            Inventory::Add("chief_dialogues");
            if (GameData::AdvanceTutorial(TutorialStep::TalkToBoss, TutorialStep::OpenBoard) &&
                tutorialClueBoardInteractable != nullptr) {
                tutorialClueBoardInteractable->SetEnabled(true);
            }
        },
        "chief"
    ));
    AddObject(dialogueController);
}

void StageState::StartPostWhisperBossDialogue() {
    if (DialogueBox::isPlaying || GameData::GetTutorialStep() != TutorialStep::SolveWhisper) return;

    GameObject* dialogueController = new GameObject();
    dialogueController->AddComponent(new DialogueBox(
        *dialogueController,
        "recursos/dialogos/chefe_pos_sussurro.json",
        []() {
            if (GameData::AdvanceTutorial(TutorialStep::SolveWhisper, TutorialStep::TutorialComplete)) {
                auto* stage = static_cast<StageState*>(&Game::GetInstance().GetCurrentState());
                if (stage) stage->BeginTutorialEndSequence();
            }
        },
        "chief"
    ));
    AddObject(dialogueController);
}

void StageState::UpdateTutorialIntro(float dt) {
    if (!tutorialIntroPending || tutorialIntroTriggered) return;
    if (currentStageId != "mansion_interior") return;
    if (GameData::GetTutorialStep() != TutorialStep::TalkToBoss) {
        tutorialIntroPending = false;
        return;
    }
    if (DialogueBox::isPlaying) return;

    tutorialIntroTimer.Update(dt);
    if (tutorialIntroTimer.Get() >= 1.5f) {
        StartInitialBossDialogue();
    }
}

void StageState::BeginTutorialEndSequence() {
    if (screenFade.IsActive()) return;

    screenFade.FadeOut(1.0f, FadeColor::Black, []() {
        Game::GetInstance().Push(new TutorialEndInterludeState());
    });
}

void StageState::LoadAssets() {

}

void StageState::UpdateWalkable(float dt) {
    (void)dt;

    for (unsigned i = 0; i < objectArray.size(); i++) {
        Collider* colA = objectArray[i]->GetComponent<Collider>();
        if (!colA) continue;

        for (unsigned j = i + 1; j < objectArray.size(); j++) {
            Collider* colB = objectArray[j]->GetComponent<Collider>();
            if (!colB) continue; 

            if (Collision::IsColliding(colA->box, colB->box, objectArray[i]->angleDeg, objectArray[j]->angleDeg)) {
                objectArray[i]->NotifyCollision(*objectArray[j]);
                objectArray[j]->NotifyCollision(*objectArray[i]);
            }
        }
    }
}

void StageState::Render() {
    RenderArray();
    screenFade.Render();
}

void StageState::Pause() {}
void StageState::Resume() {
    if (Player::player) {
        Camera::Update(0.0f);
    }
    auto pf = Game::GetInstance().ConsumePendingFadeIn();
    if (pf.active) {
        screenFade.FadeIn(pf.duration, pf.color);
    }
}
StageState::~StageState() {}

void StageState::Update(float dt) {
    screenFade.Update(dt);

    if (screenFade.IsActive()) {
        return;
    }

    UpdateTutorialIntro(dt);
    if (DialogueBox::isPlaying) {
        UpdateArray(dt);
        return;
    }

    itemNotifications->Update(dt);
    if (itemNotifications->IsActive()) {
        return;
    }

    WalkableState::Update(dt);

    // Update debug position display
    if (debugPosText && Player::player) {
        Vec2 pos = Player::player->GetPosition();
        auto* text = debugPosText->GetComponent<Text>();
        if (text) {
            text->SetText(
                "Pos: (" + std::to_string((int)pos.x) + ", " + std::to_string((int)pos.y) + ") | Tutorial: " +
                GameData::GetTutorialStepName()
            );
        }
        // Pin to top-left of screen
        debugPosText->box.x = Camera::pos.x + 10.0f;
        debugPosText->box.y = Camera::pos.y + 10.0f;
    }
}
