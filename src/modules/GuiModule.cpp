#include "GuiModule.h"

GuiModule::GuiModule(GLFWwindow* window) {

    this->window = window;

    currentPath = fs::absolute(fs::path("."));
    selection = "";

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

    ImGui::Text("Carolo Simulator v0.2");

    std::string msg = "Config: ";
    if (openedPath.empty()) { 
        msg += "none";
    } else {
        msg += openedPath.filename().string();
    }
    ImGui::Text(msg.c_str());

    ImGui::End();
        
    showMenuItems.clear();

    //ImGui::ShowDemoWindow(NULL);
}

void GuiModule::renderOpenFileDialog(Scene& scene, bool show) {

    if (show) {
        ImGui::OpenPopup("Open File");
    }

    if (ImGui::BeginPopupModal("Open File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        renderDirectoryListing();

        if (ImGui::Button("Open", ImVec2(120, 0))) {
            if(scene.load(selection.string())) {
                openedPath = selection.string();
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SetItemDefaultFocus();

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GuiModule::renderSaveFileDialog(Scene& scene, bool show, bool showSaveAs) {

    if (!openedPath.empty() && !showSaveAs) {
        scene.save(openedPath.string());
        return;
    }

    if (show || showSaveAs) {
        ImGui::OpenPopup("Save File");
    }

    if (ImGui::BeginPopupModal("Save File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        renderDirectoryListing();

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            if (scene.save(selection.string())) {
                openedPath = selection;
            }
            ImGui::CloseCurrentPopup();
        }

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

    if (ImGui::Selectable("[dir] ..", false)) {
        currentPath = currentPath.parent_path();
    }

    for (auto& i : fs::directory_iterator(currentPath)) {

        if (i.path().stem().string().empty()) {
            continue;
        }

        std::string name = "";

        if (fs::is_regular_file(i)) {
            name = i.path().filename();
        }

        if (fs::is_directory(i)) {
            name = "[dir] ";
            name += i.path().stem();
        }

        if (!name.empty()) {
            if (ImGui::Selectable(name.c_str(),
                        selection == fs::absolute(i.path()))) {
                if (fs::is_directory(i)) {
                    currentPath = i.path();
                } else {
                    selection = fs::absolute(i.path());
                }
            }
        }
    }

    ImGui::EndChild();

    char filenameInputBuf[256];

    strncpy(filenameInputBuf,
            selection.filename().c_str(),
            sizeof(filenameInputBuf));

    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.8);
    ImGui::InputText("Selected File",
            filenameInputBuf, IM_ARRAYSIZE(filenameInputBuf));
    ImGui::PopItemWidth();

    if (!fs::is_directory(selection)) {
        selection = selection.remove_filename(); 
    }
    selection /= fs::path(filenameInputBuf);
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
