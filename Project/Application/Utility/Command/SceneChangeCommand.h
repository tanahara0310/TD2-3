#pragma once
#include "ISceneCommand.h"
#include <string>

class SceneManager;

class SceneChangeCommand : public ISceneCommand {
public:
    SceneChangeCommand() = delete;
    explicit SceneChangeCommand(const std::string& sceneName, SceneManager* sceneManager);
    void Execute() override;
    void Undo() override {
        // シーン変更のUndoは実装しない
    }
#ifdef _DEBUG
    const char* GetCommandName() const override { return "SceneChangeCommand"; }
#endif // _DEBUG

private:
    std::string sceneName_;
    SceneManager* sceneManager_;
};