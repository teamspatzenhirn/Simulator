#ifndef INC_2019_EDITOR_H
#define INC_2019_EDITOR_H

#define _USE_MATH_DEFINES
#include <cmath>

#include <set>
#include <map>
#include <algorithm>

#include "helpers/Helpers.h"

#include "scene/Tracks.h"

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

    static constexpr float intersectionTrackLength{1.5f};

    // materials
    Model::Material markerDefaultMaterial;
    Model::Material markerActiveMaterial;
    Model::Material trackMaterial;

    // vertices
    std::vector<Model::Vertex> pointVertices;
    std::vector<Model::Vertex> trackLineVertices;

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
    std::shared_ptr<Model> intersectionModel = std::make_shared<Model>();

    std::map<std::shared_ptr<TrackBase>, std::shared_ptr<Model>> trackModels;
    std::map<std::shared_ptr<TrackBase>, glm::mat4> trackModelMats;

    // track rendering offset
    static constexpr float trackYOffset{0.005f};

// temporary state

    std::shared_ptr<ControlPoint> activeControlPoint;

    std::shared_ptr<TrackBase> activeTrack;
    std::shared_ptr<Model> activeTrackModel;
    glm::mat4 activeTrackMat;

    struct {
        bool dragging{false};

        std::shared_ptr<ControlPoint> connectedPoint;

        std::map<std::shared_ptr<ControlPoint>, glm::vec2> coords;

        std::map<std::shared_ptr<TrackBase>, std::shared_ptr<Model>> trackModels;
        std::map<std::shared_ptr<TrackBase>, glm::mat4> trackModelMats;
    } dragState;

    glm::vec2 cursorPos; // in ground coordinates

public:

    Editor();

    void updateInput(Camera& camera, Tracks& tracks, float groundSize);
    void onKey(int key, int action, const Tracks& tracks);
    void onButton(double cursorX, double cursorY, int windowWidth, int windowHeight,
            int button, int action, Camera& camera, Tracks& tracks, float groundSize);
    void onMouseMoved(double cursorX, double cursorY, int windowWidth, int windowHeight,
            Camera& camera, const Tracks& tracks);

    void setTrackMode(TrackMode trackMode, const Tracks& tracks);
    void setAutoAlign(bool autoAlign, const Tracks& tracks);

    void renderScene(GLuint shaderProgramId, Model& groundModel, const Tracks& tracks, float groundSize);
    void renderMarkers(GLuint shaderProgramId, const Tracks& tracks, const glm::vec3 cameraPosition);

private:

    // render a single marker
    void renderMarker(GLuint shaderProgramId, const glm::vec2& position,
            const bool active, const glm::vec3& cameraPosition);

    void selectTrack(const std::shared_ptr<TrackBase>& track, Tracks& tracks);

    void startTrack(const glm::vec2& position, const Tracks& tracks, float groundSize);
    void endTrack(const glm::vec2& position, Tracks& tracks, float groundSize);
    void createIntersection(const glm::vec2& position, Tracks& tracks, const float groundSize);
    void addTrackLine(const std::shared_ptr<ControlPoint>& start,
            const std::shared_ptr<ControlPoint>& end, Tracks& tracks);
    void addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc, Tracks& tracks);
    void addTrackIntersection(const std::shared_ptr<ControlPoint>& center,
            const std::shared_ptr<ControlPoint>& link1, const std::shared_ptr<ControlPoint>& link2,
            const std::shared_ptr<ControlPoint>& link3, const std::shared_ptr<ControlPoint>& link4,
            Tracks& tracks);

    void dragControlPoint(const std::shared_ptr<ControlPoint>& controlPoint, const Tracks& tracks);
    void moveControlPoint(std::shared_ptr<ControlPoint>& controlPoint, Tracks& tracks, float groundSize);
    void moveTracksAtControlPoint(const std::vector<std::shared_ptr<ControlPoint>>& controlPoints,
            bool applyMovement, const Tracks& tracks);
    std::shared_ptr<TrackIntersection> findIntersection(const ControlPoint& cp) const;
    glm::vec2 getDraggedPosition(const std::shared_ptr<ControlPoint>& cp) const;
    bool isDragged(const std::shared_ptr<ControlPoint>& cp) const;

    std::shared_ptr<ControlPoint> selectControlPoint(const glm::vec2& position, const Tracks& tracks) const;
    std::shared_ptr<ControlPoint> selectControlPoint(const glm::vec2& position,
            const Tracks& tracks, const bool includeActiveControlPoint, const bool includeCompleteControlPoints) const;
    std::shared_ptr<TrackBase> findTrack(const glm::vec2& position, const Tracks& tracks) const;

    bool toGroundCoordinates(const double cursorX, const double cursorY, const int windowWidth, const int windowHeight,
            Camera& camera, glm::vec2& groundCoords);

    void updateMarkers(const Tracks& tracks);
    void updateTrackLineMarker(const glm::vec2& start, const glm::vec2& end, const Tracks& tracks);
    void updateTrackArcMarker(const ControlPoint& startPoint,
            const glm::vec2& end, const Tracks& tracks);

    bool getArc(const ControlPoint& start, const glm::vec2& end,
            glm::vec2& center, float& radius, bool& rightArc);
    bool getArc(const glm::vec2& start, const glm::vec2& end, const std::vector<glm::vec2>& directions,
            glm::vec2& center, float& radius, bool& rightArc);

    float distanceLinePoint(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& x) const;
    bool intersectParam(const glm::vec2& p1, const glm::vec2& r1, const glm::vec2& p2, const glm::vec2& r2, float& t1);

    glm::vec2 align(const ControlPoint& startPoint, const glm::vec2& position, const Tracks& tracks);
    glm::vec2 getAlignedUnitVector(const glm::vec2& startPoint, const glm::vec2& endPoint,
            const std::vector<glm::vec2>& directions);
    std::vector<glm::vec2> getAlignmentVectors(const ControlPoint& cp);

    TrackMode getEffectiveTrackMode();

    bool isStartConnected();

    void deselectControlPoint();
    void deselectTrack(Tracks& tracks);
    bool canCreateTrack(const Tracks& tracks);
    bool isComplete(const ControlPoint& cp) const;
    bool maybeDragging(const Tracks& tracks);

// model creation

    std::shared_ptr<Model> genTrackLineModel(const glm::vec2& start, const glm::vec2& end,
            const LaneMarking centerLine, const Tracks& tracks);
    std::shared_ptr<Model> genTrackArcModel(const glm::vec2& start, const glm::vec2& end,
            const glm::vec2& center, const float radius, const bool rightArc,
            const LaneMarking centerLine, const Tracks& tracks);
    std::shared_ptr<Model> genTrackIntersectionModel(const Tracks& tracks);

    void genDefaultMarkerMaterial(Model& model);
    void genActiveMarkerMaterial(Model& model);
    void genTrackMaterial(Model& model);

    void genPointVertices(Model& model);
    void genTrackLineVertices(const glm::vec2& start, const glm::vec2& end,
            const LaneMarking centerLine, const Tracks& tracks, Model& model);
    void genTrackArcVertices(const glm::vec2& start, const glm::vec2& end,
            const glm::vec2& center, const float radius, const bool rightArc,
            const LaneMarking centerLine, const Tracks& tracks, Model& model);
    void genTrackIntersectionVertices(const Tracks& tracks, Model& model);
    void genTrackLineMarkerVertices(Model& model);
    void genTrackArcMarkerVertices(const glm::vec2& start, const glm::vec2& end,
            const glm::vec2& center, const float radius, const bool rightArc,
            const Tracks& tracks, Model& model);
    void genTrackIntersectionMarkerVertices(const Tracks& tracks, Model& model);

    glm::mat4 genPointMatrix(const glm::vec2& point, const float y);
    glm::mat4 genTrackLineMatrix(const glm::vec2& start, const glm::vec2& end, const float y);
    glm::mat4 genTrackArcMatrix(const glm::vec2& center, const float y);
    glm::mat4 genTrackIntersectionMatrix(const glm::vec2& center, const float angle, const float y);
    glm::mat4 genTrackLineMarkerMatrix(const glm::vec2& start,
            const glm::vec2& end, const float y, const Tracks& tracks);

    /*void appendQuad(std::vector<Model::Vertex>& vertices, const glm::vec3& vec0,
            const glm::vec3& vec1, const glm::vec3& vec2, const glm::vec3& vec3);*/

    void getArcVertexParams(const glm::vec2& start, const glm::vec2& end,
            const glm::vec2& center, const bool rightArc, float& baseAngle,
            float& angle, int& numQuads);
};

#endif

