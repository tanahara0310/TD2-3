#include "MapManager.h"
#include "EngineSystem.h"
#include "Engine/Graphics/Model/ModelManager.h"
#include "Engine/Graphics/Common/DirectXCommon.h"

MapManager::MapManager() {
	// 共有モデルの管理は不要になりました（MapTileが個別に持つため）
}

void MapManager::Initialize(uint32_t width, uint32_t height) {
	width_ = width;
	height_ = height;

	// グリッドのリサイズ
	tiles_.resize(height_);
	for (auto& row : tiles_) {
		row.resize(width_);
	}

	auto engine = GetEngineSystem();
	auto dxCommon = engine->GetComponent<DirectXCommon>();
	if (!dxCommon) return;

	// タイルの初期化
	float spacing = 1.0f; 

	// 中央揃えのためのオフセット計算
	float offsetX = (width_ - 1) * spacing * 0.5f;
	float offsetZ = (height_ - 1) * spacing * 0.5f;

	for (uint32_t z = 0; z < height_; ++z) {
		for (uint32_t x = 0; x < width_; ++x) {
			auto tile = std::make_unique<MapTile>();
			// MapTileはコンストラクタでモデル生成などの初期化を行う

			// 座標計算 (X-Z平面)
			Vector3 position = {
				static_cast<float>(x) * spacing - offsetX,
				-1.0f, // 足元に配置
				static_cast<float>(z) * spacing - offsetZ
			};

			// 初期化（位置設定）
			tile->Initialize(position, { 1.0f, 1.0f, 1.0f });

			tiles_[z][x] = std::move(tile);
		}
	}
}

void MapManager::Update() {
	for (auto& row : tiles_) {
		for (auto& tile : row) {
			if (tile) {
				tile->Update();
			}
		}
	}
}

void MapManager::Draw(const ICamera* camera) {
	for (auto& row : tiles_) {
		for (auto& tile : row) {
			if (tile) {
				// 各タイルに描画を委譲
				tile->Draw(camera);
			}
		}
	}
}

MapTile* MapManager::GetTile(uint32_t x, uint32_t y) {
	if (y >= height_ || x >= width_) {
		return nullptr;
	}
	return tiles_[y][x].get();
}
