#include "UIFrameObject.h"
#include "Engine/Graphics/Common/DirectXCommon.h"
#include "Engine/Graphics/Resource/ResourceFactory.h"
#include "Engine/Camera/ICamera.h"
#include "Engine/Math/MathCore.h"
#include "Engine/WinApp/WinApp.h"
#include "EngineSystem.h"

namespace CoreEngine
{

    UIFrameObject::UIFrameObject() {
    }

    void UIFrameObject::Initialize() {
        auto* engine = GetEngineSystem();
        auto* dxCommon = engine->GetComponent<DirectXCommon>();
        auto* resourceFactory = engine->GetComponent<ResourceFactory>();

        // トランスフォームを初期化
        transform_.Initialize(dxCommon->GetDevice());

        // ジオメトリを作成
        geometry_ = std::make_unique<UIFrameGeometry>();
        geometry_->Initialize(dxCommon, resourceFactory);

        // レンダラーを作成
        renderer_ = std::make_unique<UIFrameRenderer>();
        renderer_->Initialize(dxCommon->GetDevice());

        // トランスフォームリソースを作成
        transformResource_ = resourceFactory->CreateBufferResource(
            dxCommon->GetDevice(),
            sizeof(TransformationMatrix)
        );
        transformResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformData_));

        // シーンデータリソースを作成
        sceneDataResource_ = resourceFactory->CreateBufferResource(
            dxCommon->GetDevice(),
            sizeof(SceneData)
        );
        sceneDataResource_->Map(0, nullptr, reinterpret_cast<void**>(&sceneData_));

        // シーンデータの初期設定
        sceneData_->time = 0.0f;
        sceneData_->cameraPosition = { 0.0f, 0.0f, 0.0f };
        sceneData_->resolution = { 1280.0f, 720.0f }; // デフォルト解像度
        sceneData_->intensity = 1.0f;
        sceneData_->padding = 0.0f;

        // 初期位置とスケールを設定
        transform_.translate = { 0.0f, 0.0f, 5.0f };
        transform_.scale = { 1.0f, 1.0f, 1.0f };
        transform_.rotate = { 0.0f, 0.0f, 0.0f };

        elapsedTime_ = 0.0f;
        intensity_ = 1.0f;
    }

    void UIFrameObject::Update() {
        // 時間を更新
        elapsedTime_ += 0.016f; // 約60FPS想定
        sceneData_->time = elapsedTime_;
        sceneData_->intensity = intensity_;

        // トランスフォームを転送
        transform_.TransferMatrix();
    }

    void UIFrameObject::Draw(const ICamera* camera) {
        if (!camera) return;

        auto* engine = GetEngineSystem();
        auto* dxCommon = engine->GetComponent<DirectXCommon>();
        auto* cmdList = dxCommon->GetCommandList();

        // カメラのビュープロジェクション行列を計算
        Matrix4x4 viewMatrix = camera->GetViewMatrix();
        Matrix4x4 projectionMatrix = camera->GetProjectionMatrix();
        Matrix4x4 viewProjectionMatrix = MathCore::Matrix::Multiply(viewMatrix, projectionMatrix);

        // ビュー行列の逆行列を計算（ビルボード用）
        Matrix4x4 viewInverse = MathCore::Matrix::Inverse(viewMatrix);

        // ワールド行列を取得
        Matrix4x4 worldMatrix = transform_.GetWorldMatrix();

        // WVP行列を計算
        transformData_->WVP = MathCore::Matrix::Multiply(worldMatrix, viewProjectionMatrix);
        transformData_->World = worldMatrix;
        transformData_->ViewInverse = viewInverse;

        // カメラ位置を設定
        Vector3 camPos = camera->GetPosition();
        transformData_->cameraPosition = { camPos.x, camPos.y, camPos.z, 1.0f };

        sceneData_->cameraPosition = camPos;

        // 解像度を設定（WinAppから取得）
        sceneData_->resolution = {
            static_cast<float>(CoreEngine::WinApp::kClientWidth),
            static_cast<float>(CoreEngine::WinApp::kClientHeight)
        };

        // レンダラーの描画開始
        renderer_->BeginPass(cmdList, BlendMode::kBlendModeNormal);

        // 定数バッファを設定
        cmdList->SetGraphicsRootConstantBufferView(
            UIFrameRendererRootParam::kTransform,
            transformResource_->GetGPUVirtualAddress()
        );
        cmdList->SetGraphicsRootConstantBufferView(
            UIFrameRendererRootParam::kScene,
            sceneDataResource_->GetGPUVirtualAddress()
        );

        // ジオメトリを設定して描画
        cmdList->IASetVertexBuffers(0, 1, &geometry_->GetVertexBufferView());
        cmdList->IASetIndexBuffer(&geometry_->GetIndexBufferView());
        cmdList->DrawIndexedInstanced(geometry_->GetIndexCount(), 1, 0, 0, 0);

        // レンダラーの描画終了
        renderer_->EndPass();
    }

}
