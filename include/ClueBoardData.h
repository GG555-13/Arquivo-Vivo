#ifndef CLUEBOARDDATA_H
#define CLUEBOARDDATA_H

#include "Rect.h"
#include "Vec2.h"
#include <map>
#include <string>
#include <vector>

struct ClueBoardSlot {
    std::string entryId;
    Vec2 trayPosition;
};

struct ClueBoardData {
    static bool LoadLayout(const std::string &path);
    static const std::vector<ClueBoardSlot> &GetSlots();
    static const Rect &GetPlacementArea();
    static void ClearLayout();
    static bool GetPlacedPosition(const std::string &entryId, Vec2 &position);
    static void SetPlacedPosition(const std::string &entryId, const Vec2 &position);
    static void ClearPlacedPosition(const std::string &entryId);

private:
    static std::map<std::string, Vec2> placedPositions;
};

#endif
