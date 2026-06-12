#ifndef CLUEBOARDDATA_H
#define CLUEBOARDDATA_H

#include "Vec2.h"
#include <vector>

struct ClueBoardData {
    /// Posições das pistas no quadro (coordenadas do mundo do ClueBoardState)
    static std::vector<Vec2> cluePositions;

    /// Posições padrão usadas na primeira abertura
    static std::vector<Vec2> GetDefaultPositions();
};

#endif
