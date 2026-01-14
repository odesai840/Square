#pragma once

#include "Math/Math.h"
#include <functional>
#include <cstdint>

#include "UI/Color.h"

namespace SquareCore
{
    enum class UIElementType
    {
        RECT,
        BUTTON
    };

    struct UIElement
    {
        uint32_t ID = 0;
        UIElementType type = UIElementType::RECT;
        float x = 0.0f;
        float y = 0.0f;
        float width = 100.0f;
        float height = 50.0f;
        bool visible = true;
        RGBA color = RGBA(255, 255, 255, 255);
        int zIndex = 0;
    };

    struct UIRect : UIElement
    {
        UIRect() { type = UIElementType::RECT; }
    };

    struct UIButton : UIElement
    {
        UIButton() { type = UIElementType::BUTTON; }

        bool isHovered = false;
        bool isPressed = false;

        RGBA hoverColor = RGBA(200, 200, 200, 255);
        RGBA pressedColor = RGBA(150, 150, 150, 255);

        std::function<void()> onPress;
    };
}
