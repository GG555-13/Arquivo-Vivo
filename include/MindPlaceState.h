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

    std::weak_ptr<GameObject> CreateEntry(const Vec2 &center,
                                          const std::string &iconPath,
                                          std::function<void()> interactable,
                                          float scale = 0.22f);
    std::weak_ptr<GameObject> CreateInventoryEntry(const Vec2 &center,
                                                   const std::string &entryId);

    std::weak_ptr<GameObject> CreateFolder(const Vec2 &center,
                                           float scale = 0.22f,
                                           const std::string &spritePath = "recursos/img/Pasta.png");
    std::weak_ptr<GameObject> CreateFolder(const Vec2 &center,
                                           std::function<void()> interactable,
                                           float scale = 0.22f,
                                           const std::string &spritePath = "recursos/img/Pasta.png");
};

#endif
