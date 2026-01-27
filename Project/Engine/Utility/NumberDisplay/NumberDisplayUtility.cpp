#include "NumberDisplayUtility.h"
#include "Engine/Math/Vector/Vector4.h"
#include <cmath>
#include <algorithm>

#undef min
#undef max

namespace CoreEngine
{
    NumberDisplayUtility::NumberDisplayUtility()
        : textureBasePath_("")
        , maxDigits_(8)
        , digitSize_({ 32.0f, 64.0f })
        , currentColor_({ 1.0f, 1.0f, 1.0f, 1.0f })
        , isVisible_(true)
    {
    }

    void NumberDisplayUtility::Initialize(std::function<SpriteObject*()> createObjectFunc, const std::string& textureBasePath, int maxDigits)
    {
        textureBasePath_ = textureBasePath;
        maxDigits_ = maxDigits;
        
        // 最大桁数分のスプライトを事前に作成（CreateObjectを使用）
        digitSprites_.clear();
        digitSprites_.reserve(maxDigits_ + 1); // +1 は負号用
        
        for (int i = 0; i < maxDigits_ + 1; ++i) {
            // CreateObjectを通して生成（GameObjectManagerに自動登録される）
            auto* sprite = createObjectFunc();
            // 初期状態では0のテクスチャを読み込む
            sprite->Initialize(textureBasePath_ + "0.png");
            sprite->SetActive(false); // 初期状態では非表示
            sprite->SetAnchor({ 0.5f, 0.5f }); // 中央揃え

            digitSprites_.push_back(sprite);
        }
        
        // 数字テクスチャのサイズを取得（0のテクスチャから）
        if (!digitSprites_.empty()) {
            digitSize_ = digitSprites_[0]->GetTextureSize();
        }
    }

    void NumberDisplayUtility::DisplayNumber(int number, const Vector2& position, const Vector2& scale, float spacing)
    {
        auto digits = SplitNumberIntoDigits(number);
        ArrangeDigits(digits, position, scale, spacing);
    }

    void NumberDisplayUtility::DisplayNumberWithDigits(int number, const Vector2& position, int digits, const Vector2& scale, float spacing)
    {
        auto digitArray = SplitNumberIntoDigits(number);

        // 指定桁数に満たない場合は0で埋める
        int currentDigits = static_cast<int>(digitArray.size());
        bool hasNegativeSign = number < 0;

        // 負号がある場合は除外してカウント
        if (hasNegativeSign) {
            currentDigits--;
        }

        if (currentDigits < digits) {
            // 不足分の0を追加
            std::vector<int> paddedDigits;
            if (hasNegativeSign) {
                paddedDigits.push_back(digitArray[0]); // 負号
                digitArray.erase(digitArray.begin());
            }

            int zerosToAdd = digits - currentDigits;
            for (int i = 0; i < zerosToAdd; ++i) {
                paddedDigits.push_back(0);
            }

            // 残りの数字を追加
            paddedDigits.insert(paddedDigits.end(), digitArray.begin(), digitArray.end());
            digitArray = paddedDigits;
        }

        ArrangeDigits(digitArray, position, scale, spacing);
    }

    void NumberDisplayUtility::DisplayNumberCentered(int number, const Vector2& centerPosition, const Vector2& scale, float spacing)
    {
        auto digits = SplitNumberIntoDigits(number);

        // 全体の幅を計算
        float totalWidth = 0.0f;
        for (size_t i = 0; i < digits.size(); ++i) {
            totalWidth += digitSize_.x * scale.x;
            if (i < digits.size() - 1) {
                totalWidth += spacing;
            }
        }

        // 開始位置を計算（中央から左に半分ずらす）
        Vector2 startPosition = { centerPosition.x - totalWidth * 0.5f, centerPosition.y };
        ArrangeDigits(digits, startPosition, scale, spacing);
    }

    void NumberDisplayUtility::DisplayNumberRightAligned(int number, const Vector2& rightPosition, const Vector2& scale, float spacing)
    {
        auto digits = SplitNumberIntoDigits(number);

        // 全体の幅を計算
        float totalWidth = 0.0f;
        for (size_t i = 0; i < digits.size(); ++i) {
            totalWidth += digitSize_.x * scale.x;
            if (i < digits.size() - 1) {
                totalWidth += spacing;
            }
        }

        // 開始位置を計算（右端から左にずらす）
        Vector2 startPosition = { rightPosition.x - totalWidth, rightPosition.y };
        ArrangeDigits(digits, startPosition, scale, spacing);
    }

    void NumberDisplayUtility::Update()
    {
        // アクティブなスプライトのみ更新
        for (auto& sprite : digitSprites_) {
            if (sprite->IsActive()) {
                sprite->Update();
            }
        }
    }

    void NumberDisplayUtility::SetColor(const Vector4& color)
    {
        currentColor_ = color;
        for (auto& sprite : digitSprites_) {
            sprite->SetColor(color);
        }
    }

    void NumberDisplayUtility::SetAlpha(float alpha)
    {
        currentColor_.w = std::clamp(alpha, 0.0f, 1.0f);
        for (auto& sprite : digitSprites_) {
            Vector4 color = sprite->GetColor();
            color.w = currentColor_.w;
            sprite->SetColor(color);
        }
    }

    void NumberDisplayUtility::SetVisible(bool visible)
    {
        isVisible_ = visible;
        for (auto& sprite : digitSprites_) {
            if (!visible) {
                sprite->SetActive(false);
            }
        }
    }

    std::vector<int> NumberDisplayUtility::SplitNumberIntoDigits(int number) const
    {
        std::vector<int> digits;

        // 0の場合
        if (number == 0) {
            digits.push_back(0);
            return digits;
        }

        // 負の数の場合
        bool isNegative = number < 0;
        if (isNegative) {
            number = -number;
            digits.push_back(-1); // -1は負号を表す特殊値
        }

        // 数値を桁ごとに分解
        std::vector<int> temp;
        while (number > 0) {
            temp.push_back(number % 10);
            number /= 10;
        }

        // 逆順にして正しい順序に
        for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
            digits.push_back(*it);
        }

        return digits;
    }

    void NumberDisplayUtility::ArrangeDigits(const std::vector<int>& digits, const Vector2& startPosition, const Vector2& scale, float spacing)
    {
        if (!isVisible_) {
            return;
        }

        // すべてのスプライトを一旦非表示に
        for (auto& sprite : digitSprites_) {
            sprite->SetActive(false);
        }

        // 桁数が最大桁数を超える場合は制限
        size_t displayDigits = std::min(digits.size(), static_cast<size_t>(maxDigits_ + 1));

        float currentX = startPosition.x;

        for (size_t i = 0; i < displayDigits; ++i) {
            int digit = digits[i];

            // スプライトを設定
            if (i < digitSprites_.size()) {
                auto& sprite = digitSprites_[i];

                // テクスチャを設定
                std::string texturePath;
                if (digit == -1) {
                    // 負号
                    texturePath = textureBasePath_ + "minus.png";
                } else {
                    texturePath = textureBasePath_ + std::to_string(digit) + ".png";
                }

                sprite->SetTexture(texturePath);
                
                // ImGui表示用に名前を実際の数字に合わせて更新
                if (digit == -1) {
                    sprite->SetName("NumberDigit_Minus");
                } else {
                    sprite->SetName("NumberDigit_" + std::to_string(digit));
                }
                
                auto& transform = sprite->GetSpriteTransform();
                transform.translate = { currentX + (digitSize_.x * scale.x * 0.5f), startPosition.y, 0.0f };
                transform.scale = { scale.x, scale.y, 1.0f };
                sprite->SetColor(currentColor_);
                sprite->SetActive(true);

                // 次の位置へ
                currentX += digitSize_.x * scale.x + spacing;
            }
        }
    }
}

