#pragma once
class ICameraWork {
public:
    virtual ~ICameraWork() = default;
    virtual void Update() = 0;
};