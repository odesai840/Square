#include "Player.h"
#include "GameStateManager.h"
#include "PlayerKeybinds.h"

void Player::OnStart()
{
    player_data = GameStateManager::LoadPlayerData("Saves/S_001.square");
    player = AddEntity("Resources/Sprites/elvis-square.png", player_data.x_pos, player_data.y_pos, 0.0f, 0.05f, 0.05f, true);
    AddTagToEntity(player, "Player");
    FlipSprite(player, true, false);
    SetEntityPersistent(player, true);
    AddPropertyToEntity(player, new Character(10));

    slash_fps = 7.0f / slash_length;
    slash = AddAnimatedEntity("Resources/Sprites/slash-sheet-2.png", 7, slash_fps, player_data.x_pos, player_data.y_pos, 0.0f, 0.25f, 0.1f, false);
    SetColliderType(slash, SquareCore::ColliderType::TRIGGER);
    AddTagToEntity(slash, "PlayerSlash");
    SetEntityVisible(slash, false);
    SetEntityPersistent(slash, true);

    dash_fps = 6.0f / dash_length;
    dash = AddAnimatedEntity("Resources/Sprites/dash-sheet.png", 6, dash_fps, player_data.x_pos, player_data.y_pos, 0.0f, 1.0f, 0.5f, false);
    SetColliderType(dash, SquareCore::ColliderType::NONE);
    AddTagToEntity(dash, "PlayerDash");
    SetEntityVisible(dash, false);
    SetEntityPersistent(dash, true);
    SetEntityColor(dash, SquareCore::RGBA(200, 200, 200, 255));

    
    projectile_fps = 100.0f;
    for (int i = 0; i < 5; i++)
    {
        int slot = projectile_pool.Alloc();
        ProjectileEntity* projectile = static_cast<ProjectileEntity*>(projectile_pool.GetPointer(slot));
        projectile->id = AddAnimatedEntity("Resources/Sprites/projectile-sheet.png", 4, projectile_fps, player_data.x_pos, player_data.y_pos, 0.0f, 0.1f, 0.1f, false);
        projectile->active = false;
        projectile->timer = 0.0f;
        projectile->direction = Direction::LEFT;

        SetColliderType(projectile->id, SquareCore::ColliderType::TRIGGER);
        AddTagToEntity(projectile->id, "PlayerProjectile");
        SetEntityVisible(projectile->id, false);
        SetEntityPersistent(projectile->id, true);
        SetColliderBox(projectile->id, 50.0f, 25.0f);
        SetZIndex(projectile->id, -1);
    }
}

void Player::OnUpdate(float delta_time)
{
    Move(delta_time);
    Jump(delta_time);
    Dash(delta_time);
    Slash(delta_time);
    Projectile(delta_time);
    OnCollision(delta_time);
    UpdateBounceEntities(delta_time);

    if (!can_take_damage)
    {
        can_take_damage_timer += delta_time;
        if (can_take_damage_timer >= can_take_damage_cooldown)
        {
            can_take_damage = true;
            can_take_damage_timer = 0.0f;
        }
    }

    player_data.x_pos = GetPosition(player).x;
    player_data.y_pos = GetPosition(player).y;
    for (auto& player_property : GetAllEntityProperties(player))
    {
        if (Character* player_character = dynamic_cast<Character*>(player_property))
        {
            player_data.health = player_character->health;
        }
    }
    player_data.level = level;

    // DEBUG KEYS
    if (GetKeyPressed(debug_save))
        GameStateManager::SavePlayerData("Saves/S_001.square", player_data);
    if (GetKeyPressed(debug_collision))
        ToggleDebugCollisions();
    if (GetKeyPressed(debug_mouse_cursor))
    {
        mouse_visible = !mouse_visible;
        SetMouseVisible(mouse_visible);
    }
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
        FlipSprite(player, true, false);
        target_x = SquareCore::Lerp(player_velocity.x, -move_speed, acceleration * delta_time);
    }
    else if (GetKeyHeld(move_right_bind))
    {
        player_direction = Direction::RIGHT;
        FlipSprite(player, false, false);
        target_x = SquareCore::Lerp(player_velocity.x, move_speed, acceleration * delta_time);
    }

    bool up_held = GetKeyHeld(look_up_bind);
    bool down_held = GetKeyHeld(look_down_bind);
    
    if (up_held && down_held)
    {
        is_looking_up = last_vertical_look_was_up;
        is_looking_down = !last_vertical_look_was_up;
    }
    else if (up_held)
    {
        is_looking_up = true;
        is_looking_down = false;
        last_vertical_look_was_up = true;
    }
    else if (down_held)
    {
        is_looking_up = false;
        is_looking_down = true;
        last_vertical_look_was_up = false;
    }
    else
    {
        is_looking_up = false;
        is_looking_down = false;
    }

    SetVelocity(player, target_x, player_velocity.y);
}

void Player::Jump(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);

    bool grounded = IsGrounded(player);

    if (grounded)
        can_double_jump = true;
    
    if (GetKeyPressed(jump_bind) && (grounded || (has_double_jump && can_double_jump)))
    {
        if (!grounded)
        {
            can_double_jump = false;
            SetVelocity(player, player_velocity.x, jump_velocity / 1.2f);
        }
        else
        {
            SetVelocity(player, player_velocity.x, jump_velocity);
        }
    }
    
    if (!grounded && player_velocity.y < 0.0f)
    {
        player_velocity = GetVelocity(player);
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
        
        player_velocity = GetVelocity(player);
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

void Player::OnCollision(float delta_time)
{
    bool bouncing = false;
    bool onlyCollidingWithTop = true;
    uint32_t bounce_entity = 0;
    std::vector<std::pair<uint32_t, int>> collisions = GetEntityCollisions(player);
    for (const auto& collision : collisions)
    {
        if (!EntityExists(collision.first)) continue;
        
        // player collides with an enemy
        if (EntityHasTag(collision.first, "Enemy"))
        {
            float knockback_x = (player_direction == Direction::RIGHT ? -1.0f : 1.0f) * 500.0f;
            float knockback_y = collision.second != 0 ? 200.0f : 0.0f;
            collision.second == 0 ? knockback_x *= 10.0f : knockback_x *= 1.0f;

            SetVelocity(player, knockback_x, knockback_y);

            bool can_hit = true;
            JumpEnemy* jump_enemy = nullptr;
            ChargeEnemy* charge_enemy = nullptr;
            JumpBoss* jump_boss = nullptr;
    
            for (auto& enemy_property : GetAllEntityProperties(collision.first))
            {
                if (JumpEnemy* je = dynamic_cast<JumpEnemy*>(enemy_property))
                {
                    jump_enemy = je;
                    if (jump_enemy->hit_player_this_attack)
                        can_hit = false;
                }
                if (ChargeEnemy* ce = dynamic_cast<ChargeEnemy*>(enemy_property))
                {
                    charge_enemy = ce;
                    if (charge_enemy->hit_player_this_attack)
                        can_hit = false;
                }
                if (JumpBoss* jb = dynamic_cast<JumpBoss*>(enemy_property))
                {
                    jump_boss = jb;
                    if (jump_boss->hit_player_this_attack)
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
                                if (charge_enemy)
                                    charge_enemy->hit_player_this_attack = true;
                                if (jump_boss)
                                    jump_boss->hit_player_this_attack = true;
                        
                                TakeDamage(player_character, enemy_character->damage);
                            }
                        }
                    }
                }
            }
        }
        
        if (collision.second == 2 && EntityHasTag(collision.first, "ValidGround"))
        {
            last_grounded_position = GetPosition(player);
        }

        // player collides with a bouncy object
        if (EntityHasTag(collision.first, "Bounce"))
        {
            bouncing = true;
            bounce_entity = collision.first;
            if (collision.second != 2)
                onlyCollidingWithTop = false;
        }

        if (EntityHasTag(collision.first, "Spike"))
        {
            for (auto& player_property : GetAllEntityProperties(player))
            {
                if (Character* player_character = dynamic_cast<Character*>(player_property))
                {
                    TakeDamage(player_character, 1);
                }
            }
            SetVelocity(player, 0.0f, 0.0f);
            SetPosition(player, last_grounded_position.x, last_grounded_position.y);
            break;
        }
    }

    if (bouncing && onlyCollidingWithTop && bounce_entity)
        HandleBounceEntities(delta_time, bounce_entity);
}

void Player::TakeDamage(Character* player_character, int damage)
{
    if (!can_take_damage) return;
    can_take_damage = false;
    
    player_character->health -= damage;
    SDL_Log(("Player health: " + std::to_string(player_character->health)).c_str());

    if (player_character->health <= 0)
    {
        SetTimeScale(0.0f);
        SDL_Log("Player died");
    }
}

void Player::DealDamage(Character* enemy_character, uint32_t enemy_id, int damage)
{
    enemy_character->health -= damage;
    SDL_Log(("Enemy : " + std::to_string(enemy_id) + " now has " + std::to_string(enemy_character->health) + " health").c_str());

    if (enemy_character->health <= 0)
    {
        SDL_Log(("Enemy : " + std::to_string(enemy_id) + " died").c_str());
        enemies_to_remove.push_back(enemy_id);
    }
}

void Player::HandleBounceEntities(float delta_time, uint32_t current_bounce_entity)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    float bounce = 1200.0f;
    SetVelocity(player, player_velocity.x, bounce);
    
    bool already_bouncing = false;
    for (const auto& bounce : recently_bounced_on)
    {
        if (bounce.id == current_bounce_entity)
        {
            already_bouncing = true;
            break;
        }
    }
    
    if (!already_bouncing)
    {
        BounceEntity new_bounce;
        new_bounce.id = current_bounce_entity;
        new_bounce.original_scale_y = GetScale(current_bounce_entity).y;
        new_bounce.original_position_y = GetPosition(current_bounce_entity).y;
        recently_bounced_on.push_back(new_bounce);
        
        SquareCore::Vec2 bounce_position = GetPosition(current_bounce_entity);
        SetScale(current_bounce_entity, SquareCore::Vec2(0.15f, 0.075f));
        SetPosition(current_bounce_entity, bounce_position.x, bounce_position.y - 20.0f);
    }
}

void Player::UpdateBounceEntities(float delta_time)
{
    float lerp_speed = 5.0f * delta_time;
    
    for (int i = recently_bounced_on.size() - 1; i >= 0; i--)
    {
        BounceEntity& squish = recently_bounced_on[i];
        
        if (!EntityExists(squish.id))
        {
            recently_bounced_on.erase(recently_bounced_on.begin() + i);
            continue;
        }
        
        SquareCore::Vec2 current_scale = GetScale(squish.id);
        SquareCore::Vec2 current_position = GetPosition(squish.id);
        
        float new_scale_y = SquareCore::Lerp(current_scale.y, squish.original_scale_y, lerp_speed);
        float new_position_y = SquareCore::Lerp(current_position.y, squish.original_position_y, lerp_speed);
        
        SetScale(squish.id, {current_scale.x, new_scale_y});
        SetPosition(squish.id, current_position.x, new_position_y);
        
        if (abs(new_scale_y - squish.original_scale_y) < 0.001f && abs(new_position_y - squish.original_position_y) < 0.5f)
        {
            SetScale(squish.id, {current_scale.x, squish.original_scale_y});
            SetPosition(squish.id, current_position.x, squish.original_position_y);
            recently_bounced_on.erase(recently_bounced_on.begin() + i);
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