#include "Player.h"
#include "GameStateManager.h"

void Player::OnStart()
{
    player_data = GameStateManager::LoadPlayerData("Saves/S_001.square");
    player = AddEntity("Resources/Sprites/square.png", player_data.x_pos, player_data.y_pos, 0.0f, 0.05f, 0.05f, true);
    AddTagToEntity(player, "Player");

    SetGravity(-1500.0f);
}

void Player::OnUpdate(float delta_time)
{
    Move(delta_time);
    Jump(delta_time);
    Dash(delta_time);
    Slash(delta_time);

    // DEBUG KEYS
    if (GetKeyPressed(debug_save))
        GameStateManager::SavePlayerData("Saves/S_001.square", player_data);
    if (GetKeyPressed(debug_collision))
        ToggleDebugCollisions();
    //
}

void Player::Move(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    float target_x = 0.0f;

    if (GetKeyHeld(move_left_bind))
    {
        current_direction = Direction::LEFT;
        FlipSprite(player, false, false);
        target_x = SquareCore::Lerp(player_velocity.x, -move_speed, acceleration * delta_time);
    }
    else if (GetKeyHeld(move_right_bind))
    {
        current_direction = Direction::RIGHT;
        FlipSprite(player, true, false);
        target_x = SquareCore::Lerp(player_velocity.x, move_speed, acceleration * delta_time);
    }

    SetVelocity(player, target_x, player_velocity.y);

    player_data.x_pos = GetPosition(player).x;
    player_data.y_pos = GetPosition(player).y;
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
        SetVelocity(player, (current_direction == Direction::LEFT ? -dash_velocity : dash_velocity) + player_velocity.x, player_velocity.y);
}

void Player::Slash(float delta_time)
{
    SquareCore::Vec2 player_velocity = GetVelocity(player);
    if (player_velocity.x > 600.0f) return;

    if (GetMouseButtonHeld(0))
    {
    }
} 

bool Player::IsGrounded(uint32_t playerId)
{
    auto collisions = GetEntityCollisions(playerId);
    for (const auto& collision : collisions)
    {
        if (collision.second == 2)
        {
            SquareCore::Vec2 vel = GetVelocity(playerId);
            if (vel.y < 0)
            {
                SetVelocity(playerId, vel.x, 0.0f);
            }
            return true;
        }
    }
    return false;
}