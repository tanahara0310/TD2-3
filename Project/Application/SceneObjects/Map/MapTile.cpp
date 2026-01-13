#include "MapTile.h"
#include <EngineSystem.h>
#include "Engine/Graphics/Model/ModelManager.h"

MapTile::MapTile() {
	// エンジンシステム取得
	auto engine = GetEngineSystem();
	auto dxCommon = engine->GetComponent<DirectXCommon>();
	auto modelManager = engine->GetComponent<ModelManager>();

	if (!dxCommon || !modelManager) {
		return;
	}

	// モデル作成（各タイルが所有）
	model_ = modelManager->CreateStaticModel("ApplicationAssets/Model/Box1x1.obj");
	if (model_) {
		model_->SetMaterialColor({ 0.5f, 0.5f, 0.5f, 1.0f }); // グレー
	}

	// トランスフォーム初期化
	transform_.Initialize(dxCommon->GetDevice());

	// テクスチャ読み込み（白画像を使用）
	auto& textureManager = TextureManager::GetInstance();
	texture_ = textureManager.Load("Texture/white1x1.png");

	SetActive(true);
}

void MapTile::Initialize(const Vector3& position, const Vector3& scale) {
	transform_.translate = position;
	transform_.scale = scale;
	transform_.TransferMatrix();
}

void MapTile::Update() {
	if (!IsActive()) return;

	transform_.TransferMatrix();
}

void MapTile::Draw(const ICamera* camera) {
	if (!IsActive() || !model_) return;

	model_->Draw(transform_, camera, texture_.gpuHandle);
}
