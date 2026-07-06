#ifndef CLUEBOARDSTATE_H
#define CLUEBOARDSTATE_H

#include "State.h"

#include <memory>
#include <string>

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
    void AddPuzzleSlot(const Vec2 &center);
    void AddClue(const ClueBoardSlot &slot);
    void ShowPreview(const std::string &entryId);
    void UpdateCompletion();

    std::weak_ptr<GameObject> previewObject;
};

#endif
