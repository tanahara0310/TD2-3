#include "RhythmSpawnCompanion.h"
#include "Application/SceneObjects/RhythmController/RhythmTrigger.h"
#include "Application/SceneObjects/Player/Player.h"
#include "Application/SceneObjects/BulletObject/BulletObjectContainer.h"
#include "Application/SceneObjects/Companion/Companion.h"

RhythmSpawnCompanion::RhythmSpawnCompanion(
    RhythmTrigger& rhythmTrigger,
    Player& player,
    BulletObjectContainer& companionContainer) :
    rhythmTrigger_(rhythmTrigger),
    player_(player),
    companionContainer_(companionContainer),
    lastSpawnIndex_(-1) {
}

void RhythmSpawnCompanion::Initialize() {
    lastSpawnIndex_ = -1;
}

void RhythmSpawnCompanion::Update() {
    if (rhythmTrigger_.IsTriggered()&&rhythmTrigger_.IsSuccess()) {
        if (lastSpawnIndex_ == -1) {
            // プレイヤーの位置にコンパニオンをスポーン
            lastSpawnIndex_ = companionContainer_.Spawn(
                player_.GetTransform(),
                { 0.0f, 0.0f, 0.0f },
                { 0.5f, 0.5f, 0.5f });

        } else {
            // 既にスポーンしている場合は位置を更新
            auto companion = companionContainer_.GetObjectAt<Companion>(lastSpawnIndex_);
            if (companion) {
                lastSpawnIndex_ = companionContainer_.Spawn(
                    companion->GetTransform(),
                    { 0.0f, 0.0f, 0.0f },
                    { 0.5f, 0.5f, 0.5f });
                companionContainer_.GetObjectAt<Companion>(lastSpawnIndex_)->SetNewTargetPosition(companion->GetPosPtr());

            } else {
                lastSpawnIndex_ = -1;
            }
        }
    }
}

