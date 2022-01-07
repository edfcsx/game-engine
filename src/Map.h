#ifndef MAP_H
#define MAP_H

class Map {
    private:
        std::string textureId;
        int scale;
        int tileSize;
    public:
        Map(std::string textureId, int scale, int tileSize);
        ~Map();
        void LoadMap(std::string filePath, int mapSizeX, int mapSizeY);
        void AddTile(int sourceX, int sourceY, int x, int y);
};

#endif