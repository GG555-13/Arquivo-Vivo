#ifndef CLUEBOARDDATA_H
#define CLUEBOARDDATA_H

#include "Vec2.h"
#include <vector>

struct ClueBoardData {

    static std::vector<Vec2> cluePositions;


    static std::vector<Vec2> GetDefaultPositions();
};

#endif
