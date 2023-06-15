#include <stdio.h>

#include "../lib/lua/src/lua.h"
#include "../lib/lua/src/lualib.h"
#include "../lib/lua/src/lauxlib.h"

/**
 * The lua state is essentially the lua interpreter. Provided by the lauxlib library.
 * Using lua_newstate() provides a "slimmer" state with less libraries loaded.
 * */ 
lua_State* L;

/**
 * This function shows how we can run a lua script.
*/
void lua_example_dofile(void) {
    // Runs the specified LUA file.
    luaL_dofile(L,"./scripts/factorial_no_params.lua");
}
/**
 * This function shows how we can get a global variable from the lua stack. The way C and Lua operate is by using a
 * stack from which things can be pushed and popped.
*/
void lua_example_getvar(void) {
    // Run a Lua command setting variables and values.
    luaL_dostring(L, "my_var = \"Hello World!\"");
    // Get a global from the Lua env, and add it to the stack.
    lua_getglobal(L, "my_var");
    
    // Grabs a variable as a string from the -1 index of the shared stack. -1 refers to the top.
    const char *my_var_string = lua_tostring(L, -1);
    
    luaL_dostring(L, "my_var = 1");
    lua_getglobal(L, "my_var");

    // lua_Number represents a floating point value. We can also use int if we don't need decimals.
    lua_Number my_var_number = lua_tonumber(L, -1);

    // This can be confusing, as only a value of true will return true. Values like false, False or anything other than true will return false.
    luaL_dostring(L, "my_var = true");
    lua_getglobal(L, "my_var");

    int my_var_boolean = lua_toboolean(L, -1);

    luaL_dostring(L, "my_var = {x = 1, y = 2}");
    lua_getglobal(L, "my_var");

    int x = 0;
    int y = 0;
    if (lua_istable(L, -1)) {
        // The 2nd parameter indicates where the table is. It gets a field and places it on the stack.
        lua_getfield(L, -1, "x");
        // Now the table is the 2nd from top, because the x field above was pushed on top of the stack.
        lua_getfield(L, -2, "y");


        x = lua_tonumber(L, -2);
        // Since y was the last thing we popped, it will still be on top. At this point, the table is on the bottom at -3
        y = lua_tonumber(L, -1);

        // We can pop everything off the stack to clean up.
        printf("Items on stack before pop all: %d\n", lua_gettop(L));
        lua_settop(L,0);
        printf("Items on stack before pop all: %d\n", lua_gettop(L));
    }

    printf("String: %s\n", my_var_string);
    printf("Number: %d\n", (int)my_var_number);
    printf("Boolean: %s\n", my_var_boolean ? "true" : "false");
    printf("Table: x: %d  y: %d\n", x,y);
}

// c and lua communicate via a stack
void lua_example_stack(void) {
    lua_State* L = luaL_newstate();
    lua_pushnumber(L, 286); // stack[1] or stack[-3]
    lua_pushnumber(L, 386);// ..
    lua_pushnumber(L, 486);// stack[3] or stack[-1]

    lua_Number element;
    element = lua_tonumber(L, -1);
    printf("Last element %d\n", (int)element);

    lua_remove(L, 2); // should remove 386
    element = lua_tonumber(L, 2);
    printf("Last element %d\n", (int)element); // 486 is now 2

    lua_close(L);
}

/**
 * We can call a Lua function and pass in arguments.
*/
void lua_example_call_lua_function(int n1, int n2) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, "./scripts/pythagoras.lua") != LUA_OK) {
        luaL_error(L, "Error: %s\n", lua_tostring(L, -1));
    }
    // Push the pythagoras function to the stack.
    lua_getglobal(L, "pythagoras");
    if ( lua_isfunction(L, -1) ) {
        lua_pushnumber(L, n1); // 1st function argument
        lua_pushnumber(L, n2); // 2nd function argument
        const int NUM_ARGS = 2;
        const int NUM_RETURNS = 1;
        lua_pcall(L, NUM_ARGS, NUM_RETURNS, 0);

        lua_Number pyth_result = lua_tonumber(L, -1);

        printf("Pyth %.2f\n", (float)pyth_result);
    }
    lua_close(L);
}

void lua_example_call_lua_function_factorial(int n) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    luaL_dofile(L, "./scripts/factorial.lua");
    lua_getglobal(L, "factorial");
    if (lua_isfunction(L, -1)) {
        lua_pushnumber(L, n);
        const int NUM_ARGS = 1;
        const int NUM_RETURNS = 1;
        lua_pcall(L, NUM_ARGS, NUM_RETURNS, 0); // 0 is indicative that the function has no error handling

        lua_Number factorial = lua_tonumber(L, -1);

        printf("Factorial of %d is %d!\n", n, (int)factorial);
    }

    lua_close(L);
}

int native_pythagoras(lua_State* L) {
    lua_Number b = lua_tonumber(L, -1); // get last added param from stack
    lua_Number a = lua_tonumber(L, -2); // get first added param from stack
    lua_Number result = (a*a) + (b*b);
    lua_pushnumber(L, result);

    return 1; // returns how many values the function is returning as a result to the stack
}


// Call native function in C from Lua
void lua_example_call_c_function(void) {
    lua_pushcfunction(L, native_pythagoras);
    lua_setglobal(L, "native_pythagoras");

    luaL_dofile(L, "./scripts/pythagoras-native.lua");
    lua_getglobal(L, "pythagoras");
    if(lua_isfunction(L, -1)) {
        lua_pushnumber(L, 3); // first func arg
        lua_pushnumber(L, 4); // second func arg
        lua_pcall(L, 2, 1, 0);

        lua_Number result = lua_tonumber(L, -1);
        printf("%f\n", (float)result);
    }

}

typedef struct rectangle2d {
    int x;
    int y;
    int width;
    int height;
} rectangle;

int create_rectangle(lua_State* L) {
    rectangle* rect = (rectangle*) lua_newuserdata(L, sizeof(rectangle));
    rect->x = 0;
    rect->y = 0;
    rect->width = 69;
    rect->height = 69;

    return 1;
}
int change_rectangle_size (lua_State* L) {
    rectangle* r = lua_touserdata(L,-3);
    r->y = lua_tonumber(L, -1);
    r->x = lua_tonumber(L, -2);

    return 0;
}
void lua_example_userdata(void) {
    // Expose our C function create_rectangle to Lua.
    lua_pushcfunction(L, create_rectangle);
    // Set a Lua global before running the script.
    lua_setglobal(L, "create_rectangle");

    // Expose our C function change_rectangle_size to Lua.
    lua_pushcfunction(L, change_rectangle_size);
    // Set a Lua global before running the script.
    lua_setglobal(L, "change_rectangle_size");
    // Run the script.
    luaL_dofile(L, "./scripts/rectangle.lua");

    lua_getglobal(L, "square");
    if(lua_isuserdata(L, -1)) {
        rectangle* r = (rectangle*) lua_touserdata(L, -1);
        printf("x: %f y: %f    w:%d   h:%d\n", (float)r->x, (float)r->y, r->width, r->height);
        
    } else {
        printf("We did not get a rectangle userdata from Lua.");
    }

}

void lua_example_table(void) {

    if(luaL_dofile(L, "./scripts/configtable.lua") != LUA_OK){
        luaL_error(L, "Error: %s\n", lua_tostring(L, -1));
    }
    
    lua_getglobal(L, "config_table");

    if(lua_istable(L, -1)) {
        lua_getfield(L, -1, "window_width");
        lua_getfield(L, -2, "window_height");
        lua_getfield(L, -3, "num_enemies");
        lua_getfield(L, -4, "num_levels");
        printf("Window width: %.2f\n", (float)lua_tonumber(L, -1));
        printf("Window height: %.2f\n", (float)lua_tonumber(L, -2));
        printf("Number enemies: %.2f\n", (float)lua_tonumber(L, -3));
        printf("Number levels: %.2f\n", (float)lua_tonumber(L, -4));
    }
}



// int main(int argc, char *argv[]) {
//     L = luaL_newstate();
//     // Opens and initializes the standard Lua libraries. Usually ran once to initialize the Lua environment.
//     luaL_openlibs(L);

//     //lua_example_dofile();
//     //lua_example_getvar();
//     //lua_example_stack();
//     //lua_example_call_lua_function(3,4);
//     // lua_example_call_lua_function_factorial(5);
//     // lua_example_call_c_function();
//     //lua_example_userdata();
//     lua_example_table();

//     lua_close(L);
//     return 0;
// }

// An example of how to do everything (initialization and module loading) in a single function.
// void lua_example_getvar(void) {
//     lua_State* L = luaL_newstate();
//     luaL_dostring(L, "some_var = 69.");
//     lua_getglobal(L, "some_var");
//     lua_Number some_var_in_c = lua_tonumber(L, -1);
//     printf("%d\n", (int)some_var_in_c);
//     lua_close(L);
// }