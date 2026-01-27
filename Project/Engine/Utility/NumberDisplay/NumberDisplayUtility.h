#pragma once

#include "Engine/ObjectCommon/SpriteObject.h"
#include "Engine/Math/Vector/Vector2.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>

/// @brief 数字テクスチャを表示するユーティリティクラス
namespace CoreEngine
{
    class NumberDisplayUtility {
    public:
        /// @brief コンストラクタ
        NumberDisplayUtility();
        
        /// @brief デストラクタ
        ~NumberDisplayUtility() = default;
        
        /// @brief 初期化
        /// @param createObjectFunc CreateObject関数（BaseSceneから渡す）
        /// @param textureBasePath 数字テクスチャのベースパス（例: "Texture/Number/"）
        /// @param maxDigits 最大桁数（デフォルト: 8桁）
        void Initialize(std::function<SpriteObject*()> createObjectFunc, const std::string& textureBasePath = "Texture/Number/", int maxDigits = 8);
        
        /// @brief 数字を表示
        /// @param number 表示する数値（負の数にも対応）
        /// @param position 表示開始位置（左端の座標）
        /// @param scale 各数字のスケール
        /// @param spacing 数字間の間隔（ピクセル単位）
        void DisplayNumber(int number, const Vector2& position, const Vector2& scale = { 1.0f, 1.0f }, float spacing = 0.0f);
        
        /// @brief 数字を表示（桁数指定版 - ゼロ埋め）
        /// @param number 表示する数値
        /// @param position 表示開始位置（左端の座標）
        /// @param digits 表示桁数（足りない場合は0で埋める）
        /// @param scale 各数字のスケール
        /// @param spacing 数字間の間隔（ピクセル単位）
        void DisplayNumberWithDigits(int number, const Vector2& position, int digits, const Vector2& scale = { 1.0f, 1.0f }, float spacing = 0.0f);
        
        /// @brief 数字を中央揃えで表示
        /// @param number 表示する数値
        /// @param centerPosition 中央位置
        /// @param scale 各数字のスケール
        /// @param spacing 数字間の間隔（ピクセル単位）
        void DisplayNumberCentered(int number, const Vector2& centerPosition, const Vector2& scale = { 1.0f, 1.0f }, float spacing = 0.0f);
        
        /// @brief 数字を右揃えで表示
        /// @param number 表示する数値
        /// @param rightPosition 右端位置
        /// @param scale 各数字のスケール
        /// @param spacing 数字間の間隔（ピクセル単位）
        void DisplayNumberRightAligned(int number, const Vector2& rightPosition, const Vector2& scale = { 1.0f, 1.0f }, float spacing = 0.0f);
        
        /// @brief 更新（必要に応じてスプライトを更新）
        void Update();
        
        /// @brief 色を設定（すべての数字に適用）
        /// @param color 設定する色
        void SetColor(const Vector4& color);
        
        /// @brief アルファ値を設定（すべての数字に適用）
        /// @param alpha アルファ値（0.0f - 1.0f）
        void SetAlpha(float alpha);
        
        /// @brief 表示/非表示を設定
        /// @param visible 表示フラグ
        void SetVisible(bool visible);
        
        /// @brief 数字テクスチャのサイズを取得（ピクセル単位）
        /// @return テクスチャサイズ
        Vector2 GetDigitSize() const { return digitSize_; }
        
        /// @brief スプライトオブジェクトの配列を取得（直接アクセス用）
        /// @return スプライトオブジェクトの参照
        std::vector<SpriteObject*>& GetSprites() { return digitSprites_; }
        
    private:
        /// @brief 数値を桁ごとに分解
        /// @param number 分解する数値
        /// @return 各桁の数字の配列（左から右へ）
        std::vector<int> SplitNumberIntoDigits(int number) const;
        
        /// @brief 実際に数字を配置する内部関数
        /// @param digits 各桁の数字配列
        /// @param startPosition 開始位置
        /// @param scale スケール
        /// @param spacing 間隔
        void ArrangeDigits(const std::vector<int>& digits, const Vector2& startPosition, const Vector2& scale, float spacing);
        
    private:
        std::string textureBasePath_;                          // テクスチャのベースパス
        std::vector<SpriteObject*> digitSprites_;              // 数字スプライトの配列（GameObjectManagerが管理）
        int maxDigits_;                                        // 最大桁数
        Vector2 digitSize_;                                    // 1つの数字のサイズ（ピクセル）
        Vector4 currentColor_;                                 // 現在の色
        bool isVisible_;                                       // 表示フラグ
    };
}
