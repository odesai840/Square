#pragma once

#include "Script.h"
#include "GameStateManager.h"

class Player : public SquareCore::Script {
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    PlayerData player_data;
    uint32_t player;
    
    bool IsGrounded(uint32_t playerId);
};
