#pragma once
#include "IEnemy.h"
#include <string>
#include <EngineSystem.h>

class DummyEnemy : public IEnemy {
public:
    DummyEnemy();

    void Initialize() override;
    void EnemyUpdate() override;

    void PlaySE(const std::string& soundKey) override;

private:
    
    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;
};