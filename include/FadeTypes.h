#ifndef FADETYPES_H
#define FADETYPES_H

enum class FadeColor { Black, White };

struct PendingFadeIn {
    bool active = false;
    float duration = 0.0f;
    FadeColor color = FadeColor::Black;
};

#endif
