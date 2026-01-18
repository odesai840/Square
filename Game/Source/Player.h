#pragma once

#include "Script.h"
#include "GameStateManager.h"

enum class Direction : uint8_t
{
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
};

class Player : public SquareCore::Script {
public:
    void OnStart() override;
    void OnUpdate(float delta_time) override;

private:
    bool IsGrounded(uint32_t playerId);
    void Move(float delta_time);
    void Jump(float delta_time);
    void Dash(float delta_time);

private:
    PlayerData player_data;
    uint32_t player = 0;

    Direction current_direction = Direction::RIGHT;

    float move_speed = 350.0f;
    float acceleration = 15.0f;
    float jump_velocity = 600.0f;
    float dash_velocity = 600.0f;

    // keybinds
    SDL_Scancode move_left_bind = SDL_SCANCODE_A;
    SDL_Scancode move_right_bind = SDL_SCANCODE_D;
    SDL_Scancode jump_bind = SDL_SCANCODE_SPACE;
    SDL_Scancode dash_bind = SDL_SCANCODE_LSHIFT;

    // debug keybinds
    SDL_Scancode debug_collision = SDL_SCANCODE_T;
    SDL_Scancode debug_save = SDL_SCANCODE_P;
};