#pragma once
#include "Application/Utility/BulletObject/BulletObjectContainer.h"
class Player;
class Ball;

class BulletShotController {
public:
    BulletShotController() = delete;
    explicit BulletShotController(Player* player,Ball* ball);

    void Update(BulletObjectContainer* bulletContainer);

private:
    Player* player_;
    Ball* ball_;
};