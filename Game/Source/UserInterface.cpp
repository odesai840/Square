#include "UserInterface.h"

void UserInterface::OnStart()
{
    dialogBox = AddUIRect(50, 800, 1820, 250, SquareCore::RGBA(20, 20, 20, 220), "", {SquareCore::RGBA(255, 255, 255, 255), 2.0f, 5.0f});
    speakerText = AddUIText(100, 850, 32, SquareCore::RGBA(255, 200, 100, 255), "Resources/Fonts/Helvetica.ttf", "");
    dialogText = AddUIText(100, 900, 24, SquareCore::RGBA(255, 255, 255, 255), "Resources/Fonts/Helvetica.ttf", "");

    SetUIElementVisible(dialogBox, false);
    SetUIElementVisible(speakerText, false);
    SetUIElementVisible(dialogText, false);
    
    SetUIElementPersistent(dialogBox, true);
    SetUIElementPersistent(speakerText, true);
    SetUIElementPersistent(dialogText, true);
    
    dialogTestTrigger = GetFirstEntityWithTag("DialogTrigger");
    
    dialogManager.Load("Resources/Data/dialog.json");
    
    LoadScene("Resources/Scenes/test.square");
}

void UserInterface::OnUpdate(float deltaTime)
{
    if (!dialogManager.IsActive() && !dialogManager.HasBeenSeen(0))
    {
        auto collisions = GetEntityCollisions(dialogTestTrigger);
        for (const auto& collision : collisions)
        {
            if (EntityHasTag(collision.first, "Player"))
            {
                dialogManager.Start(0);
                break;
            }
        }
    }
    
    if (dialogManager.IsActive())
    {
        DialogUpdate();
    }
    else
    {
        SetUIElementVisible(dialogBox, false);
        SetUIElementVisible(speakerText, false);
        SetUIElementVisible(dialogText, false);
    }
}

void UserInterface::DialogUpdate()
{
    DialogLine* line = dialogManager.GetCurrentLine();
    if (!line) return;
    
    SetUIElementVisible(dialogBox, true);
    SetUIElementVisible(speakerText, true);
    SetUIElementVisible(dialogText, true);
    
    SetUIText(speakerText, line->speaker);
    SetUIText(dialogText, line->text);
    
    if (GetMouseButtonPressed(0))
    {
        dialogManager.Advance();
    }
}
