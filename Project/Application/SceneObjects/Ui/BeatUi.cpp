#include "BeatUi.h"
#include "Application/SceneObjects/BeatController/BeatScheduler.h"
#include <EngineSystem.h>

BeatUi::BeatUi(
    SpriteObject& beatUiSprite,
    const BeatScheduler& beatScheduler) :
    beatUiSprite_(beatUiSprite),
    beatScheduler_(beatScheduler) {}

void BeatUi::Initialize() {
    beatUiSprite_.Initialize("Texture/white1x1.png", "BeatUiSprite");
}

void BeatUi::Update() {
}

void BeatUi::Draw() {

}
