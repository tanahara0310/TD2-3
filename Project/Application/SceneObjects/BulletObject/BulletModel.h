#pragma once
#include <string>
#include "Engine/ObjectCommon/GameObject.h"

/// @brief 複数の同じモデルを扱うための基底クラス
class BulletModel : public GameObject {
public:
    BulletModel(const std::string& modelPath, const std::string& texturePath);
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw(const ICamera* camera) = 0;

    WorldTransform& GetTransform() { return transform_; }
};