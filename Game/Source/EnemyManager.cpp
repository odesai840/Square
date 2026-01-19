#include "EnemyManager.h"

void EnemyManager::OnStart()
{
    for (unsigned int i = 0; i < jump_enemy_count; i++)
    {
        enemies.insert(enemies.end(), AddEntity("Resources/Sprites/triangle.png", 300.0f, -100.0f, 0.0f, 0.05f, 0.05f, true));
        SetEntityColor(enemies[i], SquareCore::RGBA(120, 0, 0, 255));
        AddTagToEntity(enemies[i], "Enemy");
        SetDrag(enemies[i], 5.0f);
        SetEntityPersistent(enemies[i], true);
        AddPropertyToEntity(enemies[i], new Character(5));
        AddPropertyToEntity(enemies[i], new JumpEnemy(200.0f, 3.0f, {800.0f, 1200.0f}));
    }

    player = GetFirstEntityWithTag("Player");
}

void EnemyManager::OnUpdate(float deltaTime)
{
    if (player == 0) return;

    SquareCore::Vec2 player_position = GetPosition(player);

    for (uint32_t enemy : enemies)
    {
        if (!EntityExists(enemy)) continue;

        for (auto& property : GetAllEntityProperties(enemy))
        {
            if (JumpEnemy* jump_property = dynamic_cast<JumpEnemy*>(property))
            {
                jump_property->cooldown_timer += deltaTime;
                SquareCore::Vec2 enemy_position = GetPosition(enemy);
                float distance = std::abs(player_position.x - enemy_position.x);

                if ((distance <= jump_property->detection_range || jump_property->chasing) && jump_property->cooldown_timer >= jump_property->jump_cooldown)
                {
                    jump_property->hit_player_this_attack = false;
                    jump_property->cooldown_timer = true;
                    jump_property->cooldown_timer = 0.0f;
                    float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                    SetVelocity(enemy, direction * jump_property->jump_force.x, jump_property->jump_force.y);
                }
            }
        }
    }
}

