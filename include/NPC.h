#ifndef NPC_H
#define NPC_H

#include "Character.h"
#include <string>

class GameObject;

class NPC : public Character
{
public:
    NPC(GameObject &associated,
        const std::string &spritePath,
        int frameRows, int frameCols,
        float scale = 1.0f,
        float renderOffsetY = 0.0f);

protected:
    void SetupSprite() override;

private:
    std::string spritePath;
    int frameRows, frameCols;
    float scale;
    float renderOffsetY;
};

#endif
