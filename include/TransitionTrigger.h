#ifndef TRANSITION_TRIGGER_H
#define TRANSITION_TRIGGER_H

#include "Interactable.h"

#include <functional>

class TransitionTrigger : public Interactable
{
public:
    enum ActivationType
    {
        SPACE_ONLY,
        CLICK_ONLY,
        SPACE_OR_CLICK,
        AUTO_ENTER
    };

    TransitionTrigger(GameObject &associated,
                      ActivationType activationType,
                      float interactionRadius,
                      std::function<void()> onTransition);
};

#endif