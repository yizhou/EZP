/*--------- bjc600: A4, 360DPI ----------*/
/*-------------------------------------------------------------------
* Name: devbjc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static int bjc_init(UDATA pagew,UDATA pageh)
{
   int m,lines;
   static unsigned char CanonInitStr[]=
         {
           /* Printer initial: ESC [ K ... */
           0x1b,0x5b,0x4b,0x02,0x00,0x00,0x0f,
           /* Set data compression: ESC ( b ... */
              0x1b,0x28,0x62,0x01,0x00,0x01,
           /* Select print mode: ESC ( c ... */
           0x1b,0x28,0x63,0x03,0x00,0x10,0x01,0x00,
           /* Select raster resolution: ESC ( d ... */
           0x1b,0x28,0x64,0x02,0x00,0x01,0x68,
         };

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);
   if (fwrite(CanonInitStr,sizeof(CanonInitStr),1,prnstr)<1)
       return(-1);

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;
   // lines= printer->ypixel+32;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
            // for other malloc's space   -----------^
   lines=100+32;
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

   m=InitDitherBuf();
   if(m<0)
   {
       free(rasts[0]);
       fclose(prnstr);
       return m;
   }
   return 1;
}

static void bjc_FF()
{
   fputc(0x0c,prnstr);        //FORM FEED
}

static void bjc_over()
{
     static unsigned char CanonEndStr[]=
     {
       /* ESC ( a ... */
       0x1b,0x28,0x61,0x01,0x00,0x00,
       /* Deselect data compression: ESC ( b ... */
       0x1b,0x28,0x62,0x01,0x00,0x00,
       /* Return to emu mode: ESC @ ... */
       0x1b,0x40
     };

     free(rasts[0]);
     CloseDitherBuf();
     fwrite(CanonEndStr,1,sizeof(CanonEndStr),prnstr);
     fclose(prnstr);
}

static int bjc_getheight() { return RastHeight; }
static void bjc_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   RGB_scanline(x1,x2,y,lpdc);
}

static void CANON_COMPRESS(int len,FILE *fp,LPBYTE p,char color);
static int IsZero(LPBYTE s,ULONG Length)
{
 /*------------
    int i = 0;
    LPBYTE mids = s;

    while (!(*mids) && i<Length) { i++; mids++;}
    if (i<Length) return 1;
    return 0;
  ----------*/
   if(s[0]==0 && !memicmp(s,s+1,Length-1))
      return 0;         // is zero line
   return 1;
}

static void bjc_block()
{
   LPDC lpdc=&SysDc;
   int i;

   if(fDither) DitherRGB(lpdc);    // dither RGB to CMYK
   // else memset(rasts[0],0,RastHeight*RastWidthByte*4);

   for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
   {
       if(fDither && i<=MaxRastY)
       {
           if( IsZero(rasts[2]+(long)(i-lpdc->top)*RastWidthByte,RastWidthByte) )
           {
               CANON_COMPRESS(RastWidthByte,prnstr,rasts[2]+(i-lpdc->top)*RastWidthByte,'Y');
           }

           if( IsZero(rasts[1]+(long)(i-lpdc->top)*RastWidthByte,RastWidthByte) )
           {
               CANON_COMPRESS(RastWidthByte,prnstr,rasts[1]+(i-lpdc->top)*RastWidthByte,'M');
           }

           if( IsZero(rasts[0]+(long)(i-lpdc->top)*RastWidthByte,RastWidthByte) )
           {
               CANON_COMPRESS(RastWidthByte,prnstr,rasts[0]+(i-lpdc->top)*RastWidthByte,'C');
           }

           if( IsZero(rasts[3]+(long)(i-lpdc->top)*RastWidthByte,RastWidthByte) )
           {
               CANON_COMPRESS(RastWidthByte,prnstr,rasts[3]+(i-lpdc->top)*RastWidthByte,'K');
           }
       }
        ////////////RASTER SKIP 1 LINE///////////////
       fputc(0x1b,prnstr); fputc(0x28,prnstr); fputc(0x65,prnstr);
       fputc(0x02,prnstr); fputc(0x00,prnstr); fputc(0x00,prnstr);
       fputc(0x01,prnstr);
   }

  // if(fDither)
      memset(rasts[4],0xff,RastSize);        // clear RGB buffer
}

static void bjc_setcolor(int color)
{
   setSYScolor(color);
}
static void bjc_setRGBcolor(int r,int g,int b)
{
   setRGBcolor(r,g,b);
}
static void bjc_setCMYKcolor(int c,int m,int y,int k)
{
   setCMYKcolor(c,m,y,k);
}
static void bjc_setgray(int gray)
{
   setGray(gray);
}

static void CANON_COMPRESS(int len,FILE *fp,LPBYTE p,char color)
{
    char out[4096];
    int count = 0;
    USHORT i=0;

    out[count++] = 0x1b;    /* ESC  ( A nn mm COLOR */
    out[count++] = '(';
    out[count++] = 'A';
    out[count++] = '\0';      /* nn */
    out[count++] = '\0';      /* mm */
    out[count++] = color;      // color
    i = RLEcompress((ULONG *)p,(ULONG *)(p+len),&out[count]);
    out[3] = i & 0xff;
    out[4] = (i/256) &0xff;
    out[i+5] = 0xd;            // PrinterCR;
    fwrite(out,1,i+6,fp);
}

PRINTER BJCprinter = {
  DEV_CMYK,
  bjc_init,
  bjc_block,
  bjc_FF,
  bjc_over,
  bjc_getheight,
  bjc_scanfill,
  bjc_setcolor,
  bjc_setRGBcolor,
  bjc_setCMYKcolor,
  bjc_setgray,
  360,
  36*85,                // 360x8.5
  36*110,               // 360*11.0
  0,0,
};

