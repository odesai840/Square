#include "Player.h"
#include "GameStateManager.h"

// example for a ui button
/*ui_button = AddUIButton(500.0f, 500.0f, 200.0f, 50.0f, 
SquareCore::RGBA(255, 255, 255, 255), "Test", 
{SquareCore::RGBA(0, 0, 0, 255), 5.0f, 0.0f},
OnButtonPressedTest, "Resources/Fonts/Helvetica.ttf", 24.0f, 
SquareCore::RGBA(255, 0, 0, 255));*/

void Player::OnStart() {
    player_data = GameStateManager::LoadPlayerData("Saves/S_001.square");
    player = AddEntity("Resources/Sprites/square.png", player_data.x_pos, player_data.y_pos, 0.0f, 0.1f, 0.1f, true);
}

void Player::OnUpdate(float deltaTime) {
    SquareCore::Vec2 mousePos = ScreenToWorld(GetMousePosition());

    float moveSpeed = 350.0f;
    float acceleration = 15.0f;

    SquareCore::Vec2 currentVelocity = GetVelocity(player);
    float targetX = 0.0f;

    if (GetKeyHeld(SDL_SCANCODE_A)) {
        targetX = -moveSpeed;
        FlipSprite(player, false, false);
    }
    if (GetKeyHeld(SDL_SCANCODE_D)) {
        targetX = moveSpeed;
        FlipSprite(player, true, false);
    }

    float newVelocityX = SquareCore::Lerp(currentVelocity.x, targetX, acceleration * deltaTime);
    SetVelocity(player, newVelocityX, currentVelocity.y);
    
    if ((GetKeyPressed(SDL_SCANCODE_W) || GetKeyPressed(SDL_SCANCODE_SPACE)) && IsGrounded(player)) {
        float jumpVelocity = 600.0f;
        SetVelocity(player, newVelocityX, jumpVelocity);
    }

    player_data.x_pos = GetPosition(player).x;
    player_data.y_pos = GetPosition(player).y;

    std::vector<std::pair<uint32_t, int>> player_collisions = GetEntityCollisions(player);
    for (std::pair<uint32_t, int> collision : player_collisions)
    {
        if (EntityHasTag(collision.first, "Enemy"))
        {
            SDL_Log("Collided with an enemy");
        }
    }

    // DEBUG KEYS
    if (GetKeyPressed(SDL_SCANCODE_P))
        GameStateManager::SavePlayerData("Saves/S_001.square", player_data);
    if (GetKeyPressed(SDL_SCANCODE_T))
        ToggleDebugCollisions();
    //
}

bool Player::IsGrounded(uint32_t playerId) {
    auto collisions = GetEntityCollisions(playerId);
    for (const auto& collision : collisions) {
        if (collision.second == 2) {
            SquareCore::Vec2 vel = GetVelocity(playerId);
            if (vel.y < 0) {
                SetVelocity(playerId, vel.x, 0.0f);
            }
            return true;
        }
    }
    return false;
}
