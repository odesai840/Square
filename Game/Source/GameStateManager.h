#pragma once

#include "json/json.hpp"

struct PlayerData
{
    float x_pos = 0;
    float y_pos = 0;
    int health = 0;
    int level = 0;
};

class GameStateManager
{
public:
    static PlayerData LoadPlayerData(const std::string& save_path);
    static void SavePlayerData(const std::string& save_path, PlayerData data);
};