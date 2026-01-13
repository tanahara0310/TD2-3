#pragma once
#include "MapTile.h"
#include "Engine/ObjectCommon/GameObject.h"
#include <vector>
#include <memory>

class MapManager : public GameObject {
public:
	MapManager();
	~MapManager() override = default;

	// 初期化
	// width, height: グリッドのサイズ（ブロック数）
	// tileSize: タイル1つの大きさ
	void Initialize(uint32_t width, uint32_t height);

	// 更新
	void Update() override;

	// 描画
	void Draw(const ICamera* camera) override;

	// タイル取得（安全なアクセッサ）
	MapTile* GetTile(uint32_t x, uint32_t y);

#ifdef _DEBUG
	const char* GetObjectName() const override { return "MapManager"; }
#endif

private:
	// タイルグリッド
	std::vector<std::vector<std::unique_ptr<MapTile>>> tiles_;
	uint32_t width_ = 0;
	uint32_t height_ = 0;
};
