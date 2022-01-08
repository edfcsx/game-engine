#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <SDL2/SDL.h>
#include <iostream>
#include "../EntityManager.h"
#include "../../lib/glm/glm.hpp"
#include "../Game.h"
#include "../Constants.h"

class TransformComponent: public Component {
    public:
        glm::vec2 position;
        glm::vec2 velocity;
        int width;
        int height;
        int scale;
        
        TransformComponent(int posX, int posY, int velX, int velY, int w, int h, int s) {
            position = glm::vec2(posX, posY);
            velocity = glm::vec2(velX, velY);
            width = w;
            height = h;
            scale = s;
        }

        void initialize() {

        }

        void Update(float deltaTime) override {
            if (!InTheEdge(deltaTime)) {
                position.x += velocity.x * deltaTime;
                position.y += velocity.y * deltaTime;
            }
        }

        void Render() override {}

        bool InTheEdge (float deltaTime) {
            glm::vec2 destPos = glm::vec2(
                static_cast<int>((position.x + Game::camera.x)  + (velocity.x * deltaTime)),
                static_cast<int>((position.y + Game::camera.y)  + (velocity.y * deltaTime))
            );
            
            int mapSizeX = (Game::map->tileSize * Game::map->scale) * Game::map->mapSizeX;
            int mapSizeY = (Game::map->tileSize * Game::map->scale) * Game::map->mapSizeY;

            int objectWidthSize = width * scale;
            int objectHeightSize = height * scale;

            bool axisXLeftInEdge = destPos.x <= 0;
            bool axisXRightInEdge = destPos.x >= mapSizeX - objectWidthSize;
            bool axisYTopInEdge = destPos.y <= 0;
            bool axisYBottomInEdge = destPos.y >= mapSizeY - (objectHeightSize * 2);

            if (axisXLeftInEdge || axisXRightInEdge || axisYTopInEdge || axisYBottomInEdge) {
                return true;
            }

            return false;
        }
};

#endif