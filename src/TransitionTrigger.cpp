#include "TransitionTrigger.h"

TransitionTrigger::TransitionTrigger(GameObject &associated,
                                     ActivationType activationType,
                                     float interactionRadius,
                                     std::function<void()> onTransition,
                                     Requirement requirement)
    : Interactable(associated,
                   static_cast<Interactable::ActivationType>(activationType),
                   requirement,
                   interactionRadius,
                   onTransition)
{
}