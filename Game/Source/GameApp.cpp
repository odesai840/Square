#include "Application.h"
#include <string>
#include <iostream>

#include "Player.h"
#include "EnemyManager.h"
#include "Map.h"
#include "UserInterface.h"

int main(int argc, char* argv[]) {
    // Square application
    SquareCore::Application app;

    Map* map = new Map();
    Player* player = new Player();
    EnemyManager* enemy_manager = new EnemyManager();
    UserInterface* userInterface = new UserInterface();
    app.PushScript(userInterface);
    app.PushScript(map);
    app.PushScript(player);
    app.PushScript(enemy_manager);

    // Parse command line arguments
    if (argc > 1) {
        std::string arg1 = argv[1];

        if (arg1 == "--server") {
            // Run as dedicated server
            std::cout << "Starting River server...\n";
            app.RunServer();
            return 0;
        }
        else if (arg1 == "--client") {
            // Run as client
            std::string serverAddress = "localhost";
            if (argc > 2) {
                serverAddress = argv[2];
            }
            std::cout << "Starting Square client, connecting to: " << serverAddress << "\n";
            app.RunClient(serverAddress);
            return 0;
        }
        else if (arg1 == "--listen") {
            // Run as listen server
            std::cout << "Starting Square listen server...\n";
            app.RunServer(false);
            return 0;
        }
        else {
            std::cout << "Unknown argument: " << arg1 << "\n";
            std::cout << "Usage: Square [--server | --client [address]]\n";
            return 1;
        }
    }

    // Default: Run standalone
    std::cout << "Starting Square in standalone mode...\n";
    app.Run();
    return 0;
}
