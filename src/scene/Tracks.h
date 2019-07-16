#ifndef INC_2019_TRACKS_H
#define INC_2019_TRACKS_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

struct TrackBase;

struct ControlPoint {

    glm::vec2 coords;

    std::vector<std::shared_ptr<TrackBase>> tracks;
};

enum struct LaneMarking {
    Dashed,
    DoubleSolid
};

struct TrackBase {

    virtual ~TrackBase() = 0;

    virtual glm::vec2 getDirection(const ControlPoint& controlPoint) = 0;
};

struct TrackLine : TrackBase {

    std::weak_ptr<ControlPoint> start;
    std::weak_ptr<ControlPoint> end;

    LaneMarking centerLine{LaneMarking::Dashed};

    TrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end);

    glm::vec2 getDirection(const ControlPoint& controlPoint) override;
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

    glm::vec2 getDirection(const ControlPoint& controlPoint) override;
};

struct TrackIntersection : TrackBase {

    std::weak_ptr<ControlPoint> center;
    std::weak_ptr<ControlPoint> link1;
    std::weak_ptr<ControlPoint> link2;
    std::weak_ptr<ControlPoint> link3;
    std::weak_ptr<ControlPoint> link4;

    TrackIntersection(const std::shared_ptr<ControlPoint>& center,
            const std::shared_ptr<ControlPoint>& link1, const std::shared_ptr<ControlPoint>& link2,
            const std::shared_ptr<ControlPoint>& link3, const std::shared_ptr<ControlPoint>& link4);

    glm::vec2 getDirection(const ControlPoint& controlPoint) override;
};

struct Tracks {

    // total width of a track
    float trackWidth = 0.8f;

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

    std::shared_ptr<TrackLine> addTrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end);
    std::shared_ptr<TrackArc> addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc);
    std::shared_ptr<TrackIntersection> addTrackIntersection(const std::shared_ptr<ControlPoint>& center,
            const std::shared_ptr<ControlPoint>& link1, const std::shared_ptr<ControlPoint>& link2,
            const std::shared_ptr<ControlPoint>& link3, const std::shared_ptr<ControlPoint>& link4);

    bool controlPointExists(const std::shared_ptr<ControlPoint>& controlPoint) const;

    void removeControlPoint(std::shared_ptr<ControlPoint>& controlPoint);

    static bool isConnected(const std::shared_ptr<ControlPoint>& controlPoint, const std::shared_ptr<TrackBase>& track);

    std::vector<glm::vec2> getPath(float distBetweenPoints);
};

#endif
