#pragma once

#include "Script.h"
#include "DialogManager.h"
#include "Map.h"

class UserInterface : public SquareCore::Script
{
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;

    void OnPlay();

    DialogManager* GetDialogManager() { return &dialogManager; }
    void SetPlayerScript(Player* player_s) { this->player_script = player_s; }
    void SetMap(Map* m) { this->map = m; }

private:
    void DialogUpdate();
    void Pause();
    void UpdateHealthBar();
    void UpdateHeals();
    void ShowCredits(bool show);
    
private:
    Map* map = nullptr;
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
    Player* player_script = nullptr;
    std::vector<uint32_t> healthSquares;
    std::vector<uint32_t> healSquares;

    uint32_t main_menu_title = 0;
    uint32_t main_menu_play_button = 0;
    uint32_t main_menu_credits_button = 0;
    uint32_t main_menu_quit_button = 0;
    uint32_t main_menu_background = 0;

    uint32_t credits_title = 0;
    uint32_t credits_info_od = 0;
    uint32_t credits_info_ck = 0;
    uint32_t credits_back_button = 0;
};
