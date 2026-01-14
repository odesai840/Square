#include "Input.h"

namespace SquareCore {

Input::Input() {
    std::memset(keyStates, 0, sizeof(keyStates));
    std::memset(mouseButtonStates, 0, sizeof(mouseButtonStates));
}

void Input::UpdateKeyState(SDL_Scancode key, bool pressed) {
    if (key < 0 || key >= SDL_SCANCODE_COUNT) return;

    if (pressed) {
        keyStates[key] = KeyState::PRESSED;
    } else {
        keyStates[key] = KeyState::RELEASED;
    }
}

void Input::UpdateMouseButtonState(int button, bool pressed) {
    if (button < 0 || button >= MAX_MOUSE_BUTTONS) return;

    if (pressed) {
        mouseButtonStates[button] = KeyState::PRESSED;
    } else {
        mouseButtonStates[button] = KeyState::RELEASED;
    }
}

void Input::UpdateMousePosition(float x, float y) {
    previousMousePosition = mousePosition;
    mousePosition = {x, y};

    if (firstMouse) {
        mouseDelta = {0.0f, 0.0f};
        firstMouse = false;
        return;
    }

    mouseDelta = {
        mousePosition.x - previousMousePosition.x,
        mousePosition.y - previousMousePosition.y
    };
}

void Input::UpdateMouseScroll(float xOffset, float yOffset) {
    mouseScroll = {xOffset, yOffset};
}

void Input::ResetDeltas() {
    mouseDelta = {0.0f, 0.0f};
    mouseScroll = {0.0f, 0.0f};
}

bool Input::GetKeyPressed(SDL_Scancode key) {
    if (key < 0 || key >= SDL_SCANCODE_COUNT) return false;

    if (keyStates[key] == KeyState::PRESSED) {
        keyStates[key] = KeyState::HELD;
        return true;
    }
    return false;
}

bool Input::GetKeyHeld(SDL_Scancode key) {
    if (key < 0 || key >= SDL_SCANCODE_COUNT) return false;
    return keyStates[key] == KeyState::PRESSED || keyStates[key] == KeyState::HELD;
}

bool Input::GetKeyReleased(SDL_Scancode key) {
    if (key < 0 || key >= SDL_SCANCODE_COUNT) return false;

    if (keyStates[key] == KeyState::RELEASED) {
        keyStates[key] = KeyState::NONE;
        return true;
    }
    return false;
}

bool Input::GetMouseButtonPressed(int button) {
    if (button < 0 || button >= MAX_MOUSE_BUTTONS) return false;

    if (mouseButtonStates[button] == KeyState::PRESSED) {
        mouseButtonStates[button] = KeyState::HELD;
        return true;
    }
    return false;
}

bool Input::GetMouseButtonHeld(int button) {
    if (button < 0 || button >= MAX_MOUSE_BUTTONS) return false;
    return mouseButtonStates[button] == KeyState::PRESSED || mouseButtonStates[button] == KeyState::HELD;
}

bool Input::GetMouseButtonReleased(int button) {
    if (button < 0 || button >= MAX_MOUSE_BUTTONS) return false;

    if (mouseButtonStates[button] == KeyState::RELEASED) {
        mouseButtonStates[button] = KeyState::NONE;
        return true;
    }
    return false;
}

}
