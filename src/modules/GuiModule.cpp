#include "GuiModule.h"

GuiModule::GuiModule(GLFWwindow* window, std::string scenePath) {

    this->window = window;

    int separatorIndex = scenePath.find_last_of("\\/");

    if (separatorIndex > 0) { 
        openedPath = scenePath.substr(0, separatorIndex);
        openedFilename = scenePath.substr(
                separatorIndex+1, scenePath.size());
    } else {
        openedPath = "./";
        openedFilename = scenePath;
    }

    currentDirectory = openedPath;
    selectedFilename = openedFilename;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::StyleColorsDark();
}

GuiModule::~GuiModule() {

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiModule::renderRootWindow(Scene& scene) {

    bool showOpenFileDialog = false;
    bool showSaveFileDialog = false;
    bool showSaveAsFileDialog = false;

    ImGui::Begin("", NULL, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                scene = Scene();
                openedPath = "";
                openedFilename = "";
            }
            if (ImGui::MenuItem("Open")) {
                showOpenFileDialog = true;
            }
            if (ImGui::MenuItem("Save")) {
                showSaveFileDialog = true;
            }
            if (ImGui::MenuItem("Save as")) {
                showSaveAsFileDialog = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Create")) {
            renderCreateMenu(scene);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Show")) {

            ImGui::MenuItem("Car Properties", NULL, &showCarPropertiesWindow);
            ImGui::MenuItem("Pose", NULL, &showPoseWindow);
            ImGui::MenuItem("Help", NULL, &showHelpWindow);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    renderOpenFileDialog(scene, showOpenFileDialog);
    renderSaveFileDialog(scene, showSaveFileDialog, showSaveAsFileDialog);

    // rendering the status text

    ImGui::Text("Carolo Simulator v0.3");

    std::string msg = "Config: ";
    if (openedFilename.empty()) { 
        msg += "none";
    } else {
        msg += openedFilename;
    }
    ImGui::Text("%s", msg.c_str());

    ImGui::Text("Simulation time: %.2f seconds",
            ((double)scene.simulationTime) / 1000.0);

    if (scene.paused) {
        ImGui::Text("PAUSED");
    }

    ImGui::End();

    //ImGui::ShowDemoWindow(NULL);
}

void GuiModule::renderCreateMenu(Scene& scene) {

    ItemType newType = NONE;

    if (ImGui::MenuItem("Obstacle 10x10cm")) {
        newType = OBSTACLE;
    }
    if (ImGui::MenuItem("Start line")) {
        newType = START_LINE;
    }
    if (ImGui::MenuItem("Stop line")) {
        newType = STOP_LINE;
    }
    if (ImGui::MenuItem("Give-way line")) {
        newType = GIVE_WAY_LINE;
    }
    if (ImGui::MenuItem("Crosswalk")) {
        newType = CROSSWALK;
    }
    if (ImGui::MenuItem("Speed limit 10")) {
        newType = GROUND_10;
    }
    if (ImGui::MenuItem("Speed limit 20")) {
        newType = GROUND_20;
    }
    if (ImGui::MenuItem("Speed limit 30")) {
        newType = GROUND_30;
    }
    if (ImGui::MenuItem("Speed limit 40")) {
        newType = GROUND_40;
    }
    if (ImGui::MenuItem("Speed limit 50")) {
        newType = GROUND_50;
    }
    if (ImGui::MenuItem("Speed limit 60")) {
        newType = GROUND_60;
    }
    if (ImGui::MenuItem("Speed limit 70")) {
        newType = GROUND_70;
    }
    if (ImGui::MenuItem("Speed limit 80")) {
        newType = GROUND_80;
    }
    if (ImGui::MenuItem("Speed limit 90")) {
        newType = GROUND_90;
    }
    if (ImGui::MenuItem("End speed limit 10")) {
        newType = GROUND_10_END;
    }
    if (ImGui::MenuItem("End speed limit 20")) {
        newType = GROUND_20_END;
    }
    if (ImGui::MenuItem("End speed limit 30")) {
        newType = GROUND_30_END;
    }
    if (ImGui::MenuItem("End speed limit 40")) {
        newType = GROUND_40_END;
    }
    if (ImGui::MenuItem("End speed limit 50")) {
        newType = GROUND_50_END;
    }
    if (ImGui::MenuItem("End speed limit 60")) {
        newType = GROUND_60_END;
    }
    if (ImGui::MenuItem("End speed limit 70")) {
        newType = GROUND_70_END;
    }
    if (ImGui::MenuItem("End speed limit 80")) {
        newType = GROUND_80_END;
    }
    if (ImGui::MenuItem("End speed limit 90")) {
        newType = GROUND_90_END;
    }
    if (ImGui::MenuItem("Arrow left")) {
        newType = GROUND_ARROW_LEFT;
    }
    if (ImGui::MenuItem("Arrow right")) {
        newType = GROUND_ARROW_RIGHT;
    }
    if (ImGui::MenuItem("End")) {
        newType = END;
    }

    if (NONE != newType) {
        scene.items.emplace_back(std::make_shared<Scene::Item>(newType));
    }
}

void GuiModule::renderPoseWindow(Pose* selectedPose) {

    if (showPoseWindow) {

        ImGui::Begin("Pose", &showPoseWindow);

        if (nullptr != selectedPose) {

            ImGui::InputFloat3("position",
                    glm::value_ptr(selectedPose->position));

            ImGui::InputFloat3("scale", glm::value_ptr(selectedPose->scale));
            
            glm::vec3 eulerAngles = selectedPose->getEulerAngles();

            if (ImGui::InputFloat3("rotation", glm::value_ptr(eulerAngles), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                selectedPose->setEulerAngles(eulerAngles);
            }
        }

        ImGui::End();
    }
}

void GuiModule::renderErrorDialog(std::string& msg) {

    if (!msg.empty()) { 
        ImGui::OpenPopup("Error");
    }

    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::Text("%s", msg.c_str());

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            msg = "";
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GuiModule::renderCarPropertiesWindow(Scene::Car& car) {
        
    if (showCarPropertiesWindow) {

        ImGui::Begin("Car Properties", &showCarPropertiesWindow); 

        if (ImGui::TreeNode("Pose")) { 

            ImGui::InputFloat3("position", (float*)&car.modelPose);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("System Parameters")) {

            ImGui::InputDouble("axesDistance", &car.systemParams.axesDistance);
            ImGui::InputDouble("axesMomentRatio", &car.systemParams.axesMomentRatio);
            ImGui::InputDouble("inertia", &car.systemParams.inertia);
            ImGui::InputDouble("mass", &car.systemParams.mass);
            ImGui::InputDouble("distCogToFrontAxle", &car.systemParams.distCogToFrontAxle);
            ImGui::InputDouble("distCogToRearAxle", &car.systemParams.distCogToRearAxle);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Limits")) {

            ImGui::InputDouble("max F", &car.limits.max_F);
            ImGui::InputDouble("max delta", &car.limits.max_delta);
            ImGui::InputDouble("max d delta", &car.limits.max_d_delta);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Vesc")) {

            ImGui::InputDouble("velocity", &car.vesc.velocity);
            ImGui::InputDouble("steeringAngle", &car.vesc.steeringAngle);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Wheels")) {

            ImGui::Checkbox("use pacejka model", &car.wheels.usePacejkaModel);

            ImGui::InputDouble("B_front", &car.wheels.B_front);
            ImGui::InputDouble("B_rear", &car.wheels.B_rear);
            ImGui::InputDouble("C_front", &car.wheels.C_front);
            ImGui::InputDouble("C_rear", &car.wheels.C_rear);
            ImGui::InputDouble("D_front", &car.wheels.D_front);
            ImGui::InputDouble("D_rear", &car.wheels.D_rear);
            ImGui::InputDouble("k_front", &car.wheels.k_front);
            ImGui::InputDouble("k_rear", &car.wheels.k_rear);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Main Camera")) {

            ImGui::InputFloat3("position", (float*)&car.mainCamera.pose.position);

            glm::vec3 eulerAngles = car.mainCamera.pose.getEulerAngles();

            if (ImGui::InputFloat3("rotation", glm::value_ptr(eulerAngles), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                car.mainCamera.pose.setEulerAngles(eulerAngles);
            }

            ImGui::InputInt("image width", (int*)&car.mainCamera.imageWidth);
            ImGui::InputInt("image height", (int*)&car.mainCamera.imageHeight);
            ImGui::InputFloat("fov", &car.mainCamera.fovy);
            ImGui::InputFloat3("radial distortion",
                    car.mainCamera.distortionCoefficients.radial);
            ImGui::InputFloat3("tangential distortion",
                    car.mainCamera.distortionCoefficients.radial);

            ImGui::TreePop();
        }

        ImGui::End(); 
    }
}

void GuiModule::renderHelpWindow() {

    if (showHelpWindow) { 

        ImGui::Begin("Help", &showHelpWindow);

        ImGui::Text("Use w a s d to move the camera");
        ImGui::Text("Use arrow keys to move the vehicle");
        ImGui::Text("Right click and drag to move the camera");
        ImGui::Text("Press c to toggle car camera");
        ImGui::Text("Press p to pause the simulation");

        ImGui::Separator();

        ImGui::Text("Press r to reload the current save file");

        ImGui::Separator();

        ImGui::Text("Click anywhere on the floor to build a track");
        ImGui::Text("Press 1 to build straight tracks");
        ImGui::Text("Press 2 to build curves");
        ImGui::Text("Double/right click to exit track building");

        ImGui::Separator();

        ImGui::Text("Click on a green marker to select it");
        ImGui::Text("Click again to cycle tranformation modes");
        ImGui::Text("Press ESC or click anywhere to deselect");

        ImGui::End();
    }
}

void GuiModule::renderOpenFileDialog(Scene& scene, bool show) {

    /*
     * TODO: This does not really fit in here.
     * Thematically it seems ok though, if you count reacting to
     * key presses as "GUI" work.
     */
    if (!openedFilename.empty()) {
        for (KeyEvent& e : getKeyEvents()) {
            if (e.key == GLFW_KEY_R && e.action == GLFW_PRESS) {
                if (scene.load(selectedFilename)) {
                    Scene::history.clear();
                } else {
                    errorMessage = "Could not open " + selectedFilename + "!";
                }
            }
        }
    }

    if (show) {
        ImGui::OpenPopup("Open File");
    }

    if (ImGui::BeginPopupModal("Open File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        renderDirectoryListing();

        if (ImGui::Button("Open", ImVec2(120, 0))) {
            if (scene.load(selectedFilename)) {
                openedPath = currentDirectory;
                openedFilename = selectedFilename;
                ImGui::CloseCurrentPopup();

                Scene::history.clear();
            } else {
                errorMessage = "Could not open " + selectedFilename + "!";
            }
        }

        renderErrorDialog(errorMessage);

        ImGui::SetItemDefaultFocus();

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GuiModule::renderSaveFileDialog(Scene& scene, bool show, bool showSaveAs) {

    if (!openedFilename.empty() && !showSaveAs && show) {
        scene.save(openedPath + openedFilename);
        return;
    }

    if (show || showSaveAs) {
        ImGui::OpenPopup("Save File");
    }

    if (ImGui::BeginPopupModal("Save File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        renderDirectoryListing();

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            if (scene.save(currentDirectory + selectedFilename)) {
                openedPath = currentDirectory;
                openedFilename = selectedFilename;
                ImGui::CloseCurrentPopup();
            } else {
                errorMessage = "Saving " + selectedFilename + " failed!";
            }
        }

        renderErrorDialog(errorMessage);

        ImGui::SetItemDefaultFocus();

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GuiModule::renderRulePropertiesWindow(Scene::Rules& rules) {
    
    if (showRulePropertiesWindow) {

        ImGui::Begin("Rule Properties", &showRulePropertiesWindow);

        ImGui::Checkbox("Exit on obstacle collision", &rules.exitOnObstacleCollision);
        ImGui::Checkbox("Exit if not on track", &rules.exitIfNotOnTrack);
        ImGui::Checkbox("Exit if speed limit exceeded", &rules.exitIfSpeedLimitExceeded);
        ImGui::Checkbox("Exit if left arrow ignored", &rules.exitIfLeftArrowIgnored);
        ImGui::Checkbox("Exit if right arrow ignored", &rules.exitIfRightArrowIgnored);
        ImGui::Checkbox("Exit if stop line ignored", &rules.exitIfStopLineIgnored);
        ImGui::Checkbox("Exit if give way line ignored", &rules.exitIfGiveWayLineIgnored);
        ImGui::Checkbox("Exit if on end item", &rules.exitIfOnEndItem);

        ImGui::End();
    }
}

void GuiModule::renderDirectoryListing() {

    ImGui::BeginChild("Dir Listing",
            ImVec2(500, 400),
            false,
            ImGuiWindowFlags_HorizontalScrollbar);

    DIR* dir = opendir(currentDirectory.c_str());

    std::vector<dirent> entries;

    if (dir) {
       dirent* e;
       while (NULL != (e = readdir(dir))) {
           entries.push_back(*e);
       }
    } 

    closedir(dir);

    if (ImGui::Selectable("[dir] ..", false)) {
        // Sigh ... this is not a pretty solution but was easy to implement and
        // works quite well. Unless someone decides to rape the ".." button like
        // a fuckin lunatic the currentDirectory string should be fine.
        currentDirectory += "../";
    }

    for (dirent& e : entries) {

        if (e.d_name[0] == '.') {
            continue;
        }

        std::string filename(e.d_name);

        std::string entryName = "";
        if (e.d_type == DT_REG) {
            entryName += filename;
        }
        if (e.d_type == DT_DIR) {
            entryName = "[dir] ";
            entryName += filename;
        }
        if (entryName.empty()) {
            continue;
        }

        if (ImGui::Selectable(
                    entryName.c_str(),
                    selectedFilename == filename)) {
            if (e.d_type == DT_DIR) {
                currentDirectory += filename + "/";
            } else {
                selectedFilename = filename;
            }
        }
    }

    ImGui::EndChild();

    char filenameInputBuf[256];

    strncpy(filenameInputBuf,
            selectedFilename.c_str(),
            sizeof(filenameInputBuf));

    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.8);
    ImGui::InputText("Selected File",
            filenameInputBuf, IM_ARRAYSIZE(filenameInputBuf));
    ImGui::PopItemWidth();

    selectedFilename = std::string(filenameInputBuf);

    /*
     * Normalizes the directory path.
     * Transforms bits like "./asdf/test/../" to "./asdf/"
     */

    std::stringstream ss(currentDirectory);
    std::string pathElement;

    std::vector<std::string> pathElements;

    while (std::getline(ss, pathElement, '/')) {
        if (pathElements.empty()) {
            pathElements.push_back(pathElement);
        } else if (pathElement == "..") {
            if (pathElements.back() == ".."
                    || pathElements.back() == ".") {
                pathElements.push_back(pathElement);
            } else {
                pathElements.pop_back();
            }
        } else {
            pathElements.push_back(pathElement);
        }
    }

    currentDirectory = "";

    for (std::string& s : pathElements) {
        currentDirectory += s + "/";
    }
}

void GuiModule::begin() {

    for (KeyEvent& e : getKeyEvents()) {
        ImGui_ImplGlfw_KeyCallback(e.window, e.key, e.scancode, e.action, e.mods);
    }
    for (CharEvent& e : getCharEvents()) {
        ImGui_ImplGlfw_CharCallback(e.window, e.codepoint);
    }
    for (ScrollEvent& e : getScrollEvents()) {
        ImGui_ImplGlfw_ScrollCallback(e.window, e.xoffset, e.yoffset);
    }
    for (MouseButtonEvent& e : getMouseButtonEvents()) {
        ImGui_ImplGlfw_MouseButtonCallback(e.window, e.button, e.action, e.mods);
    }

    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse) {
        clearMouseInput();
    }
    if (io.WantCaptureKeyboard) {
        clearKeyboardInput();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiModule::end() {

    ImGui::Render();

    int display_w, display_h;
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(window);
}
