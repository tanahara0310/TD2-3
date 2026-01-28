#pragma once
#include "IEnemy.h"
#include <string>
#include <EngineSystem.h>

class FollowEnemy : public IEnemy {
public:
    FollowEnemy(CoreEngine::Vector3* targetPos);
    void Initialize() override;
    void EnemyUpdate() override;
    void OnCollisionEnter(CoreEngine::GameObject* other) override;
    void PlaySE(const std::string& soundKey) override;
private:
    int hp_;
    const CoreEngine::Vector3* targetPos_;
    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;
};