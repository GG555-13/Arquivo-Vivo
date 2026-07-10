#ifndef WHISPERSTATE_H
#define WHISPERSTATE_H

#include "SDL_include.h"
#include "State.h"
#include "Vec2.h"

#include <memory>
#include <string>
#include <vector>

class GameObject;

class WhisperState : public State
{
public:
    WhisperState();
    ~WhisperState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;

private:
    struct Config
    {
        std::string background = "recursos/img/sussurro1.png";
        std::string font = "recursos/font/SpecialElite-Regular.ttf";
        std::string dialogue = "— Quem sussurra esse nome?";
        std::string dash = "—";
        std::vector<std::string> acceptedAnswers;
        std::string incorrectText = "Esse nome não encontra eco.";
        int dialogueFontSize = 30;
        int inputFontSize = 32;
        int feedbackFontSize = 22;
        int dialogueWrapWidth = 500;
        int inputWrapWidth = 260;
        int maxLength = 32;
        SDL_Color textColor = {45, 35, 30, 255};
        SDL_Color feedbackColor = {130, 45, 45, 255};
        SDL_Color inputBoxColor = {95, 95, 95, 90};
        Vec2 dialoguePosition = Vec2(525.0f, 160.0f);
        Vec2 dashPosition = Vec2(485.0f, 160.0f);
        Vec2 inputPosition = Vec2(915.0f, 615.0f);
        Vec2 inputBoxPosition = Vec2(500.0f, 560.0f);
        Vec2 inputBoxSize = Vec2(260.0f, 58.0f);
        Vec2 feedbackPosition = Vec2(860.0f, 670.0f);
    };

    bool LoadConfig(const std::string &path);
    void AddBackground();
    void AddTextObjects();
    void StartTextInput();
    void StopTextInput();
    void UpdateAnswerText();
    void SetFeedback(const std::string &text);
    void SetInputFocused(bool value);
    void UpdateFocusFromMouse();
    void UpdateInput(float dt);
    void SubmitAnswer();

    Config config;
    std::weak_ptr<GameObject> answerObject;
    std::weak_ptr<GameObject> feedbackObject;
    std::weak_ptr<GameObject> caretObject;
    std::weak_ptr<GameObject> inputBoxObject;
    std::string typedAnswer;
    bool textInputActive = false;
    bool inputFocused = false;
    bool solved = false;
};

#endif
