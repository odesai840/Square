#pragma once

#include "json/json.hpp"

struct PlayerData
{
    float x_pos = 0;
    float y_pos = 0;
    int max_health = 5;
    int health = 5;
    int level = 1;
    int damage = 1;
    int heals = 3;
    int max_heals = 3;

    bool has_double_dash = false;
    bool has_double_jump = false;
    bool has_projectile = false;
    bool has_faster_slash = false;

    bool first_boss_dead = false;
    bool second_boss_dead = false;
    bool third_boss_dead = false;
};

class GameStateManager
{
public:
    static PlayerData LoadPlayerData(const std::string& save_path);
    static void SavePlayerData(const std::string& save_path, PlayerData data);
};