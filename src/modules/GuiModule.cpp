#include "GuiModule.h"

GuiModule::GuiModule(GLFWwindow* window) {

    this->window = window;

    openedPath = "";
    openedFilename = "";
    currentDirectory = "./";
    selectedFilename = "";

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

        if (ImGui::BeginMenu("Show")) {
            for (const auto & i : showMenuItems) {
                ImGui::MenuItem(i.first.c_str(), NULL, i.second);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    renderOpenFileDialog(scene, showOpenFileDialog);
    renderSaveFileDialog(scene, showSaveFileDialog, showSaveAsFileDialog);

    //renderErrorDialog();

    ImGui::Text("Carolo Simulator v0.2");

    std::string msg = "Config: ";
    if (openedFilename.empty()) { 
        msg += "none";
    } else {
        msg += openedFilename;
    }
    ImGui::Text(msg.c_str());

    ImGui::End();
        
    showMenuItems.clear();

    //ImGui::ShowDemoWindow(NULL);
}

void GuiModule::renderErrorDialog(std::string& msg) {

    if (!msg.empty()) { 
        ImGui::OpenPopup("Error");
    }

    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::Text(msg.c_str());

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            msg = "";
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GuiModule::renderOpenFileDialog(Scene& scene, bool show) {

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

    if (!openedFilename.empty() && !showSaveAs) {
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

void GuiModule::end(Scene& scene) {

    renderRootWindow(scene);

    ImGui::Render();

    int display_w, display_h;
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(window);
}

void GuiModule::addShowMenuItem(std::string title, bool* show) {

    showMenuItems[title] = show;
}
