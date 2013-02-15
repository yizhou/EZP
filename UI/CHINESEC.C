/*-------------------------------------------------------------------
* Name: chinesec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"
#include <assert.h>

static unsigned char *ChineseLibBuffer=NULL;
static unsigned short ChineseLibSelector=0;
static unsigned char ASC32LibBuffer[MAXEFONT][MAXASC32CODE*ASC32SIZE];

enum { BUF, UCDOS, TWAY12, TWAY23 };
static char GetChineseDotMethod=0xff;

static void FreeDosMem(unsigned short selector)
{
  union REGS Reg;

  Reg.x.eax = 0x100;           /* DPMI free low_DOS_memory */
  Reg.x.edx = selector;
  int386 (0x31, &Reg, &Reg);
}

static int GetCCDOS(void)
{
  union REGS Reg;
  struct SREGS SReg;
  unsigned short *ptr;
  unsigned short off;

  off=*(unsigned short *)(0x79*4+2);
  ptr=(unsigned short *) ((unsigned long)off*16+0x104);
  if(*ptr==0x5054) {
     Reg.w.ax= 0;
     Reg.w.bx= 0x5054;
     int386(0x7f,&Reg,&Reg);
     if(Reg.w.cflag==0)
        return UCDOS;
  }

  Reg.w.ax= 0xdb10;
  int386(0x10,&Reg,&Reg);
  if(Reg.w.ax == 0xffff)              // TWAY maybe exist
  {
      Reg.x.eax = 0x100;                  /* DPMI allocate low_DOS_memory */
      Reg.x.ebx = (32+15)>>4;      // paragraph number :: 32 = 16*16/8
      int386 (0x31, &Reg, &Reg);

      if (Reg.w.cflag)
         return BUF;            // if error, use BUF to load Chinese Lib

      ChineseLibBuffer = (char *) ( (Reg.x.eax&0xffff) << 4);
      ChineseLibSelector=Reg.w.dx;

      segread(&SReg);
      Reg.w.ax= 0xdb21;
      Reg.w.bx= 0xb0a1;
      ChineseLibBuffer[0]=0xff;
      Reg.x.edi= FP_OFF (ChineseLibBuffer);
      SReg.es = 0;              // using low memory selector
      int386x(0x10,&Reg,&Reg,&SReg);
      if(Reg.w.ax == 0xffff && ChineseLibBuffer[0]!=0xff)
         return TWAY23;

      Reg.w.ax= 0xf101;
      Reg.w.bx= 0xb0a1;
      Reg.x.edi= FP_OFF (ChineseLibBuffer);
      SReg.es = 0;
      int386x(0x10,&Reg,&Reg,&SReg);
      if(Reg.w.ax == 0xffff && ChineseLibBuffer[0]!=0xff)
         return TWAY12;

      FreeDosMem(ChineseLibSelector);
  }
  return BUF;
}

/*------ get only Chinese Code's DotMatrix ----------*/
unsigned char *GetChineseDot(Wchar Code)
{
   unsigned int MidOff;
   unsigned char *TmpBuffer;
   union REGS Reg;
   struct SREGS SReg;

   switch(GetChineseDotMethod) {
     case BUF:
          //MidOff=(Code-0xa100)>>8;
          //if (MidOff>8) MidOff-=6;     /* Only For XSDOS !!! */
          MidOff=((Code>>8)-0xa1)*94+(Code&0xff)-0xa1;
          TmpBuffer=&ChineseLibBuffer[MidOff*CHINESESIZE];
          break;

     case UCDOS:
          Reg.w.dx=Code;
          int386(0x7f,&Reg,&Reg);
          TmpBuffer=(unsigned char *)((unsigned short)Reg.w.dx*16);
          break;

     case TWAY12:
          Reg.w.ax= 0xf101;
          goto tway_proc;
     case TWAY23:
          Reg.w.ax= 0xdb21;
       tway_proc:
          segread(&SReg);
          Reg.w.bx= Code;
          Reg.x.edi= FP_OFF (ChineseLibBuffer);
          SReg.es = 0;
          int386x(0x10,&Reg,&Reg,&SReg);
          TmpBuffer=ChineseLibBuffer;
          break;
   }    // end of switch

   return(TmpBuffer);
}

static int GetChineseStringBuffer(unsigned char *String,unsigned char *ChineseBuffer)
{
   unsigned char *TmpBuffer;
   Wchar Code;
   int i,j,StringLength,StringDotWidth,TmpStringDotWidth;

   StringLength=strlen(String);
   StringDotWidth=StringLength*(ASC16WIDTH>>3);
   for (i=0;i<StringLength;)
   {
       TmpStringDotWidth=i*(ASC16WIDTH>>3);
       if (String[i]>0xa0)
       {
          Code=((unsigned short)String[i]<<8)|String[i+1];
          TmpBuffer=GetChineseDot(Code);

          for (j=0;j<CHINESEHIGHT*(CHINESEWIDTH>>3);j+=CHINESEWIDTH>>3)
          {
              //memcpy(&ChineseBuffer[TmpStringDotWidth], &TmpBuffer[j], CHINESEWIDTH>>3);
              Code=*(USHORT *)&TmpBuffer[j];
              *((USHORT *)&ChineseBuffer[TmpStringDotWidth])=Code;
              TmpStringDotWidth+=StringDotWidth;
          }
          i+=2;
       }
       else
       {
          unsigned long pChineseLibOffset;
          pChineseLibOffset=(unsigned long)String[i]*ASC16SIZE;
          TmpBuffer=&ASC16LibBuffer[pChineseLibOffset];

          for (j=0;j<ASC16HIGHT*(ASC16WIDTH>>3);/*j+=ASC16WIDTH>>3*/)
          {
              //memcpy(&ChineseBuffer[TmpStringDotWidth],&TmpBuffer[j],ASC16WIDTH>>3);
              ChineseBuffer[TmpStringDotWidth]=TmpBuffer[j++];
              TmpStringDotWidth+=StringDotWidth;
          }
          i++;
       }
   }
   return(StringLength);
}

static void PutChineseStringBuffer(unsigned char *ChineseBuffer,int StrLength,
                            int X,int Y,int Color,int BkColor)
{
   int  BufLen;

   if (BkColor==-1)
      BkColor=getbkcolor();

   BufLen=ASC16WIDTH*StrLength;
   setfillstyle(SOLID_FILL,BkColor);
   bar(X,Y,X+BufLen,Y+ASC16HIGHT);        // clear area
   copymono(ChineseBuffer,X,Y,BufLen,ASC16HIGHT,Color);
}

void DisplayString(unsigned char *String,int XPos,int YPos,int Color,int BkColor)
{
   struct viewporttype ViewInformation;
   unsigned char ChineseMatrixBuffer[400*ASC16HIGHT*(ASC16WIDTH>>3)];
   int Length;

   getviewsettings(&ViewInformation);
   MouseHidden();
   setviewport(0,0,getmaxx(),getmaxy(),1);

   Length=GetChineseStringBuffer(String,ChineseMatrixBuffer);
   if (Length>0)
      PutChineseStringBuffer(ChineseMatrixBuffer,Length,XPos,YPos,Color,BkColor);

   setviewport(ViewInformation.left,ViewInformation.top,
               ViewInformation.right,ViewInformation.bottom,
               ViewInformation.clip);
   MouseShow();
}

void ViewportDisplayString(unsigned char *String,int XPos,int YPos,int Color,int BkColor)
{
   unsigned char ChineseMatrixBuffer[400*ASC16HIGHT*(ASC16WIDTH>>3)];
   int Length;

   //MouseHidden();

   Length=GetChineseStringBuffer(String,ChineseMatrixBuffer);
   if (Length>0)
      PutChineseStringBuffer(ChineseMatrixBuffer,Length,XPos,YPos,Color,BkColor);

   //MouseShow();
}

int ChineseLibInitial(void)
{
  long Length;
  char FileName[80];
  int  i;
  FILE *fpChineseLib;

  ASC16LibBuffer=malloc(MAXASC16CODE*ASC16SIZE);
  if (ASC16LibBuffer==NULL)
     return(OUTOFMEMORY);

  strcpy(FileName,ASC16LIBNAME);
  if ((fpChineseLib=fopen(FileName,"rb"))==NULL)
  {
  open_err:
     strupr(FileName);
     printf("Can't find file<%s>.\n",FileName);
     getch();
     return(-1);
  }
  fread(ASC16LibBuffer,1,MAXASC16CODE*ASC16SIZE,fpChineseLib);

  if(!fEditor)
  {
       fseek(fpChineseLib,4096,0);
       fread(BmpBuf,1,BmpBufLen,fpChineseLib);
       fclose(fpChineseLib);

       strcpy(FileName,ASC32LIBNAME);
       if ((fpChineseLib=fopen(FileName,"rb"))==NULL)
           goto open_err;
       //fseek(fpChineseLib,0,0);     // skip head
       for(i=0;i<MAXEFONT;i++)
       {
          fseek(fpChineseLib,32,SEEK_CUR);     // skip head
          fread(ASC32LibBuffer[i],1,MAXASC32CODE*ASC32SIZE,fpChineseLib);
          fread(ASC32AW[i],sizeof(short),MAXASC32CODE,fpChineseLib);
          fseek(fpChineseLib,sizeof(short)*MAXASC32CODE,1);     // skip lsb
       }
  }
  fclose(fpChineseLib);

  GetChineseDotMethod=GetCCDOS();
  if(GetChineseDotMethod==BUF) {
     Length=CHINESELIBLENGTH;
     ChineseLibBuffer=(unsigned char*)malloc(Length);
     if(ChineseLibBuffer==NULL) {
         printf("I need more memory.\n");
         exit(-3);
     }

     strcpy(FileName,CHINESELIBNAME);
     if ((fpChineseLib=fopen(FileName,"rb"))==NULL)
         goto open_err;

     fread(ChineseLibBuffer,1,Length,fpChineseLib);
     fclose(fpChineseLib);
  }
  ReturnOK();
}

int ChineseLibDone(void)
{
  if (ASC16LibBuffer)
  {
     free(ASC16LibBuffer);
  }

  if(GetChineseDotMethod==BUF)
     free(ChineseLibBuffer);

  if(ChineseLibSelector)                // for TWAY system
      FreeDosMem(ChineseLibSelector);

  ReturnOK();
}

void ViewportDisplaySmallChar(int XPos,int YPos,int Width,int Height,int Color)
{                    // too small, use rectangle to display
   char old_mask[8];
   static char new_mask[8]={ 0x99,0x66,0x66,0x99,0x99,0x66,0x66,0x99 };

   setcolor(Color);
   _getfillmask(old_mask);
   _setfillmask(new_mask);
   bar(XPos,YPos-Height,XPos+Width,YPos);
   _setfillmask(old_mask);
   return;
}

static void ViewportDisplayRotateImage(char *pImage,
                 int XPos,int YPos,int ImageW,int ImageH,
                 int SlantAngle,int RotateAngle,int Color)
{
   unsigned char Tmp;
   int i,k;
   int x,y,bit;

   setcolor(Color);
   for(i=0;i<ImageH;i++)
   {
       for(k=0;k<ImageW;k++)
       {
          Tmp=*pImage++;
          if(Tmp==0) continue;
          for(bit=0;bit<8;bit++)
          {
             if( (Tmp&dot1tab[bit])==0 ) continue;
             x=XPos+k*8+bit;  y=YPos+i;
             if (SlantAngle)
                PointSkew(&x,&y,x,y,XPos,YPos+ImageH-1,-SlantAngle);
             if (RotateAngle)
                RotatePoint(&x,&y,x,y,XPos,YPos+ImageH-1,RotateAngle);
             putpixel(x,y,Color);
          }
       } //-- k --
   }    //---- i ------
} /*- ViewportDisplayRotateImage -*/

//------ Zoom char_buffer, put result dot_image to pImage ------
// dotlib_w(h):: CharDotOriginSize, BytesPerLine:: ImageWidth
//---------------------------------------------------------------
void ZoomCharBuffer(unsigned char *LibBuffer,int dotlib_w,int dotlib_h,
        float ZoomW,float ZoomH,unsigned char *pImage,int BytesPerLine)
{
   #define CODESIZE (24*24/8+24)       //By zjh
   unsigned char dot_buf[CODESIZE];
   unsigned char *CharBuffer;
   int i,k;
   int ZoomWInt,ZoomWFract, ZoomHInt,ZoomHFract;
   int CopyWInt,CopyWFract, CopyHInt,CopyHFract;
   int DotlibBytesPerLine;
   int offset,ThisLine;
   //char char_start;
   unsigned char Tmp;
   unsigned char bit;      //,KeepLastByte;

   DotlibBytesPerLine=(dotlib_w+7)/8;

   if(ZoomW==1.0 && ZoomH==1.0)
   {
      memcpy(pImage,LibBuffer,DotlibBytesPerLine*dotlib_h);
      return;
   }

   memcpy(dot_buf,LibBuffer,CODESIZE-24);         // keep a image   zjh -24
   CharBuffer=&dot_buf[0];          // do with this image

   ZoomWInt=ZoomW;
   ZoomWFract=(ZoomW-ZoomWInt)*1024;
   ZoomHInt=ZoomH;
   ZoomHFract=(ZoomH-ZoomHInt)*1024;

   //char_start=XPos&0x7;
   //offset=XPos/8;            // point to Image Buffer start
   //char_start=KeepLastByte=
   offset=0;

   CopyHInt=0;          // repeat lines number
   CopyHFract=0;     // 512;       // 0.5

 //---------- Zoom the dot matrix to an Image Buf ----------
   for(i=0;i<dotlib_h;i++)
   {
      CopyHInt+=ZoomHInt;
      if( (CopyHFract+=ZoomHFract)>=1024 )
           { CopyHInt++; CopyHFract-=1024; }

      if(!CopyHInt) {         // this line "or" to next line
         for(k=0;k<DotlibBytesPerLine;k++)
         {
            CharBuffer[DotlibBytesPerLine]|=CharBuffer[0];
            CharBuffer++;    // point to next char
         }
      }
      else
      {                 // at least 1 line
         char rol_n;

         //if(char_start) KeepLastByte=pImage[offset];
         pImage[offset]=0;

         ThisLine=offset;             // keep this line pointer
         //-- process 1 line --
         CopyWInt=0;          // repeat columns number
         CopyWFract=0;       // 512;       // 0.5

         rol_n=8;         //8-char_start;
         bit=8;           // 8 bits per byte
         for(k=0;k<dotlib_w;k++)
         {
            CopyWInt+=ZoomWInt;
            if( (CopyWFract+=ZoomWFract)>=1024 )
                { CopyWInt++; CopyWFract-=1024; }

            if(!CopyWInt) {
                Tmp=*CharBuffer;
                (*CharBuffer)<<=1;
                if(Tmp&0x80)        // this column "or" to next column
                    (*CharBuffer)|=0x80;
            }
            else
            {     // copy at least 1 column
                Tmp=*CharBuffer;
                (*CharBuffer)<<=1;
                if(Tmp&0x80)    // copy this column
                {
                    do {
                       pImage[offset]=(pImage[offset]<<1)|1;
                       if(--rol_n==0) {
                           rol_n=8;
                           offset++;  //pImage[offset++]|=KeepLastByte;
                                      //KeepLastByte=0;
                       }
                    } while(--CopyWInt);
                }
                else
                {               // clear this column
                    do {
                       pImage[offset]<<=1;
                       if(--rol_n==0) {
                           rol_n=8;
                           offset++;   //pImage[offset++]|=KeepLastByte;
                                       //KeepLastByte=0;
                       }
                    } while(--CopyWInt);
                }
            }   //-- if CopyWInt --

            if(--bit==0) { bit=8; CharBuffer++; }
         }  //-- k --

         pImage[offset]<<=rol_n;        // the last byte
                 // pImage[offset]|=KeepLastByte;
         offset=ThisLine+BytesPerLine;  // point to next line
         if(bit!=8) CharBuffer++;

         //-- now, process repeat line
         while(--CopyHInt)      // (CopyH-1) repeat lines
         {
            memcpy(&pImage[offset],&pImage[ThisLine],BytesPerLine);
            offset+=BytesPerLine;       // point to next line
         }
      }  //-- if CopyHInt --
   }    //-- i --
} /*- ZoomCharBuffer -*/

#ifdef OLD_VERSION
void ViewportDisplayChar(int XPos,int YPos,Wchar Code,
                        int Width,int Height,int SlantAngle,
                        int RotateAngle,int Color)
{
   unsigned char *pImage;
   int ImageLen,ImageW,ImageH;
   unsigned char *CharBuffer;
   int dotlib_w,dotlib_h;

   ImageH=Height;
   ImageW=(Width+7)/8;

   ImageLen=ImageW*ImageH;
   pImage=(unsigned char *)malloc(ImageLen);
   if(pImage==NULL)
        return;
   memset(pImage,0,ImageLen);        // clear it

   if(Code<0xa0)       // english char
   {
       int w,h;
       if(Code<0x21||Code>0x7e)
          return;

       if(fEditor)
       {
           CharBuffer=&ASC16LibBuffer[(long)Code*ASC16SIZE];
           dotlib_w=ASC16WIDTH;
           dotlib_h=h=ASC16HIGHT;
           w=2*ASC16WIDTH;
       }
       else
       {
           CharBuffer=&ASC32LibBuffer[NowEFont][(Code-0x21)*ASC32SIZE];
           dotlib_w=w=ASC32WIDTH;
           dotlib_h=h=ASC32HIGHT;
       }

       ZoomCharBuffer(CharBuffer,dotlib_w,dotlib_h,
                  (float)Width/w,(float)Height/h,pImage,ImageW);
   }
   else         // Chinese char
   {
       CharBuffer=GetChineseDot(Code);
       ZoomCharBuffer(CharBuffer,CHINESEWIDTH,CHINESEHIGHT,
           (float)Width/CHINESEWIDTH,(float)Height/CHINESEHIGHT,pImage,ImageW);
   }

 //--------- now, draw the image ---------------------------
   YPos-=Height-1;                // (X,Y)=(left,down)
   if((RotateAngle%360) || (SlantAngle%360) )
      ViewportDisplayRotateImage(pImage,XPos,YPos,ImageW,ImageH,
                    SlantAngle,RotateAngle,Color);
   else
      copymono(pImage,XPos,YPos,ImageW<<3,ImageH,Color);

   free(pImage);
} /*- ViewportDisplayChar -*/
#endif

void ViewportDisplayCharVec(int XPos,int YPos,Wchar Code,int CFont,
                        int Width,int Height,int SlantAngle,
                        int RotateAngle,int Color)
{
//    if(CFont==0)
//        ViewportDisplayChar16(XPos,YPos,Code,0,Width,Height,SlantAngle,
//                     RotateAngle,Color);
//    else
        BuildChar(XPos,YPos,Code,CFont+1,Width,Height,
                  SlantAngle,RotateAngle,Color,0,RITALICBIT|ROTATEBIT,0);
} /*- ViewportDisplayCharVec -*/

void ViewportDisplayCharTTF(int XPos,int YPos,Wchar Code,int CFont,
                        int Width,int Height,int SlantAngle,
                        int RotateAngle,int Color)
{
    if(CFont==0)
        ViewportDisplayChar16(XPos,YPos,Code,0,Width,Height,SlantAngle,
                     RotateAngle,Color);
    else
        BuildChar(XPos,YPos,Code,CFont+1,Width,Height,
                  SlantAngle,RotateAngle,Color,0,RITALICBIT|ROTATEBIT,0);
} /*- ViewportDisplayCharTTF -*/

void ViewportDisplayChar24(int XPos,int YPos,Wchar Code,int CFont,
                        int Width,int Height,int SlantAngle,
                        int RotateAngle,int Color)
{
   char str[32];
   FILE *fp;
   unsigned char *pImage;
   int ImageLen,ImageW,ImageH;
   char buffer[24*24/8];
   int qq,ww;

   assert(Code>=0xb0a1 && CFont>=0 && CFont<4);

   ImageH=Height;
   ImageW=(Width+7)>>3;
   ImageLen=ImageW*ImageH;
   pImage=(unsigned char *)malloc(ImageLen);
   //if(pImage==NULL)
   if(pImage<0x1000)
   {
      ReportMemoryError("disp24");
      return;
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

 //--------- now, draw the image ---------------------------
   YPos-=Height-1;                // (X,Y)=(left,down)
   if((RotateAngle%360) || (SlantAngle%360) )
      ViewportDisplayRotateImage(pImage,XPos,YPos,ImageW,ImageH,
                    SlantAngle,RotateAngle,Color);
   else
      copymono(pImage,XPos,YPos,ImageW<<3,ImageH,Color);

 lbl_disp_end:
   free(pImage);
} /*- ViewportDisplayChar24 -*/

void ViewportDisplaySymbol24(int XPos,int YPos,Wchar Code,
                        int Width,int Height,int SlantAngle,
                        int RotateAngle,int Color)
{
   FILE *fp;
   unsigned char *pImage;
   int ImageLen,ImageW,ImageH;
   char buffer[24*24/8];
   int qq,ww;

   assert(Code<0xb0a1 && Code>0xa1a1);
   ImageH=Height;
   ImageW=(Width+7)>>3;
   ImageLen=ImageW*ImageH;
   pImage=(unsigned char *)malloc(ImageLen);
   //if(pImage==NULL)
   if(pImage<0x1000)
   {
      ReportMemoryError("dispsmb24");
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

 //--------- now, draw the image ---------------------------
   YPos-=Height-1;                // (X,Y)=(left,down)
   if((RotateAngle%360) || (SlantAngle%360) )
      ViewportDisplayRotateImage(pImage,XPos,YPos,ImageW,ImageH,
                    SlantAngle,RotateAngle,Color);
   else
      copymono(pImage,XPos,YPos,ImageW<<3,ImageH,Color);

 lbl_disp_end:
   free(pImage);
} /*- ViewportDisplaySymbol24 -*/

void ViewportDisplayChar16(int XPos,int YPos,Wchar Code,int CFont,
                        int Width,int Height,int SlantAngle,
                        int RotateAngle,int Color)
{
   // ViewportDisplayChar(XPos,YPos,Code,Width,Height,SlantAngle,
     //      RotateAngle,Color);

   unsigned char *pImage;
   int ImageLen,ImageW,ImageH;
   unsigned char *CharBuffer;

   assert(CFont==0);

   ImageH=Height;
   ImageW=(Width+7)>>3;
   ImageLen=ImageW*ImageH;
   pImage=(unsigned char *)malloc(ImageLen);
   //if(pImage==NULL)
   if(pImage<0x1000)
   {
      ReportMemoryError("disp16");
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

 //--------- now, draw the image ---------------------------
   YPos-=Height-1;                // (X,Y)=(left,down)
   if((RotateAngle%360) || (SlantAngle%360) )
      ViewportDisplayRotateImage(pImage,XPos,YPos,ImageW,ImageH,
                    SlantAngle,RotateAngle,Color);
   else
      copymono(pImage,XPos,YPos,ImageW<<3,ImageH,Color);

   free(pImage);
} /*- ViewportDisplayChar16 -*/

void ViewportDisplayEChar(int XPos,int YPos,Wchar Code,int EFont,
                        int Width,int Height,int SlantAngle,
                        int RotateAngle,int Color)
{
   unsigned char *pImage;
   int ImageLen,ImageW,ImageH;
   unsigned char *CharBuffer;
   int dotlib_w,dotlib_h;
   int w,h;

   if(Code<0x21||Code>0x7e)
      return;

 //--------- now, draw the image ---------------------------
   ImageH=Height;
   if(fEditor) Width/=2;

   ImageW=(Width+7)>>3;
   ImageLen=ImageW*ImageH;
   pImage=(unsigned char *)malloc(ImageLen);
   //if(pImage==NULL)
   if(pImage<0x1000)
   {
      ReportMemoryError("dispee");
      return;
   }

   if(Width!=Height
   || GetCacheData(Code,EFont,Width,pImage,ImageLen)!=OpOK)
   {
      if(fEditor)
      {
          CharBuffer=&ASC16LibBuffer[(long)Code*ASC16SIZE];
          dotlib_w=w=ASC16WIDTH;
          dotlib_h=h=ASC16HIGHT;
          //w=2*ASC16WIDTH;
      }
      else
      {
          CharBuffer=&ASC32LibBuffer[EFont][(Code-0x21)*ASC32SIZE];
          dotlib_w=w=ASC32WIDTH;
          dotlib_h=h=ASC32HIGHT;
      }

      memset(pImage,0,ImageLen);        // clear it
      ZoomCharBuffer(CharBuffer,dotlib_w,dotlib_h,
                 (float)Width/w,(float)Height/h,pImage,ImageW);

      if(Width==Height)
         PutCacheData(Code,EFont,Width,pImage,ImageLen);
   }

 //--------- now, draw the image ---------------------------
   YPos-=Height-1;                // (X,Y)=(left,down)
   if((RotateAngle%360) || (SlantAngle%360) )
      ViewportDisplayRotateImage(pImage,XPos,YPos,ImageW,ImageH,
                    SlantAngle,RotateAngle,Color);
   else
      copymono(pImage,XPos,YPos,ImageW<<3,ImageH,Color);

   free(pImage);
} /*- ViewportDisplayEChar -*/
