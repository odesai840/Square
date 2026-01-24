#include "EnemyManager.h"
#include "Player.h"

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
        AlterJumpEnemy(jump_enemy);
        enemies.push_back(jump_enemy);
    }

    for (uint32_t charge_enemy : all_enemies_with_charge_enemy_tag)
    {
        AlterChargeEnemy(charge_enemy);
        enemies.push_back(charge_enemy);
    }
}

uint32_t EnemyManager::SpawnChargeEnemy(const SquareCore::Vec2& position)
{
    uint32_t charge_enemy = AddEntity("Resources/Sprites/triangle-enemy.png", position.x, position.y, 0.0f, 0.075f,
                                      0.075f, true, {"Enemy", "ChargeEnemy", "Pogo"});
    AlterChargeEnemy(charge_enemy);
    enemies.push_back(charge_enemy);

    return charge_enemy;
}

uint32_t EnemyManager::SpawnJumpEnemy(const SquareCore::Vec2& position)
{
    uint32_t jump_enemy = AddEntity("Resources/Sprites/triangle-enemy.png", position.x, position.y, 0.0f, 0.075f,
                                    0.075f, true, {"Enemy", "JumpEnemy", "Pogo"});
    AlterJumpEnemy(jump_enemy);
    enemies.push_back(jump_enemy);

    return jump_enemy;
}

uint32_t EnemyManager::SpawnJumpBoss(const SquareCore::Vec2& position)
{
    if (jump_boss)
        RemoveEntity(jump_boss);
    jump_boss = AddEntity("Resources/Sprites/triangle-enemy.png", position.x, position.y, 0.0f, 0.5f, 0.5f, true);
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

void EnemyManager::SpawnSecondBoss(const SquareCore::Vec2& position)
{
    for (int i = 0; i < 3; i++)
    {
        if (second_bosses[i])
            RemoveEntity(second_bosses[i]);
        SquareCore::Vec2 pos = {position.x + (i * 200.0f), position.y};
        second_bosses[i] = AddAnimatedEntity("Resources/Sprites/flyangle-sheet.png", 2, 10.0f, pos.x, pos.y, 0.0f,
                                             0.075f, 0.075f, true);
        SetPhysicsEnabled(second_bosses[i], true);
        SetGravityScale(second_bosses[i], 0.0f);
        SetDrag(second_bosses[i], 5.0f);
        AddTagToEntity(second_bosses[i], "Enemy");
        AddTagToEntity(second_bosses[i], "Pogo");
        AddTagToEntity(second_bosses[i], "SecondBoss");
        Character* character_property = new Character(1, 1, 1);
        AddPropertyToEntity(second_bosses[i], character_property);
        second_boss_ch_properties[i] = character_property;
        SecondBoss* boss_property = new SecondBoss(pos);
        AddPropertyToEntity(second_bosses[i], boss_property);
        second_boss_properties[i] = boss_property;
        SetColliderPolygon(second_bosses[i], boss_2_collider_vertices);
        enemies.push_back(second_bosses[i]);
    }
}

void EnemyManager::AlterChargeEnemy(uint32_t enemy_id)
{
    Direction random_direction = (rand() % 2 == 0) ? Direction::RIGHT : Direction::LEFT;
    SetEntityColor(enemy_id, SquareCore::RGBA(82, 9, 9, 255));
    SetScale(enemy_id, SquareCore::Vec2(0.075f, 0.075f));
    SetPhysicsEnabled(enemy_id, true);
    FlipSprite(enemy_id, random_direction == Direction::RIGHT, false);
    SetDrag(enemy_id, 5.0f);
    SetMass(enemy_id, 25.0f);
    SetEntityPersistent(enemy_id, false);
    AddPropertyToEntity(enemy_id, new Character(5, 5, 1));

    ChargeEnemy* charge_prop = new ChargeEnemy(GetPosition(enemy_id).x, 400.0f);
    charge_prop->facing_direction = random_direction;
    charge_prop->base_scale = GetScale(enemy_id);
    AddPropertyToEntity(enemy_id, charge_prop);

    SetColliderPolygon(enemy_id, enemy_collider_vertices);
}

void EnemyManager::AlterJumpEnemy(uint32_t enemy_id)
{
    SetEntityColor(enemy_id, SquareCore::RGBA(245, 73, 39, 255));
    SetScale(enemy_id, SquareCore::Vec2(0.075f, 0.075f));
    SetPhysicsEnabled(enemy_id, true);
    FlipSprite(enemy_id, true, false);
    SetDrag(enemy_id, 5.0f);
    SetMass(enemy_id, 25.0f);
    SetEntityPersistent(enemy_id, false);
    AddPropertyToEntity(enemy_id, new Character(5, 5, 1));

    JumpEnemy* jump_prop = new JumpEnemy(800.0f, 3.0f, {1000.0f, 1600.0f});
    jump_prop->base_scale = GetScale(enemy_id);
    AddPropertyToEntity(enemy_id, jump_prop);

    SetColliderPolygon(enemy_id, enemy_collider_vertices);
}


void EnemyManager::OnUpdate(float deltaTime)
{
    if (player == 0) return;

    SquareCore::Vec2 player_position = GetPosition(player);

    int dead_count = 0;
    for (int i = 0; i < 3; i++)
    {
        if (!EntityExists(second_bosses[i]) && boss_2_active)
            dead_count++;
        
        if (EntityExists(second_bosses[i]))
        {
            for (auto& property : GetAllEntityProperties(second_bosses[i]))
            {
                if (Character* ch = dynamic_cast<Character*>(property))
                {
                    if (ch->health <= 0)
                    {
                        for (auto& prop2 : GetAllEntityProperties(second_bosses[i]))
                        {
                            if (SecondBoss* sb = dynamic_cast<SecondBoss*>(prop2))
                                sb->is_dead = true;
                        }
                    }
                }
            }
        }
    }
    if (dead_count >= 3)
    {
        if (uint32_t boss_2_exit = GetFirstEntityWithTag("Boss2Exit"))
            RemoveEntity(boss_2_exit);
        boss_2_active = false;
        player_script->GetPlayerData().second_boss_dead = true;
    }

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

                if (jump_property->aggro_on_player)
                {
                    jump_property->chasing = true;
                }

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
            if (boss_1_active && jump_property)
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
            
            SecondBoss* second_boss = dynamic_cast<SecondBoss*>(property);
            if (second_boss && boss_2_active)
            {
                if (boss_2_active)
                {
                    if (intro_countdown > 0.0f)
                        intro_countdown -= deltaTime;
                    else
                    {
                        if (!EntityExists(second_bosses[active_boss]))
                        {
                            int start_boss = active_boss;
                            do
                            {
                                active_boss++;
                                if (active_boss == 3) active_boss = 0;
                            }
                            while ((!EntityExists(second_bosses[active_boss]) || second_boss_properties[active_boss]->
                                is_dead) && active_boss != start_boss);
                            continue;
                        }

                        int alive_count = 0;
                        for (int i = 0; i < 3; i++)
                        {
                            if (EntityExists(second_bosses[i]) && !second_boss_properties[i]->is_dead)
                                alive_count++;
                        }
                        float current_time_between_attacks = time_between_attacks * (alive_count / 3.0f);
                        float current_wind_up_time = wind_up_time * (alive_count / 3.0f);

                        if (is_winding_up)
                        {
                            wind_up_timer += deltaTime;
                            if (wind_up_timer >= current_wind_up_time)
                            {
                                is_winding_up = false;
                                switch (current_attack_type)
                                {
                                case 0:
                                    SetVelocity(second_bosses[active_boss], 0.0f, 0.0f);
                                    SetVelocity(second_bosses[active_boss], 5000.0f, 0.0f);
                                    break;
                                case 1:
                                    SetVelocity(second_bosses[active_boss], 0.0f, 0.0f);
                                    SetVelocity(second_bosses[active_boss], -5000.0f, 0.0f);
                                    break;
                                case 2:
                                    SetVelocity(second_bosses[active_boss], 0.0f, 0.0f);
                                    SetVelocity(second_bosses[active_boss], 0.0f, -5000.0f);
                                    break;
                                default: break;
                                }
                            }
                        }
                        else
                        {
                            if (time_elapsed_between_attacks < current_time_between_attacks)
                            {
                                time_elapsed_between_attacks += deltaTime;
                            }
                            else
                            {
                                if (!active_boss_has_attacked)
                                {
                                    if (time_elapsed_between_attacks >= current_time_between_attacks)
                                    {
                                        time_elapsed_between_attacks = 0.0f;
                                        DetermineSecondBossAttack(second_bosses[active_boss]);
                                        active_boss_has_attacked = true;
                                    }
                                    else
                                    {
                                        time_elapsed_between_attacks += deltaTime;
                                    }
                                }
                                else
                                {
                                    SquareCore::Vec2 spawn_pos = second_boss_properties[active_boss]->spawn_position;
                                    SetPosition(second_bosses[active_boss], spawn_pos.x, spawn_pos.y);

                                    int start_boss = active_boss;
                                    do
                                    {
                                        active_boss++;
                                        if (active_boss == 3) active_boss = 0;
                                    }
                                    while ((!EntityExists(second_bosses[active_boss]) || second_boss_properties[active_boss]->is_dead) && active_boss != start_boss);

                                    active_boss_has_attacked = false;
                                    time_elapsed_between_attacks = 0.0f;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void EnemyManager::DetermineSecondBossAttack(uint32_t boss_id)
{
    SetVelocity(boss_id, 0.0f, 0.0f);
    wind_up_timer = 0.0f;

    for (auto& property : GetAllEntityProperties(boss_id))
    {
        if (SecondBoss* sb = dynamic_cast<SecondBoss*>(property))
        {
            sb->hit_player_this_attack = false;
        }
    }

    switch (rand() % 3)
    {
    case 0:
        {
            left_attack_pos = GetPosition(GetFirstEntityWithTag("Boss2LeftAttackPosition"));
            SetPosition(boss_id, left_attack_pos.x, left_attack_pos.y);
            FlipSprite(boss_id, true, false);
            is_winding_up = true;
            current_attack_type = 0;
            break;
        }
    case 1:
        {
            right_attack_pos = GetPosition(GetFirstEntityWithTag("Boss2RightAttackPosition"));
            SetPosition(boss_id, right_attack_pos.x, right_attack_pos.y);
            FlipSprite(boss_id, false, false);
            is_winding_up = true;
            current_attack_type = 1;
            break;
        }
    case 2:
        {
            SquareCore::Vec2 player_pos = GetPosition(player);
            SetPosition(boss_id, player_pos.x, -4000.0f);
            is_winding_up = true;
            current_attack_type = 2;
            break;
        }
    default: break;
    }
}