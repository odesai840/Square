#include "EnemyManager.h"

void EnemyManager::OnStart()
{
    for (unsigned int i = 0; i < jump_enemy_count; i++)
    {
        enemies.insert(enemies.end(),
                       AddEntity("Resources/Sprites/triangle.png", 300.0f, -100.0f, 0.0f, 0.05f, 0.05f,
                                 true)); // 0.05f
        //SetEntityColor(enemies[i], SquareCore::RGBA(120, 0, 0, 255));
        FlipSprite(enemies[enemies.size() - 1], true, false);
        AddTagToEntity(enemies[enemies.size() - 1], "Enemy");
        SetDrag(enemies[enemies.size() - 1], 5.0f);
        SetEntityPersistent(enemies[enemies.size() - 1], true);
        AddPropertyToEntity(enemies[enemies.size() - 1], new Character(5));
        AddPropertyToEntity(enemies[enemies.size() - 1], new JumpEnemy(200.0f, 3.0f, {1000.0f, 1600.0f}));
    }
    for (unsigned int i = 0; i < charge_enemy_count; i++)
    {
        enemies.insert(enemies.end(),
                       AddEntity("Resources/Sprites/triangle.png", -300.0f, -100.0f, 0.0f, 0.05f, 0.05f,
                                 true)); // 0.05f
        //SetEntityColor(enemies[i], SquareCore::RGBA(120, 0, 0, 255));
        FlipSprite(enemies[enemies.size() - 1], true, false);
        AddTagToEntity(enemies[enemies.size() - 1], "Enemy");
        SetDrag(enemies[enemies.size() - 1], 5.0f);
        SetEntityPersistent(enemies[enemies.size() - 1], true);
        AddPropertyToEntity(enemies[enemies.size() - 1], new Character(5));
        AddPropertyToEntity(enemies[enemies.size() - 1], new ChargeEnemy(800.0f, 3.0f, 2000.0f));
    }

    /*
    jump_boss = AddEntity("Resources/Sprites/triangle.png", -2000.0f, 0.0f, 0.0f, 0.5f, 0.5f, true);
    AddTagToEntity(jump_boss, "Enemy");
    AddTagToEntity(jump_boss, "JumpBoss");
    SetDrag(jump_boss, 5.0f);
    SetEntityPersistent(jump_boss, true);
    AddPropertyToEntity(jump_boss, new Character(50, 2));
    AddPropertyToEntity(jump_boss, new JumpBoss());
    enemies.insert(enemies.end(), jump_boss);
    */

    player = GetFirstEntityWithTag("Player");
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
                charge_property->cooldown_timer += deltaTime;
                SquareCore::Vec2 enemy_position = GetPosition(enemy);
                float distance = std::abs(player_position.x - enemy_position.x);

                if ((distance <= charge_property->detection_range || charge_property->chasing) && charge_property->
                    cooldown_timer >= charge_property->charge_cooldown)
                {
                    charge_property->hit_player_this_attack = false;
                    charge_property->chasing = true;
                    charge_property->cooldown_timer = 0.0f;
                    float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                    SetRotation(enemy, direction * 90.0f);
                    FlipSprite(enemy, direction <= 0, false);
                    SetVelocity(enemy, direction * charge_property->charge_force, GetVelocity(enemy).y);
                }

                if (charge_property->cooldown_timer >= charge_property->charge_cooldown * 400.0f * deltaTime)
                {
                    SetRotation(enemy, 0.0f);
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
