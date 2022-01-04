#include "./Constants.h"
#include "./Game.h"
#define NO_STDIO_REDIRECT

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