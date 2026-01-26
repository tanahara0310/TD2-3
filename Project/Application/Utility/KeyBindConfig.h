#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdint.h>

namespace CoreEngine {
    class EngineSystem;
    class InputManager;
}

struct InputDevice {
	std::vector<uint8_t> keyboardKeys;
	std::vector<UINT16> controllerButtons;
};

class KeyBindConfig
{
public:
	static KeyBindConfig& Instance() {
		static KeyBindConfig instance;
		return instance;
	}

	KeyBindConfig(const KeyBindConfig&) = delete;
	KeyBindConfig& operator=(const KeyBindConfig&) = delete;

    void Initialize(CoreEngine::EngineSystem* enginesys);
	void Update();

	bool IsPress(const std::string& actionName);
	bool IsTrigger(const std::string& actionName);
	bool IsRelease(const std::string& actionName);
	float noControlTimer_ = 0.0f;
    float GetHorizontalAxis();
    float GetVerticalAxis();

private:
	std::unordered_map<std::string, InputDevice> keyBinds_;
    KeyBindConfig();
	~KeyBindConfig() = default;
    CoreEngine::EngineSystem* engineSystem_ = nullptr;
    CoreEngine::InputManager* inputManager_ = nullptr;
	
};