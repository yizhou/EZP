/*--------- Stylus Color: A4, 360DPI,720DPI ----------*/
/*-------------------------------------------------------------------
* Name: devsc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int blanklines;
static int sc_init(char *sc_init_str,int init_str_len)
{
   int m,lines;
   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);
  // init printer
   fwrite("\033@\033(G\001\000\001", 1, 8, prnstr);
  // paper size
   fwrite(sc_init_str,1,init_str_len, prnstr);
   fwrite("\033U\001", 1, 3, prnstr);   // one direction

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;
   // lines= printer->ypixel+32;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
            // for other malloc's space   -----------^
   lines=100;
   rasts[0]=NULL;

   while (rasts[0]==NULL && lines>32)
   {
       lines -= 32;
       m = lines*(4*RastWidthByte+3*RastWidth);
       rasts[0] = malloc(m);
   }

   if(rasts[0]==NULL)
   {
       fclose(prnstr);
       return(-1);
   }

   RastHeight=lines;
   m=lines*RastWidthByte;                // cmyk
   rasts[1] = rasts[0]+m;
   rasts[2] = rasts[1]+m;
   rasts[3] = rasts[2]+m;
   rasts[4] = rasts[3]+m;

   m=lines*RastWidth;                    // rgb
   RastSize = m*3;
   rasts[5] = rasts[4]+m;
   rasts[6] = rasts[5]+m;
   memset(rasts[4],0xff,RastSize);         // clear it

   fRemapRGB = TRUE;
   blanklines = 0;

   m=InitDitherBuf();
   if(m<0)
   {
       free(rasts[0]);
       fclose(prnstr);
       return m;
   }
   return 1;
}

static char *sc_step_str;

static int sc360_init(UDATA pagew,UDATA pageh)
{
  // paper size is A4 360 dpi
   static char *sc360_init_str="\033(U\001\000\012\033(C\002\000\xE0\x20\033(c\004\000\x56\x0\x7c\x1f";
   static char *sc360_step_str="\012\012\001";

   sc_step_str=sc360_step_str;
   return sc_init(sc360_init_str,22);
}

static int sc720_init(UDATA pagew,UDATA pageh)
{
  // paper size is A4 720 dpi
   static char *sc720_init_str="\033(U\001\000\005\033(C\002\000\xE0\x20\033(c\004\000\x56\x0\x7c\x1f"
                               "\033(i\001\000\001";    // MicroAdjustion: ON
   static char *sc720_step_str="\005\005\001";

   sc_step_str=sc720_step_str;
   return sc_init(sc720_init_str,28);
}

static void sc_FF()
{
   fputc(0x0c,prnstr);        //FORM FEED
   blanklines = 0;
}

static void sc_over()
{
     free(rasts[0]);
     CloseDitherBuf();
     fputs("\033@", prnstr);
     fclose(prnstr);
}

static int sc_getheight() { return RastHeight; }
static void sc_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   RGB_scanline(x1,x2,y,lpdc);
}

static void sc_block()
{
  int i,byteoff;
  int cntc,cntm,cnty,cntk,cc,mm,yy,kk;
//  int blanklines;
  UCHAR cbuf[1024],mbuf[1024],ybuf[1024],kbuf[1024];
  LPUCHAR p;
  LPDC lpdc=&SysDc;

  if(fDither) DitherRGB(lpdc);    // dither RGB to CMYK
   // else memset(rasts[0],0,RastHeight*RastWidthByte*4);

//  blanklines = 0;
  for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
  {
     if(fDither && i<=MaxRastY)
     {
         byteoff =(i-lpdc->top)*RastWidthByte;
         cc = mm = yy = kk = RastWidthByte;
         // cntc=cntm=cnty=cntk=0;

         p = rasts[0]+byteoff;
         while (cc>0&&p[cc-1]==0) cc--;
         if (cc>0) cntc = RLEcompress((ULONG *)p,(ULONG *)(p+cc),cbuf);

         p = rasts[1]+byteoff;
         while (mm>0&&p[mm-1]==0) mm--;
         if (mm>0) cntm = RLEcompress((ULONG *)p,(ULONG *)(p+mm),mbuf);

         p = rasts[2]+byteoff;
         while (yy>0&&p[yy-1]==0) yy--;
         if (yy>0) cnty = RLEcompress((ULONG *)p,(ULONG *)(p+yy),ybuf);

         p = rasts[3]+byteoff;
         while (kk>0&&p[kk-1]==0) kk--;
         if (kk>0) cntk = RLEcompress((ULONG *)p,(ULONG *)(p+kk),kbuf);

         if (cc==0&&mm==0&&yy==0&&kk==0) {
             blanklines++;
             continue;
         }

         if(blanklines) {
            fwrite("\033(v\002\000",1,5,prnstr);  // skip line relative
            fputc(blanklines%256,prnstr);
            fputc(blanklines/256,prnstr);
            blanklines = 0;
         }

         if (cc>0) {         // send cyan line
             cc *= 8;
             fwrite("\033r\002\033.\001",1,6,prnstr);
             fwrite(sc_step_str,1,3,prnstr);
             fputc(cc%256,prnstr); fputc(cc/256,prnstr);
             fwrite(cbuf,1,cntc,prnstr);
             fputc(0x0d,prnstr);
         }
         if (mm>0) {         // send magenta line
             mm *= 8;
             fwrite("\033r\001\033.\001",1,6,prnstr);
             fwrite(sc_step_str,1,3,prnstr);
             fputc(mm%256,prnstr); fputc(mm/256,prnstr);
             fwrite(mbuf,1,cntm,prnstr);
             fputc(0x0d,prnstr);
         }
         if (yy>0) {         // send yellow line
             yy *= 8;
             fwrite("\033r\004\033.\001",1,6,prnstr);
             fwrite(sc_step_str,1,3,prnstr);
             fputc(yy%256,prnstr); fputc(yy/256,prnstr);
             fwrite(ybuf,1,cnty,prnstr);
             fputc(0x0d,prnstr);
         }
         if (kk>0) {         // send black line
             kk *= 8;
             fwrite("\033r\000\033.\001",1,6,prnstr);
             fwrite(sc_step_str,1,3,prnstr);
             fputc(kk%256,prnstr); fputc(kk/256,prnstr);
             fwrite(kbuf,1,cntk,prnstr);
             fputc(0x0d,prnstr);
         }

         fwrite("\033(v\002\000\001\000",1,7,prnstr);   // skip 1 scan line
     }
     else  blanklines++;
  } /*-- i --*/
/*--------------------------
  if(blanklines) {
     fwrite("\033(v\002\000",1,5,prnstr);  // skip line relative
     fputc(blanklines%256,prnstr);
     fputc(blanklines/256,prnstr);
  }
-------------------------*/
  //if(fDither)
      memset(rasts[4],0xff,RastSize);        // clear RGB buffer
}

static void sc_setcolor(int color)
{
   setSYScolor(color);
}
static void sc_setRGBcolor(int r,int g,int b)
{
   setRGBcolor(r,g,b);
}
static void sc_setCMYKcolor(int c,int m,int y,int k)
{
   setCMYKcolor(c,m,y,k);
}
static void sc_setgray(int gray)
{
   setGray(gray);
}

PRINTER EPSON360SCprinter = {
  DEV_CMYK,
  sc360_init,
  sc_block,
  sc_FF,
  sc_over,
  sc_getheight,
  sc_scanfill,
  sc_setcolor,
  sc_setRGBcolor,
  sc_setCMYKcolor,
  sc_setgray,
  360,
  36*85,                // 360x8.5
  36*108,               // 360*10.8
  0,128,             // topmargin=9 mm
};

PRINTER EPSON720SCprinter = {
  DEV_CMYK,
  sc720_init,
  sc_block,
  sc_FF,
  sc_over,
  sc_getheight,
  sc_scanfill,
  sc_setcolor,
  sc_setRGBcolor,
  sc_setCMYKcolor,
  sc_setgray,
  720,
  72*85,                // 720x8.5
  72*108,               // 720*10.8
  0,255,             // topmargin=9 mm
};

