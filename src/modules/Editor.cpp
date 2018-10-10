#include "Editor.h"

objl::Material Editor::markerDefaultMaterial = []{

    objl::Material material;

    material.Ka = objl::Vector3(1.0f, 1.0f, 1.0f);
    material.Kd = objl::Vector3(1.0f, 1.0f, 1.0f);
    material.Ks = objl::Vector3(0.0f, 0.0f, 0.0f);
    material.Ns = 10.0f;

    return material;
}();

objl::Material Editor::markerActiveMaterial = []{

    objl::Material material;

    material.Ka = objl::Vector3(1.0f, 1.0f, 1.0f);
    material.Kd = objl::Vector3(0.0f, 1.0f, 0.0f);
    material.Ks = objl::Vector3(0.0f, 0.0f, 0.0f);
    material.Ns = 10.0f;

    return material;
}();

objl::Material Editor::trackMaterial = []{

    objl::Material material;

    material.Ka = objl::Vector3(1.0f, 1.0f, 1.0f);
    material.Kd = objl::Vector3(1.0f, 1.0f, 1.0f);
    material.Ks = objl::Vector3(0.0f, 0.0f, 0.0f);
    material.Ns = 10.0f;

    return material;
}();

std::vector<objl::Vertex> Editor::pointVertices = []{

    std::vector<objl::Vertex> vertices;

    objl::Vector3 normal(0.0f, 1.0f, 0.0f);
    objl::Vector2 texCoords(0.0f, 0.0f);
    objl::Vector3 centerCoords(0.0f, 0.0f, 0.0f);
    objl::Vertex center{centerCoords, normal, texCoords};

    const int n = 32; // number of sides
    const float radius = 0.1f;

    for (int i = 0; i < n; i++) {
        float angle1 = (2 * M_PI * i) / n;
        float angle2 = (2 * M_PI * (i + 1)) / n;

        objl::Vector3 coords(cos(angle1) * radius, 0.0f, sin(angle1) * radius);
        objl::Vertex v1{coords, normal, texCoords};
        coords = objl::Vector3(cos(angle2) * radius, 0.0f, sin(angle2) * radius);
        objl::Vertex v2{coords, normal, texCoords};

        vertices.push_back(center);
        vertices.push_back(v2);
        vertices.push_back(v1);
    }

    return vertices;
}();

std::vector<objl::Vertex> Editor::trackLineVertices = []{

    std::vector<objl::Vertex> vertices;

    objl::Vector3 vec0(-1.0f, 0.0f, -1.0f);
    objl::Vector3 vec1(-1.0f, 0.0f, 1.0f);
    objl::Vector3 vec2(1.0f, 0.0f, 1.0f);
    objl::Vector3 vec3(1.0f, 0.0f, -1.0f);

    appendQuad(vertices, vec0, vec1, vec2, vec3);

    return vertices;
}();

Editor::Editor(float groundSize) {

    // ground
    groundModelMat = glm::scale(groundModelMat, glm::vec3(groundSize, 1.0f, groundSize));
    ground.upload();

    // create markers
    genPointVertices(defaultMarker);
    genDefaultMarkerMaterial(defaultMarker);
    defaultMarker.upload();

    genPointVertices(activeMarker);
    genActiveMarkerMaterial(activeMarker);
    activeMarker.upload();

    // track marker
    genTrackLineMarkerVertices(markerTrackLine);
    genActiveMarkerMaterial(markerTrackLine);
    markerTrackLine.upload();
}

void Editor::updateInput(Camera& camera, Scene::Tracks& tracks, float groundSize) {

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

void Editor::onKey(int key, int action, const Scene::Tracks& tracks) {

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
        setTrackMode(Editor::TrackMode::Line, tracks);
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        setTrackMode(Editor::TrackMode::Arc, tracks);
    }

    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS
            || key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
        if (activeControlPoint != nullptr) {
            ((Scene::Tracks&)tracks).removeControlPoint(activeControlPoint);
            activeControlPoint = nullptr;
        }
    }
}

void Editor::onButton(double cursorX, double cursorY, int windowWidth, int windowHeight,
        int button, int action, Camera& camera, Scene::Tracks& tracks, float groundSize) {

    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS) {
                // calculate clicked location
                glm::vec2 groundCoords;
                bool positionValid = toGroundCoordinates(cursorX, cursorY, windowWidth, windowHeight, camera, groundCoords);
                if (!positionValid) {
                    return;
                }

                // handle click
                if (!activeControlPoint) {
                    startTrack(groundCoords, tracks, groundSize);
                } else {
                    std::shared_ptr<ControlPoint> selectedPoint = selectControlPoint(groundCoords, tracks);
                    if (selectedPoint == activeControlPoint) {
                        activeControlPoint = nullptr;
                    } else {
                        endTrack(groundCoords, tracks, groundSize);
                    }
                }
            }

            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_PRESS) {
                // cancel track
                activeControlPoint = nullptr;
            }

            break;
    }
}

void Editor::onMouseMoved(double cursorX, double cursorY, int windowWidth, int windowHeight, Camera& camera, const Scene::Tracks& tracks) {

    if (!activeControlPoint) {
        return;
    }

    glm::vec2 groundCoords;
    bool positionValid = toGroundCoordinates(cursorX, cursorY, windowWidth, windowHeight, camera, groundCoords);
    if (!positionValid) {
        return;
    }

    cursorPos = groundCoords;

    updateMarkers(*activeControlPoint, tracks);
}

void Editor::setTrackMode(TrackMode trackMode, const Scene::Tracks& tracks) {

    this->trackMode = trackMode;

    if (activeControlPoint) {
        updateMarkers(*activeControlPoint, tracks);
    }
}

void Editor::setAutoAlign(bool autoAlign, const Scene::Tracks& tracks) {

    this->autoAlign = autoAlign;

    if (activeControlPoint) {
        updateMarkers(*activeControlPoint, tracks);
    }
}

void Editor::renderScene(GLuint shaderProgramId, const Scene::Tracks& tracks) {

    // render ground
    ground.render(shaderProgramId, groundModelMat);

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
                        tracks,
                        *model);
                genTrackMaterial(*model);
                model->upload();
                trackModels[track] = model;

                glm::mat4 modelMat = genTrackArcMatrix(arc->center, trackYOffset);
                trackModelMats[track] = modelMat;
            } else {
                std::shared_ptr<Model> model = std::make_shared<Model>();
                genTrackLineVertices(
                        track->start.lock()->coords,
                        track->end.lock()->coords,
                        tracks,
                        *model);
                genTrackMaterial(*model);
                model->upload();

                trackModels[track] = model;
                glm::mat4 modelMat = genTrackLineMatrix(
                        track->start.lock()->coords,
                        track->end.lock()->coords,
                        trackYOffset);
                trackModelMats[track] = modelMat;
            }
        }

        trackModels[track]->render(shaderProgramId, trackModelMats[track]);
    }
}

void Editor::renderMarkers(GLuint shaderProgramId, const Scene::Tracks& tracks) {

    // render control points
    for (const std::shared_ptr<ControlPoint>& cp : tracks.getTracks()) {
        renderMarker(shaderProgramId, cp->coords, cp == activeControlPoint);
    }

    if (activeControlPoint && !tracks.controlPointExists(activeControlPoint)) {
        renderMarker(shaderProgramId, activeControlPoint->coords, true);
    }

    // render new track markers
    if (activeControlPoint) {
        activeMarker.render(shaderProgramId, markerModelMatEnd);

        if (getEffectiveTrackMode() == TrackMode::Line) {
            markerTrackLine.render(shaderProgramId, markerModelMatTrackLine);
        } else {
            markerTrackArc.render(shaderProgramId, markerModelMatTrackArc);
        }
    }
}

void Editor::renderMarker(GLuint shaderProgramId, const glm::vec2& position, const bool active) {

    // create model matrix
    glm::mat4 modelMat = genPointMatrix(position, 0.0f);

    // render
    if (active) {
        activeMarker.render(shaderProgramId, modelMat);
    } else {
        defaultMarker.render(shaderProgramId, modelMat);
    }
}

void Editor::startTrack(const glm::vec2& position, const Scene::Tracks& tracks, float groundSize) {

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
    updateMarkers(*activeControlPoint, tracks);
}

void Editor::endTrack(const glm::vec2& position, Scene::Tracks& tracks, float groundSize) {

    // update temporary state
    glm::vec2 end = align(*activeControlPoint, position, tracks);

    if (end.x < -groundSize || end.x > groundSize || end.y < -groundSize || end.y > groundSize) {
        if (!isStartConnected()) {
            activeControlPoint = nullptr;
        }

        return;
    }

    // find or create end point
    std::shared_ptr<ControlPoint> trackEnd = selectControlPoint(position, tracks);

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

    updateMarkers(*activeControlPoint, tracks);
}

void Editor::addTrackLine(const std::shared_ptr<ControlPoint>& start,
        const std::shared_ptr<ControlPoint>& end, Scene::Tracks& tracks) {

    // add track
    std::shared_ptr<TrackLine> track = tracks.addTrackLine(start, end);

    // model
    std::shared_ptr<Model> model = std::make_shared<Model>();
    genTrackLineVertices(start->coords, end->coords, tracks, *model);
    genTrackMaterial(*model);
    model->upload();
    trackModels[track] = model;

    // model mat
    glm::mat4 modelMat = genTrackLineMatrix(start->coords, end->coords, trackYOffset);
    trackModelMats[track] = modelMat;
}

void Editor::addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc, Scene::Tracks& tracks) {

    // add track
    std::shared_ptr<TrackArc> track = tracks.addTrackArc(start, end, center, radius, rightArc);

    // model
    std::shared_ptr<Model> model = std::make_shared<Model>();
    genTrackArcVertices(start->coords, end->coords, center, radius, rightArc, tracks, *model);
    genTrackMaterial(*model);
    model->upload();
    trackModels[track] = model;

    // model mat
    glm::mat4 modelMat = genTrackArcMatrix(center, trackYOffset);
    trackModelMats[track] = modelMat;
}

std::shared_ptr<ControlPoint> Editor::selectControlPoint(const glm::vec2& position, const Scene::Tracks& tracks) const {

    std::shared_ptr<ControlPoint> controlPoint;
    float closest{controlPointClickRadius};

    // check scene control points
    for (std::shared_ptr<ControlPoint> const& cp : tracks.getTracks()) {
        float distance = glm::distance(cp->coords, position);
        if (distance < closest) {
            controlPoint = cp;

            closest = distance;
        }
    }

    // check active control point (can also be a scene control point)
    if (activeControlPoint) {
        float distance = glm::distance(activeControlPoint->coords, position);
        if (distance < closest) {
            return activeControlPoint;
        }
    }

    return controlPoint;
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

void Editor::updateMarkers(const ControlPoint& startPoint, const Scene::Tracks& tracks) {

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

void Editor::updateTrackLineMarker(const glm::vec2& start, const glm::vec2& end, const Scene::Tracks& tracks) {

    markerModelMatTrackLine = genTrackLineMarkerMatrix(start, end, markerYOffset, tracks);
}

void Editor::updateTrackArcMarker(const ControlPoint& startPoint,
        const glm::vec2& end, const Scene::Tracks& tracks) {

    glm::vec2 start = startPoint.coords;

    glm::vec2 center;
    float radius{0};
    bool rightArc{false};
    bool isArc = getArc(startPoint, end, center, radius, rightArc);

    if (!isArc) {
        // line

        // update model
        genTrackLineMarkerVertices(markerTrackArc);
        genActiveMarkerMaterial(markerTrackArc);
        markerTrackArc.upload();

        // update model matrix
        markerModelMatTrackArc = genTrackLineMarkerMatrix(start, end, markerYOffset, tracks);
    } else {
        // arc

        // update model
        genTrackArcMarkerVertices(start, end, center, radius, rightArc, tracks, markerTrackArc);
        genActiveMarkerMaterial(markerTrackArc);
        markerTrackArc.upload();

        // update model matrix
        markerModelMatTrackArc = genTrackArcMatrix(center, markerYOffset);
    }
}

bool Editor::getArc(const ControlPoint& startPoint, const glm::vec2& end, glm::vec2& center, float& radius, bool& rightArc) {

    glm::vec2 start = startPoint.coords;

    glm::vec2 direction = getAlignedUnitVector(startPoint, end);
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
        radius = abs(t);
        rightArc = (t > 0);

        return true;
    }
}

bool Editor::intersectParam(const glm::vec2& p1, const glm::vec2& r1, const glm::vec2& p2, const glm::vec2& r2, float& t1) {

    // validate
    if ((r1.x == 0 && r1.y == 0) || (r2.x == 0 && r2.y == 0)) {
        return false;
    }

    // intersect
    float m(0);
    float x(0);
    if (abs(r2.x) > abs(r2.y)) {
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

glm::vec2 Editor::align(const ControlPoint& startPoint, const glm::vec2& position, const Scene::Tracks& tracks) {

    if (autoAlign) {
        glm::vec2 exactVector = position - startPoint.coords;
        glm::vec2 alignedUnitVector = getAlignedUnitVector(startPoint, position);

        if (getEffectiveTrackMode() == TrackMode::Line) {

            glm::vec2 alignedVector = glm::dot(alignedUnitVector, exactVector) * alignedUnitVector;

            return startPoint.coords + alignedVector;
        } else {
            return position;
        }
    } else {
        std::shared_ptr<ControlPoint> connectedEnd = selectControlPoint(position, tracks);
        if (connectedEnd && connectedEnd.get() != &startPoint) {
            return connectedEnd->coords;
        } else {
            return position;
        }
    }
}

glm::vec2 Editor::getAlignedUnitVector(const ControlPoint& startPoint, const glm::vec2& endPoint) {

    // calculate directions for alignment
    std::vector<glm::vec2> directions;

    for (const std::shared_ptr<TrackBase>& track : startPoint.tracks) {
        glm::vec2 dir = track->getDirection(startPoint);

        directions.push_back(glm::normalize(dir));
    }

    // use default directions if no tracks are connected
    if (directions.empty()) {
        for (int i = 0; i < 8; i++) {
            float angle = i * M_PI * 0.25f;
            directions.push_back(glm::vec2(cos(angle), sin(angle)));
        }
    }

    // calculate alignment
    glm::vec2 exactVector = endPoint - startPoint.coords;
    glm::vec2 exactUnitVector = glm::normalize(exactVector);

    glm::vec2 alignedUnitVector;
    float maxDot = -2.0f; // less than -1.0f
    for (glm::vec2 const& dir : directions) {
        float dot = glm::dot(exactUnitVector, dir);
        if (dot > maxDot) {
            alignedUnitVector = dir;
            maxDot = dot;
        }
    }

    return alignedUnitVector;
}

Editor::TrackMode Editor::getEffectiveTrackMode() {
    if (isStartConnected()) {
        return trackMode;
    } else {
        return TrackMode::Line;
    }
}

bool Editor::isStartConnected() {

    return activeControlPoint && !activeControlPoint->tracks.empty();
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

void Editor::genTrackLineVertices(const glm::vec2& start, const glm::vec2& end, const Scene::Tracks& tracks, Model& model) {

    float dx{end.x - start.x};
    float dy{end.y - start.y};
    float length{sqrt(dx * dx + dy * dy)};

    model.vertices.clear();

    // left side
    objl::Vector3 vec0(0.0f, 0.0f, -tracks.trackWidth / 2);
    objl::Vector3 vec1(0.0f, 0.0f, -tracks.trackWidth / 2 + tracks.markingWidth);
    objl::Vector3 vec2(length, 0.0f, -tracks.trackWidth / 2 + tracks.markingWidth);
    objl::Vector3 vec3(length, 0.0f, -tracks.trackWidth / 2);

    appendQuad(model.vertices, vec0, vec1, vec2, vec3);

    // right side
    vec0 = objl::Vector3(0.0f, 0.0f, tracks.trackWidth / 2 - tracks.markingWidth);
    vec1 = objl::Vector3(0.0f, 0.0f, tracks.trackWidth / 2);
    vec2 = objl::Vector3(length, 0.0f, tracks.trackWidth / 2);
    vec3 = objl::Vector3(length, 0.0f, tracks.trackWidth / 2 - tracks.markingWidth);

    appendQuad(model.vertices, vec0, vec1, vec2, vec3);

    // center line
    float x{0.0f};
    while (x < length) {
        float xEnd{x + tracks.centerLineLength};
        if (length - x < tracks.centerLineLength) {
            xEnd = length;
        }

        vec0 = objl::Vector3(x, 0.0f, -tracks.markingWidth / 2);
        vec1 = objl::Vector3(x, 0.0f, tracks.markingWidth / 2);
        vec2 = objl::Vector3(xEnd, 0.0f, tracks.markingWidth / 2);
        vec3 = objl::Vector3(xEnd, 0.0f, -tracks.markingWidth / 2);

        appendQuad(model.vertices, vec0, vec1, vec2, vec3);

        x += tracks.centerLineLength + tracks.centerLineInterrupt;
    }
}

void Editor::genTrackArcVertices(const glm::vec2& start, const glm::vec2& end, const glm::vec2& center, const float radius, const bool rightArc, const Scene::Tracks& tracks, Model& model) {

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
    float quadLength{(angle / numQuads) * radius};

    for (int i = 0; i < numQuads; i++) {
        float angle1 = baseAngle + (i * angle) / numQuads;
        float angle2 = baseAngle + ((i + 1) * angle) / numQuads;

        // left side
        objl::Vector3 vec0(cos(angle1) * rOuterOuter, 0.0f, sin(angle1) * rOuterOuter);
        objl::Vector3 vec1(cos(angle1) * rOuterInner, 0.0f, sin(angle1) * rOuterInner);
        objl::Vector3 vec2(cos(angle2) * rOuterInner, 0.0f, sin(angle2) * rOuterInner);
        objl::Vector3 vec3(cos(angle2) * rOuterOuter, 0.0f, sin(angle2) * rOuterOuter);

        appendQuad(model.vertices, vec0, vec1, vec2, vec3);

        // right side
        vec0 = objl::Vector3(cos(angle1) * rInnerOuter, 0.0f, sin(angle1) * rInnerOuter);
        vec1 = objl::Vector3(cos(angle1) * rInnerInner, 0.0f, sin(angle1) * rInnerInner);
        vec2 = objl::Vector3(cos(angle2) * rInnerInner, 0.0f, sin(angle2) * rInnerInner);
        vec3 = objl::Vector3(cos(angle2) * rInnerOuter, 0.0f, sin(angle2) * rInnerOuter);

        appendQuad(model.vertices, vec0, vec1, vec2, vec3);

        // center line
        objl::Vector3 vecStartOuter(cos(angle1) * rCenterOuter, 0.0f, sin(angle1) * rCenterOuter);
        objl::Vector3 vecStartInner(cos(angle1) * rCenterInner, 0.0f, sin(angle1) * rCenterInner);
        objl::Vector3 vecEndInner(cos(angle2) * rCenterInner, 0.0f, sin(angle2) * rCenterInner);
        objl::Vector3 vecEndOuter(cos(angle2) * rCenterOuter, 0.0f, sin(angle2) * rCenterOuter);

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
                using namespace objl::algorithm;
                vec0 = (1 - tStart) * vecStartOuter + tStart * vecEndOuter;
                vec1 = (1 - tStart) * vecStartInner + tStart * vecEndInner;
                vec2 = (1 - tEnd) * vecStartInner + tEnd * vecEndInner;
                vec3 = (1 - tEnd) * vecStartOuter + tEnd * vecEndOuter;
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
    }
}

void Editor::genTrackLineMarkerVertices(Model& model) {

    model.vertices = trackLineVertices;
}

void Editor::genTrackArcMarkerVertices(const glm::vec2& start, const glm::vec2& end,
        const glm::vec2& center, const float radius, const bool rightArc,
        const Scene::Tracks& tracks, Model& model) {

    float baseAngle{0.0f};
    float angle{0.0f};
    int numQuads{0};
    getArcVertexParams(start, end, center, rightArc, baseAngle, angle, numQuads);

    // update model vertices
    model.vertices.clear();

    float outerRadius = radius + tracks.trackWidth / 2;
    float innerRadius = radius - tracks.trackWidth / 2;

    for (int i = 0; i < numQuads; i++) {
        float angle1 = baseAngle + (i * angle) / numQuads;
        float angle2 = baseAngle + ((i + 1) * angle) / numQuads;

        objl::Vector3 vec0(cos(angle1) * outerRadius, 0.0f, sin(angle1) * outerRadius);
        objl::Vector3 vec1(cos(angle1) * innerRadius, 0.0f, sin(angle1) * innerRadius);
        objl::Vector3 vec2(cos(angle2) * innerRadius, 0.0f, sin(angle2) * innerRadius);
        objl::Vector3 vec3(cos(angle2) * outerRadius, 0.0f, sin(angle2) * outerRadius);

        appendQuad(model.vertices, vec0, vec1, vec2, vec3);
    }
}

glm::mat4 Editor::genPointMatrix(const glm::vec2& point, const float y) {

    return glm::translate(glm::mat4(1.0f), glm::vec3(point.x, y, point.y));
}

glm::mat4 Editor::genTrackLineMatrix(const glm::vec2& start, const glm::vec2& end, const float y) {

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(start.x, y, start.y));
    float dx(end.x - start.x);
    float dy(end.y - start.y);
    modelMat = glm::rotate(modelMat, atan2(-dy, dx), glm::vec3(0.0f, 1.0f, 0.0f));

    return modelMat;
}

glm::mat4 Editor::genTrackArcMatrix(const glm::vec2& center, const float y) {

    return glm::translate(glm::mat4(1.0f), glm::vec3(center.x, y, center.y));
}

glm::mat4 Editor::genTrackLineMarkerMatrix(const glm::vec2& start,
        const glm::vec2& end, const float y, const Scene::Tracks& tracks) {

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3((start.x + end.x) / 2, y, (start.y + end.y) / 2));
    float dx(end.x - start.x);
    float dy(end.y - start.y);
    modelMat = glm::rotate(modelMat, atan2(-dy, dx), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(sqrt(dx * dx + dy * dy) / 2, 1.0f, tracks.trackWidth / 2)); // division by 2 because model has size 2x2

    return modelMat;
}

void Editor::appendQuad(std::vector<objl::Vertex>& vertices, const objl::Vector3& vec0,
        const objl::Vector3& vec1, const objl::Vector3& vec2, const objl::Vector3& vec3) {

    objl::Vector3 normalVector(0.0f, 1.0f, 0.0f);
    objl::Vector2 texCoords(0.0f, 0.0f);

    objl::Vertex v0{vec0, normalVector, texCoords};
    objl::Vertex v1{vec1, normalVector, texCoords};
    objl::Vertex v2{vec2, normalVector, texCoords};
    objl::Vertex v3{vec3, normalVector, texCoords};

    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v0);
    vertices.push_back(v2);
    vertices.push_back(v3);
}

void Editor::getArcVertexParams(const glm::vec2& start, const glm::vec2& end,
        const glm::vec2& center, const bool rightArc, float& baseAngle,
        float& angle, int& numQuads) {

    // angles
    float angleStart = atan2(start.y - center.y, start.x - center.x);
    float angleEnd = atan2(end.y - center.y, end.x - center.x);

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
        angle += 2 * M_PI;
    }

    // number of quads
    numQuads = 50 * angle;
    if (numQuads < 1) {
        numQuads = 1;
    }
    if (numQuads > 64) {
        numQuads = 64;
    }
}





