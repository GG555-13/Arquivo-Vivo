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
    /// Cria uma pista arrastável na posição dada
    std::weak_ptr<GameObject> AddClue(const Vec2 &center);

    /// Salva posições atuais das pistas no ClueBoardData
    void SaveCluePositions();

    std::vector<std::weak_ptr<GameObject>> clueObjects;
};

#endif
