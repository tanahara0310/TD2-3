#include "Menu.h"
#include "../../Utility/KeyBindConfig.h"

void Menu::Initialize() {
	isOpened_ = false;
}

void Menu::Update() {
	// メニューキーが押されたらフラグをトグルする
	if (KeyBindConfig::Instance().IsTrigger("Menu")) {
		isOpened_ = !isOpened_;
	}
}
