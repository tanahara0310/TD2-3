#pragma once
#include "IEnemy.h"
#include <string>
class DummyEnemy : public IEnemy {
public:
    DummyEnemy();

    void EnemyUpdate() override;
    void OnCollisionEnter(CoreEngine::GameObject* other) override;

private:
    int hp_;
};