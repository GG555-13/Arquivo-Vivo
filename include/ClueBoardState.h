#ifndef CLUEBOARDSTATE_H
#define CLUEBOARDSTATE_H

#include "State.h"
#include <memory>
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


    void SaveCluePositions();

    std::vector<std::weak_ptr<GameObject>> clueObjects;
};

#endif
