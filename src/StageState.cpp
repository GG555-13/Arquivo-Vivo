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
#include "ObtainedItemCardPresenter.h"
#include "TutorialEndInterludeState.h"
#include "InstructionPrompt.h"

namespace
{
bool IsTutorialBoardUnlocked()
{
    if (GameData::GetFlag("tutorial_board_unlocked"))
    {
        return true;
    }

    const TutorialStep step = GameData::GetTutorialStep();
    return step == TutorialStep::OpenBoard ||
           step == TutorialStep::SolveBoard ||
           step == TutorialStep::SolveWhisper;
}
}

StageState::StageState(std::string stageId, float spawnX, float spawnY)
    : WalkableState(),
      currentStageId(stageId),
      overrideSpawnX(spawnX),
      overrideSpawnY(spawnY),
      itemNotifications(new ObtainedItemCardPresenter(*this)) {}

void StageState::Start()
{
    LoadAssets();
    StageConfig config = StageManager::GetStage(currentStageId);
    LoadStage(config);
    started = true;
    StartArray();

    auto pf = Game::GetInstance().ConsumePendingFadeIn();
    if (pf.active)
    {
        screenFade.FadeIn(pf.duration, pf.color);
    }
}

void StageState::LoadBackgroundLayers(const std::vector<BackgroundLayerConfig> &layers)
{
    this->maxStageWidth = 0.0f;
    for (const auto &layer : layers)
    {
        GameObject *layerGo = new GameObject();
        SpriteRenderer *layerSprite = new SpriteRenderer(*layerGo, layer.file);
        layerSprite->SetScale(layer.scaleX, layer.scaleY);
        layerSprite->SetParallax(Vec2(layer.parallaxX, layer.parallaxY));
        layerGo->AddComponent(layerSprite);

        float spriteWidth = layerSprite->GetWidth();
        float spriteHeight = layerSprite->GetHeight();
        float centeringOffsetX = (spriteWidth / layer.scaleX) * (1.0f - layer.scaleX) / 2.0f;
        float centeringOffsetY = (spriteHeight / layer.scaleY) * (1.0f - layer.scaleY) / 2.0f;
        layerGo->box.x = layer.baseWidth * layer.scaleX - centeringOffsetX;
        layerGo->box.y = 900.0f - spriteHeight - centeringOffsetY;

        if (!layer.isSky)
        {
            float layerRightEdge = layer.baseWidth * layer.scaleX + spriteWidth;
            if (layerRightEdge > this->maxStageWidth)
                this->maxStageWidth = layerRightEdge;
        }
        AddObject(layerGo);
    }
    Camera::stageWidth = this->maxStageWidth;
}

void StageState::LoadProps(const StageConfig &config, bool renderBehindPlayer)
{
    for (const auto &propConfig : config.props)
    {
        if (propConfig.renderBehindPlayer != renderBehindPlayer)
        {
            continue;
        }

        GameObject *propGO = new GameObject();
        const bool visualOnlyProp = propConfig.interactDialogueJson.empty() &&
                                    propConfig.unlockFlag.empty() &&
                                    propConfig.conditionFlag.empty() &&
                                    propConfig.targetState.empty();

        if (!propConfig.spriteFile.empty())
        {
            SpriteRenderer *sr = new SpriteRenderer(*propGO, propConfig.spriteFile);
            sr->SetScale(propConfig.scale, propConfig.scale);
            sr->SetUseSourceFrameOffset(false);
            propGO->AddComponent(sr);
            propGO->box.x = propConfig.x;
            propGO->box.y = propConfig.y;
        }
        else
        {
            propGO->box.SetCenter(Vec2(propConfig.x, propConfig.y));
        }

        std::function<bool()> cond = nullptr;
        if (!propConfig.conditionFlag.empty())
        {
            cond = [flag = propConfig.conditionFlag]() {
                if (flag == "tutorial_board_unlocked")
                {
                    return IsTutorialBoardUnlocked();
                }
                return GameData::GetFlag(flag);
            };
        }

        if (visualOnlyProp)
        {
            AddObject(propGO);
            continue;
        }

        Interactable *interactable = new Interactable(
            *propGO,
            Interactable::SPACE_OR_CLICK,
            Interactable::REQUIRE_NEAR,
            propConfig.interactRadius,
            [jsonFile = propConfig.interactDialogueJson,
             unlock = propConfig.unlockFlag,
             stateName = propConfig.targetState]() {
                if (DialogueBox::isPlaying)
                    return;

                if (stateName == "ClueBoardState")
                {
                    Game::GetInstance().Push(new ClueBoardState());
                    return;
                }

                if (jsonFile.empty())
                    return;

                GameObject *dialogueController = new GameObject();
                auto onComplete = [unlock]() {
                    if (!unlock.empty())
                    {
                        GameData::SetFlag(unlock, true);
                    }
                };

                dialogueController->AddComponent(new DialogueBox(*dialogueController, jsonFile, onComplete, ""));
                Game::GetInstance().GetCurrentState().AddObject(dialogueController);
            },
            propConfig.markerOffsetY,
            cond);

        if (propConfig.id == "quadro_pistas")
        {
            tutorialClueBoardInteractable = interactable;
            tutorialClueBoardInteractable->SetEnabled(IsTutorialBoardUnlocked());
        }

        propGO->AddComponent(interactable);
        AddObject(propGO);
    }
}

void StageState::LoadStage(const StageConfig &config)
{
    Resources::ClearImages();
    Resources::ClearMusics();
    tutorialClueBoardInteractable = nullptr;
    tutorialIntroDialogueJson = "recursos/dialogos/clue01.json";
    tutorialIntroPending = config.stageId == "mansion_interior" &&
                           GameData::GetTutorialStep() == TutorialStep::TalkToBoss;
    tutorialIntroTriggered = false;
    tutorialIntroTimer.Restart();
    LoadBackgroundLayers(config.layers);

    if (!config.musicFile.empty())
    {
        backgroundMusic.Open(config.musicFile);
        backgroundMusic.Play(-1);
    }

    LoadProps(config, true);

    GameObject *playerGo = new GameObject();
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

    if (config.stageId == "mansion_interior")
    {
        GameObject *foregroundDesk = new GameObject();
        SpriteRenderer *deskSprite = new SpriteRenderer(*foregroundDesk, "recursos/img/mesadelegaciaseparada.png");
        deskSprite->SetScale(0.141f, 0.19f);
        deskSprite->SetUseSourceFrameOffset(false);
        foregroundDesk->AddComponent(deskSprite);
        foregroundDesk->box.x = 1228.0f;
        foregroundDesk->box.y = 630.0f;
        AddObject(foregroundDesk);
    }

    debugPosText = new GameObject();
    Text *posText = new Text(*debugPosText,
                             "recursos/font/neodgm.ttf", 16, Text::SOLID,
                             "Pos: (0, 0)",
                             {255, 255, 0, 255});
    debugPosText->AddComponent(posText);
    AddObject(debugPosText);

    for (const auto &triggerData : config.triggers)
    {
        GameObject *triggerGo = new GameObject();
        triggerGo->box.x = triggerData.x;
        triggerGo->box.y = triggerData.y;
        triggerGo->box.w = triggerData.width;
        triggerGo->box.h = triggerData.height;

        std::function<bool()> cond = nullptr;
        if (!triggerData.conditionFlag.empty())
        {
            cond = [flag = triggerData.conditionFlag]() { return GameData::GetFlag(flag); };
        }

        triggerGo->AddComponent(new Interactable(
            *triggerGo,
            Interactable::SPACE_OR_CLICK,
            Interactable::REQUIRE_NEAR,
            std::max(triggerData.width, triggerData.height) * 0.7f,
            [targetId = triggerData.targetStageId,
             tx = triggerData.targetSpawnX,
             ty = triggerData.targetSpawnY,
             fadeCfg = triggerData.fade]() {
                auto *stage = static_cast<StageState *>(&Game::GetInstance().GetCurrentState());
                if (stage)
                    stage->TransitionTo(targetId, tx, ty, fadeCfg);
            },
            0.0f,
            cond));

        AddObject(triggerGo);
    }

    for (const auto &npcConfig : config.npcs)
    {
        GameObject *npcGO = new GameObject();
        npcGO->box.x = npcConfig.x;
        npcGO->box.y = npcConfig.y;

        npcGO->AddComponent(new NPC(*npcGO,
                                    npcConfig.spriteFile,
                                    npcConfig.frameRows,
                                    npcConfig.frameCols,
                                    npcConfig.scale,
                                    npcConfig.renderOffsetY,
                                    npcConfig.flipHorizontal));

        if (config.stageId == "mansion_interior" && npcConfig.id == "chefe_policia" && !npcConfig.dialogueJson.empty())
        {
            tutorialIntroDialogueJson = npcConfig.dialogueJson;
        }

        std::function<bool()> cond = nullptr;
        if (!npcConfig.conditionFlag.empty())
        {
            cond = [flag = npcConfig.conditionFlag]() { return GameData::GetFlag(flag); };
        }

        npcGO->AddComponent(new Interactable(
            *npcGO,
            Interactable::SPACE_ONLY,
            Interactable::REQUIRE_NEAR,
            npcConfig.interactRadius,
            [this,
             jsonFile = npcConfig.dialogueJson,
             npcId = npcConfig.id]() {
                if (DialogueBox::isPlaying)
                    return;

                if (npcId == "chefe_policia")
                {
                    const TutorialStep step = GameData::GetTutorialStep();
                    if (step == TutorialStep::TalkToBoss)
                    {
                        StartInitialBossDialogue(jsonFile);
                    }
                    else if (step == TutorialStep::SolveWhisper)
                    {
                        StartPostWhisperBossDialogue();
                    }
                    return;
                }

                if (jsonFile.empty())
                    return;

                GameObject *dialogueController = new GameObject();
                auto onComplete = [npcId]() {
                    if (!npcId.empty())
                    {
                        GameData::SetFlag("falou_com_" + npcId, true);
                    }
                };

                dialogueController->AddComponent(new DialogueBox(*dialogueController, jsonFile, onComplete, npcId));
                Game::GetInstance().GetCurrentState().AddObject(dialogueController);
            },
            0.0f,
            cond));

        AddObject(npcGO);
    }

    if (config.stageId == "mansion_interior")
    {
        GameObject *bossComputer = new GameObject();
        SpriteRenderer *computerSprite = new SpriteRenderer(*bossComputer, "recursos/img/Computador chefe.png");
        computerSprite->SetScale(0.35f, 0.35f);
        computerSprite->SetUseSourceFrameOffset(false);
        bossComputer->AddComponent(computerSprite);
        bossComputer->box.x = 1388.0f;
        bossComputer->box.y = 640.0f;
        AddObject(bossComputer);
    }

    LoadProps(config, false);

    if (config.stageId == "mansion_interior" && 
        GameData::GetTutorialStep() == TutorialStep::TalkToBoss && 
        !GameData::GetFlag("instrucao_andar_mostrada")) 
    {
        GameData::SetFlag("instrucao_andar_mostrada", true);
        GameObject* promptGO = new GameObject();
        promptGO->AddComponent(new Text(*promptGO, "recursos/font/neodgm.ttf", 26, Text::BLENDED, 
            "A/D / Clique: Andar   |   ESPACO: Interagir", {255, 255, 0, 255}));
        promptGO->AddComponent(new InstructionPrompt(*promptGO, 8.0f, Vec2(600.0f, 150.0f)));
        AddObject(promptGO);
    }
}

void StageState::PerformTransitionTo(std::string targetStageId, float spawnX, float spawnY)
{
    popRequested = true;

    if (targetStageId == "WIN_GAME")
    {
        GameData::playerVictory = true;
        Game::GetInstance().Push(new EndState());
    }
    else if (targetStageId == "LOSE_GAME")
    {
        GameData::playerVictory = false;
        Game::GetInstance().Push(new EndState());
    }
    else
    {
        Game::GetInstance().Push(new StageState(targetStageId, spawnX, spawnY));
    }
}

void StageState::TransitionTo(std::string targetStageId, float spawnX, float spawnY, const FadeTransitionConfig &fadeConfig)
{
    if (screenFade.IsActive())
        return;

    if (fadeConfig.fadeIn)
    {
        Game::GetInstance().SetPendingFadeIn({true, fadeConfig.fadeInDuration, fadeConfig.fadeInColor});
    }

    if (fadeConfig.fadeOut)
    {
        screenFade.FadeOut(fadeConfig.fadeOutDuration, fadeConfig.fadeOutColor, [=]() {
            PerformTransitionTo(targetStageId, spawnX, spawnY);
        });
    }
    else
    {
        PerformTransitionTo(targetStageId, spawnX, spawnY);
    }
}

void StageState::StartInitialBossDialogue(const std::string &dialogueJson)
{
    if (DialogueBox::isPlaying || GameData::GetTutorialStep() != TutorialStep::TalkToBoss)
        return;

    tutorialIntroPending = false;
    tutorialIntroTriggered = true;

    GameObject *dialogueController = new GameObject();
    dialogueController->AddComponent(new DialogueBox(
        *dialogueController,
        dialogueJson.empty() ? "recursos/dialogos/clue01.json" : dialogueJson,
        [this]() {
            Inventory::Add("tutorial_notebook_1");
            Inventory::Add("tutorial_notebook_2");
            Inventory::Add("cafe");
            Inventory::Add("bread_de_queijo");
            Inventory::Add("relato_1");
            Inventory::Add("relato_2");
            Inventory::Add("relato_3");
            Inventory::Add("chief_dialogues");
            GameData::SetFlag("falou_com_chefe_policia", true);
            GameData::SetFlag("tutorial_board_unlocked", true);
            if (GameData::AdvanceTutorial(TutorialStep::TalkToBoss, TutorialStep::OpenBoard) &&
                tutorialClueBoardInteractable != nullptr)
            {
                tutorialClueBoardInteractable->SetEnabled(true);
            }

            GameObject* promptGO = new GameObject();
            promptGO->AddComponent(new Text(*promptGO, "recursos/font/neodgm.ttf", 26, Text::BLENDED, 
                "S: abrir palacio mental  |   ESC: fechar telas", {255, 255, 0, 255}));
            promptGO->AddComponent(new InstructionPrompt(*promptGO, 8.0f, Vec2(600.0f, 150.0f)));
            Game::GetInstance().GetCurrentState().AddObject(promptGO);

        },
        "chief"));
    AddObject(dialogueController);
}

void StageState::StartPostWhisperBossDialogue()
{
    if (DialogueBox::isPlaying || GameData::GetTutorialStep() != TutorialStep::SolveWhisper)
        return;

    GameObject *dialogueController = new GameObject();
    dialogueController->AddComponent(new DialogueBox(
        *dialogueController,
        "recursos/dialogos/dia1_3_joca_resolvido.json",
        []() {
            if (GameData::AdvanceTutorial(TutorialStep::SolveWhisper, TutorialStep::TutorialComplete))
            {
                auto *stage = static_cast<StageState *>(&Game::GetInstance().GetCurrentState());
                if (stage)
                    stage->BeginTutorialEndSequence();
            }
        },
        "chief"));
    AddObject(dialogueController);
}

void StageState::StartPostTutorialSequence() {
    GameObject *dialogueController = new GameObject();
    
    dialogueController->AddComponent(new DialogueBox(
        *dialogueController,
        "recursos/dialogos/dia1_4_caso_machado.json", 
        []() {
            Inventory::Add("boletim_vizinha");
            Inventory::Add("cracha_machado");
            Inventory::Add("chave_casa");
        },
        "chief"));
    
    AddObject(dialogueController);
}

void StageState::UpdateTutorialIntro(float dt)
{
    if (!tutorialIntroPending || tutorialIntroTriggered)
        return;
    if (currentStageId != "mansion_interior")
        return;
    if (GameData::GetTutorialStep() != TutorialStep::TalkToBoss)
    {
        tutorialIntroPending = false;
        return;
    }
    if (DialogueBox::isPlaying)
        return;

    tutorialIntroTimer.Update(dt);
    if (tutorialIntroTimer.Get() >= 1.5f)
    {
        StartInitialBossDialogue(tutorialIntroDialogueJson);
    }
}

void StageState::BeginTutorialEndSequence()
{
    if (screenFade.IsActive())
        return;

    screenFade.FadeOut(1.0f, FadeColor::Black, []() {
        Game::GetInstance().Push(new TutorialEndInterludeState());
    });
}

void StageState::LoadAssets()
{
}

void StageState::UpdateWalkable(float dt)
{
    (void)dt;

    for (unsigned i = 0; i < objectArray.size(); i++)
    {
        Collider *colA = objectArray[i]->GetComponent<Collider>();
        if (!colA)
            continue;

        for (unsigned j = i + 1; j < objectArray.size(); j++)
        {
            Collider *colB = objectArray[j]->GetComponent<Collider>();
            if (!colB)
                continue;

            if (Collision::IsColliding(colA->box, colB->box, objectArray[i]->angleDeg, objectArray[j]->angleDeg))
            {
                objectArray[i]->NotifyCollision(*objectArray[j]);
                objectArray[j]->NotifyCollision(*objectArray[i]);
            }
        }
    }
}

void StageState::Render()
{
    for (const auto &object : objectArray)
    {
        if (debugTextVisible || object.get() != debugPosText)
        {
            object->Render();
        }
    }
    screenFade.Render();
}

void StageState::Pause() {}

void StageState::Resume()
{
    if (Player::player)
    {
        Camera::Update(0.0f);
    }
    auto pf = Game::GetInstance().ConsumePendingFadeIn();
    if (pf.active)
    {
        screenFade.FadeIn(pf.duration, pf.color);
    }
}

StageState::~StageState() {}

void StageState::Update(float dt) {
    screenFade.Update(dt);
    if (screenFade.IsActive()) return;

    InputManager &input = InputManager::GetInstance();
    if (input.KeyPress(SDLK_t))
    {
        debugTextVisible = !debugTextVisible;
    }

    UpdateTutorialIntro(dt);

    if (currentStageId == "mansion_interior" && 
        GameData::GetTutorialStep() == TutorialStep::SolveWhisper && 
        !DialogueBox::isPlaying) 
    {
        StartPostWhisperBossDialogue();
    }

    if (GameData::GetTutorialStep() == TutorialStep::TutorialComplete && !postTutorialSequenceTriggered) {
        postTutorialTimer.Update(dt);
        if (postTutorialTimer.Get() >= 1.0f) {
            postTutorialSequenceTriggered = true;
            StartPostTutorialSequence();
        }
    }

    if (DialogueBox::isPlaying) {
        UpdateArray(dt);
        return;
    }

    itemNotifications->Update(dt);
    if (itemNotifications->IsActive()) return;

    WalkableState::Update(dt);

    if (debugPosText && Player::player) {
        Vec2 pos = Player::player->GetPosition();
        auto *text = debugPosText->GetComponent<Text>();
        if (text) {
            text->SetText("Pos: (" + std::to_string((int)pos.x) + ", " + std::to_string((int)pos.y) + ")");
        }
        debugPosText->box.x = Camera::pos.x + 10.0f;
        debugPosText->box.y = Camera::pos.y + 10.0f;
    }
}
