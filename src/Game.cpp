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
Entity* mainPlayer = NULL;


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

    LoadLevel(1);

    isRunning = true;
    return;
}


void Game::LoadLevel(int levelNumber) {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math);

    std::string levelName = "Level" + std::to_string(levelNumber);
    lua.script_file("./assets/scripts/" + levelName + ".lua");

    /*********************************************/
    /* LOADS ASSETS FROM LUA CONFIG FILE         */
    /*********************************************/
    sol::table data = lua[levelName];
    sol::table assets = data["assets"];
    
    unsigned int index = 0;
    
    while (true) {
        sol::optional<sol::table> existsIndex = assets[index];

        if (existsIndex == sol::nullopt) {
            break;
        } else {
            sol::table asset = assets[index];
            std::string type = asset["type"];
            std::string id = asset["id"];
            std::string file = asset["file"];

            if (type.compare("texture") == 0) {
                assetManager->AddTexture(id, file.c_str());
            } else if (type.compare("font") == 0) {
                assetManager->AddFont(id, file.c_str(), static_cast<int>(asset["fontSize"]));
            }
        }

        index++;
    }

    /*********************************************/
    /* LOADS MAP FROM LUA CONFIG FILE            */
    /*********************************************/

    sol::table map = data["map"];
    Game::map->LoadTexture(
        map["textureAssetId"],
        map["scale"],
        map["tileSize"]
    );

    Game::map->LoadMap(
        map["file"],
        map["size"]["x"],
        map["size"]["y"]
    );

    /*********************************************/
    /* LOADS ENTITIES FROM LUA CONFIG FILE       */
    /*********************************************/
    sol::table entities = data["entities"];
    index = 0;

    while (true) {
        sol::optional<sol::table> existsIndex = entities[index];

        if (existsIndex == sol::nullopt) {
            break;
        }

        sol::table entity = entities[index];
        std::string name = entity["name"];
        LayerType layer = static_cast<LayerType>(static_cast<int>(entity["layer"]));

        auto& newEntity(manager.AddEntity(name, layer));

        sol::optional<sol::table> existsTransformComponent = entity["components"]["transform"];
        if (existsTransformComponent != sol::nullopt) {
            sol::table transform = entity["components"]["transform"];

            newEntity.AddComponent<TransformComponent>(
                static_cast<int>(transform["position"]["x"]),
                static_cast<int>(transform["position"]["y"]),
                static_cast<int>(transform["velocity"]["x"]),
                static_cast<int>(transform["velocity"]["y"]),
                static_cast<int>(transform["width"]),
                static_cast<int>(transform["height"]),
                static_cast<int>(transform["scale"])
            );
        }

        sol::optional<sol::table> existsSpriteComponent = entity["components"]["sprite"];
        if (existsSpriteComponent != sol::nullopt) {
            sol::table sprite = entity["components"]["sprite"];
            bool isAnimated = static_cast<bool>(sprite["animated"]);

            if (isAnimated) {
                newEntity.AddComponent<SpriteComponent>(
                    sprite["textureAssetId"],
                    sprite["_animated"]["frameCount"],
                    sprite["_animated"]["animationSpeed"],
                    static_cast<bool>(sprite["_animated"]["hasDirections"]),
                    static_cast<bool>(sprite["fixed"])
                );
            } else {
                newEntity.AddComponent<SpriteComponent>(sprite["textureAssetId"], sprite["fixed"]);
            }
        }

        sol::optional<sol::table> existsControlComponent = entity["components"]["input"];
        if (existsControlComponent != sol::nullopt) {
            sol::table keyboard = entity["components"]["input"]["keyboard"];

            std::string upKey = keyboard["up"];
            std::string downKey = keyboard["down"];
            std::string leftKey = keyboard["left"];
            std::string rightKey = keyboard["right"];
            std::string shootKey = keyboard["shoot"];

            newEntity.AddComponent<KeyboardControlComponent>(upKey, downKey, rightKey, leftKey, shootKey);
        }

        sol::optional<sol::table> existsColliderComponent = entity["components"]["collider"];
        if (existsColliderComponent != sol::nullopt) {
            sol::table transform = entity["components"]["transform"];
            sol::table collider = entity["components"]["collider"];

            newEntity.AddComponent<ColliderComponent>(
                collider["tag"],
                transform["position"]["x"],
                transform["position"]["y"],
                transform["width"],
                transform["height"]
            );

        }

        sol::optional<sol::table> existsProjectileEmitterComponent = entity["components"]["projectileEmitter"];
        if (existsProjectileEmitterComponent != sol::nullopt) {
            sol::table projectile = entity["components"]["projectileEmitter"];
            auto& newEntityProjectile(manager.AddEntity(projectile["name"], PROJECTILE_LAYER));

            sol::table projectileTransform = entity["components"]["transform"];
            int posX = static_cast<int>(projectileTransform["position"]["x"]) + ((static_cast<int>(projectileTransform["width"]) / 2) * static_cast<int>(projectileTransform["scale"]));
            int posY = static_cast<int>(projectileTransform["position"]["y"]) + ((static_cast<int>(projectileTransform["height"]) / 2) * static_cast<int>(projectileTransform["scale"]));

            newEntityProjectile.AddComponent<TransformComponent>(
                posX,
                posY,
                0,
                0,
                static_cast<int>(projectile["width"]),
                static_cast<int>(projectile["height"]),
                static_cast<int>(projectileTransform["scale"])
            );

            newEntityProjectile.AddComponent<SpriteComponent>(projectile["textureAssetId"], false);
            newEntityProjectile.AddComponent<ColliderComponent>(projectile["collider"]["tag"], posX, posY, projectile["width"], projectile["height"]);
            newEntityProjectile.AddComponent<ProjectileEmitterComponent>(
                projectile["speed"],
                projectile["angle"],
                projectile["range"],
                projectile["shouldLoop"]
            );
        }

        sol::optional<sol::table> existsLabelComponent = entity["components"]["label"];
        if (existsLabelComponent != sol::nullopt) {
            sol::table label = entity["components"]["label"];

            newEntity.AddComponent<TextLabelComponent>(
                static_cast<int>(label["position"]["x"]),
                static_cast<int>(label["position"]["y"]),
                label["text"],
                label["font"],
                WHITE_COLOR
            );
        }

        index++;
    }

    mainPlayer = manager.GetEntityByName("player");
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
    if (mainPlayer) {
        TransformComponent* mainPlayerTransform = mainPlayer->GetComponent<TransformComponent>();
        camera.x = mainPlayerTransform->position.x - (WINDOW_WIDTH / 2);
        camera.y = mainPlayerTransform->position.y - (WINDOW_HEIGHT / 2);

    
        camera.x = camera.x < 0 ? 0 : camera.x;
        camera.y = camera.y < 0 ? 0 : camera.y;
        camera.x = camera.x > camera.w ? camera.w : camera.x;
        camera.y = camera.y > camera.h ? camera.h : camera.y;
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
