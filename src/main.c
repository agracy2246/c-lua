#include "../lib/sdl2/include/SDL.h"
#include "../lib/lua/src/lua.h"
#include "../lib/lua/src/lualib.h"
#include "../lib/lua/src/lauxlib.h"

void initialize_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        // Show Error

    }
}

int main(int argc, char *argv[]) {
    initialize_window();
    return 0;
}