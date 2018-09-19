#include "Scene.h"

Scene::Scene() {

    version = VERSION;
}

Scene::Scene(std::string path) : Scene() {

    std::ifstream in(path, std::ios::in | std::ios::in);
    if (in) {
        in.get((char*)this, sizeof(Scene));
    }
    in.close();
}

Scene::~Scene() {

    //TODO: destructuring and stuff
}

void Scene::save(std::string path) { 

    std::ofstream out;
    out.open(path, std::ios::out | std::ios::binary);
    if (out) {
        out.write((char*)this, sizeof(Scene));
    }
    out.close();
}
