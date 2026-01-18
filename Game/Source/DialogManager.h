#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct DialogLine
{
    std::string speaker;
    std::string text;
};

class DialogManager
{
public:
    bool Load(const std::string& path);
    
    void Start(int entryId);
    void Advance();
    void Close();
    
    bool IsActive() const { return active; }
    bool HasNext();
    
    DialogLine GetCurrentLine() const;
    
private:
    std::unordered_map<int, std::vector<DialogLine>> entries;
    
    bool active = false;
    int currentEntry = -1;
    size_t currentLine = 0;
};
