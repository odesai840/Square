#pragma once

#include "GameInterface.h"

class MainBehavior : public SquareCore::GameInterface {
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    uint32_t player;
    uint32_t marcus;
    uint32_t music;
    uint32_t ui_rect;
    uint32_t ui_button;
};