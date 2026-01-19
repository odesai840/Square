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

    // upgrades
    bool has_double_dash = true;

    Direction player_direction = Direction::LEFT;

    float move_speed = 350.0f;
    float acceleration = 15.0f;
    float jump_velocity = 1000.0f;

    uint32_t dash = 0;
    bool is_dashing = false;
    float dash_length = 0.13f;
    float dash_duration = 0.0f;
    float dash_velocity = 3200.0f;
    float normal_player_scale_y = 0.05f;
    float dash_cooldown = 1.0f;
    float dash_cooldown_elapsed = 0.0f;
    float double_dash_window = 0.3f;
    int dashes_used = 0;
    float time_since_last_dash = 0.0f;
    bool in_cooldown = false;

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

    // debug keybinds
    SDL_Scancode debug_collision = SDL_SCANCODE_T;
    SDL_Scancode debug_save = SDL_SCANCODE_P;
    SDL_Scancode debug_hot_reload = SDL_SCANCODE_F5;
    SDL_Scancode debug_mouse_cursor = SDL_SCANCODE_Y;
    bool mouse_visible = true;
};