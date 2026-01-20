#pragma once

/// @brief メニューの開閉を管理するコントローラークラス
class MenuController final {
public:
    MenuController() = default;
    ~MenuController() = default;

    // 初期化
    void Initialize();
    // 開閉を入力を受け付けて更新する
    void Update();
    // メニューが開いているかを取得する
    bool IsMenuOpen() const;

private:
    bool isMenuOpen_;
};