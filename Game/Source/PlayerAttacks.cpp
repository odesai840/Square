#include "Player.h"
#include "GameStateManager.h"
#include "PlayerKeybinds.h"

void Player::Slash(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    SquareCore::Vec2 player_position = GetPosition(player);
    
    if (GetMouseButtonPressed(0) && !is_slashing && !slash_in_cooldown && !(player_velocity.x > 600.0f || player_velocity.x < -600.0f))
    {
        is_slashing = true;
        slash_in_cooldown = true;
        slash_cooldown_elapsed = 0.0f;
        
        if (is_looking_up)
        {
            slash_direction = Direction::UP;
            SquareCore::Vec2 offset_position = {0.0f, 75.0f};
            
            ResetAnimation(slash);
            SetPosition(slash, player_position.x + offset_position.x + (player_velocity.x / 10.0f), player_position.y + offset_position.y);
            SetRotation(slash, 90.0f);
            SetScale(slash, SquareCore::Vec2(0.2f, 0.15f));
            FlipSprite(slash, false, true);
            SetEntityVisible(slash, true);
        }
        else if (is_looking_down && !IsGrounded(player))
        {
            slash_direction = Direction::DOWN;
            SquareCore::Vec2 offset_position = {0.0f, -75.0f};
            
            ResetAnimation(slash);
            SetPosition(slash, player_position.x + offset_position.x + (player_velocity.x / 10.0f), player_position.y + offset_position.y);
            SetRotation(slash, -90.0f);
            SetScale(slash, SquareCore::Vec2(0.2f, 0.15f));
            FlipSprite(slash, false, false);
            SetEntityVisible(slash, true);
        }
        else
        {
            slash_direction = player_direction;
            SquareCore::Vec2 offset_position = {0.0f, 10.0f};
            if (slash_direction == Direction::RIGHT)
                offset_position.x = 100.0f;
            else if (slash_direction == Direction::LEFT)
                offset_position.x = -100.0f;

            ResetAnimation(slash);
            SetPosition(slash, player_position.x + offset_position.x + (player_velocity.x / 10.0f), player_position.y + offset_position.y);
            SetRotation(slash, 0.0f);
            SetScale(slash, SquareCore::Vec2(0.25f, 0.1f));
            FlipSprite(slash, GetFlipX(player), false);
            SetEntityVisible(slash, true);
        }
    }

    if (is_slashing)
    {
        slash_duration += delta_time;

        std::vector<std::pair<uint32_t, int>> collisions = GetEntityCollisions(slash);
        for (std::pair<uint32_t, int>& collision : collisions)
        {
            if (!EntityExists(collision.first)) continue;

            if (EntityHasTag(collision.first, "Pogo"))
            {
                if (slash_direction == Direction::DOWN)
                {
                    SquareCore::Vec2 player_velocity = GetVelocity(player);
                    float pogo_bounce = 800.0f;
                    SetVelocity(player, player_velocity.x, pogo_bounce);
                }
            }
        
            if (EntityHasTag(collision.first, "Enemy") && !EnemyHitByCurrentSlash(collision.first))
            {
                damaged_by_slash_enemies.push_back(collision.first);
                
                for (auto& property : GetAllEntityProperties(collision.first))
                {
                    if (ChargeEnemy* charge_enemy = dynamic_cast<ChargeEnemy*>(property))
                    {
                        charge_enemy->aware_of_player = true;
                    }
                }
                
                SquareCore::Vec2 enemy_velocity = GetVelocity(collision.first);
                SquareCore::Vec2 enemy_position = GetPosition(collision.first);

                float direction_x = enemy_position.x - player_position.x;
                float knockback_x = (direction_x > 0 ? 1.0f : -1.0f) * slash_knockback;
                float knockback_y = 0.0f;
            
                if (slash_direction == Direction::UP)
                {
                    knockback_y = slash_knockback;
                }
                else if (slash_direction == Direction::DOWN)
                {
                    knockback_y = -slash_knockback;
                }
                else
                {
                    knockback_y = 200.0f;
                }
            
                SetVelocity(collision.first, enemy_velocity.x + knockback_x, enemy_velocity.y + knockback_y);
            
                for (auto& property : GetAllEntityProperties(collision.first))
                {
                    if (Character* health_property = dynamic_cast<Character*>(property))
                    {
                        health_property->health -= 1;
                        SDL_Log(("Enemy : " + std::to_string(collision.first) + " now has " + std::to_string(health_property->health) + " health").c_str());

                        if (health_property->health <= 0)
                        {
                            SDL_Log(("Enemy : " + std::to_string(collision.first) + " died").c_str());
                            enemies_to_remove.push_back(collision.first);
                        }
                    }
                }
            }
        }

        if (slash_duration >= slash_length)
        {
            SetEntityVisible(slash, false);
            damaged_by_slash_enemies.clear();
            slash_duration = 0.0f;
            is_slashing = false;
        }
    }

    if (slash_in_cooldown)
    {
        slash_cooldown_elapsed += delta_time;
        if (slash_cooldown_elapsed >= slash_cooldown)
        {
            slash_in_cooldown = false;
            slash_cooldown_elapsed = 0.0f;
        }
    }
}

void Player::Projectile(float delta_time)
{
}