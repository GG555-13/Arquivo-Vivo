#ifndef MINDPLACESTATE_H
#define MINDPLACESTATE_H

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
    struct DetailObjectEntry
    {
        std::weak_ptr<GameObject> object;
        Vec2 visibleCenter;
        Vec2 hiddenCenter;
    };

    std::vector<DetailObjectEntry> activeDetailObjects;
    bool detailVisible;
    TabGroup tabs;

    void AddDetailSprite(const std::string &imagePath,
                         const Vec2 &visibleCenter,
                         float scaleX,
                         float scaleY);
    void ShowFolderContent(const std::string &imagePath,
                           const Vec2 &visibleCenter,
                           float scaleX,
                           float scaleY);
    std::weak_ptr<GameObject> CreateFolder(const Vec2 &center,
                                           float scale = 0.65f,
                                           const std::string &spritePath = "recursos/img/esboçoPasta.png");
    std::weak_ptr<GameObject> CreateFolder(const Vec2 &center,
                                           std::function<void()> interactable,
                                           float scale = 0.65f,
                                           const std::string &spritePath = "recursos/img/esboçoPasta.png");
    void EnsureInteractFolderDetail();
    void OpenInteractFolderDetail();
    void CloseDetailPanel();
};

#endif