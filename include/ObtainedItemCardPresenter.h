#ifndef OBTAINEDITEMCARDPRESENTER_H
#define OBTAINEDITEMCARDPRESENTER_H

#include "Vec2.h"

#include <memory>
#include <string>

class GameObject;
class State;

class ObtainedItemCardPresenter
{
public:
    explicit ObtainedItemCardPresenter(State &owner);

    bool IsActive() const;
    void Update(float dt);

private:
    enum class Mode
    {
        Idle,
        Opening,
        Visible,
        Closing
    };

    State &state;
    std::weak_ptr<GameObject> cardObject;
    std::weak_ptr<GameObject> imageObject;
    std::weak_ptr<GameObject> titleObject;
    std::weak_ptr<GameObject> descriptionObject;
    Mode mode;
    float elapsed;
    Vec2 cardCenter;
    Vec2 closingStartCenter;

    void EnsureObjects();
    bool OpenNext();
    void Close();
    void SetContent(const std::string &entryId);
    void PositionObjects(const Vec2 &center);
};

#endif
