#include "SceneCommandExecutor.h"



void SceneCommandExecutor::Initialize() {
    pendingCommands_.clear();
    commandHistory_.clear();
}

void SceneCommandExecutor::AddCommand(std::unique_ptr<ISceneCommand> command) {
    if (command) {
        pendingCommands_.emplace_back(std::move(command));
    }
}

void SceneCommandExecutor::ExecuteCommand() {
    for (auto& command : pendingCommands_) {
        command->Execute();
        commandHistory_.emplace_back(std::move(command));
    }
    pendingCommands_.clear();
}

void SceneCommandExecutor::UndoLastCommand() {
    commandHistory_.back()->Undo();
    commandHistory_.pop_back();
}
