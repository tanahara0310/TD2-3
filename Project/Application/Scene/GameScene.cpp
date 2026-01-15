#include "GameScene.h"
#include "Scene/SceneManager.h"

#include "Application/SceneObjects/Player/Player.h"
#include "Application/SceneObjects/Companion/Companion.h"
#include "Application/SceneObjects/CameraController/AllCameraWork.h"

#include "Application/SceneObjects/Companion/RhythmSpawnCompanion.h"

#include "Application/SceneObjects/Ui/BeatUi.h"

#include<imgui.h>

GameScene::GameScene() {
    // このシーンで使うスプライトオブジェクトの作成
    spriteObjects_.clear();
    spriteObjects_["BeatUi"].push_back(CreateObject<SpriteObject>());
    spriteObjects_["MeasureBar"].push_back(CreateObject<SpriteObject>());
    for (int i = 0; i < 16; ++i) {
        spriteObjects_["Note"].push_back(CreateObject<SpriteObject>());
    }

    // プレイヤーオブジェクトの作成
    player_ = CreateObject<Player>();
    // 弾オブジェクトコンテナの作成
    bulletObjectContainers_.clear();
    // コンパニオンの作成10体
    bulletObjectContainers_["Companion"] = std::make_unique<BulletObjectContainer>(10);
    bulletObjectContainers_["Companion"]->ApplyToScene<Companion>(
        this, "ApplicationAssets/Model/Box1x1.obj", "Texture/white1x1.png",1.0f,player_->GetPosPtr());

    // システムオブジェクトの作成
    mapManager_ = CreateObject<MapManager>();
    defaultCameraPos_ = { 0.0f, 15.0f, -15.0f };
    // リズムトリガーの作成
    rhythmTrigger_ = std::make_unique<RhythmTrigger>(beatScheduler_);
    // リズムに合わせてコンパニオンをスポーンするシステムの作成
    rhythmSpawnCompanion_ = std::make_unique<RhythmSpawnCompanion>(
        *(rhythmTrigger_.get()),
        *player_,
        *(bulletObjectContainers_["Companion"].get()));

    // UIオブジェクトの作成
    beatUi_ = std::make_unique<BeatUi>(
        *(spriteObjects_["BeatUi"].back()),
        *(spriteObjects_["MeasureBar"].back()),
        spriteObjects_["Note"],
        beatScheduler_,
        *(rhythmTrigger_.get()));
}

void GameScene::Initialize(EngineSystem* engine) {
    BaseScene::Initialize(engine);
    //* SceneObjects *//
    // プレイヤーの初期化
    player_->Initialize();
    for (auto& [name, container] : bulletObjectContainers_) {
        container->Initialize();
    }
    //bulletObjectContainers_["Companion"]->Spawn({ 2.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.5f,0.5f,0.5f });

    //* AppSystems *//
    // マップの初期化（10x10）
    mapManager_->Initialize(10, 10);
    // カメラコントローラーの初期化
    cameraController_.Initialize();
    cameraController_.SetCameraWork<FollowCamera>(*(cameraManager_.get()), player_->GetTransform(), defaultCameraPos_, 0.1f);
    // ビートスケジューラーの初期化（4拍子）
    beatScheduler_.Initialize(4);
    // リズムトリガーの初期化
    rhythmTrigger_->Initialize();
    // リズムに合わせてコンパニオンをスポーンするシステムの初期化
    rhythmSpawnCompanion_->Initialize();

    //* SceneUi *//
    // UIの初期化
    beatUi_->Initialize();
}

void GameScene::Update() {
    for (auto& [name, container] : bulletObjectContainers_) {
        container->Update();
    }

    cameraController_.Update();
    beatScheduler_.Update();
    rhythmTrigger_->Update();
    rhythmSpawnCompanion_->Update();

    beatUi_->Update();
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
