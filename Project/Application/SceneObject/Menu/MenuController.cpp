#include "MenuController.h"
#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/Command/SceneCommandExecutor.h"
#include "Application/Utility/Command/SceneAllCommand.h"

#include <memory>

MenuController::MenuController(SceneCommandExecutor& sceneCommandList) :
    sceneCommandList_(sceneCommandList),
    selectedOption_(MenuOption::ReturnToGame) {
    isMenuOpen_ = false;
}

void MenuController::Initialize() {
    isMenuOpen_ = false;
    closeTimer_ = 0.0f;
}

void MenuController::Update() {
    if (closeTimer_ > 0.0f) {
        closeTimer_ -= 1.0f / 60.0f;
        return;
    }

    // メニューの開閉入力を受け付ける
    if (KeyBindConfig::Instance().IsTrigger("Menu")) {
        isMenuOpen_ = !isMenuOpen_;
    }

    // メニューが開いている場合、選択肢の変更を受け付ける
    if (isMenuOpen_) {
        if (KeyBindConfig::Instance().IsTrigger("MoveForward") || KeyBindConfig::Instance().IsTrigger("MoveBack")) {
            // 選択肢を切り替える
            if (selectedOption_ == MenuOption::ReturnToGame) {
                selectedOption_ = MenuOption::ExitToTitle;
            } else {
                selectedOption_ = MenuOption::ReturnToGame;
            }
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

            } else if (selectedOption_ == MenuOption::ExitToTitle) {
                isRequestToExitTitle_ = true;
            }
        }
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
