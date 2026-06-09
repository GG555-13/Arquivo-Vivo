#ifndef DETAILPANEL_H
#define DETAILPANEL_H

#include "Vec2.h"

#include <memory>
#include <string>

class GameObject;
class State;

class DetailPanel
{
public:
    DetailPanel(State &owner,
                const Vec2 &defaultCenter,
                float defaultScale,
                const std::string &defaultSprite);

    bool IsVisible() const;
    void Open();
    void Close();

private:
    static constexpr float HIDDEN_OFFSET_Y = 1200.0f;

    State &state;
    std::weak_ptr<GameObject> overlayObject;
    Vec2 visibleCenter;
    Vec2 hiddenCenter;
    bool visible;

    Vec2 defaultCenter;
    float defaultScale;
    std::string defaultSprite;
};

#endif