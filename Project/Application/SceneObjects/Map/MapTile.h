#pragma once
#include "Engine/ObjectCommon/GameObject.h"

// マップタイルクラス
class MapTile : public GameObject {
public:
	MapTile();
	~MapTile() override = default;

	// 初期化
	void Initialize(const Vector3& position, const Vector3& scale = { 1.0f, 1.0f, 1.0f });

	// 更新
	void Update() override;

	// 描画
	void Draw(const ICamera* camera) override;

#ifdef _DEBUG
	const char* GetObjectName() const override { return "MapTile"; }
#endif

private:

};