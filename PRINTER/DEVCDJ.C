/*--------- HP: Deskjet color driver ----------*/
/*-------------------------------------------------------------------
* Name: devcdj.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

int blanklines;

static int cdj_init(UDATA pagew,UDATA pageh)
{
   int m,lines;
   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);

   fprintf(prnstr, "\033E");
    /* Select data compression mode 2 */
   fputs("\033*b2M",prnstr);
    /* x,y offset is 0; DPI=300, using CMYK color */
   fprintf(prnstr,"\033&l3A\033&l0E\033*p0X\033*p0Y\033*t300R\033*r-3U\033*r0A");

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;
   // lines= printer->ypixel+32;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
            // for other malloc's space   -----------^
   lines=60+32;
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

   blanklines = 0;
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


static void cdj_FF()
{
   fputs("\033*rbC\033E", prnstr);
   fputs("\033&l0H",prnstr);        //FORM FEED
   blanklines = 0;
}

static void cdj_over()
{
  /* end raster graphics & reset printer */
     fclose(prnstr);
     free(rasts[0]);
     CloseDitherBuf();
}

static int cdj_getheight() { return RastHeight; }
static void cdj_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   RGB_scanline(x1,x2,y,lpdc);
}


static void cdj_block()
{
    int i,byteoff,j;
    int cntc,cntm,cnty,cc,mm,yy;
    UCHAR cbuf[1024],mbuf[1024],ybuf[1024],v;
    LPUCHAR p,pc,pm,py,pk;
    LPDC lpdc=&SysDc;

    if(fDither) DitherRGB(lpdc);    // dither RGB to CMYK

    for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
    {
       if(fDither && i<=MaxRastY)
       {
         byteoff =(i-lpdc->top)*RastWidthByte;
         cc = mm = yy = RastWidthByte;
         pc = rasts[0]+byteoff;
         pm = rasts[1]+byteoff;
         py = rasts[2]+byteoff;
         pk = rasts[3]+byteoff;
         for (j=0;j<cc;j++) {
             v = pk[j];
             pc[j] |= v;
             pm[j] |= v;
             py[j] |= v;
         }
         p = rasts[0]+byteoff;
         while (cc>0&&p[cc-1]==0) cc--;
         if (cc>0) cntc = RLEcompress((ULONG *)p,(ULONG *)(p+cc),cbuf);
         else cntc=0;

         p = rasts[1]+byteoff;
         while (mm>0&&p[mm-1]==0) mm--;
         if (mm>0) {
               cntm = RLEcompress((ULONG *)p,(ULONG *)(p+mm),mbuf);
         }
         else cntm=0;

         p = rasts[2]+byteoff;
         while (yy>0&&p[yy-1]==0) yy--;
         if (yy>0) {
                cnty = RLEcompress((ULONG *)p,(ULONG *)(p+yy),ybuf);
         }

         else cnty=0;
         if (cc==0&&mm==0&&yy==0) {
             blanklines++;
             continue;
         }

         for(;blanklines;blanklines--) {
              fputs("\033*bW",prnstr);
         }

         fprintf(prnstr, "\033*b%dV", cntc);     //cyan
         fwrite(cbuf,1,cntc,prnstr);

         fprintf(prnstr, "\033*b%dV", cntm);    //magenta
         fwrite(mbuf,1,cntm,prnstr);

         fprintf(prnstr, "\033*b%dW", cnty);     //yellow
         fwrite(ybuf,1,cnty,prnstr);
      } else blanklines++;
    } /*--- i ---*/

   /*-----------------
    for(;blanklines;blanklines--) {
              fputs("\033*bW",prnstr);
    }
    -------*/
    memset(rasts[4],0xff,RastSize);        // clear RGB buffer
} /* cdj_block */

static void cdj_setcolor(int color)
{
   setSYScolor(color);
}
static void cdj_setGray(int gray)
{
   setGray(gray);
}
static void cdj_setRGBcolor(int r,int g,int b)
{
   setRGBcolor(r,g,b);
}
static void cdj_setCMYKcolor(int c,int m,int y,int k)
{
   setCMYKcolor(c,m,y,k);
}

PRINTER HP1200_300printer = {
  DEV_CMYK,
  cdj_init,
  cdj_block,
  cdj_FF,
  cdj_over,
  cdj_getheight,
  cdj_scanfill,
  cdj_setcolor,
  cdj_setRGBcolor,
  cdj_setCMYKcolor,
  cdj_setGray,
  300,
  30*85,                 // 300x8.5
  30*115,                // 300*11.5
  59,0,                // leftmargin=5mm
};
