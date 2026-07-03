#include "NPC.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Animator.h"

NPC::NPC(GameObject &associated,
         const std::string &spritePath,
         int frameRows, int frameCols,
         float scale,
         float renderOffsetY)
    : Character(associated),
      spritePath(spritePath),
      frameRows(frameRows),
      frameCols(frameCols),
      scale(scale),
      renderOffsetY(renderOffsetY)
{
    boundsEnabled = false;

    SetupSprite();
}

void NPC::SetupSprite()
{
    SpriteRenderer* sr = new SpriteRenderer(associated, spritePath, frameRows, frameCols);
    sr->SetUseSourceFrameOffset(false);
    sr->SetScale(scale, scale);
    if (renderOffsetY != 0.0f)
        sr->SetRenderOffsetY(renderOffsetY);
    associated.AddComponent(sr);

    if (frameCols > 1 || frameRows > 1)
    {
        Animator *animator = new Animator(associated);
        animator->AddAnimation("idle", Animation(0, 0, 0));
        animator->SetAnimation("idle");
        associated.AddComponent(animator);
    }

    associated.box.w = sr->GetWidth();
    associated.box.h = sr->GetHeight();
}
