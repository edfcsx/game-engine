#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <SDL2/SDL.h>
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
            // Todo:
            // update the position/velocity as a function of deltatime
            position.x += velocity.x * deltaTime;
            position.y += velocity.y * deltaTime;

            // if (!InTheEdge(deltaTime)) {
            //     position.x += velocity.x * deltaTime;
            //     position.y += velocity.y * deltaTime;
            // }
        }

        void Render() override {}

        bool InTheEdge (float deltaTime) {
            glm::vec2 destPos = glm::vec2(
                static_cast<int>((position.x + (width * scale)) + (velocity.x * deltaTime)),
                static_cast<int>((position.y + (height * scale)) + (velocity.y * deltaTime))
            );

            bool axisXInEdge = destPos.x <= (width * scale) || destPos.x >= WINDOW_WIDTH;
            bool axisYInEdge = destPos.y <= (height * scale) || destPos.y >= WINDOW_HEIGHT;

            if (axisXInEdge || axisYInEdge) {
                return true;
            }

            return false;
        }
};

#endif