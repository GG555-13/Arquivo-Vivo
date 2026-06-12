#ifndef TRANSITIONTRIGGER_H
#define TRANSITIONTRIGGER_H

#include "Component.h"
#include <string>

class TransitionTrigger : public Component {
public:
    TransitionTrigger(GameObject& associated, std::string targetStageId);
    
    void Update(float dt) override;
    void Render() override;
    void NotifyCollision(GameObject& other) override;

private:
    std::string targetStageId;
};

#endif