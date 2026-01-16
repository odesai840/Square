#include "UIManager.h"

namespace SquareCore
{
    uint32_t UIManager::AddRect(float x_pos, float y_pos, float width, float height, RGBA color, Border border)
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        UIRect* rect = new UIRect();
        rect->ID = nextElementID++;
        rect->x = x_pos;
        rect->y = y_pos;
        rect->width = width;
        rect->height = height;
        rect->color = color;
        rect->border = border;

        elements[rect->ID] = rect;
        return rect->ID;
    }

    uint32_t UIManager::AddButton(float x_pos, float y_pos, float width, float height, RGBA color, Border border,
                                  std::function<void()> onPress)
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        UIButton* button = new UIButton();
        button->ID = nextElementID++;
        button->x = x_pos;
        button->y = y_pos;
        button->width = width;
        button->height = height;
        button->color = color;
        button->border = border;
        button->onPress = onPress;

        elements[button->ID] = button;
        return button->ID;
    }

    void UIManager::DeleteElement(uint32_t id)
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        auto it = elements.find(id);
        if (it != elements.end())
        {
            delete it->second;
            elements.erase(it);
        }
    }

    void UIManager::ClearElements()
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        for (auto& [id, element] : elements)
        {
            delete element;
        }
        elements.clear();
    }

    void UIManager::Update()
    {
        std::function<void()> callbackExecute = nullptr;

        {
            std::lock_guard<std::mutex> lock(uiMutex);

            previousLeftButtonDown = leftButtonDown;

            SDL_MouseButtonFlags mouseState = SDL_GetMouseState(&mouseX, &mouseY);
            leftButtonDown = (mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0;

            for (auto& pair : elements)
            {
                UIElement* elem = pair.second;
                if (!elem->visible || elem->type != UIElementType::BUTTON) continue;

                UIButton* button = static_cast<UIButton*>(elem);

                button->isHovered = PointInRect(mouseX, mouseY,
                                                button->x, button->y,
                                                button->width, button->height);

                button->isPressed = button->isHovered && leftButtonDown;

                if (button->isHovered && !leftButtonDown && previousLeftButtonDown)
                {
                    if (button->onPress)
                    {
                        callbackExecute = button->onPress;
                        break;
                    }
                }
            }
        }

        if (callbackExecute)
        {
            callbackExecute();
        }
    }

    UIElement* UIManager::GetElementByID(uint32_t id)
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        auto it = elements.find(id);
        return (it != elements.end()) ? it->second : nullptr;
    }

    bool UIManager::ElementExists(uint32_t id) const
    {
        std::lock_guard<std::mutex> lock(uiMutex);
        return elements.find(id) != elements.end();
    }

    void UIManager::SetButtonCallback(uint32_t buttonID, std::function<void()> onPress)
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        auto it = elements.find(buttonID);
        if (it != elements.end() && it->second->type == UIElementType::BUTTON)
        {
            static_cast<UIButton*>(it->second)->onPress = onPress;
        }
    }

    void UIManager::SetElementVisible(uint32_t elementID, bool visible)
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        auto it = elements.find(elementID);
        if (it != elements.end())
        {
            it->second->visible = visible;
        }
    }

    void UIManager::SetElementPosition(uint32_t elementID, float x, float y)
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        auto it = elements.find(elementID);
        if (it != elements.end())
        {
            it->second->x = x;
            it->second->y = y;
        }
    }

    void UIManager::SetElementColor(uint32_t elementID, RGBA color)
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        auto it = elements.find(elementID);
        if (it != elements.end())
        {
            it->second->color = color;
        }
    }

    void UIManager::SetElementZIndex(uint32_t elementID, int zIndex)
    {
        std::lock_guard<std::mutex> lock(uiMutex);

        auto it = elements.find(elementID);
        if (it != elements.end())
        {
            it->second->zIndex = zIndex;
        }
    }

    // if it's not obvious, this one should not be replaced with input class since it doesn't actually do anything with input even though it's used alongside it
    bool UIManager::PointInRect(float px, float py, float rx, float ry, float rw, float rh) const
    {
        return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
    }

    // REPLACE LATER WITH INPUT CLASS
    Vec2 UIManager::GetMousePosition() const
    {
        return Vec2(mouseX, mouseY);
    }

    bool UIManager::IsMouseButtonDown() const
    {
        return leftButtonDown;
    }

    bool UIManager::IsMouseButtonPressed() const
    {
        return leftButtonDown && !previousLeftButtonDown;
    }
    // --
}
