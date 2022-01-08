#ifndef MAP_H
#define MAP_H

#include <string>

class Map {
    private:
        std::string textureId;
    public:
        int mapSizeX;
        int mapSizeY;
        int tileSize;
        int scale;
        Map();
        ~Map();
        void LoadTexture(std::string textureId, int scale, int tileSize);
        void LoadMap(std::string filePath, int mapSizeX, int mapSizeY);
        void AddTile(int sourceX, int sourceY, int x, int y);
};

#endif