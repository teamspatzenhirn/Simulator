#include "ItemsModule.h"

ItemsModule::ItemsModule() { 

}

ItemsModule::~ItemsModule() {

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
            case GROUND_30:
                ground30Model.render(shaderProgramId, modelMat);
                break;
            default:
                break;
        }
    }
}
