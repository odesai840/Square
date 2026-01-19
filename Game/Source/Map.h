#pragma once

#include "Script.h"

class Map : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    uint32_t ground;
    uint32_t music;
    uint32_t dialogTestTrigger = 0;
};