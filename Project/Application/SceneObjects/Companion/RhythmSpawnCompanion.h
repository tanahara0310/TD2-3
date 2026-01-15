#pragma once
class RhythmTrigger;
class Player;
class BulletObjectContainer;

class RhythmSpawnCompanion final{
public:
    RhythmSpawnCompanion(
        RhythmTrigger& rhythmTrigger,
        Player& player,
        BulletObjectContainer& companionContainer);

    void Initialize();
    void Update();

private:
    RhythmTrigger& rhythmTrigger_;
    Player& player_;
    BulletObjectContainer& companionContainer_;

    int lastSpawnIndex_;
};