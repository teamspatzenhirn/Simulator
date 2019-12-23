#include "Tracks.h"

#include <algorithm>

ControlPoint::ControlPoint() {
}

ControlPoint::ControlPoint(glm::vec2 coords)
    : coords(coords) {
}

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
        const std::vector<std::weak_ptr<ControlPoint>>& links)
    : center(center), links(links) {
}

glm::vec2 TrackIntersection::getDirection(const ControlPoint& controlPoint) {

    const glm::vec2& centerCoords = center.lock()->coords;

    for (const std::weak_ptr<ControlPoint>& link : links) {
        const std::shared_ptr<ControlPoint>& l = link.lock();
        if (l.get() == &controlPoint) {
            return l->coords - centerCoords;
        }
    }

    // center
    return glm::vec2();
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
        const std::vector<std::shared_ptr<ControlPoint>>& links) {

    // add control points
    if (!controlPointExists(center)) {
        tracks.push_back(center);
    }

    for (const std::shared_ptr<ControlPoint>& link : links) {
        if (!controlPointExists(link)) {
            tracks.push_back(link);
        }
    }

    // create track
    std::vector<std::weak_ptr<ControlPoint>> ls;
    for (const std::shared_ptr<ControlPoint>& link : links) {
        ls.push_back(link);
    }
    std::shared_ptr<TrackIntersection> track = std::make_shared<TrackIntersection>(center, ls);

    center->tracks.push_back(track);
    for (const std::shared_ptr<ControlPoint>& link : links) {
        link->tracks.push_back(track);
    }

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

    while (!controlPoint->tracks.empty()) {
        std::shared_ptr<TrackBase> t = controlPoint->tracks.front();

        auto removeTrack = [&t](const std::weak_ptr<ControlPoint>& cp) {
            std::vector<std::shared_ptr<TrackBase>>& ts = cp.lock()->tracks;
            ts.erase(std::remove(ts.begin(), ts.end(), t), ts.end());
        };

        if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(t)) {
            removeTrack(line->start);
            removeTrack(line->end);
        } else if (std::shared_ptr<TrackArc> arc = std::dynamic_pointer_cast<TrackArc>(t)) {
            removeTrack(arc->start);
            removeTrack(arc->end);
        } else {
            std::shared_ptr<TrackIntersection> intersection = std::dynamic_pointer_cast<TrackIntersection>(t);

            if (controlPoint == intersection->center.lock()
                    || intersection->links.size() == 1) {
                // Remove intersection
                removeTrack(intersection->center);
                for (const std::weak_ptr<ControlPoint>& link : intersection->links) {
                    removeTrack(link);
                }
            } else {
                // Remove link
                removeTrack(controlPoint);
                std::vector<std::weak_ptr<ControlPoint>>& ls = intersection->links;
                ls.erase(std::remove_if(ls.begin(), ls.end(), [&controlPoint](const std::weak_ptr<ControlPoint>& cp) {
                            return cp.lock() == controlPoint;
                        }), ls.end());
            }
        }
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

        for (const std::weak_ptr<ControlPoint>& link : intersection->links) {
            if (controlPoint == link.lock()) {
                return true;
            }
        }

        return controlPoint == intersection->center.lock();
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

              sampleLine(cp->coords, t.center.lock()->coords);

              for (unsigned int i = 1; i < t.links.size(); i++) {
                const std::shared_ptr<ControlPoint>& ptr = t.links[i].lock();
                if (ptr->tracks.size() > 1) {
                    sampleLine(t.center.lock()->coords, ptr->coords);
                    cp = ptr;
                }
              }
            }

            break;
        }
    }

    return pathPoints;
}
