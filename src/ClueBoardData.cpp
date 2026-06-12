#include "ClueBoardData.h"

std::vector<Vec2> ClueBoardData::cluePositions;

std::vector<Vec2> ClueBoardData::GetDefaultPositions() {
    std::vector<Vec2> defaults;
    defaults.push_back(Vec2(260.0f, 200.0f));
    defaults.push_back(Vec2(410.0f, 200.0f));
    defaults.push_back(Vec2(560.0f, 200.0f));
    defaults.push_back(Vec2(335.0f, 340.0f));
    return defaults;
}
