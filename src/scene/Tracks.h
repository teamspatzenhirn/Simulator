#ifndef INC_2019_TRACKS_H
#define INC_2019_TRACKS_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

struct TrackBase;

struct ControlPoint {

    glm::vec2 coords;

    std::vector<std::shared_ptr<TrackBase>> tracks;

    ControlPoint();
    ControlPoint(glm::vec2 coords);
};

enum struct LaneMarking {
    Dashed,
    DoubleSolid,
    DashedAndSolid,
    SolidAndDashed // same as above but with reversed order
};

struct TrackBase {

    virtual ~TrackBase() = 0;

    virtual std::vector<std::weak_ptr<ControlPoint>> getControlPoints() = 0;

    virtual std::vector<glm::vec2> getDirections(const ControlPoint& controlPoint) = 0;

    virtual std::vector<glm::vec2> getPoints(float pointDistance) = 0;
};

struct TrackLine : TrackBase {

    std::weak_ptr<ControlPoint> start;
    std::weak_ptr<ControlPoint> end;

    LaneMarking centerLine{LaneMarking::Dashed};

    TrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end);

    std::vector<std::weak_ptr<ControlPoint>> getControlPoints() override;

    std::vector<glm::vec2> getDirections(const ControlPoint& controlPoint) override;

    std::vector<glm::vec2> getPoints(float pointDistance) override;
};

struct TrackArc : TrackBase {

    std::weak_ptr<ControlPoint> start;
    std::weak_ptr<ControlPoint> end;

    glm::vec2 center;
    float radius{0};
    bool rightArc{false};

    LaneMarking centerLine{LaneMarking::Dashed};

    TrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end,
            const glm::vec2& center, const float radius, const bool rightArc);

    std::vector<std::weak_ptr<ControlPoint>> getControlPoints() override;

    std::vector<glm::vec2> getDirections(const ControlPoint& controlPoint) override;

    std::vector<glm::vec2> getPoints(float pointDistance) override;
};

struct TrackIntersection : TrackBase {

    std::weak_ptr<ControlPoint> center;
    std::vector<std::weak_ptr<ControlPoint>> links;

    TrackIntersection(const std::shared_ptr<ControlPoint>& center,
            const std::vector<std::weak_ptr<ControlPoint>>& links);

    std::vector<std::weak_ptr<ControlPoint>> getControlPoints() override;

    std::vector<glm::vec2> getDirections(const ControlPoint& controlPoint) override;

    std::vector<glm::vec2> getPoints(float pointDistance) override;
};

struct Tracks {

    // total width of a track
    float trackWidth = 0.8f;
    float laneWidth = trackWidth / 2.0;

    // lane markings
    float markingWidth = 0.02f;
    float centerLineLength = 0.2f;
    float centerLineInterrupt = 0.2f;
    float centerLineGap = 0.02f;

    float stopLineWidth = 0.038f;

private:

    std::vector<std::shared_ptr<ControlPoint>> tracks;

public:

    struct TrackSelection {

        std::shared_ptr<TrackBase> track;

        bool changed{false};

    } trackSelection;

    const std::vector<std::shared_ptr<ControlPoint>>& getTracks() const;
    const std::vector<std::shared_ptr<TrackBase>> getTrackSegments() const;

    std::shared_ptr<TrackLine> addTrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end);
    std::shared_ptr<TrackArc> addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc);
    std::shared_ptr<TrackIntersection> addTrackIntersection(const std::shared_ptr<ControlPoint>& center,
            const std::vector<std::shared_ptr<ControlPoint>>& links);

    bool controlPointExists(const std::shared_ptr<ControlPoint>& controlPoint) const;

    void removeControlPoint(std::shared_ptr<ControlPoint>& controlPoint);
    void removeTrack(const std::shared_ptr<TrackBase>& track);

    static bool isConnected(const std::shared_ptr<ControlPoint>& controlPoint, const TrackBase& track);

    std::vector<glm::vec2> getPath(float distBetweenPoints);

private:

    void removeTrackFromControlPoint(const std::weak_ptr<ControlPoint>& cp, const std::shared_ptr<TrackBase>& t);
};

#endif
