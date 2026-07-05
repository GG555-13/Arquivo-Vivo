#include "DialogueBox.h"
#include "Game.h"
#include "InputManager.h"
#include "Camera.h"
#include <fstream>
#include <iostream>
#include "json.hpp"
#include "Inventory.h"

using json = nlohmann::json;

bool DialogueBox::isPlaying = false;

DialogueBox::DialogueBox(GameObject& associated,
                         std::string jsonFilePath,
                         std::function<void()> onComplete,
                         std::string historyCharacterId)
    : Component(associated), currentSegment(0), firstFrame(true), isFinished(false),
      uiCreated(false), hasRequestedDelete(false), completionInvoked(false),
      onComplete(onComplete), historyCharacterId(historyCharacterId)
{
    DialogueBox::isPlaying = true;
    LoadJSON(jsonFilePath);
}

DialogueBox::~DialogueBox() {
    if (DialogueBox::isPlaying) {
        DialogueBox::isPlaying = false;
        DestroyUI();
    }
}

void DialogueBox::LoadJSON(std::string filePath) {
    std::ifstream file(filePath);
    if (file.is_open()) {
        json j;
        file >> j;
        for (const auto& item : j) {
            segments.push_back({
                item["name"].get<std::string>(),
                item["text"].get<std::string>(),
                item["portrait"].get<std::string>()
            });
        }
    } else {
        std::cerr << "ERROR: Could not open dialogue file: " << filePath << "\n";
    }
}

void DialogueBox::Start() {
    CreateUI();
    UpdateUI();
}

void DialogueBox::CreateUI() {
    State& state = Game::GetInstance().GetCurrentState();

    //  O RETRATO
    GameObject* pGO = new GameObject();
    portraitGO = state.AddObject(pGO);

    // CAIXA DE FUNDO 
    GameObject* bGO = new GameObject();
    SpriteRenderer* boxSprite = new SpriteRenderer(*bGO, "recursos/img/Dialogbox.png");
    float targetWidth = 700.0f;
    float scale = targetWidth / boxSprite->GetWidth();
    boxSprite->SetScale(scale, scale);
    
    bGO->AddComponent(boxSprite);
    boxGO = state.AddObject(bGO);

    // NOME 
    GameObject* nGO = new GameObject();
    nGO->AddComponent(new Text(*nGO, "recursos/font/neodgm.ttf", 32, Text::BLENDED, "", {255, 200, 0, 255}));
    nameGO = state.AddObject(nGO);

    // 4. TEXTO
    GameObject* tGO = new GameObject();
    tGO->AddComponent(new Text(*tGO, "recursos/font/neodgm.ttf", 24, Text::BLENDED, "", {255, 255, 255, 255}));
    textGO = state.AddObject(tGO);
}

void DialogueBox::UpdateUI() {
    if (currentSegment >= segments.size()) return;

    DialogueSegment& seg = segments[currentSegment];

    if (auto nGO = nameGO.lock()) {
        nGO->GetComponent<Text>()->SetText(seg.speakerName);
    }
    if (auto tGO = textGO.lock()) {
        tGO->GetComponent<Text>()->SetText(seg.text);
    }
    if (auto pGO = portraitGO.lock()) {
        if (SpriteRenderer* sr = pGO->GetComponent<SpriteRenderer>()) pGO->RemoveComponent(sr);
        if (!seg.portraitFile.empty()) pGO->AddComponent(new SpriteRenderer(*pGO, seg.portraitFile));
    }
}

void DialogueBox::PositionUI() {

    // Mudar move a caixa inteira e tudo que está dentro dela
    float boxWidth = 1000.0f;
    float uiBaseX = Camera::pos.x + (1200.0f - boxWidth) / 2.0f; 
    float uiBaseY = Camera::pos.y + 650.0f; 

    // 1. FUNDO DA CAIXA 
    if (auto bGO = boxGO.lock()) { 
        bGO->box.x = uiBaseX + 100.0f; 
        bGO->box.y = uiBaseY - 50.0f; 
    }
    // 2. RETRATO DO PERSONAGEM 
    if (auto pGO = portraitGO.lock()) { 
        pGO->box.x = uiBaseX - 200.0f; 
        pGO->box.y = uiBaseY - 300.0f; 
    }
    
    // 3. NOME DO PERSONAGEM (Texto Amarelo)
    if (auto nGO = nameGO.lock()) { 
        nGO->box.x = uiBaseX + 100.0f; 
        nGO->box.y = uiBaseY + 10.0f;  
    }
    
    // 4. TEXTO DO DIÁLOGO (Texto Branco)
    if (auto tGO = textGO.lock()) { 
        tGO->box.x = uiBaseX + 80.0f; 
        tGO->box.y = uiBaseY + 60.0f;  
    }
}

void DialogueBox::Update(float dt) {
    if (isFinished) {
        endTimer.Update(dt);
        if (endTimer.Get() > 0.1f && !hasRequestedDelete) {
            hasRequestedDelete = true;
            DialogueBox::isPlaying = false; 
            associated.RequestDelete();
        }
        return; 
    }

    PositionUI();

    if (firstFrame) {
        firstFrame = false;
        return; 
    }

    InputManager& input = InputManager::GetInstance();
    
    if (input.KeyPress(SPACE_KEY) || input.MousePress(LEFT_MOUSE_BUTTON)) {
        if (currentSegment < segments.size() - 1) {
            currentSegment++;
            UpdateUI();
        } else {
            isFinished = true;
            DestroyUI();
            if (!completionInvoked) {
                completionInvoked = true;
                if (!historyCharacterId.empty()) {
                    std::string transcript;
                    for (const DialogueSegment &segment : segments) {
                        if (!transcript.empty()) transcript += "\n\n";
                        transcript += segment.speakerName + ":\n" + segment.text;
                    }
                    Inventory::AddDialogueHistory(historyCharacterId, transcript);
                }
                if (onComplete) onComplete();
            }
        }
    }
}

void DialogueBox::Render() {}

void DialogueBox::DestroyUI() {
    if (auto bGO = boxGO.lock()) { bGO->box.x = -10000; bGO->RequestDelete(); }
    if (auto pGO = portraitGO.lock()) { pGO->box.x = -10000; pGO->RequestDelete(); }
    if (auto nGO = nameGO.lock()) { nGO->box.x = -10000; nGO->RequestDelete(); }
    if (auto tGO = textGO.lock()) { tGO->box.x = -10000; tGO->RequestDelete(); }
}
