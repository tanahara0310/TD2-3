#include "YoYoObject.h"
#include "Engine/Graphics/Common/DirectXCommon.h"
#include "Engine/Graphics/Resource/ResourceFactory.h"
#include "Engine/Camera/ICamera.h"
#include "Engine/Math/MathCore.h"
#include "EngineSystem.h"

namespace CoreEngine
{

YoYoObject::YoYoObject() {
}

void YoYoObject::Initialize() {
    auto* engine = GetEngineSystem();
    auto* dxCommon = engine->GetComponent<DirectXCommon>();
    auto* resourceFactory = engine->GetComponent<ResourceFactory>();
    
    // トランスフォームを初期化
    transform_.Initialize(dxCommon->GetDevice());
    
    // ジオメトリを作成
    geometry_ = std::make_unique<YoYoGeometry>();
    geometry_->Initialize(dxCommon, resourceFactory);
    
    // レンダラーを作成
    renderer_ = std::make_unique<YoYoRenderer>();
    renderer_->Initialize(dxCommon->GetDevice());
    
    // マテリアルリソースを作成
    materialResource_ = resourceFactory->CreateBufferResource(dxCommon->GetDevice(), sizeof(YoYoMaterialData));
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    
    // マテリアルの初期設定
    materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    materialData_->enableLighting = 1;
    materialData_->time = 0.0f;
    materialData_->sparkIntensity = 0.0f;
    materialData_->padding = 0.0f;
    
    // トランスフォームリソースを作成
    transformResource_ = resourceFactory->CreateBufferResource(dxCommon->GetDevice(), sizeof(TransformationMatrix));
    transformResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformData_));
    
    // カメラリソースを作成
    cameraResource_ = resourceFactory->CreateBufferResource(dxCommon->GetDevice(), sizeof(CameraData));
    cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
    
    // 初期位置とスケールを設定
    transform_.translate = { 0.0f, 0.0f, 5.0f };
    transform_.scale = { 1.0f, 1.0f, 1.0f };
    transform_.rotate = { 0.0f, 0.0f, 0.0f };
}

void YoYoObject::Update() {
    // ヨーヨーを回転させる
    transform_.rotate.y += rotationSpeed_ * 0.02f;
    
    // 時間を更新（コスミックエフェクト用）
    elapsedTime_ += 0.016f; // 約60FPS想定
    materialData_->time = elapsedTime_;
    materialData_->sparkIntensity = sparkIntensity_;
    
    // トランスフォームを転送
    transform_.TransferMatrix();
}

void YoYoObject::Draw(const ICamera* camera) {
    if (!camera) return;
    
    auto* engine = GetEngineSystem();
    auto* dxCommon = engine->GetComponent<DirectXCommon>();
    auto* cmdList = dxCommon->GetCommandList();
    
    // カメラのビュープロジェクション行列を計算
    Matrix4x4 viewMatrix = camera->GetViewMatrix();
    Matrix4x4 projectionMatrix = camera->GetProjectionMatrix();
    Matrix4x4 viewProjectionMatrix = MathCore::Matrix::Multiply(viewMatrix, projectionMatrix);
    
    // ワールド行列を取得
    Matrix4x4 worldMatrix = transform_.GetWorldMatrix();
    
    // WVP行列を計算
    transformData_->WVP = MathCore::Matrix::Multiply(worldMatrix, viewProjectionMatrix);
    transformData_->World = worldMatrix;
    transformData_->WorldInversTranspose = MathCore::Matrix::Transpose(MathCore::Matrix::Inverse(worldMatrix));
    
    // カメラ位置を設定
    cameraData_->worldPosition = camera->GetPosition();
    
    // レンダラーのパスを開始
    renderer_->SetCamera(camera);
    renderer_->BeginPass(cmdList, BlendMode::kBlendModeNone);
    
    // マテリアルを設定
    cmdList->SetGraphicsRootConstantBufferView(YoYoRendererRootParam::kMaterial, 
                                                materialResource_->GetGPUVirtualAddress());
    
    // トランスフォームを設定
    cmdList->SetGraphicsRootConstantBufferView(YoYoRendererRootParam::kWVP, 
                                                transformResource_->GetGPUVirtualAddress());
    
    // カメラを設定
    cmdList->SetGraphicsRootConstantBufferView(YoYoRendererRootParam::kCamera, 
                                                cameraResource_->GetGPUVirtualAddress());
    
    // 頂点バッファとインデックスバッファを設定
    cmdList->IASetVertexBuffers(0, 1, &geometry_->GetVertexBufferView());
    cmdList->IASetIndexBuffer(&geometry_->GetIndexBufferView());
    
    // 描画
    cmdList->DrawIndexedInstanced(geometry_->GetIndexCount(), 1, 0, 0, 0);
    
    renderer_->EndPass();
}

}
