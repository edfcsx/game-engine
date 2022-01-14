#include <iostream>
#include "./Constants.h"
#include "./Game.h"
#include "./AssetManager.h"
#include "./EntityManager.h"
#include "./Components/TransformComponent.h"
#include "./Components/SpriteComponent.h"
#include "./Components/KeyboardControlComponent.h"
#include "./Components/ColliderComponent.h"
#include "./Components/TextLabelComponent.h"
#include "./Components/ProjectileEmitterComponent.h"
#include "../lib/glm/glm.hpp"
#include "./Map.h"

EntityManager manager;
AssetManager* Game::assetManager = new AssetManager(&manager);
SDL_Renderer* Game::renderer;
SDL_Event Game::event;
SDL_Rect Game::camera = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
Map* Game::map = new Map();

Game::Game() {
    this->isRunning = false;
    this->isDebugEnabled = true;
}

Game::~Game() {
}

bool Game::IsRunning() const {
    return this->isRunning;
}

bool Game::IsDebugEnabled() const {
    return this->isDebugEnabled;
}

void Game::initialize(int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error on initializing SDL." << std::endl;
        return;
    }

    if (TTF_Init() != 0) {
        std::cerr << "Error on initialize SDL TTF" << std::endl;
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
    assetManager->AddTexture("projectile-image", std::string("./assets/images/bullet-enemy.png").c_str());

    assetManager->AddFont("charriot-font", std::string("./assets/fonts/charriot.ttf").c_str(), 22);
    
    Game::map->LoadTexture("jungle-tiletexture", 3, 32);
    Game::map->LoadMap("./assets/tilemaps/jungle.map", 25, 20);

    player.AddComponent<TransformComponent>(320, 160, 0, 0, 32, 32, 3);
    player.AddComponent<SpriteComponent>("chopper-image", 2, 90, true, false);
    player.AddComponent<KeyboardControlComponent>("up", "down", "right", "left", "space");
    player.AddComponent<ColliderComponent>("PLAYER", 320, 160, 32, 32);

    Entity& tankEnemy1(manager.AddEntity("tank", ENEMY_LAYER));
    tankEnemy1.AddComponent<TransformComponent>(300, 730, 0, 0, 32, 32, 2);
    tankEnemy1.AddComponent<SpriteComponent>("tank-image");
    tankEnemy1.AddComponent<ColliderComponent>("ENEMY", 400, 300, 32, 32);

    Entity& projectile(manager.AddEntity("projectile", PROJECTILE_LAYER));
    projectile.AddComponent<TransformComponent>(300+(16*2), 730+(16*2), 0, 0, 4, 4, 2);
    projectile.AddComponent<SpriteComponent>("projectile-image");
    projectile.AddComponent<ColliderComponent>("PROJECTILE", 300+(16*2), 730+(16*2), 4, 4);
    projectile.AddComponent<ProjectileEmitterComponent>(250, 35, 800, true);

    Entity& radarEntity(manager.AddEntity("radar", UI_LAYER));
    radarEntity.AddComponent<TransformComponent>(720, 15, 0, 0, 64, 64, 1);
    radarEntity.AddComponent<SpriteComponent>("radar-image", 8, 150, false, true);

    Entity& labelLevelName(manager.AddEntity("LabelLevelName", UI_LAYER));
    labelLevelName.AddComponent<TextLabelComponent>(10, 10, "First Level...", "charriot-font", WHITE_COLOR);
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

            if (event.key.keysym.sym == SDLK_F1) {
                isDebugEnabled = !isDebugEnabled;
                std::cout << "debug mode is: " << IsDebugEnabled() << std::endl;
                break;
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
    CheckCollisions();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);

    // Clear the back buffer
    SDL_RenderClear(renderer);
    
    if (manager.HasNoEntities()) {
        return;
    }

    manager.Render();

    if (IsDebugEnabled()) {
        RenderDebugs();
    }

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

void Game::CheckCollisions() {
    CollisionType CollisionType = manager.CheckCollisions();

    if (CollisionType == PLAYER_ENEMY_COLLISION) {
        std::cout << "Game Over" << std::endl;
        isRunning = false;
    }

    if (CollisionType == PLAYER_PROJECTILE_COLLISION) {
        std::cout << "Game Over" << std::endl;
        isRunning = false;
    }
}

void Game::RenderDebugs() {
    manager.RenderEntitiesDebugs();
}
