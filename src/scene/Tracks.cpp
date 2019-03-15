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

const std::vector<std::shared_ptr<ControlPoint>>& Tracks::getTracks() const {

    return tracks;
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
