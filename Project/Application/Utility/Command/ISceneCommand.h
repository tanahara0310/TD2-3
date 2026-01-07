#pragma once
class ISceneCommand {
public:
    virtual ~ISceneCommand() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
#ifdef _DEBUG
    virtual const char* GetCommandName() const { return "ISceneCommand"; }
#endif // _DEBUG
};