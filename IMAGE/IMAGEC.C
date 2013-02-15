#define  _SCREENGRAPH_
/*-------------------------------------------------------------------
* Name: imagec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define NewImageScaleX ((!PrintingSign) ? (TiffPresent->ImageScaleX*SCRX):(TiffPresent->ImageScaleX*PRNX) )
#define NewImageScaleY ((!PrintingSign) ? (TiffPresent->ImageScaleY*SCRY):(TiffPresent->ImageScaleY*PRNY) )
#define NewSCRX (1)
#define NewSCRY (1)
//#define NewImageScaleX (TiffPresent->ImageScaleX)
//#define NewImageScaleY (TiffPresent->ImageScaleY)
//#define NewSCRX ((!PrintingSign) ? (SCRX):(PRNX) )
//#define NewSCRY ((!PrintingSign) ? (SCRY):(PRNY) )


static short PutImageLineLen;
int FindRowFillData(int Row,short *buff);
void PointSkew(int *NewX,int *NewY,int OldX,int OldY,
               int SkewAxisX,int SkewAxisY,int SkewAngle)
{
  FIXED TmpFixed1,TmpFixed2,TmpFixed3;

  Long2Fixed(TmpFixed1,LSin(SkewAngle));
  Int2Fixed(TmpFixed2,(OldY-SkewAxisY));
  FixedMul(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  Int2Fixed(TmpFixed2,(OldX-SkewAxisX));
  FixedAdd(&TmpFixed3,&TmpFixed2,&TmpFixed1);
  *NewX=IntofFixed(TmpFixed1)+SkewAxisX;
  *NewY=OldY;
}

#ifdef UNUSED           // ByHance, 96,1.29
static void ImageSetNewContrast(ImageDescribes *TiffPresent,unsigned char NewConstrast)
{
  if (NewConstrast>2||ImageGetBitColor(TiffPresent)==IMAGE16COLOR
  ||ImageGetBitColor(TiffPresent)==IMAGE256COLOR
  ||ImageGetBitColor(TiffPresent)==IMAGETRUECOLOR)
      return;

  ImageSetContrast(TiffPresent,NewConstrast);
}

int GetPosterizedGray(int Color)
{
  int i;

  for (i=0;i<MAXPOSTERIZEDCOLOR;i++)
      if (Color<=PosterizedGray[i])
         return(PosterizedGray[i]);
  return(PosterizedGray[MAXPOSTERIZEDCOLOR-1]);
}

static void GetMinMaxGray(unsigned char *ImageData,char GrayBit,
                          int ImageWidth,int ImageHeight,
                          unsigned char *MinGray,unsigned char *MaxGray)
{
  int i,j,Address;
  unsigned char BitSign,ReadGray;

  *MinGray=255;
  *MaxGray=0;

  for (i=0,Address=0;i<ImageHeight;i++)
  {
      BitSign=0;
      for (j=0;j<ImageWidth;j++)
      {
          ReadGray=ImageData[Address];
          if (GrayBit==4)
          {
             if (BitSign)
             {
                BitSign=0;
                Address++;
                ReadGray&=0xf;
             }
             else
             {
                BitSign=1;
                ReadGray>>=4;
             }
          }
          else
             Address++;
          if (ReadGray>*MaxGray)
             *MaxGray=ReadGray;
          if (ReadGray<*MinGray)
             *MinGray=ReadGray;
      }
  }
  return;
}
#endif     // UNUSED           // ByHance, 96,1.29

static void FillBoxClipArea(HBOX HBox)
{
  LineFillLine *SaveLineFillLine;
  EdgeFillLine *SaveEdgeFillLine;
  ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];
#ifdef   OLD_VERSION    // ByHance, 96,4.4
  ORDINATETYPE Dots[2*MAXPOLYGONNUMBER];
  int i,Sum;
  int Polygons;
  int *Edges;
#endif
  int BoxDots;
  Boxs *Box;

  if(HBox<=0) return;
  Box=HandleLock(ItemGetHandle(HBox));
  if (Box==NULL)
     return;

#ifdef   OLD_VERSION    // ByHance, 96,4.4
  Polygons=PictureBoxGetClipPolygons(Box);
  Edges=PictureBoxGetClipEdges(Box);
  for (i=Sum=0;i<Polygons;i++)
      Sum+=Edges[i];
  memcpy(Dots,PictureBoxGetClipBoxXY(Box),Sum*2*sizeof(ORDINATETYPE));
  BoxPolygonToWindowXY(Sum,Dots);
#endif

  BoxGetPolygonDrawBorder(Box,&BoxDots,BoxXY);
  BoxPolygonToWindowXY(BoxDots,BoxXY);
  HandleUnlock(ItemGetHandle(HBox));

  if (GlobalRorate90&&PrintingSign)
  memset(ImageClipData,0,(myDC.right-myDC.left+8)/8*(myDC.bottom-myDC.top));
  else
  memset(ImageClipData,0,RastWidthByte*(SysDc.bottom-SysDc.top));

  SaveLineFillLine=CurrentLineFillLine;
  SaveEdgeFillLine=CurrentEdgeFillLine;

  CurrentLineFillLine=SetImageNoClipLineFill;
  CurrentEdgeFillLine=NULL;

  FillPolygon(&SysDc,(LPPOINT)BoxXY,BoxDots);

#ifdef   OLD_VERSION    // ByHance, 96,4.4
  if (Polygons>0)
  {
     CurrentLineFillLine=SetImageClipLineFill;
     PolyFillPolygon(&SysDc,(LPPOINT)Dots,Edges,Polygons);
  }
#endif

  CurrentLineFillLine=SaveLineFillLine;
  CurrentEdgeFillLine=SaveEdgeFillLine;
}

static int ImageDrawVideo(ImageDescribes *TiffPresent,
                   int ImagePosX,int ImagePosY, float Scale,HBOX ImageHBox)
{
  int Edges[8];
  HANDLE ImageHClip,ImageH24Data;
  MAT2 ConvertMatrix;
  int i;
  float ScaleX,ScaleY;
  MAT2 SkewMatrix,RotateMatrix,ZoomMatrix;
  int TmpX,TmpY;
  int Left,Top,Right,Bottom;
  PictureBoxs *MidPBox;
  char PictureFileName[67];
  LineFillLine *SaveLineFillLine;
  EdgeFillLine *SaveEdgeFillLine;

  ScaleX=Scale*NewImageScaleX;
  ScaleY=Scale*NewImageScaleY;

  ImageAxisX=TiffPresent->ImageWidth/2;
  ImageAxisY=TiffPresent->ImageHeight/2;
  ZoomImageAxisX=ImageAxisX*ScaleX;
  ZoomImageAxisY=ImageAxisY*ScaleY;
  ImageOriginX=UserXToWindowX(TiffPresent->ImageOriginX+ImagePosX);//+(ScaleX-1)*(ImageAxisX);
  ImageOriginY=UserYToWindowY(TiffPresent->ImageOriginY+ImagePosY);//+(ScaleY-1)*(ImageAxisY);

  GetSkewMatrix2(&SkewMatrix,TiffPresent->ImageSkewAngle);
  GetRotateMatrix2(&RotateMatrix,TiffPresent->ImageRotateAngle);
  GetZoomMatrix2(&ZoomMatrix,ScaleX,ScaleY);
  //MAT2Mul(&ZoomMatrix,&SkewMatrix,&ConvertMatrix);
  //MAT2Mul(&ConvertMatrix,&RotateMatrix,&ConvertMatrix);
  MAT2Mul(&SkewMatrix,&RotateMatrix,&ConvertMatrix);
  MAT2Mul(&ConvertMatrix,&ZoomMatrix,&ConvertMatrix);

  Edges[0]=0;                        // Compute Border of image
  Edges[1]=0;
  Edges[2]=TiffPresent->ImageWidth;
  Edges[3]=0;
  Edges[4]=TiffPresent->ImageWidth;
  Edges[5]=TiffPresent->ImageHeight;
  Edges[6]=0;
  Edges[7]=TiffPresent->ImageHeight;

  for (i=0;i<4;i++)
  {
      TmpX=Edges[2*i]-ImageAxisX;    // Skew & Rotate round (ImageAxisX,ImageAxisY)
      TmpY=Edges[2*i+1]-ImageAxisY;
      Matrix2ConvertPoint(&ConvertMatrix,&TmpX,&TmpY);
      Edges[2*i]=TmpX+ZoomImageAxisX;
      Edges[2*i]+=ImageOriginX;      // Move X & Y
      Edges[2*i+1]=TmpY+ZoomImageAxisY;
      Edges[2*i+1]+=ImageOriginY;
  }

  PolygonGetMinRectangle(4,Edges,&Left,&Top,&Right,&Bottom);

  if (GlobalRorate90&&PrintingSign)
  {
  if (!RectangleIsInRectangle(myDC.left,myDC.top,myDC.right,myDC.bottom,
       Left,Top,Right,Bottom))
     ReturnOK();
  ImageHClip=HandleAlloc((LONG)(myDC.right-myDC.left+8)/8*(myDC.bottom-myDC.top),0);
  }
  else
  {
  if (!RectangleIsInRectangle(SysDc.left,SysDc.top,SysDc.right,SysDc.bottom,
       Left,Top,Right,Bottom))
     ReturnOK();
  ImageHClip=HandleAlloc((LONG)RastWidthByte*(SysDc.bottom-SysDc.top),0);
  }

  if (ImageHClip==0)
     return(OUTOFMEMORY);
  ImageClipData=HandleLock(ImageHClip);
  if (ImageClipData==NULL)
  {
     HandleFree(ImageHClip);
     return(OUTOFMEMORY);
  }

  FillBoxClipArea(ImageHBox);

  GetSkewMatrix2(&SkewMatrix,(-1)*TiffPresent->ImageSkewAngle);
  GetRotateMatrix2(&RotateMatrix,(-1)*TiffPresent->ImageRotateAngle);
  GetZoomMatrix2(&ZoomMatrix,1./ScaleX,1./ScaleY);
  //MAT2Mul(&RotateMatrix,&SkewMatrix,&ConvertMatrix);
  //MAT2Mul(&ConvertMatrix,&ZoomMatrix,&ConvertMatrix);
  MAT2Mul(&ZoomMatrix,&RotateMatrix,&ConvertMatrix);
  MAT2Mul(&ConvertMatrix,&SkewMatrix,&ConvertMatrix);
  ImageMatrix=&ConvertMatrix;

  if (ImageGetNegative(TiffPresent))   // Negative
     NegativeSign=1;
  else
     NegativeSign=0;

  if (ImageGetBitColor(TiffPresent)==IMAGEBLACKWHITE&&TiffPresent->ImageColor)
     ColorSign=TiffPresent->ImageColor;// B&W image colorized
  else
     ColorSign=0;

  MidPBox=HandleLock(ItemGetHandle(ImageHBox));
  if (MidPBox==NULL)
     return(OUTOFMEMORY);
  strcpy(PictureFileName,PictureBoxGetPictureFileName(MidPBox));
  HandleUnlock(ItemGetHandle(ImageHBox));

  if (PictureToPresent(PictureFileName,TiffPresent,PUTTRUECOLORDEVICE,
                    screendpi,4,&ImageH24Data)<OpOK||ImageH24Data==0)
     return(OUTOFMEMORY);
  else
  {
     if ((ImageData=HandleLock(ImageH24Data))==NULL)
     {
        HandleFree(ImageH24Data);
        return(OUTOFMEMORY);
     }
  }

#ifdef UNUSED           // ByHance, 96,1.29
  ContrastSign=ImageGetContrast(TiffPresent);
  if (ContrastSign==HIGHCONTRAST)
  {
     unsigned char MinGray,MaxGray;

     switch ( ImageGetBitColor(TiffPresent) )
     {
        case IMAGE16GRAY:
           GetMinMaxGray(ImageData,4,TiffPresent->ImageWidth,
                         TiffPresent->ImageHeight,&MinGray,&MaxGray);
           PosterizedGray[0]=MinGray+(MaxGray-MinGray)*HIGHCONTRASTCONST;
           PosterizedGray[1]=MaxGray;
           break;
        case IMAGE256GRAY:
           GetMinMaxGray(ImageData,8,TiffPresent->ImageWidth,
                      TiffPresent->ImageHeight,&MinGray,&MaxGray);
           PosterizedGray[0]=MinGray+(MaxGray-MinGray)*HIGHCONTRASTCONST;
           PosterizedGray[1]=MaxGray;
           break;
       default:
           ContrastSign=0;
           break;
     }  // switch
  }
  else
  if (ContrastSign==POSTERIZED)
  {
     unsigned char MinGray,MaxGray;

     if (ImageGetBitColor(TiffPresent)==IMAGE256GRAY)
     {
        GetMinMaxGray(ImageData,8,TiffPresent->ImageWidth,
                   TiffPresent->ImageHeight,&MinGray,&MaxGray);
        for (i=0;i<MAXPOSTERIZEDCOLOR;i++)
            PosterizedGray[i]=MinGray+((int)(MaxGray-MinGray))
                             *(i+1)/MAXPOSTERIZEDCOLOR;
     }
     else
        ContrastSign=0;
  }
#else
  ContrastSign=0;
#endif    // UNUSED           // ByHance, 96,1.29

  if (TiffPresent->RGBPaletteSign)
  {
     RGBPalatteSign=1;
     ByteRGBPalatte=TiffPresent->ByteRGBPalette;
  }
  else
     RGBPalatteSign=0;

  SaveLineFillLine=CurrentLineFillLine;
  SaveEdgeFillLine=CurrentEdgeFillLine;
  CurrentEdgeFillLine=NULL;

  switch (ImageGetBitColor(TiffPresent))
  {
    case IMAGEBLACKWHITE:
         RealWidth=(((TiffPresent->ImageWidth+7)>>3)<<3);
         CurrentLineFillLine=ImageDrawFillLineBW;
         break;
    case IMAGE16COLOR:
    case IMAGE16GRAY:
         RealWidth=(((TiffPresent->ImageWidth+1)>>1)<<1);
         CurrentLineFillLine=ImageDrawFillLine16;
         break;
    case IMAGE256COLOR:
    case IMAGE256GRAY:
         RealWidth=TiffPresent->ImageWidth;
         CurrentLineFillLine=ImageDrawFillLine256;
         break;
    case IMAGETRUECOLOR:
         RealWidth=TiffPresent->ImageWidth;
         CurrentLineFillLine=ImageDrawFillLineTrueColor;
         break;
    default:
         break;
  }

  FillPolygon(&SysDc,(LPPOINT)Edges,4);

  HandleUnlock(ImageHClip);
  HandleFree(ImageHClip);
  ImageClipData=NULL;
  HandleUnlock(ImageH24Data);
  HandleFree(ImageH24Data);

  CurrentLineFillLine=SaveLineFillLine;
  CurrentEdgeFillLine=SaveEdgeFillLine;

  ReturnOK();
}

int ImageGetNewHandle(ImageDescribes *TiffPresent)
{
  int Edges[8];
  EdgeFillLine *SaveEdgeFillLine;
  LineFillLine *SaveLineFillLine;
  DC dc;
  MAT2 ConvertMatrix;
  int i;
  MAT2 SkewMatrix,RotateMatrix;
  int TmpX,TmpY;
  int Left,Top,Right,Bottom;
  long BufferLen;

  RealWidth=((TiffPresent->ImageWidth+1)/2)*2;
  ImageAxisX=TiffPresent->ImageWidth/2;
  ImageAxisY=TiffPresent->ImageHeight/2;

  GetSkewMatrix2(&SkewMatrix,TiffPresent->ImageSkewAngle);
  GetRotateMatrix2(&RotateMatrix,TiffPresent->ImageRotateAngle);
  MAT2Mul(&SkewMatrix,&RotateMatrix,&ConvertMatrix);

  Edges[0]=0;
  Edges[1]=0;
  Edges[2]=TiffPresent->ImageWidth;
  Edges[3]=0;
  Edges[4]=TiffPresent->ImageWidth;
  Edges[5]=TiffPresent->ImageHeight;
  Edges[6]=0;
  Edges[7]=TiffPresent->ImageHeight;

  for (i=0;i<4;i++)
  {
      TmpX=Edges[2*i]-ImageAxisX;
      TmpY=Edges[2*i+1]-ImageAxisY;
      Matrix2ConvertPoint(&ConvertMatrix,&TmpX,&TmpY);
      Edges[2*i]=TmpX+ImageAxisX;
      Edges[2*i+1]=TmpY+ImageAxisY;
  }

  GetSkewMatrix2(&SkewMatrix,(-1)*TiffPresent->ImageSkewAngle);
  GetRotateMatrix2(&RotateMatrix,(-1)*TiffPresent->ImageRotateAngle);
  MAT2Mul(&RotateMatrix,&SkewMatrix,&ConvertMatrix);
  ImageMatrix=&ConvertMatrix;

  ImageData=HandleLock(TiffPresent->ImageHandle);
  if (ImageData==NULL)
     return(OUTOFMEMORY);
  /*
  {
    int j,OldColor;
    long Address;
    setviewport(0,0,639,479,0);

    for (i=0;i<TiffPresent->ImageHeight;i++)
    for (j=0;j<TiffPresent->ImageWidth;j++)
      {
       Address=((long)i*RealWidth+j)>>1;
       OldColor=ImageData[Address];
       if ( (j&1)!=0 )
       OldColor&=0xf;
        else
       OldColor=(OldColor>>4)&0xf;
       setcolor(OldColor);
       line(j,i,j,i);
      }
    getch();
  }
  */

  if (TiffPresent->ImageNewHandle)
     HandleFree(TiffPresent->ImageNewHandle);

  BufferLen=(long)(TiffPresent->ImageWidth+7)/8
                     *TiffPresent->ImageHeight;
  TiffPresent->ImageNewHandle=HandleAlloc(BufferLen,0);
  if (TiffPresent->ImageNewHandle==0)
  {
     HandleUnlock(TiffPresent->ImageHandle);
     return(OUTOFMEMORY);
  }

  ImageNewData=HandleLock(TiffPresent->ImageNewHandle);
  if (ImageNewData==NULL)
  {
     HandleFree(TiffPresent->ImageNewHandle);
     TiffPresent->ImageNewHandle=0;
     HandleUnlock(TiffPresent->ImageHandle);
     return(OUTOFMEMORY);
  }
  memset(ImageNewData,0,BufferLen);

  PolygonGetMinRectangle(4,(ORDINATETYPE *)Edges,(ORDINATETYPE *)&Left,
                         (ORDINATETYPE *)&Top,(ORDINATETYPE *)&Right,
                         (ORDINATETYPE *)&Bottom);

  dc.left=Left;
  dc.top=Top;
  dc.right=Right;
  dc.bottom=Bottom;

  SaveEdgeFillLine=CurrentEdgeFillLine;
  SaveLineFillLine=CurrentLineFillLine;
  CurrentEdgeFillLine=NULL;       //ImageRotateFillEdge;    ByHance,96,1.29
  CurrentLineFillLine=ImageSlantRotateFillLine16;

  //-------- ByHance, 96,4.14, ImageSlantRotateFillLine16 may change
  // ImageNewData, TiffPresent->ImageNewHandle
  ImageBufEnd=ImageNewData+BufferLen-1;
  ImageHandle=TiffPresent->ImageNewHandle;

  FillPolygon(&dc,(LPPOINT)Edges,4);

  TiffPresent->ImageNewHandle=ImageHandle;
  ImageNewData=HandleLock(ImageHandle);

  HandleUnlock(TiffPresent->ImageNewHandle);
  HandleUnlock(TiffPresent->ImageHandle);
  CurrentEdgeFillLine=SaveEdgeFillLine;
  CurrentLineFillLine=SaveLineFillLine;

  /*
  {
    int j,OldColor;
    long Address;
    setviewport(0,0,639,479,0);

    for (i=0;i<TiffPresent->ImageHeight;i++)
    for (j=0;j<TiffPresent->ImageWidth;j++)
      {
       //BufferLen=(long)(TiffPresent->ImageWidth+7)/8
       //              *TiffPresent->ImageHeight;
       Address=((long)i*RealWidth+j)>>1;
       OldColor=ImageNewData[Address];
       if ( (j&1)!=0 )
       OldColor&=0xf;
        else
       OldColor=(OldColor>>4)&0xf;
       setcolor(OldColor);
       line(j,i,j,i);
      }
    getch();
  }
  */

  ReturnOK();
}

//////////////////////////////By zjh for fast /////////////////////////////
static unsigned char ColorToBit[8*16*4]={
        0x00,0x00,0x00,0x00,
        0x80,0x00,0x00,0x00,
        0x00,0x80,0x00,0x00,
        0x80,0x80,0x00,0x00,
        0x00,0x00,0x80,0x00,
        0x80,0x00,0x80,0x00,
        0x00,0x80,0x80,0x00,
        0x80,0x80,0x80,0x00,
        0x00,0x00,0x00,0x80,
        0x80,0x00,0x00,0x80,
        0x00,0x80,0x00,0x80,
        0x80,0x80,0x00,0x80,
        0x00,0x00,0x80,0x80,
        0x80,0x00,0x80,0x80,
        0x00,0x80,0x80,0x80,
        0x80,0x80,0x80,0x80,
        0x00,0x00,0x00,0x00,
        0x40,0x00,0x00,0x00,
        0x00,0x40,0x00,0x00,
        0x40,0x40,0x00,0x00,
        0x00,0x00,0x40,0x00,
        0x40,0x00,0x40,0x00,
        0x00,0x40,0x40,0x00,
        0x40,0x40,0x40,0x00,
        0x00,0x00,0x00,0x40,
        0x40,0x00,0x00,0x40,
        0x00,0x40,0x00,0x40,
        0x40,0x40,0x00,0x40,
        0x00,0x00,0x40,0x40,
        0x40,0x00,0x40,0x40,
        0x00,0x40,0x40,0x40,
        0x40,0x40,0x40,0x40,
        0x00,0x00,0x00,0x00,
        0x20,0x00,0x00,0x00,
        0x00,0x20,0x00,0x00,
        0x20,0x20,0x00,0x00,
        0x00,0x00,0x20,0x00,
        0x20,0x00,0x20,0x00,
        0x00,0x20,0x20,0x00,
        0x20,0x20,0x20,0x00,
        0x00,0x00,0x00,0x20,
        0x20,0x00,0x00,0x20,
        0x00,0x20,0x00,0x20,
        0x20,0x20,0x00,0x20,
        0x00,0x00,0x20,0x20,
        0x20,0x00,0x20,0x20,
        0x00,0x20,0x20,0x20,
        0x20,0x20,0x20,0x20,
        0x00,0x00,0x00,0x00,
        0x10,0x00,0x00,0x00,
        0x00,0x10,0x00,0x00,
        0x10,0x10,0x00,0x00,
        0x00,0x00,0x10,0x00,
        0x10,0x00,0x10,0x00,
        0x00,0x10,0x10,0x00,
        0x10,0x10,0x10,0x00,
        0x00,0x00,0x00,0x10,
        0x10,0x00,0x00,0x10,
        0x00,0x10,0x00,0x10,
        0x10,0x10,0x00,0x10,
        0x00,0x00,0x10,0x10,
        0x10,0x00,0x10,0x10,
        0x00,0x10,0x10,0x10,
        0x10,0x10,0x10,0x10,
        0x00,0x00,0x00,0x00,
        0x08,0x00,0x00,0x00,
        0x00,0x08,0x00,0x00,
        0x08,0x08,0x00,0x00,
        0x00,0x00,0x08,0x00,
        0x08,0x00,0x08,0x00,
        0x00,0x08,0x08,0x00,
        0x08,0x08,0x08,0x00,
        0x00,0x00,0x00,0x08,
        0x08,0x00,0x00,0x08,
        0x00,0x08,0x00,0x08,
        0x08,0x08,0x00,0x08,
        0x00,0x00,0x08,0x08,
        0x08,0x00,0x08,0x08,
        0x00,0x08,0x08,0x08,
        0x08,0x08,0x08,0x08,
        0x00,0x00,0x00,0x00,
        0x04,0x00,0x00,0x00,
        0x00,0x04,0x00,0x00,
        0x04,0x04,0x00,0x00,
        0x00,0x00,0x04,0x00,
        0x04,0x00,0x04,0x00,
        0x00,0x04,0x04,0x00,
        0x04,0x04,0x04,0x00,
        0x00,0x00,0x00,0x04,
        0x04,0x00,0x00,0x04,
        0x00,0x04,0x00,0x04,
        0x04,0x04,0x00,0x04,
        0x00,0x00,0x04,0x04,
        0x04,0x00,0x04,0x04,
        0x00,0x04,0x04,0x04,
        0x04,0x04,0x04,0x04,
        0x00,0x00,0x00,0x00,
        0x02,0x00,0x00,0x00,
        0x00,0x02,0x00,0x00,
        0x02,0x02,0x00,0x00,
        0x00,0x00,0x02,0x00,
        0x02,0x00,0x02,0x00,
        0x00,0x02,0x02,0x00,
        0x02,0x02,0x02,0x00,
        0x00,0x00,0x00,0x02,
        0x02,0x00,0x00,0x02,
        0x00,0x02,0x00,0x02,
        0x02,0x02,0x00,0x02,
        0x00,0x00,0x02,0x02,
        0x02,0x00,0x02,0x02,
        0x00,0x02,0x02,0x02,
        0x02,0x02,0x02,0x02,
        0x00,0x00,0x00,0x00,
        0x01,0x00,0x00,0x00,
        0x00,0x01,0x00,0x00,
        0x01,0x01,0x00,0x00,
        0x00,0x00,0x01,0x00,
        0x01,0x00,0x01,0x00,
        0x00,0x01,0x01,0x00,
        0x01,0x01,0x01,0x00,
        0x00,0x00,0x00,0x01,
        0x01,0x00,0x00,0x01,
        0x00,0x01,0x00,0x01,
        0x01,0x01,0x00,0x01,
        0x00,0x00,0x01,0x01,
        0x01,0x00,0x01,0x01,
        0x00,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,
};

#ifdef UNUSE
int ChangeBuff(unsigned char *p0,int len,short start,short end,int left)
{
unsigned char *p1,*p2,*p3,*p;
register int i,len1,j;
unsigned char c,c0;
p1=p0;
len1=len;
for (i=0;i<len1;i++) *p1++=0;
if (start>end)
 {
    len=start;
    start=end;
    end=len;
 }
if (left>start) start=left;
if (left+len1<end) end=left+len1-1;
*(p0-6)=((end-start+1)&0xff);
*(p0-5)=((end-start+1)>>8);
*(p0-4)=1;
*(p0-3)=0;
len=((end-start+8)>>3);
p1=p0+len;
p2=p1+len;
p3=p2+len;
j=0;
for (i=0;i<len1;)
{
   c=*ImageNewData++;
   c0=(c>>4);
   c=(c&15);

   if (i+left>=start&&i+left<=end)
   {
   *p3 |=ColorToBit[j&7][c0][0];
   *p2 |=ColorToBit[j&7][c0][1];
   *p1 |=ColorToBit[j&7][c0][2];
   *p0 |=ColorToBit[j&7][c0][3];
   j++;
   }

   if ((!(j&7))&&j>0)
    {
     p0++; p1++; p2++; p3++;
    }

   i++;
   if (i>=len1) break;

   if (i+left>=start&&i+left<=end)
   {
   *p3 |=ColorToBit[j&7][c][0];
   *p2 |=ColorToBit[j&7][c][1];
   *p1 |=ColorToBit[j&7][c][2];
   *p0 |=ColorToBit[j&7][c][3];
   j++;
   }

   i++;

   if ((!(j&7))&&j>0)
    {
     p0++; p1++; p2++; p3++;
    }
 }
 if (start>end) return -1;
 return start;
}
#endif

int ZoomChangeBuff(unsigned char *p0,int len,short start,short end,int left,char *buff)
{
   register unsigned char *p1,*p2,*p3,*p;
   register short i,j;
   int len1;
   unsigned char c;

   if (len>1023) len=1023;
   len1=len;
   if (start>end)
   {
       len=start;
       start=end;
       end=len;
   }

   if (left>start) start=left;
   if (left+len1<end) end=left+len1-1;
   if (start>end) return -1;
   //  *(p0-6)=((end-start+1)&0xff);
   //  *(p0-5)=((end-start+1)>>8);
   PutImageLineLen=end-start+1;
   len=((PutImageLineLen+7)>>3);
   p1=p0+len;
   p2=p1+len;
   p3=p2+len;
   j=0;
   *p0=0;
   *p1=0;
   *p2=0;
   *p3=0;
   buff+=start;
   for (i=start;i<=end;i++)
   {
      c=*buff++;
      c=(c&15);
      if (j==8)
      {
        j=0;
        *(++p0)=0;
        *(++p1)=0;
        *(++p2)=0;
        *(++p3)=0;
      }
      p=ColorToBit+(((j<<4)+c)<<2);
      *p3 |=*p++;
      *p2 |=*p++;
      *p1 |=*p++;
      *p0 |=*p;
      j++;
   }
   return start;
}

static int ImageDisplayNewHandle16(ImageDescribes *TiffPresent,int PosX,int PosY,
                            float ZoomX,float ZoomY)
{
  #define LocalBuffLen 20000
  int Length,ImagePosX,ImagePosY,i,OldPosX,OldPosY,NewColor,ImageRight;
  int ZoomXInt,ZoomXFract,ZoomYInt,ZoomYFract,NowZoomXInt,NowZoomXFract,
      NowZoomYInt,NowZoomYFract,NewImageX;
  int scrwid,scrhi,j,k,num,left;            //By zjh
  char zbuff[2000],MyBuf[LocalBuffLen];        //By zjh
  short mbuff[1050],*head,tail,Xleft;          //By zjh

  MyBuf[0]=0;
  MyBuf[1]=0;
  MyBuf[2]=0;
  MyBuf[3]=0;
  MyBuf[4]=4;
  MyBuf[5]=0;

  tail=6;
  Xleft=-1;

  ImageNewData=HandleLock(TiffPresent->ImageNewHandle);

  if (ImageNewData==NULL)
     return(OUTOFMEMORY);

  ZoomXInt=ZoomX*NewImageScaleX;
  ZoomXFract=(ZoomX*NewImageScaleX-ZoomXInt)*100.;
  ZoomYInt=ZoomY*NewImageScaleY;
  ZoomYFract=(ZoomY*NewImageScaleY-ZoomYInt)*100.;

  ImagePosX=ImageLineBufferGetLeft(ImageNewData);
  Length=ImageLineBufferGetLength(ImageNewData);
  ImageNewData+=4;  // point to real data, see also ..\kernl\polyfunc.c
                    // function: ImageSlantRotateFillLine16(...)
  //ImagePosY=PosY+TiffPresent->ImageOriginY/GlobalPageScale;
  ImagePosY=PosY+myUserYToWindowY(TiffPresent->ImageOriginY);

  OldPosY=0;
  NowZoomYInt=NowZoomYFract=0;
  scrwid=getmaxx();
  scrhi=getmaxy();
  while (Length)
  {
    NowZoomYInt+=ZoomYInt;
    NowZoomYFract+=ZoomYFract;
    if (NowZoomYFract>=100)
    {
       NowZoomYInt++;
       NowZoomYFract-=100;
    }

    //NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX+TiffPresent->ImageOriginX/GlobalPageScale;
    NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX
              +myUserXToWindowX(TiffPresent->ImageOriginX);
    ImageRight=ImagePosX+Length;

    NowZoomXInt=NewImageX;       //By zjh
    NowZoomXFract=0;

    for (i=ImagePosX;i<ImageRight;i++)
       {

           /*   Test Zoom = 1  for speed
           OldColor=*ImageNewData++;
           NewColor=(OldColor>>4);
           OldColor &=15;
           if (NowZoomXInt>0&&NowZoomXInt<scrwid)
            {
                zbuff[NowZoomXInt++]=NewColor;
                zbuff[NowZoomXInt++]=OldColor;
            }
            else
            {
                NowZoomXInt++;
                NowZoomXInt++;
            }
           */

           OldPosX=NowZoomXInt;
           NowZoomXInt+=ZoomXInt;
           NowZoomXFract+=ZoomXFract;
           if (NowZoomXFract>=100)
           {
              NowZoomXInt++;
              NowZoomXFract-=100;
           }

          if (OldPosX<scrwid)
          {
           if( (i&1)!=0 )
              NewColor=(*ImageNewData++) & 0x0f;
           else
              NewColor=((*ImageNewData)>>4) & 0x0f;

           if (ImageGetBitColor(TiffPresent)==IMAGEBLACKWHITE&&NewColor==EGA_BLACK)
              NewColor=TiffPresent->ImageColor;

           for (j=OldPosX;j<NowZoomXInt;j++)
             if (j>=0&&j<scrwid) zbuff[j]=NewColor;
          }
          else
           if( (i&1)!=0 ) ImageNewData++;
       }

    if( (ImageRight&1)!=0 )
          ImageNewData++;

    for (j=OldPosY;j<NowZoomYInt;j++)
     {
        num=FindRowFillData(ImagePosY+j,mbuff);

        #ifdef FastDisp
        if (num<2)
         {
            //This branch is never enter,otherwise ,mistaken occer
            if (Xleft>=0)
             {
              putimage(Xleft,Yleft,MyBuf,COPY_PUT);
              head=(short *)MyBuf;
              *head++=0;
              *head=0;
              tail=6;
              Xleft=-1;
             }
            continue;
         }
        for (k=0;k<num-1;k=k+2)
        {
         left=ZoomChangeBuff(MyBuf+tail,NowZoomXInt-NewImageX,mbuff[k],mbuff[k+1],NewImageX,zbuff);
         head=(short *)MyBuf;
         if (((Xleft==left&&PutImageLineLen==*head)||(*head==0))&&left>=0)
         {
            tail+=(((PutImageLineLen+7)>>3)<<2);
            *head++=PutImageLineLen;
            (*head)++;
            if (*head==1) Yleft=ImagePosY+j;
            Xleft=left;
            if (tail>LocalBuffLen-500)
             {
              putimage(Xleft,Yleft,MyBuf,COPY_PUT);
              *head--=0;
              *head=0;
              tail=6;
              Xleft=-1;
             }
            continue;
         }
         if (Xleft>=0)
            putimage(Xleft,Yleft,MyBuf,COPY_PUT);
         if (left>=0)
         {
         head=(short*)(&MyBuf[tail-6]);
         *head++=PutImageLineLen;
         *head++=1;
         *head=4;
         putimage(left,ImagePosY+j,MyBuf+tail-6,COPY_PUT);
         }
         head=(short *)MyBuf;
         *head++=0;
         *head=0;
         tail=6;
         Xleft=-1;
        }

       #else

       if (num<2)
            continue;
        for (k=0;k<num-1;k=k+2)
        {
         left=ZoomChangeBuff(MyBuf+6,NowZoomXInt-NewImageX,mbuff[k],mbuff[k+1],NewImageX,zbuff);
         head=(short *)MyBuf;
         if (left>=0)
         {
         head=(short*)MyBuf;
         *head++=PutImageLineLen;
         *head++=1;
         *head=4;
         putimage(left,ImagePosY+j,MyBuf,COPY_PUT);
         }
        }

       #endif
     }
    ImagePosX=ImageLineBufferGetLeft(ImageNewData);
    Length=ImageLineBufferGetLength(ImageNewData);
    ImageNewData+=4;
    OldPosY=NowZoomYInt;
    if (OldPosY+ImagePosY>=scrhi) break;
  } /*-- while length --*/

  #ifdef FastDisp
  if (Xleft>=0) putimage(Xleft,Yleft,MyBuf,COPY_PUT);
  #endif

  HandleUnlock(TiffPresent->ImageNewHandle);
  ReturnOK();
}

////////////////////By zjh//////////////////////////////
#ifdef UNUSE
static int _ImageDisplayNewHandle16(ImageDescribes *TiffPresent,int PosX,int PosY,
                            float ZoomX,float ZoomY)
{
  int Length,ImagePosX,ImagePosY,i,OldPosX,OldPosY,OldColor,NewColor,ImageRight;
  int ZoomXInt,ZoomXFract,ZoomYInt,ZoomYFract,NowZoomXInt,NowZoomXFract,
      NowZoomYInt,NowZoomYFract,NewImageX;
  int k;
  char zbuff[2000];        //By zjh
 //  unsigned char *SaveImageNewData;

  /*SaveImageNewData=*/
  ImageNewData=HandleLock(TiffPresent->ImageNewHandle);

  if (ImageNewData==NULL)
     return(OUTOFMEMORY);

#ifdef UNUSED           // ByHance, 96,1.29
  if (ImageGetContrast(TiffPresent)==HIGHCONTRAST)
  {
     unsigned char MinGray,MaxGray;

     if (ImageGetBitColor(TiffPresent)==IMAGE16GRAY)
     {
        GetMinMaxGray(ImageData,4,TiffPresent->ImageWidth,
                      TiffPresent->ImageHeight,&MinGray,&MaxGray);
        PosterizedGray[0]=MinGray+(MaxGray-MinGray)*HIGHCONTRASTCONST;
        PosterizedGray[1]=MaxGray;
     }
     else
        if (ImageGetBitColor(TiffPresent)==IMAGE256GRAY)
        {
           GetMinMaxGray(ImageData,8,TiffPresent->ImageWidth,
                      TiffPresent->ImageHeight,&MinGray,&MaxGray);
           PosterizedGray[0]=MinGray+(MaxGray-MinGray)*HIGHCONTRASTCONST;
           PosterizedGray[1]=MaxGray;
        }
        else
           ImageSetContrast(TiffPresent,0);
  }
  else
  {
     if (ImageGetContrast(TiffPresent)==POSTERIZED)
     {
        unsigned char MinGray,MaxGray;

        if (ImageGetBitColor(TiffPresent)==IMAGE256GRAY)
        {
           int i;

           GetMinMaxGray(ImageData,8,TiffPresent->ImageWidth,
                      TiffPresent->ImageHeight,&MinGray,&MaxGray);
           for (i=0;i<MAXPOSTERIZEDCOLOR;i++)
               PosterizedGray[i]=MinGray+((int)(MaxGray-MinGray))
                                *(i+1)/MAXPOSTERIZEDCOLOR;
        }
        else
           ImageSetContrast(TiffPresent,0);
     }
  }
#endif     // UNUSED           // ByHance, 96,1.29

  ZoomXInt=ZoomX*NewImageScaleX;
  ZoomXFract=(ZoomX*NewImageScaleX-ZoomXInt)*100.;
  ZoomYInt=ZoomY*NewImageScaleY;
  ZoomYFract=(ZoomY*NewImageScaleY-ZoomYInt)*100.;

  // ImageNewData=SaveImageNewData;
  ImagePosX=ImageLineBufferGetLeft(ImageNewData);
  Length=ImageLineBufferGetLength(ImageNewData);
  ImageNewData+=4;  // point to real data, see also ..\kernl\polyfunc.c
                    // function: ImageSlantRotateFillLine16(...)
  //ImagePosY=PosY+TiffPresent->ImageOriginY/GlobalPageScale;

  ImagePosY=PosY+myUserYToWindowY(TiffPresent->ImageOriginY);

  #ifdef UNUSE
  {
    unsigned char MyBuf[1000];
    short mbuff[1000];
    int num,left;
    //int len;
    //len=imagesize(0,0,13,10);
    //setviewport(0,0,639,479,0);
    //setfillstyle(1,0);
    //bar(0,0,639,479);
    //memset(MyBuf,0,10000-1);

    while (Length)
    {
     num=FindRowFillData(ImagePosY,mbuff);
     //NewImageX=ImagePosX+PosX+TiffPresent->ImageOriginX/GlobalPageScale;
     NewImageX=ImagePosX+PosX+myUserXToWindowX(TiffPresent->ImageOriginX);
     left=ChangeBuff(MyBuf+6,Length,mbuff[0],mbuff[1],NewImageX);
     MyBuf[4]=4;
     MyBuf[5]=0;
     if (num>0&&left!=-1)
       putimage(left,ImagePosY,MyBuf,COPY_PUT);
     ImagePosX=ImageLineBufferGetLeft(ImageNewData);
     Length=ImageLineBufferGetLength(ImageNewData);
     ImageNewData+=4;
     ImagePosY++;
    }
    //getch();
    HandleUnlock(TiffPresent->ImageNewHandle);
    ReturnOK();
  }
  #endif

  OldPosY=0;
  NowZoomYInt=NowZoomYFract=0;
  while (Length)
  {
    NowZoomYInt+=ZoomYInt;
    NowZoomYFract+=ZoomYFract;
    if (NowZoomYFract>=100)
    {
       NowZoomYInt++;
       NowZoomYFract-=100;
    }

    if( (ImagePosX&1)!=0 )
       OldColor=(*ImageNewData++)&0x0f;
    else
       OldColor=((*ImageNewData)>>4)&0x0f;

 #ifdef UNUSED           // ByHance, 96,1.29
    if (ImageGetContrast(TiffPresent))
       OldColor=GetPosterizedGray(OldColor);
 #endif

    if (ImageGetBitColor(TiffPresent)==IMAGEBLACKWHITE && OldColor==EGA_BLACK)
       OldColor=TiffPresent->ImageColor;

    //NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX+TiffPresent->ImageOriginX/GlobalPageScale;
    NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX
              +myUserXToWindowX(TiffPresent->ImageOriginX);
    OldPosX=0;
    ImageRight=ImagePosX+Length;
    NowZoomXInt=0;
    NowZoomXFract=0;

    if (Length>1)
    {
       for (i=ImagePosX+1;i<ImageRight;i++)
       {
           NowZoomXInt+=ZoomXInt;
           NowZoomXFract+=ZoomXFract;
           if (NowZoomXFract>=100)
           {
              NowZoomXInt++;
              NowZoomXFract-=100;
           }

           if( (i&1)!=0 )
              NewColor=(*ImageNewData++) & 0x0f;
           else
              NewColor=((*ImageNewData)>>4) & 0x0f;

    #ifdef UNUSED           // ByHance, 96,1.29
           if (ImageGetContrast(TiffPresent))
              NewColor=GetPosterizedGray(NewColor);
    #endif

           if (ImageGetBitColor(TiffPresent)==IMAGEBLACKWHITE&&NewColor==EGA_BLACK)
              NewColor=TiffPresent->ImageColor;

           if (NewColor!=OldColor)
           {
              #ifdef UNUSED           // ByHance, 96,1.29
                if (ImageGetNegative(TiffPresent))
                   setcolor(~OldColor);
                else
                   setcolor(OldColor);
              #else
                setcolor(OldColor);
              #endif

              ///////////////////////GGGGGGGGGG///////////////////
              for (k=OldPosY;k<NowZoomYInt;k++)
                  HLine(OldPosX+NewImageX,ImagePosY+k,NowZoomXInt+NewImageX);
                  //line(OldPosX+NewImageX,ImagePosY+k,NowZoomXInt+NewImageX,ImagePosY+k);

              OldPosX=NowZoomXInt;
              OldColor=NewColor;
           }
       }

       #ifdef UNUSED           // ByHance, 96,1.29
         if (ImageGetNegative(TiffPresent))
            setcolor(~OldColor);
         else
            setcolor(OldColor);
       #else
         setcolor(OldColor);
       #endif

       for (k=OldPosY;k<NowZoomYInt;k++)
           HLine(OldPosX+NewImageX,ImagePosY+k,NowZoomXInt+NewImageX);

       if( (ImageRight&1)!=0 )
          ImageNewData++;
    }
    else
    {
       NowZoomXInt+=ZoomXInt;
       NowZoomXFract+=ZoomXFract;
       if (NowZoomXFract>=100)
       {
          NowZoomXInt++;
          NowZoomXFract-=100;
       }

       #ifdef UNUSED           // ByHance, 96,1.29
         if (ImageGetNegative(TiffPresent))
            setcolor(~OldColor);
         else
            setcolor(OldColor);
       #else
         setcolor(OldColor);
       #endif

       for (k=OldPosY;k<NowZoomYInt;k++)
           HLine(OldPosX+NewImageX,ImagePosY+k,NowZoomXInt+NewImageX);

       if( (ImageRight&1)!=0 )
          ImageNewData++;
    }

    ImagePosX=ImageLineBufferGetLeft(ImageNewData);
    Length=ImageLineBufferGetLength(ImageNewData);
    ImageNewData+=4;
    OldPosY=NowZoomYInt;
  } /*-- while length --*/

  HandleUnlock(TiffPresent->ImageNewHandle);
  ReturnOK();
}
#endif

static int ImageClearNewHandle16(ImageDescribes *TiffPresent,int PosX,int PosY,
                          float ZoomX,float ZoomY)
{
  int Length,ImagePosX,ImagePosY;
  int ZoomYInt,ZoomYFract,NowZoomXInt,NowZoomYInt,NowZoomYFract,NewImageX;
  int k;
  // unsigned char *SaveImageNewData;

  /*SaveImageNewData=*/
  ImageNewData=HandleLock(TiffPresent->ImageNewHandle);

  if (ImageNewData==NULL)
     return(OUTOFMEMORY);

  ZoomYInt=ZoomY*NewImageScaleY;
  ZoomYFract=(ZoomY*NewImageScaleY-ZoomYInt)*100.;
  NowZoomYInt=NowZoomYFract=0;

  ImagePosX=ImageLineBufferGetLeft(ImageNewData);
  Length=ImageLineBufferGetLength(ImageNewData);
  ImageNewData+=4;
  //ImagePosY=PosY+TiffPresent->ImageOriginY/GlobalPageScale;
  ImagePosY=PosY+myUserYToWindowY(TiffPresent->ImageOriginY);

  setcolor(EGA_WHITE);
  while (Length)
  {
    k=NowZoomYInt;
    NowZoomYInt+=ZoomYInt;
    NowZoomYFract+=ZoomYFract;
    if (NowZoomYFract>=100)
    {
       NowZoomYInt++;
       NowZoomYFract-=100;
    }
    NowZoomXInt=ZoomX*Length*NewImageScaleX;
    //NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX+TiffPresent->ImageOriginX/GlobalPageScale;
    NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX
              +myUserXToWindowX(TiffPresent->ImageOriginX);

    for (;k<NowZoomYInt;k++)
  #ifdef _SCREENGRAPH_
      HLine(NewImageX,ImagePosY+k,NowZoomXInt+NewImageX);
  #else
      line(NewImageX,ImagePosY+k,NowZoomXInt+NewImageX,ImagePosY+k);
  #endif

    if (Length>1)
    {
       if (ImagePosX&1)
           ImageNewData++;

       for (k=1;k<Length;k++)
       {
         if( ((k+ImagePosX)&1)!=0 )
            ImageNewData++;
       }

       if( ((ImagePosX+Length)&1)!=0 && Length>1 )
          ImageNewData++;
    }
    else
       ImageNewData++;

    ImagePosX=ImageLineBufferGetLeft(ImageNewData);
    Length=ImageLineBufferGetLength(ImageNewData);
    ImageNewData+=4;
  }

  HandleUnlock(TiffPresent->ImageNewHandle);
  ReturnOK();
}

static int ImageMoveClearNewHandle16(ImageDescribes *TiffPresent,int PosX,int PosY,
                              float ZoomX,float ZoomY,int MoveX,int MoveY)
{
  int Length,ImagePosX,ImagePosY;
  int ZoomYInt,ZoomYFract,NowZoomXInt,NowZoomYInt,NowZoomYFract,NewImageX;
  int k;
  unsigned char *SaveImageNewData;
  int Bottom;
  int Lefts[4096],Rights[4096];

  SaveImageNewData=ImageNewData=HandleLock(TiffPresent->ImageNewHandle);
  if (ImageNewData==NULL)
     return(OUTOFMEMORY);

  //MoveX=MoveX/GlobalPageScale;
  //MoveY=MoveY/GlobalPageScale;
  MoveX=myUserXToWindowX(MoveX);
  MoveY=myUserYToWindowY(MoveY);


  ZoomYInt=ZoomY*NewImageScaleY;
  ZoomYFract=(ZoomY*NewImageScaleY-ZoomYInt)*100.;
  NowZoomYInt=NowZoomYFract=0;

  //ImagePosY=PosY+TiffPresent->ImageOriginY/GlobalPageScale;
  ImagePosY=PosY+myUserYToWindowY(TiffPresent->ImageOriginY);
  setcolor(EGA_WHITE);

  ImagePosX=ImageLineBufferGetLeft(ImageNewData);
  Length=ImageLineBufferGetLength(ImageNewData);
  ImageNewData+=4;
  while (Length)
  {
    k=NowZoomYInt;

    NowZoomYInt+=ZoomYInt;
    NowZoomYFract+=ZoomYFract;
    if (NowZoomYFract>=100)
    {
       NowZoomYInt++;
       NowZoomYFract-=100;
    }

    if (MoveY<0)
    {
       NowZoomXInt=ZoomX*Length*NewImageScaleX;
       //NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX+TiffPresent->ImageOriginX/GlobalPageScale;
       NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX
                 +myUserXToWindowX(TiffPresent->ImageOriginX);
       for (;k<NowZoomYInt;k++)
       {
           Lefts[k]=NewImageX;
           Rights[k]=NowZoomXInt+NewImageX;
       }
    }
    else
       if (MoveY>0&&k<=MoveY)
       {
          NowZoomXInt=ZoomX*Length*NewImageScaleX;
          //NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX+TiffPresent->ImageOriginX/GlobalPageScale;
          NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX
                    +myUserXToWindowX(TiffPresent->ImageOriginX);
          for (;k<NowZoomYInt;k++)
          {
              Lefts[k]=NewImageX;
              Rights[k]=NowZoomXInt+NewImageX;
          }
       }

    if (Length>1)
    {
       if( (ImagePosX&1)!=0 )
          ImageNewData++;

       for (k=1;k<Length;k++)
       {
         if( ((k+ImagePosX)&1)!=0 )
            ImageNewData++;
       }
       if( ((ImagePosX+Length)&1)!=0 && Length>1 )
          ImageNewData++;
    }
    else
       ImageNewData++;

    ImagePosX=ImageLineBufferGetLeft(ImageNewData);
    Length=ImageLineBufferGetLength(ImageNewData);
    if (Length>0)
       ImageNewData+=4;
  }
  Bottom=NowZoomYInt;

  if (MoveY<0)
  {
     ImageNewData=SaveImageNewData;
     ImagePosX=ImageLineBufferGetLeft(ImageNewData);
     Length=ImageLineBufferGetLength(ImageNewData);
     NowZoomYInt=NowZoomYFract=0;
     ImageNewData+=4;
     while (Length)
     {
       k=NowZoomYInt;

       NowZoomYInt+=ZoomYInt;
       NowZoomYFract+=ZoomYFract;
       if (NowZoomYFract>=100)
       {
          NowZoomYInt++;
          NowZoomYFract-=100;
       }

       NowZoomXInt=ZoomX*Length*NewImageScaleX;
       //NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX+TiffPresent->ImageOriginX/GlobalPageScale;
       NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX
                 +myUserXToWindowX(TiffPresent->ImageOriginX);

       for (;k<NowZoomYInt;k++)
       {
          int DrawLeft,DrawRight;

          if (k>=Bottom+MoveY)
          {
             DrawLeft=NewImageX;
             DrawRight=NowZoomXInt+NewImageX;
             HLine(DrawLeft,ImagePosY+k,DrawRight);
          }
          else
          {
             DrawLeft=Lefts[k-MoveY]+MoveX;
             DrawRight=Rights[k-MoveY]+MoveX;
             if (NewImageX<DrawLeft)
             #ifdef _SCREENGRAPH_
                HLine(NewImageX,ImagePosY+k,DrawLeft);
             #else
                line(NewImageX,ImagePosY+k,DrawLeft,ImagePosY+k);
             #endif

             if (NowZoomXInt+NewImageX>DrawRight)
             #ifdef _SCREENGRAPH_
                HLine(DrawRight,ImagePosY+k,NowZoomXInt+NewImageX);
             #else
                line(DrawRight,ImagePosY+k,NowZoomXInt+NewImageX,ImagePosY+k);
             #endif
          }
       }

       if (Length>1)
       {
          if( (ImagePosX&1)!=0 )
             ImageNewData++;

          for (k=1;k<Length;k++)
          {
            if( ((k+ImagePosX)&1)!=0 )
               ImageNewData++;
          }
          if( ((ImagePosX+Length)&1)!=0 && Length>1 )
             ImageNewData++;
       }
       else
          ImageNewData++;

       ImagePosX=ImageLineBufferGetLeft(ImageNewData);
       Length=ImageLineBufferGetLength(ImageNewData);
       ImageNewData+=4;
     }
  }
  else
  {             /*--- MoveY>=0 -----*/
     ImageNewData=SaveImageNewData;
     NowZoomYInt=NowZoomYFract=0;
     ImagePosX=ImageLineBufferGetLeft(ImageNewData);
     Length=ImageLineBufferGetLength(ImageNewData);
     ImageNewData+=4;
     while (Length)
     {
       k=NowZoomYInt;
       NowZoomYInt+=ZoomYInt;
       NowZoomYFract+=ZoomYFract;
       if (NowZoomYFract>=100)
       {
          NowZoomYInt++;
          NowZoomYFract-=100;
       }
       NowZoomXInt=ZoomX*Length*NewImageScaleX;
       //NewImageX=ImagePosX*ZoomX*NewImageScaleX+PosX+TiffPresent->ImageOriginX/GlobalPageScale;
       NewImageX=ImagePosX*ZoomX*NewImageScaleX
              +PosX+myUserXToWindowX(TiffPresent->ImageOriginX);

       for (;k<NowZoomYInt;k++)
       {
           int DrawLeft,DrawRight;

           if( MoveY>0 && k<=MoveY )
           {
              DrawLeft=NewImageX;
              DrawRight=NowZoomXInt+NewImageX;
              HLine(DrawLeft,ImagePosY+k,DrawRight);
           }
           else
           if (MoveY==0)
           {
              if (MoveX<0)
              {
                 DrawRight=NowZoomXInt+NewImageX;
                 DrawLeft=DrawRight+MoveX;
              }
              else
              if (MoveX>0)
              {
                 DrawLeft=NewImageX;
                 DrawRight=DrawLeft+MoveX;
              }
              else
              continue;

              HLine(DrawLeft,ImagePosY+k,DrawRight);
           }
           else
           if (MoveY>0)
           {
              DrawLeft=Lefts[k%MoveY]+MoveX;
              DrawRight=Rights[k%MoveY]+MoveX;

              if (NewImageX<DrawLeft)
              #ifdef _SCREENGRAPH_
                 HLine(NewImageX,ImagePosY+k,DrawLeft);
              #else
                 line(NewImageX,ImagePosY+k,DrawLeft,ImagePosY+k);
              #endif

              if (NowZoomXInt+NewImageX>DrawRight)
              #ifdef _SCREENGRAPH_
                 HLine(DrawRight,ImagePosY+k,NowZoomXInt+NewImageX);
              #else
                 line(DrawRight,ImagePosY+k,NowZoomXInt+NewImageX,ImagePosY+k);
              #endif

              Lefts[k%MoveY]=NewImageX;
              Rights[k%MoveY]=NowZoomXInt+NewImageX;
           }
       }

       if (Length>1)
       {
          if( (ImagePosX&1)!=0 )
             ImageNewData++;

          for (k=1;k<Length;k++)
          {
            if( ((k+ImagePosX)&1)!=0 )
               ImageNewData++;
          }
          if( ((ImagePosX+Length)&1)!=0 && Length>1 )
             ImageNewData++;
       }
       else
          ImageNewData++;

       ImagePosX=ImageLineBufferGetLeft(ImageNewData);
       Length=ImageLineBufferGetLength(ImageNewData);
       ImageNewData+=4;
     }
  }

  HandleUnlock(TiffPresent->ImageNewHandle);
  ReturnOK();
}

int PictureBoxImportTiff(char *PictureFileName,HBOX HPictureBox)
{
  int Result;
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent,SavePresent;
  HANDLE ImageH24Data;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
  UndoInsertImageInsert(PictureBoxGetPictureFileName(PictureBox));
  if (TiffPresent->ImageHandle)
  {
     HandleFree(TiffPresent->ImageHandle);
     TiffPresent->ImageHandle=0;
  }
  if (TiffPresent->ImageNewHandle)
  {
     HandleFree(TiffPresent->ImageNewHandle);
     TiffPresent->ImageNewHandle=0;
  }

  if (DoUndoSign)
     memcpy(&SavePresent,TiffPresent,sizeof(SavePresent));

  if ((Result=PictureToPresent(PictureFileName,TiffPresent,PUTDITHERDEVICE,
                            screendpi,4,&ImageH24Data))<OpOK)
  {
     HandleUnlock(ItemGetHandle(HPictureBox));
     return(Result);
  }

  PictureBoxSetPictureFileName(PictureBox,PictureFileName);
  if (DoUndoSign)
  {
     HANDLE SaveHandle;

     SaveHandle=TiffPresent->ImageHandle;
     memcpy(TiffPresent,&SavePresent,sizeof(SavePresent));
     TiffPresent->ImageHandle=SaveHandle;
  }
  else
     if (!TiffPresent->ImageRotateAngle&&PictureBoxGetRotateAngle(PictureBox))
        //ImageRotate(TiffPresent,PictureBoxGetRotateAngle(PictureBox));
        TiffPresent->ImageRotateAngle=PictureBoxGetRotateAngle(PictureBox);

  if ((Result=ImageGetNewHandle(TiffPresent))<OpOK)
  {
     HandleUnlock(ItemGetHandle(HPictureBox));
     return(Result);
  }
  PictureBoxClearPicture(HPictureBox);               // ByHance
  PictureBoxDisplayPicture(HPictureBox);
  HandleUnlock(ItemGetHandle(HPictureBox));
//  FileSetModified();
  ReturnOK();
}

int PictureBoxDisplayPicture(HBOX HPictureBox)
{
  //int Result;
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int PosX,PosY;
  float ZoomX,ZoomY;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  if (!((PictureBoxGetPictureFileName(PictureBox))[0]))
  {
     HandleUnlock(ItemGetHandle(HPictureBox));
     ReturnOK();
  }

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));

  if (PrintingSign)
  {
     ZoomX=(float)(PrinterDPI)/(float)(screendpi);
     PosX=PictureBoxGetBoxLeft(PictureBox);
                         //+TiffPresent->ImageOriginX);
     PosY=PictureBoxGetBoxTop(PictureBox);
                         //+TiffPresent->ImageOriginY);
     ImageDrawVideo(TiffPresent,PosX,PosY,ZoomX,HPictureBox);
  }
  else
  {
     struct viewporttype SaveViewport;
     int Left,Top,Right,Bottom;

     getviewsettings(&SaveViewport);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);

     #ifdef _SCREENGRAPH_
     {
       int i /*,j*/ ,BoxDots;
       ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];

       if (!Init_template())
       {
          HandleUnlock(ItemGetHandle(HPictureBox));
          return(OUTOFMEMORY);
       }

       BoxGetPolygonDrawBorder(PictureBox,&BoxDots,BoxXY);
       BoxPolygonToWindowXY(BoxDots,BoxXY);
       SetDeviceBoundary(Right,Bottom);
       for (i=0;i<BoxDots;i++)
       {
           BoxXY[2*i]+=Left;
           BoxXY[2*i+1]+=Top;
       }
       SetViewportPolygon(BoxDots,BoxXY);
     #ifdef   OLD_VERSION    // ByHance, 96,4.4
       BoxDots=0;
       for (j=0;j<PictureBoxGetClipPolygons(PictureBox);j++)
       {
           for (i=0;i<PictureBoxGetClipEdges(PictureBox)[j];i++)
           {
               BoxXY[2*i]=UserXToWindowX(PictureBoxGetClipBoxXY(PictureBox)[2*i+BoxDots])
                          +Left;
               BoxXY[2*i+1]=UserYToWindowY(PictureBoxGetClipBoxXY(PictureBox)[2*i+1+BoxDots])
                            +Top;
           }
           SetClipportPolygon(PictureBoxGetClipEdges(PictureBox)[j],BoxXY);
//           BoxDots+=(PictureBoxGetClipEdges(PictureBox)[j]);
           BoxDots+=2*(PictureBoxGetClipEdges(PictureBox)[j]);  // !!!!!
       }
     #endif
     }
     #endif

     //ZoomY=ZoomX=(float)((SCALEMETER*1./screendpi))/(float)GlobalPageScale;
     ZoomX=(float)((SCALEMETER*1./screendpi))/(float)GlobalPageScale*NewSCRX;
     ZoomY=(float)((SCALEMETER*1./screendpi))/(float)GlobalPageScale*NewSCRY;
     PosX=UserXToWindowX(PictureBoxGetBoxLeft(PictureBox));
     PosY=UserYToWindowY(PictureBoxGetBoxTop(PictureBox));
     MouseHidden();
     ImageDisplayNewHandle16(TiffPresent,PosX,PosY,ZoomX,ZoomY);
     // ImageDisplayNewHandle16(TiffPresent,0,0,ZoomX,ZoomY);
     MouseShow();

     #ifdef _SCREENGRAPH_
       FinishTemplate();
     #endif

     setviewport(SaveViewport.left,SaveViewport.top,SaveViewport.right,
                 SaveViewport.bottom,SaveViewport.clip);
  }

  HandleUnlock(ItemGetHandle(HPictureBox));
  ReturnOK();
}

int PictureBoxClearPicture(HBOX HPictureBox)
{
 // int Result;
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int PosX,PosY;
  float ZoomX,ZoomY;
  struct viewporttype SaveViewport;
  int Left,Top,Right,Bottom;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  getviewsettings(&SaveViewport);
  WindowGetRect(1,&Left,&Top,&Right,&Bottom);
  setviewport(Left,Top,Right,Bottom,1);

  #ifdef _SCREENGRAPH_
  {
    int i /*,j*/,BoxDots;
    ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];

    if (!Init_template())
    {
       HandleUnlock(ItemGetHandle(HPictureBox));
       return(OUTOFMEMORY);
    }

    BoxGetPolygonDrawBorder(PictureBox,&BoxDots,BoxXY);
    BoxPolygonToWindowXY(BoxDots,BoxXY);
    SetDeviceBoundary(Right,Bottom);
    for (i=0;i<BoxDots;i++)
    {
        BoxXY[2*i]+=Left;
        BoxXY[2*i+1]+=Top;
    }
    SetViewportPolygon(BoxDots,BoxXY);
  #ifdef   OLD_VERSION    // ByHance, 96,4.4
    BoxDots=0;
    for (j=0;j<PictureBoxGetClipPolygons(PictureBox);j++)
    {
        for (i=0;i<PictureBoxGetClipEdges(PictureBox)[j];i++)
        {
            BoxXY[2*i]=UserXToWindowX(PictureBoxGetClipBoxXY(PictureBox)[2*i+BoxDots])
                       +Left;
            BoxXY[2*i+1]=UserYToWindowY(PictureBoxGetClipBoxXY(PictureBox)[2*i+1+BoxDots])
                         +Top;
        }
        SetClipportPolygon(PictureBoxGetClipEdges(PictureBox)[j],BoxXY);
  //      BoxDots+=(PictureBoxGetClipEdges(PictureBox)[j]);
        BoxDots+=2*(PictureBoxGetClipEdges(PictureBox)[j]);  // !!!!!
    }
  #endif
  }
  #endif

  if (!((PictureBoxGetPictureFileName(PictureBox))[0]))
  {
     HandleUnlock(ItemGetHandle(HPictureBox));
     ReturnOK();
  }

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));

  //ZoomY=ZoomX=(float)((SCALEMETER*1./screendpi))/(float)GlobalPageScale;
  ZoomX=(float)((SCALEMETER*1./screendpi))/(float)GlobalPageScale*NewSCRX;
  ZoomY=(float)((SCALEMETER*1./screendpi))/(float)GlobalPageScale*NewSCRY;
  PosX=UserXToWindowX(PictureBoxGetBoxLeft(PictureBox));
  PosY=UserYToWindowY(PictureBoxGetBoxTop(PictureBox));
  MouseHidden();
//  ImageClearNewHandle16(TiffPresent,0,0,ZoomX,ZoomY);
  ImageClearNewHandle16(TiffPresent,PosX,PosY,ZoomX,ZoomY); // !!!!!

  HandleUnlock(ItemGetHandle(HPictureBox));

  #ifdef _SCREENGRAPH_
    FinishTemplate();
  #endif

  MouseShow();
  setviewport(SaveViewport.left,SaveViewport.top,SaveViewport.right,
              SaveViewport.bottom,SaveViewport.clip);
  ReturnOK();
}

int PictureBoxMoveClearPicture(HBOX HPictureBox,int MoveX,int MoveY)
{
 // int Result;
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int PosX,PosY;
  float ZoomX,ZoomY;
  struct viewporttype SaveViewport;
  int Left,Top,Right,Bottom;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  if (!((PictureBoxGetPictureFileName(PictureBox))[0]))
  {
     HandleUnlock(ItemGetHandle(HPictureBox));
     ReturnOK();
  }

  getviewsettings(&SaveViewport);
  WindowGetRect(1,&Left,&Top,&Right,&Bottom);
  setviewport(Left,Top,Right,Bottom,1);

  #ifdef _SCREENGRAPH_
  {
    int i /*,j*/ ,BoxDots;
    ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER];

    if (!Init_template())
    {
       HandleUnlock(ItemGetHandle(HPictureBox));
       return(OUTOFMEMORY);
    }

    BoxGetPolygonDrawBorder(PictureBox,&BoxDots,BoxXY);
    BoxPolygonToWindowXY(BoxDots,BoxXY);
    SetDeviceBoundary(Right,Bottom);
    for (i=0;i<BoxDots;i++)
    {
        BoxXY[2*i]+=Left;
        BoxXY[2*i+1]+=Top;
    }
    SetViewportPolygon(BoxDots,BoxXY);
  #ifdef   OLD_VERSION    // ByHance, 96,4.4
    BoxDots=0;
    for (j=0;j<PictureBoxGetClipPolygons(PictureBox);j++)
    {
        for (i=0;i<PictureBoxGetClipEdges(PictureBox)[j];i++)
        {
            BoxXY[2*i]=UserXToWindowX(PictureBoxGetClipBoxXY(PictureBox)[2*i+BoxDots])
                       +Left;
            BoxXY[2*i+1]=UserYToWindowY(PictureBoxGetClipBoxXY(PictureBox)[2*i+1+BoxDots])
                         +Top;
        }
        SetClipportPolygon(PictureBoxGetClipEdges(PictureBox)[j],BoxXY);
  //      BoxDots+=(PictureBoxGetClipEdges(PictureBox)[j]);
        BoxDots+=2*(PictureBoxGetClipEdges(PictureBox)[j]);  // !!!!!
    }
  #endif
  }
  #endif

  //ZoomY=ZoomX=(float)((SCALEMETER*1./screendpi))/(float)GlobalPageScale;
  ZoomX=(float)((SCALEMETER*1./screendpi))/(float)GlobalPageScale*NewSCRX;
  ZoomY=(float)((SCALEMETER*1./screendpi))/(float)GlobalPageScale*NewSCRY;
  PosX=UserXToWindowX(PictureBoxGetBoxLeft(PictureBox));
  PosY=UserYToWindowY(PictureBoxGetBoxTop(PictureBox));
  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
  MouseHidden();
//  ImageMoveClearNewHandle16(TiffPresent,0,0,ZoomX,ZoomY,MoveX,MoveY);
  ImageMoveClearNewHandle16(TiffPresent,PosX,PosY,ZoomX,ZoomY,MoveX,MoveY); // !!!!!

  HandleUnlock(ItemGetHandle(HPictureBox));
  #ifdef _SCREENGRAPH_
    FinishTemplate();
  #endif

  MouseShow();
  setviewport(SaveViewport.left,SaveViewport.top,SaveViewport.right,
              SaveViewport.bottom,SaveViewport.clip);
  ReturnOK();
}

int PictureBoxMovePicture(HBOX HPictureBox,int MoveX,int MoveY)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;

  if (MoveX||MoveY)
  {
     PictureBox=HandleLock(ItemGetHandle(HPictureBox));
     if (PictureBox==NULL)
        return(OUTOFMEMORY);

     TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
     PictureBoxMoveClearPicture(HPictureBox,MoveX,MoveY);
     UndoInsertImageMove(MoveX,MoveY);
     TiffPresent->ImageOriginX+=MoveX;
     TiffPresent->ImageOriginY+=MoveY;
     PictureBoxDisplayPicture(HPictureBox);
     BoxDrawBorder(HPictureBox,0);

     HandleUnlock(ItemGetHandle(HPictureBox));
     FileSetModified();
  }
  ReturnOK();
}

int PictureBoxRotatePicture(HBOX HPictureBox,int NewRotateAngle)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int Result;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));

  if (TiffPresent->ImageRotateAngle!=NewRotateAngle)
  {
     PictureBoxClearPicture(HPictureBox);
     UndoInsertImageRotate(TiffPresent->ImageRotateAngle);
     //ImageRotate(TiffPresent,NewRotateAngle);
     TiffPresent->ImageRotateAngle=NewRotateAngle;

     if ((Result=ImageGetNewHandle(TiffPresent))<OpOK)
     {
        HandleUnlock(ItemGetHandle(HPictureBox));
        return(Result);
     }
     PictureBoxDisplayPicture(HPictureBox);
     FileSetModified();
  }

  HandleUnlock(ItemGetHandle(HPictureBox));
  ReturnOK();
}

int PictureBoxSkewPicture(HBOX HPictureBox,int NewSkewAngle)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int Result;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));

  if (TiffPresent->ImageSkewAngle!=NewSkewAngle
  && NewSkewAngle>=-75&&NewSkewAngle<75)
  {
     PictureBoxClearPicture(HPictureBox);
     UndoInsertImageSkew(TiffPresent->ImageSkewAngle);
     //ImageSkew(TiffPresent,NewSkewAngle);
     TiffPresent->ImageSkewAngle=NewSkewAngle;

     if ((Result=ImageGetNewHandle(TiffPresent))<OpOK)
     {
        HandleUnlock(ItemGetHandle(HPictureBox));
        return(Result);
     }
     PictureBoxDisplayPicture(HPictureBox);
     FileSetModified();
  }

  HandleUnlock(ItemGetHandle(HPictureBox));
  ReturnOK();
}

int PictureBoxZoomPicture(HBOX HPictureBox,float ZoomX,float ZoomY)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));

  if (NewImageScaleX!=ZoomX||NewImageScaleY!=ZoomY)
  {
     PictureBoxClearPicture(HPictureBox);
     UndoInsertImageZoom(NewImageScaleX,NewImageScaleY);
     // ImageZoom(TiffPresent,ZoomX,ZoomY);
     if (ZoomX>=0.1&&ZoomX<10)
         TiffPresent->ImageScaleX=ZoomX;
     if (ZoomY>=0.1&&ZoomY<10)
         TiffPresent->ImageScaleY=ZoomY;
     PictureBoxDisplayPicture(HPictureBox);
     FileSetModified();
  }

  HandleUnlock(ItemGetHandle(HPictureBox));
  ReturnOK();
}

int PictureBoxSetPictureColor(HBOX HPictureBox,int NewColor)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int ColorBit;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
  ColorBit=ImageGetBitColor(TiffPresent);
  if (ColorBit==IMAGEBLACKWHITE)
  {
     if (TiffPresent->ImageColor!=NewColor)
     {
        PictureBoxClearPicture(HPictureBox);
        UndoInsertImageColor(TiffPresent->ImageColor);
        // ImageSetColor(TiffPresent,NewColor);
        TiffPresent->ImageColor=NewColor;
        PictureBoxDisplayPicture(HPictureBox);
     }
  }
  HandleUnlock(ItemGetHandle(HPictureBox));
  FileSetModified();
  ReturnOK();
}

int PictureBoxGetPictureColor(HBOX HPictureBox,int *Color)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int ColorBit,ret=-1;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
  if(TiffPresent->ImageHandle) {        // ByHance, 96,1.19
      ColorBit=ImageGetBitColor(TiffPresent);
      if (ColorBit==IMAGEBLACKWHITE)
      {
         *Color=TiffPresent->ImageColor;
         ret=0;
      }
  }
  else ret=-2;      // no picture, return -2

  HandleUnlock(ItemGetHandle(HPictureBox));
  return(ret);
}

#ifdef UNUSED           // ByHance, 96,1.29
int PictureBoxSetPictureNewContrast(HBOX HPictureBox,unsigned char NewConstrast)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int ColorBit,ret=-1;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));

  ColorBit=ImageGetBitColor(TiffPresent);
  if ((ColorBit==IMAGE16GRAY)||(ColorBit==IMAGE256GRAY))
  {
     if (ImageGetContrast(TiffPresent)!=NewConstrast)
     {
        PictureBoxClearPicture(HPictureBox);
        UndoInsertImagePosterized(ImageGetContrast(TiffPresent));
        ImageSetNewContrast(TiffPresent,NewConstrast);
        PictureBoxDisplayPicture(HPictureBox);
        FileSetModified();
     }
  }
  HandleUnlock(ItemGetHandle(HPictureBox));
  ReturnOK();
}

int PictureBoxGetPictureContrast(HBOX HPictureBox,int *Constrast)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int ColorBit,ret=-1;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
  if(TiffPresent->ImageHandle) {        // ByHance, 96,1.19
     ColorBit=ImageGetBitColor(TiffPresent);
     if ((ColorBit==IMAGE16GRAY)||(ColorBit==IMAGE256GRAY))
     {
        *Constrast=ImageGetContrast(TiffPresent);
        ret=0;
     }
  }

  HandleUnlock(ItemGetHandle(HPictureBox));
  return(ret);
}

int PictureBoxSetPictureNegative(HBOX HPictureBox)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));

  PictureBoxClearPicture(HPictureBox);
  UndoInsertImageNegative();
  if (!ImageGetNegative(TiffPresent))
     ImageSetNegative(TiffPresent,1);
  else
     ImageSetNegative(TiffPresent,0);
  PictureBoxDisplayPicture(HPictureBox);

  HandleUnlock(ItemGetHandle(HPictureBox));
  FileSetModified();
  ReturnOK();
}
#endif    // UNUSED           // ByHance, 96,1.29

int ImageChangeNewParameter(HBOX HPictureBox)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;
  int Result;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
  FileSetModified();

  if ((Result=ImageGetNewHandle(TiffPresent))<OpOK)
  {
     HandleUnlock(ItemGetHandle(HPictureBox));
     return(Result);
  }
  PictureBoxDisplayPicture(HPictureBox);

  HandleUnlock(ItemGetHandle(HPictureBox));
  ReturnOK();
}

int PictureBoxClearImage(HBOX HPictureBox)
{
  PictureBoxs *PictureBox;
  ImageDescribes *TiffPresent;

  PictureBox=HandleLock(ItemGetHandle(HPictureBox));
  if (PictureBox==NULL)
     return(OUTOFMEMORY);

  TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));

  if (PictureBoxGetPictureFileName(PictureBox)[0])
  {
     FileSetModified();
     PictureBoxClearPicture(HPictureBox);
     if (TiffPresent->ImageNewHandle)
     {
        HandleFree(TiffPresent->ImageNewHandle);
        TiffPresent->ImageNewHandle=0;
     }
     if (TiffPresent->ImageHandle)
     {
        HandleFree(TiffPresent->ImageHandle);
        TiffPresent->ImageHandle=0;
     }
     //UndoInsertImageDelete(PictureBoxGetPictureFileName(PictureBox));
     PictureBoxSetPictureFileName(PictureBox,"");
  }
  HandleUnlock(ItemGetHandle(HPictureBox));
  ReturnOK();
}
