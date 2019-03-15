#include "Scene.h"

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

/*
 * JSON (de-)serialization functions comin' up here!
 */

using json = nlohmann::json;

/*
 * Wrapper function, which tries to get a property from a json file
 * and writes it to the given variable. If the property is not
 * found, false is returned and the value of variable remains unchanged.
 */
template <typename T>
bool tryGet(const json& j, std::string name, T& variable) {

    try {
        variable = j.at(name).get<T>();
    } catch(std::exception& e) {
        json err(variable);
        std::cout << "Property \""
            << name
            << "\" not found. Using default: "
            << err.dump(4)
            << std::endl;
        return false;
    }
    
    return true;
}

namespace glm {

    /*
     * GLM Matrices
     */

    template <int w, int h, typename T, glm::qualifier Q>
    void to_json(json& j, const mat<w, h, T, Q>& o) {

        const T* p = value_ptr(o);

        json tmp;

        for (int i = 0; i < w*h; i++) {
            tmp.push_back(p[i]);
        }

        j = tmp;
    }

    template <int w, int h, typename T, glm::qualifier Q>
    void from_json(const json& j, mat<w, h, T, Q>& o) {

        T* p = (T*) value_ptr(o);

        for (int i = 0; i < w*h; i++) {
            p[i] = j[i];
        }
    }

    /*
     * GLM Vectors
     */

    template <int l, typename T, glm::qualifier Q>
    void to_json(json& j, const vec<l, T, Q>& o) {

        const T* p = value_ptr(o);

        json tmp;

        for (int i = 0; i < l; i++) {
            tmp.push_back(p[i]);
        }

        j = tmp;
    }

    template <int l, typename T, glm::qualifier Q>
    void from_json(const json& j, vec<l, T, Q>& o) {

        T* p = (T*) value_ptr(o);

        for (int i = 0; i < l; i++) {
            p[i] = j[i];
        }
    }

    /*
     * GLM Quaternion
     */

    template <typename T, glm::qualifier Q>
    void to_json(json& j, const tquat<T, Q>& o) {

        j = json{o.w, o.x, o.y, o.z};
    }

    template <typename T, glm::qualifier Q>
    void from_json(const json& j, tquat<T, Q>& o) {

        o = tquat<T, Q>(j[0], j[1], j[2], j[3]);
    }
}

void to_json(json& j, const Pose& p) {

    j = json({
            {"position", p.position},
            {"rotation", p.rotation},
            {"scale", p.scale}
        });
}

void from_json(const json& j, Pose& p) {

    p.position = j.at("position").get<glm::vec3>();
    p.rotation = j.at("rotation").get<glm::quat>();
    p.scale = j.at("scale").get<glm::vec3>();
}


/*
 * FpsCamera
 */

void to_json(json& j, const FpsCamera& o) {

    j = json({
            {"pitch", o.pitch},
            {"yaw", o.yaw},
            {"fov", o.fov},
            {"aspectRatio", o.aspectRatio},
            {"pose", o.pose}
        });
}

void from_json(const json& j, FpsCamera& o) {

    o.pitch = j.at("pitch").get<float>();
    o.yaw = j.at("yaw").get<float>();
    o.fov = j.at("fov").get<float>();
    o.aspectRatio = j.at("aspectRatio").get<float>();
    o.pose = j.at("pose").get<Pose>();
}

/*
 * Settings
 */

void to_json(json& j, const Settings& s) {

    j = json({
            {"simulationSpeed", s.simulationSpeed},
            {"configPath", s.configPath},
            {"showMarkers", s.showMarkers},
            {"showVehiclePath", s.showVehiclePath},
            {"fancyVehiclePath", s.fancyVehiclePath},
            {"showVehicleTrajectory", s.showVehicleTrajectory},
            {"showLaserSensor", s.showLaserSensor},
            {"showBinaryLightSensor", s.showBinaryLightSensor}
        });
}

void from_json(const json& j, Settings& s) {

    tryGet(j, "simulationSpeed", s.simulationSpeed);
    tryGet(j, "configPath", s.configPath);
    tryGet(j, "showMarkers", s.showMarkers);
    tryGet(j, "showVehiclePath", s.showVehiclePath);
    tryGet(j, "fancyVehiclePath", s.fancyVehiclePath);
    tryGet(j, "showVehicleTrajectory", s.showVehicleTrajectory);
    tryGet(j, "showLaserSensor", s.showLaserSensor);
    tryGet(j, "showBinaryLightSensor", s.showBinaryLightSensor);
}

/*
 * Car::SystemParams
 */

void to_json(json& j, const Car::SystemParams& o) {

    j = json({
            {"axesDistance", o.axesDistance},
            {"axesMomentRatio", o.axesMomentRatio},
            {"inertia", o.inertia},
            {"mass", o.mass},
            {"distCogToFrontAxle", o.distCogToFrontAxle},
            {"distCogToRearAxle", o.distCogToFrontAxle},
        });

}

void from_json(const json& j, Car::SystemParams& o) {

    o.axesDistance = j.at("axesDistance").get<double>();
    o.axesMomentRatio = j.at("axesMomentRatio").get<double>();
    o.inertia = j.at("inertia").get<double>();
    o.mass = j.at("mass").get<double>();
    o.distCogToFrontAxle = j.at("distCogToFrontAxle").get<double>();
    o.distCogToRearAxle = j.at("distCogToRearAxle").get<double>();
}

/*
 * Car::SimulatorState
 */

void to_json(json& j, const Car::SimulatorState& o) {

    j = json({
            {"x1", o.x1},
            {"x2", o.x2},
            {"psi", o.psi},
            {"delta", o.delta},
            {"v", o.v},
            {"vLon", o.v_lon},
            {"vLat", o.v_lat},
            {"dPsi", o.d_psi},
        });
}

void from_json(const json& j, Car::SimulatorState& o) {

    o.x1 = j.at("x1").get<double>();
    o.x2 = j.at("x2").get<double>();
    o.psi = j.at("psi").get<double>();
    o.delta = j.at("delta").get<double>();
    o.v = j.at("v").get<double>();
    o.v_lon = j.at("vLon").get<double>();
    o.v_lat = j.at("vLat").get<double>();
    o.d_psi = j.at("dPsi").get<double>();
}

/*
 * Car::Limits
 */

void to_json(json& j, const Car::Limits& o) {

    j = json({
            {"maxF", o.max_F},
            {"maxDelta", o.max_delta},
            {"maxDDelta", o.max_d_delta},
        });
}

void from_json(const json& j, Car::Limits& o) {

    o.max_F = j.at("maxF").get<double>();
    o.max_delta = j.at("maxDelta").get<double>();
    o.max_d_delta = j.at("maxDDelta").get<double>();
}

/*
 * Car::Wheels
 */

void to_json(json& j, const Car::Wheels& o) {
    
    j = json({
            {"usePacejkaModel", o.usePacejkaModel},
            {"bFront", o.B_front},
            {"bRear", o.B_rear},
            {"cFront", o.C_front},
            {"cRear", o.C_rear},
            {"dFront", o.D_front},
            {"dRear", o.D_rear},
            {"kFront", o.k_front},
            {"kRear", o.k_rear}
        });

}

void from_json(const json& j, Car::Wheels& o) {

    o.usePacejkaModel = j.at("usePacejkaModel").get<bool>();

    o.B_front = j.at("bFront").get<double>();
    o.B_rear = j.at("bRear").get<double>();
    o.C_front = j.at("cFront").get<double>();
    o.C_rear = j.at("cRear").get<double>();
    o.D_front = j.at("dFront").get<double>();
    o.D_rear = j.at("dRear").get<double>();
    o.k_front = j.at("kFront").get<double>();
    o.k_rear = j.at("kRear").get<double>();
}

/*
 * Car::MainCamera
 */

void to_json(json& j, const Car::MainCamera& o) {

    j = json({
            {"pose", o.pose},
            {"imageWidth", o.imageWidth},
            {"imageHeight", o.imageHeight},
            {"fov", o.fovy},
            {"noise", o.noise},
        });
}

void from_json(const json& j, Car::MainCamera& o) {

    tryGet(j, "pose", o.pose);
    tryGet(j, "imageWidth", o.imageWidth);
    tryGet(j, "imageHeight", o.imageHeight);
    tryGet(j, "fov", o.fovy);
    tryGet(j, "noise", o.noise);
}

/*
 * Car::LaserSensor
 */

void to_json(json& j, const Car::LaserSensor& o) {

    j = json({
            {"pose", o.pose},
        });
}

void from_json(const json& j, Car::LaserSensor& o) {

    tryGet(j, "pose", o.pose);
}

/*
 * Car::BinaryLightSensor
 */

void to_json(json& j, const Car::BinaryLightSensor& o) {

    j = json({
            {"pose", o.pose},
            {"triggerDistance", o.triggerDistance},
        });
}

void from_json(const json& j, Car::BinaryLightSensor& o) {

    tryGet(j, "pose", o.pose);
    tryGet(j, "triggerDistance", o.triggerDistance);
}

/*
 * Car
 */

void to_json(json& j, const Car& o) {

    j = json({
            {"modelPose", o.modelPose},
            {"simulatorState", o.simulatorState},
            {"systemParams", o.systemParams},
            {"limits", o.limits},
            {"wheels", o.wheels},
            {"mainCamera", o.mainCamera},
            {"laserSensor", o.laserSensor},
            {"binaryLightSensor", o.binaryLightSensor}
        });
}

void from_json(const json& j, Car& o) {

    tryGet(j, "modelPose", o.modelPose);
    tryGet(j, "simulatorState", o.simulatorState);
    tryGet(j, "systemParams", o.systemParams);
    tryGet(j, "limits", o.limits);
    tryGet(j, "wheels", o.wheels);
    tryGet(j, "mainCamera", o.mainCamera);
    tryGet(j, "laserSensor", o.laserSensor);
    tryGet(j, "binaryLightSensor", o.binaryLightSensor);
}

/*
 * Tracks
 */


void to_json(json& j, const Tracks& t) {

    json jsonControlPoints;
    json jsonTracks;

    const std::vector<std::shared_ptr<ControlPoint>>& tracks = t.getTracks();

    std::set<std::shared_ptr<TrackBase>> ts;

    for (auto& controlPoint : tracks) {

        jsonControlPoints.push_back({{"coords", controlPoint->coords}});

        for (auto& track : controlPoint->tracks) {

            ts.insert(track);
        }
    }

    for (const std::shared_ptr<TrackBase>& track : ts) {

        if (const std::shared_ptr<TrackIntersection>& intersection = std::dynamic_pointer_cast<TrackIntersection>(track)) {
            json jsonTrack;

            jsonTrack["type"] = "intersection";

            jsonTrack["center"] = std::find(tracks.begin(), tracks.end(), intersection->center.lock()) - tracks.begin();
            jsonTrack["link1"] = std::find(tracks.begin(), tracks.end(), intersection->link1.lock()) - tracks.begin();
            jsonTrack["link2"] = std::find(tracks.begin(), tracks.end(), intersection->link2.lock()) - tracks.begin();
            jsonTrack["link3"] = std::find(tracks.begin(), tracks.end(), intersection->link3.lock()) - tracks.begin();
            jsonTrack["link4"] = std::find(tracks.begin(), tracks.end(), intersection->link4.lock()) - tracks.begin();

            jsonTracks.push_back(jsonTrack);

            continue;
        }

        std::weak_ptr<ControlPoint> trackStart;
        std::weak_ptr<ControlPoint> trackEnd;
        if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(track)) {
            trackStart = line->start;
            trackEnd = line->end;
        } else {
            std::shared_ptr<TrackArc> arc = std::dynamic_pointer_cast<TrackArc>(track);
            trackStart = arc->start;
            trackEnd = arc->end;
        }

        unsigned int start = 0;
        unsigned int end = 0;

        for (unsigned int i = 0; i < tracks.size(); i++) {
            if (trackStart.lock().get() == tracks.at(i).get()) {
                start = i;
                break;
            }
        }
        for (unsigned int i = 0; i < tracks.size(); i++) {
            if (trackEnd.lock().get() == tracks.at(i).get()) {
                end = i;
                break;
            }
        }

        json jsonTrack;

        jsonTrack["start"] = start;
        jsonTrack["end"] = end;

        if (TrackArc* arc = dynamic_cast<TrackArc*>(track.get())) {
            jsonTrack["type"] = "arc";
            jsonTrack["center"] = arc->center;
            jsonTrack["radius"] = arc->radius;
            jsonTrack["rightArc"] = arc->rightArc;
            switch (arc->centerLine) {
                case LaneMarking::Dashed: {
                    jsonTrack["centerLine"] = "Dashed";
                    break;
                }
                case LaneMarking::DoubleSolid: {
                    jsonTrack["centerLine"] = "DoubleSolid";
                    break;
                }
            }
        } else {
            std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(track);
            jsonTrack["type"] = "line";
            switch (line->centerLine) {
                case LaneMarking::Dashed: {
                    jsonTrack["centerLine"] = "Dashed";
                    break;
                }
                case LaneMarking::DoubleSolid: {
                    jsonTrack["centerLine"] = "DoubleSolid";
                    break;
                }
            }
        }

        jsonTracks.push_back(jsonTrack);
    }

    j = json({
            {"trackWidth", t.trackWidth},
            {"markingWidth", t.markingWidth},
            {"centerLineLength", t.centerLineLength},
            {"centerLineInterrupt", t.centerLineInterrupt},
            {"centerLineGap", t.centerLineGap},
            {"controlPoints", jsonControlPoints},
            {"tracks", jsonTracks}
        });
}

void from_json(const json& j, Tracks& t) {

    t.trackWidth = j.at("trackWidth").get<float>();
    t.markingWidth = j.at("markingWidth").get<float>();
    t.centerLineLength = j.at("centerLineLength").get<float>();
    t.centerLineInterrupt = j.at("centerLineInterrupt").get<float>();
    try {
        t.centerLineGap = j.at("centerLineGap").get<float>();
    } catch (json::exception& e) {
        // use default value
    }

    std::vector<std::shared_ptr<ControlPoint>> controlPoints;

    for (const json& jsonControlPoint : j["controlPoints"]) {
        ControlPoint* controlPoint = new ControlPoint();
        controlPoint->coords = jsonControlPoint.at("coords").get<glm::vec2>();
        controlPoints.emplace_back(controlPoint);
    }

    for (const json& jsonTrack : j["tracks"]) {

        std::string type = jsonTrack.at("type").get<std::string>();

        if ("intersection" == type) {
            std::shared_ptr<ControlPoint>& center = controlPoints.at(jsonTrack.at("center").get<int>());
            std::shared_ptr<ControlPoint>& link1 = controlPoints.at(jsonTrack.at("link1").get<int>());
            std::shared_ptr<ControlPoint>& link2 = controlPoints.at(jsonTrack.at("link2").get<int>());
            std::shared_ptr<ControlPoint>& link3 = controlPoints.at(jsonTrack.at("link3").get<int>());
            std::shared_ptr<ControlPoint>& link4 = controlPoints.at(jsonTrack.at("link4").get<int>());

            t.addTrackIntersection(center, link1, link2, link3, link4);

            continue;
        }

        std::shared_ptr<ControlPoint> start =
            controlPoints.at(jsonTrack.at("start").get<int>());
        std::shared_ptr<ControlPoint> end =
            controlPoints.at(jsonTrack.at("end").get<int>());

        if ("arc" == type) {
            glm::vec2 center = jsonTrack.at("center").get<glm::vec2>();
            float radius = jsonTrack.at("radius").get<float>();
            float rightArc = jsonTrack.at("rightArc").get<bool>();
            std::shared_ptr<TrackArc> arc = t.addTrackArc(start, end, center, radius, rightArc);

            try {
                std::string centerLine = jsonTrack.at("centerLine").get<std::string>();
                if (centerLine == "Dashed") {
                    arc->centerLine = LaneMarking::Dashed;
                } else if (centerLine == "DoubleSolid") {
                    arc->centerLine = LaneMarking::DoubleSolid;
                }
            } catch (json::exception& e) {
                // use default value
            }
        } else {
            std::shared_ptr<TrackLine> line = t.addTrackLine(start, end);

            try {
                std::string centerLine = jsonTrack.at("centerLine").get<std::string>();
                if (centerLine == "Dashed") {
                    line->centerLine = LaneMarking::Dashed;
                } else if (centerLine == "DoubleSolid") {
                    line->centerLine = LaneMarking::DoubleSolid;
                }
            } catch (json::exception& e) {
                // use default value
            }
        }
    }
}

/*
 * Items
 */

void to_json(json& j, const std::vector<std::shared_ptr<Scene::Item>>& is) {

    json jsonItems;

    for (const std::shared_ptr<Scene::Item>& i : is) {
        jsonItems.push_back({
                {"pose", i->pose},
                {"type", (int)i->type},
                {"name", i->name}
            });
    }

    j = jsonItems;
}

void from_json(const json& j, std::vector<std::shared_ptr<Scene::Item>>& is) {

    for (const json& jsonItem : j) {
        std::shared_ptr<Scene::Item> i = std::make_shared<Scene::Item>(NONE);
        i->pose = jsonItem.at("pose").get<Pose>();
        i->type = (ItemType) jsonItem.at("type").get<int>();

        try {
            i->name = jsonItem.at("name").get<std::string>();
        } catch (json::exception& e) {
            i->name = "unnamed_item";
        }

        is.push_back(i);
    }
}

/*
 * DynamicItemSettings
 */
void to_json(json& j, const Scene::DynamicItemSettings& dis) {

    j = json({
            {"speed", dis.speed}
        });
}

void from_json(const json& j, Scene::DynamicItemSettings& dis) {

    tryGet(j, "speed", dis.speed);
}

/*
 * Scene::Rules
 */

void to_json(json& j, const Scene::Rules& r) {

    j = json({
            {"exitOnObstacleCollision", r.exitOnObstacleCollision},
            {"exitIfNotOnTrack", r.exitIfNotOnTrack},
            {"exitIfSpeedLimitExceeded", r.exitIfSpeedLimitExceeded},
            {"exitIfLeftArrowIgnored", r.exitIfLeftArrowIgnored},
            {"exitIfRightArrowIgnored", r.exitIfRightArrowIgnored},
            {"exitIfStopLineIgnored", r.exitIfStopLineIgnored},
            {"exitIfGiveWayLineIgnored", r.exitIfGiveWayLineIgnored},
            {"exitIfCrosswalkIgnored", r.exitIfCrosswalkIgnored},
            {"exitIfNoParkingIgnored", r.exitIfCrosswalkIgnored},
            {"exitIfAllCheckpointsPassed", r.exitIfAllCheckpointsPassed}
        });
}

void from_json(const json& j, Scene::Rules& r) {

    tryGet(j, "exitOnObstacleCollision", r.exitOnObstacleCollision);
    tryGet(j, "exitIfNotOnTrack", r.exitIfNotOnTrack);
    tryGet(j, "exitIfSpeedLimitExceeded", r.exitIfSpeedLimitExceeded);
    tryGet(j, "exitIfLeftArrowIgnored", r.exitIfLeftArrowIgnored);
    tryGet(j, "exitIfRightArrowIgnored", r.exitIfRightArrowIgnored);
    tryGet(j, "exitIfStopLineIgnored", r.exitIfStopLineIgnored);
    tryGet(j, "exitIfGiveWayLineIgnored", r.exitIfGiveWayLineIgnored);
    tryGet(j, "exitIfCrosswalkIgnored", r.exitIfCrosswalkIgnored);
    tryGet(j, "exitIfNoParkingIgnored", r.exitIfNoParkingIgnored);
    tryGet(j, "exitIfAllCheckpointsPassed", r.exitIfAllCheckpointsPassed);
}

/*
 * Scene
 */

void to_json(json& j, const Scene& s) {

    j = json({
            {"version", s.version},
            {"paused", s.paused},
            {"fpsCamera", s.fpsCamera},
            {"car", s.car},
            {"tracks", s.tracks},
            {"items", s.items},
            {"dynamicItemSettings", s.dynamicItemSettings},
            {"rules", s.rules}
        });
}

void from_json(const json& j, Scene& s) {

    s.version = j.at("version").get<unsigned int>();
    s.paused = j.at("paused").get<bool>();
    s.fpsCamera = j.at("fpsCamera").get<FpsCamera>();
    s.car = j.at("car").get<Car>();
    s.tracks = j.at("tracks").get<Tracks>();
    s.items = j.at("items").get<std::vector<std::shared_ptr<Scene::Item>>>();

    try {
        s.rules = j.at("rules").get<Scene::Rules>();
    } catch (std::exception e) {
        std::cout << "Loading defaults for Scene::Rules" << std::endl;
    }
}

/*
 * Settings comin right up, mate!

bool Settings::save() {

    std::ofstream out(settingsFilePath);

    if (!out) {
        return false;
    } 

    out << json(*this).dump(4);
    out.close();

    return true;
}

bool Settings::load() {

    std::ifstream in(settingsFilePath);

    if (!in) {
        return false;
    }

    try {
        json j;
        in >> j;
        *this = j;
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }

    in.close();

    return true;
}
*/

/*
 * Now that's the actual Scene implementation, fellas!
 */

Scene::Scene() : version{VERSION} {
}

Scene::Scene(std::string path) : version{VERSION} {
    load(path);
}

Scene::~Scene() {
}

bool Scene::save(std::string path) { 

    std::ofstream out(path);

    if (!out) {
        return false;
    } 

    out << json(*this).dump(4);
    out.close();

    return true;
}

bool Scene::load(std::string path) {

    std::ifstream in(path);

    if (!in) {
        return false;
    }

    try {
        json j;
        in >> j;
        *this = j;
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return false;
    }

    in.close();

    return true;
}

std::deque<Scene> Scene::history;

void Scene::addToHistory() {
    
    history.push_back(*this);

    if (history.size() > 0) {
        if (simulationTime - history.front().simulationTime > 10000) {
            history.pop_front();
        }
    }
}

Scene& Scene::getFromHistory(float simulationTimePoint) {

    if (simulationTimePoint < history.front().simulationTime) {
        return history.front();
    }   

    for (auto it = history.crbegin(); it != history.crend(); ++it) {
        if ((*it).simulationTime <= simulationTimePoint) {
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
