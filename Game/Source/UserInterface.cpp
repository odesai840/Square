#include "UserInterface.h"

void UserInterface::OnStart()
{
    dialogBox = AddUIRect(50, 800, 1820, 250, SquareCore::RGBA(20, 20, 20, 220), "", {SquareCore::RGBA(255, 255, 255, 255), 2.0f, 5.0f});
    speakerText = AddUIText(100, 850, 32, SquareCore::RGBA(255, 200, 100, 255), "Resources/Fonts/Helvetica.ttf", "Speaker");
    dialogText = AddUIText(100, 900, 24, SquareCore::RGBA(255, 255, 255, 255), "Resources/Fonts/Helvetica.ttf", "Hello my name is Speaker. Nice to meet you!");
}

void UserInterface::OnUpdate(float deltaTime)
{
    
}
