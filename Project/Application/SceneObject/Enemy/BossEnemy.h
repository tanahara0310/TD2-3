#pragma once
#include "IEnemy.h"
#include <string>
#include <EngineSystem.h>

class BossEnemy : public IEnemy {
public:
    BossEnemy();
    virtual ~BossEnemy() = default;

    void Initialize() override;
    void EnemyUpdate() override;
    void OnCollisionEnter(CoreEngine::GameObject* other) override;

    void PlaySE(const std::string& soundKey) override;

private:
    int hp_;
    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;
};