#include "StringRenderer.h"
#include "Engine/Graphics/Model/ModelManager.h"
#include "Engine/Graphics/Common/DirectXCommon.h"
#include "Graphics/TextureManager.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Input/MouseInput.h"
#include "Engine/Camera/CameraManager.h"
#include "Engine/Camera/Release/Camera.h"
#include "Engine/WinApp/WinApp.h"
#include "Application/Utility/MatsumotoUtility.h"

StringRenderer::StringRenderer() {
    isActive_ = true;
}

StringRenderer::~StringRenderer() {}

void StringRenderer::Initialize(int segmentCount) {
    auto engine = GetEngineSystem();
    if (!engine) return;

    auto dxCommon = engine->GetComponent<CoreEngine::DirectXCommon>();
    auto modelManager = engine->GetComponent<CoreEngine::ModelManager>();

    if (!dxCommon || !modelManager) return;

    // テクスチャを一度だけロード
    auto& textureManager = CoreEngine::TextureManager::GetInstance();
    whiteTexture_ = textureManager.Load("Texture/white1x1.png");

    segments_.clear();
    for (int i = 0; i < segmentCount; ++i) {
        Segment seg;
        seg.model = modelManager->CreateStaticModel("SampleAssets/Sphere/sphere.obj");
        seg.model->SetMaterialColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        seg.transform.Initialize(dxCommon->GetDevice());
        seg.transform.scale = { 0.5f, 0.5f, 0.5f };
        segments_.push_back(std::move(seg));
    }
}

void StringRenderer::Update() {
    Update(startPos_, endPos_, sagDir_, ballVelocity_);
}

void StringRenderer::Update(const CoreEngine::Vector3& startPos, const CoreEngine::Vector3& endPos, const CoreEngine::Vector3& sagDir, const CoreEngine::Vector3& ballVelocity) {
    startPos_ = startPos;
    endPos_ = endPos;
    sagDir_ = sagDir;
    ballVelocity_ = ballVelocity;

    if (segments_.empty()) return;

    CoreEngine::Vector3 sagOffset{ 0.0f, 0.0f, 0.0f };
    
    // 物理的なたわみと引っ張りの計算
    float dist = CoreEngine::Math::Vector::Length(endPos - startPos);
    
    // 1. 狙っている方向に「引かれる」効果（向きが逆との指摘により反転）
    float pullStrength = 0.03f; // さらに控えめに
    CoreEngine::Vector3 pullOffset = sagDir * dist * (-pullStrength); // 符号を反転

    // 2. ボールの速度による慣性の残り（ラグ効果）
    float lagFactor = 0.05f; // さらに控えめに
    CoreEngine::Vector3 lagOffset = -ballVelocity * lagFactor;

    // オフセットの合算
    sagOffset = pullOffset + lagOffset;

    CoreEngine::Vector3 p1 = startPos;
    CoreEngine::Vector3 p3 = endPos;
    
    CoreEngine::Vector3 mid = (p1 + p3) * 0.5f;
    
    // 基本的な重力によるわずかなたわみ（さらに控えめに）
    mid.y -= dist * 0.03f; 
    
    // 引っ張りと慣性を追加
    mid += sagOffset;

    CoreEngine::Vector3 p2 = mid;

    CoreEngine::Vector3 p0 = p1 + (p1 - p2);
    CoreEngine::Vector3 p4 = p3 + (p3 - p2);

    int count = static_cast<int>(segments_.size());
    for (int i = 0; i < count; ++i) {
        float totalT = static_cast<float>(i) / static_cast<float>(count - 1);
        
        CoreEngine::Vector3 pos;
        CoreEngine::Vector3 nextPos;
        float nextT = static_cast<float>(i + 1) / static_cast<float>(count - 1);

        if (totalT < 0.5f) {
            float t = totalT * 2.0f;
            pos = CatmullRomInterpolation(p0, p1, p2, p3, t);
        } else {
            float t = (totalT - 0.5f) * 2.0f;
            pos = CatmullRomInterpolation(p1, p2, p3, p4, t);
        }

        if (nextT < 0.5f) {
            float t = nextT * 2.0f;
            nextPos = CatmullRomInterpolation(p0, p1, p2, p3, t);
        } else {
            float t = (nextT - 0.5f) * 2.0f;
            nextPos = CatmullRomInterpolation(p1, p2, p3, p4, t);
        }

        segments_[i].transform.translate = pos;
        
        CoreEngine::Vector3 dir = CoreEngine::Math::Vector::Normalize(nextPos - pos);
        if (CoreEngine::Math::Vector::Length(dir) > 0.0001f) {
            segments_[i].transform.rotate = MatsumotoUtility::DirectionToEulerAngle(dir);
        }
        
        segments_[i].transform.TransferMatrix();
    }
}

void StringRenderer::Draw(const CoreEngine::ICamera* camera) {
    if (!camera) return;
    for (auto& seg : segments_) {
        seg.model->Draw(seg.transform, camera, whiteTexture_.gpuHandle);
    }
}

CoreEngine::Vector3 StringRenderer::CatmullRomInterpolation(
    const CoreEngine::Vector3& p0,
    const CoreEngine::Vector3& p1,
    const CoreEngine::Vector3& p2,
    const CoreEngine::Vector3& p3,
    float t) {
    
    float t2 = t * t;
    float t3 = t2 * t;

    return (
        (p1 * 2.0f) +
        (p2 - p0) * t +
        (p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t2 +
        (p1 * 3.0f - p0 - p2 * 3.0f + p3) * t3
    ) * 0.5f;
}
