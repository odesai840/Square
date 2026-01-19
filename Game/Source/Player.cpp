#include "Player.h"
#include "GameStateManager.h"

void Player::OnStart()
{
    player_data = GameStateManager::LoadPlayerData("Saves/S_001.square");
    player = AddEntity("Resources/Sprites/square.png", player_data.x_pos, player_data.y_pos, 0.0f, 0.05f, 0.05f, true);
    AddTagToEntity(player, "Player");
    SetEntityPersistent(player, true);

    AddPropertyToEntity(player, new Health());

    float slash_fps = 7.0f / slash_length;
    slash = AddAnimatedEntity("Resources/Sprites/slash-sheet.png", 7, slash_fps, player_data.x_pos, player_data.y_pos, 0.0f, 0.04f, 0.1f, false);
    SetColliderType(slash, SquareCore::ColliderType::TRIGGER);
    AddTagToEntity(slash, "PlayerSlash");
    SetEntityVisible(slash, false);
    SetEntityPersistent(slash, true);

    SetGravity(-1500.0f);
}

void Player::OnUpdate(float delta_time)
{
    Move(delta_time);
    Jump(delta_time);
    Dash(delta_time);
    Slash(delta_time);
    
    OnCollision(delta_time);

    player_data.x_pos = GetPosition(player).x;
    player_data.y_pos = GetPosition(player).y;
    player_data.health = health.value;
    player_data.level = level;

    // DEBUG KEYS
    if (GetKeyPressed(debug_save))
        GameStateManager::SavePlayerData("Saves/S_001.square", player_data);
    if (GetKeyPressed(debug_collision))
        ToggleDebugCollisions();
    if (GetKeyPressed(debug_hot_reload))
        LoadScene("Resources/Scenes/test.square");
    //
}

void Player::Move(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    float target_x = 0.0f;

    if (GetKeyHeld(move_left_bind))
    {
        player_direction = Direction::LEFT;
        FlipSprite(player, false, false);
        target_x = SquareCore::Lerp(player_velocity.x, -move_speed, acceleration * delta_time);
    }
    else if (GetKeyHeld(move_right_bind))
    {
        player_direction = Direction::RIGHT;
        FlipSprite(player, true, false);
        target_x = SquareCore::Lerp(player_velocity.x, move_speed, acceleration * delta_time);
    }

    SetVelocity(player, target_x, player_velocity.y);
}

void Player::Jump(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    
    if (GetKeyPressed(jump_bind) && IsGrounded(player))
        SetVelocity(player, player_velocity.x, jump_velocity);
}

void Player::Dash(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);

    if (GetKeyPressed(dash_bind))
        SetVelocity(player, (player_direction == Direction::LEFT ? -dash_velocity : dash_velocity) + player_velocity.x, player_velocity.y);
}

void Player::Slash(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    SquareCore::Vec2 player_position = GetPosition(player);
    
    if (GetKeyPressed(slash_bind) && !is_slashing && !(player_velocity.x > 600.0f || player_velocity.x < -600.0f))
    {
        is_slashing = true;
        slash_direction = player_direction;
    
        SquareCore::Vec2 offset_position = {0.0f, 10.0f};
        if (slash_direction == Direction::RIGHT)
            offset_position.x = 50.0f;
        else if (slash_direction == Direction::LEFT)
            offset_position.x = -50.0f;

        ResetAnimation(slash);
        SetPosition(slash, player_position.x + offset_position.x + (player_velocity.x / 5.0f), player_position.y + offset_position.y);
        FlipSprite(slash, GetFlipX(player), false);
        SetEntityVisible(slash, true);
    }

    if (is_slashing)
    {
        slash_duration += delta_time;

        std::vector<std::pair<uint32_t, int>> collisions = GetEntityCollisions(slash);
        for (std::pair<uint32_t, int>& collision : collisions)
        {
            if (EntityHasTag(collision.first, "Enemy") && !EnemyHitByCurrentSlash(collision.first))
            {
                damaged_by_slash_enemies.push_back(collision.first);
                SquareCore::Vec2 enemy_velocity = GetVelocity(collision.first);
                 
                float knockback_x = (slash_direction == Direction::RIGHT ? 1.0f : -1.0f) * slash_knockback;
                SetVelocity(collision.first, enemy_velocity.x + knockback_x, enemy_velocity.y + 200.0f);
                
                for (auto& property : GetAllEntityProperties(collision.first))
                {
                    if (Health* health_property = dynamic_cast<Health*>(property))
                    {
                        health_property->value -= 1;
                        SDL_Log(("Enemy : " + std::to_string(collision.first) + " now has " + std::to_string(health_property->value) + " health").c_str());
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
}

void Player::OnCollision(float delta_time)
{
    std::vector<std::pair<uint32_t, int>> collisions = GetEntityCollisions(player);
    for (const auto& collision : collisions)
    {
        // player collides with an enemy
        if (EntityHasTag(collision.first, "Enemy"))
        {
            SquareCore::Vec2 player_velocity = GetVelocity(player);
            SquareCore::Vec2 player_pos = GetPosition(player);
            SquareCore::Vec2 enemy_pos = GetPosition(collision.first);
            
            float direction_x = player_pos.x - enemy_pos.x;
            float knockback_x = (direction_x > 0 ? 1.0f : -1.0f) * 7.0f;
            
            SetVelocity(player, player_velocity.x + knockback_x, player_velocity.y + 2.0f);
        }
    }
}

bool Player::EnemyHitByCurrentSlash(uint32_t enemy_id)
{
    for (uint32_t id : damaged_by_slash_enemies)
    {
        if (enemy_id == id)
            return true;
    }
    return false;
}

bool Player::IsGrounded(uint32_t playerId)
{
    auto collisions = GetEntityCollisions(playerId);
    for (const auto& collision : collisions)
    {
        if (collision.second == 2 && !EntityHasTag(collision.first, "PlayerSlash"))
        {
            SquareCore::Vec2 vel = GetVelocity(playerId);
            if (vel.y < 0)
            {
                SetVelocity(playerId, vel.x, vel.y);
            }
            return true;
        }
    }
    return false;
}