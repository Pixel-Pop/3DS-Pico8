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


   error = lodepng_decode32_file(&image, &width, &height, "/Pico8Carts/celeste.p8.png");
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
   for (u8 x = 0; x < 64; x++) {
      for (u8 y = 0; y < 64; y++) {
         pico8.sprites[x][y] = decodeImage[x + y * 64];
      }
   }

   // Load overlapping region.
   for (u8 x = 0; x < 64; x++) {
      for (u8 y = 0; y < 64; y++) {
         pico8.overlap[x][y] = decodeImage[x + y * 64 + 0x1000];
      }
   }

   // Load Map
   for (u8 x = 0; x < 128; x++) {
      for (u8 y = 0; y < 32; y++) {
         pico8.map[x][y] = decodeImage[x + y * 128 + 0x2000];
      }
   }

   // Load sprite flags.
   memcpy(pico8.spriteFlags, decodeImage + 0x3000, 256);

   // TODO New Compression Lua Code
   
   if (strncmp((char *)decodeImage + 0x4300, ":c:\0", 4) == 0) {
      char lookupTable[] = "\n 0123456789abcdefghijklmnopqrstuvwxyz!#%(){}[]<>+=/*:;.,~_";
      int length = (int)(decodeImage[0x4304] << 8) + (int)(decodeImage[0x4305]);
      int offset;
      int copyLength;
      int currByte;
      pico8.script = malloc(length+1);
      pico8.script[length] = '\0';
      for (int i = 0, position = 0; position < length; i++) {
         currByte = decodeImage[0x4308 + i];
         if (currByte == 0x00) {
            pico8.script[position] = decodeImage[0x4308 + ++i];
            position++;
         }
         else if (currByte <= 0x3b) {
            pico8.script[position] = lookupTable[currByte - 1];
            position++;
         }
         else { //0x3c-0xff
            offset = ((currByte - 0x3c) << 4) + (decodeImage[0x4308 + ++i] & 0xf);
            copyLength = (decodeImage[0x4308 + i] >> 4) + 2;
            memcpy(pico8.script + position, pico8.script + (position - offset), copyLength);
            position += copyLength;
         }
      }
   }
   else {
      pico8.script = malloc(0x3D00 + 1);
      memcpy(pico8.script, decodeImage + 0x4300, 0x3D00);
   }

   free(image);
   free(decodeImage);

   return false;
}

#endif
