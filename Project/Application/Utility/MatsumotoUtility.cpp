#include "MatsumotoUtility.h"
#include <cmath>

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
