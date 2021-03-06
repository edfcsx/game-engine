#include <iostream>
#include "./Entity.h"
#include "./Constants.h"

Entity::Entity(EntityManager& manager): manager(manager) {
    this->isActive = true;
}

Entity::Entity(EntityManager& manager, std::string name, LayerType layer): manager(manager), name(name), layer(layer) {
    this->isActive = true;
}

void Entity::Update(float deltaTime) {
    for (auto& component: components) {
        component->Update(deltaTime);
    }
}

void Entity::Render() {
    for (auto& component: components) {
        component->Render();
    }
}

void Entity::Destroy() {
    this->isActive = false;
}

// const function is not allow this changes
bool Entity::IsActive() const {
    return this->isActive;
}

void Entity::ListAllComponents() const {
    for (auto mapELement: componentTypeMap) {
        std::cout << " Component<" << mapELement.first->name() << ">" << std::endl;
    }
}