#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

struct DialogLine
{
    std::string speaker;
    std::string text;
    bool seen = false;
};

class DialogManager
{
public:
    bool Load(const std::string& path);
    
    void Start(int entryId);
    void Advance();
    void Close();
    
    bool IsActive() const { return active; }
    bool HasBeenSeen(int entryId) const { return seenEntries.count(entryId) > 0; }
    
    DialogLine* GetCurrentLine() const;
    
private:
    std::unordered_map<int, std::vector<DialogLine*>> entries;
    std::unordered_set<int> seenEntries;
    
    bool active = false;
    int currentEntry = -1;
    size_t currentLine = 0;
};
