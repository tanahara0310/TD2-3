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
};