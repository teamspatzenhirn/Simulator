#include "Editor.h"

void appendQuad(std::vector<Model::Vertex>& vertices, const glm::vec3& vec0,
        const glm::vec3& vec1, const glm::vec3& vec2, const glm::vec3& vec3) {

    glm::vec3 normalVector(0.0f, 1.0f, 0.0f);
    glm::vec2 texCoords(0.0f, 0.0f);

    Model::Vertex v0{vec0, normalVector, texCoords};
    Model::Vertex v1{vec1, normalVector, texCoords};
    Model::Vertex v2{vec2, normalVector, texCoords};
    Model::Vertex v3{vec3, normalVector, texCoords};

    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v0);
    vertices.push_back(v2);
    vertices.push_back(v3);
}

Model::Material makeMarkerDefaultMaterial() {

    Model::Material material;

    material.ka = glm::vec3(1.0f, 1.0f, 1.0f);
    material.kd = glm::vec3(1.0f, 1.0f, 1.0f);
    material.ks = glm::vec3(0.0f, 0.0f, 0.0f);
    material.ns = 10.0f;

    return material;
}

Model::Material makeMarkerActiveMaterial() {

    Model::Material material;

    material.ka = glm::vec3(1.0f, 1.0f, 1.0f);
    material.kd = glm::vec3(0.0f, 1.0f, 0.0f);
    material.ks = glm::vec3(0.0f, 0.0f, 0.0f);
    material.ns = 10.0f;

    return material;
}


Model::Material makeTrackMaterial() {

    Model::Material material;

    material.ka = glm::vec3(1.0f, 1.0f, 1.0f);
    material.kd = glm::vec3(1.0f, 1.0f, 1.0f);
    material.ks = glm::vec3(0.0f, 0.0f, 0.0f);
    material.ns = 10.0f;

    return material;
}

std::vector<Model::Vertex> makePointVertices() { 

    std::vector<Model::Vertex> vertices;

    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    glm::vec2 texCoords(0.0f, 0.0f);
    glm::vec3 centerCoords(0.0f, 0.0f, 0.0f);
    Model::Vertex center{centerCoords, normal, texCoords};

    const int n = 32; // number of sides
    const float radius = 0.1f;

    for (int i = 0; i < n; i++) {
        float angle1 = (float)(2 * M_PI * i) / n;
        float angle2 = (float)(2 * M_PI * (i + 1)) / n;

        glm::vec3 coords(cos(angle1) * radius, 0.0f, sin(angle1) * radius);
        Model::Vertex v1{coords, normal, texCoords};
        coords = {cos(angle2) * radius, 0.0f, sin(angle2) * radius};
        Model::Vertex v2{coords, normal, texCoords};

        vertices.push_back(center);
        vertices.push_back(v2);
        vertices.push_back(v1);
    }

    return vertices;
}

std::vector<Model::Vertex> makeTrackLineVertices() {

    std::vector<Model::Vertex> vertices;

    glm::vec3 vec0(-1.0f, 0.0f, -1.0f);
    glm::vec3 vec1(-1.0f, 0.0f, 1.0f);
    glm::vec3 vec2(1.0f, 0.0f, 1.0f);
    glm::vec3 vec3(1.0f, 0.0f, -1.0f);

    appendQuad(vertices, vec0, vec1, vec2, vec3);

    return vertices;
}

Editor::Editor() : 
    markerDefaultMaterial{makeMarkerDefaultMaterial()},
    markerActiveMaterial{makeMarkerActiveMaterial()},
    trackMaterial{makeTrackMaterial()},
    pointVertices{makePointVertices()},
    trackLineVertices{makeTrackLineVertices()} {

    // create markers
    genPointVertices(defaultMarker);
    genDefaultMarkerMaterial(defaultMarker);
    defaultMarker.upload();

    genPointVertices(activeMarker);
    genActiveMarkerMaterial(activeMarker);
    activeMarker.upload();

    // track markers
    genTrackLineMarkerVertices(*markerTrackLine);
    genActiveMarkerMaterial(*markerTrackLine);
    markerTrackLine->upload();
}

void Editor::updateInput(Camera& camera, Tracks& tracks, float groundSize) {

    // ImGui input
    if (tracks.trackSelection.changed) {
        std::shared_ptr<TrackBase> track = tracks.trackSelection.track;
        if (track) {
            if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(track)) {
                glm::vec2 start = line->start.lock()->coords;
                glm::vec2 end = line->end.lock()->coords;

                genTrackLineVertices(start, end, line->centerLine, tracks, *trackModels[track]);
                trackModels[track]->upload();

                genTrackLineVertices(start, end, line->centerLine, tracks, *activeTrackModel);
                activeTrackModel->upload();
            } else if (std::shared_ptr<TrackArc> arc = std::dynamic_pointer_cast<TrackArc>(track)) {
                glm::vec2 start = arc->start.lock()->coords;
                glm::vec2 end = arc->end.lock()->coords;

                genTrackArcVertices(start, end, arc->center, arc->radius, arc->rightArc, arc->centerLine, tracks, *trackModels[track]);
                trackModels[track]->upload();

                genTrackArcVertices(start, end, arc->center, arc->radius, arc->rightArc, arc->centerLine, tracks, *activeTrackModel);
                activeTrackModel->upload();
            }
        }

        tracks.trackSelection.changed = false;
    }

    // keyboard / mouse input
    for (const KeyEvent& event : getKeyEvents()) {
        onKey(event.key, event.action, tracks);
    }

    for (const MouseButtonEvent& event : getMouseButtonEvents()) {
        int windowWidth{0}, windowHeight{0};
        glfwGetWindowSize(event.window, &windowWidth, &windowHeight);

        onButton(getCursorX(), getCursorY(), windowWidth, windowHeight, event.button, event.action, camera, tracks, groundSize);
    }

    for (const CursorPosEvent& event : getCursorPosEvents()) {
        int windowWidth{0}, windowHeight{0};
        glfwGetWindowSize(event.window, &windowWidth, &windowHeight);

        onMouseMoved(getCursorX(), getCursorY(), windowWidth, windowHeight, camera, tracks);
    }
}

void Editor::onKey(int key, int action, Tracks& tracks) {

    // track alignment
    if (key == GLFW_KEY_LEFT_CONTROL) {
        if (action == GLFW_PRESS) {
            setAutoAlign(true, tracks);
        }

        if (action == GLFW_RELEASE) {
            setAutoAlign(false, tracks);
        }

        // could also be GLFW_REPEAT
    }

    // track mode
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        setTrackMode(TrackMode::Line, tracks);
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        setTrackMode(TrackMode::Arc, tracks);
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        setTrackMode(TrackMode::Intersection, tracks);
    }

    if ((key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
            || (key == GLFW_KEY_DELETE && action == GLFW_PRESS)) {
        removeActiveControlPoint(tracks);
    }
}

void Editor::onButton(double cursorX, double cursorY, int windowWidth, int windowHeight,
        int button, int action, Camera& camera, Tracks& tracks, float groundSize) {

    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS) {
                if (dragState.dragging) {
                    return;
                }

                // calculate clicked location
                glm::vec2 groundCoords;
                bool positionValid = toGroundCoordinates(cursorX, cursorY, windowWidth, windowHeight, camera, groundCoords);
                if (!positionValid) {
                    return;
                }

                // handle click
                if (!activeControlPoint) {
                    std::shared_ptr<TrackBase> selectedTrack = findTrack(groundCoords, tracks);
                    if (selectedTrack && !selectControlPoint(groundCoords, tracks)) {
                        selectTrack(selectedTrack, tracks);
                    } else {
                        deselectTrack(tracks);

                        if (getEffectiveTrackMode() == TrackMode::Intersection) {
                            createIntersection(groundCoords, tracks, groundSize);
                        } else {
                            startTrack(groundCoords, tracks, groundSize);
                        }
                    }
                } else {
                    std::shared_ptr<ControlPoint> selectedPoint = selectControlPoint(groundCoords, tracks);
                    if (selectedPoint == activeControlPoint) {
                        if (isStartConnected()) {
                            dragState.dragging = true;
                            dragControlPoint(activeControlPoint, tracks);
                        }
                    } else {
                        if (getEffectiveTrackMode() == TrackMode::Intersection) {
                            createIntersection(groundCoords, tracks, groundSize);
                        } else {
                            if (canCreateTrack(tracks)) {
                                endTrack(groundCoords, tracks, groundSize);
                            }
                        }
                    }
                }
            } else {
                if (dragState.dragging) {
                    dragState.dragging = false;
                    moveControlPoint(activeControlPoint, tracks, groundSize);
                }

                updateMarkers(tracks);
            }

            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_PRESS) {
                // cancel track
                deselectControlPoint();
                deselectTrack(tracks);
            }

            break;
    }
}

void Editor::onMouseMoved(double cursorX, double cursorY, int windowWidth, int windowHeight,
        Camera& camera, const Tracks& tracks) {

    glm::vec2 groundCoords;
    bool positionValid = toGroundCoordinates(cursorX, cursorY, windowWidth, windowHeight, camera, groundCoords);
    if (!positionValid) {
        return;
    }

    cursorPos = groundCoords;

    if (dragState.dragging) {
        if (activeControlPoint) {
            dragControlPoint(activeControlPoint, tracks);
        }
    } else {
        updateMarkers(tracks);
    }
}

void Editor::setTrackMode(TrackMode trackMode, const Tracks& tracks) {

    this->trackMode = trackMode;

    updateMarkers(tracks);
}

void Editor::setAutoAlign(bool autoAlign, const Tracks& tracks) {

    this->autoAlign = autoAlign;

    updateMarkers(tracks);
}

void Editor::renderScene(GLuint shaderProgramId, Model& groundModel, const Tracks& tracks, float groundSize) {

    // render ground
    glm::mat4 groundModelMat(1.0f);
    groundModelMat = glm::scale(groundModelMat, glm::vec3(groundSize, 1.0f, groundSize));

    glm::vec3 groundColor{0.05, 0.05, 0.05};

    groundModel.material.ka = groundColor;
    groundModel.material.kd = groundColor;
    groundModel.material.ks = groundColor;

    groundModel.render(shaderProgramId, groundModelMat);

    // render tracks
    std::set<std::shared_ptr<TrackBase>> ts;
    for (std::shared_ptr<ControlPoint> const& cp : tracks.getTracks()) {
        for (std::shared_ptr<TrackBase> const& t : cp->tracks) {
            ts.insert(t);
        }
    }

    for (std::shared_ptr<TrackBase> const& track : ts) {

        if (trackModels.end() == trackModels.find(track)) {

            // TODO: move model creation to somewhere else!

            if (TrackArc* arc = dynamic_cast<TrackArc*>(track.get())) {
                std::shared_ptr<Model> model = std::make_shared<Model>();
                genTrackArcVertices(
                        arc->start.lock()->coords,
                        arc->end.lock()->coords,
                        arc->center,
                        arc->radius,
                        arc->rightArc,
                        arc->centerLine,
                        tracks,
                        *model);
                genTrackMaterial(*model);
                model->upload();
                trackModels[track] = model;

                glm::mat4 modelMat = genTrackArcMatrix(arc->center, trackYOffset);
                trackModelMats[track] = modelMat;
            } else if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(track)) {
                std::shared_ptr<Model> model = std::make_shared<Model>();
                genTrackLineVertices(
                        line->start.lock()->coords,
                        line->end.lock()->coords,
                        line->centerLine,
                        tracks,
                        *model);
                genTrackMaterial(*model);
                model->upload();

                trackModels[track] = model;
                glm::mat4 modelMat = genTrackLineMatrix(
                        line->start.lock()->coords,
                        line->end.lock()->coords,
                        trackYOffset);
                trackModelMats[track] = modelMat;
            } else {
                std::shared_ptr<TrackIntersection> intersection = std::dynamic_pointer_cast<TrackIntersection>(track);
                trackModels[track] = genTrackIntersectionModel(*intersection, tracks);

                const glm::vec2& center = intersection->center.lock()->coords;
                trackModelMats[track] = genTrackIntersectionMatrix(center, trackYOffset);
            }
        }

        if (dragState.dragging) {
            const std::shared_ptr<Model>& model =
                    (dragState.trackModels.find(track) != dragState.trackModels.end()
                    ? dragState.trackModels[track] : trackModels[track]);
            const glm::mat4& mat =
                    (dragState.trackModelMats.find(track) != dragState.trackModelMats.end()
                    ? dragState.trackModelMats[track] : trackModelMats[track]);
            model->render(shaderProgramId, mat);
        } else {
            trackModels[track]->render(shaderProgramId, trackModelMats[track]);
        }
    }
}

void Editor::renderMarkers(GLuint shaderProgramId, const Tracks& tracks, const glm::vec3 cameraPosition) {

    // render control points
    for (const std::shared_ptr<ControlPoint>& cp : tracks.getTracks()) {

        // only render active point at connection
        if (cp == dragState.connectedPoint) {
            continue;
        }

        glm::vec2 position;
        if (dragState.dragging && dragState.coords.find(cp) != dragState.coords.end()) {
            position = dragState.coords[cp];
        } else {
            position = cp->coords;
        }

        renderMarker(shaderProgramId, position, cp == activeControlPoint, cameraPosition);
    }

    if (activeControlPoint && !tracks.controlPointExists(activeControlPoint)) {

        renderMarker(shaderProgramId, activeControlPoint->coords, true, cameraPosition);
    }

    // render new track markers
    if (canCreateTrack(tracks)) {
        if (getEffectiveTrackMode() == TrackMode::Intersection) {
            trackMarker->render(shaderProgramId, trackMarkerMat);
        } else {
            if (activeControlPoint) {
                activeMarker.render(shaderProgramId, markerModelMatEnd);

                trackMarker->render(shaderProgramId, trackMarkerMat);
            }
        }
    }

    // render active track marker
    if (activeTrack) {
        activeTrackModel->render(shaderProgramId, activeTrackMat);
    }
}

void Editor::renderMarker(GLuint shaderProgramId, const glm::vec2& position,
        const bool active, const glm::vec3& cameraPosition) {

    // scale marker by distance
    float scale = glm::length(cameraPosition - glm::vec3(position.x, 0, position.y)) * 0.15f;

    // create model matrix
    glm::mat4 modelMat = genPointMatrix(position, 0.0f);
    modelMat = glm::scale(modelMat, glm::vec3(scale, 0, scale));

    // render
    if (active) {
        activeMarker.render(shaderProgramId, modelMat);
    } else {
        defaultMarker.render(shaderProgramId, modelMat);
    }
}

void Editor::selectTrack(const std::shared_ptr<TrackBase>& track, Tracks& tracks) {

    activeTrack = track;

    activeTrackModel = std::make_shared<Model>();

    if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(track)) {
        glm::vec2 start = line->start.lock()->coords;
        glm::vec2 end = line->end.lock()->coords;

        genTrackLineVertices(start, end, line->centerLine, tracks, *activeTrackModel);

        activeTrackMat = genTrackLineMatrix(start, end, markerYOffset);
    } else if (std::shared_ptr<TrackArc> arc = std::dynamic_pointer_cast<TrackArc>(track)) {
        genTrackArcVertices(arc->start.lock()->coords, arc->end.lock()->coords,
                arc->center, arc->radius, arc->rightArc, arc->centerLine, tracks, *activeTrackModel);

        activeTrackMat = genTrackArcMatrix(arc->center, markerYOffset);
    } else {
        std::shared_ptr<TrackIntersection> intersection = std::dynamic_pointer_cast<TrackIntersection>(track);

        genTrackIntersectionVertices(*intersection, tracks, *activeTrackModel);

        glm::vec2 center = intersection->center.lock()->coords;
        activeTrackMat = genTrackIntersectionMatrix(center, markerYOffset);
    }

    genActiveMarkerMaterial(*activeTrackModel);
    activeTrackModel->upload();

    tracks.trackSelection.track = track;
}

void Editor::startTrack(const glm::vec2& position, const Tracks& tracks, float groundSize) {

    // check connection to existing tracks
    std::shared_ptr<ControlPoint> connectedStart = selectControlPoint(position, tracks);

    if (connectedStart) {
        activeControlPoint = connectedStart;
    } else {
        // validate position
        if (position.x < -groundSize || position.x > groundSize || position.y < -groundSize || position.y > groundSize) {
            return;
        }

        activeControlPoint = std::make_shared<ControlPoint>();
        activeControlPoint->coords = position;
    }

    // update temporary state
    cursorPos = position;

    // update markers (end and track marker depend on start position updated before)
    updateMarkers(tracks);
}

void Editor::endTrack(const glm::vec2& position, Tracks& tracks, float groundSize) {

    // update temporary state
    glm::vec2 end = align(*activeControlPoint, position, tracks);

    if (end.x < -groundSize || end.x > groundSize || end.y < -groundSize || end.y > groundSize) {
        if (!isStartConnected()) {
            deselectControlPoint();
        }

        return;
    }

    // find or create end point
    std::shared_ptr<ControlPoint> trackEnd = selectControlPoint(position, tracks, true, false);

    if (autoAlign || !trackEnd || trackEnd == activeControlPoint) {
        trackEnd = std::make_shared<ControlPoint>();
        trackEnd->coords = end;
    }

    // update scene and temporary state
    if (getEffectiveTrackMode() == TrackMode::Line) {
        addTrackLine(activeControlPoint, trackEnd, tracks);
    } else {
        // track should always be connected at its start

        glm::vec2 center;
        float radius{0};
        bool rightArc{false};
        bool isArc = getArc(*activeControlPoint, end, center, radius, rightArc);

        if (isArc) {
            addTrackArc(activeControlPoint, trackEnd, center, radius, rightArc, tracks);
        } else {
            addTrackLine(activeControlPoint, trackEnd, tracks);
        }
    }

    activeControlPoint = trackEnd;

    updateMarkers(tracks);
}

void Editor::createIntersection(const glm::vec2& position, Tracks& tracks, const float groundSize) {

    float d = intersectionTrackLength + tracks.trackWidth / 2;

    if (position.x < -groundSize + d || position.x > groundSize - d || position.y < -groundSize + d || position.y > groundSize - d) {
        return;
    }

    std::shared_ptr<ControlPoint> center = std::make_shared<ControlPoint>(position);
    std::vector<std::shared_ptr<ControlPoint>> links = {
        std::make_shared<ControlPoint>(position + glm::vec2(d, 0)),
        std::make_shared<ControlPoint>(position + glm::vec2(0, d)),
        std::make_shared<ControlPoint>(position + glm::vec2(-d, 0)),
        std::make_shared<ControlPoint>(position + glm::vec2(0, -d))
    };

    addTrackIntersection(center, links, tracks);

    activeControlPoint = center;
}

void Editor::addTrackLine(const std::shared_ptr<ControlPoint>& start,
        const std::shared_ptr<ControlPoint>& end, Tracks& tracks) {

    // add track
    std::shared_ptr<TrackLine> track = tracks.addTrackLine(start, end);

    // create model
    trackModels[track] = genTrackLineModel(start->coords, end->coords, track->centerLine, tracks);
    trackModelMats[track] = genTrackLineMatrix(start->coords, end->coords, trackYOffset);
}

void Editor::addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc, Tracks& tracks) {

    // add track
    std::shared_ptr<TrackArc> track = tracks.addTrackArc(start, end, center, radius, rightArc);

    // create model
    trackModels[track] = genTrackArcModel(start->coords, end->coords, center, radius, rightArc, track->centerLine, tracks);
    trackModelMats[track] = genTrackArcMatrix(center, trackYOffset);
}

void Editor::addTrackIntersection(const std::shared_ptr<ControlPoint>& center,
        const std::vector<std::shared_ptr<ControlPoint>>& links, Tracks& tracks) {

    // add track
    std::shared_ptr<TrackIntersection> track = tracks.addTrackIntersection(center, links);

    // create model
    trackModels[track] = genTrackIntersectionModel(*track, tracks);
    trackModelMats[track] = genTrackIntersectionMatrix(center->coords, trackYOffset);
}

void Editor::removeActiveControlPoint(Tracks& tracks) {

    if (activeControlPoint != nullptr) {
        std::vector<std::shared_ptr<TrackBase>> ts = activeControlPoint->tracks;

        // Remove control point and update tracks
        tracks.removeControlPoint(activeControlPoint);

        // Deselect removed control point
        deselectControlPoint();

        // Update affected track models
        for (const std::shared_ptr<TrackBase>& t : ts) {
            if (const std::shared_ptr<TrackIntersection> intersection = std::dynamic_pointer_cast<TrackIntersection>(t)) {
                trackModels[t] = genTrackIntersectionModel(*intersection, tracks);
            }
        }
    }
}

void Editor::dragControlPoint(const std::shared_ptr<ControlPoint>& controlPoint, const Tracks& tracks) {

    std::shared_ptr<TrackIntersection> intersection = findIntersection(*controlPoint);

    if (intersection && intersection->center.lock() == controlPoint) {
        // Dragging center point of intersection

        // Move all intersection control points by the same offset
        const glm::vec2 cc = intersection->center.lock()->coords;
        glm::vec2 offset = cursorPos - cc;
        dragState.coords[intersection->center.lock()] = cursorPos;
        for (const std::weak_ptr<ControlPoint>& link : intersection->links) {
            dragState.coords[link.lock()] = link.lock()->coords + offset;
        }

        std::vector<std::shared_ptr<ControlPoint>> controlPoints{{intersection->center.lock()}};
        for (const std::weak_ptr<ControlPoint>& link : intersection->links) {
            controlPoints.push_back(link.lock());
        }
        moveTracksAtControlPoint(controlPoints, false, tracks);
    } else {
        // Dragging any other control point

        std::shared_ptr<ControlPoint> selectedPoint = selectControlPoint(cursorPos, tracks, false, false);

        dragState.coords[controlPoint] = (selectedPoint ? selectedPoint->coords : cursorPos);
        dragState.connectedPoint = selectedPoint;

        moveTracksAtControlPoint({controlPoint}, false, tracks);
    }
}

void Editor::moveControlPoint(std::shared_ptr<ControlPoint>& controlPoint, Tracks& tracks, float groundSize) {

    // validate new positions
    for (const std::pair<const std::shared_ptr<ControlPoint>, glm::vec2>& p : dragState.coords) {
        const glm::vec2& pos = p.second;
        if (pos.x < -groundSize || pos.x > groundSize || pos.y < -groundSize || pos.y > groundSize) {
            return;
        }
    }

    // update connected tracks
    std::vector<std::shared_ptr<ControlPoint>> controlPoints;
    for (const std::pair<const std::shared_ptr<ControlPoint>, glm::vec2>& p : dragState.coords) {
        controlPoints.push_back(p.first);
    }
    moveTracksAtControlPoint(controlPoints, true, tracks);

    // update control points
    for (const std::pair<const std::shared_ptr<ControlPoint>, glm::vec2>& p : dragState.coords) {
        p.first->coords = p.second;
    }

    // connect control points
    if (dragState.connectedPoint) {
        for (auto it = controlPoint->tracks.begin(); it != controlPoint->tracks.end();) {
            std::shared_ptr<TrackBase>& track = *it;

            // Handle tracks connected to both control points
            if (Tracks::isConnected(dragState.connectedPoint, *track)) {
                if (std::shared_ptr<TrackIntersection> intersection = std::dynamic_pointer_cast<TrackIntersection>(track)) {
                    // Remove duplicate link
                    intersection->links.erase(
                            std::remove_if(intersection->links.begin(), intersection->links.end(),
                                [&controlPoint](const std::weak_ptr<ControlPoint>& link) {
                                    return link.lock() == controlPoint;
                                }),
                            intersection->links.end());
                } else {
                    // Remove line or arc entirely
                    dragState.connectedPoint->tracks.erase(
                            std::remove(dragState.connectedPoint->tracks.begin(),
                                dragState.connectedPoint->tracks.end(),
                                track),
                            dragState.connectedPoint->tracks.end());
                }

                it = controlPoint->tracks.erase(it);

                continue;
            }

            // Disconnect track from active control point and connect it to the other control point
            if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(track)) {
                if (line->start.lock() == controlPoint) {
                    line->start = dragState.connectedPoint;
                } else {
                    line->end = dragState.connectedPoint;
                }
            } else if (std::shared_ptr<TrackArc> arc = std::dynamic_pointer_cast<TrackArc>(track)) {
                if (arc->start.lock() == controlPoint) {
                    arc->start = dragState.connectedPoint;
                } else {
                    arc->end = dragState.connectedPoint;
                }
            } else {
                std::shared_ptr<TrackIntersection> intersection = std::dynamic_pointer_cast<TrackIntersection>(track);
                for (unsigned int i = 0; i < intersection->links.size(); i++) {
                    if (intersection->links[i].lock() == controlPoint) {
                        intersection->links[i] = dragState.connectedPoint;
                        break;
                    }
                }
            }

            dragState.connectedPoint->tracks.push_back(track);

            it++;
        }

        controlPoint->tracks.clear();
        tracks.removeControlPoint(controlPoint);

        activeControlPoint = dragState.connectedPoint;
    }

    // clear drag state
    dragState.connectedPoint = nullptr;
    dragState.coords.clear();
    dragState.trackModels.clear();
    dragState.trackModelMats.clear();
}

void Editor::moveTracksAtControlPoint(const std::vector<std::shared_ptr<ControlPoint>>& controlPoints,
        bool applyMovement, const Tracks& tracks) {

    // collect affected tracks
    std::set<std::shared_ptr<TrackBase>> movedTracks;
    for (const std::shared_ptr<ControlPoint>& cp : controlPoints) {
        for (const std::shared_ptr<TrackBase>& t : cp->tracks) {
            movedTracks.insert(t);
        }
    }

    // update tracks
    for (const std::shared_ptr<TrackBase>& track : movedTracks) {
        if (std::shared_ptr<TrackIntersection> intersection = std::dynamic_pointer_cast<TrackIntersection>(track)) {
            if (applyMovement) {
                trackModels[track] = dragState.trackModels[track];
                trackModelMats[track] = dragState.trackModelMats[track];
            } else {
                dragState.trackModels[track] = genTrackIntersectionModel(*intersection, tracks);

                glm::vec2 pos = getDraggedPosition(intersection->center.lock());
                dragState.trackModelMats[track] = genTrackIntersectionMatrix(pos, trackYOffset);
            }

            continue;
        }

        std::shared_ptr<ControlPoint> trackStart;
        std::shared_ptr<ControlPoint> trackEnd;
        LaneMarking centerLine{LaneMarking::Dashed};
        if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(track)) {
            trackStart = line->start.lock();
            trackEnd = line->end.lock();
            centerLine = line->centerLine;
        } else {
            std::shared_ptr<TrackArc> arc = std::dynamic_pointer_cast<TrackArc>(track);
            trackStart = arc->start.lock();
            trackEnd = arc->end.lock();
            centerLine = arc->centerLine;
        }

        glm::vec2 startCoords = getDraggedPosition(trackStart);
        glm::vec2 endCoords = getDraggedPosition(trackEnd);

        if (std::shared_ptr<TrackArc> trackArc = std::dynamic_pointer_cast<TrackArc>(track)) {
            // keep direction at fixed point
            bool movingStart = false;
            glm::vec2 arcStart;
            glm::vec2 arcEnd;
            glm::vec2 direction;
            if (isDragged(trackStart) && isDragged(trackEnd)) {
                arcStart = getDraggedPosition(trackStart);
                arcEnd = getDraggedPosition(trackEnd);

                glm::vec2 v1 = trackEnd->coords - trackStart->coords;
                glm::vec2 v2 = dragState.coords[trackEnd] - dragState.coords[trackStart];
                direction = -track->getDirection(*trackStart);
                float angle = std::atan2(direction.y, direction.x) + std::atan2(v2.y, v2.x) - std::atan2(v1.y, v1.x);
                direction = glm::vec2(cos(angle) * glm::length(direction), sin(angle) * glm::length(direction));
            } else if (isDragged(trackStart)) {
                movingStart = true;
                arcStart = getDraggedPosition(trackEnd);
                arcEnd = getDraggedPosition(trackStart);
                direction = -track->getDirection(*trackEnd);
            } else {
                arcStart = getDraggedPosition(trackStart);
                arcEnd = getDraggedPosition(trackEnd);
                direction = -track->getDirection(*trackStart);
            }
            std::vector<glm::vec2> directions = {direction};

            glm::vec2 center;
            float radius{0};
            bool rightArc{false};
            bool isArc = getArc(arcStart, arcEnd, directions, center, radius, rightArc);

            if (isArc) {
                if (movingStart) {
                    rightArc = !rightArc;
                }

                if (applyMovement) {
                    // update data
                    trackArc->center = center;
                    trackArc->radius = radius;
                    trackArc->rightArc = rightArc;

                    // update model
                    trackModels[track] = dragState.trackModels[track];
                    trackModelMats[track] = dragState.trackModelMats[track];
                } else {
                    // update temporary model
                    dragState.trackModels[track] = genTrackArcModel(startCoords, endCoords, center, radius, rightArc, centerLine, tracks);
                    dragState.trackModelMats[track] = genTrackArcMatrix(center, trackYOffset);
                }
            } // TODO properly handle else case
        } else {
            if (applyMovement) {
                // update model
                trackModels[track] = dragState.trackModels[track];
                trackModelMats[track] = dragState.trackModelMats[track];
            } else {
                // update temporary model
                dragState.trackModels[track] = genTrackLineModel(startCoords, endCoords, centerLine, tracks);
                dragState.trackModelMats[track] = genTrackLineMatrix(startCoords, endCoords, trackYOffset);
            }
        }
    }
}

std::shared_ptr<TrackIntersection> Editor::findIntersection(const ControlPoint& cp) const {

    auto intersection = std::find_if(
            cp.tracks.begin(), cp.tracks.end(),
            [](const std::shared_ptr<TrackBase>& track) {
                return std::dynamic_pointer_cast<TrackIntersection>(track);
            });

    if (intersection == cp.tracks.end()) {
        return nullptr;
    } else {
        return std::dynamic_pointer_cast<TrackIntersection>(*intersection);
    }
}

glm::vec2 Editor::getDraggedPosition(const std::shared_ptr<ControlPoint>& cp) const {

    if (isDragged(cp)) {
        return dragState.coords.at(cp);
    } else {
        return cp->coords;
    }
}

bool Editor::isDragged(const std::shared_ptr<ControlPoint>& cp) const {

    return dragState.coords.find(cp) != dragState.coords.end();
}

std::shared_ptr<ControlPoint> Editor::selectControlPoint(const glm::vec2& position, const Tracks& tracks) const {

    return selectControlPoint(position, tracks, true, true);
}

std::shared_ptr<ControlPoint> Editor::selectControlPoint(const glm::vec2& position,
        const Tracks& tracks, const bool includeActiveControlPoint, const bool includeCompleteControlPoints) const {

    std::shared_ptr<ControlPoint> controlPoint;
    float closest{controlPointClickRadius};

    // check scene control points
    for (std::shared_ptr<ControlPoint> const& cp : tracks.getTracks()) {
        if (cp == activeControlPoint && !includeActiveControlPoint) {
            continue;
        }

        if (isComplete(*cp) && !includeCompleteControlPoints) {
            continue;
        }

        float distance = glm::distance(cp->coords, position);
        if (distance < closest) {
            controlPoint = cp;

            closest = distance;
        }
    }

    // check active control point (can also be a scene control point)
    if (activeControlPoint && includeActiveControlPoint) {
        if (!isComplete(*activeControlPoint) || includeCompleteControlPoints) {
            float distance = glm::distance(activeControlPoint->coords, position);
            if (distance < closest) {
                return activeControlPoint;
            }
        }
    }

    return controlPoint;
}

std::shared_ptr<TrackBase> Editor::findTrack(const glm::vec2& position, const Tracks& tracks) const {

    std::shared_ptr<TrackBase> selectedTrack;
    float minDistance{tracks.trackWidth / 2};

    for (const std::shared_ptr<ControlPoint>& controlPoint : tracks.getTracks()) {
        for (const std::shared_ptr<TrackBase>& track : controlPoint->tracks) {
            float distance{0};
            if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(track)) {
                glm::vec2 start(line->start.lock()->coords);
                glm::vec2 end(line->end.lock()->coords);
                if (glm::dot(end - start, position - start) < 0
                        || glm::dot(start - end, position - end) < 0) {
                    continue;
                }
                distance = std::fabs(distanceLinePoint(start, end, position));
            } else if (std::shared_ptr<TrackArc> arc = std::dynamic_pointer_cast<TrackArc>(track)) {
                glm::vec2 start = arc->start.lock()->coords;
                glm::vec2 end = arc->end.lock()->coords;
                glm::vec2 center = arc->center;
                float angleStart = std::atan2(start.y - center.y, start.x - center.x);
                float angleEnd = std::atan2(end.y - center.y, end.x - center.x);
                float angle1 = (arc->rightArc ? angleStart : angleEnd);
                float angle2 = (arc->rightArc ? angleEnd : angleStart);
                float anglePosition = std::atan2(position.y - center.y, position.x - center.x);
                if (angle2 > angle1) {
                    if (anglePosition < angle1 || anglePosition > angle2) {
                        continue;
                    }
                } else {
                    if (anglePosition > angle2 && anglePosition < angle1) {
                        continue;
                    }
                }
                distance = std::fabs(glm::distance(center, position) - arc->radius);
            } else {
                std::shared_ptr<TrackIntersection> intersection = std::dynamic_pointer_cast<TrackIntersection>(track);
                distance = minDistance;
                glm::vec2 cc = intersection->center.lock()->coords;
                for (const std::weak_ptr<ControlPoint>& link : intersection->links) {
                    glm::vec2 lc = link.lock()->coords;
                    if (glm::dot(lc - cc, position - cc) < 0
                            || glm::dot(cc - lc, position - lc) < 0) {
                        continue;
                    }
                    float d = std::fabs(distanceLinePoint(cc, lc, position));
                    if (d < distance) {
                        distance = d;
                    }
                }
            }

            if (distance < minDistance) {
                selectedTrack = track;
                minDistance = distance;
            }
        }
    }

    return selectedTrack;
}

bool Editor::toGroundCoordinates(const double cursorX, const double cursorY, const int windowWidth, const int windowHeight,
        Camera& camera, glm::vec2& groundCoords) {

    glm::vec3 pick = camera.pickRay(cursorX, cursorY, windowWidth, windowHeight);
    if (0 == pick.y) {
        return false;
    }

    glm::vec3 cameraPos = camera.pose.position; 

    float t = -cameraPos.y / pick.y;
    if (t < 0) {
        return false;
    }

    groundCoords.x = cameraPos.x + t * pick.x;
    groundCoords.y = cameraPos.z + t * pick.z;

    return true;
}

void Editor::updateMarkers(const Tracks& tracks) {

    if (!canCreateTrack(tracks)) {
        return;
    }

    if (getEffectiveTrackMode() == TrackMode::Intersection) {

        if (markerTrackIntersection->vertices.size() == 0) {
            genTrackIntersectionMarkerVertices(tracks, *markerTrackIntersection);
            genActiveMarkerMaterial(*markerTrackIntersection);
            markerTrackIntersection->upload();
        }

        trackMarker = markerTrackIntersection;
        trackMarkerMat = genTrackIntersectionMatrix(cursorPos, markerYOffset);
    } else {
        if (!activeControlPoint) {
            return;
        }

        const ControlPoint& startPoint = *activeControlPoint;

        glm::vec2 end = align(startPoint, cursorPos, tracks);

        // update end marker
        markerModelMatEnd = genPointMatrix(end, markerYOffset);

        // update track marker
        if (getEffectiveTrackMode() == TrackMode::Line) {
            updateTrackLineMarker(startPoint.coords, end, tracks);
        } else {
            updateTrackArcMarker(startPoint, end, tracks);
        }
    }
}

void Editor::updateTrackLineMarker(const glm::vec2& start, const glm::vec2& end, const Tracks& tracks) {

    trackMarker = markerTrackLine;
    trackMarkerMat = genTrackLineMarkerMatrix(start, end, markerYOffset, tracks);
}

void Editor::updateTrackArcMarker(const ControlPoint& startPoint,
        const glm::vec2& end, const Tracks& tracks) {

    glm::vec2 start = startPoint.coords;

    glm::vec2 center;
    float radius{0};
    bool rightArc{false};
    bool isArc = getArc(startPoint, end, center, radius, rightArc);

    if (!isArc) {
        // line

        // update model
        trackMarker = markerTrackLine;

        // update model matrix
        trackMarkerMat = genTrackLineMarkerMatrix(start, end, markerYOffset, tracks);
    } else {
        // arc

        // update model
        trackMarker = std::make_shared<Model>();
        genTrackArcMarkerVertices(start, end, center, radius, rightArc, tracks, *trackMarker);
        genActiveMarkerMaterial(*trackMarker);
        trackMarker->upload();

        // update model matrix
        trackMarkerMat = genTrackArcMatrix(center, markerYOffset);
    }
}

bool Editor::getArc(const ControlPoint& start, const glm::vec2& end,
        glm::vec2& center, float& radius, bool& rightArc) {

    std::vector<glm::vec2> directions = getAlignmentVectors(start);
    return getArc(start.coords, end, directions, center, radius, rightArc);
}

bool Editor::getArc(const glm::vec2& start, const glm::vec2& end, const std::vector<glm::vec2>& directions,
        glm::vec2& center, float& radius, bool& rightArc) {

    glm::vec2 direction = getAlignedUnitVector(start, end, directions);
    glm::vec2 r1 = glm::vec2(-direction.y, direction.x); // 90 degrees right

    glm::vec2 m = (start + end) * 0.5f;
    glm::vec2 h = end - start;
    glm::vec2 r2 = glm::vec2(-h.y, h.x);

    float t{0};
    bool intersected = intersectParam(start, r1, m, r2, t);

    if (!intersected) {
        // line
        return false;
    } else {
        // arc
        center = start + t * r1;
        radius = std::fabs(t);
        rightArc = (t > 0);

        return true;
    }
}

float Editor::distanceLinePoint(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& x) const {

    glm::vec2 n = glm::normalize(glm::vec2(p1.y - p2.y, p2.x - p1.x));
    return glm::dot(n, x - p1);
}

bool Editor::intersectParam(const glm::vec2& p1, const glm::vec2& r1, const glm::vec2& p2, const glm::vec2& r2, float& t1) {

    // validate
    if ((r1.x == 0 && r1.y == 0) || (r2.x == 0 && r2.y == 0)) {
        return false;
    }

    // intersect
    float m(0);
    float x(0);
    if (fabs(r2.x) > fabs(r2.y)) {
        float f = r2.y / r2.x;
        m = r1.y - f * r1.x;
        x = (p2.y - p1.y) - f * (p2.x - p1.x);
    } else {
        float f = r2.x / r2.y;
        m = r1.x - f * r1.y;
        x = (p2.x - p1.x) - f * (p2.y - p1.y);
    }

    if (0 == m) {
        // lines are parallel
        return false;
    }

    t1 = x / m;

    return true;
}

glm::vec2 Editor::align(const ControlPoint& startPoint, const glm::vec2& position, const Tracks& tracks) {

    if (autoAlign) {
        glm::vec2 exactVector = position - startPoint.coords;
        std::vector<glm::vec2> directions = getAlignmentVectors(startPoint);
        glm::vec2 alignedUnitVector = getAlignedUnitVector(startPoint.coords, position, directions);

        if (getEffectiveTrackMode() == TrackMode::Line) {

            glm::vec2 alignedVector = glm::dot(alignedUnitVector, exactVector) * alignedUnitVector;

            return startPoint.coords + alignedVector;
        } else {
            return position;
        }
    } else {
        std::shared_ptr<ControlPoint> connectedEnd = selectControlPoint(position, tracks, true, false);
        if (connectedEnd && connectedEnd.get() != &startPoint) {
            return connectedEnd->coords;
        } else {
            return position;
        }
    }
}

glm::vec2 Editor::getAlignedUnitVector(const glm::vec2& startPoint, const glm::vec2& endPoint,
        const std::vector<glm::vec2>& directions) {

    glm::vec2 exactVector = endPoint - startPoint;
    glm::vec2 exactUnitVector = glm::normalize(exactVector);

    glm::vec2 alignedUnitVector;
    float maxDot = -2.0f; // less than -1.0f
    for (glm::vec2 const& dir : directions) {
        glm::vec2 unitDir = glm::normalize(dir);
        float dot = glm::dot(exactUnitVector, unitDir);
        if (dot > maxDot) {
            alignedUnitVector = unitDir;
            maxDot = dot;
        }
    }

    return alignedUnitVector;
}

std::vector<glm::vec2> Editor::getAlignmentVectors(const ControlPoint& cp) {

    std::vector<glm::vec2> directions;

    // calculate directions from connected tracks
    for (const std::shared_ptr<TrackBase>& track : cp.tracks) {
        glm::vec2 dir = track->getDirection(cp);

        directions.push_back(dir);
    }

    // use default directions if no tracks are connected
    if (directions.empty()) {
        for (int i = 0; i < 8; i++) {
            float angle = (float)i * (float)M_PI * 0.25f;
            directions.push_back(glm::vec2(cos(angle), sin(angle)));
        }
    }

    return directions;
}

Editor::TrackMode Editor::getEffectiveTrackMode() {
    if (!isStartConnected() && TrackMode::Arc == trackMode) {
        return TrackMode::Line;
    } else {
        return trackMode;
    }
}

bool Editor::isStartConnected() {

    return activeControlPoint && !activeControlPoint->tracks.empty();
}

void Editor::deselectControlPoint() {

    activeControlPoint = nullptr;

    dragState.dragging = false;
    dragState.connectedPoint = nullptr;
    dragState.coords.clear();
    dragState.trackModels.clear();
    dragState.trackModelMats.clear();
}

void Editor::deselectTrack(Tracks& tracks) {

    activeTrack = nullptr;
    activeTrackModel = nullptr;

    tracks.trackSelection.track = nullptr;
}

bool Editor::canCreateTrack(const Tracks& tracks) {

    if (maybeDragging(tracks)) {
        return false;
    }

    if (getEffectiveTrackMode() != TrackMode::Intersection
            && activeControlPoint && isComplete(*activeControlPoint)) {
        return false;
    }

    return true;
}

bool Editor::isComplete(const ControlPoint& cp) const {

    std::shared_ptr<TrackIntersection> intersection = findIntersection(cp);
    return intersection && intersection->center.lock().get() == &cp;
}

bool Editor::maybeDragging(const Tracks& tracks) {

    return dragState.dragging || (activeControlPoint && selectControlPoint(cursorPos, tracks) == activeControlPoint);
}

std::shared_ptr<Model> Editor::genTrackLineModel(const glm::vec2& start, const glm::vec2& end,
        const LaneMarking centerLine, const Tracks& tracks) {

    std::shared_ptr<Model> model = std::make_shared<Model>();
    genTrackLineVertices(start, end, centerLine, tracks, *model);
    genTrackMaterial(*model);
    model->upload();

    return model;
}

std::shared_ptr<Model> Editor::genTrackArcModel(const glm::vec2& start, const glm::vec2& end,
        const glm::vec2& center, const float radius, const bool rightArc,
        const LaneMarking centerLine, const Tracks& tracks) {

    std::shared_ptr<Model> model = std::make_shared<Model>();
    genTrackArcVertices(start, end, center, radius, rightArc, centerLine, tracks, *model);
    genTrackMaterial(*model);
    model->upload();

    return model;
}

std::shared_ptr<Model> Editor::genTrackIntersectionModel(const TrackIntersection& intersection,
        const Tracks& tracks) {

    std::shared_ptr<Model> model = std::make_shared<Model>();
    genTrackIntersectionVertices(intersection, tracks, *model);
    genTrackMaterial(*model);
    model->upload();

    return model;
}

void Editor::genDefaultMarkerMaterial(Model& model) {

    model.material = markerDefaultMaterial;
}

void Editor::genActiveMarkerMaterial(Model& model) {

    model.material = markerActiveMaterial;
}

void Editor::genTrackMaterial(Model& model) {

    model.material = trackMaterial;
}

void Editor::genPointVertices(Model& model) {

    model.vertices = pointVertices;
}

void Editor::genTrackLineVertices(const glm::vec2& start, const glm::vec2& end,
        const LaneMarking centerLine, const Tracks& tracks, Model& model) {

    float dx{end.x - start.x};
    float dy{end.y - start.y};
    float length{(float)sqrt(dx * dx + dy * dy)};

    model.vertices.clear();

    // left side
    glm::vec3 vec0(0.0f, 0.0f, -tracks.trackWidth / 2);
    glm::vec3 vec1(0.0f, 0.0f, -tracks.trackWidth / 2 + tracks.markingWidth);
    glm::vec3 vec2(length, 0.0f, -tracks.trackWidth / 2 + tracks.markingWidth);
    glm::vec3 vec3(length, 0.0f, -tracks.trackWidth / 2);

    appendQuad(model.vertices, vec0, vec1, vec2, vec3);

    // right side
    vec0 = {0.0f, 0.0f, tracks.trackWidth / 2 - tracks.markingWidth};
    vec1 = {0.0f, 0.0f, tracks.trackWidth / 2};
    vec2 = {length, 0.0f, tracks.trackWidth / 2};
    vec3 = {length, 0.0f, tracks.trackWidth / 2 - tracks.markingWidth};

    appendQuad(model.vertices, vec0, vec1, vec2, vec3);

    // center line
    switch (centerLine) {
        case LaneMarking::Dashed: {
            float x{0.0f};
            while (x < length) {
                float xEnd{x + tracks.centerLineLength};
                if (length - x < tracks.centerLineLength) {
                    xEnd = length;
                }

                vec0 = {x, 0.0f, -tracks.markingWidth / 2};
                vec1 = {x, 0.0f, tracks.markingWidth / 2};
                vec2 = {xEnd, 0.0f, tracks.markingWidth / 2};
                vec3 = {xEnd, 0.0f, -tracks.markingWidth / 2};

                appendQuad(model.vertices, vec0, vec1, vec2, vec3);

                x += tracks.centerLineLength + tracks.centerLineInterrupt;
            }

            break;
        }
        case LaneMarking::DoubleSolid: {
            vec0 = {0.0f, 0.0f, -tracks.centerLineGap / 2 - tracks.markingWidth};
            vec1 = {0.0f, 0.0f, -tracks.centerLineGap / 2};
            vec2 = {length, 0.0f, -tracks.centerLineGap / 2};
            vec3 = {length, 0.0f, -tracks.centerLineGap / 2 - tracks.markingWidth};

            appendQuad(model.vertices, vec0, vec1, vec2, vec3);

            vec0 = {0.0f, 0.0f, tracks.centerLineGap / 2};
            vec1 = {0.0f, 0.0f, tracks.centerLineGap / 2 + tracks.markingWidth};
            vec2 = {length, 0.0f, tracks.centerLineGap / 2 + tracks.markingWidth};
            vec3 = {length, 0.0f, tracks.centerLineGap / 2};

            appendQuad(model.vertices, vec0, vec1, vec2, vec3);

            break;
        }
    }
}

void Editor::genTrackArcVertices(const glm::vec2& start, const glm::vec2& end,
        const glm::vec2& center, const float radius, const bool rightArc,
        const LaneMarking centerLine, const Tracks& tracks, Model& model) {

    float baseAngle{0.0f};
    float angle{0.0f};
    int numQuads{0};
    getArcVertexParams(start, end, center, rightArc, baseAngle, angle, numQuads);

    // update model vertices
    model.vertices.clear();

    float rOuterOuter = radius + tracks.trackWidth / 2;
    float rOuterInner = radius + tracks.trackWidth / 2 - tracks.markingWidth;
    float rCenterOuter = radius + tracks.markingWidth / 2;
    float rCenterInner = radius - tracks.markingWidth / 2;
    float rInnerOuter = radius - tracks.trackWidth / 2 + tracks.markingWidth;
    float rInnerInner = radius - tracks.trackWidth / 2;

    float offset{0.0f};
    float quadLength{(angle / (float)numQuads) * radius};

    for (int i = 0; i < numQuads; i++) {
        float angle1 = baseAngle + ((float)i * angle) / (float)numQuads;
        float angle2 = baseAngle + ((float)(i + 1) * angle) / (float)numQuads;

        // left side
        glm::vec3 vec0(cos(angle1) * rOuterOuter, 0.0f, sin(angle1) * rOuterOuter);
        glm::vec3 vec1(cos(angle1) * rOuterInner, 0.0f, sin(angle1) * rOuterInner);
        glm::vec3 vec2(cos(angle2) * rOuterInner, 0.0f, sin(angle2) * rOuterInner);
        glm::vec3 vec3(cos(angle2) * rOuterOuter, 0.0f, sin(angle2) * rOuterOuter);

        appendQuad(model.vertices, vec0, vec1, vec2, vec3);

        // right side
        vec0 = glm::vec3{cos(angle1) * rInnerOuter, 0.0f, sin(angle1) * rInnerOuter};
        vec1 = glm::vec3{cos(angle1) * rInnerInner, 0.0f, sin(angle1) * rInnerInner};
        vec2 = glm::vec3{cos(angle2) * rInnerInner, 0.0f, sin(angle2) * rInnerInner};
        vec3 = glm::vec3{cos(angle2) * rInnerOuter, 0.0f, sin(angle2) * rInnerOuter};

        appendQuad(model.vertices, vec0, vec1, vec2, vec3);

        // center line
        glm::vec3 vecStartOuter(cos(angle1) * rCenterOuter, 0.0f, sin(angle1) * rCenterOuter);
        glm::vec3 vecStartInner(cos(angle1) * rCenterInner, 0.0f, sin(angle1) * rCenterInner);
        glm::vec3 vecEndInner(cos(angle2) * rCenterInner, 0.0f, sin(angle2) * rCenterInner);
        glm::vec3 vecEndOuter(cos(angle2) * rCenterOuter, 0.0f, sin(angle2) * rCenterOuter);

        switch (centerLine) {
            case LaneMarking::Dashed: {
                float markingStart{0.0f};
                while (markingStart < quadLength) {
                    float markingEnd{0.0f};

                    if (offset < tracks.centerLineLength) {
                        markingEnd = markingStart + tracks.centerLineLength - offset;
                        if (markingEnd > quadLength) {
                            markingEnd = quadLength;
                            offset += markingEnd - markingStart;
                        } else {
                            offset = tracks.centerLineLength;
                        }

                        // add quad from markingStart to markingEnd
                        float tStart{markingStart / quadLength};
                        float tEnd{markingEnd / quadLength};
                        vec0 = (1.0f - tStart) * vecStartOuter + tStart * vecEndOuter;
                        vec1 = (1.0f - tStart) * vecStartInner + tStart * vecEndInner;
                        vec2 = (1.0f - tEnd) * vecStartInner + tEnd * vecEndInner;
                        vec3 = (1.0f - tEnd) * vecStartOuter + tEnd * vecEndOuter;
                        appendQuad(model.vertices, vec0, vec1, vec2, vec3);
                    } else {
                        markingEnd = markingStart + tracks.centerLineLength
                                + tracks.centerLineInterrupt - offset;
                        if (markingEnd > quadLength) {
                            markingEnd = quadLength;
                            offset += markingEnd - markingStart;
                        } else {
                            offset = 0.0f;
                        }
                    }

                    markingStart = markingEnd;
                }

                break;
            }
            case LaneMarking::DoubleSolid: {
                float rCenter1Outer = radius + tracks.centerLineGap / 2 + tracks.markingWidth;
                float rCenter1Inner = radius + tracks.centerLineGap / 2;

                vec0 = {cos(angle1) * rCenter1Outer, 0.0f, sin(angle1) * rCenter1Outer};
                vec1 = {cos(angle1) * rCenter1Inner, 0.0f, sin(angle1) * rCenter1Inner};
                vec2 = {cos(angle2) * rCenter1Inner, 0.0f, sin(angle2) * rCenter1Inner};
                vec3 = {cos(angle2) * rCenter1Outer, 0.0f, sin(angle2) * rCenter1Outer};

                appendQuad(model.vertices, vec0, vec1, vec2, vec3);

                float rCenter2Outer = radius - tracks.centerLineGap / 2;
                float rCenter2Inner = radius - tracks.centerLineGap / 2 - tracks.markingWidth;

                vec0 = {cos(angle1) * rCenter2Outer, 0.0f, sin(angle1) * rCenter2Outer};
                vec1 = {cos(angle1) * rCenter2Inner, 0.0f, sin(angle1) * rCenter2Inner};
                vec2 = {cos(angle2) * rCenter2Inner, 0.0f, sin(angle2) * rCenter2Inner};
                vec3 = {cos(angle2) * rCenter2Outer, 0.0f, sin(angle2) * rCenter2Outer};

                appendQuad(model.vertices, vec0, vec1, vec2, vec3);

                break;
            }
        }
    }
}

void Editor::genTrackIntersectionVertices(const TrackIntersection& intersection,
        const Tracks& tracks, Model& model) {

    model.vertices.clear();

    glm::vec2 center = getDraggedPosition(intersection.center.lock());
    std::vector<std::weak_ptr<ControlPoint>> links = intersection.links;

    // Remove duplicate link resulting from dragging
    if (activeControlPoint && dragState.connectedPoint
            && Tracks::isConnected(activeControlPoint, intersection)
            && Tracks::isConnected(dragState.connectedPoint, intersection)) {

        links.erase(
                std::remove_if(links.begin(), links.end(),
                    [this](const std::weak_ptr<ControlPoint>& link) {
                        return link.lock() == activeControlPoint;
                    }),
                links.end());
    }

    // Sort links by their angle, since their order may change
    std::sort(links.begin(), links.end(),
            [this, center](const std::weak_ptr<ControlPoint>& link1, const std::weak_ptr<ControlPoint>& link2) {
                glm::vec2 c1 = getDraggedPosition(link1.lock()) - center;
                glm::vec2 c2 = getDraggedPosition(link2.lock()) - center;

                return std::atan2(c1.y, c1.x) < std::atan2(c2.y, c2.x);
            });

    // Left and right lane boundaries
    if (links.size() == 1) {
        glm::vec2 c1 = getDraggedPosition(links.front().lock()) - center;
        glm::vec2 dir1 = glm::normalize(c1);
        glm::vec2 dir1Left(dir1.y, -dir1.x);
        glm::vec2 inner = (tracks.trackWidth / 2.0f - tracks.markingWidth) * dir1Left;
        glm::vec2 outer = (tracks.trackWidth / 2.0f) * dir1Left;

        appendGroundQuad(model.vertices, -inner, -outer, c1 - outer, c1 - inner);
        appendGroundQuad(model.vertices, outer, inner, c1 + inner, c1 + outer);
    } else {
        for (unsigned int i = 0; i < links.size(); i++) {
            glm::vec2 c1 = getDraggedPosition(links[i].lock()) - center;
            glm::vec2 c2 = getDraggedPosition(links[(i + 1) % links.size()].lock()) - center;

            float a1 = std::atan2(c1.y, c1.x);
            float a2 = std::atan2(c2.y, c2.x);

            if (a1 < a2) {
                a1 += 2.0f * M_PI;
            }
            float a = (a1 + a2) / 2.0f;
            if (a > M_PI) {
                a -= 2.0f * M_PI;
            }

            glm::vec2 centerDir = glm::vec2(cosf(a), sinf(a));

            glm::vec2 dir1 = glm::normalize(c1);
            glm::vec2 dir2 = glm::normalize(c2);
            glm::vec2 dir1Right(-dir1.y, dir1.x);
            glm::vec2 dir2Left(dir2.y, -dir2.x);

            glm::vec2 c1Inner = c1 + (tracks.trackWidth / 2.0f - tracks.markingWidth) * dir1Right;
            glm::vec2 c1Outer = c1 + (tracks.trackWidth / 2.0f) * dir1Right;
            glm::vec2 c2Inner = c2 + (tracks.trackWidth / 2.0f - tracks.markingWidth) * dir2Left;
            glm::vec2 c2Outer = c2 + (tracks.trackWidth / 2.0f) * dir2Left;

            float t{0};
            bool valid = intersectParam(glm::vec2(0.0f), centerDir, c1Inner, dir1, t);
            if (!valid) {
                // This should never happen
                continue;
            }
            glm::vec2 mInner = t * centerDir;

            t = 0;
            valid = intersectParam(glm::vec2(0.0f), centerDir, c1Outer, dir1, t);
            if (!valid) {
                // This should never happen
                continue;
            }
            glm::vec2 mOuter = t * centerDir;

            appendGroundQuad(model.vertices, mInner, mOuter, c1Outer, c1Inner);
            appendGroundQuad(model.vertices, c2Inner, c2Outer, mOuter, mInner);
        }
    }

    // Center lines
    for (const std::weak_ptr<ControlPoint>& link : links) {
        glm::vec2 c1 = getDraggedPosition(link.lock()) - center;

        float xEnd = glm::length(c1);
        glm::vec2 dir = glm::normalize(c1);
        glm::vec2 left = (tracks.markingWidth / 2.0f) * glm::vec2(dir.y, -dir.x);

        for (float x1 = tracks.trackWidth / 2.0f - tracks.markingWidth; x1 < xEnd;
                x1 += tracks.centerLineLength + tracks.centerLineInterrupt) {

            float x2 = std::min(x1 + tracks.centerLineLength, xEnd);

            appendGroundQuad(model.vertices,
                    x1 * dir + left,
                    x1 * dir - left,
                    x2 * dir - left,
                    x2 * dir + left);
        }
    }
}

void Editor::genTrackLineMarkerVertices(Model& model) {

    model.vertices = trackLineVertices;
}

void Editor::genTrackArcMarkerVertices(const glm::vec2& start, const glm::vec2& end,
        const glm::vec2& center, const float radius, const bool rightArc,
        const Tracks& tracks, Model& model) {

    float baseAngle{0.0f};
    float angle{0.0f};
    int numQuads{0};
    getArcVertexParams(start, end, center, rightArc, baseAngle, angle, numQuads);

    // update model vertices
    model.vertices.clear();

    float outerRadius = radius + tracks.trackWidth / 2;
    float innerRadius = radius - tracks.trackWidth / 2;

    for (int i = 0; i < numQuads; i++) {
        float angle1 = baseAngle + ((float)(i) * angle) / (float)numQuads;
        float angle2 = baseAngle + ((float)(i + 1) * angle) / (float)numQuads;

        glm::vec3 vec0(cosf(angle1) * outerRadius, 0.0f, sinf(angle1) * outerRadius);
        glm::vec3 vec1(cosf(angle1) * innerRadius, 0.0f, sinf(angle1) * innerRadius);
        glm::vec3 vec2(cosf(angle2) * innerRadius, 0.0f, sinf(angle2) * innerRadius);
        glm::vec3 vec3(cosf(angle2) * outerRadius, 0.0f, sinf(angle2) * outerRadius);

        appendQuad(model.vertices, vec0, vec1, vec2, vec3);
    }
}

void Editor::genTrackIntersectionMarkerVertices(const Tracks& tracks, Model& model) {

    model.vertices.clear();

    float d1 = tracks.trackWidth / 2 + intersectionTrackLength;
    float d2 = tracks.trackWidth / 2;

    glm::vec3 vec0(-d1, 0.0f, -d2);
    glm::vec3 vec1(-d1, 0.0f, d2);
    glm::vec3 vec2(d1, 0.0f, d2);
    glm::vec3 vec3(d1, 0.0f, -d2);

    appendQuad(model.vertices, vec0, vec1, vec2, vec3);

    vec0 = {-d2, 0.0f, -d1};
    vec1 = {-d2, 0.0f, d1};
    vec2 = {d2, 0.0f, d1};
    vec3 = {d2, 0.0f, -d1};

    appendQuad(model.vertices, vec0, vec1, vec2, vec3);
}

glm::mat4 Editor::genPointMatrix(const glm::vec2& point, const float y) {

    return glm::translate(glm::mat4(1.0f), glm::vec3(point.x, y, point.y));
}

glm::mat4 Editor::genTrackLineMatrix(const glm::vec2& start, const glm::vec2& end, const float y) {

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(start.x, y, start.y));
    float dx(end.x - start.x);
    float dy(end.y - start.y);
    modelMat = glm::rotate(modelMat, atan2f(-dy, dx), glm::vec3(0.0f, 1.0f, 0.0f));

    return modelMat;
}

glm::mat4 Editor::genTrackArcMatrix(const glm::vec2& center, const float y) {

    return glm::translate(glm::mat4(1.0f), glm::vec3(center.x, y, center.y));
}

glm::mat4 Editor::genTrackIntersectionMatrix(const glm::vec2& center, const float y) {

    return glm::translate(glm::mat4(1.0f), glm::vec3(center.x, y, center.y));
}

glm::mat4 Editor::genTrackLineMarkerMatrix(const glm::vec2& start,
        const glm::vec2& end, const float y, const Tracks& tracks) {

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3((start.x + end.x) / 2, y, (start.y + end.y) / 2));
    float dx(end.x - start.x);
    float dy(end.y - start.y);
    modelMat = glm::rotate(modelMat, atan2f(-dy, dx), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(sqrt(dx * dx + dy * dy) / 2, 1.0f, tracks.trackWidth / 2)); // division by 2 because model has size 2x2

    return modelMat;
}

void Editor::appendGroundQuad(std::vector<Model::Vertex>& vertices, const glm::vec2& vec0,
        const glm::vec2& vec1, const glm::vec2& vec2, const glm::vec2& vec3) {
    
    appendQuad(vertices,
            glm::vec3(vec0.x, 0.0f, vec0.y),
            glm::vec3(vec1.x, 0.0f, vec1.y),
            glm::vec3(vec2.x, 0.0f, vec2.y),
            glm::vec3(vec3.x, 0.0f, vec3.y));
}

void Editor::getArcVertexParams(const glm::vec2& start, const glm::vec2& end,
        const glm::vec2& center, const bool rightArc, float& baseAngle,
        float& angle, int& numQuads) {

    // angles
    float angleStart = std::atan2(start.y - center.y, start.x - center.x);
    float angleEnd = std::atan2(end.y - center.y, end.x - center.x);

    if (rightArc) {
        // right arc
        baseAngle = angleStart;
        angle = angleEnd - angleStart;
    } else {
        // left arc
        baseAngle = angleEnd;
        angle = angleStart - angleEnd;
    }

    if (angle < 0) {
        angle += 2.0f * (float)M_PI;
    }

    // number of quads
    numQuads = (int)(50 * angle);
    if (numQuads < 1) {
        numQuads = 1;
    }
    if (numQuads > 64) {
        numQuads = 64;
    }
}


