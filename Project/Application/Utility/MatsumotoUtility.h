#pragma once
#include "Engine/Math/Vector/Vector3.h"
#include "Engine/Math/Vector/Vector4.h"

namespace MatsumotoUtility {
    float SimpleEaseIn(float from, float to, float transitionSpeed);
    Vector3 SimpleEaseIn(const Vector3& from, const Vector3& to, float transitionSpeed);
    Vector4 SimpleEaseIn(const Vector4& from, const Vector4& to, float transitionSpeed);
}