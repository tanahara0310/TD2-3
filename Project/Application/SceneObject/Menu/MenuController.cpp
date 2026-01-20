#include "MenuController.h"
#include "Application/Utility/KeyBindConfig.h"

void MenuController::Initialize() {
    isMenuOpen_ = false;
}

void MenuController::Update() {
    if (KeyBindConfig::Instance().IsTrigger("Menu")) {
        isMenuOpen_ = !isMenuOpen_;
    }
}

bool MenuController::IsMenuOpen() const {
    return isMenuOpen_;
}
