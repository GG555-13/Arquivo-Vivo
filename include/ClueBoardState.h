#ifndef CLUEBOARDSTATE_H
#define CLUEBOARDSTATE_H

#include "State.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

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

    std::weak_ptr<GameObject> AddClue(const Vec2 &center);
    std::weak_ptr<GameObject> AddClue(const std::string &entryId,
                                     const Vec2 &initialPosition);


    void SaveCluePositions();
    void UpdateProgressText();

    std::vector<std::weak_ptr<GameObject>> clueObjects;
    std::vector<std::pair<std::string, std::weak_ptr<GameObject>>> entryClueObjects;
    std::weak_ptr<GameObject> progressTextObject;
};

#endif
