#include "EnemyManager.h"

void EnemyManager::OnStart()
{
    for (unsigned int i = 0; i < jump_enemy_count; i++)
    {
        enemies.insert(enemies.end(), AddEntity("Resources/Sprites/grasshopper.png", 300.0f, -100.0f, 0.0f, 0.1f, 0.1f, true)); // 0.05f
        //SetEntityColor(enemies[i], SquareCore::RGBA(120, 0, 0, 255));
        FlipSprite(enemies[enemies.size()-1], true, false);
        AddTagToEntity(enemies[enemies.size()-1], "Enemy");
        SetDrag(enemies[enemies.size()-1], 5.0f);
        SetEntityPersistent(enemies[enemies.size()-1], true);
        AddPropertyToEntity(enemies[enemies.size()-1], new Character(5));
        AddPropertyToEntity(enemies[enemies.size()-1], new JumpEnemy(200.0f, 3.0f, {1000.0f, 1600.0f}));
    }
    for (unsigned int i = 0; i < charge_enemy_count; i++)
    {
        enemies.insert(enemies.end(), AddEntity("Resources/Sprites/grasshopper.png", -300.0f, -100.0f, 0.0f, 0.1f, 0.1f, true)); // 0.05f
        //SetEntityColor(enemies[i], SquareCore::RGBA(120, 0, 0, 255));
        FlipSprite(enemies[enemies.size()-1], true, false);
        AddTagToEntity(enemies[enemies.size()-1], "Enemy");
        SetDrag(enemies[enemies.size()-1], 5.0f);
        SetEntityPersistent(enemies[enemies.size()-1], true);
        AddPropertyToEntity(enemies[enemies.size()-1], new Character(5));
        AddPropertyToEntity(enemies[enemies.size()-1], new ChargeEnemy(800.0f, 3.0f, 2000.0f));
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
            // jump enemy logic
            if (JumpEnemy* jump_property = dynamic_cast<JumpEnemy*>(property))
            {
                jump_property->cooldown_timer += deltaTime;
                SquareCore::Vec2 enemy_position = GetPosition(enemy);
                float distance = std::abs(player_position.x - enemy_position.x);

                if ((distance <= jump_property->detection_range || jump_property->chasing) && jump_property->cooldown_timer >= jump_property->jump_cooldown)
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

                if ((distance <= charge_property->detection_range || charge_property->chasing) && charge_property->cooldown_timer >= charge_property->charge_cooldown)
                {
                    charge_property->hit_player_this_attack = false;
                    charge_property->chasing = true;
                    charge_property->cooldown_timer = 0.0f;
                    float direction = (player_position.x > enemy_position.x) ? 1.0f : -1.0f;
                    FlipSprite(enemy, direction <= 0, false);
                    SetVelocity(enemy, direction * charge_property->charge_force, GetVelocity(enemy).y);
                }
            }
        }
    }
}

