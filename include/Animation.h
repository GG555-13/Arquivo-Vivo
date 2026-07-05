#ifndef ANIMATION_H
#define ANIMATION_H

#include "SDL_include.h"
class Animation
{
public:
    Animation(int frameStart, int frameEnd, float frameTime, SDL_RendererFlip flip = SDL_FLIP_NONE)
        : frameStart(frameStart), frameEnd(frameEnd), frameTime(frameTime), flip(flip) {}

    int frameStart;
    int frameEnd;
    float frameTime;
    SDL_RendererFlip flip;
};

#endif