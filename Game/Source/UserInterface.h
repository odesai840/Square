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
    
private:
    DialogManager dialogManager;
    uint32_t dialogBox = 0;
    uint32_t speakerText = 0;
    uint32_t dialogText = 0;
    
    uint32_t dialogTestTrigger = 0;
    
    int counter = 1;
};
