#include "GameObjectManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include <algorithm>

#ifdef _DEBUG
#include <imgui.h>
#endif

void GameObjectManager::UpdateAll() {
	// コピーを作成してから更新（ループ中の配列変更に対応）
	std::vector<GameObject*> objectsToUpdate;
	objectsToUpdate.reserve(objects_.size());
	
	for (auto& obj : objects_) {
		if (obj && obj.get() != nullptr && obj->IsActive() && !obj->IsMarkedForDestroy()) {
			objectsToUpdate.push_back(obj.get());
		}
	}
	
	// コピーしたリストを使って更新
	for (auto* obj : objectsToUpdate) {
		if (obj && !obj->IsMarkedForDestroy()) {
			obj->Update();
		}
	}
}

void GameObjectManager::RegisterAllToRender(RenderManager* renderManager) {
	if (!renderManager) return;

	// コピーを作成してから登録
	std::vector<GameObject*> objectsToRender;
	objectsToRender.reserve(objects_.size());
	
	for (auto& obj : objects_) {
		if (obj && obj.get() != nullptr && obj->IsActive() && !obj->IsMarkedForDestroy()) {
			objectsToRender.push_back(obj.get());
		}
	}
	
	// コピーしたリストを使って登録
	for (auto* obj : objectsToRender) {
		if (obj && !obj->IsMarkedForDestroy()) {
			renderManager->AddDrawable(obj);
		}
	}
}

void GameObjectManager::CleanupDestroyed() {
	// 前フレームの削除キューをクリア（デストラクタ呼び出し）
	destroyQueue_.clear();

	// 削除マークされたオブジェクトを検出して削除キューに移動
	// 逆順でループして削除時のインデックスずれを防ぐ
	for (int i = static_cast<int>(objects_.size()) - 1; i >= 0; --i) {
		auto& obj = objects_[i];
		
		// unique_ptrが無効またはポインタがnullptrの場合は削除
		if (!obj || obj.get() == nullptr) {
			objects_.erase(objects_.begin() + i);
			continue;
		}
		
		if (obj->IsMarkedForDestroy()) {
			// 削除マークされたオブジェクトは削除キューに移動
			destroyQueue_.push_back(std::move(obj));
			objects_.erase(objects_.begin() + i);
		}
	}

	// destroyQueue_ は次のフレームの CleanupDestroyed() 呼び出しまで保持され、
	// その時にクリアされてデストラクタが呼ばれる（1フレーム遅延）
}

void GameObjectManager::Clear() {
	objects_.clear();
	destroyQueue_.clear();
}

#ifdef _DEBUG
void GameObjectManager::DrawAllImGui() {
	ImGui::Text("Total Objects: %zu", objects_.size());
	ImGui::Text("Destroy Queue: %zu", destroyQueue_.size());
	ImGui::Separator();

	// コピーを作成してから表示
	std::vector<GameObject*> objectsToShow;
	objectsToShow.reserve(objects_.size());
	
	for (auto& obj : objects_) {
		if (obj && obj.get() != nullptr) {
			objectsToShow.push_back(obj.get());
		}
	}
	
	// コピーしたリストを使って表示
	for (auto* obj : objectsToShow) {
		if (obj) {
			// 削除マークされているオブジェクトは赤で表示
			if (obj->IsMarkedForDestroy()) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
			}

			obj->DrawImGui();

			if (obj->IsMarkedForDestroy()) {
				ImGui::PopStyleColor();
			}
		}
	}
}
#endif
