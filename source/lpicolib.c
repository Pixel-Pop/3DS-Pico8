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

/* Audio Manipulation */

static int l_music(lua_State* L) {
   // Placeholder
   return 0;
}

static int l_sfx(lua_State* L) {
   // Placeholder
   return 0;
}

// Cart Data Manipulation


// Coroutines


/* Graphics Manipulation */

static int l_camera(lua_State* L) {
   // Placeholder
   return 0;
}

static int l_circ(lua_State* L) {
   // TODO draw circle outline w/ lines potentially
   // Placeholder
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
   return 0;
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
   return 0;
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
   drawState.cursor.x = 0;
   drawState.cursor.y = 0;
   
   return 0;
}

static int l_color(lua_State* L) {
   // Return previous color.
   lua_pushnumber(L, drawState.color);
   drawState.color = (int)lua_tonumber(L, 1);
   return 1;
}

static int l_fget(lua_State* L) {
   if (lua_gettop(L) > 1) {
      lua_pushboolean(L, pico8.spriteFlags[(int)lua_tonumber(L, 1)] & (1 << (int) lua_tonumber(L,2) ));
   }
   else {
      lua_pushnumber(L, pico8.spriteFlags[(int)lua_tonumber(L, 1)]);
   }
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
         return 0;
      }

   }
   else {
      if (arguments == 1) { drawState.color = lua_tonumber(L, 1); }
      pico8.lineEndpoint.active = false;
      return 0;
   }

   C2D_DrawLine(x0, y0, drawState.palette[drawState.color], toScreenX(pico8.lineEndpoint.x1), toScreenY(pico8.lineEndpoint.y1), drawState.palette[drawState.color], 1.875f, 0.0f);
   return 0;
}

static int l_pal(lua_State* L) {
   u8 arguments = lua_gettop(L);
   if (arguments > 0) {
      u8 origColor = lua_tonumber(L, 1);
      u8 newColor = lua_tonumber(L, 2);
      drawState.palette[origColor] = pico8.colors[newColor];
      drawState.paletteTransparency[origColor] = drawState.paletteTransparency[newColor];
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
   return 0;
}

static int l_palt(lua_State* L) {
   if (lua_gettop(L) > 0) {
      drawState.paletteTransparency[(int)lua_tonumber(L, 1)] = lua_toboolean(L, 2);
   }
   else {
      drawState.paletteTransparency[0] = true;
      for (u8 i = 1; i < 16; i++) {
         drawState.paletteTransparency[i] = false;
      }
   }
   return 0;
}

static int l_pget(lua_State* L) {
   /* Can only check pixels from previous frame. */
   /* Not guaranteed to be 100% accurate.        */
   int x = lua_tonumber(L, 1);
   int y = lua_tonumber(L, 2);

   u32 index = 240 * 3 - round((float)y * 1.875f) * 3 - 3 + 240 * 3 * (80 + round((float)x * 1.875f));
   u32 color = C2D_Color32(pico8.previousScreen[index + 2], pico8.previousScreen[index + 1], pico8.previousScreen[index], 0xFF);
   for (u8 i = 0; i < 16; i++) {
      if (pico8.colors[i] == color) {
         lua_pushnumber(L, i);
         return 1;
      }
   }
   return 0;
}

static int l_pset(lua_State* L) {
   int x1 = toScreenX(lua_tonumber(L, 1));
   int y1 = toScreenY(lua_tonumber(L, 2));
   u8 useColor;
   if (lua_gettop(L) > 2) {
      useColor = lua_tonumber(L, 3);
   }
   else {
      useColor = drawState.color;
   }
   C2D_DrawRectSolid(x1, y1, 0.0f, 1.875f, 1.875f, drawState.palette[useColor]);

   return 0;
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
   C2D_TextFontParse(&g_staticText, pico8.font, pico8.textBuffer, text);
   C2D_TextOptimize(&g_staticText);

   C2D_DrawText(&g_staticText, C2D_WithColor, x, y, 0.0f, 0.3125f, 0.3125f, drawState.palette[useColor]);
   return 0;
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

   C2D_DrawRectSolid(x1, y1, 0.0f, x2 - x1, y2 - y1, drawState.palette[useColor]);
   return 0;
}

static int l_spr(lua_State* L) {
   // Get arguments
   u8 spriteNum = lua_tonumber(L, 1);
   float x = lua_tonumber(L, 2);
   float y = lua_tonumber(L, 3);
   float width = 8.0f;
   float height = 8.0f;
   bool flip_x;
   bool flip_y;

   switch (lua_gettop(L)) {
   case 7:
      flip_y = lua_toboolean(L, 7);
   case 6:
      flip_x = lua_toboolean(L, 6);
   case 5:
      height = lua_tonumber(L, 5) * 8.0f;
   case 4:
      width = lua_tonumber(L, 4) * 8.0f;
      break;
   }

   u8 sheetX = (spriteNum % 16) * 8;
   u8 sheetY = (spriteNum / 16) * 8;

   u8 currColor;
   int currSheetX;
   int currSheetY;
   for (int loopX = 0; loopX < width; loopX++) {
      for (int loopY = 0; loopY < height; loopY++) {
         currSheetX = loopX + sheetX;
         currSheetY = loopY + sheetY;

         currColor = (currSheetY < 128) ? pico8.sprites[currSheetX / 2][currSheetY] : pico8.overlap[currSheetX][currSheetY - 128];
         currColor = (currSheetX % 2 == 0) ? currColor & 15 : currColor >> 4;
         
         if (!drawState.paletteTransparency[currColor]) {
            C2D_DrawRectSolid(toScreenX(x + loopX), toScreenY(y + loopY), 0.0f, 1.875f, 1.875f, drawState.palette[currColor]);
         }
      }
   }

   return 0;
}

static int l_sspr(lua_State* L) {
   u8 sheetX = lua_tonumber(L, 1);
   u8 sheetY = lua_tonumber(L, 2);
   u8 sheetWidth = lua_tonumber(L, 3);
   u8 sheetHeight = lua_tonumber(L, 4);
   u8 x = lua_tonumber(L, 5);
   u8 y = lua_tonumber(L, 6);
   u8 width = sheetWidth;
   u8 height = sheetHeight;
   bool flip_x;
   bool flip_y;
   switch (lua_gettop(L)) {
   case 10:
      flip_y = lua_toboolean(L, 10);
   case 9:
      flip_x = lua_toboolean(L, 9);
   case 8:
      height = lua_tonumber(L, 8);
   case 7:
      width = lua_tonumber(L, 7);
      break;
   }

   float stretchX = (float) (sheetWidth) / (float) (width);
   float stretchY = (float) (sheetHeight) / (float) (height);

   // Helping variables.
   u8 currSheetX;
   u8 currSheetY;
   u8 currColor;
   float stretchHoldX = ceil(stretchX);
   float stretchHoldY = ceil(stretchY);
   u8 stretchOffsetX = 0;
   u8 stretchOffsetY = 0;
   // Loop through each pixel on the sprite sheet.
   for (u8 loopX = 0; loopX < sheetWidth; loopX++) {
      currSheetX = loopX + sheetX;
      stretchOffsetY = 0;
      for (u8 loopY = 0; loopY < sheetHeight; loopY++) {
         currSheetY = loopY + sheetY;

         // Get either from sprite or overlap region. Get either first or second pixel color.
         currColor = (currSheetY < 128) ? pico8.sprites[currSheetX/2][currSheetY] : pico8.overlap[currSheetX][currSheetY - 128];
         currColor = (currSheetX % 2 == 0) ? currColor & 15 : currColor >> 4;

         // Draw necessary pixels, depending on stretch.
         if (!drawState.paletteTransparency[currColor]) {
            for (int i = 0; i < floor(stretchHoldX); i++) {
               for (int j = 0; j < floor(stretchHoldY); j++) {
                  C2D_DrawRectSolid(toScreenX(x + i + stretchOffsetX), toScreenY(y + j + stretchOffsetY), 0.0f, 1.875f, 1.875f, drawState.palette[currColor]);
               }
            }
         }

         stretchOffsetX += floor(stretchHoldX);
         stretchOffsetY += floor(stretchHoldY);
         stretchHoldX = fmod(stretchHoldX, 1.0f);
         stretchHoldY = fmod(stretchHoldY, 1.0f);
         stretchHoldX += stretchX;
         stretchHoldY += stretchY;
      }
   }
   
   return 0;
}

/* Input Manipulation */

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


/* Map Manipulation */

static int l_map(lua_State* L) {
   u8 celx = lua_tonumber(L, 1);
   u8 cely = lua_tonumber(L, 2);
   int sx = toScreenX(lua_tonumber(L, 3));
   int sy = toScreenY(lua_tonumber(L, 4));
   u8 celw = lua_tonumber(L, 5);
   u8 celh = lua_tonumber(L, 6);
   u8 layers = 0;
   if (lua_gettop(L) > 6) { layers = lua_tonumber(L, 7); }

   u8 currSprite;
   for (u8 currX = 0; currX < celw; currX++) {
      for (u8 currY = 0; currY < celh; currY++) {
         if (cely + currY >= 32) {
            // Read from overlap region
            currSprite = pico8.overlap[(celx + currX) % 64][(cely + currY - 32) * 2 + (celx + currX) / 64];
         }
         else {
            // Read from map region
            currSprite = pico8.map[celx + currX][cely + currY];
         }

         if (currSprite == 0) {
            continue;
         }
         // Check flags
         if ((pico8.spriteFlags[currSprite] & layers) != layers) {
            continue;
         }

         /* --- Draw Sprite --- */

         // Create necessary variables.
         u8 currColor;
         u8(*graphicsPointer)[64][64] = &pico8.sprites;
         // Handle if the specified sprite is in the overlapping memory region.
         if (currSprite >= 128) {
            currSprite -= 128;
            graphicsPointer = &pico8.overlap;
         }

         u8 pixelX = (currSprite % 16) * 4;
         u8 pixelY = (currSprite / 16) * 8;

         for (u8 i = 0; i < 4; i++) {
            for (u8 j = 0; j < 8; j++) {
               currColor = (*graphicsPointer)[i + pixelX][j + pixelY];
               if (!drawState.paletteTransparency[currColor & 15]) {
                  C2D_DrawRectSolid(sx + (i * 2 + currX * 8) * 1.875, sy + (j + currY * 8) * 1.875, 0.0f, 1.875f, 1.875f, drawState.palette[currColor & 15]);
               }
               if (!drawState.paletteTransparency[currColor >> 4]) {
                  C2D_DrawRectSolid(sx + (i * 2 + 1 + currX * 8) * 1.875, sy + (j + currY * 8) * 1.875, 0.0f, 1.875f, 1.875f, drawState.palette[currColor >> 4]);
               }
            }
         }

         /* === End === */

      }
   } 
   return 0;
}

static int l_mget(lua_State* L) {
   u8 celx = lua_tonumber(L, 1);
   u8 cely = lua_tonumber(L, 2);

   if (cely >= 32) {
      // Read from lower (shared) portion of map.
      lua_pushnumber(L, pico8.overlap[celx % 64][(cely - 32) * 2 + celx / 64]);
   }
   else {
      // Read from upper portion of the map.
      lua_pushnumber(L, pico8.map[celx][cely]);
   }

   return 1;
}

/* Math Functions */

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
   lua_pushnumber(L, -sin(lua_tonumber(L, 1) * M_PI * 2));
   return 1;
}


// Memory Manipulation


// Peek / Poke

static int l_stat(lua_State* L) {
   // TODO think of allowing frame rate changes?
   // Placeholder
   lua_pushnumber(L, 30);
   return 1;
}

/* String Manipulation */

static int l_sub(lua_State* L) {
   size_t* length = malloc(sizeof(size_t));
   const char* str = luaL_tolstring(L, 1, length);
   int start = lua_tonumber(L, 2);
   int end;
   if (lua_gettop(L) > 2) {
      end = lua_tonumber(L, 3);
   }
   else {
      end = *length - 1;
   }

   // Convert to proper string index. 1 -> 0, 2 -> 1, etc. / -1 -> length - 1, -2 -> length - 2, etc.
   if (start < 0) { start += (int)*length; } else { start -= 1; }
   if (end < 0) { end += (int)*length; } else { end -= 1; }

   char* subStr = malloc(sizeof(char) * (end - start + 2));
   strncpy(subStr, &str[start], end - start + 1);
   subStr[end - start + 1] = '\0';

   lua_pushstring(L, subStr);
   free(length);
   free(subStr);
   return 1;
}

/* Table Manipulation */

static int l_add(lua_State* L) {
   lua_Integer tableLen = luaL_len(L, 1);
   lua_seti(L, 1, tableLen + 1);
   // Return the added value.
   lua_pushvalue(L, 1);
   return 1;
}

static u16* currIndex = NULL;
static int all_iterator(lua_State* L) {
   // Not sure how to implement all() so it exactly matches the one in Pico8 wiki.

   while (*currIndex <= luaL_len(L, 1)) {
      if (lua_geti(L, 1, *currIndex) != LUA_TNIL) {
         *currIndex += 1;
         return 1;
      }
      *currIndex += 1;
   }

   lua_pushnil(L);
   free(currIndex);
   return 1;
}

static int l_all(lua_State* L) {
   currIndex = malloc(sizeof(u16));
   *currIndex = 1;
   lua_pushcfunction(L, all_iterator);  /* iteration function */
   lua_pushvalue(L, 1);  /* invariant table */
   lua_pushinteger(L, 0);  /* initial value */
   return 3;
}

static int l_count(lua_State* L) {
   lua_Integer tableLen = luaL_len(L, 1);
   size_t count = 0;
   for (size_t i = 1; i <= tableLen; i++) {
      lua_geti(L, 1, i);
      count += !lua_isnil(L, 2);
      lua_pop(L, 1);
   }
   lua_pushnumber(L, count);
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
   /* Stack:
   1. Table parameter
   2. Function parameter
   */

   // Duplicate the table to stack position 3
   lua_newtable(L);
   // Get element to position 4
   lua_geti(L, 1, 1);
   for (size_t i = 2; !lua_isnil(L, 4); i++) {
      lua_seti(L, 3, i - 1);
      lua_geti(L, 1, i);
   }
   // Pop nil element from 4;
   lua_pop(L, 1);

   /* Stack:
   1. Table parameter
   2. Function parameter
   3. Duplicated Table
   */

   // Push function to position 4 for initial loop
   lua_pushvalue(L, 2);
   // Get first value from duplicated table to position 5
   lua_geti(L, 3, 1);

   for (size_t i = 2; !lua_isnil(L, 5); i++) {
      lua_call(L, 1, LUA_MULTRET);
      lua_pushvalue(L, 2);
      lua_geti(L, 3, i);
   }
   // Pop duplicated table, function copy, and remaining nil
   lua_pop(L, 3);
   return 0;
}


static const luaL_Reg picolib[] = {
   // Audio Manipulation
     {"music", l_music},
     {"sfx", l_sfx},
   // Graphics Manipulation
     {"camera", l_camera},
     {"circ", l_circ},
     {"circfill", l_circfill},
     {"cls", l_cls},
     {"color", l_color},
     {"fget", l_fget},
     {"line", l_line},
     {"pal", l_pal},
     {"palt", l_palt},
     {"pget", l_pget},
     {"pset", l_pset},
     {"print", l_print},
     {"rectfill", l_rectfill},
     {"spr", l_spr},
     {"sspr", l_sspr},
   // Input Manipulation
     {"btn", l_btn},
   // Map Manipulation
     {"map", l_map},
     {"mget", l_mget},
   // Math Functions
     {"abs", l_abs},
     {"cos", l_cos},
     {"flr", l_flr},
     {"max", l_max},
     {"min", l_min},
     {"rnd", l_rnd},
     {"sgn", l_sgn},
     {"sin", l_sin},
   // Pico 8
     {"stat", l_stat},
   // String Manipulation
     {"sub", l_sub},
   // Table Manipulation
     {"add", l_add},
     {"all", l_all},
     {"count", l_count},
     {"del", l_del},
     {"foreach", l_foreach},
     {NULL, NULL}  /* sentinel */
};

int luaopen_pico(lua_State* L) {
   lua_pushglobaltable(L);
   luaL_setfuncs(L, picolib, 0);
   return 1;
}