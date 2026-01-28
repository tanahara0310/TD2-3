#pragma once
#include "IEnemy.h"
#include <string>
#include <EngineSystem.h>

class DummyEnemy : public IEnemy {
public:
    DummyEnemy();

    void Initialize() override;
    void EnemyUpdate() override;
    void OnCollisionEnter(CoreEngine::GameObject* other) override;

    void PlaySE(const std::string& soundKey) override;

private:
    int hp_;
    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;
};