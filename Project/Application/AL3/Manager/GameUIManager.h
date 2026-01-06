#pragma once

#include <string>
#include <functional>

// 前方宣言
class EngineSystem;
class PlayerObject;
class BossObject;
class ReticleObject;
class SpriteObject;

/// @brief ゲーム内UIを管理するクラス
class GameUIManager {
public:
	/// @brief 初期化
	/// @param engine エンジンシステム
	/// @param player プレイヤー
	/// @param boss ボス
	/// @param reticle レティクル
	void Initialize(
		EngineSystem* engine,
		PlayerObject* player,
		BossObject* boss,
		ReticleObject* reticle
	);

	/// @brief ボス名画像の初期化
	/// @param createSpriteCallback スプライト生成コールバック
	void InitializeBossName(std::function<SpriteObject*(const std::string&, const std::string&)> createSpriteCallback);

	/// @brief ボスHPバーの初期化
	/// @param createSpriteCallback スプライト生成コールバック
	void InitializeBossHPBar(std::function<SpriteObject*(const std::string&, const std::string&)> createSpriteCallback);

	/// @brief 更新
	void Update();

private:
	EngineSystem* engine_ = nullptr;
	PlayerObject* player_ = nullptr;
	BossObject* boss_ = nullptr;
	ReticleObject* reticle_ = nullptr;

	// ボス名画像
	SpriteObject* bossNameSprite_ = nullptr;

	// ボスHPバー（背景、遅延、前景）
	SpriteObject* bossHPBarBackground_ = nullptr;  // HP背景（グレー）
	SpriteObject* bossHPBarDelay_ = nullptr;       // HP遅延バー（赤・ダメージ表示用）
	SpriteObject* bossHPBarForeground_ = nullptr;  // HP前景（緑）

	// HPバーのサイズと位置の定数
	static constexpr float HP_BAR_WIDTH = 600.0f;   // HPバー最大幅（800 → 600に縮小）
	static constexpr float HP_BAR_HEIGHT = 16.0f;   // HPバーの高さ（20 → 16に縮小）
	static constexpr float HP_BAR_Y = 280.0f;       // HPバーのY座標（ボス名の下）

	// 遅延HPバー用の変数
	float delayedHPRatio_ = 1.0f;                   // 遅延HPの割合（0.0〜1.0）
	static constexpr float HP_DELAY_SPEED = 0.15f;  // HP減少の遅延速度（0.3 → 0.15に変更、より遅く約6-7秒）
};
