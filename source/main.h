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

struct LineEndpoint {
   bool active;
   double x1;
   double y1;
};

struct Pico8 {
   C3D_RenderTarget* screen;
   C2D_Font font;
   C2D_TextBuf textBuffer;
   u32 colors[16];
   u32 transparentColor;
   u8	sprites[64][64];
   u8 overlap[64][64];
   u8 map[128][32];
   u8 spriteFlags[256];
   char* script;
   TickCounter* tickCounter;
   double elapsedTime;
   struct LineEndpoint lineEndpoint;
   u8* previousScreen;
};

extern struct Pico8 pico8;