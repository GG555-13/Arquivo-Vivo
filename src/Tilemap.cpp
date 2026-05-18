#include "TileMap.h"
#include "GameObject.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Camera.h"

TileMap::TileMap(GameObject &associated, const std::string &file, TileSet *tileSet)
    : Component(associated), tileSet(tileSet), mapWidth(0), mapHeight(0), mapDepth(0)
{
    Load(file);
}

void TileMap::Load(const std::string &file)
{
    std::ifstream mapFile(file);

    if (!mapFile.is_open())
    {
        std::cerr << "Erro ao abrir arquivo de mapa: " << file << std::endl;
        return;
    }

    char comma;
    mapFile >> mapWidth >> comma >> mapHeight >> comma >> mapDepth >> comma;

    tileMatrix.clear();
    tileMatrix.reserve(mapWidth * mapHeight * mapDepth);

    std::string line;
    std::getline(mapFile, line);

    while (std::getline(mapFile, line))
    {
        std::stringstream ss(line);
        int tileIndex;

        while (ss >> tileIndex)
        {
            tileMatrix.push_back(tileIndex);

            if (ss.peek() == ',')
            {
                ss.ignore();
            }
        }
    }

    mapFile.close();
}

void TileMap::SetTileSet(TileSet *newTileSet)
{
    tileSet.reset(newTileSet);
}

int &TileMap::At(int x, int y, int z)
{

    int index = z * (mapWidth * mapHeight) + y * mapWidth + x;
    return tileMatrix[index];
}

void TileMap::RenderLayer(int layer)
{
    if (!tileSet)
        return;

    int tileW = tileSet->GetTileWidth();
    int tileH = tileSet->GetTileHeight();
    float parallaxFactor = (layer == 0) ? 0.5f : 1.0f;

    for (int j = 0; j < mapHeight; ++j)
    {
        for (int i = 0; i < mapWidth; ++i)
        {
            int tileIndex = At(i, j, layer);

            if (tileIndex >= 0)
            {
                float renderX = (i * tileW) - (Camera::pos.x * parallaxFactor);
                float renderY = (j * tileH) - (Camera::pos.y * parallaxFactor);
                tileSet->RenderTile(tileIndex, renderX, renderY);
            }
        }
    }
}

void TileMap::Render()
{

    for (int layer = 0; layer < mapDepth; ++layer)
    {
        RenderLayer(layer);
    }
}

void TileMap::Update(float /*dt*/)
{
}

int TileMap::GetWidth() const
{
    return mapWidth;
}

int TileMap::GetHeight() const
{
    return mapHeight;
}

int TileMap::GetDepth() const
{
    return mapDepth;
}