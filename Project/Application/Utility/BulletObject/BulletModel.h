#pragma once
#include <string>
#include "Engine/ObjectCommon/GameObject.h"

/// @brief 複数の同じモデルを扱うための基底クラス
class BulletModel : public CoreEngine::GameObject {
public:
    BulletModel(const std::string& modelPath, const std::string& texturePath);
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw(const CoreEngine::ICamera* camera) = 0;

    CoreEngine::WorldTransform& GetTransform() { return transform_; }
};