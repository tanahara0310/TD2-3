#pragma once
#include "Engine/Math/Vector/Vector3.h"
#include "Engine/Math/Vector/Vector4.h"
#include <externals/nlohmann/single_include/nlohmann/json.hpp>
#include <string>

namespace MatsumotoUtility {
    inline constexpr CoreEngine::Vector4 ColorMagenta =
    { 0.98f, 0.0f, 0.906f, 1.0f };
    inline constexpr CoreEngine::Vector4 ColorTurquoise =
    { 0.0f, 0.98f, 0.875f, 1.0f };
    inline constexpr CoreEngine::Vector4 ColorYellow =
    { 0.98f, 0.776f, 0.0f, 1.0f };
    inline constexpr CoreEngine::Vector4 ColorBrass =
    { 0.647f, 0.561f, 0.216f, 1.0f };
    inline constexpr CoreEngine::Vector4 ColorEggplant =
    { 0.478f, 0.239f, 0.463f, 1.0f };
    inline constexpr CoreEngine::Vector4 ColorGrey =
    { 0.239f, 0.478f, 0.455f, 1.0f };

    float SimpleEaseIn(float from, float to, float transitionSpeed);
    CoreEngine::Vector3 SimpleEaseIn(const CoreEngine::Vector3& from, const CoreEngine::Vector3& to, float transitionSpeed);
    CoreEngine::Vector4 SimpleEaseIn(const CoreEngine::Vector4& from, const CoreEngine::Vector4& to, float transitionSpeed);
    float SimpleEaseInAngle(float from, float to, float transitionSpeed);

    CoreEngine::Vector3 SphericalToCartesian(float radius, float theta, float phi);
    CoreEngine::Vector3 CartesianToSpherical(const CoreEngine::Vector3& cartesian);

    void SaveSceneObjectConfig(nlohmann::json& json,const std::string& fileName);
    void LoadSceneObjectConfig(nlohmann::json& json,const std::string& fileName);

    CoreEngine::Vector4 ColorCodeToVector4(const std::string& colorCode);

    CoreEngine::Vector3 DirectionToEulerAngle(const CoreEngine::Vector3& direction);

    float DegreesToRadians(float degrees);

    float Lerp(float a, float b, float t);
}