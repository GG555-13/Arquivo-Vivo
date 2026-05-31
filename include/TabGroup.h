#ifndef TABGROUP_H
#define TABGROUP_H

#include "Vec2.h"

#include <memory>
#include <string>
#include <vector>

class GameObject;
class State;

class TabGroup
{
public:
    explicit TabGroup(State& owner);

    int AddTab(Vec2 buttonCenter,
               const std::string& downSprite, const std::string& upSprite);
    void AddContent(int tabIndex, std::weak_ptr<GameObject> obj);
    void SwitchTo(int index);
    int GetActiveIndex() const;

private:
    static constexpr float OFFSCREEN_Y = 2000.0f;

    struct TabEntry
    {
        Vec2 buttonCenter;
        std::weak_ptr<GameObject> buttonDown;
        std::weak_ptr<GameObject> buttonUp;
        std::vector<std::weak_ptr<GameObject>> contents;
    };

    void CancelDraggables(TabEntry& tab);

    State& state;
    std::vector<TabEntry> tabs;
    int activeIndex;
};

#endif
