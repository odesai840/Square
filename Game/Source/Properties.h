#pragma once

#include "Script.h"

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
    float detection_range = 200.0f;
    float charge_cooldown = 3.0f;
    float cooldown_timer = 0.0f;
    float charge_force = 1000.0f;
    bool chasing = false;
    bool hit_player_this_attack = false;
    
    ChargeEnemy(float detection_range, float charge_cooldown, float charge_force)
    {
        this->detection_range = detection_range;
        this->charge_cooldown = charge_cooldown;
        this->charge_force = charge_force;
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