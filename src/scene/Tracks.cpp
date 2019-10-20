#include "Tracks.h"

#include <algorithm>

TrackBase::~TrackBase() {
}

TrackLine::TrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end)
    : start(start), end(end) {
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

std::vector<glm::vec2> TrackLine::getPoints(float pointDistance) {

    std::vector<glm::vec2> pathPoints;

    glm::vec2 start = this->start.lock()->coords;
    glm::vec2 end = this->end.lock()->coords;

    float len = glm::length(end - start);
    glm::vec2 dir = glm::normalize(end - start);

    for(float i = 0; i < len; i += pointDistance) {
        glm::vec2 point = start + i * dir;
        pathPoints.push_back(point);
    }

    return pathPoints;
}

TrackArc::TrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end,
        const glm::vec2& center, const float radius, const bool rightArc)
    : start(start), end(end), center(center), radius(radius), rightArc(rightArc) {
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

std::vector<glm::vec2> TrackArc::getPoints(float pointDistance) {

    glm::vec2 start = this->start.lock()->coords;
    glm::vec2 end = this->end.lock()->coords;

    float angleStart = std::atan2(start.y - center.y, start.x - center.x);
    float angleEnd = std::atan2(end.y - center.y, end.x - center.x);
    float baseAngle = 0;
    float angle = 0;

    if (rightArc) {
        baseAngle = angleStart;
        angle = angleEnd - angleStart;
    } else {
        baseAngle = angleEnd;
        angle = angleStart - angleEnd;
    }
    if (angle < 0) {
        angle += (float)(2 * M_PI);
    }

    float pointRatio = radius * angle / pointDistance;

    std::vector<glm::vec2> points;

    for(float i = 0; i < pointRatio; i += 1) {
        const float currentAngle = baseAngle + i * angle / pointRatio;

        glm::vec2 point{
            center.x + std::cos(currentAngle) * radius,
            center.y + std::sin(currentAngle) * radius };

            points.push_back(point);
    }

    return points;
}

TrackIntersection::TrackIntersection(const std::shared_ptr<ControlPoint>& center,
        const std::shared_ptr<ControlPoint>& link1, const std::shared_ptr<ControlPoint>& link2,
        const std::shared_ptr<ControlPoint>& link3, const std::shared_ptr<ControlPoint>& link4)
    : center(center), link1(link1), link2(link2), link3(link3), link4(link4) {
}

glm::vec2 TrackIntersection::getDirection(const ControlPoint& controlPoint) {

    const std::shared_ptr<ControlPoint>& l1 = this->link1.lock();
    const std::shared_ptr<ControlPoint>& l2 = this->link2.lock();
    const std::shared_ptr<ControlPoint>& l3 = this->link3.lock();
    const std::shared_ptr<ControlPoint>& l4 = this->link4.lock();
    const glm::vec2& centerCoords = this->center.lock()->coords;

    if (l1.get() == &controlPoint) {
        return l1->coords - centerCoords;
    } else if (l2.get() == &controlPoint) {
        return l2->coords - centerCoords;
    } else if (l3.get() == &controlPoint) {
        return l3->coords - centerCoords;
    } else if (l4.get() == &controlPoint) {
        return l4->coords - centerCoords;
    } else {
        // center
        return glm::vec2();
    }
}

std::vector<glm::vec2> TrackIntersection::getPoints(float pointDistance) {

    std::vector<glm::vec2> pathPoints;
    return pathPoints;
}

const std::vector<std::shared_ptr<ControlPoint>>& Tracks::getTracks() const {

    return tracks;
}

const std::vector<std::shared_ptr<TrackBase>> Tracks::getTrackSegments() const {

    std::vector<std::shared_ptr<TrackBase>> trackSegments;
    
    for (const std::shared_ptr<ControlPoint>& cp : tracks) {
        for (std::shared_ptr<TrackBase>& tb : cp->tracks) {
            if (trackSegments.end() == std::find(
                        trackSegments.begin(), 
                        trackSegments.end(), 
                        tb)) {
                trackSegments.push_back(tb);
            }
        }
    }

    return trackSegments;
}

std::shared_ptr<TrackLine> Tracks::addTrackLine(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end) {

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

std::shared_ptr<TrackArc> Tracks::addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc) {

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

std::shared_ptr<TrackIntersection> Tracks::addTrackIntersection(const std::shared_ptr<ControlPoint>& center,
        const std::shared_ptr<ControlPoint>& link1, const std::shared_ptr<ControlPoint>& link2,
        const std::shared_ptr<ControlPoint>& link3, const std::shared_ptr<ControlPoint>& link4) {

    // add control points
    if (!controlPointExists(center)) {
        tracks.push_back(center);
    }

    if (!controlPointExists(link1)) {
        tracks.push_back(link1);
    }

    if (!controlPointExists(link2)) {
        tracks.push_back(link2);
    }

    if (!controlPointExists(link3)) {
        tracks.push_back(link3);
    }

    if (!controlPointExists(link4)) {
        tracks.push_back(link4);
    }

    // create track
    std::shared_ptr<TrackIntersection> track = std::make_shared<TrackIntersection>(center, link1, link2, link3, link4);

    center->tracks.push_back(track);
    link1->tracks.push_back(track);
    link2->tracks.push_back(track);
    link3->tracks.push_back(track);
    link4->tracks.push_back(track);

    return track;
}

bool Tracks::controlPointExists(const std::shared_ptr<ControlPoint>& controlPoint) const {

    for (std::shared_ptr<ControlPoint> const& cp : tracks) {
        if (controlPoint == cp) {
            return true;
        }
    }

    return false;
}

void Tracks::removeControlPoint(std::shared_ptr<ControlPoint>& controlPoint) {

    for (std::shared_ptr<ControlPoint>& other : tracks) {
        other->tracks.erase(
                std::remove_if(
                    other->tracks.begin(),
                    other->tracks.end(),
                    [&](const std::shared_ptr<TrackBase>& b){
                        return isConnected(controlPoint, b);
                    }),
                other->tracks.end());
    }

    tracks.erase(
            std::remove_if(
                tracks.begin(),
                tracks.end(),
                [&](const std::shared_ptr<ControlPoint>& c){
                    return c == controlPoint || c->tracks.empty();
                }),
            tracks.end());
}

bool Tracks::isConnected(const std::shared_ptr<ControlPoint>& controlPoint, const std::shared_ptr<TrackBase>& track) {

    if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(track)) {
        return controlPoint == line->start.lock() || controlPoint == line->end.lock();
    } else if (std::shared_ptr<TrackArc> arc = std::dynamic_pointer_cast<TrackArc>(track)) {
        return controlPoint == arc->start.lock() || controlPoint == arc->end.lock();
    } else {
        std::shared_ptr<TrackIntersection> intersection = std::dynamic_pointer_cast<TrackIntersection>(track);
        return controlPoint == intersection->center.lock()
                || controlPoint == intersection->link1.lock()
                || controlPoint == intersection->link2.lock()
                || controlPoint == intersection->link3.lock()
                || controlPoint == intersection->link4.lock();
    }
}

std::vector<glm::vec2> Tracks::getPath(float distBetweenPoints) {

    if (getTracks().size() < 1) { 
        return {};
    }

    std::shared_ptr<ControlPoint> cp = getTracks().front();

    for (const std::shared_ptr<ControlPoint>& c : getTracks()) {
        if (c->tracks.size() == 1) {
            cp = c;
            break;
        }
    }

    std::vector<std::shared_ptr<ControlPoint>> visitedCps;
    std::vector<std::shared_ptr<TrackBase>> visitedTracks;

    std::vector<glm::vec2> pathPoints;

    auto sampleLine = [&](glm::vec2 start, glm::vec2 end) {
        float len = glm::length(end - start);
        glm::vec2 dir = glm::normalize(end - start);

        for(float i = 0; i < len; i += distBetweenPoints) {
            glm::vec2 point = start + i * dir;
            pathPoints.push_back(point);
        }
    };

    while (std::find(visitedCps.begin(), visitedCps.end(), cp) == visitedCps.end()) {
        visitedCps.push_back(cp);

        if (cp->tracks.size() == 0) {
          break;
        }

        for (std::shared_ptr<TrackBase>& track : cp->tracks) {

            if (std::find(visitedTracks.begin(), visitedTracks.end(), track)
                    != visitedTracks.end()) {
                continue; 
            }

            visitedTracks.push_back(track);

            if(nullptr != dynamic_cast<TrackLine*>(track.get())) {
              TrackLine& t = (TrackLine&)*track;

              std::shared_ptr<ControlPoint> other = t.end.lock();
              if(t.end.lock() == cp) {
                other = t.start.lock();
              }

              sampleLine(cp->coords, other->coords);

              cp = other;
            } else if (nullptr != dynamic_cast<TrackArc*>(track.get())) {
              TrackArc& t = (TrackArc&)*track;

              std::shared_ptr<ControlPoint> other = t.end.lock();
              if(other == cp) {
                other = t.start.lock();
              }

              glm::vec2 start = t.start.lock()->coords;
              glm::vec2 end = t.end.lock()->coords;

              float angleStart = std::atan2(start.y - t.center.y, start.x - t.center.x);
              float angleEnd = std::atan2(end.y - t.center.y, end.x - t.center.x);
              float baseAngle = 0;
              float angle = 0;

              if (t.rightArc) {
                baseAngle = angleStart;
                angle = angleEnd - angleStart;
              }
              else {
                baseAngle = angleEnd;
                angle = angleStart - angleEnd;
              }
              if (angle < 0) {
                angle += (float)(2 * M_PI);
              }

              float pointRatio = t.radius * angle / distBetweenPoints;

              std::vector<glm::vec2> points;

              for(float i = 0; i < pointRatio; i += 1) {
                const float currentAngle = baseAngle + i * angle / pointRatio;

                glm::vec2 point{
                    t.center.x + std::cos(currentAngle) * t.radius,
                    t.center.y + std::sin(currentAngle) * t.radius };

                points.push_back(point);
              }
              
              if (other == t.end.lock() && !t.rightArc) {
                std::reverse(points.begin(), points.end());
              }

              for (glm::vec2& p : points) { 
                  pathPoints.push_back(p);
              }

              cp = other;
            } else if (nullptr != dynamic_cast<TrackIntersection*>(track.get())) {

              TrackIntersection& t = (TrackIntersection&)*track;

              std::shared_ptr<ControlPoint> other = t.link1.lock();

              if(t.link1.lock() == cp) {
                other = t.link3.lock();
              } else if (t.link2.lock() == cp) {
                other = t.link4.lock();
              } else if (t.link3.lock() == cp) {
                other = t.link1.lock();
              } else if (t.link4.lock() == cp) {
                other = t.link2.lock();
              }

              sampleLine(cp->coords, other->coords);

              cp = other;
            }

            break;
        }
    }

    return pathPoints;
}
