#pragma once
#include "Engine/Scene/BaseScene.h"
#include "ObjectCommon/SpriteObject.h"
#include <memory>
#include <externals/nlohmann/single_include/nlohmann/json.hpp>

class MenuController;

// メニューの表示を担当するクラス
class MenuView final {
public:
    ~MenuView() = default;
    MenuView() = delete;
    explicit MenuView(CoreEngine::BaseScene* baseScene,MenuController* controller);

    void Initialize();
    void Update();

private:
    const MenuController* menuController_;
    CoreEngine::BaseScene* baseScene_;

    CoreEngine::SpriteObject* bgSprite_;
    CoreEngine::SpriteObject* gameReturnSprite_;
    CoreEngine::SpriteObject* gameExitSprite_;
    CoreEngine::SpriteObject* selectorSprite_;

    nlohmann::json config_;
    CoreEngine::Vector3 gameReturnOffset_;
    CoreEngine::Vector3 gameExitOffset_;
    CoreEngine::Vector3 selectorOffset_;
};