#ifndef DETAILPANEL_H
#define DETAILPANEL_H

#include "Vec2.h"

#include <memory>
#include <string>

class GameObject;
class State;
struct DetailContent;

class DetailPanel
{
public:
    DetailPanel(State &owner,
                const Vec2 &defaultCenter,
                float defaultScale,
                const std::string &defaultSprite);

    bool IsVisible() const;
    void Open();
    void Open(const DetailContent &content);
    void Close();

private:
    static constexpr float HIDDEN_OFFSET_Y = 1200.0f;

    State &state;
    std::weak_ptr<GameObject> overlayObject;
    std::weak_ptr<GameObject> contentImageObject;
    std::weak_ptr<GameObject> titleObject;
    std::weak_ptr<GameObject> descriptionObject;
    Vec2 visibleCenter;
    Vec2 hiddenCenter;
    bool visible;

    Vec2 defaultCenter;
    float defaultScale;
    std::string defaultSprite;
};

#endif
