#include "TileSet.h"
#include <iostream>

TileSet::TileSet(int tileWidth, int tileHeight, const std::string &file)
    : tileWidth(tileWidth), tileHeight(tileHeight), tileCount(0)
{
    tileSet.Open(file);
    
    if (tileSet.IsOpen())
    {
        int columns = tileSet.GetWidth() / tileWidth;
        int rows = tileSet.GetHeight() / tileHeight;
        
        tileCount = columns * rows;
        
        tileSet.SetFrameCount(columns, rows);
        tileSet.SetCameraFollower(true);
    }
}

void TileSet::RenderTile(unsigned index, float x, float y)
{

    if (index < static_cast<unsigned>(tileCount))
    {

        tileSet.SetFrame(index);
        

        tileSet.Render(static_cast<int>(x), static_cast<int>(y));
    }
}

int TileSet::GetTileWidth() const
{
    return tileWidth;
}

int TileSet::GetTileHeight() const
{
    return tileHeight;
}