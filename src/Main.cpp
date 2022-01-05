#include "./Constants.h"
#include "./Game.h"
#include "./EntityManager.h"

int main (int argc, char *args[]) {
    Game game;
    game.initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    while (game.IsRunning()) {
        game.processInput();
        game.Update();
        game.Render();
    }

    game.Destroy();

    return 0;
}