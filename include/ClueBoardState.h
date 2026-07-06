#ifndef CLUEBOARDSTATE_H
#define CLUEBOARDSTATE_H

#include "State.h"

#include <memory>
#include <string>
#include <vector>

struct ClueBoardSlot;

class ClueBoardState : public State {
public:
    ClueBoardState();
    ~ClueBoardState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;

private:
    void AddQuestionPaper();
    void AddQuestions();
    void AddClue(const ClueBoardSlot &slot);
    void ShowPreview(const std::string &entryId);
    void UpdateQuestionInput(float dt);
    void RefreshAnswerText();
    void SubmitAnswer();
    void StartTextInput();
    void StopTextInput();

    std::weak_ptr<GameObject> previewObject;
    std::vector<std::weak_ptr<GameObject>> answerObjects;
    std::vector<std::string> typedAnswers;
    size_t activeQuestion = 0;
    bool textInputActive = false;
};

#endif
