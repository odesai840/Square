#include "UserInterface.h"
#include "PlayerKeybinds.h"

void UserInterface::OnStart()
{
    dialogBox = AddUIRect(50, 800, 1820, 250, SquareCore::RGBA(20, 20, 20, 220), "",
                          {SquareCore::RGBA(255, 255, 255, 255), 2.0f, 5.0f});
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

    float boxWidth = 400.0f;
    float boxHeight = 400.0f;
    pauseMenuBox = AddUIRect(1920.0f / 2.0f - boxWidth / 2.0f, 1080.0f / 2.0f - boxHeight / 2.0f, boxWidth, boxHeight,
                             SquareCore::RGBA(20, 20, 20, 230), "", {SquareCore::RGBA(0, 0, 0, 0)});
    pauseMenuResumeButton = AddUIButton(1920.0f / 2.0f - 150.0f, 1080.0f / 2.0f - 125.0f, 300, 75,
                                        SquareCore::RGBA(50, 50, 50, 255), "RESUME", {SquareCore::RGBA(0, 0, 0, 0)},
                                        [this] { Pause(); }, "Resources/Fonts/Helvetica.ttf", 32, SquareCore::RGBA(255, 255, 255, 255));
    pauseMenuQuitButton = AddUIButton(1920.0f / 2.0f - 150.0f, 1080.0f / 2.0f + 25.0f, 300, 75,
                                        SquareCore::RGBA(50, 50, 50, 255), "QUIT", {SquareCore::RGBA(0, 0, 0, 0)},
                                        [this] { Quit(); }, "Resources/Fonts/Helvetica.ttf", 32, SquareCore::RGBA(255, 255, 255, 255));

    SetUIElementVisible(pauseMenuBox, false);
    SetUIElementVisible(pauseMenuResumeButton, false);
    SetUIElementVisible(pauseMenuQuitButton, false);

    SetUIElementPersistent(pauseMenuBox, true);
    SetUIElementPersistent(pauseMenuResumeButton, true);
    SetUIElementPersistent(pauseMenuQuitButton, true);

    SetButtonSprites(pauseMenuResumeButton, "Resources/Sprites/albinauric_fatty.png", "Resources/Sprites/fish.png","Resources/Sprites/albinauric_fatty.png");
    SetButtonSprites(pauseMenuQuitButton, "Resources/Sprites/fish.png", "Resources/Sprites/albinauric_fatty.png","Resources/Sprites/albinauric_fatty.png");

    LoadScene(level_path);
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

    if (GetKeyPressed(pause_bind))
        Pause();
}

void UserInterface::Pause()
{
    if (paused)
    {
        paused = false;
        SetUIElementVisible(pauseMenuBox, false);
        SetUIElementVisible(pauseMenuResumeButton, false);
        SetUIElementVisible(pauseMenuQuitButton, false);
        SetTimeScale(1.0f);
    }
    else if (!paused)
    {
        paused = true;
        SetUIElementVisible(pauseMenuBox, true);
        SetUIElementVisible(pauseMenuResumeButton, true);
        SetUIElementVisible(pauseMenuQuitButton, true);    
        SetTimeScale(0.0f);
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
