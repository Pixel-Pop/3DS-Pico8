#ifndef LOADCART_H
#define LOADCART_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#include "lodepng.h"

bool loadCart() {

   unsigned char* decodeImage = malloc(32800 + 1);

   // Lode PNG
   unsigned error;
   unsigned char* image;
   unsigned width, height;
   unsigned char temp;


   error = lodepng_decode32_file(&image, &width, &height, "/Pico8Carts/helloworld.p8.png");
   if (error) {
      printf("error %u: %s\n", error, lodepng_error_text(error));
      return true;
   }

   // Get hidden data from raw RGBA.
   for (int i = 0; i < 0x8020; i++) {
      temp = 0x00;
      temp = temp + (image[i * 4 + 3] & 3);
      temp = temp << 2;
      temp = temp + (image[i * 4] & 3);
      temp = temp << 2;
      temp = temp + (image[i * 4 + 1] & 3);
      temp = temp << 2;
      temp = temp + (image[i * 4 + 2] & 3);
      decodeImage[i] = temp;
   }

   // Load Sprites
   for (int i = 0, spriteY = 0; spriteY < 8; spriteY++) {
      for (int currRow = 0; currRow < 8; currRow++) {
         for (int spriteX = 0; spriteX < 16; spriteX++) {
            for (int currCol = 0; currCol < 4; currCol++) {
               pico8.sprites[spriteX + spriteY * 16][currRow][currCol] = decodeImage[i];
               i = i + 1;
            }
         }
      }
   }

   // TODO Load Shared Region
   // TODO Load Map
   // TODO Load Lua Code
   pico8.script = malloc(0x3D00 + 1);
   memcpy(pico8.script, decodeImage + 0x4300, 0x3D00);

   free(image);
   free(decodeImage);

   return false;
}

#endif
