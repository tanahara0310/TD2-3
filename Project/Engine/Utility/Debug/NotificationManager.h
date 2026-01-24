#pragma once
#include <string>
#include <deque>

namespace CoreEngine
{
class NotificationManager {
public:
    static NotificationManager& GetInstance() {
        static NotificationManager instance;
        return instance;
    }

    void ShowNotification(const std::string& message, float duration = 3.0f);
    void Update(float deltaTime);
    void Draw();

private:
    NotificationManager() = default;
    ~NotificationManager() = default;
    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;

    struct Notification {
        std::string message;
        float remainingTime;
    };

    std::deque<Notification> notifications_;
    const float kNotificationFadeTime = 0.5f;
};
}
