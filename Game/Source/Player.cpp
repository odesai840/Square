#include "Player.h"
#include "GameStateManager.h"
#include "PlayerKeybinds.h"

void Player::OnStart()
{
    player_data = GameStateManager::LoadPlayerData("Saves/S_001.square");
    player = AddEntity("Resources/Sprites/square.png", player_data.x_pos, player_data.y_pos, 0.0f, 0.05f, 0.05f, true);
    AddTagToEntity(player, "Player");
    SetEntityPersistent(player, true);
    AddPropertyToEntity(player, new Character(10));

    float slash_fps = 7.0f / slash_length;
    slash = AddAnimatedEntity("Resources/Sprites/slash-sheet.png", 7, slash_fps, player_data.x_pos, player_data.y_pos, 0.0f, 0.25f, 0.1f, false);
    SetColliderType(slash, SquareCore::ColliderType::TRIGGER);
    AddTagToEntity(slash, "PlayerSlash");
    SetEntityVisible(slash, false);
    SetEntityPersistent(slash, true);

    float dash_fps = 6.0f / dash_length;
    dash = AddAnimatedEntity("Resources/Sprites/dash-sheet.png", 6, dash_fps, player_data.x_pos, player_data.y_pos, 0.0f, 1.0f, 0.5f, false);
    SetColliderType(dash, SquareCore::ColliderType::TRIGGER);
    AddTagToEntity(dash, "PlayerDash");
    SetEntityVisible(dash, false);
    SetEntityPersistent(dash, true);
    SetEntityColor(dash, SquareCore::RGBA(200, 200, 200, 255));

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
    player_data.health = 10;
    player_data.level = level;

    // DEBUG KEYS
    if (GetKeyPressed(debug_save))
        GameStateManager::SavePlayerData("Saves/S_001.square", player_data);
    if (GetKeyPressed(debug_collision))
        ToggleDebugCollisions();
    if (GetKeyPressed(debug_hot_reload))
        LoadScene(level_path);
    if (GetKeyPressed(debug_mouse_cursor))
        SetMouseVisible(!mouse_visible);
    if (GetKeyPressed(debug_restart_game))
    {
        for (auto& player_property : GetAllEntityProperties(player))
        {
            if (Character* player_character = dynamic_cast<Character*>(player_property))
                player_character->health = 10;
        }
        SetTimeScale(1.0f);
    }
    //

    if (!enemies_to_remove.empty())
    {
        for (uint32_t enemy : enemies_to_remove)
        {
            RemoveEntity(enemy);
        }
        enemies_to_remove.clear();
    }
    
    FollowCameraTarget(GetPosition(player), 10.0f, delta_time);
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

    if (GetKeyHeld(look_up_bind))
        is_looking_up = true;
    else
        is_looking_up = false;

    SetVelocity(player, target_x, player_velocity.y);
}

void Player::Jump(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    
    if (GetKeyPressed(jump_bind) && IsGrounded(player))
        SetVelocity(player, player_velocity.x, jump_velocity);
    
    if (!IsGrounded(player) && player_velocity.y < 0.0f)
    {
        SetVelocity(player, player_velocity.x, player_velocity.y - (1500.0f * delta_time));
    }
}

void Player::Dash(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    SquareCore::Vec2 player_position = GetPosition(player);

    if (GetKeyPressed(dash_bind) && !is_dashing && player_velocity.x != 0.0f && !in_cooldown)
    {
        is_dashing = true;
        dashes_used++;
        time_since_last_dash = 0.0f;

        int max_dashes = has_double_dash ? 2 : 1;
        if (dashes_used >= max_dashes)
        {
            in_cooldown = true;
            dash_cooldown_elapsed = 0.0f;
        }
        
        SetVelocity(player, (player_direction == Direction::LEFT ? -dash_velocity : dash_velocity) + player_velocity.x, 0.0f);

        SquareCore::Vec2 offset_position = {0.0f, 0.0f};
        if (player_direction == Direction::RIGHT)
            offset_position.x = -25.0f;
        else if (player_direction == Direction::LEFT)
            offset_position.x = 25.0f;

        ResetAnimation(dash);
        SetPosition(dash, player_position.x + offset_position.x, player_position.y + offset_position.y);
        FlipSprite(dash, !GetFlipX(player), false);
        SetEntityVisible(dash, true);
        
        normal_player_scale_y = GetScale(player).y;
    }

    if (is_dashing)
    {
        dash_duration += delta_time;
        
        SquareCore::Vec2 player_scale = GetScale(player);
        float squished_scale_y = normal_player_scale_y / 1.5f;
        
        float squish_start = dash_length * 0.1f;
        float squish_end = dash_length * 0.9f;
        
        if (dash_duration <= squish_start)
        {
            float t = dash_duration / squish_start;
            float new_scale_y = SquareCore::Lerp(normal_player_scale_y, squished_scale_y, t);
            SetScale(player, {player_scale.x, new_scale_y});
        }
        else if (dash_duration >= squish_end)
        {
            float t = (dash_duration - squish_end) / (dash_length - squish_end);
            float new_scale_y = SquareCore::Lerp(squished_scale_y, normal_player_scale_y, t);
            SetScale(player, {player_scale.x, new_scale_y});
        }

        if (dash_duration >= dash_length)
        {
            SetEntityVisible(dash, false);
            dash_duration = 0.0f;
            is_dashing = false;
            
            SetScale(player, {player_scale.x, normal_player_scale_y});
        }
    }

    int max_dashes = has_double_dash ? 2 : 1;
    if (!in_cooldown && dashes_used > 0 && dashes_used < max_dashes)
    {
        time_since_last_dash += delta_time;
        if (time_since_last_dash > double_dash_window)
        {
            in_cooldown = true;
            dash_cooldown_elapsed = 0.0f;
        }
    }

    if (in_cooldown)
    {
        dash_cooldown_elapsed += delta_time;
        if (dash_cooldown_elapsed >= dash_cooldown)
        {
            in_cooldown = false;
            dashes_used = 0;
            time_since_last_dash = 0.0f;
            dash_cooldown_elapsed = 0.0f;
        }
    }
}

void Player::Slash(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    SquareCore::Vec2 player_position = GetPosition(player);
    
    if (GetMouseButtonPressed(0) && !is_slashing && !(player_velocity.x > 600.0f || player_velocity.x < -600.0f))
    {
        is_slashing = true;
        
        if (is_looking_up)
        {
            slash_direction = Direction::UP;
            SquareCore::Vec2 offset_position = {0.0f, 75.0f};
            
            ResetAnimation(slash);
            SetPosition(slash, player_position.x + offset_position.x+ (player_velocity.x / 10.0f), player_position.y + offset_position.y);
            SetRotation(slash, 90.0f);
            SetScale(slash, SquareCore::Vec2(0.2f, 0.15f));
            FlipSprite(slash, false, true);
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

            if (slash_direction == Direction::UP)
                offset_position.y = 0.0f;

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
            
            if (EntityHasTag(collision.first, "Enemy") && !EnemyHitByCurrentSlash(collision.first))
            {
                damaged_by_slash_enemies.push_back(collision.first);
                SquareCore::Vec2 enemy_velocity = GetVelocity(collision.first);
                SquareCore::Vec2 enemy_position = GetPosition(collision.first);

                float direction_x = enemy_position.x - player_position.x;
                float knockback_x = (direction_x > 0 ? 1.0f : -1.0f) * slash_knockback;
                float knockback_y = 0.0f;
                
                if (slash_direction == Direction::UP)
                {
                    knockback_y = slash_knockback;
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
}

void Player::OnCollision(float delta_time)
{
    bool bouncing = false;
    bool onlyCollidingWithTop = true;
    std::vector<std::pair<uint32_t, int>> collisions = GetEntityCollisions(player);
    for (const auto& collision : collisions)
    {
        if (!EntityExists(collision.first)) continue;
        
        // player collides with an enemy
        if (EntityHasTag(collision.first, "Enemy"))
        {
            SquareCore::Vec2 player_velocity = GetVelocity(player);
    
            float knockback_x = (player_direction == Direction::RIGHT ? -1.0f : 1.0f) * 500.0f;
            float knockback_y = 200.0f;

            SetVelocity(player, knockback_x, knockback_y);

            bool can_hit = true;
            JumpEnemy* jump_enemy = nullptr;
    
            for (auto& enemy_property : GetAllEntityProperties(collision.first))
            {
                if (JumpEnemy* je = dynamic_cast<JumpEnemy*>(enemy_property))
                {
                    jump_enemy = je;
                    if (jump_enemy->hit_player_this_attack)
                        can_hit = false;
                }
            }
    
            if (can_hit)
            {
                for (auto& enemy_property : GetAllEntityProperties(collision.first))
                {
                    if (Character* enemy_character = dynamic_cast<Character*>(enemy_property))
                    {
                        for (auto& player_property : GetAllEntityProperties(player))
                        {
                            if (Character* player_character = dynamic_cast<Character*>(player_property))
                            {
                                if (jump_enemy)
                                    jump_enemy->hit_player_this_attack = true;
                        
                                player_character->health -= enemy_character->damage;
                                SDL_Log(("Player health: " + std::to_string(player_character->health)).c_str());

                                if (player_character->health <= 0)
                                {
                                    SetTimeScale(0.0f);
                                }
                            }
                        }
                    }
                }
            }
        }

        // player collides with a bouncy object
        if (EntityHasTag(collision.first, "Bounce"))
        {
            bouncing = true;
            if (collision.second != 2)
                onlyCollidingWithTop = false;
        }
    }

    if (bouncing && onlyCollidingWithTop)
    {
        bouncing = false;
        SquareCore::Vec2 player_velocity = GetVelocity(player);
        float bounce = 1200.0f;
        SetVelocity(player, player_velocity.x, bounce);
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