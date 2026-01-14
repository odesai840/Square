#include "MainBehavior.h"

void MainBehavior::OnStart() {
    player = AddEntity("Resources/Sprites/GaelPingu.png", 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, true);
    marcus = AddEntity("Resources/Sprites/marcus.png", 0.0f, -300.0f, 0.0f, 5.0f, 0.5f, false);
    
    music = AddAudioClip("Resources/Audio/Cathedral KF.mp3");
    SetAudioMasterVolume(0.05f);
    SetAudioLooping(music, true);
    PlayAudioClip(music);

    ui_rect = AddUIRect(0.0f, 0.0f, 100.0f, 50.0f, SquareCore::RGBA(255, 255, 255, 255));
    
    ui_button = AddUIButton(500.0f, 500.0f, 200.0f, 50.0f, SquareCore::RGBA(255, 255, 255, 255),
    [] {
        SDL_Log("Pressed button");
    });
}

void MainBehavior::OnUpdate(float deltaTime) {

    float moveSpeed = 350.0f;
        
    SquareCore::Vec2 targetVelocity = SquareCore::Vec2(GetVelocity(player).x, GetVelocity(player).y);
    if (IsKeyPressed(SDL_SCANCODE_A))
    {
        targetVelocity.x -= moveSpeed * deltaTime;
        FlipSprite(player, false, false);
    }
    if (IsKeyPressed(SDL_SCANCODE_D))
    {
        targetVelocity.x += moveSpeed * deltaTime;
        FlipSprite(player, true, false);
    }

    SetVelocity(player, targetVelocity.x, targetVelocity.y);
}
