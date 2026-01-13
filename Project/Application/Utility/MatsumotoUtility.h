#pragma once
#include "Engine/Math/Vector/Vector3.h"

namespace MatsumotoUtility {
    float SimpleEaseIn(float from, float to, float transitionSpeed);
    Vector3 SimpleEaseIn(const Vector3& from, const Vector3& to, float transitionSpeed);
}