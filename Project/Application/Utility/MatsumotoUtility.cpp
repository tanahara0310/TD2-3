#include "MatsumotoUtility.h"
#include <cmath>
#include <numbers>
#include <fstream>

#ifdef _DEBUG
#include "Engine/Utility/Debug/NotificationManager.h"
#endif

using namespace CoreEngine;

namespace {
    const std::string kSceneObjectConfigDirectory = "Assets/ApplicationAssets/SceneObjectsParameters/";
}

float MatsumotoUtility::SimpleEaseIn(float from, float to, float transitionSpeed) {
    float value = from;
    value += (to - value) * transitionSpeed;
    if (fabsf(value - to) <= 0.01f) {
        return to;
    }
    return value;
}

Vector3 MatsumotoUtility::SimpleEaseIn(const Vector3& from, const Vector3& to, float transitionSpeed) {
    Vector3 value = from;
    value.x = SimpleEaseIn(from.x, to.x, transitionSpeed);
    value.y = SimpleEaseIn(from.y, to.y, transitionSpeed);
    value.z = SimpleEaseIn(from.z, to.z, transitionSpeed);
    return value;

}

Vector4 MatsumotoUtility::SimpleEaseIn(const Vector4& from, const Vector4& to, float transitionSpeed) {
    Vector4 value = from;
    value.x = SimpleEaseIn(from.x, to.x, transitionSpeed);
    value.y = SimpleEaseIn(from.y, to.y, transitionSpeed);
    value.z = SimpleEaseIn(from.z, to.z, transitionSpeed);
    value.w = SimpleEaseIn(from.w, to.w, transitionSpeed);
    return value;
}

float MatsumotoUtility::SimpleEaseInAngle(float from, float to, float transitionSpeed) {
    // 角度差を -π～π に正規化
    float diff = fmodf(to - from + 3.0f * 3.14f, 2.0f * 3.14f) - 3.14f;
    float value = from + diff * transitionSpeed;
    // 0～2πに正規化（必要なら）
    if (value < 0.0f) value += 2.0f * 3.14f;
    if (value >= 2.0f * 3.14f) value -= 2.0f * 3.14f;
    // 収束判定
    if (fabsf(diff) <= 0.01f) {
        return to;
    }
    return value;
}

CoreEngine::Vector3 MatsumotoUtility::SphericalToCartesian(float radius, float theta, float phi) {
    CoreEngine::Vector3 result;
    result.x = radius * sinf(phi) * cosf(theta);
    result.y = radius * cosf(phi);
    result.z = radius * sinf(phi) * sinf(theta);
    return result;
}

CoreEngine::Vector3 MatsumotoUtility::CartesianToSpherical(const CoreEngine::Vector3& cartesian) {
    CoreEngine::Vector3 result;
    float r = sqrtf(cartesian.x * cartesian.x + cartesian.y * cartesian.y + cartesian.z * cartesian.z);
    float theta = atan2f(cartesian.x, cartesian.z);
    float phi = acosf(cartesian.y / r);
    result.x = r;
    result.y = theta;
    result.z = phi;
    return result;
}

void MatsumotoUtility::SaveSceneObjectConfig(nlohmann::json& json, const std::string& fileName) {
    try
    {
        std::ifstream inputFile(kSceneObjectConfigDirectory + fileName);
        if (inputFile.is_open())
        {
            nlohmann::json existingJson;
            inputFile >> existingJson;
            inputFile.close();
            // 既存のJSONと新しいJSONをマージ
            for (auto& [key, value] : json.items())
            {
                existingJson[key] = value;
            }
            // マージしたJSONをファイルに保存
            std::ofstream outputFile(kSceneObjectConfigDirectory + fileName);
            if (outputFile.is_open())
            {
                outputFile << existingJson.dump(4); // インデント付きで保存
                outputFile.close();
#ifdef _DEBUG
                CoreEngine::NotificationManager::GetInstance().ShowNotification("設定を保存しました", 2.0f);
#endif
            }
        } else
        {
            // ファイルが存在しない場合、新規作成
            std::ofstream outputFile(kSceneObjectConfigDirectory + fileName);
            if (outputFile.is_open())
            {
                outputFile << json.dump(4); // インデント付きで保存
                outputFile.close();
#ifdef _DEBUG
                CoreEngine::NotificationManager::GetInstance().ShowNotification("設定を保存しました", 2.0f);
#endif
            }
        }
    }
    catch (const std::exception&)
    {

    }
}

void MatsumotoUtility::LoadSceneObjectConfig(nlohmann::json& json, const std::string& fileName) {
    try
    {
        std::ifstream inputFile(kSceneObjectConfigDirectory + fileName);
        if (inputFile.is_open())
        {
            nlohmann::json loadedJson;
            inputFile >> loadedJson;
            inputFile.close();

            // 読み込んだJSONを引数のJSONにマージ
            for (auto& [key, value] : loadedJson.items())
            {
                json[key] = value;
            }

#ifdef _DEBUG
            CoreEngine::NotificationManager::GetInstance().ShowNotification("設定を読み込みました", 2.0f);
#endif
        }
    }
    catch (const std::exception&)
    {
            
    }
}

CoreEngine::Vector4 MatsumotoUtility::ColorCodeToVector4(const std::string& colorCode) {
    CoreEngine::Vector4 result;

    if (colorCode.length() != 7 || colorCode[0] != '#') {
        // 無効なカラーコードの場合、白色を返す
        return { 1.0f, 1.0f, 1.0f, 1.0f };
    }
    int r = std::stoi(colorCode.substr(1, 2), nullptr, 16);
    int g = std::stoi(colorCode.substr(3, 2), nullptr, 16);
    int b = std::stoi(colorCode.substr(5, 2), nullptr, 16);
    result.x = static_cast<float>(r) / 255.0f;
    result.y = static_cast<float>(g) / 255.0f;
    result.z = static_cast<float>(b) / 255.0f;
    result.w = 1.0f;
    return result;
}

CoreEngine::Vector3 MatsumotoUtility::DirectionToEulerAngle(const CoreEngine::Vector3& direction) {
    CoreEngine::Vector3 eulerAngle;
    eulerAngle.y = atan2f(direction.x, direction.z); // Yaw
    eulerAngle.x = asinf(-direction.y / CoreEngine::Length(direction)); // Pitch
    eulerAngle.z = 0.0f; // Roll
    return eulerAngle;
}

float MatsumotoUtility::DegreesToRadians(float degrees) {
    return degrees * (std::numbers::pi_v<float> / 180.0f);
}

float MatsumotoUtility::Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
