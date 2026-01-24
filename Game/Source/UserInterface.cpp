#include "UserInterface.h"
#include "PlayerKeybinds.h"
#include "Properties.h"

void UserInterface::OnStart()
{
    main_menu_background = AddUIRect(0.0f, 0.0f, 1920, 1080,
                             SquareCore::RGBA(70, 70, 70, 255), "", {SquareCore::RGBA(0, 0, 0, 0)});
    //SetElementSprite(main_menu_background, "Resources/Sprites/main-menu.png");
    
    main_menu_title = AddUIText(1920.0f / 2.0f, 150.0f, 64, SquareCore::RGBA(255, 255, 255, 255), "Resources/Fonts/Helvetica.ttf", "SQUARE SOULS");
    SquareCore::Vec2 text_size = GetTextSize(main_menu_title);
    SetUIElementPosition(main_menu_title, 1920.0f / 2.0f - text_size.x / 2.0f, 150.0f);
    
    main_menu_play_button = AddUIButton(1920.0f / 2.0f - 150.0f, 1080.0f / 2.0f - 100.0f, 300, 75,
                                        SquareCore::RGBA(50, 50, 50, 255), "PLAY", {SquareCore::RGBA(0, 0, 0, 0)},
                                        [this] { map->LoadMap(player_script->GetPlayerData().level, {-100.0f, 200.0f}); OnPlay(); }, "Resources/Fonts/Helvetica.ttf", 32,
                                        SquareCore::RGBA(255, 255, 255, 255));
    main_menu_credits_button = AddUIButton(1920.0f / 2.0f - 150.0f, 1080.0f / 2.0f + 0.0f, 300, 75,
                                      SquareCore::RGBA(50, 50, 50, 255), "CREDITS", {SquareCore::RGBA(0, 0, 0, 0)},
                                      [this] { ShowCredits(true); }, "Resources/Fonts/Helvetica.ttf", 32,
                                      SquareCore::RGBA(255, 255, 255, 255));
    main_menu_quit_button = AddUIButton(1920.0f / 2.0f - 150.0f, 1080.0f / 2.0f + 100.0f, 300, 75,
                                      SquareCore::RGBA(50, 50, 50, 255), "QUIT", {SquareCore::RGBA(0, 0, 0, 0)},
                                      [this] { Quit(); }, "Resources/Fonts/Helvetica.ttf", 32,
                                      SquareCore::RGBA(255, 255, 255, 255));

    SetUIElementPersistent(main_menu_title, true);
    SetUIElementPersistent(main_menu_background, true);
    SetUIElementPersistent(main_menu_play_button, true);
    SetUIElementPersistent(main_menu_credits_button, true);
    SetUIElementPersistent(main_menu_quit_button, true);
    
    credits_title = AddUIText(1920.0f / 2.0f, 150.0f, 64, SquareCore::RGBA(255, 255, 255, 255), "Resources/Fonts/Helvetica.ttf", "CREDITS");
    text_size = GetTextSize(credits_title);
    SetUIElementPosition(credits_title, 1920.0f / 2.0f - text_size.x / 2.0f, 150.0f);
    credits_info_od = AddUIText(1920.0f / 2.0f, 300.0f, 24, SquareCore::RGBA(255, 255, 255, 255), "Resources/Fonts/Helvetica.ttf",
    "Ohm Desai - Lead Engine Programmer, Gameplay Programmer, Story, Art, Music, Level Design");
    text_size = GetTextSize(credits_info_od);
    SetUIElementPosition(credits_info_od, 1920.0f / 2.0f - text_size.x / 2.0f, 300.0f);
    credits_info_ck = AddUIText(1920.0f / 2.0f, 300.0f, 24, SquareCore::RGBA(255, 255, 255, 255), "Resources/Fonts/Helvetica.ttf",
    "Caleb Kronstad - Lead Gameplay Programmer, Engine Programmer, Story, Art, Music, Level Design");
    text_size = GetTextSize(credits_info_ck);
    SetUIElementPosition(credits_info_ck, 1920.0f / 2.0f - text_size.x / 2.0f, 350.0f);
    
    credits_back_button = AddUIButton(1920.0f / 2.0f - 150.0f, 900.0f, 300, 75,
                                      SquareCore::RGBA(50, 50, 50, 255), "BACK", {SquareCore::RGBA(0, 0, 0, 0)},
                                      [this] { ShowCredits(false); }, "Resources/Fonts/Helvetica.ttf", 32,
                                      SquareCore::RGBA(255, 255, 255, 255));
    
    SetUIElementPersistent(credits_title, true);
    SetUIElementPersistent(credits_info_od, true);
    SetUIElementPersistent(credits_info_ck, true);
    SetUIElementPersistent(credits_back_button, true);
    ShowCredits(false);


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
                                        [this] { Pause(); }, "Resources/Fonts/Helvetica.ttf", 32,
                                        SquareCore::RGBA(255, 255, 255, 255));
    pauseMenuQuitButton = AddUIButton(1920.0f / 2.0f - 150.0f, 1080.0f / 2.0f + 25.0f, 300, 75,
                                      SquareCore::RGBA(50, 50, 50, 255), "QUIT", {SquareCore::RGBA(0, 0, 0, 0)},
                                      [this] { Quit(); }, "Resources/Fonts/Helvetica.ttf", 32,
                                      SquareCore::RGBA(255, 255, 255, 255));

    SetUIElementVisible(pauseMenuBox, false);
    SetUIElementVisible(pauseMenuResumeButton, false);
    SetUIElementVisible(pauseMenuQuitButton, false);

    SetUIElementPersistent(pauseMenuBox, true);
    SetUIElementPersistent(pauseMenuResumeButton, true);
    SetUIElementPersistent(pauseMenuQuitButton, true);

    SetButtonSprites(pauseMenuResumeButton, "Resources/Sprites/albinauric_fatty.png", "Resources/Sprites/fish.png",
                     "Resources/Sprites/albinauric_fatty.png");
    SetButtonSprites(pauseMenuQuitButton, "Resources/Sprites/fish.png", "Resources/Sprites/albinauric_fatty.png",
                     "Resources/Sprites/albinauric_fatty.png");

    player = GetFirstEntityWithTag("Player");

    for (int i = 0; i < player_script->GetPlayerData().max_health; i++)
    {
        uint32_t healthSquare = AddUIRect(50.0f + i * 40.0f, 50.0f, 30.0f, 30.0f,
                                          SquareCore::RGBA(150, 0, 0, 255), "",
                                          {SquareCore::RGBA(0, 0, 0, 0)});
        SetUIElementPersistent(healthSquare, true);
        SetUIElementVisible(healthSquare, false);
        healthSquares.push_back(healthSquare);
    }
    for (int i = 0; i < player_script->GetPlayerData().max_heals; i++)
    {
        uint32_t healSquare = AddUIRect(50.0f + i * 40.0f, 100.0f, 30.0f, 30.0f,
                                          SquareCore::RGBA(200, 80, 65, 255), "",
                                          {SquareCore::RGBA(0, 0, 0, 0)});
        SetUIElementPersistent(healSquare, true);
        SetUIElementVisible(healSquare, false);
        healSquares.push_back(healSquare);
    }

    UpdateHealthBar();
    UpdateHeals();
}

void UserInterface::ShowCredits(bool show)
{
    if (show)
    {   
        SetUIElementVisible(credits_title, true);
        SetUIElementVisible(credits_info_od, true);
        SetUIElementVisible(credits_info_ck, true);
        SetUIElementVisible(credits_back_button, true);

        SetUIElementVisible(main_menu_title, false);
        SetUIElementVisible(main_menu_play_button, false);
        SetUIElementVisible(main_menu_credits_button, false);
        SetUIElementVisible(main_menu_quit_button, false);
        return;
    }
    SetUIElementVisible(credits_title, false);
    SetUIElementVisible(credits_info_od, false);
    SetUIElementVisible(credits_info_ck, false);
    SetUIElementVisible(credits_back_button, false);
    SetUIElementVisible(main_menu_title, true);
    SetUIElementVisible(main_menu_play_button, true);
    SetUIElementVisible(main_menu_credits_button, true);
    SetUIElementVisible(main_menu_quit_button, true);
}

void UserInterface::UpdateHeals()
{
    int current_heals = player_script->GetPlayerData().heals;
    for (int i = 0; i < healSquares.size(); i++)
    {
        if (i < current_heals)
            SetUIElementColor(healSquares[i], SquareCore::RGBA(200, 80, 65, 255));
        else
            SetUIElementColor(healSquares[i], SquareCore::RGBA(100, 100, 100, 255));
    }
}

void UserInterface::UpdateHealthBar()
{
    int currentHealth = 0;
    for (auto& property : GetAllEntityProperties(player))
    {
        if (Character* character = dynamic_cast<Character*>(property))
        {
            currentHealth = character->health;
            break;
        }
    }

    for (int i = 0; i < healthSquares.size(); i++)
    {
        if (i < currentHealth)
            SetUIElementColor(healthSquares[i], SquareCore::RGBA(150, 0, 0, 255));
        else
            SetUIElementColor(healthSquares[i], SquareCore::RGBA(100, 100, 100, 255));
    }
}

void UserInterface::OnUpdate(float deltaTime)
{
    UpdateHealthBar();
    UpdateHeals();

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

void UserInterface::OnPlay()
{
    for (uint32_t health_square : healthSquares)
    {
        SetUIElementVisible(health_square, true);
    }
    for (uint32_t heal_square : healSquares)
    {
        SetUIElementVisible(heal_square, true);
    }

    SetUIElementVisible(main_menu_title, false);
    SetUIElementVisible(main_menu_background, false);
    SetUIElementVisible(main_menu_play_button, false);
    SetUIElementVisible(main_menu_credits_button, false);
    SetUIElementVisible(main_menu_quit_button, false);
    SetUIElementVisible(credits_title, false);
    SetUIElementVisible(credits_info_od, false);
    SetUIElementVisible(credits_info_ck, false);
    SetUIElementVisible(credits_back_button, false);
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
