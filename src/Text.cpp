#include "Text.h"
#include "Game.h"
#include "Resources.h"
#include "Camera.h"

#include <algorithm>

Text::Text(GameObject& associated, std::string fontFile, int fontSize, TextStyle style, std::string text, SDL_Color color)
    : Component(associated), texture(nullptr), text(text), style(style), fontFile(fontFile), fontSize(fontSize), color(color), wrapWidth(0) {
    RemakeTexture();
}

Text::Text(GameObject& associated, std::string fontFile, int fontSize, TextStyle style,
           std::string text, SDL_Color color, int wrapWidth)
    : Component(associated), texture(nullptr), text(text), style(style), fontFile(fontFile), fontSize(fontSize), color(color), wrapWidth(wrapWidth) {
    RemakeTexture();
}

Text::~Text() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
}

void Text::Update(float dt) {}

void Text::Render() {
    if (texture != nullptr) {
        const int visibleHeight = viewportHeight > 0
            ? std::min(viewportHeight, textureHeight - verticalOffset)
            : textureHeight;
        if (visibleHeight <= 0) return;
        SDL_Rect clipRect = {0, verticalOffset, textureWidth, visibleHeight};
        SDL_Rect dstRect = {
            (int)(associated.box.x - Camera::pos.x),
            (int)(associated.box.y - Camera::pos.y),
            clipRect.w,
            visibleHeight
        };
        SDL_RenderCopyEx(Game::GetInstance().GetRenderer(), texture, &clipRect, &dstRect, associated.angleDeg, nullptr, SDL_FLIP_NONE);
    }
}

void Text::SetText(std::string text) {
    this->text = text;
    verticalOffset = 0;
    RemakeTexture();
}

void Text::SetColor(SDL_Color color) {
    this->color = color;
    RemakeTexture();
}

void Text::SetStyle(TextStyle style) {
    this->style = style;
    RemakeTexture();
}

void Text::SetFontFile(std::string fontFile) {
    this->fontFile = fontFile;
    RemakeTexture();
}

void Text::SetFontSize(int fontSize) {
    this->fontSize = fontSize;
    RemakeTexture();
}

void Text::SetWrapWidth(int width) {
    wrapWidth = width;
    RemakeTexture();
}

void Text::SetViewportHeight(int height) {
    viewportHeight = std::max(0, height);
    SetVerticalOffset(verticalOffset);
    associated.box.h = viewportHeight > 0
        ? std::min(viewportHeight, textureHeight)
        : textureHeight;
}

void Text::SetVerticalOffset(int offset) {
    const int maxOffset = viewportHeight > 0
        ? std::max(0, textureHeight - viewportHeight)
        : 0;
    verticalOffset = std::max(0, std::min(offset, maxOffset));
}

void Text::ScrollVertical(int amount) {
    SetVerticalOffset(verticalOffset + amount);
}

void Text::RemakeTexture() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    font = Resources::GetFont(fontFile, fontSize);
    if (!font) return; 

    SDL_Surface* surface = nullptr;

    switch (style) {
        case SOLID:
            surface = TTF_RenderUTF8_Solid(font.get(), text.c_str(), color);
            break;
        case SHADED:
            surface = TTF_RenderUTF8_Shaded(font.get(), text.c_str(), color, {0, 0, 0, 255}); 
            break;
        case BLENDED:
            surface = wrapWidth > 0
                ? TTF_RenderUTF8_Blended_Wrapped(font.get(), text.c_str(), color, static_cast<Uint32>(wrapWidth))
                : TTF_RenderUTF8_Blended(font.get(), text.c_str(), color);
            break;
    }

    if (surface != nullptr) {
        texture = SDL_CreateTextureFromSurface(Game::GetInstance().GetRenderer(), surface);
        textureWidth = surface->w;
        textureHeight = surface->h;
        associated.box.w = textureWidth;
        associated.box.h = viewportHeight > 0
            ? std::min(viewportHeight, textureHeight)
            : textureHeight;
        SetVerticalOffset(verticalOffset);
        SDL_FreeSurface(surface);
    } else {
        textureWidth = 0;
        textureHeight = 0;
        associated.box.w = 0;
        associated.box.h = 0;
        verticalOffset = 0;
    }
}
