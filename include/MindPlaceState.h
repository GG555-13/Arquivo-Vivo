#ifndef MINDPLACESTATE_H
#define MINDPLACESTATE_H

#include "DetailPanel.h"
#include "State.h"
#include "TabGroup.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class MindPlaceState : public State
{
public:
    MindPlaceState();
    ~MindPlaceState();

    void LoadAssets();
    void Update(float dt);
    void Render();

    void Start();
    void Pause();
    void Resume();

private:
    TabGroup tabs;
    DetailPanel detailPanel;

    std::weak_ptr<GameObject> CreateFolder(const Vec2 &center,
                                           float scale = 0.65f,
                                           const std::string &spritePath = "recursos/img/esboçoPasta.png");
    std::weak_ptr<GameObject> CreateFolder(const Vec2 &center,
                                           std::function<void()> interactable,
                                           float scale = 0.65f,
                                           const std::string &spritePath = "recursos/img/esboçoPasta.png");
};

#endif