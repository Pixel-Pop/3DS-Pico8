#define lpicolib_c
#define LUA_LIB

#include "lprefix.h"

#include "main.h"

#define PICO8_3DS_RATIO 1.875

struct DrawState drawState = { 0, {0, 0}, {0, 0} };


/* Helper Functions */
float toScreenX(int x) {
   return x * PICO8_3DS_RATIO + 80;
}

float toScreenY(int y) {
   return y * PICO8_3DS_RATIO;
}

// Audio Manipulation
static int l_music(lua_State* L) {
   printf("Music was called! (But it doesn't exist yet...)");
   return 1;
}

static int l_sfx(lua_State* L) {
   // TODO figure out sfx
   return 1;
}

// Cart Data Manipulation


// Coroutines


// Graphics Manipulation

static int l_circ(lua_State* L) {
   // TODO draw an outline only somehow
   double x = toScreenX(lua_tonumber(L, 1));
   double y = toScreenY(lua_tonumber(L, 2));
   double radius = 4;
   u8 useColor = drawState.color;
   if (lua_gettop(L) >= 3) {
      radius = lua_tonumber(L, 3);
   }
   if (lua_gettop(L) >= 4) {
      useColor = lua_tonumber(L, 4);
   }
   C2D_DrawCircle(x, y, 0.0f, radius * PICO8_3DS_RATIO, drawState.palette[useColor], drawState.palette[useColor], drawState.palette[useColor], drawState.palette[useColor]);
   return 1;
}

static int l_circfill(lua_State* L) {
   double x = toScreenX(lua_tonumber(L, 1));
   double y = toScreenY(lua_tonumber(L, 2));
   double radius = 4;
   u8 useColor = drawState.color;
   if (lua_gettop(L) >= 3) {
      radius = lua_tonumber(L, 3);
   }
   if (lua_gettop(L) >= 4) {
      useColor = lua_tonumber(L, 4);
   }
   C2D_DrawCircleSolid(x, y, 0.0f, radius * PICO8_3DS_RATIO, drawState.palette[useColor]);
   return 1;
}

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

static int l_line(lua_State* L) {
   u8 arguments = lua_gettop(L);
   double x0;
   double y0;
   // x1 and y1 are stored in pico8.lineEndpoint

   if (arguments >= 4) {
      if (arguments == 5) { drawState.color = lua_tonumber(L, 5); }
      pico8.lineEndpoint.active = true;
      x0 = toScreenX(lua_tonumber(L, 1));
      y0 = toScreenY(lua_tonumber(L, 2));
      pico8.lineEndpoint.x1 = lua_tonumber(L, 3);
      pico8.lineEndpoint.y1 = lua_tonumber(L, 4);
   }
   else if (arguments >= 2) {
      if (arguments == 3) { drawState.color = lua_tonumber(L, 3); }
      if (pico8.lineEndpoint.active) {
         x0 = toScreenX(pico8.lineEndpoint.x1);
         y0 = toScreenY(pico8.lineEndpoint.y1);
         pico8.lineEndpoint.x1 = lua_tonumber(L, 1);
         pico8.lineEndpoint.y1 = lua_tonumber(L, 2);
      }
      else {
         pico8.lineEndpoint.x1 = lua_tonumber(L, 1);
         pico8.lineEndpoint.y1 = lua_tonumber(L, 2);
         pico8.lineEndpoint.active = true;
         return 1;
      }

   }
   else {
      if (arguments == 1) { drawState.color = lua_tonumber(L, 1); }
      pico8.lineEndpoint.active = false;
      return 1;
   }

   C2D_DrawLine(x0, y0, drawState.palette[drawState.color], toScreenX(pico8.lineEndpoint.x1), toScreenY(pico8.lineEndpoint.y1), drawState.palette[drawState.color], 1.875f, 0.0f);
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
   const char* text = luaL_tolstring(L, 1, NULL);
   int arguments = lua_gettop(L);
   double x;
   double y;
   u8 useColor;
   if (arguments > 1) {
      x = lua_tonumber(L, 2);
   }
   else {
      x = drawState.cursor.x;
   }
   x = toScreenX(x);
   if (arguments > 2) {
      y = lua_tonumber(L, 3);
   }
   else {
      y = drawState.cursor.y;
      drawState.cursor.y += 6;
   }
   y = toScreenY(y);
   if (arguments > 3) {
      useColor = lua_tonumber(L, 4);
   }
   else {
      useColor = drawState.color;
   }

   C2D_Text g_staticText;
   C2D_TextFontParse(&g_staticText, pico8.font, g_staticBuf, text);
   C2D_TextOptimize(&g_staticText);

   C2D_DrawText(&g_staticText, C2D_WithColor, x, y, 0.0f, 0.3125f, 0.3125f, drawState.palette[useColor]);
   return 1;
}

static int l_rectfill(lua_State* L) {
   double x1 = toScreenX(lua_tonumber(L, 1));
   double y1 = toScreenY(lua_tonumber(L, 2));
   double x2 = toScreenX(lua_tonumber(L, 3));
   double y2 = toScreenY(lua_tonumber(L, 4));

   u8 useColor;
   if (lua_gettop(L) == 5) {
      useColor = (u8)lua_tonumber(L, 5);
   }
   else {
      useColor = drawState.color;
   }

   C2D_DrawRectangle(x1, y1, 1, x2 - x1, y2 - y1, drawState.palette[useColor], drawState.palette[useColor], drawState.palette[useColor], drawState.palette[useColor]);
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
static int l_btn(lua_State* L) {
   u32 keyHeld = hidKeysHeld();
   /* Pico 8 Button Numbers
      0 - Left
      1 - Right
      2 - Up
      3 - Down
      4 - A
      5 - B
   */
   u8 buttonMap[] = { 32, 16, 64, 128, 1, 2 };
   int args = lua_gettop(L);
   if (args > 0) {
      int buttonNum = lua_tonumber(L, 1);
      if (args > 1) {
         if (lua_tonumber(L, 2) != 0) {
            lua_pushboolean(L, false);
            return 1;
         }
      }
      lua_pushboolean(L, keyHeld & buttonMap[buttonNum]);
   }
   else {
      int bitfield = 0;
      for (u8 i = 5; i >= 0; i--) {
         if (keyHeld & buttonMap[i]) {
            bitfield += 1;
         }
         bitfield = bitfield << 1;
      }
      lua_pushnumber(L, bitfield);
   }
   return 1;
}


// Map Manipulation


// Math Functions
static int l_abs(lua_State* L) {
   lua_Number input = lua_tonumber(L, 1);
   lua_pushnumber(L, fabs(input));
   return 1;
}

static int l_cos(lua_State* L) {
   double input = lua_tonumber(L, 1);
   lua_pushnumber(L, cos(input * M_PI * 2));
   return 1;
}

static int l_flr(lua_State* L) {
   double input = lua_tonumber(L, 1);
   lua_pushnumber(L, floor(input));
   return 1;
}

static int l_max(lua_State* L) {
   lua_Number first = lua_tonumber(L, 1);
   lua_Number second = 0;
   if (lua_gettop(L) > 1) {
      second = lua_tonumber(L, 2);
   }
   if (first > second) {
      lua_pushnumber(L, first);
   }
   else {
      lua_pushnumber(L, second);
   }
   return 1;
}

static int l_min(lua_State* L) {
   lua_Number first = lua_tonumber(L, 1);
   lua_Number second = 0;
   if (lua_gettop(L) > 1) {
      second = lua_tonumber(L, 2);
   }
   if (first < second) {
      lua_pushnumber(L, first);
   }
   else {
      lua_pushnumber(L, second);
   }
   return 1;
}

static int l_rnd(lua_State* L) {
   lua_Number cutoff = lua_tonumber(L, 1);
   float randomNum = (float)((double)rand() / (double)(RAND_MAX / cutoff));
   lua_pushnumber(L, randomNum);
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

// Table Manipulation
static int l_add(lua_State* L) {
   lua_Integer tableLen = luaL_len(L, 1);
   lua_seti(L, 1, tableLen + 1);
   return 1;
}

static int l_del(lua_State* L) {
   lua_Integer tableLen = luaL_len(L, 1);
   u16 i;
   for (i = 1; i <= tableLen; i++) {
      lua_geti(L, 1, i);
      if (lua_compare(L, 2, 3, LUA_OPEQ)) {
         break;
      }
      lua_pop(L, 1);
   }
   for (; i < tableLen; i++) {
      lua_geti(L, 1, i + 1);
      lua_seti(L, 1, i);
   }
   lua_pushnil(L);
   lua_seti(L, 1, i);
   return 1;
}

static int l_foreach(lua_State* L) {
   lua_Integer length = luaL_len(L, 1);

   for (u16 i = 1; i <= length; i++) {
      lua_pushvalue(L, 2);
      lua_geti(L, 1, i);
      lua_call(L, 1, LUA_MULTRET);
   }
   return 1;
}


static const luaL_Reg picolib[] = {
   // Audio Manipulation
     {"music", l_music},
     {"sfx", l_sfx},
   // Graphics Manipulation
     {"circ", l_circ},
     {"circfill", l_circfill},
     {"cls", l_cls},
     {"color", l_color},
     {"line", l_line},
     {"pal", l_pal},
     {"print", l_print},
     {"rectfill", l_rectfill},
     {"spr", l_spr},
   // Input Manipulation
     {"btn", l_btn},
   // Math Functions
     {"abs", l_abs},
     {"cos", l_cos},
     {"flr", l_flr},
     {"max", l_max},
     {"min", l_min},
     {"rnd", l_rnd},
     {"sgn", l_sgn},
     {"sin", l_sin},
   // Table Manipulation
     {"add", l_add},
     {"del", l_del},
     {"foreach", l_foreach},
     {NULL, NULL}  /* sentinel */
};

int luaopen_pico(lua_State* L) {
   lua_pushglobaltable(L);
   luaL_setfuncs(L, picolib, 0);
   return 1;
}