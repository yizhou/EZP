/*-------------------------------------------------------------------
* Name: devbw.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern unsigned char PattnBuf6[250][36];

void BW_setGray(int gray)
{
     if(gray>250) sysColor=0xfffe;      // need not draw
     else
     if(gray==0) sysColor=EGA_BLACK;
     else { sysColor=0xffff; sysGray=gray; }
}

static unsigned char zjhdot[]={0x80,0x40,0x20,0x10,8,4,2,1,0,0,0};
void BW_scanline(int x1,int x2,int y1,LPDC lpdc)
{
   LONG byteoff;
   int headbit,tailbit,byte1,byte2;
   LPBYTE p;
   int i,nx,ny1,ny2,len;
   int top,bottom,left,right;

   //right=printer->xpixel;
   if (UsePrintCut())
   {
     right=min(RastWidth,printer->xpixel);
     left=0;
     top=PrinterBlockTop[CurrentPrinterBlock];
     bottom=PrinterBlockBottom[CurrentPrinterBlock];
   }
   else
   {
     right=lpdc->right;
     left=lpdc->left;
     top=lpdc->top;
     bottom=lpdc->bottom;
   }

   if (sysColor==0xfffe) return;

   //if (GlobalRorate90)
   //  lpdc=&myDC;

  /*  By zjh 96.9.6  for print reverse */

  /* add end */

   if (x1>x2) {
      int tmp=x2;
      x2=x1;
      x1=tmp;
   }

if (GlobalRorate90)
{
      nx=PageHightDot-y1;
      ny1=x1;
      ny2=x2;

      nx +=GlobalXOffset;
      ny1 +=GlobalYOffset;
      ny2 +=GlobalYOffset;

      if (GlobalReverse)
      {
        nx=GlobalXRes-nx;
      }

      if (GlobalYReverse)
      {
        i=GlobalYRes-ny1;
        ny1=GlobalYRes-ny2;
        ny2=i;
      }

      /*
      if (GlobalReverse)
      {
        nx=lpdc->left+lpdc->right-nx;
      }
      */

      if ((ny1>=bottom)||(ny2<top)) return;
      if ((nx>=right)||(nx<left)) return;
      if (ny1<top) ny1=top;
      if (ny2>=bottom) ny2=bottom-1;

      /*
      if (GlobalYReverse)
      {
        i=lpdc->top+lpdc->bottom-ny1;
        ny1=lpdc->top+lpdc->bottom-ny2;
        ny2=i;
      }
      */

   /*--------- added ByHance, 96,4.11, for printer's fixed margin ----*/
      nx-=PrinterFixedLeftMargin;
      if(nx<0) return;
    /*------- end ------*/

    fDither=TRUE;
    //   if(y1>MaxRastY) MaxRastY=y1;

   //line(nx/6,ny1/6,nx/6,ny1/6);
   byte1 = nx>>3;
   headbit = nx & 7;

   byteoff = (ny1 - top)*RastWidthByte+byte1;

   p = &rasts[0][byteoff];
   len=ny2-ny1+1;

   if (sysColor==EGA_BLACK)
   {

       for (i=0;i<len;i++)
        {
          *p |= zjhdot[headbit];
          p=p+RastWidthByte;
        }
   }
   else
   if (sysColor==0xffff)
   {            // use gray
       unsigned char *patbuf=&PattnBuf6[sysGray-1][0];
       unsigned char v;
       int pttnWidthByte = 3;
       int pttnHeight = 12;
       int pttByte;

       pttByte = byte1%pttnWidthByte;
       ny1=(ny1%pttnHeight)*pttnWidthByte+pttByte;
       ny2=pttnHeight*pttnWidthByte;
       for (i=0;i<len;i++)
        {
            v= (patbuf[ny1]&zjhdot[headbit]);
            *p &= (~zjhdot[headbit]);
            *p |= v;
            p=p+RastWidthByte;
            ny1+=pttnWidthByte;
            if (ny1>ny2) ny1=ny1-ny2;
        }
   }
   else
   {
       for (i=0;i<len;i++)
         {
         *p &= (~zjhdot[headbit]);
         p=p+RastWidthByte;
         }
   }

    return ;
}
   /*
   if (GlobalReverse)
    {
        i=lpdc->left+lpdc->right-x1;
        x1=lpdc->left+lpdc->right-x2;
        x2=i;
    }
   */
      x1 +=GlobalXOffset;
      x2 +=GlobalXOffset;
      y1  +=GlobalYOffset;

     if (GlobalReverse)
     {
        i=GlobalXRes-x1;
        x1=GlobalXRes-x2;
        x2=i;
     }

    if (GlobalYReverse)
     {
        y1=GlobalYRes-y1;
     }

   if ((x1>=right)||(x2<left)) return;
   if ((y1>=bottom)||(y1<top)) return;
   /*
   if (GlobalYReverse)
    {
       y1=lpdc->top+lpdc->bottom-y1;
    }
    */

   if (x1<left) x1=left;
   if (x2>=right) x2=right-1;

 /*--------- added ByHance, 96,4.11, for printer's fixed margin ----*/
   x2-=PrinterFixedLeftMargin;
   if(x2<0) return;
   x1-=PrinterFixedLeftMargin;
   if(x1<0) x1=0;
  /*------- end ------*/

   fDither=TRUE;
//   if(y1>MaxRastY) MaxRastY=y1;

   byte1 = x1>>3;
   byte2 = x2>>3;
   headbit = x1 & 7;
   tailbit = x2 & 7;

   byteoff = (y1 - top)*RastWidthByte+byte1;
   p = &rasts[0][byteoff];

   if (sysColor==EGA_BLACK)
   {
       if (byte1==byte2) {
          *p |= headdot[headbit]&taildot[tailbit];
       } else {
          *p |= headdot[headbit];
          p++; byte1++;
          while (byte1++<byte2) *p++ = 0xff;
          *p |=taildot[tailbit];
       }
   }
   else
   if (sysColor==0xffff)
   {            // use gray
       unsigned char *patbuf=&PattnBuf6[sysGray-1][0], *pttn;
       unsigned char v;
       int pttnWidthByte = 3;
       int pttnHeight = 12;
       int pttByte;

       pttByte = byte1%pttnWidthByte;
       pttn = &patbuf[(y1%pttnHeight)*pttnWidthByte];

       if (byte1==byte2) {
          v = pttn[pttByte];
          v &= headdot[headbit]&taildot[tailbit];
          *p |= v;
       }
       else
       {
          v = pttn[pttByte];
          v &= headdot[headbit];
          *p |= v;
          p++; byte1++; pttByte++;
          if (pttByte == pttnWidthByte) pttByte = 0;

          while (byte1++<byte2){
              v = pttn[pttByte];
              *p |= v;
              p++;
              pttByte++;
              if (pttByte == pttnWidthByte) pttByte = 0;
          }
          v = pttn[pttByte];
          v &= taildot[tailbit];
          *p |= v;
       }
   }
   else
   {            // EGA_WHITE
       if (byte1==byte2) {
          *p &= ~(headdot[headbit]&taildot[tailbit]);
       } else {
          *p &= ~(headdot[headbit]);
          p++; byte1++;
          while (byte1++<byte2) *p++ = 0;
          *p &= ~(taildot[tailbit]);
       }
   }
} /* BW_scanfill */

