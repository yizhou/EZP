/*--------- Epson stylus 800,1000( using esc/p2): A4,360DPI ----------*/
/*-------------------------------------------------------------------
* Name: devhp.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int blanklines;
static int escp_init(UDATA pagew,UDATA pageh)
{
   int m,lines;

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);

  // init printer
   fwrite("\033@\033(G\001\000\001", 1, 8, prnstr);
  // paper size is A4 360 dpi
   //fwrite("\033(U\001\000\012\033(C\002\0t\020\033(c\004\0\0\0t\020",1,22,prnstr);
   fwrite("\033(U\001\000\012\033(C\002\000\xE0\x20\033(c\004\000\x56\x0\x7c\x1f",1,22, prnstr);

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;
   //lines= printer->ypixel+32;
   lines=512+32;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
            // for other malloc's space   -----------^
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
   fFirstBlock=TRUE;
   blanklines = 0;
   return 1;
}

static void escp_FF()
{
   fputc(0x0c,prnstr);        //FORM FEED
   blanklines = 0;
}

static void escp_over()
{
     fputs("\033@", prnstr);
     fclose(prnstr);
     free(rasts[0]);
}

static int escp_getheight() { return RastHeight; }
static void escp_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   BW_scanline(x1,x2,y,lpdc);
}
static void escp_block()
{
    int i;
    int cntk,kk;
//  int blanklines;
    UCHAR kbuf[1024];
    LPUCHAR p;
    LPDC lpdc=&SysDc;

//  blanklines = 0;
    for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
    {
       p=&rasts[0][(i-lpdc->top)*RastWidthByte];
       kk = RastWidthByte;

       while (kk>0&&p[kk-1]==0) kk--;
       if (kk>0) cntk = RLEcompress((ULONG *)p,(ULONG *)(p+kk),kbuf);
       else
       {
           blanklines++;
           continue;
       }

       if(blanklines) {
          fwrite("\033(v\002\000",1,5,prnstr);  // skip line relative
          fputc(blanklines%256,prnstr);
          fputc(blanklines/256,prnstr);
          blanklines = 0;
       }

        // send black line
       kk *= 8;
       fwrite("\033r\000\033.\001\012\012\001",1,9,prnstr);
       fputc(kk%256,prnstr); fputc(kk/256,prnstr);
       fwrite(kbuf,1,cntk,prnstr);
       fputc(0x0d,prnstr);

       fwrite("\033(v\002\000\001\000",1,7,prnstr);   // skip 1 scan line
    } /*-- i --*/
/*-------------------------
    if(blanklines) {
       fwrite("\033(v\002\000",1,5,prnstr);  // skip line relative
       fputc(blanklines%256,prnstr);
       fputc(blanklines/256,prnstr);
    }
----------------------------*/
    // if(fDither)
      memset(rasts[0],0,RastSize);        // clear buffer
} /* escp_block */

static void escp_setcolor(int color)
{
   sysColor = color;
}
static void escp_setGray(int gray)
{
   BW_setGray(gray);
}
static void escp_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   escp_setGray(gray);
}
static void escp_setCMYKcolor(int c,int m,int y,int k)
{
}

PRINTER ESCP2printer = {
  DEV_BW,
  escp_init,
  escp_block,
  escp_FF,
  escp_over,
  escp_getheight,
  escp_scanfill,
  escp_setcolor,
  escp_setRGBcolor,
  escp_setCMYKcolor,
  escp_setGray,
  360,
  36*85,                 // 360x8.5
  36*108,                // 360*10.8
  0,128,                // topmargin=9 mm
};

