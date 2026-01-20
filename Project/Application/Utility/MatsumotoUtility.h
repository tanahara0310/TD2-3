#pragma once
#include "Engine/Math/Vector/Vector3.h"
#include "Engine/Math/Vector/Vector4.h"

namespace MatsumotoUtility {
    float SimpleEaseIn(float from, float to, float transitionSpeed);
    CoreEngine::Vector3 SimpleEaseIn(const CoreEngine::Vector3& from, const CoreEngine::Vector3& to, float transitionSpeed);
    CoreEngine::Vector4 SimpleEaseIn(const CoreEngine::Vector4& from, const CoreEngine::Vector4& to, float transitionSpeed);
    float SimpleEaseInAngle(float from, float to, float transitionSpeed);

    CoreEngine::Vector3 SphericalToCartesian(float radius, float theta, float phi);
    CoreEngine::Vector3 CartesianToSpherical(const CoreEngine::Vector3& cartesian);
}