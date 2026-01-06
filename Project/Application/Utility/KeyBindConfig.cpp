#include "KeyBindConfig.h"
#include "Input/InputManager.h"
#include "Input/GamepadInput.h"
#include "Input/KeyboardInput.h"

#include "EngineSystem/EngineSystem.h"
#include "Input/InputManager.h"

KeyBindConfig::KeyBindConfig() {
	keyBinds_["MoveLeft"].keyboardKeys.push_back(DIK_A);
	keyBinds_["MoveRight"].keyboardKeys.push_back(DIK_D);
	keyBinds_["MoveForward"].keyboardKeys.push_back(DIK_W);
	keyBinds_["MoveBack"].keyboardKeys.push_back(DIK_S);
	keyBinds_["MoveLeft"].keyboardKeys.push_back(DIK_LEFT);
	keyBinds_["MoveRight"].keyboardKeys.push_back(DIK_RIGHT);
	keyBinds_["MoveForward"].keyboardKeys.push_back(DIK_UP);
	keyBinds_["MoveBack"].keyboardKeys.push_back(DIK_DOWN);
	keyBinds_["Menu"].keyboardKeys.push_back(DIK_ESCAPE);
	keyBinds_["Menu"].keyboardKeys.push_back(DIK_E);

	keyBinds_["MoveLeft"].controllerButtons.push_back(XINPUT_GAMEPAD_DPAD_LEFT);
	keyBinds_["MoveRight"].controllerButtons.push_back(XINPUT_GAMEPAD_DPAD_RIGHT);
	keyBinds_["MoveForward"].controllerButtons.push_back(XINPUT_GAMEPAD_DPAD_UP);
	keyBinds_["MoveBack"].controllerButtons.push_back(XINPUT_GAMEPAD_DPAD_DOWN);
	keyBinds_["Shot"].controllerButtons.push_back(XINPUT_GAMEPAD_A);
	keyBinds_["Shot"].controllerButtons.push_back(XINPUT_GAMEPAD_X);
	keyBinds_["Menu"].controllerButtons.push_back(XINPUT_GAMEPAD_START);
	keyBinds_["MenuExit"].controllerButtons.push_back(XINPUT_GAMEPAD_B);
	keyBinds_["MenuExit"].controllerButtons.push_back(XINPUT_GAMEPAD_Y);
}

void KeyBindConfig::Initialize(EngineSystem* enginesys) {
    engineSystem_ = enginesys;
    inputManager_ = engineSystem_->GetComponent<InputManager>();
}

void KeyBindConfig::Update() {
	bool isControll = false;
	for (auto& [actionName, inputDevice] : keyBinds_) {
		isControll |= IsPress(actionName);
	}
}

bool KeyBindConfig::IsPress(const std::string& actionName) {
	for (const auto& keyBind : keyBinds_[actionName].keyboardKeys) {
        if (inputManager_->GetKeyboard()->IsKeyPressed(keyBind)) {
			return true;
		}
	}
	for (const auto& buttonBind : keyBinds_[actionName].controllerButtons) {
        GamepadButton button = static_cast<GamepadButton>(buttonBind);
		if (inputManager_->GetGamepad()->IsButtonPressed(button)) {
			return true;
		}
	}
	return false;
}

bool KeyBindConfig::IsTrigger(const std::string& actionName) {
	for (const auto& keyBind : keyBinds_[actionName].keyboardKeys) {
		if (inputManager_->GetKeyboard()->IsKeyTriggered(keyBind)) {
			return true;
		}
	}
	for (const auto& buttonBind : keyBinds_[actionName].controllerButtons) {
        GamepadButton button = static_cast<GamepadButton>(buttonBind);
        if (inputManager_->GetGamepad()->IsButtonTriggered(button)) {
            return true;
        }
	}
    return false;
}

bool KeyBindConfig::IsRelease(const std::string& actionName) {
	for (const auto& keyBind : keyBinds_[actionName].keyboardKeys) {
		if (inputManager_->GetKeyboard()->IsKeyReleased(keyBind)) {
			return true;
		}
	}
    for (const auto& buttonBind : keyBinds_[actionName].controllerButtons) {
        GamepadButton button = static_cast<GamepadButton>(buttonBind);
        if (inputManager_->GetGamepad()->IsButtonReleased(button)) {
            return true;
        }
    }
	return false;
}
