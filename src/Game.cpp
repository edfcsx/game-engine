#include <iostream>
#include "./Constants.h"
#include "./Game.h"
#include "./AssetManager.h"
#include "./EntityManager.h"
#include "./Components/TransformComponent.h"
#include "./Components/SpriteComponent.h"
#include "./Components/KeyboardControlComponent.h"
#include "../lib/glm/glm.hpp"
#include "./Map.h"

EntityManager manager;
AssetManager* Game::assetManager = new AssetManager(&manager);
SDL_Renderer* Game::renderer;
SDL_Event Game::event;
SDL_Rect Game::camera = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
Map* Game::map = new Map("jungle-tiletexture", 3, 32);;

Game::Game() {
    this->isRunning = false;
}

Game::~Game() {
}

bool Game::IsRunning() const {
    return this->isRunning;
}

void Game::initialize(int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error on initializing SDL." << std::endl;
        return;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        std::cerr << "Error on creating SDL window." << std::endl;
        return;
    }

    /**
     * SDL_CreateRenderer 
     * window parameter is equal SDL_WINDOW
     * index param is a monitor number, case user use one or more monitors
     * this -1 indicate to user default driver
    */
    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        std::cerr << "Error on creating SDL renderer." << std::endl;
        return;
    }

    LoadLevel(0);

    isRunning = true;
    return;
}

Entity& player(manager.AddEntity("chopper", PLAYER_LAYER));

void Game::LoadLevel(int loadNumber) {
    assetManager->AddTexture("tank-image", std::string("./assets/images/tank-big-right.png").c_str());
    assetManager->AddTexture("chopper-image", std::string("./assets/images/chopper-spritesheet.png").c_str());
    assetManager->AddTexture("radar-image", std::string("./assets/images/radar.png").c_str());
    assetManager->AddTexture("jungle-tiletexture", std::string("./assets/tilemaps/jungle.png").c_str());
    
    // Entity& newEntity(manager.AddEntity("tank"));
    // newEntity.AddComponent<TransformComponent>(0, 0, 20, 20, 32, 32, 1);
    // newEntity.AddComponent<SpriteComponent>("tank-image");

    Game::map->LoadMap("./assets/tilemaps/jungle.map", 25, 20);

    player.AddComponent<TransformComponent>(320, 160, 0, 0, 32, 32, 3);
    player.AddComponent<SpriteComponent>("chopper-image", 2, 90, true, false);
    player.AddComponent<KeyboardControlComponent>("up", "down", "right", "left", "space");

    Entity& radarEntity(manager.AddEntity("radar", UI_LAYER));
    radarEntity.AddComponent<TransformComponent>(720, 15, 0, 0, 64, 64, 1);
    radarEntity.AddComponent<SpriteComponent>("radar-image", 8, 150, false, true);
}

void Game::processInput() {
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT: {
            isRunning = false;
            break;
        }
        case SDL_KEYDOWN: {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }
        }
        default: {
            break;
        }
    }
}

void Game::Update() {
    // Sleep the execution until we reach the target frame time in milliseconds
    int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - ticksLastFrame);

    // only call delay if we are too fast too process this frame
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
        SDL_Delay(timeToWait);
    }

    // Delta time is the difference in ticks from the last frame converted to seconds
    float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

    // Clamp deltaTime to a maximum value
    deltaTime = (deltaTime > 0.05f) ? 0.05f : deltaTime;
    
    // Sets the new ticks for the current frame to be used in the next pass 
    ticksLastFrame = SDL_GetTicks();

    manager.Update(deltaTime);
    HandleCameraMovement();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);

    // Clear the back buffer
    SDL_RenderClear(renderer);
    
    if (manager.HasNoEntities()) {
        return;
    }

    manager.Render();

    // Swap front and back buffers
    SDL_RenderPresent(renderer);
}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::HandleCameraMovement () {
    TransformComponent* mainPlayerTransform = player.GetComponent<TransformComponent>();
    camera.x = mainPlayerTransform->position.x - (WINDOW_WIDTH / 2);
    camera.y = mainPlayerTransform->position.y - (WINDOW_HEIGHT / 2);

    if (GAME_ENGINE_LOG_ACTIVE) {
        std::cout << "Camera Position -> x: " << camera.x << "y: " << camera.y << std::endl;

        std::cout << "HandleCameraMovement" << std::endl;
        std::cout << "Player position x: " << mainPlayerTransform->position.x << std::endl;
        std::cout << "Player position y: " << mainPlayerTransform->position.y << std::endl;
        std::cout << "Window widh: " << WINDOW_WIDTH << std::endl;
        std::cout << "Window Height: " << WINDOW_HEIGHT << std::endl;
    }

    camera.x = camera.x < 0 ? 0 : camera.x;
    camera.y = camera.y < 0 ? 0 : camera.y;
    camera.x = camera.x > camera.w ? camera.w : camera.x;
    camera.y = camera.y > camera.h ? camera.h : camera.y;

    if (GAME_ENGINE_LOG_ACTIVE) {
        std::cout << "Camera posisition x: " << camera.x << std::endl;
        std::cout << "Camera position y: " << camera.y << std::endl;
        std::cout << "Camera width: " << camera.w << std::endl;
        std::cout << "Camera height: " << camera.h << std::endl;
    }
}
