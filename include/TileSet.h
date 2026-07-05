#ifndef TILESET_H
#define TILESET_H

#include "Sprite.h"
#include <string>

class TileSet
{
public:
    TileSet(int tileWidth, int tileHeight, const std::string &file);
    
    void RenderTile(unsigned index, float x, float y);
    
    int GetTileWidth() const;
    int GetTileHeight() const;

private:
    Sprite tileSet;
    int tileWidth;
    int tileHeight;
    int tileCount;
};

#endif