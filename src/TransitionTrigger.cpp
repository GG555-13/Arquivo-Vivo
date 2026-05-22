#include "TransitionTrigger.h"

TransitionTrigger::TransitionTrigger(GameObject &associated,
                                     ActivationType activationType,
                                     float interactionRadius,
                                     std::function<void()> onTransition)
    : Interactable(associated,
                   static_cast<Interactable::ActivationType>(activationType),
                   interactionRadius,
                   onTransition)
{
}