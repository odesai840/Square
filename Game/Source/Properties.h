#pragma once

#include "Script.h"

struct Character : SquareCore::Property
{
    int health = 10;

    Character(int health) { this->health = health; }
};

