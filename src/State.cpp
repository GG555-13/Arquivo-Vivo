#include "State.h"

#include <limits>

State::State() : popRequested(false), quitRequested(false), started(false) {
}

State::~State() {
    objectArray.clear();
}

std::weak_ptr<GameObject> State::AddObject(GameObject* go) {
    std::shared_ptr<GameObject> sharedGo(go);
    objectArray.push_back(sharedGo);
    if (started) {
        sharedGo->Start();
    }
    return std::weak_ptr<GameObject>(sharedGo);
}

std::weak_ptr<GameObject> State::GetObjectPtr(GameObject* go) {
    for (auto& obj : objectArray) {
        if (obj.get() == go) {
            return std::weak_ptr<GameObject>(obj);
        }
    }
    return std::weak_ptr<GameObject>();
}

Interactable* State::GetInteractable(const Interactable::InteractionContext& context) {
    Interactable* selectedInteractable = nullptr;
    float shortestDistance = std::numeric_limits<float>::max();

    for (auto& obj : objectArray) {
        Interactable* interactable = obj->GetComponent<Interactable>();
        if (!interactable || !interactable->CanActivate(context)) {
            continue;
        }

        float distance = 0.0f;
        if (context.hasActor) {
            distance = obj->box.Center().Distance(context.actorPos);
        } else if (context.hasInteractionPoint) {
            distance = obj->box.Center().Distance(context.interactionPoint);
        }

        if (distance < shortestDistance) {
            shortestDistance = distance;
            selectedInteractable = interactable;
        }
    }

    return selectedInteractable;
}

bool State::PopRequested() {
    return popRequested;
}

bool State::QuitRequested() {
    return quitRequested;
}

void State::StartArray() {
    for (unsigned i = 0; i < objectArray.size(); i++) {
        objectArray[i]->Start();
    }
}

void State::UpdateArray(float dt) {
    for (unsigned i = 0; i < objectArray.size(); i++) {
        objectArray[i]->Update(dt);
    }
}

void State::RenderArray() {
    for (auto& obj : objectArray) {
        obj->Render();
    }
}