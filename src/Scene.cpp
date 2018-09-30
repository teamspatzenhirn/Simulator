#include "Scene.h"

/*
 * JSON (de-)serialization functions comin' up here!
 */

using json = nlohmann::json;

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
 * Scene::Car::SystemParams
 */

void to_json(json& j, const Scene::Car::SystemParams& o) {

    j = json({
            {"axesDistance", o.axesDistance},
            {"axesMomentRatio", o.axesMomentRatio},
            {"inertia", o.inertia},
            {"mass", o.mass},
            {"distCogToFrontAxle", o.distCogToFrontAxle},
            {"distCogToRearAxle", o.distCogToFrontAxle},
        });

}

void from_json(const json& j, Scene::Car::SystemParams& o) {

    o.axesDistance = j.at("axesDistance").get<double>();
    o.axesMomentRatio = j.at("axesMomentRatio").get<double>();
    o.inertia = j.at("inertia").get<double>();
    o.mass = j.at("mass").get<double>();
    o.distCogToFrontAxle = j.at("distCogToFrontAxle").get<double>();
    o.distCogToRearAxle = j.at("distCogToRearAxle").get<double>();
}

/*
 * Scene::Car::Limits
 */

void to_json(json& j, const Scene::Car::Limits& o) {

    j = json({
            {"maxF", o.max_F},
            {"maxDelta", o.max_delta},
            {"maxDDelta", o.max_d_delta},
        });
}

void from_json(const json& j, Scene::Car::Limits& o) {

    o.max_F = j.at("maxF").get<double>();
    o.max_delta = j.at("maxDelta").get<double>();
    o.max_d_delta = j.at("maxDDelta").get<double>();
}

/*
 * Scene::Car::Wheels
 */

void to_json(json& j, const Scene::Car::Wheels& o) {
    
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

void from_json(const json& j, Scene::Car::Wheels& o) {

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
 * Scene::Car::MainCamera
 */

void to_json(json& j, const Scene::Car::MainCamera& o) {

    j = json({
            {"pose", o.pose},
            {"imageWidth", o.imageWidth},
            {"imageHeight", o.imageHeight},
            {"fov", o.fovy},
        });
}

void from_json(const json& j, Scene::Car::MainCamera& o) {

    o.pose = j.at("pose").get<Pose>();
    o.imageWidth = j.at("imageWidth").get<unsigned int>();
    o.imageHeight = j.at("imageHeight").get<unsigned int>();
    o.fovy = j.at("fov").get<float>();
}

/*
 * Car
 */

void to_json(json& j, const Scene::Car& o) {

    j = json({
            {"modelPose", o.modelPose},
            {"systemParams", o.systemParams},
            {"limits", o.limits},
            {"wheels", o.wheels},
            {"mainCamera", o.mainCamera}
        });
}

void from_json(const json& j, Scene::Car& o) {

    o.modelPose = j.at("modelPose").get<Pose>();
    o.systemParams = j.at("systemParams").get<Scene::Car::SystemParams>();
    o.limits = j.at("limits").get<Scene::Car::Limits>();
    o.wheels = j.at("wheels").get<Scene::Car::Wheels>();
    o.mainCamera = j.at("mainCamera").get<Scene::Car::MainCamera>();
}

/*
 * Scene
 */

void to_json(json& j, const Scene& s) {

    j = json({
            {"version", s.version},
            {"car", s.car}
        });
}

void from_json(const json& j, Scene& s) {

    s.version = j.at("version").get<unsigned int>();
    s.car = j.at("car").get<Scene::Car>();
}

/*
 * Now that's the actual Scene implementation, fellas!
 */

TrackBase::TrackBase(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end)
    : start(start), end(end) {
}

TrackBase::~TrackBase() {
}

TrackLine::TrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end)
    : TrackBase(start, end) {
}

glm::vec2 TrackLine::getDirection(const ControlPoint& controlPoint) {

    std::shared_ptr<ControlPoint> start = this->start.lock();
    std::shared_ptr<ControlPoint> end = this->end.lock();

    if (start.get() == &controlPoint) {
        return start->coords - end->coords;
    } else {
        return end->coords - start->coords;
    }
}

TrackArc::TrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end,
        const glm::vec2& center, const float radius, const bool rightArc)
    : TrackBase(start, end), center(center), radius(radius), rightArc(rightArc) {
}

glm::vec2 TrackArc::getDirection(const ControlPoint& controlPoint) {

    std::shared_ptr<ControlPoint> start = this->start.lock();
    std::shared_ptr<ControlPoint> end = this->end.lock();

    glm::vec2 dir;
    if (start.get() == &controlPoint) {
        if (rightArc) {
            dir = center - start->coords;
        } else {
            dir = start->coords - center;
        }
    } else {
        if (rightArc) {
            dir = end->coords - center;
        } else {
            dir = center - end->coords;
        }
    }

    dir = glm::vec2(-dir.y, dir.x);

    return dir;
}

const std::vector<std::shared_ptr<ControlPoint>>& Scene::Tracks::getTracks() const {

    return tracks;
}

std::shared_ptr<TrackLine> Scene::Tracks::addTrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end) {

    // add control points
    if (!controlPointExists(start)) {
        tracks.push_back(start);
    }

    if (!controlPointExists(end)) {
        tracks.push_back(end);
    }

    // create track
    std::shared_ptr<TrackLine> track = std::make_shared<TrackLine>(start, end);

    start->tracks.push_back(track);
    end->tracks.push_back(track);

    return track;
}

std::shared_ptr<TrackArc> Scene::Tracks::addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc) {

    // add control points
    if (!controlPointExists(start)) {
        tracks.push_back(start);
    }

    if (!controlPointExists(end)) {
        tracks.push_back(end);
    }

    // create track
    std::shared_ptr<TrackArc> track = std::make_shared<TrackArc>(start, end, center, radius, rightArc);

    start->tracks.push_back(track);
    end->tracks.push_back(track);

    return track;
}

bool Scene::Tracks::controlPointExists(const std::shared_ptr<ControlPoint>& controlPoint) const {

    for (std::shared_ptr<ControlPoint> const& cp : tracks) {
        if (controlPoint == cp) {
            return true;
        }
    }

    return false;
}

Scene::Scene() : version{VERSION} {
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
