#include "GuiModule.h"

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <filesystem>

#include "Storage.h"

#include "ocornut_imgui/imgui.h"
#include "ocornut_imgui/imgui_impl_glfw.h"
#include "ocornut_imgui/imgui_impl_opengl3.h"

namespace fs = std::filesystem;

GuiModule::GuiModule(GLFWwindow* window, std::string scenePath) {

    this->window = window;

    fs::path homePath = getResourcePath();
    imguiIniPath = homePath / "imgui.ini";

    if (scenePath.empty()) {
        scenePath = fs::canonical("./");
    }

    openedFilePath = fs::path(scenePath);
    selectedFilePath = scenePath;

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

void GuiModule::renderRootWindow(Scene& scene, Settings& settings) {

    bool showOpenFileDialog = false;
    bool showSaveFileDialog = false;
    bool showSaveAsFileDialog = false;

    ImGui::SetNextWindowBgAlpha(0.3f);

    ImGui::SetNextWindowPos(
            ImVec2(10.0f, ImGui::GetIO().DisplaySize.y - 10.0f),
            ImGuiCond_Always,
            ImVec2(0.0f, 1.0f));

    ImGui::Begin(" ", NULL, ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoFocusOnAppearing
            | ImGuiWindowFlags_NoNav);

    if (ImGui::BeginMainMenuBar()) {

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                scene = Scene();
                openedFilePath = "./";
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

            ImGui::MenuItem("Scene", NULL, &showSceneWindow);
            ImGui::MenuItem("Settings", NULL, &showSettingsWindow);
            ImGui::MenuItem("Rules", NULL, &showRuleWindow);
            ImGui::MenuItem("Help", NULL, &showHelpWindow);
            ImGui::MenuItem("About", NULL, &showAboutWindow);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    renderOpenFileDialog(scene, settings, showOpenFileDialog);
    renderSaveFileDialog(scene, showSaveFileDialog, showSaveAsFileDialog);

    // rendering the status text

    ImGui::Text("Version: 1.3");

    std::string msg = "Config: ";
    if (!fs::is_regular_file(openedFilePath)) { 
        msg += "none";
    } else {
        msg += fs::path(openedFilePath).filename();
    }
    ImGui::Text("%s", msg.c_str());

    ImGui::Text("Simulation time: %.2f seconds", (double)scene.simulationClock.time);

    if (scene.paused) {
        ImGui::Text("PAUSED");
    }

    // ImGui::ShowDemoWindow();

    ImGui::End();

    // updating hotkeys and stuff ...
    
    for (KeyEvent& e : getKeyEvents()) {

        if (e.key == GLFW_KEY_R && e.action == GLFW_PRESS) {

            if (fs::is_regular_file(openedFilePath)) {

                double savedSimulationTime = scene.simulationClock.time;

                if (load(scene, openedFilePath)) {
                    Scene::history.clear();
                    scene.simulationClock.time = savedSimulationTime;
                } else {
                    errorMessage = "Could not open " + openedFilePath + "!";
                }
            }
        }

        if (e.key == GLFW_KEY_H && e.action == GLFW_PRESS) {

            settings.showMarkers = !settings.showMarkers;
        }
    }
}

void GuiModule::renderCreateMenu(Scene& scene) {

    ItemType newType = NONE;
    std::string newName = "none";

    if (ImGui::BeginMenu("Obstacles")) {

        if (ImGui::MenuItem("Obstacle 10x10cm")) {
            newType = OBSTACLE;
            newName = "obstacle";
        }
        if (ImGui::MenuItem("Dynamic Obstacle")) {
            newType = DYNAMIC_OBSTACLE;
            newName = "dynamic_obstacle";
        }
        if (ImGui::MenuItem("Pedestrian")) {
            newType = PEDESTRIAN;
            newName = "pedestrian";
        }
        if (ImGui::MenuItem("Dynamic Pedestrian (walking right)")) {
            newType = DYNAMIC_PEDESTRIAN_RIGHT;
            newName = "dynamic_pedestrian_right";
        }
        if (ImGui::MenuItem("Dynamic Pedestrian (walking left)")) {
            newType = DYNAMIC_PEDESTRIAN_LEFT;
            newName = "dynamic_pedestrian_left";
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Ground markings")) {

        if (ImGui::MenuItem("Start line")) {
            newType = START_LINE;
            newName = "start_line";
        }
        if (ImGui::MenuItem("Stop line")) {
            newType = STOP_LINE;
            newName = "stop_line";
        }
        if (ImGui::MenuItem("Give-way line")) {
            newType = GIVE_WAY_LINE;
            newName = "give_way_line";
        }
        if (ImGui::MenuItem("Crosswalk")) {
            newType = CROSSWALK;
            newName = "crosswalk";
        }
        if (ImGui::MenuItem("Crosswalk small")) {
            newType = CROSSWALK_SMALL;
            newName = "crosswalk_small";
        }
        if (ImGui::MenuItem("Arrow left")) {
            newType = GROUND_ARROW_LEFT;
            newName = "arrow_left";
        }
        if (ImGui::MenuItem("Arrow right")) {
            newType = GROUND_ARROW_RIGHT;
            newName = "arrow_right";
        }
        if (ImGui::MenuItem("Barred area small")) {
            newType = BARRED_AREA_SMALL;
            newName = "barred_area_small";
        }
        if (ImGui::MenuItem("Barred area medium")) {
            newType = BARRED_AREA_MEDIUM;
            newName = "barred_area_medium";
        }
        if (ImGui::MenuItem("Barred area large")) {
            newType = BARRED_AREA_LARGE;
            newName = "barred_area_large";
        }
        if (ImGui::MenuItem("Turn Lane")) {
            newType = TURN_LANE;
            newName = "turn_lane";
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Speed limits")) {

        if (ImGui::MenuItem("Speed limit 10")) {
            newType = GROUND_10;
            newName = "speed_limit_10";
        }
        if (ImGui::MenuItem("Speed limit 20")) {
            newType = GROUND_20;
            newName = "speed_limit_20";
        }
        if (ImGui::MenuItem("Speed limit 30")) {
            newType = GROUND_30;
            newName = "speed_limit_30";
        }
        if (ImGui::MenuItem("Speed limit 40")) {
            newType = GROUND_40;
            newName = "speed_limit_40";
        }
        if (ImGui::MenuItem("Speed limit 50")) {
            newType = GROUND_50;
            newName = "speed_limit_50";
        }
        if (ImGui::MenuItem("Speed limit 60")) {
            newType = GROUND_60;
            newName = "speed_limit_60";
        }
        if (ImGui::MenuItem("Speed limit 70")) {
            newType = GROUND_70;
            newName = "speed_limit_70";
        }
        if (ImGui::MenuItem("Speed limit 80")) {
            newType = GROUND_80;
            newName = "speed_limit_80";
        }
        if (ImGui::MenuItem("Speed limit 90")) {
            newType = GROUND_90;
            newName = "speed_limit_90";
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("End speed limits")) {

        if (ImGui::MenuItem("End speed limit 10")) {
            newType = GROUND_10_END;
            newName = "end_speed_limit_10";
        }
        if (ImGui::MenuItem("End speed limit 20")) {
            newType = GROUND_20_END;
            newName = "end_speed_limit_20";
        }
        if (ImGui::MenuItem("End speed limit 30")) {
            newType = GROUND_30_END;
            newName = "end_speed_limit_30";
        }
        if (ImGui::MenuItem("End speed limit 40")) {
            newType = GROUND_40_END;
            newName = "end_speed_limit_40";
        }
        if (ImGui::MenuItem("End speed limit 50")) {
            newType = GROUND_50_END;
            newName = "end_speed_limit_50";
        }
        if (ImGui::MenuItem("End speed limit 60")) {
            newType = GROUND_60_END;
            newName = "end_speed_limit_60";
        }
        if (ImGui::MenuItem("End speed limit 70")) {
            newType = GROUND_70_END;
            newName = "end_speed_limit_70";
        }
        if (ImGui::MenuItem("End speed limit 80")) {
            newType = GROUND_80_END;
            newName = "end_speed_limit_80";
        }
        if (ImGui::MenuItem("End speed limit 90")) {
            newType = GROUND_90_END;
            newName = "end_speed_limit_90";
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Special")) {

        if (ImGui::MenuItem("Start box")) {
            newType = START_BOX;
            newName = "start_box";
        }
        if (ImGui::MenuItem("Traffic island")) {
            newType = TRAFFIC_ISLAND;
            newName = "traffic_island";
        }
        if (ImGui::MenuItem("Park section")) {
            newType = PARK_SECTION;
            newName = "park_section";
        }
        if (ImGui::MenuItem("Park slots")) {
            newType = PARK_SLOTS;
            newName = "park_slots";
        }
        if (ImGui::MenuItem("No parking")) {
            newType = NO_PARKING;
            newName = "no_parking";
        }
        if (ImGui::MenuItem("Calib mat")) {
            newType = CALIB_MAT;
            newName = "calib_mat";
        }
        if (ImGui::MenuItem("Checkpoint")) {
            newType = CHECKPOINT;
            newName = "checkpoint";
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Signs")) {

        if (ImGui::MenuItem("Forbidden")) {
            newType = SIGN_FORBIDDEN;
            newName = "sign_forbidden";
        }

        if (ImGui::MenuItem("Downhill")) {
            newType = SIGN_DOWNHILL;
            newName = "sign_downhill";
        }

        if (ImGui::MenuItem("Expressway start")) {
            newType = SIGN_EXPRESSWAY_START;
            newName = "sign_expressway_start";
        }

        if (ImGui::MenuItem("Expressway end")) {
            newType = SIGN_EXPRESSWAY_END;
            newName = "sign_expressway_end";
        }

        if (ImGui::MenuItem("Giveway")) {
            newType = SIGN_GIVEWAY;
            newName = "sign_giveway_end";
        }

        if (ImGui::MenuItem("No passing start")) {
            newType = SIGN_NO_PASSING_START;
            newName = "sign_no_passing_start";
        }

        if (ImGui::MenuItem("No passing end")) {
            newType = SIGN_NO_PASSING_END;
            newName = "sign_no_passing_end";
        }

        ImGui::EndMenu();
    }

    if (NONE != newType) {
        scene.items.emplace_back(Scene::Item(newType, newName));
    }
}

void GuiModule::renderPoseGui(Pose& pose) {

    ImGui::InputFloat3("position",
            glm::value_ptr(pose.position));

    ImGui::InputFloat3("scale", glm::value_ptr(pose.scale));
    
    glm::vec3 eulerAngles = pose.getEulerAngles();

    if (ImGui::InputFloat3("rotation", glm::value_ptr(eulerAngles), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
        pose.setEulerAngles(eulerAngles);
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

void GuiModule::renderSceneWindow(Scene& scene) {
        
    if (showSceneWindow) {

        ImGui::Begin("Scene", &showSceneWindow); 

        bool car_open = ImGui::TreeNode("Car");

        if(ImGui::IsItemClicked()) {
            scene.selection.pose = &scene.car.modelPose;
            scene.selection.handled = true;
        }

        if (car_open) {

            if (ImGui::TreeNode("Pose")) { 
                renderPoseGui(scene.car.modelPose);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("System Parameters")) {

                ImGui::InputDouble("axesDistance", &scene.car.systemParams.axesDistance);
                ImGui::InputDouble("axesMomentRatio", &scene.car.systemParams.axesMomentRatio);
                ImGui::InputDouble("inertia", &scene.car.systemParams.inertia);
                ImGui::InputDouble("mass", &scene.car.systemParams.mass);
                ImGui::InputDouble("distCogToFrontAxle", &scene.car.systemParams.distCogToFrontAxle);
                ImGui::InputDouble("distCogToRearAxle", &scene.car.systemParams.distCogToRearAxle);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Limits")) {

                ImGui::InputDouble("max F", &scene.car.limits.max_F);
                ImGui::InputDouble("max delta", &scene.car.limits.max_delta);
                ImGui::InputDouble("max d delta", &scene.car.limits.max_d_delta);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Vesc")) {

                ImGui::InputDouble("velocity", &scene.car.vesc.velocity);
                ImGui::InputDouble("steeringAngle", &scene.car.vesc.steeringAngle);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Wheels")) {

                ImGui::Checkbox("use pacejka model", &scene.car.wheels.usePacejkaModel);

                ImGui::InputDouble("B_front", &scene.car.wheels.B_front);
                ImGui::InputDouble("B_rear", &scene.car.wheels.B_rear);
                ImGui::InputDouble("C_front", &scene.car.wheels.C_front);
                ImGui::InputDouble("C_rear", &scene.car.wheels.C_rear);
                ImGui::InputDouble("D_front", &scene.car.wheels.D_front);
                ImGui::InputDouble("D_rear", &scene.car.wheels.D_rear);
                ImGui::InputDouble("k_front", &scene.car.wheels.k_front);
                ImGui::InputDouble("k_rear", &scene.car.wheels.k_rear);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Main Camera")) {

                ImGui::InputFloat3("position", (float*)&scene.car.mainCamera.pose.position);

                glm::vec3 eulerAngles = scene.car.mainCamera.pose.getEulerAngles();

                if (ImGui::InputFloat3("rotation", glm::value_ptr(eulerAngles), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                    scene.car.mainCamera.pose.setEulerAngles(eulerAngles);
                }

                ImGui::InputInt("image width", (int*)&scene.car.mainCamera.imageWidth);
                ImGui::InputInt("image height", (int*)&scene.car.mainCamera.imageHeight);

                ImGui::InputFloat("fov", &scene.car.mainCamera.fovy);
                ImGui::InputFloat3("radial distortion",
                        scene.car.mainCamera.distortionCoefficients.radial);
                ImGui::InputFloat3("tangential distortion",
                        scene.car.mainCamera.distortionCoefficients.radial);

                ImGui::DragFloat("noise", &scene.car.mainCamera.noise, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Laser Sensor")) {

                renderPoseGui(scene.car.laserSensor.pose);
                ImGui::InputFloat("value", &scene.car.laserSensor.value);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Binary Light Sensor")) {

                renderPoseGui(scene.car.binaryLightSensor.pose);
                ImGui::InputFloat("trigger distance",
                        &scene.car.binaryLightSensor.triggerDistance);
                ImGui::Checkbox("triggered",
                        &scene.car.binaryLightSensor.triggered);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("DynamicItemSettings")) {

            ImGui::InputFloat("speed", &scene.dynamicItemSettings.speed);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Rules")) {

            ImGui::Checkbox("Exit on obstacle collision", &scene.rules.exitOnObstacleCollision);
            ImGui::Checkbox("Exit if not on track", &scene.rules.exitIfNotOnTrack);
            ImGui::Checkbox("Exit if speed limit exceeded", &scene.rules.exitIfSpeedLimitExceeded);
            ImGui::Checkbox("Exit if left arrow ignored", &scene.rules.exitIfLeftArrowIgnored);
            ImGui::Checkbox("Exit if right arrow ignored", &scene.rules.exitIfRightArrowIgnored);
            ImGui::Checkbox("Exit if stop line ignored", &scene.rules.exitIfStopLineIgnored);
            ImGui::Checkbox("Exit if give way line ignored", &scene.rules.exitIfGiveWayLineIgnored);
            ImGui::Checkbox("Exit if crosswalk ignored", &scene.rules.exitIfCrosswalkIgnored);
            ImGui::Checkbox("Exit if no parking ignored", &scene.rules.exitIfNoParkingIgnored);
            ImGui::Checkbox("Exit if all checkpoints passed", &scene.rules.exitIfAllCheckpointsPassed);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Items")) {

            for (Scene::Item i : scene.items) {

                ImGui::PushID((int)i.id);

                int flags = 0;
                if (&(i.pose) == scene.selection.pose) {
                    flags = ImGuiTreeNodeFlags_Selected;
                }
                
                bool open = ImGui::TreeNodeEx((void*)&i, flags, "%s", i.name.c_str());

                if(ImGui::IsItemClicked()) {
                    scene.selection.pose = &i.pose;
                    scene.selection.handled = true;
                }

                if (open) {
                    char nameInputBuf[256];
                    strncpy(nameInputBuf, i.name.c_str(), sizeof(nameInputBuf));
                    ImGui::InputText("name", nameInputBuf, IM_ARRAYSIZE(nameInputBuf));
                    i.name = std::string(nameInputBuf);

                    renderPoseGui(i.pose);

                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Track")) {

            Tracks::TrackSelection& selection = scene.tracks.trackSelection;

            if (selection.track) {

                LaneMarking* centerLine{nullptr};
                if (std::shared_ptr<TrackLine> line = std::dynamic_pointer_cast<TrackLine>(selection.track)) {
                    centerLine = &line->centerLine;
                } else if (std::shared_ptr<TrackArc> arc = std::dynamic_pointer_cast<TrackArc>(selection.track)) {
                    centerLine = &arc->centerLine;
                }

                if (centerLine) {

                    ImGui::Text("Center line");

                    if (ImGui::RadioButton("Dashed", *centerLine == LaneMarking::Dashed)) {
                        *centerLine = LaneMarking::Dashed;
                        selection.changed = true;
                    }
                    if (ImGui::RadioButton("Double solid", *centerLine == LaneMarking::DoubleSolid)) {
                        *centerLine = LaneMarking::DoubleSolid;
                        selection.changed = true;
                    }
                }
            }

            ImGui::TreePop();
        }

        ImGui::End(); 
    }
}

void GuiModule::renderSettingsWindow(Settings& settings) {

    if (showSettingsWindow) { 

        bool changed = false;

        ImGui::Begin("Settings", &showSettingsWindow, ImGuiWindowFlags_AlwaysAutoResize);

        changed |= ImGui::DragFloat("Simulation speed", &settings.simulationSpeed, 0.05f, 0.01f, 10.0f);
        settings.simulationSpeed = std::max(std::min(settings.simulationSpeed, 10.0f), 0.01f);

        changed |= ImGui::DragFloat("Update delta time", &settings.updateDeltaTime, 0.001f, 0.001f, 1.0f);
        settings.updateDeltaTime = std::max(std::min(settings.updateDeltaTime, 1.0f), 0.001f);

        ImGui::Separator();

        changed |= ImGui::Checkbox("Show markers", &settings.showMarkers);
        changed |= ImGui::Checkbox("Show vehicle path", &settings.showVehiclePath);
        changed |= ImGui::Checkbox("Fancy vehicle path", &settings.fancyVehiclePath);
        changed |= ImGui::Checkbox("Show vehicle trajectory", &settings.showVehicleTrajectory);
        changed |= ImGui::Checkbox("Show laser sensor", &settings.showLaserSensor);
        changed |= ImGui::Checkbox("Show binary light sensor", &settings.showBinaryLightSensor);

        ImGui::End();

        if (changed) {
            save(settings);
        }
    }
}

void GuiModule::renderRuleWindow(const Scene::Rules& rules) {

    if (showRuleWindow) {

        std::string message = "";

        if (rules.isColliding) {
            message += "Collision with obstacle\n";
        }
        if (!rules.onTrack) {
            message += "Vehicle left track\n";
        }
        if (rules.speedLimitExceeded) {
            message += "Speed limit exceeded\n";
        }
        if (rules.leftArrowIgnored) {
            message += "Left arrow ignored\n";
        }
        if (rules.rightArrowIgnored) {
            message += "Right arrow ignored\n";
        }
        if (rules.stopLineIgnored) {
            message += "Stop line ignored\n";
        }
        if (rules.giveWayLineIgnored) {
            message += "Give way line ignored\n";
        }
        if (rules.noParkingIgnored) {
            message += "No parking ignored\n";
        }

        if ("" != message) { 
            ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(255, 0, 0, 1));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(255, 0, 0, 1));
            ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(255, 0, 0, 1));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(255, 0, 0, 1));
        }

        ImGui::Begin("Rules", &showRuleWindow,
                ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("%s", message.c_str());

        ImGui::End();

        if ("" != message) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
    }
}

void GuiModule::renderHelpWindow() {

    if (showHelpWindow) { 

        ImGui::Begin("Help", &showHelpWindow,
                ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Use w a s d to move the camera");
        ImGui::Text("Press shift to to move the camera down");
        ImGui::Text("Press space to to move the camera up");
        ImGui::Text("Use arrow keys to move the vehicle");
        ImGui::Text("Right click and drag to move the camera");
        ImGui::Text("Press c to switch car cameras");
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
        ImGui::Text("Press h to toggle marker visibility");

        ImGui::End();
    }
}

void GuiModule::renderAboutWindow() {

    if (showAboutWindow) { 

        ImGui::Begin("About", &showAboutWindow,
                ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("SpatzSim 1.3");
        ImGui::Text(" ");
        ImGui::Text("Initiated in fall 2018 by");
        ImGui::Text(" ");
        ImGui::Text("Gilberto Rossi");
        ImGui::Text("Johannes Herschel");
        ImGui::Text("Jona Ruof");

        ImGui::End();
    }
}

void GuiModule::renderOpenFileDialog(Scene& scene, Settings& settings, bool show) {

    if (show) {
        ImGui::OpenPopup("Open File");
    }

    if (ImGui::BeginPopupModal("Open File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        renderDirectoryListing();

        if (ImGui::Button("Open", ImVec2(120, 0))) {

            if (!fs::is_directory(selectedFilePath)) {

                if (load(scene, selectedFilePath)) {
                    openedFilePath = selectedFilePath;

                    settings.configPath = selectedFilePath;
                    save(settings);

                    Scene::history.clear();

                    ImGui::CloseCurrentPopup();
                } else {
                    errorMessage = "Could not open "
                        + std::string(fs::path(selectedFilePath).filename()) + "!";
                }
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

    if (!fs::is_regular_file(openedFilePath) && !showSaveAs && show) {
        save(scene,openedFilePath);
        return;
    }

    if (show || showSaveAs) {
        ImGui::OpenPopup("Save File");
    }

    if (ImGui::BeginPopupModal("Save File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        renderDirectoryListing();

        if (ImGui::Button("Save", ImVec2(120, 0))) {

            if (!fs::is_directory(selectedFilePath)) {

                if (save(scene, selectedFilePath)) {
                    openedFilePath = selectedFilePath;
                    ImGui::CloseCurrentPopup();
                } else {
                    errorMessage = "Saving " + selectedFilePath + " failed!";
                }
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

void GuiModule::renderDirectoryListing() {

    ImGui::BeginChild("Dir Listing",
            ImVec2(500, 400),
            false,
            ImGuiWindowFlags_HorizontalScrollbar);

    if (ImGui::Selectable("./..", false)) {
        if (fs::is_regular_file(selectedFilePath)) {
            selectedFilePath = fs::path(selectedFilePath)
                .parent_path()
                .parent_path();
        } else {
            selectedFilePath = fs::path(selectedFilePath).parent_path();
        }
    }

    fs::path listPath = selectedFilePath;
    if (!fs::is_directory(listPath)) {
        listPath = listPath.parent_path();
    }

    std::vector<fs::directory_entry> entries;

    for (fs::directory_entry e : fs::directory_iterator(listPath)) {
        entries.push_back(e);
    }

    std::sort(
            entries.begin(),
            entries.end(),
            [](auto& a, auto& b) -> bool {
                if (a.is_directory() && !b.is_directory()) {
                    return true;
                } else if (!a.is_directory() && b.is_directory()) {
                    return false;
                } else {
                    return a.path().filename() < b.path().filename();
                }
            }
        );

    for (fs::directory_entry e : entries) {

        std::string displayName = e.path().stem();
        displayName += e.path().extension();

        if (ImGui::Selectable(
                    displayName.c_str(),
                    selectedFilePath == e.path())) {

            selectedFilePath = e.path();
        }
    }

    ImGui::EndChild();

    //std::string inputFileName = "";

    //if (fs::is_regular_file(selectedFilePath)) { 
    std::string inputFileName = fs::path(selectedFilePath).filename();
    //}

    char filenameInputBuf[256];

    strncpy(filenameInputBuf, 
            inputFileName.c_str(), 
            sizeof(filenameInputBuf));

    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.8f);
    ImGui::InputText("Selected File",
            filenameInputBuf, IM_ARRAYSIZE(filenameInputBuf));
    ImGui::PopItemWidth();

    selectedFilePath = fs::path(selectedFilePath)
        .replace_filename(filenameInputBuf);
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
    io.IniFilename = imguiIniPath.c_str();

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
