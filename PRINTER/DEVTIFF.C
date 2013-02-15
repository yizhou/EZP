/*--------- tiff ----------*/
/*-------------------------------------------------------------------
* Name: devtiff.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#include "tiffhead.h"

extern int pic_dpi;
static int tiff_init(UDATA pagew,UDATA pageh)
{
   int m,lines;

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);

   printer->resolution=pic_dpi;
   printer->xpixel=((pagew*pic_dpi/SCALEMETER+7)/8)*8;
   printer->ypixel=pageh*pic_dpi/SCALEMETER;

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;

   *(short *)&tifhead[10+1*12+8] = RastWidthByte*8;
   *(short *)&tifhead[10+2*12+8] = printer->ypixel;
       /*-- 0x117: bytesPerStrip --*/
   *(long  *)&tifhead[10+10*12+8] = (long)RastWidthByte*printer->ypixel;
   *(short *)&tifhead[TIFF_HEAD_LEN-2*8] = pic_dpi;
   *(short *)&tifhead[TIFF_HEAD_LEN-8] = pic_dpi;
   fwrite(tifhead,1,TIFF_HEAD_LEN,prnstr);

   //lines= printer->ypixel+32;
   lines=160+32;
   rasts[0]=NULL;
   while (rasts[0]==NULL && lines>32)
   {
       lines -= 32;
       m = lines*RastWidthByte;
       rasts[0] = malloc(m);
   }

   if(rasts[0]==NULL)
   {
       fclose(prnstr);
       return(-1);
   }

   RastHeight=lines;
   RastSize=m;
   memset(rasts[0],0,RastSize);         // clear it
   return 1;
}

static void tiff_FF()
{
}

static void tiff_over()
{
   fclose(prnstr);
   free(rasts[0]);
}

static int tiff_getheight() { return RastHeight; }
static void tiff_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   BW_scanline(x1,x2,y,lpdc);
}

static void tiff_block()
{
    int  i;
    char *linebuf;
    LPDC lpdc=&SysDc;

    for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
    {
         linebuf=&rasts[0][(i-lpdc->top)*RastWidthByte];
         fwrite(linebuf,sizeof(char),RastWidthByte,prnstr);
    } /*--- i ---*/

    memset(rasts[0],0,RastSize);        // clear buffer
} /* tiff_block */

static void tiff_setcolor(int color)
{
   sysColor = color;
}
static void tiff_setGray(int gray)
{
   BW_setGray(gray);
}
static void tiff_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   tiff_setGray(gray);
}
static void tiff_setCMYKcolor(int c,int m,int y,int k)
{
}

PRINTER TIFFprinter = {
  DEV_BW,
  tiff_init,
  tiff_block,
  tiff_FF,
  tiff_over,
  tiff_getheight,
  tiff_scanfill,
  tiff_setcolor,
  tiff_setRGBcolor,
  tiff_setCMYKcolor,
  tiff_setGray,
  600,
  60*115,
  60*170,
  0,0
};

