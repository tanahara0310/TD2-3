#include "NotificationManager.h"
#include <imgui.h>

namespace CoreEngine
{

void NotificationManager::ShowNotification(const std::string& message, float duration)
{
    Notification notification;
    notification.message = message;
    notification.remainingTime = duration;
    notifications_.push_back(notification);
}

void NotificationManager::Update(float deltaTime)
{
    for (auto it = notifications_.begin(); it != notifications_.end(); ) {
        it->remainingTime -= deltaTime;
        if (it->remainingTime <= 0.0f) {
            it = notifications_.erase(it);
        } else {
            ++it;
        }
    }
}

void NotificationManager::Draw()
{
    ImGuiIO& io = ImGui::GetIO();

    const float notificationWidth = 300.0f;
    const float notificationHeight = 60.0f;
    const float padding = 10.0f;
    const float startY = 50.0f;

    for (size_t i = 0; i < notifications_.size(); ++i) {
        const auto& notification = notifications_[i];
        
        float alpha = 1.0f;
        if (notification.remainingTime < kNotificationFadeTime) {
            alpha = notification.remainingTime / kNotificationFadeTime;
        }

        ImVec2 windowPos(io.DisplaySize.x - notificationWidth - padding, startY + i * (notificationHeight + padding));
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(notificationWidth, notificationHeight), ImGuiCond_Always);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.6f, 1.0f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        std::string windowName = "##Notification" + std::to_string(i);
        ImGui::Begin(windowName.c_str(), nullptr, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoScrollbar | 
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoFocusOnAppearing);

        ImVec2 textSize = ImGui::CalcTextSize(notification.message.c_str());
        ImGui::SetCursorPosX((notificationWidth - textSize.x) * 0.5f);
        ImGui::SetCursorPosY((notificationHeight - textSize.y) * 0.5f);
        ImGui::Text("%s", notification.message.c_str());

        ImGui::End();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }
}

}
