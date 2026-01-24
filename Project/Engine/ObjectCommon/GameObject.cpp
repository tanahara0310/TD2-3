#include "GameObject.h"
#include "Engine/Graphics/Model/Model.h"
#include "Application/Utility/MatsumotoUtility.h"
#include <cstdio>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG



namespace CoreEngine
{
	namespace {
		EngineSystem* sEngine = nullptr;
	}

	void GameObject::Initialize(EngineSystem* engine) {
		if (sEngine == nullptr) {
			sEngine = engine;
		}
	}

	EngineSystem* GameObject::GetEngineSystem() const {
		return sEngine;
	}

#ifdef _DEBUG
	bool GameObject::DrawImGui() {
		bool changed = false;

		// オブジェクト名とアドレスを組み合わせた一意のヘッダー
		// 設定された名前がある場合はそれを使用、なければクラス名を使用
		const char* displayName = name_.empty() ? GetObjectName() : name_.c_str();
		char headerLabel[256];
		snprintf(headerLabel, sizeof(headerLabel), "%s##%p", displayName, (void*)this);

		if (ImGui::CollapsingHeader(headerLabel)) {
			ImGui::PushID(this);

			// アクティブ状態
			bool active = isActive_;
			if (ImGui::Checkbox("Active", &active)) {
				isActive_ = active;
				changed = true;
			}

			// 自動更新フラグ
			bool autoUpdate = autoUpdate_;
			if (ImGui::Checkbox("Auto Update", &autoUpdate)) {
				autoUpdate_ = autoUpdate;
				changed = true;
			}

			ImGui::Separator();

			// トランスフォーム
			if (ImGui::TreeNode("Transform")) {
				Vector3& pos = transform_.translate;
				Vector3& rot = transform_.rotate;
				Vector3& scale = transform_.scale;

				changed |= ImGui::DragFloat3("Position", &pos.x, 0.1f);
				changed |= ImGui::DragFloat3("Rotation", &rot.x, 0.01f);
				changed |= ImGui::DragFloat3("Scale", &scale.x, 0.01f);

				ImGui::TreePop();
			}

			// 派生クラスの拡張UI
			changed |= DrawImGuiExtended();

			ImGui::Separator();

			// 設定の保存/読込
			if (ImGui::TreeNode("Config File")) {
				static char configFileName[256] = "config.json";
				ImGui::InputText("File Name", configFileName, sizeof(configFileName));

				if (ImGui::Button("Load Config")) {
					LoadConfigFromFile(configFileName);
					changed = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Save Config")) {
					SaveConfigToFile(configFileName);
				}

				// 現在の設定をJSON形式で表示
				if (ImGui::TreeNode("Current Config (JSON)")) {
					nlohmann::json currentConfig = GetConfig();
					std::string jsonStr = currentConfig.dump(2);
					ImGui::TextWrapped("%s", jsonStr.c_str());
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		return changed;
	}

#endif // _DEBUG

	void GameObject::LoadConfigFromFile(const std::string& fileName) {
		nlohmann::json config;
		MatsumotoUtility::LoadSceneObjectConfig(config, fileName);
		SetConfig(config);
	}

	void GameObject::SaveConfigToFile(const std::string& fileName) {
		nlohmann::json config = GetConfig();
		MatsumotoUtility::SaveSceneObjectConfig(config, fileName);
	}

	void GameObject::SetConfig(const nlohmann::json& config) {
		// Transform情報の読み込み
		if (config.contains("transform")) {
			const auto& transformConfig = config["transform"];
			
			if (transformConfig.contains("position") && transformConfig["position"].is_array() && transformConfig["position"].size() == 3) {
				transform_.translate.x = transformConfig["position"][0];
				transform_.translate.y = transformConfig["position"][1];
				transform_.translate.z = transformConfig["position"][2];
			}
			
			if (transformConfig.contains("rotation") && transformConfig["rotation"].is_array() && transformConfig["rotation"].size() == 3) {
				transform_.rotate.x = transformConfig["rotation"][0];
				transform_.rotate.y = transformConfig["rotation"][1];
				transform_.rotate.z = transformConfig["rotation"][2];
			}
			
			if (transformConfig.contains("scale") && transformConfig["scale"].is_array() && transformConfig["scale"].size() == 3) {
				transform_.scale.x = transformConfig["scale"][0];
				transform_.scale.y = transformConfig["scale"][1];
				transform_.scale.z = transformConfig["scale"][2];
			}
		}

		// 基本情報の読み込み
		if (config.contains("active")) {
			isActive_ = config["active"];
		}
		
		if (config.contains("autoUpdate")) {
			autoUpdate_ = config["autoUpdate"];
		}
		
		if (config.contains("name")) {
			name_ = config["name"].get<std::string>();
		}
	}

	nlohmann::json GameObject::GetConfig() const {
		nlohmann::json config;
		
		// Transform情報の保存
		config["transform"]["position"] = { transform_.translate.x, transform_.translate.y, transform_.translate.z };
		config["transform"]["rotation"] = { transform_.rotate.x, transform_.rotate.y, transform_.rotate.z };
		config["transform"]["scale"] = { transform_.scale.x, transform_.scale.y, transform_.scale.z };
		
		// 基本情報の保存
		config["active"] = isActive_;
		config["autoUpdate"] = autoUpdate_;
		config["name"] = name_;
		
		return config;
	}
}
