#include "ItemsModule.h"

ItemsModule::ItemsModule() { 

}

ItemsModule::~ItemsModule() {

}

void ItemsModule::update(
        std::vector<std::shared_ptr<Scene::Item>>& items,
        Pose* selection) {

    if (nullptr == selection) {
        return;
    }

    bool del = false;
    for (KeyEvent& e : getKeyEvents()) {
        if ((e.key == GLFW_KEY_BACKSPACE && e.action == GLFW_PRESS)
                || (e.key == GLFW_KEY_DELETE && e.action == GLFW_PRESS)) {
            del = true;
        }
    }

    if (!del) {
        return;
    }

    items.erase(
            std::remove_if(
                items.begin(),
                items.end(),
                [&](const std::shared_ptr<Scene::Item>& i){
                    return &i.get()->pose == selection;
                }),
            items.end());
}

void ItemsModule::render(GLuint shaderProgramId,
        std::vector<std::shared_ptr<Scene::Item>>& items) {
    
    for (std::shared_ptr<Scene::Item>& i : items) {
        glm::mat4 modelMat = i->pose.getMatrix();

        switch (i->type) {
            case OBSTACLE:
                obstacleModel.render(shaderProgramId, modelMat);
                break;
            case START_LINE:
                startLineModel.render(shaderProgramId, modelMat);
                break;
            case STOP_LINE:
                stopLineModel.render(shaderProgramId, modelMat);
                break;
            case GIVE_WAY_LINE:
                giveWayLineModel.render(shaderProgramId, modelMat);
                break;
            case CROSSWALK:
                crosswalkModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_10:
                ground10Model.render(shaderProgramId, modelMat);
                break;
            case GROUND_20:
                ground20Model.render(shaderProgramId, modelMat);
                break;
            case GROUND_30:
                ground30Model.render(shaderProgramId, modelMat);
                break;
            case GROUND_40:
                ground40Model.render(shaderProgramId, modelMat);
                break;
            case GROUND_50:
                ground50Model.render(shaderProgramId, modelMat);
                break;
            case GROUND_60:
                ground60Model.render(shaderProgramId, modelMat);
                break;
            case GROUND_70:
                ground70Model.render(shaderProgramId, modelMat);
                break;
            case GROUND_80:
                ground80Model.render(shaderProgramId, modelMat);
                break;
            case GROUND_90:
                ground90Model.render(shaderProgramId, modelMat);
                break;
            case GROUND_10_END:
                ground10EndModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_20_END:
                ground20EndModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_30_END:
                ground30EndModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_40_END:
                ground40EndModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_50_END:
                ground50EndModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_60_END:
                ground60EndModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_70_END:
                ground70EndModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_80_END:
                ground80EndModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_90_END:
                ground90EndModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_ARROW_LEFT:
                groundArrowLeftModel.render(shaderProgramId, modelMat);
                break;
            case GROUND_ARROW_RIGHT:
                groundArrowRightModel.render(shaderProgramId, modelMat);
                break;
            default:
                break;
        }
    }
}
