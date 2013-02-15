/*-------------------------------------------------------------------
* Name: floyd.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#ifdef UNUSED   // ByHance, 96,1.30
#include "ezpHead.h"

void GetRGBPalette(int Color,unsigned char *Red,unsigned char *Green,
                   unsigned char *Blue)
{
  union REGS Reg1,Reg2;

  Reg1.w.ax=0x1015;
  Reg1.w.bx=Color;
  int86(0x10,&Reg1,&Reg2);
  *Red=Reg2.h.dh;
  *Green=Reg2.h.ch;
  *Blue=Reg2.h.cl;
}

#define MAXRED 255
#define MINRED 0
#define MAXGREEN 255
#define MINGREEN 0
#define MAXBLUE 255
#define MINBLUE 0

#define MAXDITHERCOLOR 16

#define MAXLINECOLUMN 4096

typedef struct tagERRCOLOR
{
   int red,green,blue;
}  ERRCOLOR;

typedef struct tagMYPIXEL
{
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} MYPIXEL;

// static variables used by:

static unsigned char huge * pSource24;
static unsigned char * pDest4;
static int hPic;
static int wPic;
static MYPIXEL palDither[MAXDITHERCOLOR];
static char palUsing[16]={ 1,1,1,1,
                           1,1,1,1,
                           1,1,1,1,
                           1,1,1,1
                         };

void GetDitherPal(void)
{
   int i;
   for (i=0;i<MAXDITHERCOLOR;i++)
   {
      GetRGBPalette(i,&(palDither[i].red),&(palDither[i]).green,&(palDither[i].blue));
      palDither[i].red<<=2;
      palDither[i].green<<=2;
      palDither[i].blue<<=2;
   }
}

void MyGetPixel24(MYPIXEL *pPixel,int i,int j)
{
   unsigned char huge * p;
   p=pSource24+((long)i*wPic+j)*3;
   pPixel->red=p[0];
   pPixel->green=p[1];
   pPixel->blue=p[2];
}

void MySetPixel24(MYPIXEL * pPixel,int i,int j)
{
   unsigned char huge * p;
   p=pSource24+((long)i*wPic+j)*3;
   p[0]=pPixel->red;
   p[1]=pPixel->green;
   p[2]=pPixel->blue;
}

void MySetPixel4(int iColor,int i,int j)
{
   unsigned char * p4;
   p4=pDest4+(wPic+1)/2*i+j/2;
   if (j&1)
   // odd cloumn pixel locates at low 4 bits
   {
      p4[0]|=iColor;
   }
   else
   // even column pixel locates at hight 4 bits
   {
      p4[0]|=(iColor<<4);
   }
}

int FindClosestColor(MYPIXEL * pPixel,ERRCOLOR *perrColor)
{
   int i,colorFind,ErrorRed,ErrorGreen,ErrorBlue;
   long ldisMin,ldisCur;


   ldisMin=2000000l;
   for (i=0;i<MAXDITHERCOLOR;i++)
   {
       if (palUsing[i]==0) continue;

       ErrorRed=(pPixel->red-palDither[i].red);
       ErrorGreen=(pPixel->green-palDither[i].green);
       ErrorBlue=(pPixel->blue-palDither[i].blue);

       ldisCur=(long)ErrorRed*(long)ErrorRed+
               (long)ErrorGreen*(long)ErrorGreen+
               (long)ErrorBlue*(long)ErrorBlue;
       /*  written By zjh , But not test 96.10.12
       {
        long R,G,B;
        R=abs(ErrorRed);
        G=abs(ErrorGreen);
        B=abs(ErrorBlue);
        ldisCur=(R*30+G*59+B*11);
       }
       */

       if (ldisCur<ldisMin)
       {
          colorFind=i;
          ldisMin=ldisCur;
       }
   }

   perrColor->red=pPixel->red-palDither[colorFind].red;
   perrColor->green=pPixel->green-palDither[colorFind].green;
   perrColor->blue=pPixel->blue-palDither[colorFind].blue;

   return colorFind;
}

#define BETWEENLIMIT(limit1,value,limit2) \
    ((value<limit1)                       \
     ? limit1                             \
     : (value>limit2)                     \
       ? (limit2)                         \
       : value )                          \

void DitherXY(int i,int j,ERRCOLOR * perrColor,int multi)
{
   MYPIXEL pixDither;
   int test;
   MyGetPixel24(&pixDither,i,j);

   test=pixDither.red+perrColor->red*multi/16;
   pixDither.red=BETWEENLIMIT(0,test,255);

   test=pixDither.green+perrColor->green*multi/16;
   pixDither.green=BETWEENLIMIT(0,test,255);

   test=pixDither.blue+perrColor->blue*multi/16;
   pixDither.blue=BETWEENLIMIT(0,test,255);

   MySetPixel24(&pixDither,i,j);
}

void DitherPixel(int i,int j,ERRCOLOR *perrColor)
{
   DitherXY(i+1,j,perrColor,7);
   DitherXY(i-1,j+1,perrColor,3);
   DitherXY(i,j+1,perrColor,5);
   DitherXY(i+1,j+1,perrColor,1);
}

void Dither24To4(unsigned char FHUGE *Image24Data,unsigned char *Image4Data,
                 int Width24,int Height24)
{
   int i,j,iColor;
   ERRCOLOR errColor;
   MYPIXEL pix24Cur;

// Set up local static variables.
// These varibles will be used as global variables by function
// MyGetPixel24(...),FindClosestColor(...),MySetPixel4(...),
// DitherPixel(...).
   wPic=Width24;
   hPic=Height24;
   pSource24=Image24Data;
   pDest4=Image4Data;

// Get dither palette
   GetDitherPal();

// Reset the destination data
   memset(Image4Data,0,(long)((Width24+1)/2)*(Height24));

// Do dither
   for (i=0;i<Height24-1;i++)
   {
      if (i&1)
      // odd line, we do dither from left to right
      {
         for (j=1;j<Width24-1;j++)
         {
            MyGetPixel24(&pix24Cur,i,j);
            iColor=FindClosestColor(&pix24Cur,&errColor);
            MySetPixel4(iColor,i,j);
            DitherPixel(i,j,&errColor);
         }
      }
      else
      // even line, we do dither from right to left.
      {
         for (j=Width24-2;j>=1;j--)
         {
            MyGetPixel24(&pix24Cur,i,j);
            iColor=FindClosestColor(&pix24Cur,&errColor);
            MySetPixel4(iColor,i,j);
            DitherPixel(i,j,&errColor);
         }
      }
   }
}
#endif     // UNUSED   // ByHance, 96,1.30
