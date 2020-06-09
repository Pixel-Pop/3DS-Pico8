#include "main.h"
#include "loadcart.h"

struct Pico8 pico8;

void init() {
	romfsInit();
	cfguInit();

	// Create screens
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS * 32);
	C2D_Prepare();
	pico8.screen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	consoleInit(GFX_BOTTOM, NULL);

	pico8.textBuffer = C2D_TextBufNew(4096);

	// Set up Pico 8 stuff.
	pico8.font = C2D_FontLoad("romfs:/gfx/pico8upper.bcfnt");
	pico8.colors[0] = C2D_Color32(0x00, 0x00, 0x00, 0xFF); // black
	pico8.colors[1] = C2D_Color32(0x1D, 0x2B, 0x53, 0xFF); // dark-blue
	pico8.colors[2] = C2D_Color32(0x7E, 0x25, 0x53, 0xFF); // dark-purple
	pico8.colors[3] = C2D_Color32(0x00, 0x87, 0x51, 0xFF); // dark-green
	pico8.colors[4] = C2D_Color32(0xAB, 0x52, 0x36, 0xFF); // brown
	pico8.colors[5] = C2D_Color32(0x5F, 0x57, 0x4F, 0xFF); // dark-gray
	pico8.colors[6] = C2D_Color32(0xC2, 0xC3, 0xC7, 0xFF); // light-gray
	pico8.colors[7] = C2D_Color32(0xFF, 0xF1, 0xE8, 0xFF); // white
	pico8.colors[8] = C2D_Color32(0xFF, 0x00, 0x4D, 0xFF); // red
	pico8.colors[9] = C2D_Color32(0xFF, 0xA3, 0x00, 0xFF); // orange
	pico8.colors[10] = C2D_Color32(0xFF, 0xEC, 0x27, 0xFF); // yellow
	pico8.colors[11] = C2D_Color32(0x00, 0xE4, 0x36, 0xFF); // green
	pico8.colors[12] = C2D_Color32(0x29, 0xAD, 0xFF, 0xFF); // blue
	pico8.colors[13] = C2D_Color32(0x83, 0x76, 0x9C, 0xFF); // indigo
	pico8.colors[14] = C2D_Color32(0xFF, 0x77, 0xA8, 0xFF); // pink
	pico8.colors[15] = C2D_Color32(0xFF, 0xCC, 0xAA, 0xFF); // peach
	pico8.transparentColor = C2D_Color32(0x00, 0x00, 0x00, 0x00);
	memcpy(drawState.palette, pico8.colors, sizeof(u32) * 16);
	drawState.paletteTransparency[0] = true;

	pico8.tickCounter = malloc(sizeof(TickCounter));

	srand(time(NULL));
}

void finish() {
	C2D_TextBufDelete(pico8.textBuffer);
	C2D_FontFree(pico8.font);

	free(pico8.tickCounter);
	free(pico8.script);
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

int main(int argc, char* argv[])
{
	init();

	// Load Lua and open custom Pico8 library.
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requiref(L, "pico", luaopen_pico, 1);

	// Check for error.
	if (loadCart()) { finish(); return 0; }

	// Clear homebrew launcher screen
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_SceneBegin(pico8.screen);
	C2D_TargetClear(pico8.screen, pico8.colors[0]);
	C3D_FrameEnd(0);

	// Run script to create necessary functions.
	luaL_dostring(L, pico8.script);	

	luaL_dostring(L, "_init()");

	osTickCounterStart(pico8.tickCounter);

	// Main loop
	while (aptMainLoop())
	{
		osTickCounterUpdate(pico8.tickCounter);
		pico8.elapsedTime = pico8.elapsedTime + osTickCounterRead(pico8.tickCounter);
		
		pico8.previousScreen = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
		
		// Render the scene		
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_SceneBegin(pico8.screen);

		// Draw transparent rectangle to prevent softlock.
		C2D_DrawRectSolid(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, pico8.transparentColor);

		// Limit fps to ~30.
		while (pico8.elapsedTime > 33.33333333f) {
			luaL_dostring(L, "_update()");
			pico8.elapsedTime -= 33.33333333f;
		}
		luaL_dostring(L, "_draw()");

		C2D_TextBufClear(pico8.textBuffer);

		C3D_FrameEnd(0);
	}

	finish();

	return 0;
}
