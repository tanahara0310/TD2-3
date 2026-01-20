#include "MatsumotoUtility.h"
#include <cmath>
#include <numbers>

using namespace CoreEngine;

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
