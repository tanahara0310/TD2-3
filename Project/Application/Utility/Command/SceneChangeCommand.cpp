#include "SceneChangeCommand.h"
#include "Scene/SceneManager.h"

SceneChangeCommand::SceneChangeCommand(const std::string& sceneName, SceneManager* sceneManager)
    : sceneName_(sceneName), sceneManager_(sceneManager) {}

void SceneChangeCommand::Execute() {
    if (sceneManager_) {
        sceneManager_->ChangeScene(sceneName_);
    }
}
