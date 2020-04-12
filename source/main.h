#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <3ds.h>
#include <citro2d.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* Pico8 Library */

// DrawState
struct Camera {
   int x;
   int y;
};

struct Cursor {
   int x;
   int y;
};

struct Clip {
   int x;
   int y;
   int width;
   int height;
};

struct DrawState {
   u8 color;
   struct Camera camera;
   struct Cursor cursor;
   u32 palette[16];
   bool paletteTransparency[16];
};

extern struct DrawState drawState;

// Allow Lua to open library.
extern int luaopen_pico(lua_State* L);

/* Main */

struct Pico8 {
   C3D_RenderTarget* screen;
   C2D_Font font;
   u32 colors[16];
   // Total amount of sprites - Columns - Rows
   u8	sprites[128][8][4];
   char* script;
   TickCounter* tickCounter;
   double elapsedTime;
   bool screenCleared;
};

extern struct Pico8 pico8;

extern C2D_TextBuf g_staticBuf;
