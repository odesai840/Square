#pragma once

#include "Script.h"
#include "GameStateManager.h"
#include "Properties.h"

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
    void Move(float delta_time);
    void Jump(float delta_time);
    void Dash(float delta_time);
    void Slash(float delta_time);
    
    void OnCollision(float delta_time);
    bool IsGrounded(uint32_t playerId);
    bool EnemyHitByCurrentSlash(uint32_t enemy_id);

private:
    PlayerData player_data;
    uint32_t player = 0;
    int level = 0;

    Direction player_direction = Direction::LEFT;

    float move_speed = 350.0f;
    float acceleration = 15.0f;
    float jump_velocity = 1000.0f;
    float dash_velocity = 1800.0f;

    uint32_t slash = 0;
    bool is_slashing = false;
    float slash_length = 0.15f;
    float slash_duration = 0.0f;
    float slash_knockback = 600.0f;
    Direction slash_direction = Direction::LEFT;
    std::vector<uint32_t> damaged_by_slash_enemies;

    std::vector<uint32_t> enemies_to_remove;

    // keybinds
    SDL_Scancode move_left_bind = SDL_SCANCODE_A;
    SDL_Scancode move_right_bind = SDL_SCANCODE_D;
    SDL_Scancode jump_bind = SDL_SCANCODE_SPACE;
    SDL_Scancode dash_bind = SDL_SCANCODE_LSHIFT;
    SDL_Scancode slash_bind = SDL_SCANCODE_I;

    // debug keybinds
    SDL_Scancode debug_collision = SDL_SCANCODE_T;
    SDL_Scancode debug_save = SDL_SCANCODE_P;
    SDL_Scancode debug_hot_reload = SDL_SCANCODE_F5;
};