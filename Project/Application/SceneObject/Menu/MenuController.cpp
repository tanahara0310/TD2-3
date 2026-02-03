#include "MenuController.h"
#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/Command/SceneCommandExecutor.h"
#include "Application/Utility/Command/SceneAllCommand.h"

#include <memory>

MenuController::MenuController(SceneCommandExecutor& sceneCommandList, CoreEngine::SoundManager* soundManager) :
    sceneCommandList_(sceneCommandList),
    soundManager_(soundManager),
    selectedOption_(MenuOption::ReturnToGame) {
    isMenuOpen_ = false;
}

void MenuController::Initialize() {
    isMenuOpen_ = false;
    closeTimer_ = 0.0f;
    oldSelectedOption_ = MenuOption::ReturnToGame;

    // 効果音リソースの作成
    if (soundManager_) {
        soundResources_["MenuOpen"] = soundManager_->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_Menu.mp3");
        soundResources_["MenuClose"] = soundManager_->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_Menu.mp3");
        soundResources_["MenuSelect"] = soundManager_->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_Throw.mp3");
        soundResources_["MenuDecide"] = soundManager_->CreateSoundResource("Assets/ApplicationAssets/Sound/Title/SE_Decide.mp3");
    }
}

void MenuController::Update() {
    if (closeTimer_ > 0.0f) {
        closeTimer_ -= 1.0f / 60.0f;
        return;
    }

    // メニューの開閉入力を受け付ける
    if (KeyBindConfig::Instance().IsTrigger("Menu")) {
        isMenuOpen_ = !isMenuOpen_;
        soundResources_["MenuOpen"]->Play(false);
    }

    // メニューが開いている場合、選択肢の変更を受け付ける
    if (isMenuOpen_) {
        if (KeyBindConfig::Instance().GetVerticalAxis() > 0.0f) {
            selectedOption_ = MenuOption::ReturnToGame;
        } else if (KeyBindConfig::Instance().GetVerticalAxis() < 0.0f) {
            selectedOption_ = MenuOption::ExitToTitle;
        }

    } else {
        // メニューが閉じている場合、選択肢をデフォルトに戻す
        if (selectedOption_ != MenuOption::ReturnToGame) {
            selectedOption_ = MenuOption::ReturnToGame;
        }
    }
    // メニュー決定入力を受け付ける
    if (isMenuOpen_) {
        if (KeyBindConfig::Instance().IsTrigger("Start")) {
            // 選択肢に応じた処理を行う
            if (selectedOption_ == MenuOption::ReturnToGame) {
                isMenuOpen_ = false; // ゲームに戻る
                closeTimer_ = 0.1f; // メニュー閉じた直後は少し入力を受け付けない
                soundResources_["MenuDecide"]->Play(false);
            } else if (selectedOption_ == MenuOption::ExitToTitle) {
                isRequestToExitTitle_ = true;
            }
        }
    }

    if (selectedOption_ != oldSelectedOption_) {
        soundResources_["MenuSelect"]->Play(false);
        oldSelectedOption_ = selectedOption_;
    }
}

bool MenuController::IsMenuOpen() const {
    if (closeTimer_ > 0.0f) {
        return true;
    }
    return isMenuOpen_;
}

const MenuOption& MenuController::GetSelectedOption() const {
    return selectedOption_;
}
