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

    material.Ka = objl::Vector3(0.2f, 0.2f, 0.2f);
    material.Kd = objl::Vector3(0.6f, 0.6f, 0.6f);
    material.Ks = objl::Vector3(0.5f, 0.5f, 0.5f);
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

    objl::Vertex v0{objl::Vector3(-1.0f, 0.0f, -1.0f), objl::Vector3(0.0f, 1.0f, 0.0f), objl::Vector2(0.0f, 0.0f)};
    objl::Vertex v1{objl::Vector3(-1.0f, 0.0f, 1.0f), objl::Vector3(0.0f, 1.0f, 0.0f), objl::Vector2(0.0f, 0.0f)};
    objl::Vertex v2{objl::Vector3(1.0f, 0.0f, 1.0f), objl::Vector3(0.0f, 1.0f, 0.0f), objl::Vector2(0.0f, 0.0f)};
    objl::Vertex v3{objl::Vector3(1.0f, 0.0f, -1.0f), objl::Vector3(0.0f, 1.0f, 0.0f), objl::Vector2(0.0f, 0.0f)};

    return std::vector<objl::Vertex>{v0, v1, v2, v0, v2, v3};
}();

Editor::Editor(float groundSize) {

    // ground
    groundModelMat = glm::scale(groundModelMat, glm::vec3(groundSize, 1.0f, groundSize));
    ground.upload();

    // create track model
    genTrackLineVertices(*trackLineModel);
    genTrackMaterial(*trackLineModel);
    trackLineModel->upload();

    // create markers
    genPointVertices(defaultMarker);
    genDefaultMarkerMaterial(defaultMarker);
    defaultMarker.upload();

    genPointVertices(activeMarker);
    genActiveMarkerMaterial(activeMarker);
    activeMarker.upload();

    // track marker
    genTrackLineVertices(markerTrackLine);
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
    trackModels[track] = trackLineModel;

    // model mat
    glm::mat4 modelMat = genTrackLineMatrix(start->coords, end->coords, trackYOffset);
    trackModelMats[track] = modelMat;
}

void Editor::addTrackArc(const std::shared_ptr<ControlPoint>& start, const std::shared_ptr<ControlPoint>& end, const glm::vec2& center, const float radius, const bool rightArc, Scene::Tracks& tracks) {

    // add track
    std::shared_ptr<TrackArc> track = tracks.addTrackArc(start, end, center, radius, rightArc);

    // model
    std::shared_ptr<Model> model = std::make_shared<Model>();
    genTrackArcVertices(start->coords, end->coords, center, radius, rightArc, *model);
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

    glm::vec3 cameraPos = glm::vec3(glm::inverse(camera.view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

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
        updateTrackLineMarker(startPoint.coords, end);
    } else {
        updateTrackArcMarker(startPoint, end);
    }
}

void Editor::updateTrackLineMarker(const glm::vec2& start, const glm::vec2& end) {

    markerModelMatTrackLine = genTrackLineMatrix(start, end, markerYOffset);
}

void Editor::updateTrackArcMarker(const ControlPoint& startPoint, const glm::vec2& end) {

    glm::vec2 start = startPoint.coords;

    glm::vec2 center;
    float radius{0};
    bool rightArc{false};
    bool isArc = getArc(startPoint, end, center, radius, rightArc);

    if (!isArc) {
        // line

        // update model
        genTrackLineVertices(markerTrackArc);
        genActiveMarkerMaterial(markerTrackArc);
        markerTrackArc.upload();

        // update model matrix
        markerModelMatTrackArc = genTrackLineMatrix(start, end, markerYOffset);
    } else {
        // arc

        // update model
        genTrackArcVertices(start, end, center, radius, rightArc, markerTrackArc);
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

void Editor::genTrackLineVertices(Model& model) {

    model.vertices = trackLineVertices;
}

void Editor::genTrackArcVertices(const glm::vec2& start, const glm::vec2& end, const glm::vec2& center, const float radius, const bool rightArc, Model& model) {

    // calculate necessary angles
    float angleStart = atan2(start.y - center.y, start.x - center.x);
    float angleEnd = atan2(end.y - center.y, end.x - center.x);
    float baseAngle{0};
    float angle{0};
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

    // update model vertices
    model.vertices.clear();

    objl::Vector3 normalVector(0.0f, 1.0f, 0.0f);
    objl::Vector2 texCoords(0.0f, 0.0f);

    int n = 50 * angle;
    if (n < 1) {
        n = 1;
    }
    if (n > 64) {
        n = 64;
    }

    float outerRadius = radius + TrackBase::trackWidth / 2;
    float innerRadius = radius - TrackBase::trackWidth / 2;

    for (int i = 0; i < n; i++) {
        float angle1 = baseAngle + (i * angle) / n;
        float angle2 = baseAngle + ((i + 1) * angle) / n;

        objl::Vector3 coords(cos(angle1) * outerRadius, 0.0f, sin(angle1) * outerRadius);
        objl::Vertex v0{coords, normalVector, texCoords};
        coords = objl::Vector3(cos(angle1) * innerRadius, 0.0f, sin(angle1) * innerRadius);
        objl::Vertex v1{coords, normalVector, texCoords};
        coords = objl::Vector3(cos(angle2) * innerRadius, 0.0f, sin(angle2) * innerRadius);
        objl::Vertex v2{coords, normalVector, texCoords};
        coords = objl::Vector3(cos(angle2) * outerRadius, 0.0f, sin(angle2) * outerRadius);
        objl::Vertex v3{coords, normalVector, texCoords};

        model.vertices.push_back(v0);
        model.vertices.push_back(v1);
        model.vertices.push_back(v2);
        model.vertices.push_back(v0);
        model.vertices.push_back(v2);
        model.vertices.push_back(v3);
    }
}

glm::mat4 Editor::genPointMatrix(const glm::vec2& point, const float y) {

    return glm::translate(glm::mat4(1.0f), glm::vec3(point.x, y, point.y));
}

glm::mat4 Editor::genTrackLineMatrix(const glm::vec2& start, const glm::vec2& end, const float y) {

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3((start.x + end.x) / 2, y, (start.y + end.y) / 2));
    float dx(end.x - start.x);
    float dy(end.y - start.y);
    modelMat = glm::rotate(modelMat, atan2(-dy, dx), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(sqrt(dx * dx + dy * dy) / 2, 1.0f, TrackBase::trackWidth / 2)); // division by 2 because model has size 2x2

    return modelMat;
}

glm::mat4 Editor::genTrackArcMatrix(const glm::vec2& center, const float y) {

    return glm::translate(glm::mat4(1.0f), glm::vec3(center.x, y, center.y));
}






