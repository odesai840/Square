#include "GameStateManager.h"
#include <SDL3/SDL_log.h>
#include <filesystem>
#include <fstream>

PlayerData GameStateManager::LoadPlayerData(const std::string& save_path)
{
    std::ifstream file(save_path);
    if (!file.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open file at path %s\n ... Proceeding with default data", save_path.c_str());
        return PlayerData();
    }
    
    nlohmann::json json;
    try
    {
        file >> json;
    }
    catch (const nlohmann::json::exception& e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to parse JSON at path %s ... %s\n ... Proceeding with default data", save_path.c_str(), e.what());
        return PlayerData();
    }

    PlayerData data = PlayerData();
    if (json.contains("x_pos"))
        data.x_pos = json["x_pos"];
    if (json.contains("y_pos"))
        data.y_pos = json["y_pos"];
    if (json.contains("health"))
        data.health = json["health"];
    if (json.contains("level"))
        data.level = json["level"];

    return data;
}

void GameStateManager::SavePlayerData(const std::string& save_path, PlayerData data)
{
    nlohmann::json json;
    json["x_pos"] = data.x_pos;
    json["y_pos"] = data.y_pos;
    json["health"] = data.health;
    json["level"] = data.level;

    std::filesystem::path path(save_path);
    if (path.has_parent_path())
    {
        std::filesystem::create_directories(path.parent_path());
    }
    
    std::ofstream file(save_path);
    if (!file.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open file at path %s", save_path.c_str());
        return;
    }

    try
    {
        file << json.dump(4);
    }
    catch (const nlohmann::json::exception& e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to write to file %s ... %s", save_path.c_str(), e.what());
    }
}