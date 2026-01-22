#include "MenuView.h"
#include "MenuController.h"
#include "Application//Utility/MatsumotoUtility.h"

namespace {
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;
}

MenuView::MenuView(CoreEngine::BaseScene* baseScene, MenuController* controller) :
    menuController_(controller),
    baseScene_(baseScene) {

    bgSprite_ = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    gameReturnSprite_ = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    gameExitSprite_ = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    selectorSprite_ = baseScene_->CreateObject<CoreEngine::SpriteObject>();

    config_.emplace("ScreenWidth", screenWidth);
    config_.emplace("ScreenHeight", screenHeight);
    config_.emplace("MenuOpenSpeed", 0.3f);
    config_.emplace("MenuCloseSpeed", 0.4f);
}

void MenuView::Initialize() {
    if (!bgSprite_) {
        return;
    }
    bgSprite_->Initialize("Texture/menuUiBg.png","MenuBG");
    gameReturnSprite_->Initialize("Texture/menuUiGame.png", "MenuReturn");
    gameExitSprite_->Initialize("Texture/menuUiTitle.png", "MenuExit");
    selectorSprite_->Initialize("Texture/menuUiSelector.png", "MenuSelector");

    gameExitOffset_ = { 400.0f, -200.0f, 0.0f };
    gameReturnOffset_ = { 400.0, 0.0f, 0.0f };
    selectorOffset_ = { -250.0f, 0.0f, 0.0f };

    bgSprite_->GetSpriteTransform().translate = { -screenWidth, 0.0f, 0.0f };
    gameExitSprite_->GetSpriteTransform().translate = gameExitOffset_;
    gameReturnSprite_->GetSpriteTransform().translate = gameReturnOffset_;
    selectorSprite_->GetSpriteTransform().translate = gameReturnOffset_ + selectorOffset_;
}

void MenuView::Update() {
    if (!bgSprite_ || !menuController_) {
        return;
    }

    // メニュー表示中かどうかで処理を分岐
    if (menuController_->IsMenuOpen()) {
        // メニュー表示中
        bgSprite_->GetSpriteTransform().translate.x = MatsumotoUtility::SimpleEaseIn(
            bgSprite_->GetSpriteTransform().translate.x,
            0.0f,
            config_["MenuCloseSpeed"].get<float>()
        );

    } else {
        // メニュー非表示中
        bgSprite_->GetSpriteTransform().translate.x = MatsumotoUtility::SimpleEaseIn(
            bgSprite_->GetSpriteTransform().translate.x,
            screenWidth*2.0f,
            config_["MenuOpenSpeed"].get<float>()
        );
    }
    // メニュー項目の位置更新
    CoreEngine::Vector3 bgPos = bgSprite_->GetSpriteTransform().translate;
    gameReturnSprite_->GetSpriteTransform().translate = bgPos + gameReturnOffset_;
    gameExitSprite_->GetSpriteTransform().translate = bgPos + gameExitOffset_;
    // セレクターの位置更新
    selectorSprite_->GetSpriteTransform().translate = bgPos +
        (menuController_->GetSelectedOption() == MenuOption::ReturnToGame ?
            gameReturnOffset_ : gameExitOffset_) + selectorOffset_;
}

