#define lpicolib_c
#define LUA_LIB

#include "lprefix.h"

#include "main.h"

#define PICO8_3DS_RATIO 1.875

struct DrawState drawState = { 0, {0, 0}, {0, 0} };


/* Helper Functions */
float getPosOnScreenX(int x) {
   return x * PICO8_3DS_RATIO + 80;
}

// Audio Manipulation
static int l_music(lua_State* L) {
   printf("Music was called! (But it doesn't exist yet...)");
   return 1;
}

// Cart Data Manipulation


// Coroutines


// Graphics Manipulation

static int l_cls(lua_State* L) {
   u8 useColor;
   if (lua_gettop(L) > 0) {
      useColor = lua_tonumber(L, 1);
      C2D_TargetClear(pico8.screen, drawState.palette[useColor]);
   }
   else {
      useColor = 0;
      C2D_TargetClear(pico8.screen, pico8.colors[useColor]);
   }
   pico8.screenCleared = true;
   C2D_TextBufClear(g_staticBuf);
   drawState.cursor.x = 0;
   drawState.cursor.y = 0;

   return 1;
}

static int l_color(lua_State* L) {
   drawState.color = (int)lua_tonumber(L, 1);
   return 1;
}

static int l_pal(lua_State* L) {
   u8 arguments = lua_gettop(L);
   if (arguments > 0) {
      u8 origColor = lua_tonumber(L, 1);
      u8 newColor = lua_tonumber(L, 2);
      drawState.palette[origColor] = pico8.colors[newColor];
   }
   else {
      memcpy(drawState.palette, pico8.colors, sizeof(u32) * 16);
      drawState.paletteTransparency[0] = true;
      for (u8 i = 1; i < 16; i++) {
         drawState.paletteTransparency[i] = false;
      }
   }
   if (arguments > 2) {

   }
   return 1;
}

static int l_print(lua_State* L) {
   size_t* length;
   const char* text = luaL_tolstring(L, 1, length);
   int arguments = lua_gettop(L);
   int x;
   int y;
   u8 useColor;
   if (arguments > 1) {
      x = lua_tonumber(L, 2);
   }
   else {
      x = drawState.cursor.x;
   }
   if (arguments > 2) {
      y = lua_tonumber(L, 3);
   }
   else {
      y = drawState.cursor.y;
   }
   if (arguments > 3) {
      useColor = lua_tonumber(L, 4);
   }
   else {
      useColor = drawState.color;
   }

   C2D_Text g_staticText;

   C2D_TextFontParse(&g_staticText, pico8.font, g_staticBuf, text);
   C2D_TextOptimize(&g_staticText);
   C2D_DrawText(&g_staticText, C2D_WithColor, getPosOnScreenX(x), (float)y*PICO8_3DS_RATIO, 0.0f, 0.3125f, 0.3125f, drawState.palette[useColor]);
   return 1;
}

static int l_rectfill(lua_State* L) {
   double x1 = lua_tonumber(L, 1) * 1.875;
   double y1 = lua_tonumber(L, 2) * 1.875;
   double x2 = lua_tonumber(L, 3) * 1.875;
   double y2 = lua_tonumber(L, 4) * 1.875;
   
   u8 useColor;
   if (lua_gettop(L) == 5) {
      useColor = (u8)lua_tonumber(L, 5);
   }
   else {
      useColor = drawState.color;
   }

   C2D_DrawRectangle(x1+80, y1, 1, x2 - x1, y2 - y1, drawState.palette[useColor], drawState.palette[useColor], drawState.palette[useColor], drawState.palette[useColor]);
   return 1;
}

static int l_spr(lua_State* L) {
   u8 spriteNum = lua_tonumber(L, 1);
   int x = lua_tonumber(L, 2) * PICO8_3DS_RATIO + 80;
   int y = lua_tonumber(L, 3) * PICO8_3DS_RATIO;
   u8 currColor;

   for (u8 i = 0; i < 8; i++) {
      for (u8 j = 0; j < 4; j++) {
         currColor = pico8.sprites[spriteNum][i][j];
         if (!drawState.paletteTransparency[currColor & 15]) {
            C2D_DrawRectangle(x + j * 2 * 1.875, y + i * 1.875, 1.0f, 1.875f, 1.875f, drawState.palette[currColor & 15], drawState.palette[currColor & 15], drawState.palette[currColor & 15], drawState.palette[currColor & 15]);
         }
         if (!drawState.paletteTransparency[currColor >> 4]) {
            C2D_DrawRectangle(x + (j * 2 + 1) * 1.875, y + i * 1.875, 1.0f, 1.875f, 1.875f, drawState.palette[currColor >> 4], drawState.palette[currColor >> 4], drawState.palette[currColor >> 4], drawState.palette[currColor >> 4]);
         }
      }
   }
   
   return 1;
}

// Input Manipulation


// Map Manipulation


// Math Functions
static int l_cos(lua_State* L) {
   double input = lua_tonumber(L, 1);
   lua_pushnumber(L, cos(input * M_PI * 2));
   return 1;
}

static int l_sgn(lua_State* L) {
   double input = lua_tonumber(L, 1);
   if (input >= 0) {
      lua_pushnumber(L, 1);
   }
   else {
      lua_pushnumber(L, -1);
   }
   return 1;
}

static int l_sin(lua_State* L) {
   double input = lua_tonumber(L, 1);
   lua_pushnumber(L, -sin(input * M_PI * 2));
   return 1;
}


// Memory Manipulation


// Peek / Poke


static const luaL_Reg picolib[] = {
   // Audio Manipulation
     {"music", l_music},
   // Graphics Manipulation
     {"cls", l_cls},
     {"color", l_color},
     {"pal", l_pal},
     {"print", l_print},
     {"rectfill", l_rectfill},
     {"spr", l_spr},
   // Math Functions
     {"cos", l_cos},
     {"sgn", l_sgn},
     {"sin", l_sin},
     {NULL, NULL}  /* sentinel */
};

int luaopen_pico(lua_State* L) { 

   lua_pushglobaltable(L);
   luaL_setfuncs(L, picolib, 0);
   return 1;
}