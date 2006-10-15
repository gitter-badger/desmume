/*  Copyright (C) 2006 yopyop
    yopyop156@ifrance.com
    yopyop156.ifrance.com

    This file is part of DeSmuME

    DeSmuME is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    DeSmuME is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with DeSmuME; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "NDSSystem.h"
#include <stdlib.h>

NDSSystem nds;

void NDS_Init(void) {
     nds.ARM9Cycle = 0;
     nds.ARM7Cycle = 0;
     nds.cycles = 0;
     MMU_Init();
     nds.nextHBlank = 3168;
     nds.VCount = 0;
     nds.lignerendu = FALSE;

     Screen_Init();
     
     armcpu_new(&NDS_ARM7,1);
     armcpu_new(&NDS_ARM9,0);

     //ARM7 BIOS IRQ HANDLER
     MMU_writeWord(1, 0x00, 0xE25EF002);
     MMU_writeWord(1, 0x04, 0xEAFFFFFE);
     MMU_writeWord(1, 0x18, 0xEA000000);
     MMU_writeWord(1, 0x20, 0xE92D500F);
     MMU_writeWord(1, 0x24, 0xE3A00301);
     MMU_writeWord(1, 0x28, 0xE28FE000);
     MMU_writeWord(1, 0x2C, 0xE510F004);
     MMU_writeWord(1, 0x30, 0xE8BD500F);
     MMU_writeWord(1, 0x34, 0xE25EF004);
    
     //ARM9 BIOS IRQ HANDLER
     MMU_writeWord(0, 0xFFF0018, 0xEA000000);
     MMU_writeWord(0, 0xFFF0020, 0xE92D500F);
     MMU_writeWord(0, 0xFFF0024, 0xEE190F11);
     MMU_writeWord(0, 0xFFF0028, 0xE1A00620);
     MMU_writeWord(0, 0xFFF002C, 0xE1A00600);
     MMU_writeWord(0, 0xFFF0030, 0xE2800C40);
     MMU_writeWord(0, 0xFFF0034, 0xE28FE000);
     MMU_writeWord(0, 0xFFF0038, 0xE510F004);
     MMU_writeWord(0, 0xFFF003C, 0xE8BD500F);
     MMU_writeWord(0, 0xFFF0040, 0xE25EF004);
        
     MMU_writeWord(0, 0x0000004, 0xE3A0010E);
     MMU_writeWord(0, 0x0000008, 0xE3A01020);
//     MMU_writeWord(0, 0x000000C, 0xE1B02110);
     MMU_writeWord(0, 0x000000C, 0xE1B02040);
     MMU_writeWord(0, 0x0000010, 0xE3B02020);
//     MMU_writeWord(0, 0x0000010, 0xE2100202);
}

void NDS_DeInit(void) {
     if(MMU.CART_ROM != MMU.UNUSED_RAM)
        NDS_FreeROM();

     nds.nextHBlank = 3168;
     Screen_DeInit();
     MMU_DeInit();
}

BOOL NDS_SetROM(u8 * rom, u32 mask)
{
     u32 i;

     MMU_setRom(rom, mask);
     
     NDS_header * header = (NDS_header *)MMU.CART_ROM;
     
     u32 src = header->ARM9src>>2;
     u32 dst = header->ARM9cpy;

     for(i = 0; i < (header->ARM9binSize>>2); ++i)
     {
          MMU_writeWord(0, dst, ((u32 *)rom)[src]);
          dst+=4;
          ++src;
     }

     src = header->ARM7src>>2;
     dst = header->ARM7cpy;
     
     for(i = 0; i < (header->ARM7binSize>>2); ++i)
     {
          MMU_writeWord(1, dst, ((u32 *)rom)[src]);
          dst+=4;
          ++src;
     }
     armcpu_init(&NDS_ARM7, header->ARM7exe);
     armcpu_init(&NDS_ARM9, header->ARM9exe);
     
     nds.ARM9Cycle = 0;
     nds.ARM7Cycle = 0;
     nds.cycles = 0;
     nds.nextHBlank = 3168;
     nds.VCount = 0;
     nds.lignerendu = FALSE;

     MMU_writeHWord(0, 0x04000130, 0x3FF);
     MMU_writeHWord(1, 0x04000130, 0x3FF);
     MMU_writeByte(1, 0x04000136, 0x43);

     MMU_writeByte(0, 0x027FFCDC, 0x20);
     MMU_writeByte(0, 0x027FFCDD, 0x20);
     MMU_writeByte(0, 0x027FFCE2, 0xE0);
     MMU_writeByte(0, 0x027FFCE3, 0x80);
     
     MMU_writeHWord(0, 0x027FFCD8, 0x20<<4);
     MMU_writeHWord(0, 0x027FFCDA, 0x20<<4);
     MMU_writeHWord(0, 0x027FFCDE, 0xE0<<4);
     MMU_writeHWord(0, 0x027FFCE0, 0x80<<4);

     MMU_writeWord(0, 0x027FFE40, 0xE188);
     MMU_writeWord(0, 0x027FFE44, 0x9);
     MMU_writeWord(0, 0x027FFE48, 0xE194);
     MMU_writeWord(0, 0x027FFE4C, 0x0);
//     logcount = 0;
     
     MMU_writeByte(0, 0x023FFC80, 1);
     MMU_writeByte(0, 0x023FFC82, 10);
     MMU_writeByte(0, 0x023FFC83, 7);
     MMU_writeByte(0, 0x023FFC84, 15);
     
     MMU_writeHWord(0, 0x023FFC86, 'y');
     MMU_writeHWord(0, 0x023FFC88, 'o');
     MMU_writeHWord(0, 0x023FFC8A, 'p');
     MMU_writeHWord(0, 0x023FFC8C, 'y');
     MMU_writeHWord(0, 0x023FFC8E, 'o');
     MMU_writeHWord(0, 0x023FFC90, 'p');
     MMU_writeHWord(0, 0x023FFC9A, 6);
     
     MMU_writeHWord(0, 0x023FFC9C, 'H');
     MMU_writeHWord(0, 0x023FFC9E, 'i');
     MMU_writeHWord(0, 0x023FFCA0, ',');
     MMU_writeHWord(0, 0x023FFCA2, 'i');
     MMU_writeHWord(0, 0x023FFCA4, 't');
     MMU_writeHWord(0, 0x023FFCA6, '\'');
     MMU_writeHWord(0, 0x023FFCA8, 's');
     MMU_writeHWord(0, 0x023FFCAA, ' ');
     MMU_writeHWord(0, 0x023FFCAC, 'm');
     MMU_writeHWord(0, 0x023FFCAE, 'e');
     MMU_writeHWord(0, 0x023FFCB0, '!');
     MMU_writeHWord(0, 0x023FFCD0, 11);

     MMU_writeHWord(0, 0x023FFCE4, 2);
          
     MMU_writeWord(0, 0x027FFE40, header->FNameTblOff);
     MMU_writeWord(0, 0x027FFE44, header->FNameTblSize);
     MMU_writeWord(0, 0x027FFE48, header->FATOff);
     MMU_writeWord(0, 0x027FFE4C, header->FATSize);
     
     MMU_writeWord(0, 0x027FFE50, header->ARM9OverlayOff);
     MMU_writeWord(0, 0x027FFE54, header->ARM9OverlaySize);
     MMU_writeWord(0, 0x027FFE58, header->ARM7OverlayOff);
     MMU_writeWord(0, 0x027FFE5C, header->ARM7OverlaySize);
     
     MMU_writeWord(0, 0x027FFE60, header->unknown2a);
     MMU_writeWord(0, 0x027FFE64, header->unknown2b);  //merci �EACKiX

     MMU_writeWord(0, 0x027FFE70, header->ARM9unk);
     MMU_writeWord(0, 0x027FFE74, header->ARM7unk);

     MMU_writeWord(0, 0x027FFF9C, 0x027FFF90); // ?????? besoin d'avoir la vrai valeur sur ds
     
     nds.touchX = nds.touchY = 0;
     MainScreen.offset = 192;
     SubScreen.offset = 0;
     
     //MMU_writeWord(0, 0x02007FFC, 0xE92D4030);
     
     return TRUE;
} 

NDS_header * NDS_getROMHeader(void)
{
     return (NDS_header *)MMU.CART_ROM;
} 

void NDS_setTouchPos(u16 x, u16 y)
{
     nds.touchX = (x <<4);
     nds.touchY = (y <<4);
     
     MMU.ARM7_REG[0x136] &= 0xBF;
}

void NDS_releasTouch(void)
{ 
     nds.touchX = 0;
     nds.touchY = 0;
     
     MMU.ARM7_REG[0x136] |= 0x40;
}

void debug()
{
     //if(NDS_ARM9.R[15]==0x020520DC) execute = FALSE;
     //DSLinux
          //if(NDS_ARM9.CPSR.bits.mode == 0) execute = FALSE;
          //if((NDS_ARM9.R[15]&0xFFFFF000)==0) execute = FALSE;
          //if((NDS_ARM9.R[15]==0x0201B4F4)/*&&(NDS_ARM9.R[1]==0x0)*/) execute = FALSE;
     //AOE
          //if((NDS_ARM9.R[15]==0x01FFE194)&&(NDS_ARM9.R[0]==0)) execute = FALSE;
          //if((NDS_ARM9.R[15]==0x01FFE134)&&(NDS_ARM9.R[0]==0)) execute = FALSE;

     //BBMAN
          //if(NDS_ARM9.R[15]==0x02098B4C) execute = FALSE;
          //if(NDS_ARM9.R[15]==0x02004924) execute = FALSE;
          //if(NDS_ARM9.R[15]==0x02004890) execute = FALSE;
     
     //if(NDS_ARM9.R[15]==0x0202B800) execute = FALSE;
     //if(NDS_ARM9.R[15]==0x0202B3DC) execute = FALSE;
     //if((NDS_ARM9.R[1]==0x9AC29AC1)&&(!fait)) {execute = FALSE;fait = TRUE;}
     //if(NDS_ARM9.R[1]==0x0400004A) {execute = FALSE;fait = TRUE;}
     /*if(NDS_ARM9.R[4]==0x2E33373C) execute = FALSE;
     if(NDS_ARM9.R[15]==0x02036668) //execute = FALSE;
     {
     nds.logcount++;
     sprintf(logbuf, "%d %08X", nds.logcount, NDS_ARM9.R[13]);
     log::ajouter(logbuf);
     if(nds.logcount==89) execute=FALSE;
     }*/
     //if(NDS_ARM9.instruction==0) execute = FALSE;
     //if((NDS_ARM9.R[15]>>28)) execute = FALSE;
}

#define DSGBA_EXTENSTION ".ds.gba"
#define DSGBA_LOADER_SIZE 512
enum
{
	ROM_NDS = 0,
	ROM_DSGBA
};

int NDS_LoadROM(const char *filename)
{
   int i;
   int type;
   const char *p = filename;
   FILE *file;
   u32 size, mask;
   u8 *data;

   if (filename == NULL)
      return -1;

   type = ROM_NDS;
	
   p += strlen(p);
   p -= strlen(DSGBA_EXTENSTION);

   if(memcmp(p, DSGBA_EXTENSTION, strlen(DSGBA_EXTENSTION)) == 0)
      type = ROM_DSGBA;
	
   if ((file = fopen(filename, "rb")) == NULL)
      return -1;
	
   fseek(file, 0, SEEK_END);
   size = ftell(file);
   fseek(file, 0, SEEK_SET);
	
   if(type == ROM_DSGBA)
   {
      fseek(file, DSGBA_LOADER_SIZE, SEEK_SET);
      size -= DSGBA_LOADER_SIZE;
   }
	
   mask = size;
   mask |= (mask >>1);
   mask |= (mask >>2);
   mask |= (mask >>4);
   mask |= (mask >>8);
   mask |= (mask >>16);

   // Make sure old ROM is freed first(at least this way we won't be eating
   // up a ton of ram before the old ROM is freed)
   if(MMU.CART_ROM != MMU.UNUSED_RAM)
      NDS_FreeROM();

   if ((data = (u8*)malloc(mask + 1)) == NULL)
   {
      fclose(file);
      return -1;
   }
	
   i = fread(data, 1, size, file);
	
   fclose(file);
	
   MMU_unsetRom();
   NDS_Reset();
   NDS_SetROM(data, mask);

/* // Will be added later
   strcpy(szRomPath, dirname((char *) filename));
   cflash_close();
   cflash_init();
*/

   return i;
}

void NDS_FreeROM(void)
{
   if (MMU.CART_ROM != MMU.UNUSED_RAM)
      free(MMU.CART_ROM);
   MMU_unsetRom();
}

void NDS_Reset(void)
{
   BOOL oldexecute=execute;
   int i;

   execute = FALSE;

   // Reset emulation here

   execute = oldexecute;
}

typedef struct
{
    u32 size;
    s32 width;
    s32 height;
    u16 planes;
    u16 bpp;
    u32 cmptype;
    u32 imgsize;
    s32 hppm;
    s32 vppm;
    u32 numcol;
    u32 numimpcol;
} bmpimgheader_struct;

typedef struct
{
    u16 id __PACKED;
    u32 size __PACKED;
    u16 reserved1 __PACKED;
    u16 reserved2 __PACKED;
    u32 imgoffset __PACKED;
} bmpfileheader_struct;

int NDS_WriteBMP(const char *filename)
{
    bmpfileheader_struct fileheader;
    bmpimgheader_struct imageheader;
    FILE *file;
    int i,j,k;
    u16 *bmp = GPU_screen;

    memset(&fileheader, 0, sizeof(fileheader));
    fileheader.size = sizeof(fileheader);
    fileheader.id = 'B' | ('M' << 8);
    fileheader.imgoffset = sizeof(fileheader)+sizeof(imageheader);
    
    memset(&imageheader, 0, sizeof(imageheader));
    imageheader.size = sizeof(imageheader);
    imageheader.width = 256;
    imageheader.height = 192*2;
    imageheader.planes = 1;
    imageheader.bpp = 24;
    imageheader.cmptype = 0; // None
    imageheader.imgsize = imageheader.width * imageheader.height * 3;
    
    if ((file = fopen(filename,"wb")) == NULL)
       return 0;

    fwrite(&fileheader, 1, sizeof(fileheader), file);
    fwrite(&imageheader, 1, sizeof(imageheader), file);

    for(j=0;j<192*2;j++)
    {
       for(i=0;i<256;i++)
       {
          u8 r,g,b;
          u16 pixel = bmp[(192*2-j-1)*256+i];
          r = pixel>>10;
          pixel-=r<<10;
          g = pixel>>5;
          pixel-=g<<5;
          b = pixel;
          r*=255/31;
          g*=255/31;
          b*=255/31;
          fwrite(&r, 1, sizeof(u8), file); 
          fwrite(&g, 1, sizeof(u8), file); 
          fwrite(&b, 1, sizeof(u8), file);
       }
    }
    fclose(file);

    return 1;
}
