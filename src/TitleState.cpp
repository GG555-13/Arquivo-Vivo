#include "TitleState.h"
#include "StageState.h"
#include "Game.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "InputManager.h"
#include <Camera.h>
#include <Text.h>

TitleState::TitleState() : State()
{
}

TitleState::~TitleState()
{
}

void TitleState::Start()
{
    LoadAssets();

    Camera::Unfollow(); 
    Camera::pos = Vec2(0, 0);

    GameObject *go = new GameObject();
    go->box.x = 0;
    go->box.y = 0;

    go->AddComponent(new SpriteRenderer(*go, "recursos/img/Title.png"));
    AddObject(go);

    GameObject* textGO = new GameObject();
    textGO->box.x = 300; 
    textGO->box.y = 500;
    SDL_Color color = {255, 255, 255, 255};
    Text* text = new Text(*textGO, "recursos/font/neodgm.ttf", 40, Text::BLENDED, "Pressione Espaco para Iniciar", color); 

    textGO->AddComponent(text);
    AddObject(textGO);

    StartArray();
    started = true;
}

void TitleState::LoadAssets()
{
}

void TitleState::Update(float dt)
{
    UpdateArray(dt);

    textTimer.Update(dt);
    if (objectArray.size() > 1) { 
        auto textGO = objectArray[1]; 
        if (textTimer.Get() < 0.5f) {
            Text* txt = textGO->GetComponent<Text>();
            if(txt) txt->SetColor({255, 255, 255, 255});
        } else {
            Text* txt = textGO->GetComponent<Text>();
            if(txt) txt->SetColor({0, 0, 0, 0}); 
        }
        if (textTimer.Get() > 1.0f) textTimer.Restart();
    }

    InputManager &input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY))
    {
        quitRequested = true;
    }

    if (input.KeyPress(SPACE_KEY))
    {
        Game::GetInstance().Push(new StageState());
    }
}

void TitleState::Render()
{
    RenderArray();
}

void TitleState::Pause() {}
void TitleState::Resume() {}