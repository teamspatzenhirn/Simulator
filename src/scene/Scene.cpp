#include "Scene.h"

#include <iostream>
#include <fstream>

#include "Storage.h"

Scene::Scene() : version{VERSION} {
}

Scene::Scene(std::string path) : version{VERSION} {

    storage::load(*this, path);
}

Scene::~Scene() {
}

std::deque<Scene> Scene::history;

void Scene::addToHistory() {
    
    history.push_back(*this);

    if (history.size() > 0) {
        if (simulationClock.time - history.front().simulationClock.time > 10000) {
            history.pop_front();
        }
    }
}

Scene& Scene::getFromHistory(float simulationTimePoint) {

    if (simulationTimePoint < history.front().simulationClock.time) {
        return history.front();
    }   

    for (auto it = history.crbegin(); it != history.crend(); ++it) {
        if ((*it).simulationClock.time <= simulationTimePoint) {
            return (Scene&)(*it);
        }
    }

    return history.back();
}

Scene& Scene::getHistoryBackStep(size_t step) {
    
    if (step >= history.size()) {
        return history.front();
    } 

    size_t index = history.size() - step - 1;

    return history[index];
}
