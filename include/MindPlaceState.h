#ifndef MINDPLACESTATE_H
#define MINDPLACESTATE_H

#include "State.h"

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

    void AddDetailSprite(const std::string &imagePath,
                         const Vec2 &visibleCenter,
                         float scaleX,
                         float scaleY);
    void EnsureInteractFolderDetail();
    void OpenInteractFolderDetail();
    void CloseDetailPanel();
};

#endif