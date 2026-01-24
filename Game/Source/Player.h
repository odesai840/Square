#pragma once

#include "DialogManager.h"
#include "EnemyManager.h"
#include "Script.h"
#include "GameStateManager.h"
#include "Properties.h"

struct BounceEntity
{
    uint32_t id;
    float original_scale_y;
    float original_position_y;
};

struct ProjectileEntity
{
    uint32_t id;
    bool active = false;
    float timer = 0.0f;
    Direction direction = Direction::LEFT;
};

class Player : public SquareCore::Script {
public:
    Player() : projectile_pool(sizeof(ProjectileEntity), 5) {}
    void OnStart() override;
    void OnUpdate(float delta_time) override;
    void OnExit() override;
    void SetDialogManager(DialogManager* dialog_manager) {this->dialog_manager = dialog_manager;}
    void SetEnemyManager(EnemyManager* enemy_manager) { this->enemy_manager = enemy_manager; }
    void TeleportPlayer(const SquareCore::Vec2& position);
    PlayerData& GetPlayerData() { return player_data; }

private:
    void Move(float delta_time);
    void Jump(float delta_time);
    void Dash(float delta_time);
    void Slash(float delta_time);
    void Projectile(float delta_time);
    void Heal(float delta_time);
    
    void OnCollision(float delta_time);
    bool IsGrounded(uint32_t playerId);
    bool EnemyHitByCurrentSlash(uint32_t enemy_id);
    void HandleBounceEntities(float delta_time, uint32_t current_bounce_entity);
    void UpdateBounceEntities(float delta_time);
    void TakeDamage(Character* player_character, int damage);
    void DealDamage(Character* enemy_character, uint32_t enemy_id, int damage);

private:
    bool loaded = false;
    
    PlayerData player_data;
    uint32_t player = 0;
    int level = 0;

    DialogManager* dialog_manager;
    EnemyManager* enemy_manager;

    SquareCore::Vec2 last_grounded_position;
    uint32_t last_valid_ground_entity = 0;

    Direction player_direction = Direction::LEFT;

    int slash_damage = 1;
    int projectile_damage = 2;

    float move_speed = 350.0f;
    float acceleration = 15.0f;
    float jump_velocity = 1000.0f;

    float slash_fps = 0.0f;
    float dash_fps = 0.0f;
    float projectile_fps = 0.0f;

    bool can_double_jump = false;

    bool can_take_damage = true;
    float can_take_damage_cooldown = 0.1f;
    float can_take_damage_timer = 0.0f;

    bool is_looking_up = false;
    bool is_looking_down = false;
    bool last_vertical_look_was_up = true;

    SquareCore::PoolAllocator projectile_pool;
    float projectile_max_life = 3.0f;
    float projectile_speed = 1200.0f;
    float projectile_cooldown = 1.0f;
    float projectile_cooldown_elapsed = 0.0f;
    bool projectile_in_cooldown = false;
    float projectile_knockback = 1200.0f;

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
    
    std::vector<uint32_t> trap_walls;
    std::vector<uint32_t> spawned_enemy_ids;

    std::vector<float> bounds = {-10000.0f, -6000.0f, 6000.0f, 10000.0f};
    float target_bounds_y_min = bounds[1];
    float bounds_lerp_speed = 5.0f;
};