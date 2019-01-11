#ifndef INC_2019_EDITOR_H
#define INC_2019_EDITOR_H

#define _USE_MATH_DEFINES
#include <cmath>

#include <set>
#include <map>

#include "helpers/Helpers.h"

#include "Scene.h"

class Editor {

public:

    enum class TrackMode {
        Line,
        Arc,
        Intersection
    };

private:

    static constexpr float controlPointClickRadius{0.2f};

// settings

    TrackMode trackMode{TrackMode::Line};

    bool autoAlign{false};

// models

    static constexpr float intersectionTrackLength{0.75f};

    // materials
    static objl::Material markerDefaultMaterial;
    static objl::Material markerActiveMaterial;
    static objl::Material trackMaterial;

    // vertices
    static std::vector<objl::Vertex> pointVertices;
    static std::vector<objl::Vertex> trackLineVertices;

    // ground
    Model ground{"models/ground.obj"};
    glm::mat4 groundModelMat{1.0f};

    // markers
    static constexpr float markerYOffset{0.005f};

    Model defaultMarker;
    Model activeMarker;

    glm::mat4 markerModelMatEnd;

    std::shared_ptr<Model> trackMarker;
    glm::mat4 trackMarkerMat;

    std::shared_ptr<Model> markerTrackLine = std::make_shared<Model>();
    std::shared_ptr<Model> markerTrackIntersection = std::make_shared<Model>();

    // tracks
    std::shared_ptr<Model> intersectionModel;

    std::map<std::shared_ptr<TrackBase>, std::shared_ptr<Model>> trackModels;
    std::map<std::shared_ptr<TrackBase>, glm::mat4> trackModelMats;

    // track rendering offset
    static constexpr float trackYOffset{0.005f};

// temporary state

    std::shared_ptr<ControlPoint> activeControlPoint;

    struct {
        bool dragging{false};

        std::shared_ptr<ControlPoint> connectedPoint;

        std::map<std::shared_ptr<ControlPoint>, glm::vec2> coords;

        std::map<std::shared_ptr<TrackBase>, std::shared_ptr<Model>> trackModels;
        std::map<std::shared_ptr<TrackBase>, glm::mat4> trackModelMats;
    } dragState;

    glm::vec2 cursorPos; // in ground coordinates

public:

    Editor(const Scene::Tracks& tracks, float groundSize);

    void updateInput(Camera& camera, Scene::Tracks& tracks, float groundSize);
    void onKey(int key, int action, const Scene::Tracks& tracks);
    void onButton(double cursorX, double cursorY, int windowWidth, int windowHeight,
            int button, int action, Camera& camera, Scene::Tracks& tracks, float groundSize);
    void onMouseMoved(double cursorX, double cursorY, int windowWidth, int windowHeight,
            Camera& camera, const Scene::Tracks& tracks, float groundSize);

    void setTrackMode(TrackMode trackMode, const Scene::Tracks& tracks);
    void setAutoAlign(bool autoAlign, const Scene::Tracks& tracks);

    void renderScene(GLuint shaderProgramId, const Scene::Tracks& tracks);
    void renderMarkers(GLuint shaderProgramId, const Scene::Tracks& tracks, const glm::vec3 cameraPosition);

private:

    // render a single marker
    void renderMarker(GLuint shaderProgramId, const glm::vec2& position,
            const bool active, const glm::vec3& cameraPosition);

    void startTrack(const glm::vec2& position, const Scene::Tracks& tracks, float groundSize);
    void endTrack(const glm::vec2& position, Scene::Tracks& tracks, float groundSize);
    void createIntersection(const glm::vec2& position, Scene::Tracks& tracks, const float groundSize);
    void addTrackLine(const std::shared_ptr<ControlPoint>& start,
            const std::shared_ptr<ControlPoint>& end, Scene::Tracks& tracks);
    void addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc, Scene::Tracks& tracks);
    void addTrackIntersection(const std::shared_ptr<ControlPoint>& center,
            const std::shared_ptr<ControlPoint>& link1, const std::shared_ptr<ControlPoint>& link2,
            const std::shared_ptr<ControlPoint>& link3, const std::shared_ptr<ControlPoint>& link4,
            Scene::Tracks& tracks);

    void dragControlPoint(const std::shared_ptr<ControlPoint>& controlPoint, const Scene::Tracks& tracks);
    void moveControlPoint(std::shared_ptr<ControlPoint>& controlPoint, Scene::Tracks& tracks, float groundSize);
    void moveTracksAtControlPoint(const std::vector<std::shared_ptr<ControlPoint>>& controlPoints,
            bool applyMovement, const Scene::Tracks& tracks);
    std::shared_ptr<TrackIntersection> findIntersection(const ControlPoint& cp) const;
    glm::vec2 getDraggedPosition(const std::shared_ptr<ControlPoint>& cp) const;
    bool isDragged(const std::shared_ptr<ControlPoint>& cp) const;

    std::shared_ptr<ControlPoint> selectControlPoint(const glm::vec2& position, const Scene::Tracks& tracks) const;
    std::shared_ptr<ControlPoint> selectControlPoint(const glm::vec2& position,
            const Scene::Tracks& tracks, const bool includeActiveControlPoint, const bool includeCompleteControlPoints) const;

    bool toGroundCoordinates(const double cursorX, const double cursorY, const int windowWidth, const int windowHeight,
            Camera& camera, glm::vec2& groundCoords);

    void updateMarkers(const Scene::Tracks& tracks);
    void updateTrackLineMarker(const glm::vec2& start, const glm::vec2& end, const Scene::Tracks& tracks);
    void updateTrackArcMarker(const ControlPoint& startPoint,
            const glm::vec2& end, const Scene::Tracks& tracks);
    void updateTrackIntersectionMarker(const glm::vec2& center);

    bool getArc(const ControlPoint& start, const glm::vec2& end,
            glm::vec2& center, float& radius, bool& rightArc);
    bool getArc(const glm::vec2& start, const glm::vec2& end, const std::vector<glm::vec2>& directions,
            glm::vec2& center, float& radius, bool& rightArc);

    bool intersectParam(const glm::vec2& p1, const glm::vec2& r1, const glm::vec2& p2, const glm::vec2& r2, float& t1);

    glm::vec2 align(const ControlPoint& startPoint, const glm::vec2& position, const Scene::Tracks& tracks);
    glm::vec2 getAlignedUnitVector(const glm::vec2& startPoint, const glm::vec2& endPoint,
            const std::vector<glm::vec2>& directions);
    std::vector<glm::vec2> getAlignmentVectors(const ControlPoint& cp);

    TrackMode getEffectiveTrackMode();

    bool isStartConnected();

    void deselect();
    bool canCreateTrack(const Scene::Tracks& tracks);
    bool isComplete(const ControlPoint& cp) const;
    bool maybeDragging(const Scene::Tracks& tracks);

// model creation

    static std::shared_ptr<Model> genTrackLineModel(const glm::vec2& start, const glm::vec2& end,
            const Scene::Tracks& tracks);
    static std::shared_ptr<Model> genTrackArcModel(const glm::vec2& start, const glm::vec2& end,
            const glm::vec2& center, const float radius, const bool rightArc, const Scene::Tracks& tracks);
    static std::shared_ptr<Model> genTrackIntersectionModel(const Scene::Tracks& tracks);

    static void genDefaultMarkerMaterial(Model& model);
    static void genActiveMarkerMaterial(Model& model);
    static void genTrackMaterial(Model& model);

    static void genPointVertices(Model& model);
    static void genTrackLineVertices(const glm::vec2& start, const glm::vec2& end, const Scene::Tracks& tracks, Model& model);
    static void genTrackArcVertices(const glm::vec2& start, const glm::vec2& end, const glm::vec2& center, const float radius, const bool rightArc, const Scene::Tracks& tracks, Model& model);
    static void genTrackIntersectionVertices(const Scene::Tracks& tracks, Model& model);
    static void genTrackLineMarkerVertices(Model& model);
    static void genTrackArcMarkerVertices(const glm::vec2& start, const glm::vec2& end,
            const glm::vec2& center, const float radius, const bool rightArc,
            const Scene::Tracks& tracks, Model& model);
    static void genTrackIntersectionMarkerVertices(const Scene::Tracks& tracks, Model& model);

    static glm::mat4 genPointMatrix(const glm::vec2& point, const float y);
    static glm::mat4 genTrackLineMatrix(const glm::vec2& start, const glm::vec2& end, const float y);
    static glm::mat4 genTrackArcMatrix(const glm::vec2& center, const float y);
    static glm::mat4 genTrackIntersectionMatrix(const glm::vec2& center, const float angle, const float y);
    static glm::mat4 genTrackLineMarkerMatrix(const glm::vec2& start,
            const glm::vec2& end, const float y, const Scene::Tracks& tracks);

    static void appendQuad(std::vector<objl::Vertex>& vertices, const objl::Vector3& vec0,
            const objl::Vector3& vec1, const objl::Vector3& vec2, const objl::Vector3& vec3);

    static void getArcVertexParams(const glm::vec2& start, const glm::vec2& end,
            const glm::vec2& center, const bool rightArc, float& baseAngle,
            float& angle, int& numQuads);
};

#endif

