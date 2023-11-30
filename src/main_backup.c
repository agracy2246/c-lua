#include "../lib/lua/src/lua.h"
#include "../lib/lua/src/lualib.h"
#include "../lib/lua/src/lauxlib.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *string = "Hello, World!";
    printf("%s",string);
    return 0;
}
