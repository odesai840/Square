#pragma once

#include "Script.h"
#include "DialogManager.h"

class UserInterface : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

    DialogManager* GetDialogManager() { return &dialogManager; }

private:
    void DialogUpdate();
    void Pause();
    void UpdateHealthBar();
    
private:
    DialogManager dialogManager;
    uint32_t dialogBox = 0;
    uint32_t speakerText = 0;
    uint32_t dialogText = 0;
    
    uint32_t dialogTestTrigger = 0;

    uint32_t pauseMenuBox = 0;
    uint32_t pauseMenuResumeButton = 0;
    uint32_t pauseMenuQuitButton = 0;
    bool paused = false;

    uint32_t player = 0;
    std::vector<uint32_t> healthSquares;
    int maxHealth = 0;
};