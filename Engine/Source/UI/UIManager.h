#pragma once

#include "UIElement.h"
#include <unordered_map>
#include <mutex>
#include <memory>
#include <functional>
#include "SDL3/SDL_mouse.h"
#include "Math/Math.h"

namespace SquareCore
{
    class UIManager
    {
    public:
        uint32_t AddRect(float x_pos, float y_pos, float width, float height, RGBA color, Border border = {false, RGBA(0, 0, 0, 255), 2.0f});
        uint32_t AddButton(float x_pos, float y_pos, float width, float height, RGBA color, Border border = {false, RGBA(0, 0, 0, 255), 2.0f}, std::function<void()> onPress = nullptr);
        void DeleteElement(uint32_t id);
        void ClearElements();

        UIElement* GetElementByID(uint32_t id);
        std::unordered_map<uint32_t, UIElement*>& GetElements() { return elements; }
        bool ElementExists(uint32_t id) const;

        void Update();
        
        void SetButtonCallback(uint32_t buttonID, std::function<void()> onPress);
        void SetElementVisible(uint32_t elementID, bool visible);
        void SetElementPosition(uint32_t elementID, float x, float y);
        void SetElementColor(uint32_t elementID, RGBA color);
        void SetElementZIndex(uint32_t elementID, int zIndex);

        Vec2 GetMousePosition() const;
        bool IsMouseButtonDown() const;
        bool IsMouseButtonPressed() const;

    private:
        mutable std::mutex uiMutex;
        std::unordered_map<uint32_t, UIElement*> elements;
        uint32_t nextElementID = 1;
    
        float mouseX = 0.0f;
        float mouseY = 0.0f;
        bool leftButtonDown = false;
        bool previousLeftButtonDown = false;
    
        bool PointInRect(float px, float py, float rx, float ry, float rw, float rh) const;
    };
}