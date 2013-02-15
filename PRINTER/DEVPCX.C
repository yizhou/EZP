/*--------- pcx ----------*/
/*-------------------------------------------------------------------
* Name: devpcx.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int pic_dpi;
static int pcx_init(UDATA pagew,UDATA pageh)
{
   int m,lines;
     #define PCX_HEAD_LEN   0x80
   unsigned char pcxhead[PCX_HEAD_LEN];

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);

   printer->resolution=pic_dpi;
   // printer->xpixel=((pagew*pic_dpi/SCALEMETER+15)/16)*16;
   printer->xpixel=((pagew*pic_dpi/SCALEMETER+7)/8)*8;
   printer->ypixel=pageh*pic_dpi/SCALEMETER;

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;

   memset(pcxhead,0,PCX_HEAD_LEN);
   *(short *)&pcxhead[0]=0x50a;
   *(short *)&pcxhead[2]=0x101;
   *(short *)&pcxhead[0x8]=printer->xpixel-1;
   *(short *)&pcxhead[0xa]=printer->ypixel-1;
   *(short *)&pcxhead[0xc]=pic_dpi;
   *(short *)&pcxhead[0xe]=pic_dpi;
   pcxhead[0x10+3]=pcxhead[0x10+4]=pcxhead[0x10+5]=0xff;
   pcxhead[0x41]=1;     /*- plane -*/
   *(short *)&pcxhead[0x42]=RastWidthByte;     /*- bytes per line -*/
   pcxhead[0x44]=1;
   fwrite(pcxhead,1,PCX_HEAD_LEN,prnstr);

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

static void pcx_FF()
{
}

static void pcx_over()
{
   fclose(prnstr);
   free(rasts[0]);
}

static int pcx_getheight() { return RastHeight; }
static void pcx_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   BW_scanline(x1,x2,y,lpdc);
}

#define byte    unsigned char
/* Write one line in PCX run-length-encoded format. */
static int
pcx_write_rle(const byte *from, const byte *end)
{
        while ( from < end )
        {       byte data = *from++;
                if ( data != *from || from == end )
                  {     if ( (byte)(~data) >= 0xc0 )
                           if(putc(0xc1, prnstr)==EOF)
                              return 1;
                  }
                else
                  {     const byte *start = from;
                        while ( (from < end) && (*from == data) )
                          from ++;
                        /* Now (from - start) + 1 is the run length. */
                        while ( from - start >= 0x3f )
                          {
                            if(putc(0xff, prnstr)==EOF)
                               return 1;
                            if(putc(~data, prnstr)==EOF)
                               return 1;
                            start += 0x3f;
                          }

                        if ( from > start || (byte)(~data) >= 0xc0 )
                          if(putc(from - start + 0xc1, prnstr)==EOF)
                               return 1;
                  }

                if(putc(~data, prnstr)==EOF)
                   return 1;
        }
        return 0;
}

static void pcx_block()
{
    int  i;
    char *linebuf;
    LPDC lpdc=&SysDc;
    unsigned int bytes_per_line=RastWidthByte;

    for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
    {
         linebuf=&rasts[0][(i-lpdc->top)*RastWidthByte];
         if(pcx_write_rle(linebuf,linebuf+bytes_per_line))
            return;
    } /*--- i ---*/

    memset(rasts[0],0,RastSize);        // clear buffer
} /* pcx_block */

static void pcx_setcolor(int color)
{
   sysColor = color;
}
static void pcx_setGray(int gray)
{
   BW_setGray(gray);
}
static void pcx_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   pcx_setGray(gray);
}
static void pcx_setCMYKcolor(int c,int m,int y,int k)
{
}

PRINTER PCXprinter = {
  DEV_BW,
  pcx_init,
  pcx_block,
  pcx_FF,
  pcx_over,
  pcx_getheight,
  pcx_scanfill,
  pcx_setcolor,
  pcx_setRGBcolor,
  pcx_setCMYKcolor,
  pcx_setGray,
  600,
  60*115,
  60*170,
  0,0
};

