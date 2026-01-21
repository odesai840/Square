#pragma once

#include "Script.h"
#include "GameStateManager.h"
#include "Properties.h"

struct BounceEntity
{
    uint32_t id;
    float original_scale_y;
    float original_position_y;
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
    void Projectile(float delta_time);
    
    void OnCollision(float delta_time);
    bool IsGrounded(uint32_t playerId);
    bool EnemyHitByCurrentSlash(uint32_t enemy_id);
    void HandleBounceEntities(float delta_time, uint32_t current_bounce_entity);
    void UpdateBounceEntities(float delta_time);
    void TakeDamage(Character* player_character, int damage);

private:
    PlayerData player_data;
    uint32_t player = 0;
    int level = 0;
    
    // upgrades
    bool has_double_dash = true;
    bool has_double_jump = true;
    bool has_projectile = true;

    SquareCore::Vec2 last_grounded_position;

    Direction player_direction = Direction::LEFT;

    float move_speed = 350.0f;
    float acceleration = 15.0f;
    float jump_velocity = 1000.0f;

    bool can_double_jump = false;

    bool can_take_damage = true;
    float can_take_damage_cooldown = 0.1f;
    float can_take_damage_timer = 0.0f;

    bool is_looking_up = false;
    bool is_looking_down = false;
    bool last_vertical_look_was_up = true;\
    
    uint32_t projectile = 0;
    bool projectile_active = false;
    float projectile_speed = 800.0f;
    float projectile_cooldown = 1.0f;
    float projectile_cooldown_elapsed = 0.0f;
    bool projectile_in_cooldown = false;
    Direction projectile_direction = Direction::LEFT;

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
    float slash_cooldown = 0.3f;
    float slash_cooldown_elapsed = 0.0f;
    bool slash_in_cooldown = false;

    std::vector<uint32_t> recently_hit_by_enemies;
    std::vector<uint32_t> enemies_to_remove;
    std::vector<BounceEntity> recently_bounced_on;
};