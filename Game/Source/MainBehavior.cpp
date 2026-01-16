#include "MainBehavior.h"

void MainBehavior::OnStart() {
    player = AddEntity("Resources/Sprites/GaelPingu.png", 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, true);
    marcus = AddEntity("Resources/Sprites/marcus.png", 0.0f, -300.0f, 0.0f, 5.0f, 0.5f, false);
    
    music = AddAudioClip("Resources/Audio/Cathedral KF.mp3");
    SetAudioMasterVolume(0.05f);
    SetAudioLooping(music, true);
    PlayAudioClip(music);

    ui_rect = AddUIRect(0.0f, 0.0f, 100.0f, 50.0f, SquareCore::RGBA(255, 255, 255, 255));
    
    ui_button = AddUIButton(500.0f, 500.0f, 200.0f, 50.0f, SquareCore::RGBA(255, 255, 255, 255), {true, SquareCore::RGBA(0, 0, 0, 255), 5.0f},
    [] {
        SDL_Log("Pressed button");
    });
    
    mouseFollower = AddSpritelessEntity(20.0f, 20.0f, 255, 0, 0, 255, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, false);
    SetColliderType(mouseFollower, SquareCore::ColliderType::NONE);
}

void MainBehavior::OnUpdate(float deltaTime) {
    SquareCore::Vec2 mousePos = ScreenToWorld(GetMousePosition());
    SetPosition(mouseFollower, mousePos.x, mousePos.y);

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
    
    if (GetKeyPressed(SDL_SCANCODE_W) && IsGrounded(player)) {
        float jumpVelocity = 600.0f;
        SetVelocity(player, newVelocityX, jumpVelocity);
    }
}

bool MainBehavior::IsGrounded(uint32_t playerId) {
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
