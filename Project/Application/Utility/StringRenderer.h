#pragma once
#include <vector>
#include <memory>
#include <EngineSystem.h>
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Graphics/Model/Model.h"
#include "WorldTransfom/WorldTransform.h"

namespace CoreEngine {
    class DirectXCommon;
}

class StringRenderer final : public CoreEngine::GameObject {
public:
    StringRenderer();
    ~StringRenderer() override;

    void Initialize(int segmentCount = 16);
    void Update() override; // Dummy override if needed, but we need custom update
    void Update(const CoreEngine::Vector3& startPos, const CoreEngine::Vector3& endPos, const CoreEngine::Vector3& sagDir, const CoreEngine::Vector3& ballVelocity);
    void Draw(const CoreEngine::ICamera* camera) override;

private:
    CoreEngine::Vector3 CatmullRomInterpolation(
        const CoreEngine::Vector3& p0,
        const CoreEngine::Vector3& p1,
        const CoreEngine::Vector3& p2,
        const CoreEngine::Vector3& p3,
        float t);

    struct Segment {
        std::unique_ptr<CoreEngine::Model> model;
        CoreEngine::WorldTransform transform;
    };

    std::vector<Segment> segments_;
    CoreEngine::TextureManager::LoadedTexture whiteTexture_{};
    CoreEngine::Vector3 startPos_;
    CoreEngine::Vector3 endPos_;
    CoreEngine::Vector3 sagDir_;
    CoreEngine::Vector3 ballVelocity_;
};
