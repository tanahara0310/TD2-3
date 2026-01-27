#pragma once
#include <EngineSystem.h>
#include "Engine/ObjectCommon/GameObject.h"

class Ground final : public CoreEngine::GameObject {
public:
    Ground();
    void Initialize();
    void Update() override;
    void Draw(const CoreEngine::ICamera* camera) override;
#ifdef _DEBUG
    const char* GetObjectName() const override { return "Ground"; }
#endif

};