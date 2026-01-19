#pragma once

#include "Renderer/EntityManager.h"
#include "UI/UIManager.h"
#include <string>

namespace SquareCore {

class SceneManager {
public:
    bool SaveScene(const std::string& filepath);
    bool LoadScene(const std::string& filepath);
    
    void SetEntityManager(EntityManager* entityManager) {entityManagerRef = entityManager; }
    void SetUIManager(UIManager* uiManager) { uiManagerRef = uiManager; }
    
private:
    EntityManager* entityManagerRef = nullptr;
    UIManager* uiManagerRef = nullptr;
    
};

}
