#ifndef CLUEBOARDDATA_H
#define CLUEBOARDDATA_H

#include "Vec2.h"
#include <string>
#include <vector>

struct ClueBoardSlot {
    std::string entryId;
    Vec2 trayPosition;
    Vec2 targetPosition;
    float snapRadius = 0.0f;
};

struct ClueBoardData {

    static std::vector<std::string> lockedEntries;

    static bool LoadLayout(const std::string &path);
    static const std::vector<ClueBoardSlot> &GetSlots();
    static void ClearLayout();
    static bool IsLocked(const std::string &entryId);
    static void Lock(const std::string &entryId);
};

#endif
