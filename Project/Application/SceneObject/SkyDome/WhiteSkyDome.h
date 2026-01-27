#pragma once
#include "Engine/ObjectCommon/GameObject.h"

class WhiteSkyDome final : public CoreEngine::GameObject {
public:
    WhiteSkyDome();

    void Initialize();
    void Update() override;
    void Draw(const CoreEngine::ICamera* camera) override;
#ifdef _DEBUG
    const char* GetObjectName() const override { return "WhiteSkyDome"; }
#endif

    CoreEngine::Vector3& GetTransform();
    void SetColor(const CoreEngine::Vector4& color) { model_->SetMaterialColor(color); }
};