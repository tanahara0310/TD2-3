#include "BeatUi.h"
#include "Application/SceneObjects/BeatController/BeatScheduler.h"
#include "Application/SceneObjects/RhythmController/RhythmTrigger.h"
#include <EngineSystem.h>

#include "Application/Utility/MatsumotoUtility.h"

BeatUi::BeatUi(
    SpriteObject& beatUiSprite,
    SpriteObject& barSprite,
    std::vector<SpriteObject*> noteSprites,
    const BeatScheduler& beatScheduler,
    const RhythmTrigger& rhythmTrigger) :
    beatUiSprite_(beatUiSprite),
    beatBarUiSprite_(barSprite),
    noteSprites_(noteSprites),
    beatScheduler_(beatScheduler),
    rhythmTrigger_(rhythmTrigger) {
    defaultPos_ = { 0.0f, -250.0f, 0.0f };
    barColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    shakeTimer_ = 0.0f;

    // タイムラインスプライトの初期化
    beatUiSprite_.Initialize("Texture/Measure.png", "BeatUiSprite");
    beatUiSprite_.GetSpriteTransform().translate = defaultPos_;
    beatUiSprite_.SetActive(true);
    // ビートバーの初期化
    beatBarUiSprite_.Initialize("Texture/MeasureBar.png", "MeasureBarSprite");
    barWidth_ = beatUiSprite_.GetActualSize().x;
    beatBarUiSprite_.GetSpriteTransform().translate = defaultPos_;
    beatBarUiSprite_.GetSpriteTransform().translate.x -= -(barWidth_ / 2.0f); // 左端に合わせる
    beatBarUiSprite_.SetActive(true);
    // ノートスプライトの初期化
    for (auto& noteSprite : noteSprites_) {
        noteSprite->Initialize("Texture/Note.png", "NoteSprite");
        noteSprite->GetSpriteTransform().scale = { 0.3f, 1.0f, 1.0f };
        noteSprite->SetActive(false);
    }
}

void BeatUi::Initialize() {
    // タイムラインスプライトの初期化
    beatUiSprite_.GetSpriteTransform().translate = defaultPos_;
    beatUiSprite_.SetActive(true);
    // ビートバーの初期化
    barWidth_ = beatUiSprite_.GetActualSize().x;
    beatBarUiSprite_.GetSpriteTransform().translate = defaultPos_;
    beatBarUiSprite_.GetSpriteTransform().translate.x -= -(barWidth_ / 2.0f); // 左端に合わせる
    beatBarUiSprite_.SetActive(true);
    // ノートスプライトの初期化
    for (auto& noteSprite : noteSprites_) {
        noteSprite->GetSpriteTransform().scale = { 0.3f, 1.0f, 1.0f };
        noteSprite->SetActive(false);
    }
}

void BeatUi::Update() {

    // ノートスプライトの更新
    int beatsPerMeasure = beatScheduler_.GetCurrentMeasure().GetBeatsPerMeasure();
    for (size_t i = 0; i < noteSprites_.size(); ++i) {
        if (i < static_cast<size_t>(beatsPerMeasure)) {
            float beatPos = beatScheduler_.GetCurrentMeasure().GetBeatPosition(static_cast<int>(i));
            float noteX = defaultPos_.x - (barWidth_ / 2.0f) + (barWidth_ * beatPos);
            noteSprites_[i]->GetSpriteTransform().translate = { noteX, defaultPos_.y, 0.0f };
            noteSprites_[i]->SetActive(true);
        } else {
            noteSprites_[i]->SetActive(false);
        }
    }
    // リズムトリガーの状態に応じてエフェクトを適用
    if (rhythmTrigger_.IsTriggered()) {
        if (rhythmTrigger_.IsSuccess()) {
            // 成功時のエフェクト（例: 色を緑に変える）
            barColor_ = { 0.0f, 1.0f, 0.0f, 1.0f };
            beatUiSprite_.GetSpriteTransform().scale.y = 1.5f;
        } else {
            // ミス時のエフェクト（例: 色を赤に変える）
            barColor_ = { 1.0f, 0.0f, 0.0f, 1.0f };
            shakeTimer_ = 0.5f; // シェイクタイマーをセット
        }
    }

    // シェイクエフェクトの更新
    if (shakeTimer_ > 0.0) {
        shakeTimer_ -= 0.016f;
    } else {
        shakeTimer_ = 0.0f;
    }
    float shakeOffset = sin(shakeTimer_ * 50.0f) * shakeTimer_ * 10.0f;
    // 色を元に戻す
    barColor_ = MatsumotoUtility::SimpleEaseIn(barColor_, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.1f);

    // ビートバーの位置更新
    if (rhythmTrigger_.IsOnBeat()) {
        beatBarUiSprite_.GetSpriteTransform().scale.y = 
            MatsumotoUtility::SimpleEaseIn(beatBarUiSprite_.GetSpriteTransform().scale.y, 1.2f, 0.6f);
    } else {
        beatBarUiSprite_.GetSpriteTransform().scale.y =
            MatsumotoUtility::SimpleEaseIn(beatBarUiSprite_.GetSpriteTransform().scale.y, 0.8f, 0.6f);
    }

    float measurePos = beatScheduler_.GetCurrentMeasurePosition();
    float barX = defaultPos_.x - (barWidth_ / 2.0f) + (barWidth_ * measurePos) + shakeOffset;
    beatBarUiSprite_.GetSpriteTransform().translate.x = barX;
    beatBarUiSprite_.SetColor(barColor_);
    // タイムラインの位置更新
    beatUiSprite_.GetSpriteTransform().translate.x = defaultPos_.x + shakeOffset;
    beatUiSprite_.GetSpriteTransform().scale = MatsumotoUtility::SimpleEaseIn(beatUiSprite_.GetSpriteTransform().scale, { 1.0f, 1.0f, 1.0f }, 0.1f);
    beatUiSprite_.SetColor(barColor_);
}

void BeatUi::Draw() {

}
