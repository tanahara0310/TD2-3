#include "BulletModel.h"
#include <EngineSystem.h>

BulletModel::BulletModel(const std::string& modelPath, const std::string& texturePath) {
    // 必須コンポーネントの取得
    auto engine = GetEngineSystem();

    auto dxCommon = engine->GetComponent<DirectXCommon>();
    auto modelManager = engine->GetComponent<ModelManager>();

    if (!dxCommon || !modelManager) {
        return;
    }

    // 静的モデルとして作成
    model_ = modelManager->CreateStaticModel(modelPath);
    model_->SetMaterialColor({ 0.0f, 1.0f, 0.0f, 1.0f });

    // トランスフォームの初期化
    transform_.Initialize(dxCommon->GetDevice());

    // テクスチャの読み込み
    auto& textureManager = TextureManager::GetInstance();
    texture_ = textureManager.Load(texturePath);

    // アクティブ状態に設定
    SetActive(false);
}
