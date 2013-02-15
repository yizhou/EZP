/*-------------------------------------------------------------------
* Name: bldchar.c       build char to rasts[]
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"
#include <assert.h>

/*--- these functions are called only when print ----*/
void BuildImage(char *pImage,int XPos,int YPos,
            int Width,int Height,int Slant,int RotateAngle,int Color)
{
   LONG ImgByteOff,ThisLine;
   int  ImgBytesPerLine,x1,x2,y1,y2;
   int  bit,oldx,y,OX,OY;
   LPDC lpdc=&SysDc;

   OX=x1=XPos;      OY=y2=YPos;
   YPos-=Height-1;  y1=YPos;
   x2=XPos+Width-1;
/*----------------
   if((RotateAngle%360)!=0)
   {
      SetDeviceColor(Color,0);
      return;
   }
 -----------------------*/

   if (!PrintingSign||!GlobalRorate90)
   {
   if ((x1>=lpdc->right)||(x2<lpdc->left)) return;
   if ((y1>=lpdc->bottom)||(y2<lpdc->top)) return;
   if (x1<lpdc->left) x1=lpdc->left;
   if (x2>=lpdc->right) x2=lpdc->right-1;
   if(y1<lpdc->top) y1=lpdc->top;
   if(y2>=lpdc->bottom) y2=lpdc->bottom-1;
   }

   Slant%=360;

   SetDeviceColor(Color,1);
   XPos=x1;
   ImgBytesPerLine=(Width+7)/8;
   ThisLine=(y1-YPos)*ImgBytesPerLine;
   for(y=y1;y<=y2;y++)
   {
       ImgByteOff = ThisLine;
       x1=XPos;
      /*---- print one line ----*/
       while(x1<=x2)
       {
          oldx=x1;
          bit = (x1-XPos) & 7;
          while( x1<=x2 && (dot1tab[bit]&pImage[ImgByteOff]) )
          {
              x1++;
              bit++;
              if(bit==8) { bit=0; ImgByteOff++; }
          }

          if(x1>oldx)
          {             // line(oldx,y,x1-1,y)
      #define SLANT_DEBUG
            #ifdef SLANT_DEBUG
             int sx=oldx,ex=x1-1,yy;

             if(Slant)
             {
                 PointSkew(&sx,&yy,sx,y,OX,OY,-Slant);
                 //PointSkew(&sx,&yy,sx,y,OX,OY,-Slant);
                 ex=sx+(x1-1-oldx);
             }

             printer->printScanLine(sx,ex,y,lpdc);
            #else
             printer->printScanLine(oldx,x1-1,y,lpdc);
            #endif // SLANT_DEBUG
          }
          x1++;
          if( ((x1-XPos)&7)==0 ) ImgByteOff++;
       }

       ThisLine+=ImgBytesPerLine;        // point to next line
   } /*--- y ---*/
}

void BuildChar16(int XPos,int YPos,Wchar Code,int CFont,
          int Width,int Height,int Slant,int RotateAngle,int Color)
{
   unsigned char *pImage;
   int ImageLen,ImageW,ImageH;
   unsigned char *CharBuffer;

   assert(CFont==0);

   ImageH=Height;
   ImageW=(Width+7)/8;
   ImageLen=ImageW*ImageH;
   pImage=(unsigned char *)malloc(ImageLen);
   //if(pImage==NULL)
   if(pImage<0x1000)
   {
       ReportMemoryError("bld16");
       return;
   }

   if(Width!=Height
   || GetCacheData(Code,CFont,Width,pImage,ImageLen)!=OpOK)
   {
      CharBuffer=GetChineseDot(Code);
      memset(pImage,0,ImageLen);        // clear it
      ZoomCharBuffer(CharBuffer,CHINESEWIDTH,CHINESEHIGHT,
          (float)Width/CHINESEWIDTH,(float)Height/CHINESEHIGHT,pImage,ImageW);

      if(Width==Height)
         PutCacheData(Code,CFont,Width,pImage,ImageLen);
   }

   BuildImage(pImage,XPos,YPos,Width,Height,Slant,RotateAngle,Color);
   free(pImage);
}

////Jerry 's 13x13 DOTLIB
#define DOT13LEN 202878L
char DOT13LIB[DOT13LEN];
void  InitDot13LIB()
{
   FILE *fp;

   fp = fopen("c:\\eZp\\fonts\\Hzk13","rb");
   if (fp == NULL) {
           GlobalUseLIB13 = 0;
           return;
   }
   if (DOT13LEN !=fread(DOT13LIB,1,DOT13LEN,fp)) {
       GlobalUseLIB13 = 0;
       fclose(fp);
       return;
   }
   GlobalUseLIB13 = 1;
   fclose(fp);
}

void BuildChar13(int XPos,int YPos,Wchar Code,int Color)
{
   unsigned char pImage[32];
   int qq,ww;

   qq = Code /256 - 0xa1;
   ww = Code % 256 - 0xa1;

   memset(pImage,0,32);        // clear it
   if (qq<9)
      memcpy(pImage,&DOT13LIB[(qq*94+ww)*26],26);
   else if (qq>=0x0f)
      memcpy(pImage,&DOT13LIB[((qq-4)*94+ww)*26],26);

   YPos-=13;
   copymono(pImage,XPos,YPos,16,13,Color);
}


void BuildCharVec(int XPos,int YPos,Wchar Code,int CFont,
                  int Width,int Height,int Slant,
                  int RotateAngle,int Color)
{
   BuildChar(XPos,YPos,Code,CFont+1,Width,Height,
             Slant,RotateAngle,Color,0,RITALICBIT|ROTATEBIT,0);
} /*- BuildCharVec -*/

void BuildCharTTF(int XPos,int YPos,Wchar Code,int CFont,
                  int Width,int Height,int Slant,
                  int RotateAngle,int Color)
{
   BuildChar(XPos,YPos,Code,CFont+1,Width,Height,
             Slant,RotateAngle,Color,0,RITALICBIT|ROTATEBIT,0);
} /*- BuildCharTTF -*/

void BuildChar24(int XPos,int YPos,Wchar Code,int CFont,
                  int Width,int Height,int Slant,
                  int RotateAngle,int Color)
{
   char str[32];
   FILE *fp;
   unsigned char *pImage;
   int ImageLen,ImageW,ImageH;
   char buffer[24*24/8];
   int qq,ww;

   //assert(Code>=0xb0a1 && CFont>=0 && CFont<4);
   ImageH=Height;
   ImageW=(Width+7)/8;
   ImageLen=ImageW*ImageH;
   pImage=(unsigned char *)malloc(ImageLen);
   //if(pImage==NULL)
   if(pImage<0x1000)
   {
      ReportMemoryError("bld24");
      goto lbl_disp_end;
   }

   if(Width!=Height
   || GetCacheData(Code,CFont,Width,pImage,ImageLen)!=OpOK)
   {
      strcpy (str,Dotlib24FileName);
      str[19] = SKHF_Name[CFont];

      fp = fopen(str,"rb");
      if (NULL==fp)
         goto lbl_disp_end;

      qq=(Code>>8)-0xb0;  ww=(Code&0xff)-0xa1;
      fseek(fp,72L*(qq*94+ww),SEEK_SET);
      qq=fread(buffer,1,72,fp);
      fclose(fp);
      if(qq!=72)
         goto lbl_disp_end;

      memset(pImage,0,ImageLen);        // clear it
      ZoomCharBuffer(buffer,24,24,
                (float)Width/24,(float)Height/24,pImage,ImageW);

      if(Width==Height)
         PutCacheData(Code,CFont,Width,pImage,ImageLen);
   }

   BuildImage(pImage,XPos,YPos,Width,Height,Slant,RotateAngle,Color);
 lbl_disp_end:
   free(pImage);
} /*- BuildChar24 -*/

void BuildSymbol24(int XPos,int YPos,Wchar Code,
                  int Width,int Height,int Slant,
                  int RotateAngle,int Color)
{
   FILE *fp;
   unsigned char *pImage;
   int ImageLen,ImageW,ImageH;
   char buffer[24*24/8];
   int qq,ww;

   //assert(Code<0xb0a1 && Code>0xa1a1);
   ImageH=Height;
   ImageW=(Width+7)/8;
   ImageLen=ImageW*ImageH;
   pImage=(unsigned char *)malloc(ImageLen);
   //if(pImage==NULL)
   if(pImage<0x1000)
   {
      ReportMemoryError("bldsymb24");
      return;
   }

   if(Width!=Height
   || GetCacheData(Code,SYMBOL_LIB,Width,pImage,ImageLen)!=OpOK)
   {
      fp = fopen(Symbol24FileName,"rb");
      if (NULL==fp)
        goto lbl_disp_end;

      qq=(Code>>8)-0xa1;  ww=(Code&0xff)-0xa1;
      fseek(fp,72L*(qq*94+ww),SEEK_SET);
      qq=fread(buffer,1,72,fp);
      fclose(fp);
      if(qq!=72)
         goto lbl_disp_end;

      memset(pImage,0,ImageLen);        // clear it
      ZoomCharBuffer(buffer,24,24,
                (float)Width/24,(float)Height/24,pImage,ImageW);

      if(Width==Height)
         PutCacheData(Code,SYMBOL_LIB,Width,pImage,ImageLen);
   }

   BuildImage(pImage,XPos,YPos,Width,Height,Slant,RotateAngle,Color);

 lbl_disp_end:
   free(pImage);
} /*- BuildSymbol24 -*/
