#pragma once
#include <cstdint>
#include <EngineSystem.h>
class SceneCommandExecutor;

// / メニューの選択肢を表す列挙型
enum class MenuOption {
    ReturnToGame,
    ExitToTitle,
};

/// @brief メニューの開閉を管理するコントローラークラス
class MenuController final {
public:
    MenuController() = delete;
    explicit MenuController(SceneCommandExecutor& sceneCommandList, CoreEngine::SoundManager* soundManager);
    ~MenuController() = default;

    // 初期化
    void Initialize();
    // 開閉を入力を受け付けて更新する
    void Update();
    // メニューが開いているかを取得する
    bool IsMenuOpen() const;
    // 選択されているメニューオプションを取得する
    const MenuOption& GetSelectedOption() const;

    // ゲーム終了要求フラグ
    bool isRequestToExitTitle_ = false;

private:
    float closeTimer_;
    bool isMenuOpen_;
    MenuOption oldSelectedOption_;
    MenuOption selectedOption_;
    SceneCommandExecutor& sceneCommandList_;
    CoreEngine::SoundManager* soundManager_;
    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;
};