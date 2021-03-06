#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../lib/lua/sol.hpp"
#include "./Map.h"

class AssetManager;

class Game {
    private:
        bool isRunning;
        SDL_Window *window;
    public:
        Game();
        ~Game();
        int ticksLastFrame = 0;
        bool isDebugEnabled;
        bool IsRunning() const;
        bool IsDebugEnabled() const;
        static SDL_Renderer *renderer;
        static AssetManager* assetManager;
        static SDL_Event event;
        static SDL_Rect camera;
        static Map* map;
        void LoadLevel(int levelNumber);
        void initialize(int width, int height);
        void processInput();
        void Update();
        void Render();
        void Destroy();
        void HandleCameraMovement();
        void CheckCollisions();
        void RenderDebugs();
};

#endif