#pragma once
#include <vector>
#include <memory>
#include <stdint.h>
#include "BulletModel.h"
#include "Engine/Scene/BaseScene.h"

/// @brief 複数のBulletModelオブジェクトを管理するコンテナクラス
class BulletObjectContainer {
public:
    BulletObjectContainer() = delete;
    explicit BulletObjectContainer(uint32_t num);
    ~BulletObjectContainer() = default;

    /// @brief 指定した型のBulletModelオブジェクトをシーン上に作成して格納する(BulletModelが基底クラスでないといけない)
    template<typename T, typename... Args>
    void ApplyToScene(BaseScene* scene, Args&&... args) {
        static_assert(std::is_base_of_v<BulletModel, T>, "T must be derived from BulletModel");
        for (auto& obj : bulletObjects_) {
            obj = scene->CreateObject<T>(std::forward<Args>(args)...);
        }
    }

    /// @brief 指定した型のBulletModelオブジェクトをインデックスで取得する(BulletModelが基底クラスでないといけない)
    template<typename T>
    T* GetObjectAt(size_t index) {
        static_assert(std::is_base_of_v<BulletModel, T>, "T must be derived from BulletModel");
        if (index >= bulletObjects_.size()) {
            return nullptr;
        }
        return dynamic_cast<T*>(bulletObjects_[index]);
    }

    /// @brief 全てのBulletModelオブジェクトを初期化
    void Initialize();
    /// @brief 全てのBulletModelオブジェクトを更新
    void Update();
    /// @brief 全てのBulletModelオブジェクトを描画
    void Draw(const ICamera* camera);

    /// @brief 全てのBulletModelオブジェクトのアクティブ状態を設定
    void SetActiveAll(bool isActive);
    /// @brief BulletModelオブジェクトの配列を取得
    std::vector<BulletModel*>& GetBulletObjects();

    /// @brief 指定のトランスフォームでオブジェクトをアクティブ化
    int Spawn(const Vector3& pos, const Vector3& rotate, const Vector3& scale);

private:
    std::vector<BulletModel*> bulletObjects_;
};