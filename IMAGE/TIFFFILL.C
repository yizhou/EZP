/*-------------------------------------------------------------------
* Name: tifffill.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"
#include <assert.h>

#define PICSCALE()
/*
                    {                       \
                      if (!PrintingSign)    \
                      {                     \
                        XRate *= SCRX;      \
                        YRate *= SCRY;      \
                      }                     \
                      else                  \
                      {                     \
                        XRate *= PRNX;      \
                        YRate *= PRNY;      \
                      };                    \
                    }
*/
#define SCRDPI screendpi

static int Scrx[]={320,640,800,1024,1280};
static int Scry[]={200,480,600,768,1024};
int IsScreenPic(int x,int y)
{
   int i,j;
   j=sizeof(Scrx)/sizeof(int);
   for (i=0;i<j;i++) if (x==Scrx[i]&&y==Scry[i]) return 1;
   if ((x==720||x==72||x==300||x==600||x==1200||x==2400||x==180||x==360||x==150)
        &&(x==y||x+x==y||y+y==x)) return 0;
   if (x!=y) return 1;
   return 0;
}

GetTiffCompreeLine(FILE *fp,char *buff,UINT myBytesPerRow);

static int deltaY;

/*------ for picture compression method --------*/
  #define NoPackByteCOMP        0
  #define RunLenPackCOMP        1
  #define HuffCOMP              2
  #define LZW_COMP              5

  #define NoPackWordCOMP        0x10
  #define MacPainRunLenPackCOMP 0x11

#define SOURCEFORMAT -114
#define SOURCECOLORS -112
#define SOURCECOMPRESS -113


static unsigned short GetWord(FILE *fp)
{
   int c=fgetc(fp);
   return( c|(fgetc(fp)<<8) );
}

#define BufLen     (3*8192)         // 24k

static int GetDitherColor(long NowColor,int X,int Y);

static unsigned long ClearWhiteColor;
static void FindClearWhiteColor(ImageDescribes *TiffPresent,UCHAR BitsPerPixel)
{
    int i,total;
    if(BitsPerPixel==24)
    {
        ClearWhiteColor=0xffffff;
        return;
    }

    total=1<<BitsPerPixel;
    ClearWhiteColor=total-1;

    total*=3;
    if(TiffPresent->RGBPaletteSign)
       for(i=0;i<total;i+=3)
          if(TiffPresent->ByteRGBPalette[i]==0xff
           &&TiffPresent->ByteRGBPalette[i+1]==0xff
           &&TiffPresent->ByteRGBPalette[i+2]==0xff)
           {  ClearWhiteColor=i/3; break; }
}

static void  ColorBlock(unsigned char FHUGE *pBuf,UINT x1,UINT y1,UINT x2,UINT y2,\
         ImageDescribes *TiffPresent,char PutSign,ULONG Color,UCHAR BitsPerPixel)
{
    UINT midx,midy;
    UINT R,G,B;
    UINT WidthBits=TiffPresent->ImageWidth;
    UINT byte1=x1>>3, byte2=x2>>3;
    unsigned char FHUGE *TmpP;
    long Tmpaddr;
    int  TmpColor;

    //return;             // for test
    //////////////////////////GGGGGGGGGGGGGGGGG///////////////
    for(midy=y1;midy<=y2;midy++)
        if (PutSign==PUTTRUECOLORDEVICE)
        {
           switch (BitsPerPixel)
           {
             case 1:
                  if (!Color)        // 0==white,   1=black
                     return;

                  ///////////////zjh Debug ///////////////
                  /*
                  {
                  char *mp=(char*)0xa0000;
                  int addr=midy*80;
                  mp=mp+addr;
                  if ( byte1==byte2)
                     *mp|=headdot[(x1&7)]&taildot[(x2&7)];
                  else           // byte1<byte2
                  {
                     *(mp++)|=headdot[(x1&7)];
                     for (midx=byte1+1; midx<byte2; midx++)
                     {
                         *(mp++)=0xff;
                     }
                     *mp|=taildot[(x2&7)];
                  }

                  }
                  */
                  //////////////////////////////////

                  Tmpaddr=(long)((WidthBits+7)>>3)*midy+byte1;
                  TmpP=&pBuf[Tmpaddr];

                  if ( byte1==byte2)
                     *TmpP|=headdot[(x1&7)]&taildot[(x2&7)];
                     // *TmpP|=BitFill[7-(x1&7)]&(~BitFill[7-(x2&7)]);
                  else           /* byte1<byte2 */
                  {
                     *(TmpP++)|=headdot[(x1&7)];
                     for (midx=byte1+1; midx<byte2; midx++)
                     {
                         *(TmpP++)=0xff;
                     }
                     *TmpP|=taildot[(x2&7)];
                  }
                  break;
             case 4:
                  Tmpaddr=(long)((WidthBits+1)>>1)*midy+(x1>>1);
                  TmpP=&pBuf[Tmpaddr];
                  TmpColor=Color&0xf;
                  for (midx=x1;midx<=x2;midx++)
                  {
                      if (midx&0x1)
                      {
                         *TmpP|=TmpColor;
                         TmpP++;
                      }
                      else
                         *TmpP|=(TmpColor<<4);
                  }
                  break;
             case 8:
                  Tmpaddr=(long)midy*TiffPresent->ImageWidth+x1;
                  TmpP=&pBuf[Tmpaddr];
                  memset(TmpP,Color&0xff,x2-x1+1);
                  break;
             case 24:
                  Tmpaddr=3L*(midy*TiffPresent->ImageWidth+x1);
                  TmpP=&pBuf[Tmpaddr];
                  R=(Color>>16)&0xff;
                  G=(Color>>8)&0xff;
                  B=Color&0xff;
                  for (midx=x1;midx<=x2;midx++)
                  {
                      *(TmpP++)=R;
                      *(TmpP++)=G;
                      *(TmpP++)=B;
                  }
                  break;
           }
        }
        else
        {             // PUTDITHERCOLOR
           Tmpaddr=(long)((WidthBits+1)>>1)*midy+(x1>>1);
           TmpP=&pBuf[Tmpaddr];

           if (BitsPerPixel==1)
           {
                if (Color)       // 1=Black, same as default_color
                    return;
                // TmpColor=15;
                // R=(TmpColor<<4);

                //setviewport(0,0,639,479,0);  //
                //setcolor(12);                //
                for (midx=x1;midx<=x2;midx++)
                {
                  if (midx&0x1)
                     (*TmpP++) |= 0xf;       // TmpColor;
                  else  *TmpP |= 0xf0;         // R

                  //line(midx,midy,midx,midy);    //Test
                }
                //break;                         //BUG found By zjh 96.10.12
                continue;
           }

           if(BitsPerPixel<8 && !TiffPresent->RGBPaletteSign)
                  Color*=(1<<BitsPerPixel);       // change to [0..255]

           for (midx=x1;midx<=x2;midx++)
           {
               TmpColor=GetDitherColor(Color,midx,midy);

               #ifdef UNUSE
               {
                setviewport(0,0,639,479,0);  //
                setcolor(TmpColor);          //
                line(midx,midy,midx,midy);
                /*
                if (TmpColor==0)
                if (getch()==27)
                  {
                   printf("a");
                  };
                  */
               }
               #endif

               //if (TmpColor==0) continue;      //delete By zjh Because buffer is not set to zero

               if(!TiffPresent->RGBPaletteSign) {
                    if(TmpColor) TmpColor=EGA_WHITE;
               }

               TmpColor &= 0xf;           //add By zjh 10.15;

               if (midx&0x1) (*TmpP++) |= TmpColor;
               else  *TmpP |= (TmpColor<<4);

           }
        }
}  /* ColorBlock */

static void  ClearBlockTail(unsigned char FHUGE *pBuf,UINT x1,UINT y1,
                            UINT x2,UINT y2,
                 ImageDescribes *TiffPresent,char PutSign,UCHAR BitsPerPixel)
{
    UINT midx,midy;
    // UINT R,G,B;
    UINT WidthBits=(TiffPresent->ImageWidth+1)>>1;
    unsigned char FHUGE *TmpP;
    long Tmpaddr;

    //return;             // for test

    if(PutSign==PUTDITHERDEVICE)
    {
        Tmpaddr=(long)WidthBits*y1+(x1>>1);
        for(midy=y1;midy<=y2;midy++)
        {
           TmpP=&pBuf[Tmpaddr];

               // Use EGA_WHITE (15) color
           for (midx=x1;midx<=x2;midx++)
             if (midx&1)
                (*TmpP++) |= 0xf;
             else  *TmpP |= 0xf0;

           Tmpaddr+=WidthBits;
        } //-- midy --
    }
    else
    {        // (PutSign==PUTTRUECOLORDEVICE)
       if(BitsPerPixel>1)    // if ==1, default is white, so, needn't clear
        ColorBlock(pBuf,x1,y1,x2,y2,TiffPresent,PUTTRUECOLORDEVICE,
                     ClearWhiteColor,BitsPerPixel);
    } /* end if putsign */
}

static void GetOneLine(FILE *fp,ULONG *ImageBuf,UINT Flag_BitsPerPixel,\
       UINT BytesPerLine,char PutSign,ImageDescribes *TiffPresent,
       UINT CompressMethod)
{
   UCHAR  MyBuf[BufLen];
   UINT   Col,n;
   UCHAR  *buf;
   int    value,c,i;
   int    TotalCols;


   if(CompressMethod&0x80) {   /* for BMP */
       switch(CompressMethod&0x7f) {
           case NoPackByteCOMP:
              fread(MyBuf,BytesPerLine,1,fp);

          #ifdef NOT_CORRECT            // ByHance,97,5.6
              if ((Flag_BitsPerPixel&0x7f)==24)
              for (i=0;i<BytesPerLine;i=i+3)
                {
                  ch=MyBuf[i];
                  MyBuf[i]=MyBuf[i+2];
                  MyBuf[i+2]=ch;
                }
          #endif

              fseek(fp,0L-2*BytesPerLine,1);     /* SEEK_CUR */
              break;
           default: goto NotSupported;
        } /* method */
   } else

   if(CompressMethod&0x40) {   /* for 4 planes, PCX */
       switch(CompressMethod&0x3f) {
           case RunLenPackCOMP:
                buf=(UCHAR *)ImageBuf;
                n=0;  TotalCols=BytesPerLine*4;
                do {
                    c=fgetc(fp) & 0xff;
                    if ((UCHAR)c>0xc0) {   /* repeat */
                          i=c&0x3f;
                          c=fgetc(fp) & 0xff;
                          while(i--) buf[n++]=c;
                    } else buf[n++]=c;
                } while (n<TotalCols);

                /*------- change 4 planes to 1 plane --------*/
                n=0;
                for(i=0;i<BytesPerLine;i++) {
                    unsigned char c1,c2,c3,c4,c;
                    c1=buf[i];
                    c2=buf[BytesPerLine+i];
                    c3=buf[2*BytesPerLine+i];
                    c4=buf[3*BytesPerLine+i];
                    for ( c=0; c<4; c++) {
                        value = 0;
                        /* If the most significant bit is set... */
                        /* Set the appropriate bit in the higher order value */
                        if (c1 & 0x80) value |= 0x10;
                        if (c2 & 0x80) value |= 0x20;
                        if (c3 & 0x80) value |= 0x40;
                        if (c4 & 0x80) value |= 0x80;
                        c1<<=1; c2<<=1; c3<<=1; c4<<=1;

                        /* Repeat for the lower order value */
                        if (c1 & 0x80) value |= 0x01;
                        if (c2 & 0x80) value |= 0x02;
                        if (c3 & 0x80) value |= 0x04;
                        if (c4 & 0x80) value |= 0x08;
                        if(c!=3) { c1<<=1; c2<<=1; c3<<=1; c4<<=1; }
                        MyBuf[n++]=value;
                    }
                }
                break;
           default: goto NotSupported;
        }
   } else    /* otherwise, for PCX, TIFF */

   switch(CompressMethod) {
       case LZW_COMP:
            GetTiffCompreeLine(fp,MyBuf,BytesPerLine);
            break;
       case NoPackByteCOMP:
       case NoPackWordCOMP:
            fread(MyBuf,BytesPerLine,1,fp);
            if ( (CompressMethod==NoPackWordCOMP)
               && (BytesPerLine%2) )  /* must start at word_bianJie */
                    fgetc(fp);
            break;
       case RunLenPackCOMP:
            n=0;
            do {
                c=fgetc(fp) & 0xff;
                if ((UCHAR)c>0xc0) {   /* repeat */
                      i=c&0x3f;
                      c=fgetc(fp) & 0xff;
                      while(i--) MyBuf[n++]=c;
                } else MyBuf[n++]=c;

            } while (n<BytesPerLine);
            break;
       case MacPainRunLenPackCOMP:
            n=0;
            do {
                c=fgetc(fp) & 0xff;
                if ((UCHAR)c>=0x80) {   /* repeat */
                      i=( (~c)&0xff ) + 2;
                      c=fgetc(fp);
                      while(i--) MyBuf[n++]=c;
                } else {      /* get i bytes from file */
                      i=c+1;
                      while(i--) MyBuf[n++]=fgetc(fp);
                }
            } while (n<BytesPerLine);
            break;
       default:
       NotSupported:
            memset(MyBuf,0,BytesPerLine);
            break;
   } /* switch */

   if(Flag_BitsPerPixel & 0x8000)  { /*-- for TiffPhotemet ---*/
     for(Col=0;Col<BytesPerLine;Col++) MyBuf[Col]=~MyBuf[Col];
   }

/*------------ case BitsPerPixel, translate MyBuf to ImageBuf:
       ImageBuf[i]= Color of Per Pixel
               or = gray of pixel,if TrueColor(24BitsPerPixel)
  -------------------------------------------------------------*/

  switch (Flag_BitsPerPixel&0xff) {

    #define BLANK_FILL   0xffffffff       //By zjh 96.10.11
    case 1:      /* 1 bit per color */
         memset(ImageBuf,0,BytesPerLine*8*sizeof(ULONG));
         for(Col=0;Col<BytesPerLine;Col++) {
            value=MyBuf[Col];
            if(!value) {
               ImageBuf+=8;
               continue;
            } else
            if(value==0xff) {
               for(i=0;i<8;i++)
                 *ImageBuf++=BLANK_FILL;        //By zjh 96.10.11
               continue;
            }

            if( value & 0x80 ) *ImageBuf=BLANK_FILL;        //By zjh 96.10.11
            ImageBuf++;
            if( value & 0x40 ) *ImageBuf=BLANK_FILL;        //By zjh 96.10.11
            ImageBuf++;
            if( value & 0x20 ) *ImageBuf=BLANK_FILL;        //By zjh 96.10.11
            ImageBuf++;
            if( value & 0x10 ) *ImageBuf=BLANK_FILL;        //By zjh 96.10.11
            ImageBuf++;
            if( value & 0x8 ) *ImageBuf=BLANK_FILL;        //By zjh 96.10.11
            ImageBuf++;
            if( value & 0x4 ) *ImageBuf=BLANK_FILL;        //By zjh 96.10.11
            ImageBuf++;
            if( value & 0x2 ) *ImageBuf=BLANK_FILL;        //By zjh 96.10.11
            ImageBuf++;
            if( value & 0x1 ) *ImageBuf=BLANK_FILL;        //By zjh 96.10.11
            ImageBuf++;
         }
         break;
    case 4:      /* 4 bits per color */
         n=BytesPerLine;
         if(CompressMethod&0x40) n*=4;  /* 4 planes */

         for(Col=0;Col<n;Col++)  {
            value=MyBuf[Col];
            *(ImageBuf+0)=value>>4;
            *(ImageBuf+1)=value&0xf;
            if (PutSign==PUTDITHERDEVICE&&TiffPresent->RGBPaletteSign) {
                value=*(ImageBuf+0)*3;
                *(ImageBuf+0)=((long)TiffPresent->ByteRGBPalette[value]<<16)
                      |((long)TiffPresent->ByteRGBPalette[value+1]<<8)
                      |TiffPresent->ByteRGBPalette[value+2];
                value=*(ImageBuf+1)*3;
                *(ImageBuf+1)=((long)TiffPresent->ByteRGBPalette[value]<<16)
                      |((long)TiffPresent->ByteRGBPalette[value+1]<<8)
                      |TiffPresent->ByteRGBPalette[value+2];
            }
            ImageBuf+=2;
         }
         break;
    case 8:      /* 1 byte per color */
         // if (PutSign==PUTDITHERDEVICE){
         if (PutSign==PUTDITHERDEVICE&&TiffPresent->RGBPaletteSign) {
              for(Col=0;Col<BytesPerLine;Col++)  {
                 value=3*MyBuf[Col];
                 *ImageBuf++=((long)TiffPresent->ByteRGBPalette[value]<<16)
                      |((long)TiffPresent->ByteRGBPalette[value+1]<<8)
                      |((long)TiffPresent->ByteRGBPalette[value+2]);
              }
         } else {
          // memcpy(ImageBuf,MyBuf,BytesPerLine);
              for(Col=0;Col<BytesPerLine;Col++)
                *ImageBuf++=MyBuf[Col];
         }
         break;
    case 24:     /* 3 bytes per color */
         if(CompressMethod&0x80) {   /* for BMP */
            for(Col=0;Col<BytesPerLine;Col+=3)  {
               *ImageBuf++=((long)MyBuf[Col+2]<<16)
                    |((long)MyBuf[Col+1]<<8)
                    |MyBuf[Col];
            }
            break;
         }

         for(Col=0;Col<BytesPerLine;Col+=3)  {
            *ImageBuf++=((long)MyBuf[Col]<<16)
                 |((long)MyBuf[Col+1]<<8)
                 |MyBuf[Col+2];
         }
         break;
  }  /* switch BitsPerPixel */
} /* GetOneLine */

static int DrawImage(FILE *fp,unsigned char FHUGE *pBuf,UINT YStart, \
         UINT ImageWidth,UINT ImageHeight,\
         UINT BytesPerLine,float XRate,float YRate,UINT Flag_BitsPerPixel,\
         ImageDescribes *TiffPresent,char PutSign,UINT CompressMethod)
{
      UINT BitsPerPixel=Flag_BitsPerPixel & 0xff;
      UINT XRateI,XRateF,YRateI,YRateF;
      UINT NextRow,NextCol,LastRow,LastCol;
      UINT Row,Col,NextRowf,NextColf;
      ULONG OldColor;
      ULONG *ColorBuf;

      //ColorBuf=(ULONG *)malloc(ImageWidth*sizeof(ULONG));
      ColorBuf=(ULONG *)malloc(BytesPerLine*8*sizeof(ULONG));
      // if(ColorBuf==NULL)
      if(ColorBuf<0x1000)
      {
          ReportMemoryError("drawimg");
          return -1;         // max row
      }

       XRateI=(int)(XRate+0.0001);  XRateF=0.01+(XRate-XRateI)*1024;
       YRateI=(int)(YRate+0.0001);  YRateF=0.01+(YRate-YRateI)*1024;

       NextRowf=deltaY;
       NextRow=LastRow=0;

       if (PutSign==PUTTRUECOLORDEVICE)
            FindClearWhiteColor(TiffPresent,BitsPerPixel);

       for(Row=0;Row<ImageHeight;Row++) {
            GetOneLine(fp,ColorBuf,Flag_BitsPerPixel,BytesPerLine,
                      PutSign,TiffPresent,CompressMethod);
         /* color'data is CharType, so, do not need to ConvertMMII */

            NextRow+=YRateI;
            NextRowf+=YRateF;
            if(NextRowf>=1024) { NextRowf-=1024; NextRow++; }

            NextCol=LastCol=0;
            NextColf=0;
            OldColor=ColorBuf[0];

            for(Col=0;Col<ImageWidth;Col++) {
 /*-------- 5.4 : process Col's color ------*/
                if ( ColorBuf[Col]==OldColor )  {
                       NextCol+=XRateI;
                       NextColf+=XRateF;
                       if (NextColf>=1024) { NextColf-=1024; NextCol++; }
                       continue;
                 }

                 if(NextCol>LastCol && NextRow>LastRow) {
                       ColorBlock(pBuf,LastCol,YStart+LastRow,NextCol-1,\
                         YStart+NextRow-1,TiffPresent,PutSign,OldColor,BitsPerPixel);
                 }

                 OldColor=ColorBuf[Col];
                 LastCol=NextCol;
                 NextCol+=XRateI;
                 NextColf+=XRateF;
                 if (NextColf>=1024) { NextColf-=1024; NextCol++; }
            }  /*-- Col --*/

            //if(OldColor==ColorBuf[ImageWidth-2] &&        // exp. xxxxxxx
            if(NextCol>LastCol && NextRow>LastRow)
                 ColorBlock(pBuf,LastCol,YStart+LastRow,NextCol-1,\
                    YStart+NextRow-1,TiffPresent,PutSign,OldColor,BitsPerPixel);

       /*------- clear tail with white color ---------*/
            if(NextCol<TiffPresent->ImageWidth && NextRow>LastRow)
              ClearBlockTail(pBuf,NextCol,YStart+LastRow,TiffPresent->ImageWidth-1,
                  YStart+NextRow-1,TiffPresent,PutSign,BitsPerPixel);
           /*--------------*/
            LastRow=NextRow;
       }  /*-- Row --*/

    /*------- clear tail with white color ---------
     if(NextCol<TiffPresent->ImageWidth && NextRow>LastRow)
       ClearBlockTail(pBuf,NextCol,YStart+LastRow,TiffPresent->ImageWidth-1,
           YStart+NextRow-1,TiffPresent,PutSign,BitsPerPixel);
        --------------*/

     deltaY=NextRowf;
     free(ColorBuf);
     //For test
     /*
     {
        int i,j,k;
        UCHAR c;
        int len;
        len=TiffPresent->ImageWidth;
        len=((len+1)>>1);
              setviewport(0,0,639,479,0);
        for (i=YStart;i<YStart+NextRow;i++)
          for (j=0;j<TiffPresent->ImageWidth;j++)
           {
            c=pBuf[i*len+j/2];
            if (j&1)
             c=(c&15); else c=c/16;
            setcolor(c);
            line(j,i,j,i);
           }

     }
       */
     return(NextRow);

} /* DrawImage */

/* ------- Followed for TIFF restore ----- */

#define ConvertMMII(p,Size,TiffSign)
/*-------------------------
static UINT MotorIntel(UINT MM)
{
  return((MM>>8)&0xff+(MM<<8)&0xff00);
}
static void ConvertMMII(UINT *p,unsigned int Size,unsigned TiffSign)
{
  unsigned int i,j;

  #ifdef __MOTORALA__
  if (TiffSign==0x4d4d) return;
  #else
  if (TiffSign==0x4949) return;
  #endif

  for (i=0,j=Size/sizeof(short);i<j;i++) p[i]=(p[i]>>8)&0xff+(p[i]<<8)&0xff00;
}
--------------------------*/

#ifdef NOT_USED
ColorMap1[5][5][5][4]={
{
{
/*  000,000,000  */  {0,0,0,0},
/*  000,000,064  */  {0,1,1,0},
/*  000,000,128  */  {1,1,1,1},
/*  000,000,192  */  {1,9,9,1},
/*  000,000,256  */  {9,9,9,9},
},
{
/*  000,064,000  */  {0,2,2,0},
/*  000,064,064  */  {0,3,3,0},
/*  000,064,128  */  {1,0,2,1},
/*  000,064,192  */  {1,0,2,9},
/*  000,064,256  */  {9,0,2,9},
},
{
/*  000,128,000  */  {2,2,2,2},
/*  000,128,064  */  {0,2,2,1},
/*  000,128,128  */  {3,3,3,3},
/*  000,128,192  */  {1,2,2,9},
/*  000,128,256  */  {9,2,2,9},
},
{
/*  000,192,000  */  {2,10,10,2},
/*  000,192,064  */  {0,2,10,1},
/*  000,192,128  */  {1,2,10,1},
/*  000,192,192  */  {3,11,11,3},
/*  000,192,256  */  {9,2,10,9},
},
{
/*  000,256,000  */  {10,10,10,10},
/*  000,256,064  */  {0,10,10,1},
/*  000,256,128  */  {1,10,10,1},
/*  000,256,192  */  {1,10,10,9},
/*  000,256,256  */  {11,11,11,11},
},
},
{
{
/*  064,000,000  */  {0,4,4,0},
/*  064,000,064  */  {0,5,5,0},
/*  064,000,128  */  {0,1,1,4},
/*  064,000,192  */  {0,1,9,4},
/*  064,000,256  */  {0,9,9,4},
},
{
/*  064,064,000  */  {0,6,6,0},
/*  064,064,064  */  {0,7,7,0},
/*  064,064,128  */  {0,1,1,6},
/*  064,064,192  */  {0,1,9,6},
/*  064,064,256  */  {0,9,9,6},
},
{
/*  064,128,000  */  {0,2,2,4},
/*  064,128,064  */  {0,2,2,5},
/*  064,128,128  */  {3,0,4,3},
/*  064,128,192  */  {0,3,3,5},
/*  064,128,256  */  {0,3,1,4},
},
{
/*  064,192,000  */  {0,2,10,4},
/*  064,192,064  */  {0,2,10,5},
/*  064,192,128  */  {0,3,3,6},
/*  064,192,192  */  {3,0,4,11},
/*  064,192,256  */  {0,3,11,5},
},
{
/*  064,256,000  */  {0,10,10,4},
/*  064,256,064  */  {0,10,10,5},
/*  064,256,128  */  {0,2,3,4},
/*  064,256,192  */  {0,3,11,6},
/*  064,256,256  */  {11,0,4,11},
},
},
{
{
/*  128,000,000  */  {4,4,4,4},
/*  128,000,064  */  {4,0,1,4},
/*  128,000,128  */  {5,5,5,5},
/*  128,000,192  */  {4,1,9,4},
/*  128,000,256  */  {4,9,9,4},
},
{
/*  128,064,000  */  {4,0,2,4},
/*  128,064,064  */  {0,4,4,3},
/*  128,064,128  */  {5,0,2,5},
/*  128,064,192  */  {5,0,3,5},
/*  128,064,256  */  {5,0,2,1},
},
{
/*  128,128,000  */  {6,6,6,6},
/*  128,128,064  */  {6,0,1,6},
/*  128,128,128  */  {7,7,7,7},
/*  128,128,192  */  {6,1,9,6},
/*  128,128,256  */  {6,9,9,6},
},
{
/*  128,192,000  */  {4,2,10,4},
/*  128,192,064  */  {6,0,3,6},
/*  128,192,128  */  {5,2,10,5},
/*  128,192,192  */  {3,4,4,11},
/*  128,192,256  */  {2,5,1,10},
},
{
/*  128,256,000  */  {4,10,10,4},
/*  128,256,064  */  {6,0,1,2},
/*  128,256,128  */  {5,10,10,5},
/*  128,256,192  */  {6,1,9,2},
/*  128,256,256  */  {11,4,4,11},
},
},
{
{
/*  192,000,000  */  {4,12,12,4},
/*  192,000,064  */  {4,0,1,12},
/*  192,000,128  */  {4,1,1,12},
/*  192,000,192  */  {5,13,13,5},
/*  192,000,256  */  {4,9,9,12},
},
{
/*  192,064,000  */  {4,0,2,12},
/*  192,064,064  */  {0,4,12,3},
/*  192,064,128  */  {5,0,6,5},
/*  192,064,192  */  {5,0,2,13},
/*  192,064,256  */  {5,0,3,13},
},
{
/*  192,128,000  */  {4,2,2,12},
/*  192,128,064  */  {6,0,5,6},
/*  192,128,128  */  {3,4,12,3},
/*  192,128,192  */  {5,2,2,13},
/*  192,128,256  */  {4,3,1,12},
},
{
/*  192,192,000  */  {6,14,14,6},
/*  192,192,064  */  {6,0,1,14},
/*  192,192,128  */  {6,1,1,14},
/*  192,192,192  */  {8,8,8,8},
/*  192,192,256  */  {6,9,9,14},
},
{
/*  192,256,000  */  {4,10,10,12},
/*  192,256,064  */  {6,0,3,14},
/*  192,256,128  */  {4,2,3,12},
/*  192,256,192  */  {5,10,10,13},
/*  192,256,256  */  {11,4,12,11},
},
},
{
{
/*  256,000,000  */  {12,12,12,12},
/*  256,000,064  */  {12,0,1,12},
/*  256,000,128  */  {12,1,1,12},
/*  256,000,192  */  {12,1,9,12},
/*  256,000,256  */  {13,13,13,13},
},
{
/*  256,064,000  */  {12,0,2,12},
/*  256,064,064  */  {0,12,12,3},
/*  256,064,128  */  {4,0,2,5},
/*  256,064,192  */  {0,5,13,6},
/*  256,064,256  */  {13,0,2,13},
},
{
/*  256,128,000  */  {12,2,2,12},
/*  256,128,064  */  {4,0,1,6},
/*  256,128,128  */  {3,12,12,3},
/*  256,128,192  */  {4,1,9,6},
/*  256,128,256  */  {13,2,2,13},
},
{
/*  256,192,000  */  {12,2,10,12},
/*  256,192,064  */  {0,6,14,5},
/*  256,192,128  */  {4,2,10,5},
/*  256,192,192  */  {3,12,12,11},
/*  256,192,256  */  {13,2,10,13},
},
{
/*  256,256,000  */  {14,14,14,14},
/*  256,256,064  */  {14,0,1,14},
/*  256,256,128  */  {14,1,1,14},
/*  256,256,192  */  {14,1,9,14},
/*  256,256,256  */  {15,15,15,15},
}
}
};

static int GetDitherColor(long NowColor,int X,int Y)
{
  unsigned short R,G,B,I,ret,s1,s2,s3,m1,m2,m3,l;
  unsigned short Light;
  static unsigned ColorMap[3][3][3][2]={
   {
        {
        {0,0},
        {1,1},
        {9,9}
        },
        {
        {2,2},
        {3,3},
        {3,1},
        },
        {
        {10,10},
        {2,3},
        {11,11}
        }
   },
   {
        {
        {4,4},
        {5,5},
        {5,1}
        },
        {
        {6,6},
        {7,7},
        {7,1}
        },
        {
        {6,2},
        {7,2},
        {7,3}
        }
   },
   {
        {
        {12,12},
        {4,5},
        {13,13}
        },
        {
        {4,6},
        {4,7},
        {5,7}
        },
        {
        {14,14},
        {6,7},
        {15,15}
        }
   }
};
  static unsigned char LightnessMatrix[16][16]=
  {
     { 0,235,59,219,15,231,55,215,2,232,56,217,12,229,52,213 },
     { 128,64,187,123,143,79,183,119,130,66,184,120,140,76,180,116 },
     { 33,192,16,251,47,207,31,247,34,194,18,248,44,204,28,244 },
     { 161,97,144,80,175,111,159,95,162,98,146,82,172,108,156,92 },
     { 8,255,48,208,5,239,63,223,10,226,50,210,6,236,60,220 },
     { 136,72,176,112,133,69,191,127,138,74,178,114,134,70,188,124 },
     { 41,200,24,240,36,197,20,255,42,202,26,242,38,198,22,252 },
     { 169,105,152,88,164,100,148,84,170,106,154,90,166,102,150,86 },
     { 3,233,57,216,13,228,53,212,1,234,58,218,14,230,54,214 },
     { 131,67,185,121,141,77,181,117,129,65,186,122,142,78,182,118 },
     { 35,195,19,249,45,205,29,245,32,193,17,250,46,206,30,246 },
     { 163,99,147,83,173,109,157,93,160,96,145,81,174,110,158,94 },
     { 11,227,51,211,7,237,61,221,9,224,49,209,4,238,62,222 },
     { 139,75,179,115,135,71,189,125,137,73,177,113,132,68,190,126 },
     { 43,203,27,243,39,199,23,253,40,201,25,241,37,196,21,254 },
     { 171,107,155,91,167,103,151,87,168,104,153,89,165,101,149,85 }
  };

  R=(NowColor>>16)&0xff;
  G=(NowColor>>8)&0xff;
  B=NowColor&0xff;
  I=(R+G+B)/3;
  /*
  R=R*256/255;
  G=G*256/255;
  B=B*256/255;
  */
  R=(R+2)&0x1fc;
  G=(G+2)&0x1fc;
  B=(B+2)&0x1fc;
  s1=R/64;
  s2=G/64;
  s3=B/64;
  m1=R%64;
  m2=G%64;
  m3=B%64;

  Light=LightnessMatrix[Y&0xf][X&0xf];
  l=Light/4;

  /*-------- R(G,B) :: [0..255], changed to [0..256],
      if it great than Light, get array[1], else get array[0]
   --------------------------------------------------------*/

  return (ColorMap1[s1+(m1>l)]
                  [s2+(m2>l)]
                  [s3+(m3>l)]
                   [(X&1)*2+(Y&1)]);
  /*
  return (ColorMap[((R*256/255) > Light)+(R>127)]
                   [((G*256/255) > Light)+(G>127)]
                   [((B*256/255) > Light)+(B>127)]
                   [(x&1)*2+(y&1)]);
  */

}
#endif

#ifdef UNUSE
unsigned char PureColor[][2]={
                        {4,12},
                        {2,10},
                        {1,9},
                        {6,14},   /* RG */
                        {5,13},   /* RB */
                        {3,11},   /* GB */
            {7,8},
                        {15,0}
                        };
static unsigned char LM[16][16]=
  {
     { 0,235,59,219,15,231,55,215,2,232,56,217,12,229,52,213 },
     { 128,64,187,123,143,79,183,119,130,66,184,120,140,76,180,116 },
     { 33,192,16,251,47,207,31,247,34,194,18,248,44,204,28,244 },
     { 161,97,144,80,175,111,159,95,162,98,146,82,172,108,156,92 },
     { 8,255,48,208,5,239,63,223,10,226,50,210,6,236,60,220 },
     { 136,72,176,112,133,69,191,127,138,74,178,114,134,70,188,124 },
     { 41,200,24,240,36,197,20,255,42,202,26,242,38,198,22,252 },
     { 169,105,152,88,164,100,148,84,170,106,154,90,166,102,150,86 },
     { 3,233,57,216,13,228,53,212,1,234,58,218,14,230,54,214 },
     { 131,67,185,121,141,77,181,117,129,65,186,122,142,78,182,118 },
     { 35,195,19,249,45,205,29,245,32,193,17,250,46,206,30,246 },
     { 163,99,147,83,173,109,157,93,160,96,145,81,174,110,158,94 },
     { 11,227,51,211,7,237,61,221,9,224,49,209,4,238,62,222 },
     { 139,75,179,115,135,71,189,125,137,73,177,113,132,68,190,126 },
     { 43,203,27,243,39,199,23,253,40,201,25,241,37,196,21,254 },
     { 171,107,155,91,167,103,151,87,168,104,153,89,165,101,149,85 }
  };

int GetDitherColor(ULONG TureColor,int x,int y)
{
  #define RR 0
  #define GG 1
  #define BB 2
  #define RG 3
  #define RB 4
  #define GB 5

  int li,i;
  unsigned short R,G,B;
  unsigned short i1,i2,i3,s1,s2,s3,s;
  unsigned short p1,p2,p3;
  unsigned short color,index;
  unsigned short p[6],b1,b2,b3;
  unsigned short co[6];

  R=((TureColor>>16)&0xff);
  G=((TureColor>>8)&0xff);
  B=(TureColor&0xff);


  if (R+G+B==0) return 0;
  if (R>G)
  {
    if (R>B)
      {
         if (G>B)
         /*  R>G>B */
         {
          i1=B;    s1=6;
          i2=G-B;  s2=RG;
          i3=R-G;  s3=RR;
         }
         else
         /* R>B>G */
         {
          i1=G;    s1=6;
          i2=B-G;  s2=RB;
          i3=R-G;  s3=RR;
         }
      }
      else
      /*  G<R<B */
      {
          i1=G;    s1=6;
          i2=R-G;  s2=RB;
          i3=B-R;  s3=BB;
      }
  }
  else
  {
    /*  R<G */
    if (R<B)
      {
         if (G>B)
         /*  R<B<G */
         {
          i1=R;    s1=6;
          i2=B-R;  s2=GB;
          i3=G-B;  s3=GG;
         }
         else
         /* R<G<B */
         {
          i1=R;    s1=6;
          i2=G-R;  s2=GB;
          i3=B-G;  s3=BB;
         }
      }
    else
      /*  G>R>B */
      {
          i1=B;    s1=6;
          i2=R-B;  s2=RG;
          i3=G-R;  s3=GG;
      }
  }

  if (i1<254) i1=((i1+2)&0xfc);
  if (i2<254) i2=((i2+2)&0xfc);
  if (i3<254) i3=((i3+2)&0xfc);

  if (i1<128)
     {
     b1=i1*255/127;
     co[0]=PureColor[6][0];
     co[3]=0;
     }
  else if (i1<192) {
     b1=(i1-128)*255/63;
     co[0]=PureColor[6][1];
     co[3]=PureColor[6][0];
     }
  else {
     b1=(i1-192)*255/63;
     co[0]=PureColor[7][0];
     co[3]=PureColor[6][1];
     }

  if (i2<128)
     {
     b2=i2*255/127;
     co[1]=PureColor[s2][0];
     co[4]=0;
     }
  else {
     b2=(i2-128)*255/127;
     co[1]=PureColor[s2][1];
     co[4]=PureColor[s2][0];
     }

  if (i3<128)
     {
     b3=i3*255/127;
     co[2]=PureColor[s3][0];
     co[5]=0;
     }
  else {
     b3=(i3-128)*255/127;
     co[2]=PureColor[s3][1];
     co[5]=PureColor[s3][0];
     }

  i1=i1+i1+i1;
  i2=i2+i2;
  s=i1+i2+i3;
  if (s==0) return 0;

  /* Nnew=(n-c1)*255/(c2-c1)  */

  p1=((long)(i1)<<8)/s;
  p2=((long)(i2)<<8)/s;
  p3=(256-p1-p2);
  p[0]=(p1*b1)/255;
  p[1]=(p2*b2)/255;
  p[2]=(p3*b3)/255;
  p[3]=p1-p[0];
  p[4]=p2-p[1];
  p[5]=p3-p[2];


  if (co[3]==0)
  {
   s=256-p[0]-p[3];
   if (s>0)
   {
   p[1]=p[1]+p[3]*p[1]/s;
   p[4]=p[4]+p[3]*p[4]/s;
   p[2]=p[2]+p[3]*p[2]/s;
   p[5]=p[5]+p[3]*p[5]/s;
   //p[5]=s-p[1]-p[4]-p[2];
   p[3]=0;
   }
  }

  if (co[4]==0)
  {
   s=p[2]+p[5];
   if (s>0)
   {
   p[2]=p[2]+p[4]*p[2]/s;
   p[5]=p[5]+p[4]*p[5]/s;
   //p[5]=s-p[2];
   p[4]=0;
   }
  }

  li=LM[x&15][y&15];

  s=0;
  b1=0;
  for (i=0;i<5;i++)
  {
    s=s+p[i];
    if (p[i]>p[b1]) b1=i;
    if (p[i]>3&&li<s) return co[i];
  }
  if (p[5]>3) return co[5];
  else if (p[b1]>3) return co[b1];
  else return 0;

  /*
  s=0;
  for (i=0;i<6;i++)
  {
    s=s+p[i];
    if (li<s) return co[i];
  }

  //assert(i<6);
  return co[5];
  */
  #undef RR
  #undef GG
  #undef BB
  #undef RG
  #undef RB
  #undef GB
}
#endif

#ifdef UNUSE
int GetDitherColor(ULONG NowColor,int x,int y)
{
   int i,colorFind,ErrorRed,ErrorGreen,ErrorBlue;
   long ldisMin,ldisCur;
   int Red,Green,Blue;
   Red=(NowColor>>16)&0xff;
   Green=(NowColor>>8)&0xff;
   Blue=NowColor&0xff;

   ldisMin=2000000l;
   for (i=0;i<16;i++)
   {
       //if (palUsing[i]==0) continue;

       ErrorRed=(Red-RGB[i][0]);
       ErrorGreen=(Green-RGB[i][1]);
       ErrorBlue=(Blue-RGB[i][2]);

       //ldisCur=(long)ErrorRed*(long)ErrorRed+
       //        (long)ErrorGreen*(long)ErrorGreen+
       //        (long)ErrorBlue*(long)ErrorBlue;
       {
        long R,G,B;
        R=abs(ErrorRed);
        G=abs(ErrorGreen);
        B=abs(ErrorBlue);
        if (R+G+B==0) return i;
        ldisCur=(R*30+G*59+B*11);
       }

       if (ldisCur<ldisMin)
       {
          colorFind=i;
          ldisMin=ldisCur;
       }
   }

   return colorFind;
}

static int GetDitherColor(long NowColor,int X,int Y)
{
  unsigned short R,G,B;
  unsigned short Light;
  static unsigned char LightnessMatrix[16][16]=
  {
     { 0,235,59,219,15,231,55,215,2,232,56,217,12,229,52,213 },
     { 128,64,187,123,143,79,183,119,130,66,184,120,140,76,180,116 },
     { 33,192,16,251,47,207,31,247,34,194,18,248,44,204,28,244 },
     { 161,97,144,80,175,111,159,95,162,98,146,82,172,108,156,92 },
     { 8,255,48,208,5,239,63,223,10,226,50,210,6,236,60,220 },
     { 136,72,176,112,133,69,191,127,138,74,178,114,134,70,188,124 },
     { 41,200,24,240,36,197,20,255,42,202,26,242,38,198,22,252 },
     { 169,105,152,88,164,100,148,84,170,106,154,90,166,102,150,86 },
     { 3,233,57,216,13,228,53,212,1,234,58,218,14,230,54,214 },
     { 131,67,185,121,141,77,181,117,129,65,186,122,142,78,182,118 },
     { 35,195,19,249,45,205,29,245,32,193,17,250,46,206,30,246 },
     { 163,99,147,83,173,109,157,93,160,96,145,81,174,110,158,94 },
     { 11,227,51,211,7,237,61,221,9,224,49,209,4,238,62,222 },
     { 139,75,179,115,135,71,189,125,137,73,177,113,132,68,190,126 },
     { 43,203,27,243,39,199,23,253,40,201,25,241,37,196,21,254 },
     { 171,107,155,91,167,103,151,87,168,104,153,89,165,101,149,85 }
  };

  static unsigned char ColorTable[2][2][2]={      /* R,G,B */
 /*-------------------
     {
       // [0,0,0]       [0,0,1]
       { EGA_BLACK, EGA_LIGHTBLUE },
       // [0,1,0]       [0,1,1]
       { EGA_LIGHTGREEN, EGA_LIGHTCYAN }
     },
     {
       // [1,0,0]       [1,0,1]
       { EGA_LIGHTRED, EGA_LIGHTMAGENTA },
       // [1,1,0]       [1,1,1]
       { EGA_YELLOW, EGA_WHITE }
     }
 -----------------*/
     {
       { BLACK, LIGHTBLUE }, { LIGHTGREEN, LIGHTCYAN }
     },
     {
       { LIGHTRED, LIGHTMAGENTA }, { YELLOW, WHITE }
     }
  };

  R=(NowColor>>16)&0xff;
  G=(NowColor>>8)&0xff;
  B=NowColor&0xff;

  Light=LightnessMatrix[Y&0xf][X&0xf];

  /*-------- R(G,B) :: [0..255], changed to [0..256],
      if it great than Light, get array[1], else get array[0]
   --------------------------------------------------------*/
  return(ColorTable[(R*256/255) > Light]
                   [(G*256/255) > Light]
                   [(B*256/255) > Light]);
}
//#endif

static int GetDitherColor(long NowColor,int X,int Y)
{
  unsigned short R,G,B,ret,s1,s2,s3,m1,m2,m3,l1,s4;
  unsigned short Light;
  static unsigned ColorMap[3][3][3][2]={
   {
        {
        {0,0},
        {1,1},
        {9,9}
        },
        {
        {2,2},
        {3,3},
    {3,1}    //{2,9} //{3,1}, //011 001
        },
        {
        {10,10},
    {2,3},     //{10,1},//{2,3},     //010 011
        {11,11}
        }
   },
   {
        {
        {4,4},
        {5,5},
    {5,1},   //{4,9}    //{5,1}  //101 001
        },
        {
        {6,6},
        {7,7},
    {7,1}         //{6,9}      //{7,1}   //111 001
        },
        {
    {6,2}, //{4,10},//{6,2}, //110,010
    {7,2}, // {5,10},//{7,2}, //111 010
    {7,3}  //{4,11} //{7,3}    //111,011
        }
   },
   {
        {
        {12,12},
    {4,5},  //{12,1},//{4,5},  //100 101
        {13,13}
        },
        {
    {4,6},   //{12,2},//{4,6}, //100 110
    {4,7},   //{12,3},//{4,7}, //100 111
    {5,7},   //{13,2} //{5,7}  //101 111
        },
        {
        {14,14},
    {6,7},  //{14,1},//{6,7},   //110 111
        {15,15}
        }
   }
};
  static unsigned char LightnessMatrix[16][16]=
  {
     { 0,235,59,219,15,231,55,215,2,232,56,217,12,229,52,213 },
     { 128,64,187,123,143,79,183,119,130,66,184,120,140,76,180,116 },
     { 33,192,16,251,47,207,31,247,34,194,18,248,44,204,28,244 },
     { 161,97,144,80,175,111,159,95,162,98,146,82,172,108,156,92 },
     { 8,255,48,208,5,239,63,223,10,226,50,210,6,236,60,220 },
     { 136,72,176,112,133,69,191,127,138,74,178,114,134,70,188,124 },
     { 41,200,24,240,36,197,20,255,42,202,26,242,38,198,22,252 },
     { 169,105,152,88,164,100,148,84,170,106,154,90,166,102,150,86 },
     { 3,233,57,216,13,228,53,212,1,234,58,218,14,230,54,214 },
     { 131,67,185,121,141,77,181,117,129,65,186,122,142,78,182,118 },
     { 35,195,19,249,45,205,29,245,32,193,17,250,46,206,30,246 },
     { 163,99,147,83,173,109,157,93,160,96,145,81,174,110,158,94 },
     { 11,227,51,211,7,237,61,221,9,224,49,209,4,238,62,222 },
     { 139,75,179,115,135,71,189,125,137,73,177,113,132,68,190,126 },
     { 43,203,27,243,39,199,23,253,40,201,25,241,37,196,21,254 },
     { 171,107,155,91,167,103,151,87,168,104,153,89,165,101,149,85 }
  };

  R=(NowColor>>16)&0xff;
  G=(NowColor>>8)&0xff;
  B=NowColor&0xff;
  Light=LightnessMatrix[Y&0xf][X&0xf];

  R=((R+2)&0x1fc);
  G=((G+2)&0x1fc);
  B=((B+2)&0x1fc);
  l1=(Light>>1);
  //l1=(Light&0x7f);
  m1=(R&0x7f);
  m2=(G&0x7f);
  m3=(B&0x7f);
  s1=(R>>7)+(m1>l1);
  s2=(G>>7)+(m2>l1);
  s3=(B>>7)+(m3>l1);
  s4=((X+Y)&1);

  /*-------- R(G,B) :: [0..255], changed to [0..256],
      if it great than Light, get array[1], else get array[0]
   --------------------------------------------------------*/
  /*
  return (ColorMap[((R*256/255) > Light)+(R>127)]
                   [((G*256/255) > Light)+(G>127)]
                   [((B*256/255) > Light)+(B>127)]
                   [(X+Y)&1]);
  */
  ret=(ColorMap[s1][s2][s3][s4]);
  if (s1==s2&&s3==s2&&ret)
  {

    l1=(Light>>2);
    //l1=(Light&0x3f);
    m1=(R&0x3f);
    m2=(G&0x3f);
    m3=(B&0x3f);
    s1=(R>>6)+(m1>l1);
    s2=(G>>6)+(m2>l1);
    s3=(B>>6)+(m3>l1);
    if ((s1+s2+s3+1)/3==3) ret=8;

    //s1=R*30+G*59+B*11/100;
    //if (abs(s1-192)<32) ret=8;
  }
  return ret;

  }
#endif

static int GetDitherColor(long NowColor,int X,int Y)
{
  unsigned short R,G,B,ret,s1,s2,s3,m1,m2,m3,l1,s4;
  unsigned short Light;
  static unsigned ColorMap[3][3][3][2]=
  {   /*-- bits=[Bright Red Green Blue]  --*/
     {   /*- 0: RedBit=0 -*/
         { { 0, 0}, { 1, 1}, { 9, 9} },   /* GreenBit=0 */
         { { 2, 2}, { 3, 3}, { 9,11} },
         { {10,10}, {10,11}, {11,11} }    /* GreenBit=1 */
     },
     {   /*- 1:  -*/
         { { 4, 4}, { 5, 5}, { 9,13} },   /* GreenBit=0 */
         { { 6, 6}, { 7, 7}, { 9,15} },
         { {10,14}, {10,15}, {11,15} }    /* GreenBit=1 */
     },
     {   /*- 2: RedBit=1 -*/
         { {12,12}, {12,13}, {13,13} },   /* GreenBit=0 */
         { {12,14}, {12,15}, {13,15} },
         { {14,14}, {14,15}, {15,15} }    /* GreenBit=1 */
       /*   --^       ^  ^      ^
        BlueBit=0     x  1      1
        ------------------------*/
     }
  };

  static unsigned char LightnessMatrix[16][16]=
  {
     { 0,235,59,219,15,231,55,215,2,232,56,217,12,229,52,213 },
     { 128,64,187,123,143,79,183,119,130,66,184,120,140,76,180,116 },
     { 33,192,16,251,47,207,31,247,34,194,18,248,44,204,28,244 },
     { 161,97,144,80,175,111,159,95,162,98,146,82,172,108,156,92 },
     { 8,255,48,208,5,239,63,223,10,226,50,210,6,236,60,220 },
     { 136,72,176,112,133,69,191,127,138,74,178,114,134,70,188,124 },
     { 41,200,24,240,36,197,20,255,42,202,26,242,38,198,22,252 },
     { 169,105,152,88,164,100,148,84,170,106,154,90,166,102,150,86 },
     { 3,233,57,216,13,228,53,212,1,234,58,218,14,230,54,214 },
     { 131,67,185,121,141,77,181,117,129,65,186,122,142,78,182,118 },
     { 35,195,19,249,45,205,29,245,32,193,17,250,46,206,30,246 },
     { 163,99,147,83,173,109,157,93,160,96,145,81,174,110,158,94 },
     { 11,227,51,211,7,237,61,221,9,224,49,209,4,238,62,222 },
     { 139,75,179,115,135,71,189,125,137,73,177,113,132,68,190,126 },
     { 43,203,27,243,39,199,23,253,40,201,25,241,37,196,21,254 },
     { 171,107,155,91,167,103,151,87,168,104,153,89,165,101,149,85 }
  };

  R=(NowColor>>16)&0xfc;
  G=(NowColor>>8)&0xfc;
  B=NowColor&0xfc;
  Light=LightnessMatrix[Y&0xf][X&0xf];

  R=(R*256/0xfc)&0x1fc;
  G=(G*256/0xfc)&0x1fc;
  B=(B*256/0xfc)&0x1fc;

  l1=(Light>>1);
  //l1=(Light&0x7f);
  m1=(R&0x7f);
  m2=(G&0x7f);
  m3=(B&0x7f);
  s1=(R>>7)+(m1>l1);
  s2=(G>>7)+(m2>l1);
  s3=(B>>7)+(m3>l1);
  s4=((X+Y)&1);

  ret=(ColorMap[s1][s2][s3][s4]);
  if (s1==s2&&s3==s2&&ret)       /*-- ret=0,7,15 --*/
  {
    l1=(Light>>2);
    //l1=(Light&0x3f);
    m1=(R&0x3f);
    m2=(G&0x3f);
    m3=(B&0x3f);
    s1=(R>>6)+(m1>l1);
    s2=(G>>6)+(m2>l1);
    s3=(B>>6)+(m3>l1);
    if ((s1+s2+s3+1)/3==3) ret=8;   /*- change 7 to 8 -*/
  }

  return ret;
}


///////////////////By zjh For lzw compress///////////////////////
int unlzw(char *sbuff,int slen,char *tbuff,int tlen);
#define ScripBuffLen 20000
long myScripBytes[1000];
long myScripOffset[1000];
char myScripBuff[ScripBuffLen];
short myRowsPerScrip;
short myScripPerImage;
long myCurrentRow=0L;
short myCurrentScrip=-1;



int GetTiffCompreeLine(FILE *fp,char *buff,UINT myBytesPerRow)
{
   int i;
   char data[ScripBuffLen];
   if (myCurrentRow/myRowsPerScrip!=myCurrentScrip)
   {
       i=myCurrentRow/myRowsPerScrip;
       myCurrentScrip++;
       //fseek(fp,myScripOffset[i],SEEK_SET);
       i=min(ScripBuffLen-1,myScripBytes[i]);
       i=fread(data,1,i,fp);
       i=unlzw(data,i,myScripBuff,ScripBuffLen-1);
   }

   //get compress data form buff
   {
       i=myCurrentRow%myRowsPerScrip;
       i=i*myBytesPerRow;
       memcpy(buff,myScripBuff+i,myBytesPerRow);
       myCurrentRow++;
       return myBytesPerRow;
   }

}
////////////////////add end  96.10.10////////////////////////////


static int DealIFD(FILE *fp,ULONG IFDOffset,UINT TiffSign,
                   ImageDescribes *TiffPresent,char PutSign,int DeviceDPI,
                   int DeviceColorBits,HANDLE *ImageH24Data)
{
  static UCHAR IFDDataType[6]={ 0,1,1,2,4,8 };
  /* byte(s)|type  ===>    1: byte      2: char         3: short
                           4: long      5: fen_shu(fen_zi,fen_mu:4 bytes)
  ---------------------------------------------------------*/
  ULONG ImageHeight;
  ULONG ImageWidth,BytesPerLine;
  ULONG BitsPerPixel=1;
  USHORT RowsPerStrip=0xffff;   // ULONG RowsPerStrip=-1L;
  UINT  ResolutionUnit=2;     /* 1: None, 2:Inch, 3:cm */
  ULONG Xres,Yres;
  int   i,IFD_n;
  ULONG StripOffset,PerStripOffset,StripByteCounts;
  UINT  Compression=1;
  ULONG RGBPaletteOffset;
  UINT  RGBPalDataLen=0;
  IFDType IFD;
  UINT  StripNum,Photomet=1;
  UINT  YFrom;
  long  BufferLen;
  float XRate,YRate;
  unsigned char FHUGE *PresentImageBuffer, FHUGE *pImageBuf;
  unsigned char FHUGE *Tmp24ImageBuffer;
  HANDLE TmpHandle;
  char myType,myType1;

  Xres=Yres=TiffPresent->RGBPaletteSign=0;
/*---------- 1: read IFDs_toal_n and First IFD ---------*/
  fseek(fp,IFDOffset,SEEK_SET);
  IFD_n=GetWord(fp);
  ConvertMMII((UINT *)&IFD_n,sizeof(int),TiffSign);

/*---------- 2: read IFDs, and get parameters ------*/
  for(i=0;i<IFD_n;i++)  {
    fread(&IFD,sizeof(IFDType),1,fp);
    ConvertMMII((UINT *)&IFD,sizeof(IFDType),TiffSign);

/*---------- 3: process This IFD -----*/
    switch (IFD.SignCode) {
      case TIFCompression:
           Compression=IFD.Data;
           switch(Compression) {
                case 1:      Compression=NoPackByteCOMP; break;
                case 5:      Compression=LZW_COMP; break;
                case 32771:  Compression=NoPackWordCOMP; break;
                case 32773:  Compression=MacPainRunLenPackCOMP; break;
              default : return(SOURCECOMPRESS);
           }  /* switch Compression */
           break;
      case TIFImageWidth:
           ImageWidth=IFD.Data;
           break;
      case TIFImageHeight:
           ImageHeight=IFD.Data;
           break;
      case TIFResolutionUnit:
           ResolutionUnit=IFD.Data;
           break;
      case TIFXResolution:
           Xres=IFD.Data;
           break;
      case TIFYResolution:
           Yres=IFD.Data;
           break;
      case TIFRowsPerStrip:
           RowsPerStrip=IFD.Data;
           break;
      case TIFStripOffset:
           StripOffset=IFD.Data;
           myScripPerImage=IFD.Length;
           myType=IFDDataType[IFD.DataType];
           break;
      #define TIFStripByteCounts 279
      case TIFStripByteCounts:               //By zjh 96.10.10
           StripByteCounts=IFD.Data;
           myScripPerImage=IFD.Length;
           myType1=IFDDataType[IFD.DataType];
           break;
      case TIFPlanarConfiguration:
           if( (UINT)IFD.Data!=1 )
                return(SOURCEFORMAT);
           break;
      case TIFBitsPerSample:
           if (IFDDataType[IFD.DataType]*IFD.Length<=4)
                  BitsPerPixel=IFD.Data;
           else
           {
              BitsPerPixel=24;
              TiffPresent->RGBPaletteSign=1;
              //ImageSetBitColor(TiffPresent,IMAGETRUECOLOR);
           }
           break;
      case TIFColorMap:
           RGBPaletteOffset=IFD.Data;
           RGBPalDataLen=IFD.Length;
           break;
      case TIFPhotometricInterpretation:
           Photomet=(UINT)IFD.Data;
           break;
      default:
           break;
    }
  }

  //////////////////////By zjh for lzw compress/////////////////////////////
  {
    int i;
    myCurrentRow=0L;
    myCurrentScrip=-1;
    myRowsPerScrip=RowsPerStrip;

    fseek(fp,StripOffset,SEEK_SET);
    for (i=0;i<myScripPerImage;i++)
    {
    myScripOffset[i]=0L;
    fread(&myScripOffset[i],myType,1,fp);
    }

    fseek(fp,StripByteCounts,SEEK_SET);
    for (i=0;i<myScripPerImage;i++)
    {
    myScripBytes[i]=0L;
    fread(&myScripBytes[i],myType1,1,fp);
    }

  }
  ////////////////////////add end 10.10.96///////////////////////////////////
  if(Xres && Yres) {
         ULONG t;
         fseek(fp,Xres,SEEK_SET);
         fread(&Xres,4,1,fp);
         fread(&t,4,1,fp);
         if(t) Xres/=t;

         fseek(fp,Yres,SEEK_SET);
         fread(&Yres,4,1,fp);
         fread(&t,4,1,fp);
         if(t) Yres/=t;
  }

  /*
  if(!Xres)           //By zjh for screen
    Xres=ImageWidth;
  if(!Yres)
    Yres=ImageHeight;
  */

  if(!Xres||!Yres||(Xres==ImageWidth&&Yres==ImageHeight)||IsScreenPic(Xres,Yres))           //By zjh for screen
   {
    Xres=SCRDPI;
    Yres=SCRDPI;
   }

  XRate=(float)DeviceDPI/(float)Xres;
  YRate=(float)DeviceDPI/(float)Yres;
  if (ResolutionUnit==3)                // resolution uint is cm
  {                             // 1 inch = 2.54 cm, so Rate must / (1/2.54)
     XRate/=2.54;                      /* cm to Inch */
     YRate/=2.54;
  }
  PICSCALE();

  if(RGBPalDataLen) {
       UCHAR RGBPalette[768*2];

       TiffPresent->RGBPaletteSign=1;
       fseek(fp,RGBPaletteOffset,SEEK_SET);
       if (fread(RGBPalette,sizeof(short)*RGBPalDataLen,1,fp)!=1)
          return(FILEREAD);
       ConvertMMII((UINT *)RGBPalette,sizeof(short)*RGBPalDataLen,TiffSign);

       if(Photomet==3 && BitsPerPixel<=8)
       {
         /*  RRR...R GGG...G BBB...B
             |_____| |_____| |_____|
             each length=RGBPalDataLen/3
          */
          int j;
          RGBPalDataLen=2*(1<<BitsPerPixel);
          for (i=j=0;i<RGBPalDataLen;i+=2,j++) {
            TiffPresent->ByteRGBPalette[3*j]=RGBPalette[i+1];
            TiffPresent->ByteRGBPalette[3*j+1]=RGBPalette[RGBPalDataLen+i+1];
            TiffPresent->ByteRGBPalette[3*j+2]=RGBPalette[2*RGBPalDataLen+i+1];
          }  // for i ...
       }   // Photomet==3
       else
          memcpy(TiffPresent->ByteRGBPalette,RGBPalette,768);
  }  /* if RGBoffset */

  switch(BitsPerPixel) {
    case 1:
         ImageSetBitColor(TiffPresent,IMAGEBLACKWHITE);
         break;
    case 4:
         if(TiffPresent->RGBPaletteSign)
             ImageSetBitColor(TiffPresent,IMAGE16COLOR);
         else
             ImageSetBitColor(TiffPresent,IMAGE16GRAY);
         break;
    case 8:
         if(TiffPresent->RGBPaletteSign)
             ImageSetBitColor(TiffPresent,IMAGE256COLOR);
         else
             ImageSetBitColor(TiffPresent,IMAGE256GRAY);
         break;
    case 24:
         ImageSetBitColor(TiffPresent,IMAGETRUECOLOR);
         break;
   } // switch


  if (PutSign==PUTDITHERDEVICE)
  {
      TiffPresent->ImageWidth =(float)ImageWidth*XRate+0.5;
      TiffPresent->ImageHeight=(float)ImageHeight*YRate+0.5;
      /*-----------
      TiffPresent->ImageWidth =(float)ImageWidth*XRate+1;
      TiffPresent->ImageHeight=(float)ImageHeight*YRate+1;
      switch (BitsPerPixel)
      {
        case 1:
             TiffPresent->ImageWidth=((TiffPresent->ImageWidth+7)>>3)<<3;
             break;
        case 4:
             TiffPresent->ImageWidth=((TiffPresent->ImageWidth+1)>>1)<<1;
             break;
      }

     BufferLen=(long)(TiffPresent->ImageWidth+DeviceColorBits-1)/DeviceColorBits
                    *TiffPresent->ImageHeight;
     --------------------------*/

     BufferLen=(long)(TiffPresent->ImageWidth+7)/8
                    *TiffPresent->ImageHeight*DeviceColorBits;
     TiffPresent->ImageHandle=HandleAlloc(BufferLen,0);
     if (TiffPresent->ImageHandle==0)
        return(OUTOFMEMORY);

     PresentImageBuffer=HandleLock(TiffPresent->ImageHandle);
     if (PresentImageBuffer==NULL)
     {
        HandleFree(TiffPresent->ImageHandle);
        return(OUTOFMEMORY);
     }

     memset(PresentImageBuffer,0,BufferLen);
     pImageBuf=PresentImageBuffer;
  }
  else
  {                           // PutSign==PUTTRUECOLORDEVICE
      switch (BitsPerPixel)
      {
        case 1:
             BufferLen=(long)((TiffPresent->ImageWidth+7)>>3)*TiffPresent->ImageHeight;
             break;
        case 4:
             BufferLen=(long)((TiffPresent->ImageWidth+1)>>1)*TiffPresent->ImageHeight;
             break;
        case 8:
             BufferLen=(long)TiffPresent->ImageWidth*TiffPresent->ImageHeight;
             break;
        case 24:
         /*----------------------
             i=32768l/(TiffPresent->ImageWidth*3);
             ImageSegmentLength=i*(TiffPresent->ImageWidth*3);
             TotalImageSegment=TiffPresent->ImageHeight/i+1;
             for (i=0;i<TotalImageSegment;i++)
                 ImageSegmentHandle[i]=HandleAlloc(ImageSegmentLength,0);
         ----------------*/
             BufferLen=(long)TiffPresent->ImageWidth*TiffPresent->ImageHeight*3;
             break;
         default:           // not support
             return(SOURCECOMPRESS);
      }  // switch

      TmpHandle=HandleAlloc(BufferLen,0);
      if (TmpHandle==0)
            goto lock_err_1;

      Tmp24ImageBuffer=HandleLock(TmpHandle);
      if (Tmp24ImageBuffer==NULL)
      {
         HandleFree(TmpHandle);
      lock_err_1:
         HandleUnlock(TiffPresent->ImageHandle);
         HandleFree(TiffPresent->ImageHandle);
         TmpHandle=0;
         return(OUTOFMEMORY);
      }

      memset(Tmp24ImageBuffer,0,BufferLen);
      pImageBuf=Tmp24ImageBuffer;
  }

/*-----------------------------
  if (BitsPerPixel>1&&BitsPerPixel<24&&!TiffPresent->RGBPaletteSign)
  {
     int ppow;

     for (i=0,ppow=1<<BitsPerPixel;i<ppow;i+=3)
     {
        GetRGBPalette(i,&(TiffPresent->ByteRGBPalette[i]),
                        &(TiffPresent->ByteRGBPalette[i+1]),
                        &(TiffPresent->ByteRGBPalette[i+2]));
        TiffPresent->ByteRGBPalette[i]<<=2;
        TiffPresent->ByteRGBPalette[i+1]<<=2;
        TiffPresent->ByteRGBPalette[i+2]<<=2;
     }
  }
------------------------------*/

  BytesPerLine=(ImageWidth*BitsPerPixel+7)/8;

   /* Set PhotometFlag */

  //if(!Photomet)  BitsPerPixel|=0x8000;          // 96.12.05 Reverse
  //delete By zjh 12.21/96  for Black and white pic

/*--------- 5: process Tiff Data(color) ------*/
  if (RowsPerStrip==0xffff) { StripNum=1; RowsPerStrip=ImageHeight; }
  else StripNum=(ImageHeight+RowsPerStrip-1)/RowsPerStrip;

 /*-------- 5.1 : process one Strip ------*/
  if(StripNum==1) PerStripOffset=StripOffset;

  YFrom=0; deltaY=0;
  for(i=0;i<StripNum;i++) {
       if(StripNum!=1) {    /* StripOffset ==> table of PerStripOffset */
            fseek(fp,StripOffset+(long)i*sizeof(PerStripOffset),SEEK_SET);
            fread(&PerStripOffset,sizeof(PerStripOffset),1,fp);
            ConvertMMII((UINT *)&PerStripOffset,sizeof(PerStripOffset),TiffSign);
       }
       fseek(fp,PerStripOffset,SEEK_SET); /* file pointer ==> PerStripData */

 /*-------- 5.2 : process one Line (not compressed) ------*/
       if(RowsPerStrip>ImageHeight) RowsPerStrip=ImageHeight;

       { int result=DrawImage(fp,pImageBuf,YFrom,ImageWidth,RowsPerStrip,BytesPerLine,
               XRate,YRate,BitsPerPixel,TiffPresent,PutSign,Compression);
         if(result<0)
             break;
         YFrom+=result;
       }
       ImageHeight-=(int)RowsPerStrip;   /* Rows left */
  } /*-- i --*/

   /*------- clear left rows with white color ---------*/
  if(YFrom<TiffPresent->ImageHeight)
    ClearBlockTail(pImageBuf,0,YFrom,TiffPresent->ImageWidth-1,
       TiffPresent->ImageHeight-1,TiffPresent,PutSign,BitsPerPixel);
       /*-------------*/

//  if (BitsPerPixel>1&&BitsPerPixel<24)  TiffPresent->RGBPaletteSign=1;

  if (PutSign==PUTDITHERDEVICE)
  {
     HandleUnlock(TiffPresent->ImageHandle);
  }
  else
  {
     *ImageH24Data=TmpHandle;
     HandleUnlock(TmpHandle);
  }

  ReturnOK();
}

int TiffToPresent(char *FileName,ImageDescribes *TiffPresent,char PutSign,
                  int DeviceDPI,int DeviceColorBits,HANDLE *ImageH24Data)
{
  int Result;
  FILE *fp;
  TIFFHeadType TIFFHead;

  if ((fp=fopen(FileName,"rb"))==NULL) {
       Result=FILEOPEN; goto tiff_err_exit;
  }

  if (fread(&TIFFHead,sizeof(TIFFHead),1,fp)!=1) {
       Result=FILEREAD; goto tiff_err_exit;
  }

  ConvertMMII(&(((UINT *)&TIFFHead)[1]),6,TIFFHead.Sign);

  if (  (TIFFHead.Sign!=0x4949)
      ||(TIFFHead.Version!=42) )  {
       Result=FILEFORMAT; goto tiff_err_exit;
  }

  if (PutSign==PUTDITHERDEVICE)
  {
     if (TiffPresent->ImageHandle)
     {
        HandleFree(TiffPresent->ImageHandle);
        TiffPresent->ImageHandle=0;
     }
     memset(TiffPresent,0,sizeof(*TiffPresent));
     TiffPresent->ImageScaleX=TiffPresent->ImageScaleY=1;
  }

  Result=DealIFD(fp,TIFFHead.FirstIFDOffset,TIFFHead.Sign,TiffPresent,PutSign,
                 DeviceDPI,DeviceColorBits,ImageH24Data);

tiff_err_exit:
  if(fp) fclose(fp);
  return(Result);
}

static int PcxToPresent(char *FileName,ImageDescribes *TiffPresent,char PutSign,
                     int DeviceDPI,int DeviceColorBits,HANDLE *ImageH24Data)
{
  int Result;
  FILE *fp;
  USHORT ImageHeight,ImageWidth;
  PCXHeadType PcxHead;
  float XRate,YRate;
  long  BufferLen;
  unsigned char FHUGE *PresentImageBuffer, FHUGE *pImageBuf;
  unsigned char FHUGE *Tmp24ImageBuffer;
  HANDLE TmpHandle;
  UINT flag_p=0;        //By zjh

  if ((fp=fopen(FileName,"rb"))==NULL) {
       Result=FILEOPEN; goto pcx_err_exit;
  }

  if (fread(&PcxHead,sizeof(PcxHead),1,fp)!=1) {
       Result=FILEREAD; goto pcx_err_exit;
  }

 /*--------- Only support 1 planes( White&Black or 256Color )----------*/
  if (PcxHead.Sign!=0xa) {
       Result=FILEFORMAT; goto pcx_err_exit;
  }
  if(PcxHead.Planes==4 && PcxHead.BitsPerPixel!=1) {
       Result=FILEFORMAT; goto pcx_err_exit;
  }

  ImageWidth=(PcxHead.Xmax-PcxHead.Xmin)+1;
  ImageHeight=(PcxHead.Ymax-PcxHead.Ymin)+1;
  //if(PcxHead.Hres==0) PcxHead.Hres=ImageWidth;
  //if(PcxHead.Vres==0) PcxHead.Vres=ImageHeight;
  if(PcxHead.Hres==0||PcxHead.Vres==0||
    (PcxHead.Hres==ImageWidth&&PcxHead.Vres==ImageHeight)||
    IsScreenPic(PcxHead.Hres,PcxHead.Vres))     //By zjh for screen pic
    {
     PcxHead.Hres=SCRDPI;
     PcxHead.Vres=SCRDPI;
    }

  XRate=(float)DeviceDPI/PcxHead.Hres;
  YRate=(float)DeviceDPI/PcxHead.Vres;
  PICSCALE();


  if(PcxHead.BitsPerPixel==1 && PcxHead.Planes==4)
  {
     PcxHead.BitsPerPixel=4;
     PcxHead.PcxCompress|=0x40;
  }

  if (PutSign==PUTDITHERDEVICE)
  {
     TiffPresent->RGBPaletteSign=1;      /* PCX always has ColorPaletteValue */
     /*------- 1: get  Palette ---------*/
     switch (PcxHead.BitsPerPixel) {
          case 1:
               ImageSetBitColor(TiffPresent,IMAGEBLACKWHITE);
               break;   /* Black & White Image */
          case 8:      /* 256 color */
              fseek(fp,-769L,2);      /* SEEK_END */
              if(fgetc(fp)!=0xc) {      /* tag must be 0xc */
                    Result=FILEFORMAT; goto pcx_err_exit;
               }

           /*-------- read 256*3(R,G,B) bytes from file tail -----*/
              if (fread(TiffPresent->ByteRGBPalette,768,1,fp)!=1) {
                   Result=FILEFORMAT; goto pcx_err_exit;
              }
              ImageSetBitColor(TiffPresent,IMAGE256COLOR);
              fseek(fp,0x80,0);       /* SEEK_SET, file pointer ==> data */
              break;
          default:   /* color<=16, using Head.Palette */
              memcpy(TiffPresent->ByteRGBPalette,PcxHead.Palette,3*16);
              ImageSetBitColor(TiffPresent,IMAGE16COLOR);
              break;
     }    /*- switch BitsPerPixel -*/

     TiffPresent->ImageWidth =(float)ImageWidth*XRate+0.5;
     TiffPresent->ImageHeight=(float)ImageHeight*YRate+0.5;
     BufferLen=(long)(TiffPresent->ImageWidth+7)/8
                    *TiffPresent->ImageHeight*DeviceColorBits;
     TiffPresent->ImageHandle=HandleAlloc(BufferLen,0);
     if (TiffPresent->ImageHandle==0)
        return(OUTOFMEMORY);

     PresentImageBuffer=HandleLock(TiffPresent->ImageHandle);
     if (PresentImageBuffer==NULL)
     {
        HandleFree(TiffPresent->ImageHandle);
        return(OUTOFMEMORY);
     }

     memset(PresentImageBuffer,0,BufferLen);
     pImageBuf=PresentImageBuffer;
  }
  else
  {                           // PutSign==PUTTRUECOLORDEVICE
     switch (PcxHead.BitsPerPixel) {
          case 1:
              BufferLen=(long)((TiffPresent->ImageWidth+7)>>3)*TiffPresent->ImageHeight;
              break;   /* Black & White Image */
          case 8:      /* 256 color */
              BufferLen=(long)TiffPresent->ImageWidth*TiffPresent->ImageHeight;
              break;
          default:   /* color<=16, using Head.Palette */
              BufferLen=(long)((TiffPresent->ImageWidth+1)>>1)*TiffPresent->ImageHeight;
              break;
     }    /*- switch BitsPerPixel -*/

      TmpHandle=HandleAlloc(BufferLen,0);
      if (TmpHandle==0)
            goto lock_err_1;

      Tmp24ImageBuffer=HandleLock(TmpHandle);
      if (Tmp24ImageBuffer==NULL)
      {
         HandleFree(TmpHandle);
      lock_err_1:
         HandleUnlock(TiffPresent->ImageHandle);
         HandleFree(TiffPresent->ImageHandle);
         TmpHandle=0;
         return(OUTOFMEMORY);
      }

      memset(Tmp24ImageBuffer,0,BufferLen);
      pImageBuf=Tmp24ImageBuffer;
  }

  deltaY=0;

  if (PcxHead.BitsPerPixel==1)  flag_p=0x8000;        //By zjh 96.10.11
  flag_p |=(PcxHead.BitsPerPixel&0xff);   //By zjh for Blank and White 96.10.11
  //DrawImage(fp,pImageBuf,0,ImageWidth,ImageHeight,PcxHead.BytesPerLine,
  //      XRate,YRate,PcxHead.BitsPerPixel,TiffPresent,PutSign,PcxHead.PcxCompress);
  DrawImage(fp,pImageBuf,0,ImageWidth,ImageHeight,PcxHead.BytesPerLine,
        XRate,YRate,flag_p,TiffPresent,PutSign,PcxHead.PcxCompress);

  if (PutSign==PUTDITHERDEVICE)
  {
     HandleUnlock(TiffPresent->ImageHandle);
  }
  else
  {
     *ImageH24Data=TmpHandle;
     HandleUnlock(TmpHandle);
  }

  Result=OpOK;
pcx_err_exit:
  if(fp) fclose(fp);
  return(Result);
} /* PcxToPresent */


int BmpToPresent(char *FileName,ImageDescribes *TiffPresent,char PutSign,
             int DeviceDPI,int DeviceColorBits,HANDLE *ImageH24Data)
{
  long  BufferLen;
  unsigned char FHUGE *PresentImageBuffer, FHUGE *pImageBuf;
  unsigned char FHUGE *Tmp24ImageBuffer;
  HANDLE TmpHandle;
  int Result;
  USHORT PalNum,i,XRes,YRes;
  FILE *fp;
  USHORT ImageWidth,ImageHeight;
  USHORT BytesPerLine,R,G,B;
  float XRate,YRate;

  BMPFileHead BMPHead;
  BMPInfoHead BMPInfo;
  UCHAR MyBuf[2048];

  if ((fp=fopen(FileName,"rb"))==NULL) {
       Result=FILEOPEN; goto bmp_err_exit;
  }

/*----- 1: read File head, compare "BM" tag ---------*/
  if (fread(&BMPHead,sizeof(BMPHead),1,fp) != 1) {
       Result=FILEREAD; goto bmp_err_exit;
  }
  if(BMPHead.Type!=0x4d42) {
       Result=FILEFORMAT; goto bmp_err_exit;
  }

/*----- 2: read File Information head, compare Compression tag ---------*/
  if (fread(&BMPInfo,sizeof(BMPInfo),1,fp) != 1 ) {
       Result=FILEREAD; goto bmp_err_exit;
  }

  /*----- don't support Compressed BMP or planes>1 -----*/
  if(BMPInfo.Compression!=0 || BMPInfo.Planes!=1) {
       Result=FILEFORMAT; goto bmp_err_exit;
  }

  ImageWidth=BMPInfo.ImageWidth;
  ImageHeight=BMPInfo.ImageHeight;

  XRes=BMPInfo.XResolution/100*2.54+0.5;
  YRes=BMPInfo.YResolution/100*2.54+0.5;

  if(!XRes||!YRes||(XRes==ImageWidth&&YRes==ImageHeight)||IsScreenPic(XRes,YRes))           //By zjh for screen
   {
    XRes=SCRDPI;
    YRes=SCRDPI;
   }

  /*
  if (XRes==0||YRes==0)
  {
    XRes=YRes=SCRDPI;
  }*/

  XRate=(float)DeviceDPI/XRes;
  YRate=(float)DeviceDPI/YRes;
  PICSCALE();

/*----- 3: read Palette ---------*/
  PalNum=(DWORD)BMPHead.ImageOffset-sizeof(BMPHead)-sizeof(BMPInfo);
  PalNum/=4;          /* 4 bytes per RGBPalette */
  if(PalNum>256) PalNum=256;

  if(PalNum) {
       fread(MyBuf,4,PalNum,fp);
       TiffPresent->RGBPaletteSign=1;
   } else
       //TiffPresent->RGBPaletteSign=0;
       TiffPresent->RGBPaletteSign=1;       //Ture color;

  for(i=0;i<PalNum;i++) {
       /*------ change to Gray Value -------*/
       B=MyBuf[4*i];
       G=MyBuf[4*i+1];
       R=MyBuf[4*i+2];
       //TiffPresent->ByteRGBPalette[i]=(R*30+G*59+B*11)/100;
       TiffPresent->ByteRGBPalette[i*3]=R;
       TiffPresent->ByteRGBPalette[i*3+1]=G;
       TiffPresent->ByteRGBPalette[i*3+2]=B;
  }  // for i ...

  /*----- 4: read color data (the first line data is at FileTail ---------*/
  fseek(fp,0L,2);   /* SEEK_END */

  BytesPerLine=(ftell(fp)-BMPHead.ImageOffset) / ImageHeight;
  fseek(fp,0L-BytesPerLine,SEEK_END);   /* SEEK_END */

//////////////////////////////add By zjh////////////////////////
  if (PutSign==PUTDITHERDEVICE)
  {
     /*------- 1: get  Palette ---------*/
     switch (BMPInfo.BitsPerPixel) {
          case 1:
              ImageSetBitColor(TiffPresent,IMAGEBLACKWHITE);
              break;   /* Black & White Image */
          case 8:      /* 256 color */
              ImageSetBitColor(TiffPresent,IMAGE256COLOR);
              break;
          case 24:
              ImageSetBitColor(TiffPresent,IMAGETRUECOLOR);
              break;

          default:   /* color<=16, using Head.Palette */
              ImageSetBitColor(TiffPresent,IMAGE16COLOR);
              break;
     }    /*- switch BitsPerPixel -*/

     TiffPresent->ImageWidth =(float)ImageWidth*XRate+0.5;
     TiffPresent->ImageHeight=(float)ImageHeight*YRate+0.5;
     BufferLen=(long)(TiffPresent->ImageWidth+7)/8
                    *TiffPresent->ImageHeight*DeviceColorBits;
     TiffPresent->ImageHandle=HandleAlloc(BufferLen,0);
     if (TiffPresent->ImageHandle==0)
        return(OUTOFMEMORY);

     PresentImageBuffer=HandleLock(TiffPresent->ImageHandle);
     if (PresentImageBuffer==NULL)
     {
        HandleFree(TiffPresent->ImageHandle);
        return(OUTOFMEMORY);
     }

     memset(PresentImageBuffer,0,BufferLen);
     pImageBuf=PresentImageBuffer;
  }
  else
  {                           // PutSign==PUTTRUECOLORDEVICE
     switch (BMPInfo.BitsPerPixel) {
          case 1:
              BufferLen=(long)((TiffPresent->ImageWidth+7)>>3)*TiffPresent->ImageHeight;
              break;   /* Black & White Image */
          case 8:      /* 256 color */
              BufferLen=(long)TiffPresent->ImageWidth*TiffPresent->ImageHeight;
              break;
          case 24:      /* TRUE color */
              BufferLen=(long)TiffPresent->ImageWidth*TiffPresent->ImageHeight*3;
              break;
          default:   /* color<=16, using Head.Palette */
              BufferLen=(long)((TiffPresent->ImageWidth+1)>>1)*TiffPresent->ImageHeight;
              break;
     }    /*- switch BitsPerPixel -*/

      TmpHandle=HandleAlloc(BufferLen,0);
      if (TmpHandle==0)
            goto lock_err_1;

      Tmp24ImageBuffer=HandleLock(TmpHandle);
      if (Tmp24ImageBuffer==NULL)
      {
         HandleFree(TmpHandle);
      lock_err_1:
         HandleUnlock(TiffPresent->ImageHandle);
         HandleFree(TiffPresent->ImageHandle);
         TmpHandle=0;
         return(OUTOFMEMORY);
      }

      memset(Tmp24ImageBuffer,0,BufferLen);
      pImageBuf=Tmp24ImageBuffer;
  }

//////////////////////////////end By zjh///////////////////////



  deltaY=0;
  DrawImage(fp,pImageBuf,0,ImageWidth,ImageHeight,BytesPerLine,
        XRate,YRate,BMPInfo.BitsPerPixel,TiffPresent,PutSign,0x80|(USHORT)BMPInfo.Compression);

  if (PutSign==PUTDITHERDEVICE)
  {
     HandleUnlock(TiffPresent->ImageHandle);
  }
  else
  {
     *ImageH24Data=TmpHandle;
     HandleUnlock(TmpHandle);
  }

  Result=OpOK;

bmp_err_exit:
  if(fp) fclose(fp);
  return(Result);
} /* BmpToPresent */

int PictureToPresent(char *FileName,ImageDescribes *TiffPresent,char PutSign,
                  int DeviceDPI,int DeviceColorBits,HANDLE *ImageH24Data)
{
     int i;
     int (*fun)(char *FileName,ImageDescribes *TiffPresent,char PutSign,
                 int DeviceDPI,int DeviceColorBits,HANDLE *ImageH24Data);

     i=0;
     while(FileName[i]) i++;     /* find Name_str_len */

     if(FileName[i-4]!='.')
        return FILEFORMAT;

     switch (toupper(FileName[i-3]) ) {
          case 'T':
           fun=TiffToPresent;
           break;
          case 'P':
           fun=PcxToPresent;
           break;
          case 'B':
           fun=BmpToPresent;
           break;
         default:
           return FILEFORMAT;
     }

     if (PutSign==PUTDITHERDEVICE)
     {
        if (TiffPresent->ImageHandle)
        {
           HandleFree(TiffPresent->ImageHandle);
           TiffPresent->ImageHandle=0;
        }
        memset(TiffPresent,0,sizeof(*TiffPresent));
        TiffPresent->ImageScaleX=TiffPresent->ImageScaleY=1;
     }

     return (*fun)(FileName,TiffPresent,PutSign,DeviceDPI,DeviceColorBits,ImageH24Data);
}

