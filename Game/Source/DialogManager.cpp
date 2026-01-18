#include "DialogManager.h"
#include <SDL3/SDL_log.h>
#include <json/json.hpp>
#include <fstream>

bool DialogManager::Load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open dialog file: %s", path.c_str());
        return false;
    }

    nlohmann::json json;
    try
    {
        file >> json;
    }
    catch (const nlohmann::json::exception& e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to parse dialog JSON: %s", e.what());
        return false;
    }

    for (auto& entry : json)
    {
        int id = entry["id"];
        std::vector<DialogLine> dialogLines;

        for (auto& line : entry["lines"])
        {
            DialogLine dl;
            dl.speaker = line.value("speaker", "");
            dl.text = line.value("text", "");
            dialogLines.push_back(dl);
        }

        entries[id] = dialogLines;
    }

    return true;
}

void DialogManager::Start(int entryId)
{
    if (entries.find(entryId) == entries.end())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Dialog entry not found: %d", entryId);
        return;
    }

    currentEntry = entryId;
    currentLine = 0;
    active = true;
}

void DialogManager::Advance()
{
    if (!active) return;

    currentLine++;
    if (currentLine >= entries[currentEntry].size())
    {
        Close();
    }
}

void DialogManager::Close()
{
    active = false;
    currentLine = 0;
    currentEntry = -1;
}

bool DialogManager::HasNext()
{
    if (!active) return false;
    return currentLine + 1 < entries[currentEntry].size();
}

DialogLine DialogManager::GetCurrentLine() const
{
    if (!active || entries.find(currentEntry) == entries.end())
        return {"", ""};

    return entries.at(currentEntry)[currentLine];
}
