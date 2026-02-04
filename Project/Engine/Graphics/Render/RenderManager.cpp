#include "RenderManager.h"
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Particle/ParticleSystem.h"
#include "Engine/Graphics/Render/Particle/ParticleRenderer.h"
#include "Engine/Graphics/Render/Particle/ModelParticleRenderer.h"
#include "Engine/Camera/CameraManager.h"
#include "Engine/Camera/ICamera.h"
#include <algorithm>


namespace CoreEngine
{
void RenderManager::Initialize(ID3D12Device* device) {
    // 現時点では特に初期化処理なし
    (void)device; // 未使用警告を回避
}

void RenderManager::RegisterRenderer(RenderPassType type, std::unique_ptr<IRenderer> renderer) {
    renderers_[type] = std::move(renderer);
}

IRenderer* RenderManager::GetRenderer(RenderPassType type) {
    auto it = renderers_.find(type);
    if (it != renderers_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void RenderManager::SetCameraManager(CameraManager* cameraManager) {
    cameraManager_ = cameraManager;
}

void RenderManager::SetCamera(const ICamera* camera) {
    camera_ = camera;
    
    // 各レンダラーにもカメラを設定（互換性維持）
    for (auto& [type, renderer] : renderers_) {
        renderer->SetCamera(camera);
    }
}

void RenderManager::SetCommandList(ID3D12GraphicsCommandList* cmdList) {
    cmdList_ = cmdList;
}

void RenderManager::AddDrawable(GameObject* obj) {
    // GameObjectManagerで事前フィルタリング済み（null/Active/MarkedForDestroyチェック済み）
    DrawCommand cmd;
    cmd.object = obj;
    cmd.passType = obj->GetRenderPassType();
    cmd.blendMode = obj->GetBlendMode();
    cmd.registrationOrder = drawQueue_.size();  // 登録順序を保存

    drawQueue_.push_back(cmd);
}

const ICamera* RenderManager::GetCameraForPass(RenderPassType passType) {
    // カメラマネージャーがある場合はタイプ別のカメラを取得
    if (cameraManager_) {
        // 2D描画パス（Sprite、Text）は2Dカメラを使用
        if (passType == RenderPassType::Sprite || passType == RenderPassType::Text) {
            return cameraManager_->GetActiveCamera(CameraType::Camera2D);
        }
        // その他は3Dカメラを使用
        else {
            return cameraManager_->GetActiveCamera(CameraType::Camera3D);
        }
    }
    
    // カメラマネージャーがない場合は従来のカメラを使用
    return camera_;
}

void RenderManager::DrawAll() {
    if (drawQueue_.empty()) {
        #ifdef _DEBUG
            // 描画キューが空の場合は警告を出力（通常は問題ないが、意図しない場合に気づくため）
            static bool firstWarning = true;
            if (firstWarning) {
                OutputDebugStringA("INFO: RenderManager draw queue is empty.\n");
                firstWarning = false;
            }
        #endif
        return;
    }
    
    if (!cmdList_) {
        #ifdef _DEBUG
            OutputDebugStringA("ERROR: CommandList is null in RenderManager::DrawAll!\n");
        #endif
        return;
    }

	SortDrawQueue();

#ifdef _DEBUG
	// デバッグ用：描画キューの内容をログ出力
	static int frameCount = 0;
	frameCount++;
	if (frameCount % 60 == 0) { // 60フレームに1回だけログ出力
		char buffer[256];
		sprintf_s(buffer, "=== DrawQueue (Frame %d) ===\n", frameCount);
		OutputDebugStringA(buffer);
		for (size_t i = 0; i < drawQueue_.size(); ++i) {
			sprintf_s(buffer, "  [%zu] PassType=%d, BlendMode=%d\n", 
				i, static_cast<int>(drawQueue_[i].passType), static_cast<int>(drawQueue_[i].blendMode));
			OutputDebugStringA(buffer);
		}
		OutputDebugStringA("=========================\n");
	}
#endif

	RenderPassType currentPass = RenderPassType::Invalid;
	BlendMode currentBlendMode = BlendMode::kBlendModeNone;
	IRenderer* currentRenderer = nullptr;
	const ICamera* currentCamera = nullptr;

    for (const auto& cmd : drawQueue_) {
        // GameObjectManagerで事前フィルタリング済み
        // 削除マークのみチェック（更新中に削除マークされた可能性があるため）
        if (!cmd.object || cmd.object->IsMarkedForDestroy()) {
            continue;
        }

		// パスが切り替わったら処理
		if (cmd.passType != currentPass) {
			// 前のパスを終了
			if (currentRenderer) {
#ifdef _DEBUG
				char buffer[128];
				sprintf_s(buffer, "[RenderManager] EndPass: PassType=%d\n", static_cast<int>(currentPass));
				OutputDebugStringA(buffer);
#endif
				currentRenderer->EndPass();
			}

            // 新しいパスを開始
            currentPass = cmd.passType;
            currentBlendMode = cmd.blendMode;
            auto it = renderers_.find(currentPass);
            if (it != renderers_.end()) {
                currentRenderer = it->second.get();
                
				// パスに応じたカメラを取得
				currentCamera = GetCameraForPass(currentPass);
				currentRenderer->SetCamera(currentCamera);
				
#ifdef _DEBUG
				char buffer[128];
				sprintf_s(buffer, "[RenderManager] BeginPass: PassType=%d, BlendMode=%d\n", 
					static_cast<int>(currentPass), static_cast<int>(cmd.blendMode));
				OutputDebugStringA(buffer);
#endif
				currentRenderer->BeginPass(cmdList_, cmd.blendMode);
            } else {
                #ifdef _DEBUG
                    OutputDebugStringA("WARNING: Renderer not found for pass type!\n");
                #endif
                currentRenderer = nullptr;
                currentCamera = nullptr;
            }
        }
		// 同じパス内でブレンドモードが切り替わった場合
		else if (cmd.blendMode != currentBlendMode) {
#ifdef _DEBUG
			char buffer[128];
			sprintf_s(buffer, "[RenderManager] BlendMode changed: %d -> %d (PassType=%d)\n", 
				static_cast<int>(currentBlendMode), static_cast<int>(cmd.blendMode), static_cast<int>(currentPass));
			OutputDebugStringA(buffer);
#endif
			currentBlendMode = cmd.blendMode;
			// パスを一旦終了してから再開（状態をリセット）
			if (currentRenderer) {
				currentRenderer->EndPass();
				currentRenderer->BeginPass(cmdList_, cmd.blendMode);
			}
		}

        // オブジェクトを描画
        if (currentRenderer) {
            // パーティクルの場合は、レンダラーに描画コマンド発行を委託
            if (cmd.passType == RenderPassType::Particle) {
                // パーティクルは描画データの準備（Draw）と実際の描画コマンド発行を分けて実行
                cmd.object->Draw(currentCamera);  // データ準備
                if (auto* particleRenderer = static_cast<ParticleRenderer*>(currentRenderer)) {
                    auto* particleSystem = static_cast<ParticleSystem*>(cmd.object);
                    particleRenderer->Draw(particleSystem);  // 描画コマンド発行
                }
            }
            // モデルパーティクルの場合
            else if (cmd.passType == RenderPassType::ModelParticle) {
                // モデルパーティクルも同様に2段階で処理
                cmd.object->Draw(currentCamera);  // データ準備
                if (auto* modelParticleRenderer = static_cast<ModelParticleRenderer*>(currentRenderer)) {
                    auto* particleSystem = static_cast<ParticleSystem*>(cmd.object);
                    modelParticleRenderer->Draw(particleSystem);  // 描画コマンド発行
                }
            }
            // その他のオブジェクト（Sprite、Text、Model等）は通常通り描画
            else {
                cmd.object->Draw(currentCamera);
            }
        }
    }

    // 最後のパスを終了
    if (currentRenderer) {
#ifdef _DEBUG
		char buffer[128];
		sprintf_s(buffer, "[RenderManager] Final EndPass: PassType=%d\n", static_cast<int>(currentPass));
		OutputDebugStringA(buffer);
#endif
        currentRenderer->EndPass();
    }
}

void RenderManager::ClearQueue() {
    drawQueue_.clear();
}

void RenderManager::SortDrawQueue() {
    // 描画コマンドを最適化してステート変更を最小化
    // 優先順位: 1. パスタイプ > 2. ブレンドモード > 3. 登録順序
    std::stable_sort(drawQueue_.begin(), drawQueue_.end(),
        [](const DrawCommand& a, const DrawCommand& b) {
            // 1. パスタイプでソート（パイプライン切り替え最小化）
            if (a.passType != b.passType) {
                return static_cast<int>(a.passType) < static_cast<int>(b.passType);
            }
            
            // 2. 同一パス内ではブレンドモードでソート（ブレンドステート切り替え最小化）
            if (a.blendMode != b.blendMode) {
                return static_cast<int>(a.blendMode) < static_cast<int>(b.blendMode);
            }
            
            // 3. 同一パス・同一ブレンドモード内では登録順序を維持
            return a.registrationOrder < b.registrationOrder;
        });
}
}
