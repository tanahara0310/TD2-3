#pragma once
#include "ISceneCommand.h"
#include <memory>
#include <vector>

class SceneCommandExecutor final {
public:
    SceneCommandExecutor() = default;
    ~SceneCommandExecutor() = default;

    void Initialize();
    void AddCommand(std::unique_ptr<ISceneCommand> command);
    void ExecuteCommand();
    void UndoLastCommand();
private:
    std::vector<std::unique_ptr<ISceneCommand>> pendingCommands_;
    std::vector<std::unique_ptr<ISceneCommand>> commandHistory_;
};