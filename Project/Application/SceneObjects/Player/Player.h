#pragma once
#include "Engine/ObjectCommon/GameObject.h"

#include <externals/nlohmann/single_include/nlohmann/json.hpp>

class Player final : public GameObject {
public:
    Player();

    void Initialize();
    void Update() override;
    void Draw(const ICamera* camera) override;
#ifdef _DEBUG
    const char* GetObjectName() const override { return "Player"; }
#endif

    Vector3& GetTransform();

private:
    nlohmann::json config_;

    // Grid movement state
    bool isMoving_ = false;
    Vector3 startPos_;
    Vector3 targetPos_;
    float moveTimer_ = 0.0f;
    float moveDuration_ = 0.2f; // Time to move one grid unit
};