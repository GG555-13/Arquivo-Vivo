#include "WhisperState.h"

#include "BlinkingCaret.h"
#include "Camera.h"
#include "ColoredRect.h"
#include "Game.h"
#include "GameData.h"
#include "GameObject.h"
#include "InputManager.h"
#include "MindPlaceState.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "json.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace
{
std::string NormalizeAnswer(const std::string &answer)
{
    size_t first = 0;
    while (first < answer.size() && std::isspace(static_cast<unsigned char>(answer[first]))) ++first;
    size_t last = answer.size();
    while (last > first && std::isspace(static_cast<unsigned char>(answer[last - 1]))) --last;
    std::string normalized = answer.substr(first, last - first);
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char value) {
        return static_cast<char>(std::tolower(value));
    });
    return normalized;
}

void RemoveLastUtf8Character(std::string &text)
{
    if (text.empty()) return;
    size_t position = text.size() - 1;
    while (position > 0 && (static_cast<unsigned char>(text[position]) & 0xC0) == 0x80) --position;
    text.erase(position);
}

SDL_Color ReadColor(const json &value, SDL_Color fallback)
{
    if (!value.is_object()) return fallback;
    return {
        static_cast<Uint8>(value.value("r", fallback.r)),
        static_cast<Uint8>(value.value("g", fallback.g)),
        static_cast<Uint8>(value.value("b", fallback.b)),
        static_cast<Uint8>(value.value("a", fallback.a))
    };
}

Vec2 ReadVec2(const json &value, Vec2 fallback)
{
    if (!value.is_object()) return fallback;
    return Vec2(value.value("x", fallback.x), value.value("y", fallback.y));
}
}

WhisperState::WhisperState()
    : State()
{
}

WhisperState::~WhisperState()
{
    StopTextInput();
}

void WhisperState::LoadAssets()
{
}

bool WhisperState::LoadConfig(const std::string &path)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cerr << "Whisper config not found: " << path << '\n';
        config.acceptedAnswers = {"cabra"};
        return false;
    }

    try
    {
        json root;
        file >> root;
        config.background = root.value("background", config.background);
        config.font = root.value("font", config.font);
        config.dialogue = root.value("dialogue", config.dialogue);
        config.dash = root.value("dash", config.dash);
        config.incorrectText = root.value("incorrectText", config.incorrectText);
        config.dialogueFontSize = root.value("dialogueFontSize", config.dialogueFontSize);
        config.inputFontSize = root.value("inputFontSize", config.inputFontSize);
        config.feedbackFontSize = root.value("feedbackFontSize", config.feedbackFontSize);
        config.dialogueWrapWidth = root.value("dialogueWrapWidth", config.dialogueWrapWidth);
        config.inputWrapWidth = root.value("inputWrapWidth", config.inputWrapWidth);
        config.maxLength = root.value("maxLength", config.maxLength);
        config.textColor = ReadColor(root.value("textColor", json::object()), config.textColor);
        config.feedbackColor = ReadColor(root.value("feedbackColor", json::object()), config.feedbackColor);
        config.inputBoxColor = ReadColor(root.value("inputBoxColor", json::object()), config.inputBoxColor);
        config.dialoguePosition = ReadVec2(root.value("dialoguePosition", json::object()), config.dialoguePosition);
        config.dashPosition = ReadVec2(root.value("dashPosition", json::object()), config.dashPosition);
        config.inputPosition = ReadVec2(root.value("inputPosition", json::object()), config.inputPosition);
        config.inputBoxPosition = ReadVec2(root.value("inputBoxPosition", json::object()), config.inputBoxPosition);
        config.inputBoxSize = ReadVec2(root.value("inputBoxSize", json::object()), config.inputBoxSize);
        config.feedbackPosition = ReadVec2(root.value("feedbackPosition", json::object()), config.feedbackPosition);
        config.acceptedAnswers.clear();
        if (root.contains("acceptedAnswers") && root["acceptedAnswers"].is_array())
        {
            for (const auto &answer : root["acceptedAnswers"])
            {
                if (answer.is_string() && !answer.get<std::string>().empty())
                {
                    config.acceptedAnswers.push_back(answer.get<std::string>());
                }
            }
        }
        if (config.acceptedAnswers.empty())
        {
            std::cerr << "Whisper config has no accepted answers, using fallback.\n";
            config.acceptedAnswers = {"cabra"};
        }
        return true;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Invalid whisper config " << path << ": " << error.what() << '\n';
        config.acceptedAnswers = {"cabra"};
        return false;
    }
}

void WhisperState::Start()
{
    LoadAssets();
    LoadConfig("recursos/dados/tutorial_whisper.json");
    Camera::pos = Vec2(0, 0);

    AddBackground();
    AddTextObjects();

    StartArray();
    SetInputFocused(false);
    started = true;
}

void WhisperState::AddBackground()
{
    GameObject *background = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*background, config.background);
    const float scaleX = static_cast<float>(Game::GetInstance().GetWidth()) / static_cast<float>(sprite->GetWidth());
    const float scaleY = static_cast<float>(Game::GetInstance().GetHeight()) / static_cast<float>(sprite->GetHeight());
    const float scale = std::max(scaleX, scaleY);
    sprite->SetScale(scale, scale);
    sprite->SetUseSourceFrameOffset(false);
    background->AddComponent(sprite);
    background->box.SetCenter(Vec2(Game::GetInstance().GetWidth() / 2.0f,
                                   Game::GetInstance().GetHeight() / 2.0f));
    AddObject(background);
}

void WhisperState::AddTextObjects()
{
    GameObject *dash = new GameObject();
    dash->box.x = config.dashPosition.x;
    dash->box.y = config.dashPosition.y;
    dash->AddComponent(new Text(*dash, config.font, config.dialogueFontSize, Text::BLENDED,
                                config.dash, config.textColor));
    AddObject(dash);

    GameObject *dialogue = new GameObject();
    dialogue->box.x = config.dialoguePosition.x;
    dialogue->box.y = config.dialoguePosition.y;
    dialogue->AddComponent(new Text(*dialogue, config.font, config.dialogueFontSize, Text::BLENDED,
                                    config.dialogue, config.textColor, config.dialogueWrapWidth));
    AddObject(dialogue);

    GameObject *inputBox = new GameObject();
    inputBox->box.x = config.inputBoxPosition.x;
    inputBox->box.y = config.inputBoxPosition.y;
    inputBox->box.w = config.inputBoxSize.x;
    inputBox->box.h = config.inputBoxSize.y;
    inputBox->AddComponent(new ColoredRect(*inputBox, config.inputBoxColor));
    inputBoxObject = AddObject(inputBox);

    GameObject *answer = new GameObject();
    answer->box.x = config.inputPosition.x;
    answer->box.y = config.inputPosition.y;
    answer->AddComponent(new Text(*answer, config.font, config.inputFontSize, Text::BLENDED,
                                  " ", config.textColor, config.inputWrapWidth));
    answerObject = AddObject(answer);

    GameObject *caret = new GameObject();
    caret->box.x = config.inputPosition.x - 10.0f;
    caret->box.y = config.inputPosition.y + 2.0f;
    caret->box.w = 4.0f;
    caret->box.h = static_cast<float>(config.inputFontSize + 8);
    caret->AddComponent(new BlinkingCaret(*caret));
    caretObject = AddObject(caret);

    GameObject *feedback = new GameObject();
    feedback->box.x = config.feedbackPosition.x;
    feedback->box.y = config.feedbackPosition.y;
    feedback->AddComponent(new Text(*feedback, config.font, config.feedbackFontSize, Text::BLENDED,
                                    "", config.feedbackColor, config.inputWrapWidth));
    feedbackObject = AddObject(feedback);
}

void WhisperState::StartTextInput()
{
    if (!textInputActive)
    {
        SDL_StartTextInput();
        textInputActive = true;
    }
}

void WhisperState::StopTextInput()
{
    if (textInputActive)
    {
        SDL_StopTextInput();
        textInputActive = false;
    }
}

void WhisperState::UpdateAnswerText()
{
    float textWidth = 0.0f;
    if (auto answer = answerObject.lock())
    {
        if (Text *text = answer->GetComponent<Text>())
        {
            text->SetText(typedAnswer.empty() ? " " : typedAnswer);
            if (!typedAnswer.empty())
            {
                textWidth = static_cast<float>(text->GetTextureWidth());
            }
        }
    }
    if (auto caret = caretObject.lock())
    {
        caret->box.x = config.inputPosition.x + textWidth + 4.0f;
        caret->box.y = config.inputPosition.y + 2.0f;
        caret->box.h = static_cast<float>(config.inputFontSize + 8);
        if (BlinkingCaret *component = caret->GetComponent<BlinkingCaret>())
        {
            component->SetEnabled(inputFocused && !solved);
        }
    }
}

void WhisperState::SetFeedback(const std::string &text)
{
    if (auto feedback = feedbackObject.lock())
    {
        if (Text *component = feedback->GetComponent<Text>())
        {
            component->SetText(text);
        }
    }
}

void WhisperState::SetInputFocused(bool value)
{
    if (solved)
    {
        value = false;
    }

    inputFocused = value;
    if (inputFocused)
    {
        StartTextInput();
    }
    else
    {
        StopTextInput();
    }
    UpdateAnswerText();
}

void WhisperState::UpdateFocusFromMouse()
{
    InputManager &input = InputManager::GetInstance();
    if (!input.MousePress(LEFT_MOUSE_BUTTON))
    {
        return;
    }

    std::shared_ptr<GameObject> box = inputBoxObject.lock();
    if (!box)
    {
        return;
    }

    Vec2 mousePoint(input.GetMouseX() + Camera::pos.x, input.GetMouseY() + Camera::pos.y);
    SetInputFocused(box->box.Contains(mousePoint));
}

void WhisperState::UpdateInput(float dt)
{
    (void)dt;
    if (!textInputActive || !inputFocused || solved) return;

    InputManager &input = InputManager::GetInstance();
    const std::string &newText = input.GetTextInput();
    if (!newText.empty())
    {
        const size_t remaining = static_cast<size_t>(config.maxLength) > typedAnswer.size()
            ? static_cast<size_t>(config.maxLength) - typedAnswer.size() : 0;
        typedAnswer += newText.substr(0, remaining);
        SetFeedback("");
        UpdateAnswerText();
    }
    if (input.KeyPress(SDLK_BACKSPACE))
    {
        RemoveLastUtf8Character(typedAnswer);
        SetFeedback("");
        UpdateAnswerText();
    }
    if (input.KeyPress(SDLK_RETURN) || input.KeyPress(SDLK_KP_ENTER))
    {
        SubmitAnswer();
    }
}

void WhisperState::SubmitAnswer()
{
    const std::string submitted = NormalizeAnswer(typedAnswer);
    const bool correct = std::any_of(config.acceptedAnswers.begin(), config.acceptedAnswers.end(),
                                     [&](const std::string &answer) { return NormalizeAnswer(answer) == submitted; });
    if (!correct)
    {
        SetFeedback(config.incorrectText);
        return;
    }

    solved = true;
    SetInputFocused(false);
    UpdateAnswerText();
    SetFeedback("");
    GameData::AdvanceTutorial(TutorialStep::SolveBoard, TutorialStep::SolveWhisper);
    screenFade.FadeOut(1.0f, FadeColor::White, [this]() {
        Game::GetInstance().SetPendingFadeIn({true, 1.0f, FadeColor::White});
        popRequested = true;
    });
}

void WhisperState::Update(float dt)
{
    InputManager &input = InputManager::GetInstance();

    if (input.QuitRequested())
    {
        quitRequested = true;
    }

    screenFade.Update(dt);
    if (popRequested)
    {
        return;
    }
    if (screenFade.IsActive())
    {
        return;
    }

    UpdateFocusFromMouse();

    if (inputFocused && input.KeyPress(ESCAPE_KEY))
    {
        SetInputFocused(false);
        return;
    }

    if (!inputFocused && input.KeyPress(MIND_PLACE_KEY))
    {
        Game::GetInstance().Push(new MindPlaceState());
        return;
    }

    UpdateInput(dt);
    UpdateArray(dt);
}

void WhisperState::Render()
{
    RenderArray();
    screenFade.Render();
}

void WhisperState::Pause()
{
    StopTextInput();
}

void WhisperState::Resume()
{
    if (!solved)
    {
        SetInputFocused(inputFocused);
    }
}
