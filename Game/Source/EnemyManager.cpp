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

    SDL_Log("Found %d JumpEnemies, %d ChargeEnemies", 
            (int)all_enemies_with_jump_enemy_tag.size(), 
            (int)all_enemies_with_charge_enemy_tag.size());

    for (uint32_t jump_enemy : all_enemies_with_jump_enemy_tag)
    {
        SDL_Log("Setting up JumpEnemy %u", jump_enemy);
        FlipSprite(jump_enemy, true, false);
        SetDrag(jump_enemy, 5.0f);
        SetEntityPersistent(jump_enemy, false);
        AddPropertyToEntity(jump_enemy, new Character(5));
        AddPropertyToEntity(jump_enemy, new JumpEnemy(200.0f, 3.0f, {1000.0f, 1600.0f}));
        SetColliderPolygon(jump_enemy, enemy_collider_vertices);
        enemies.push_back(jump_enemy);
        
        auto props = GetAllEntityProperties(jump_enemy);
        SDL_Log("JumpEnemy %u now has %d properties", jump_enemy, (int)props.size());
    }
    
    for (uint32_t charge_enemy : all_enemies_with_charge_enemy_tag)
    {
        SDL_Log("Setting up ChargeEnemy %u", charge_enemy);
        Direction random_direction = (rand() % 2 == 0) ? Direction::RIGHT : Direction::LEFT;
        FlipSprite(charge_enemy, random_direction == Direction::RIGHT, false);
        SetDrag(charge_enemy, 5.0f);
        SetEntityPersistent(charge_enemy, false);
        AddPropertyToEntity(charge_enemy, new Character(5));
        
        ChargeEnemy* charge_prop = new ChargeEnemy(GetPosition(charge_enemy).x, 400.0f);
        charge_prop->facing_direction = random_direction;
        AddPropertyToEntity(charge_enemy, charge_prop);
        
        SetColliderPolygon(charge_enemy, enemy_collider_vertices);
        enemies.push_back(charge_enemy);
        
        auto props = GetAllEntityProperties(charge_enemy);
        SDL_Log("ChargeEnemy %u now has %d properties", charge_enemy, (int)props.size());
    }
    
    /*jump_boss = AddEntity("Resources/Sprites/triangle.png", -2000.0f, 0.0f, 0.0f, 0.5f, 0.5f, true);
    AddTagToEntity(jump_boss, "Enemy");
    AddTagToEntity(jump_boss, "Pogo");
    AddTagToEntity(jump_boss, "JumpBoss");
    SetDrag(jump_boss, 5.0f);
    SetEntityPersistent(jump_boss, true);
    AddPropertyToEntity(jump_boss, new Character(50, 2));
    AddPropertyToEntity(jump_boss, new JumpBoss());
    SetColliderPolygon(jump_boss, boss_collider_vertices);
    enemies.insert(enemies.end(), jump_boss);*/
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
            // jump enemy logic
            if (JumpEnemy* jump_property = dynamic_cast<JumpEnemy*>(property))
            {
                jump_property->cooldown_timer += deltaTime;
                SquareCore::Vec2 enemy_position = GetPosition(enemy);
                float distance = std::abs(player_position.x - enemy_position.x);

                if ((distance <= jump_property->detection_range || jump_property->chasing) && jump_property->
                    cooldown_timer >= jump_property->jump_cooldown)
                {
                    jump_property->hit_player_this_attack = false;
                    jump_property->chasing = true;
                    jump_property->cooldown_timer = 0.0f;
                    float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                    FlipSprite(enemy, direction <= 0, false);
                    SetVelocity(enemy, direction * jump_property->jump_force.x, jump_property->jump_force.y);
                }
            }

            // charge enemy logic
            if (ChargeEnemy* charge_property = dynamic_cast<ChargeEnemy*>(property))
            {
                SquareCore::Vec2 enemy_position = GetPosition(enemy);
                SquareCore::Vec2 enemy_velocity = GetVelocity(enemy);
                
                if (charge_property->state == ChargeState::PATROLLING)
                {
                    if (!charge_property->aware_of_player)
                    {
                        bool player_in_view = false;
                        if (charge_property->facing_direction == Direction::RIGHT)
                        {
                            player_in_view = player_position.x > enemy_position.x;
                        }
                        else if (charge_property->facing_direction == Direction::LEFT)
                        {
                            player_in_view = player_position.x < enemy_position.x;
                        }
                        
                        if (player_in_view)
                        {
                            charge_property->aware_of_player = true;
                        }
                    }
                    
                    if (charge_property->aware_of_player)
                    {
                        Direction target_direction = (player_position.x > enemy_position.x) ? Direction::RIGHT : Direction::LEFT;
                        if (charge_property->facing_direction != target_direction)
                        {
                            charge_property->facing_direction = target_direction;
                            FlipSprite(enemy, target_direction == Direction::RIGHT, false);
                        }
                        
                        charge_property->state = ChargeState::CHARGING;
                        charge_property->charge_elapsed = 0.0f;
                        charge_property->hit_player_this_attack = false;
                    }
                    else
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
                }
                else if (charge_property->state == ChargeState::CHARGING)
                {
                    charge_property->charge_elapsed += deltaTime;
                    
                    float direction = (charge_property->facing_direction == Direction::RIGHT) ? 1.0f : -1.0f;
                    SetVelocity(enemy, direction * charge_property->charge_speed, enemy_velocity.y);
                    
                    if (charge_property->charge_elapsed >= charge_property->charge_duration)
                    {
                        charge_property->state = ChargeState::STUNNED;
                        charge_property->stun_elapsed = 0.0f;
                        SetVelocity(enemy, 0.0f, enemy_velocity.y);
                    }
                }
                else if (charge_property->state == ChargeState::STUNNED)
                {
                    charge_property->stun_elapsed += deltaTime;
                    SetVelocity(enemy, 0.0f, enemy_velocity.y);
                    
                    if (charge_property->stun_elapsed >= charge_property->stun_duration)
                    {
                        charge_property->state = ChargeState::PATROLLING;
                    }
                }
            }

            // jump boss logic
            if (JumpBoss* jump_property = dynamic_cast<JumpBoss*>(property))
            {
                jump_property->jump_cooldown_timer += deltaTime;
                SquareCore::Vec2 enemy_position = GetPosition(enemy);
                float distance = std::abs(player_position.x - enemy_position.x);

                if (jump_property->is_winding_up)
                {
                    jump_property->charge_windup_timer += deltaTime;

                    float pulse = (std::sin(jump_property->charge_windup_timer * 15.0f) + 1.0f) * 0.5f;
                    float scale = 0.2f + (pulse * 0.2f);

                    if (jump_property->charge_windup_timer >= jump_property->charge_windup_time)
                    {
                        jump_property->is_winding_up = false;
                        jump_property->charge_windup_timer = 0.0f;
                        jump_property->jump_cooldown_timer = 0.0f;
                        
                        float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                        FlipSprite(enemy, direction <= 0, false);
                        SetVelocity(enemy, direction * jump_property->charge_force, GetVelocity(enemy).y);
                    }
                }
                else if (jump_property->jump_cooldown_timer >= jump_property->jump_cooldown)
                {
                    jump_property->hit_player_this_attack = false;

                    float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                    FlipSprite(enemy, direction <= 0, false);

                    if (distance > jump_property->far_range)
                    {
                        jump_property->is_winding_up = true;
                        jump_property->charge_windup_timer = 0.0f;

                        float indicator_offset = direction > 0 ? 0.0f : -0.0f;
                    }
                    else if (distance < jump_property->close_range)
                    {
                        jump_property->jump_cooldown_timer = 0.0f;
                        SetVelocity(enemy, direction * distance * jump_property->jump_force.x, jump_property->jump_force.y);
                    }
                    else
                    {
                        if (rand() % 2 == 0)
                        {
                            jump_property->is_winding_up = true;
                            jump_property->charge_windup_timer = 0.0f;

                            float indicator_offset = direction > 0 ? 0.0f : -0.0f;
                        }
                        else
                        {
                            jump_property->jump_cooldown_timer = 0.0f;
                            SetVelocity(enemy, direction * distance  * jump_property->jump_force.x, jump_property->jump_force.y);
                        }
                    }
                }
            }
        }
    }
}