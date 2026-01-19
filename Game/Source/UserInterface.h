#pragma once

#include "Script.h"
#include "DialogManager.h"

class UserInterface : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

private:
    void DialogUpdate();
    void Pause();
    
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
    
};
