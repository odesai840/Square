#pragma once

#include "Script.h"

enum class Direction : uint8_t
{
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
};

enum class ChargeState : uint8_t
{
    PATROLLING,
    CHARGING,
    STUNNED
};

struct Character : SquareCore::Property
{
    int health = 10;
    int damage = 1;

    Character(int health = 10, int damage = 1) { this->health = health; this->damage = damage; }
};

struct JumpEnemy : SquareCore::Property
{
    float detection_range = 200.0f;
    float jump_cooldown = 2.0f;
    float cooldown_timer = 0.0f;
    SquareCore::Vec2 jump_force = {400.0f, 800.0f};
    bool chasing = false;
    bool hit_player_this_attack = false;

    JumpEnemy(float detection_range, float jump_cooldown, SquareCore::Vec2 jump_force)
    {
        this->detection_range = detection_range;
        this->jump_cooldown = jump_cooldown;
        this->jump_force = jump_force;
    }
};

struct ChargeEnemy : SquareCore::Property
{
    ChargeState state = ChargeState::PATROLLING;
    Direction facing_direction = Direction::RIGHT;
    bool aware_of_player = false;
    
    float patrol_point_a_x;
    float patrol_point_b_x;
    float patrol_speed = 100.0f;
    
    float charge_duration = 2.0f;
    float charge_elapsed = 0.0f;
    float charge_speed = 400.0f;
    
    float stun_duration = 3.0f;
    float stun_elapsed = 0.0f;
    
    bool hit_player_this_attack = false;
    
    ChargeEnemy(float spawn_x, float patrol_range = 400.0f)
    {
        this->patrol_point_a_x = spawn_x;
        this->patrol_point_b_x = spawn_x + patrol_range;
    }
};

struct JumpBoss : SquareCore::Property
{
    float jump_cooldown = 3.0f;
    float jump_cooldown_timer = 0.0f;
    SquareCore::Vec2 jump_force = {5.0f, 3500.0f};
    
    float charge_force = 6000.0f;
    float charge_windup_time = 0.5f;
    float charge_windup_timer = 0.0f;
    bool is_winding_up = false;

    float close_range = 500.0f;
    float far_range = 2000.0f;
    
    bool hit_player_this_attack = false;
};