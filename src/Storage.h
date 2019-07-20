#include <string>

#include "scene/Settings.h"
#include "scene/Scene.h"
#include "helpers/Model.h"

std::string getResourcePath();
bool createResourcePath();

template <typename T>
bool load(T& t, std::string path);
template <typename T>
bool save(T& t, std::string path);

bool load(Settings& settings);
bool save(Settings& settings);
