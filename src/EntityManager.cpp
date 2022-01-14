#include <SDL2/SDL.h>
#include <iostream>
#include "./Constants.h"
#include "./EntityManager.h"
#include "Entity.h"
#include "./Components/ColliderComponent.h"
#include "./Components/TransformComponent.h"
#include "./Components/SpriteComponent.h"
#include "./Components/TextLabelComponent.h"
#include "./Collision.h"
#include "./Game.h"

void EntityManager::ClearData() {
    for (auto& entity: entities) {
        entity->Destroy();
    }
}

bool EntityManager::HasNoEntities() {
    return entities.size() == 0;
}

void EntityManager::Update(float deltaTime) {
    for (auto& entity: entities) {
        entity->Update(deltaTime);
    }

    if (GAME_ENGINE_LOG_ACTIVE) {
        EntityManager::ListAllEntities();
    }
}

void EntityManager::Render() {
    for (int layerNumber = 0; layerNumber < NUM_LAYERS; layerNumber++) {
        for (auto& entity: GetEntitiesByLayer(static_cast<LayerType>(layerNumber))) {
            entity->Render();
        }
    }
}

Entity& EntityManager::AddEntity(std::string entityName, LayerType layer) {
    Entity *entity = new Entity(*this, entityName, layer);
    entities.emplace_back(entity);
    return *entity;
}

std::vector<Entity*> EntityManager::GetEntities() const {
    return entities;
}

std::vector<Entity*> EntityManager::GetEntitiesByLayer(LayerType layer) const {
    std::vector<Entity*> selectedEntitties;

    for (auto& entity: entities) {
        if (entity->layer == layer) {
            selectedEntitties.emplace_back(entity);
        }
    }

    return selectedEntitties;
}

unsigned int EntityManager::GetEntityCount() {
    return entities.size();
}

void EntityManager::ListAllEntities () const {
    unsigned int i = 0;

    for (auto& entity: entities) {
        std::cout << "Entity[" << i << "]: " << entity->name << std::endl;
        entity->ListAllComponents();
        i++;
    }
}

CollisionType EntityManager::CheckCollisions () const {
    for (int i = 0; i < entities.size() - 1; i++) {
        auto& thisEntity = entities[i];

        if (thisEntity->HasComponent<ColliderComponent>()) {
            ColliderComponent* thisCollider = thisEntity->GetComponent<ColliderComponent>();

            for (int j = i + 1; j < entities.size(); j++) {
                auto& thatEntity = entities[j];

                if (thisEntity->name.compare(thatEntity->name) != 0 && thatEntity->HasComponent<ColliderComponent>()) {
                    ColliderComponent* thatCollider = thatEntity->GetComponent<ColliderComponent>();

                    if (Collision::checkRectangleCollision(thisCollider->collider, thatCollider->collider)) {
                        if (thisCollider->colliderTag.compare("PLAYER") == 0 && thatCollider->colliderTag.compare("ENEMY") == 0) {
                            return PLAYER_ENEMY_COLLISION;
                        }

                        if (thisCollider->colliderTag.compare("PLAYER") == 0 && thatCollider->colliderTag.compare("PROJECTILE") == 0) {
                            return PLAYER_PROJECTILE_COLLISION;
                        }

                        if (thisCollider->colliderTag.compare("ENEMY") == 0 && thatCollider->colliderTag.compare("FRIENDLY_PROJECTILE") == 0) {
                            return ENEMY_PROJECTILE_COLLISION;
                        }

                        if (thisCollider->colliderTag.compare("PLAYER") == 0 && thatCollider->colliderTag.compare("LEVEL_COMPLETE") == 0) {
                            return PLAYER_LEVEL_COMPLETE_COLLISION;
                        }
                    }
                }
            }
        }
    }

    return NO_COLLISION;
}

void EntityManager::RenderEntitiesDebugs() const {
    for (auto& entity: entities) {
        if (entity->name.compare("Tile") != 0 && !entity->HasComponent<TextLabelComponent>()) {
            TransformComponent* transform = entity->GetComponent<TransformComponent>();
            bool isFixed = false;

            if (entity->HasComponent<SpriteComponent>()) {
                SpriteComponent* sprite = entity->GetComponent<SpriteComponent>();
                isFixed = sprite->IsFixed();
            }

            SDL_Rect transformRectDebug = {
                transform->position.x - (isFixed ? 0 : Game::camera.x) - 2,
                transform->position.y - (isFixed ? 0 : Game::camera.y) - 2,
                transform->width * transform->scale + 5,
                transform->height * transform->scale + 5
            };

            SDL_SetRenderDrawColor(Game::renderer, 192, 57, 43, 255);
            SDL_RenderDrawRect(Game::renderer, &transformRectDebug);

            if (entity->HasComponent<ColliderComponent>()) {
                ColliderComponent* collider = entity->GetComponent<ColliderComponent>();

                SDL_Rect transformRectDebug = {
                    collider->collider.x - (isFixed ? 0 : Game::camera.x),
                    collider->collider.y - (isFixed ? 0 : Game::camera.y),
                    collider->collider.w,
                    collider->collider.h
                };

                SDL_SetRenderDrawColor(Game::renderer, 39, 174, 96, 255);
                SDL_RenderDrawRect(Game::renderer, &transformRectDebug);
            }
        }
    }
}
