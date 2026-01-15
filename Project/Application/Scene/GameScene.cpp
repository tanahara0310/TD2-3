#include "GameScene.h"
#include "Scene/SceneManager.h"

#include "Application/SceneObjects/Player/Player.h"
#include "Application/SceneObjects/CameraController/AllCameraWork.h"

#include<imgui.h>

GameScene::GameScene() {
    player_ = CreateObject<Player>();
    mapManager_ = CreateObject<MapManager>();
    defaultCameraPos_ = { 0.0f, 15.0f, -15.0f };
}

void GameScene::Initialize(EngineSystem* engine) {
    BaseScene::Initialize(engine);

    // マップの初期化（10x10）
    mapManager_->Initialize(10, 10);

    // カメラコントローラーの初期化
    cameraController_.Initialize();
    cameraController_.SetCameraWork<FollowCamera>(*(cameraManager_.get()), player_->GetTransform(), defaultCameraPos_, 0.1f);
    // ビートスケジューラーの初期化（4拍子）
    beatScheduler_.Initialize(4);
}

void GameScene::Update() {
    cameraController_.Update();
    beatScheduler_.Update();
    BaseScene::Update();
}

void GameScene::Draw() {
#ifdef _DEBUG
    ImGui::Begin("GameApp Info");
    // BeatSchedulerの情報表示
    if (ImGui::CollapsingHeader("BeatScheduler Info"))
    {
        // 再生/一時停止ボタン
        if (ImGui::Button(beatScheduler_.IsPlaying() ? "Pause" : "Resume")) {
            if (beatScheduler_.IsPlaying()) {
                beatScheduler_.Pause();
            } else {
                beatScheduler_.Resume();
            }
        }
        // 現在の小節情報
        const Measure& currentMeasure = beatScheduler_.GetCurrentMeasure();
        ImGui::Text("Current Measure Index: %zu", beatScheduler_.GetCurrentMeasureIndex());
        ImGui::Text("Current Measure Beats: %d", currentMeasure.GetBeatsPerMeasure());
        ImGui::Text("Current Measure Duration: %.2f sec", beatScheduler_.GetCurrentMeasureDuration());
        ImGui::Text("Current BPM: %.2f", beatScheduler_.GetCurrentBPM());
        // 小節進行度のプログレスバー
        const int beats = currentMeasure.GetBeatsPerMeasure();
        const float progress = beatScheduler_.GetCurrentMeasurePosition();
        ImVec2 barSize = ImVec2(0.0f, 0.0f);
        ImGui::ProgressBar(progress, barSize, "Measure Progress");

        // ProgressBarの矩形を取得
        ImVec2 barMin = ImGui::GetItemRectMin();
        ImVec2 barMax = ImGui::GetItemRectMax();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // 拍数分グリッド線を描画
        for (int i = 1; i < beats; ++i) {
            float t = (float)i / beats;
            float x = barMin.x + (barMax.x - barMin.x) * t;
            drawList->AddLine(
                ImVec2(x, barMin.y),
                ImVec2(x, barMax.y),
                IM_COL32(200, 200, 200, 255), // 薄いグレー
                1.0f
            );
        }
        // 小節の拍の位置に赤い線を描画
        for (int i = 0; i < beats; ++i) {
            float beatPos = currentMeasure.GetBeatPosition(i);
            float x = barMin.x + (barMax.x - barMin.x) * beatPos;
            drawList->AddLine(
                ImVec2(x, barMin.y),
                ImVec2(x, barMax.y),
                IM_COL32(255, 0, 0, 255), // 赤色
                2.0f
            );
        }
        // 区切り線
        ImGui::Separator();
        // 次の小節情報
        const Measure& nextMeasure = beatScheduler_.GetNextMeasure();
        ImGui::Text("Next Measure Beats: %d", nextMeasure.GetBeatsPerMeasure());
        // 小節キューの情報
        const auto& measureQueue = beatScheduler_.GetMeasureQueue();
        ImGui::Text("Measure Queue Size: %zu", measureQueue.size());
        size_t index = 0;
        for (size_t i = 0; i < index; i++) {
            const auto& measure = measureQueue.front();
            ImGui::Text("  [%zu] Beats: %d, Duration: %.2f sec", index, measure.first, measure.second);
            index++;
        }
    }
    ImGui::End();
#endif // _DEBUG

        BaseScene::Draw();
}

void GameScene::Finalize() {
    BaseScene::Finalize();
}
