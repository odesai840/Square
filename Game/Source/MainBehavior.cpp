#include "MainBehavior.h"

void MainBehavior::OnStart() {
    player = AddEntity("Resources/Sprites/GaelPingu.png", 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, true);
    marcus = AddEntity("Resources/Sprites/marcus.png", 0.0f, -300.0f, 0.0f, 5.0f, 0.5f, false);
    
    music = AddAudioClip("Resources/Audio/AbyssOST.wav");
    SetAudioMasterVolume(0.5f);
    SetAudioLooping(music, true);
    PlayAudioClip(music);
}

void MainBehavior::OnUpdate(float deltaTime) {

    float moveSpeed = 350.0f;
        
    SquareCore::Vec2 targetVelocity = SquareCore::Vec2(GetVelocity(player).x, GetVelocity(player).y);
    if (IsKeyPressed(SDL_SCANCODE_A))
        targetVelocity.x -= moveSpeed * deltaTime;
    if (IsKeyPressed(SDL_SCANCODE_D))
        targetVelocity.x += moveSpeed * deltaTime;

    SetVelocity(player, targetVelocity.x, targetVelocity.y);
}
