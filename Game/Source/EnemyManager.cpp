#include "EnemyManager.h"

void EnemyManager::OnStart()
{
    LoadEnemies();
    player = GetFirstEntityWithTag("Player");
}

void EnemyManager::LoadEnemies()
{
    enemies.clear();

    std::vector<uint32_t> all_enemies_with_jump_enemy_tag = GetAllEntitiesWithTag("JumpEnemy");
    std::vector<uint32_t> all_enemies_with_charge_enemy_tag = GetAllEntitiesWithTag("ChargeEnemy");

    for (uint32_t jump_enemy : all_enemies_with_jump_enemy_tag)
    {
        SetEntityColor(jump_enemy, SquareCore::RGBA(245, 73, 39, 255));
        SetPhysicsEnabled(jump_enemy, true);
        FlipSprite(jump_enemy, true, false);
        SetDrag(jump_enemy, 5.0f);
        SetEntityPersistent(jump_enemy, false);
        AddPropertyToEntity(jump_enemy, new Character(5, 5, 1));

        JumpEnemy* jump_prop = new JumpEnemy(200.0f, 3.0f, {1000.0f, 1600.0f});
        jump_prop->base_scale = GetScale(jump_enemy);
        AddPropertyToEntity(jump_enemy, jump_prop);

        SetColliderPolygon(jump_enemy, enemy_collider_vertices);
        enemies.push_back(jump_enemy);

        auto props = GetAllEntityProperties(jump_enemy);
    }

    for (uint32_t charge_enemy : all_enemies_with_charge_enemy_tag)
    {
        Direction random_direction = (rand() % 2 == 0) ? Direction::RIGHT : Direction::LEFT;
        SetEntityColor(charge_enemy, SquareCore::RGBA(82, 9, 9, 255));
        SetPhysicsEnabled(charge_enemy, true);
        FlipSprite(charge_enemy, random_direction == Direction::RIGHT, false);
        SetDrag(charge_enemy, 5.0f);
        SetEntityPersistent(charge_enemy, false);
        AddPropertyToEntity(charge_enemy, new Character(5, 5, 1));

        ChargeEnemy* charge_prop = new ChargeEnemy(GetPosition(charge_enemy).x, 400.0f);
        charge_prop->facing_direction = random_direction;
        charge_prop->base_scale = GetScale(charge_enemy);
        AddPropertyToEntity(charge_enemy, charge_prop);

        SetColliderPolygon(charge_enemy, enemy_collider_vertices);
        enemies.push_back(charge_enemy);

        auto props = GetAllEntityProperties(charge_enemy);
    }
}

uint32_t EnemyManager::SpawnChargeEnemy(const SquareCore::Vec2& position)
{
    uint32_t charge_enemy = AddEntity("Resources/Sprites/triangle-enemy.png", position.x, position.y, 0.0f, 0.05f, 0.05f, true, {"Enemy", "ChargeEnemy", "Pogo"});
    Direction random_direction = (rand() % 2 == 0) ? Direction::RIGHT : Direction::LEFT;
    SetEntityColor(charge_enemy, SquareCore::RGBA(82, 9, 9, 255));
    SetPhysicsEnabled(charge_enemy, true);
    FlipSprite(charge_enemy, random_direction == Direction::RIGHT, false);
    SetDrag(charge_enemy, 5.0f);
    SetEntityPersistent(charge_enemy, false);
    AddPropertyToEntity(charge_enemy, new Character(5, 5, 1));

    ChargeEnemy* charge_prop = new ChargeEnemy(GetPosition(charge_enemy).x, 400.0f);
    charge_prop->facing_direction = random_direction;
    charge_prop->base_scale = GetScale(charge_enemy);
    AddPropertyToEntity(charge_enemy, charge_prop);

    SetColliderPolygon(charge_enemy, enemy_collider_vertices);
    enemies.push_back(charge_enemy);

    return charge_enemy;
}

uint32_t EnemyManager::SpawnJumpEnemy(const SquareCore::Vec2& position)
{
    uint32_t jump_enemy = AddEntity("Resources/Sprites/triangle-enemy.png", position.x, position.y, 0.0f, 0.05f, 0.05f, true, {"Enemy", "JumpEnemy", "Pogo"});
    SetEntityColor(jump_enemy, SquareCore::RGBA(245, 73, 39, 255));
    FlipSprite(jump_enemy, true, false);
    SetPhysicsEnabled(jump_enemy, true);
    SetDrag(jump_enemy, 5.0f);
    SetEntityPersistent(jump_enemy, false);
    AddPropertyToEntity(jump_enemy, new Character(5, 5, 1));

    JumpEnemy* jump_prop = new JumpEnemy(200.0f, 3.0f, {1000.0f, 1600.0f});
    jump_prop->base_scale = GetScale(jump_enemy);
    AddPropertyToEntity(jump_enemy, jump_prop);

    SetColliderPolygon(jump_enemy, enemy_collider_vertices);
    enemies.push_back(jump_enemy);

    return jump_enemy;
}

uint32_t EnemyManager::SpawnJumpBoss(const SquareCore::Vec2& position)
{
    if (jump_boss)
        RemoveEntity(jump_boss);
    jump_boss = AddEntity("Resources/Sprites/triangle-enemy.png", -6000.0f, 0.0f, 0.0f, 0.5f, 0.5f, true);
    SetEntityColor(jump_boss, SquareCore::RGBA(70, 0, 0, 255));
    SetPhysicsEnabled(jump_boss, true);
    AddTagToEntity(jump_boss, "Enemy");
    AddTagToEntity(jump_boss, "Pogo");
    AddTagToEntity(jump_boss, "JumpBoss");
    SetDrag(jump_boss, 5.0f);
    SetEntityPersistent(jump_boss, true);
    AddPropertyToEntity(jump_boss, new Character(50, 50, 2));
    AddPropertyToEntity(jump_boss, new JumpBoss());
    SetColliderPolygon(jump_boss, boss_collider_vertices);
    enemies.push_back(jump_boss);
    return jump_boss;
}


void EnemyManager::OnUpdate(float deltaTime)
{
    if (player == 0) return;

    SquareCore::Vec2 player_position = GetPosition(player);

    for (uint32_t enemy : enemies)
    {
        if (!EntityExists(enemy)) continue;

        SquareCore::Vec2 enemy_velocity = GetVelocity(enemy);
        if (enemy_velocity.y < 0.0f)
        {
            SetVelocity(enemy, enemy_velocity.x, enemy_velocity.y - (10000.0f * deltaTime));
        }

        for (auto& property : GetAllEntityProperties(enemy))
        {
            if (JumpEnemy* jump_property = dynamic_cast<JumpEnemy*>(property))
            {
                SquareCore::Vec2 enemy_position = GetPosition(enemy);
                enemy_velocity = GetVelocity(enemy);
                float horizontal_distance = std::abs(player_position.x - enemy_position.x);
                float vertical_distance = std::abs(player_position.y - enemy_position.y);

                bool is_grounded = std::abs(enemy_velocity.y) < 50.0f;
                bool can_see_player = horizontal_distance <= jump_property->detection_range && vertical_distance < 100.0f;

                switch (jump_property->state)
                {
                case JumpEnemyState::IDLE:
                    {
                        jump_property->cooldown_timer += deltaTime;

                        if ((can_see_player || jump_property->chasing) &&
                            jump_property->cooldown_timer >= jump_property->jump_cooldown && is_grounded)
                        {
                            jump_property->state = JumpEnemyState::WINDING_UP;
                            jump_property->windup_timer = 0.0f;
                            jump_property->chasing = true;
                            jump_property->hit_player_this_attack = false;

                            float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                            FlipSprite(enemy, !(direction <= 0), false);
                        }
                        break;
                    }

                case JumpEnemyState::WINDING_UP:
                    {
                        jump_property->windup_timer += deltaTime;

                        float pulse = (std::sin(jump_property->windup_timer * 20.0f) + 1.0f) * 0.5f;
                        float scale_multiplier = 0.9f + (pulse * 0.5f);
                        SetScale(enemy, SquareCore::Vec2(jump_property->base_scale.x,
                                                         jump_property->base_scale.y * scale_multiplier));

                        SetVelocity(enemy, 0.0f, enemy_velocity.y);

                        if (jump_property->windup_timer >= jump_property->windup_duration)
                        {
                            jump_property->state = JumpEnemyState::JUMPING;
                            jump_property->windup_timer = 0.0f;

                            float clamped_distance = std::max(jump_property->min_distance_for_scaling,
                                                              std::min(horizontal_distance,
                                                                       jump_property->max_distance_for_scaling));
                            float distance_ratio = (clamped_distance - jump_property->min_distance_for_scaling) /
                                (jump_property->max_distance_for_scaling - jump_property->min_distance_for_scaling);

                            SquareCore::Vec2 scaled_jump_force;
                            scaled_jump_force.x = jump_property->min_jump_force.x +
                                (jump_property->max_jump_force.x - jump_property->min_jump_force.x) * distance_ratio;
                            scaled_jump_force.y = jump_property->min_jump_force.y +
                                (jump_property->max_jump_force.y - jump_property->min_jump_force.y) * distance_ratio;

                            float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                            SetVelocity(enemy, direction * scaled_jump_force.x, scaled_jump_force.y);

                            SetScale(enemy, jump_property->base_scale);
                        }
                        break;
                    }

                case JumpEnemyState::JUMPING:
                    {
                        if (is_grounded && !jump_property->was_grounded)
                        {
                            jump_property->state = JumpEnemyState::RECOVERING;
                            jump_property->recovery_timer = 0.0f;
                        }

                        jump_property->was_grounded = is_grounded;
                        break;
                    }

                case JumpEnemyState::RECOVERING:
                    {
                        jump_property->recovery_timer += deltaTime;

                        SetVelocity(enemy, enemy_velocity.x * 0.85f, enemy_velocity.y);

                        if (jump_property->recovery_timer >= jump_property->recovery_duration)
                        {
                            jump_property->state = JumpEnemyState::IDLE;
                            jump_property->cooldown_timer = 0.0f;
                            jump_property->recovery_timer = 0.0f;
                        }
                        break;
                    }
                }
            }

            if (ChargeEnemy* charge_property = dynamic_cast<ChargeEnemy*>(property))
            {
                SquareCore::Vec2 enemy_position = GetPosition(enemy);
                enemy_velocity = GetVelocity(enemy);
                float horizontal_distance = std::abs(player_position.x - enemy_position.x);
                float vertical_distance = std::abs(player_position.y - enemy_position.y);

                switch (charge_property->state)
                {
                case ChargeEnemyState::PATROLLING:
                    {
                        if (!charge_property->aware_of_player)
                        {
                            bool player_in_front = false;
                            if (charge_property->facing_direction == Direction::RIGHT)
                            {
                                player_in_front = player_position.x > enemy_position.x;
                            }
                            else if (charge_property->facing_direction == Direction::LEFT)
                            {
                                player_in_front = player_position.x < enemy_position.x;
                            }

                            bool can_see_player = player_in_front && horizontal_distance < 600.0f && vertical_distance < 100.0f;

                            if (can_see_player)
                            {
                                charge_property->aware_of_player = true;
                                charge_property->state = ChargeEnemyState::NOTICING;
                                charge_property->notice_timer = 0.0f;
                                SetVelocity(enemy, 0.0f, enemy_velocity.y);
                            }
                        }

                        if (!charge_property->aware_of_player)
                        {
                            if (charge_property->facing_direction == Direction::RIGHT)
                            {
                                if (enemy_position.x >= charge_property->patrol_point_b_x)
                                {
                                    charge_property->facing_direction = Direction::LEFT;
                                    FlipSprite(enemy, false, false);
                                }
                                else
                                {
                                    SetVelocity(enemy, charge_property->patrol_speed, enemy_velocity.y);
                                }
                            }
                            else if (charge_property->facing_direction == Direction::LEFT)
                            {
                                if (enemy_position.x <= charge_property->patrol_point_a_x)
                                {
                                    charge_property->facing_direction = Direction::RIGHT;
                                    FlipSprite(enemy, true, false);
                                }
                                else
                                {
                                    SetVelocity(enemy, -charge_property->patrol_speed, enemy_velocity.y);
                                }
                            }
                        }
                        break;
                    }

                case ChargeEnemyState::NOTICING:
                    {
                        charge_property->notice_timer += deltaTime;

                        Direction target_direction = (player_position.x > enemy_position.x)
                                                         ? Direction::RIGHT
                                                         : Direction::LEFT;
                        if (charge_property->facing_direction != target_direction)
                        {
                            charge_property->facing_direction = target_direction;
                            FlipSprite(enemy, target_direction == Direction::RIGHT, false);
                        }

                        SetVelocity(enemy, 0.0f, enemy_velocity.y);

                        if (charge_property->notice_timer >= charge_property->notice_duration)
                        {
                            charge_property->state = ChargeEnemyState::PREPARING;
                            charge_property->prepare_timer = 0.0f;
                            charge_property->hit_player_this_attack = false;
                        }
                        break;
                    }

                case ChargeEnemyState::PREPARING:
                    {
                        charge_property->prepare_timer += deltaTime;

                        float pulse = (std::sin(charge_property->prepare_timer * 18.0f) + 1.0f) * 0.5f;
                        float squash = 0.85f + (pulse * 0.5f);
                        SetScale(enemy, SquareCore::Vec2(charge_property->base_scale.x * squash,
                                                         charge_property->base_scale.y));

                        SetVelocity(enemy, 0.0f, enemy_velocity.y);

                        if (charge_property->prepare_timer >= charge_property->prepare_duration)
                        {
                            charge_property->state = ChargeEnemyState::CHARGING;
                            charge_property->charge_elapsed = 0.0f;
                            SetScale(enemy, charge_property->base_scale);
                        }
                        break;
                    }

                case ChargeEnemyState::CHARGING:
                    {
                        charge_property->charge_elapsed += deltaTime;

                        float distance_ratio = std::min(1.0f, horizontal_distance / 1000.0f);
                        float charge_speed = charge_property->min_charge_speed +
                            (charge_property->max_charge_speed - charge_property->min_charge_speed) * (1.0f -
                                distance_ratio);

                        float direction = (charge_property->facing_direction == Direction::RIGHT) ? 1.0f : -1.0f;
                        SetVelocity(enemy, direction * charge_speed, enemy_velocity.y);

                        if (charge_property->charge_elapsed >= charge_property->charge_duration)
                        {
                            charge_property->state = ChargeEnemyState::STUNNED;
                            charge_property->stun_elapsed = 0.0f;
                            SetVelocity(enemy, 0.0f, enemy_velocity.y);
                        }
                        break;
                    }

                case ChargeEnemyState::STUNNED:
                    {
                        charge_property->stun_elapsed += deltaTime;
                        SetVelocity(enemy, 0.0f, enemy_velocity.y);

                        if (charge_property->stun_elapsed >= charge_property->stun_duration)
                        {
                            charge_property->state = ChargeEnemyState::PATROLLING;
                            charge_property->aware_of_player = false;
                        }
                        break;
                    }
                }
            }
            
            JumpBoss* jump_property = dynamic_cast<JumpBoss*>(property);
            if (jump_boss_active && jump_property)
            {
                jump_property->jump_cooldown_timer += deltaTime;
                SquareCore::Vec2 enemy_position = GetPosition(enemy);
                float distance = std::abs(player_position.x - enemy_position.x);

                if (jump_property->is_winding_up)
                {
                    jump_property->charge_windup_timer += deltaTime;

                    if (jump_property->charge_windup_timer >= jump_property->charge_windup_time)
                    {
                        jump_property->is_winding_up = false;
                        jump_property->charge_windup_timer = 0.0f;
                        jump_property->jump_cooldown_timer = 0.0f;

                        float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                        FlipSprite(enemy, !(direction <= 0), false);
                        SetVelocity(enemy, direction * jump_property->charge_force, GetVelocity(enemy).y);
                    }
                }
                else if (jump_property->jump_cooldown_timer >= jump_property->jump_cooldown)
                {
                    jump_property->hit_player_this_attack = false;

                    float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                    FlipSprite(enemy, !(direction <= 0), false);

                    if (distance > jump_property->far_range)
                    {
                        jump_property->is_winding_up = true;
                        jump_property->charge_windup_timer = 0.0f;
                    }
                    else if (distance < jump_property->close_range)
                    {
                        jump_property->jump_cooldown_timer = 0.0f;
                        SetVelocity(enemy, direction * distance * jump_property->jump_force.x,
                                    jump_property->jump_force.y);
                    }
                    else
                    {
                        if (rand() % 2 == 0)
                        {
                            jump_property->is_winding_up = true;
                            jump_property->charge_windup_timer = 0.0f;
                        }
                        else
                        {
                            jump_property->jump_cooldown_timer = 0.0f;
                            SetVelocity(enemy, direction * distance * jump_property->jump_force.x,
                                        jump_property->jump_force.y);
                        }
                    }
                }
            }
        }
    }
}