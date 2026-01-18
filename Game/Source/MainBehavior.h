#pragma once

#include "GameInterface.h"
#include "GameStateManager.h"

class MainBehavior : public SquareCore::GameInterface {
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    PlayerData player_data;
    
    uint32_t player;
    uint32_t marcus;
    uint32_t music;
    uint32_t ui_rect;
    uint32_t ui_button;
    uint32_t ui_text;
    uint32_t mouseFollower;
    
    bool IsGrounded(uint32_t playerId);
};
