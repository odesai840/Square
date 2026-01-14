#ifndef MAIN_BEHAVIOR_H
#define MAIN_BEHAVIOR_H

#include "GameInterface.h"

class MainBehavior : public SquareCore::GameInterface {
public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
};

#endif
