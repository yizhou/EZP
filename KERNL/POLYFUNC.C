/*-------------------------------------------------------------------
* Name: polyfunc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

/*----- This function is called only when printing -----*/
static void RastLineClipLine(int x1,int x2,int y)
{
  LONG byteoff;
  int  bit,oldx;
  LPDC lpdc=&SysDc;

  //x2--;
  if (GlobalRorate90&&PrintingSign)
  {
  if ((x1>=myDC.right)||(x2<myDC.left)) return;
  if ((y>=myDC.bottom)||(y<myDC.top)) return;
  if (x1<myDC.left) x1=myDC.left;
  if (x2>=myDC.right) x2=myDC.right-1;
  byteoff = (y-myDC.top)*((myDC.right-myDC.left+7)/8)+(x1>>3);
  }
  else
  {
  if ((x1>=lpdc->right)||(x2<lpdc->left)) return;
  if ((y>=lpdc->bottom)||(y<lpdc->top)) return;
  if (x1<lpdc->left) x1=lpdc->left;
  if (x2>=lpdc->right) x2=lpdc->right-1;
  byteoff = (y-lpdc->top)*RastWidthByte+(x1>>3);
  }

  while(x1<=x2)
  {
     oldx=x1;
     bit = x1 & 7;
     while( x1<=x2 && (dot1tab[bit]&ImageClipData[byteoff]) )
     {
         x1++;
         bit++;
         if(bit==8) { bit=0; byteoff++; }
     }

     if(x1>oldx) printer->printScanLine(oldx,x1-1,y,lpdc);
     x1++;
     if((x1&7)==0) byteoff++;
  }
}

/*-----------
void PolyEdgeFillLine(LPDC lpdc,int x1,int y1,int x2,int y2)
{
}

void ImageRotateFillEdge(LPDC lpdc,int x1,int y1,int x2,int y2)
{
}
------------*/

#ifdef UNUSED    // ByHance
void ImageSlantRotateFillLineBW(int x1,int x2,int y,LPDC lpdc)
{
  int j,OldX,TmpX,TmpY,byte1,byte2;
  unsigned char OldColor,NewColor,Bits;
  long Address;
  FIXED TmpFixedX,TmpFixedY,TmpFixed1,TmpFixed2,TmpFixed3;

  if (PrintingSign&&GlobalRorate90)
  ImageLineBufferSetLeft(ImageNewData,x1-myDC.left);
  else
  ImageLineBufferSetLeft(ImageNewData,x1-lpdc->left);

  byte1=x1>>3;  byte2=x2>>3;

 #ifdef ERROR_ByHance
  if (byte1==byte2)
     ImageLineBufferSetLength(ImageNewData,1);
  else
  {
      /*--- Error! 96,2.5,  exp:[0,15] ------*/
     ImageLineBufferSetLength(ImageNewData,byte2-byte1+((x1&7)?1:0)
                              +(((x2&7)==7)?0:1));
  }
 #else
  ImageLineBufferSetLength(ImageNewData,byte2-byte1+1);
 #endif   // ERROR_ByHance


  ImageNewData+=4;                     /* Save Image line left and width */

  Int2Fixed(TmpFixedY,(y-ImageAxisY));
  Int2Fixed(TmpFixedX,(x1-ImageAxisX));

  FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
  FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

  Address=((long)TmpY*RealWidth+TmpX)>>3;
  //Bits=7-TmpX%8;
  //OldColor=(ImageData[Address]&BitMatrixs[Bits])>>Bits;
  OldColor=0;
  if(ImageData[Address]&dot1tab[TmpX&7])
     OldColor=1;

  OldX=x1;
  //for (j=x1+1;j<x2;j++)
  for (j=x1;j<x2;j++)            //By zjh
  {
      TmpFixedX.value++;
      FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
      FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

      Address=((long)TmpY*RealWidth+TmpX)>>3;
      //Bits=7-TmpX%8;
      //NewColor=(ImageData[Address]&BitMatrixs[Bits])>>Bits;
      NewColor=0;
      if(ImageData[Address]&dot1tab[TmpX&7])
         NewColor=1;

      if (NewColor!=OldColor)
      {
         // if (!NewColor)      // ByHance, Error!, 96,2.5
         if (!OldColor)         // when OldColor=BLACK, needn't do it
         {
            if ((OldX>>3)==(j>>3))
            {
               if (!(j&7))
                  ImageNewData++;
            }
            else
            {
               if (!(OldX&7))
                  ImageNewData++;
               ImageNewData+=(j>>3)-(OldX>>3);
            }
         }
         else
         {
            if ((OldX>>3)==(j>>3))
            {
               //(*ImageNewData)|=BitFill[7-(OldX&7)]&(~BitFill[7-(j&7)]);
               (*ImageNewData)|=headdot[(OldX&7)]&taildot[(j&7)];
               if (!(j&7))
                  ImageNewData++;
            }
            else
            {
               if( (OldX&7)!=0 )
                  (*(ImageNewData++))|=headdot[(OldX&7)];
                  //(*(ImageNewData++))|=BitFill[7-(OldX&7)];
               memset(ImageNewData,0xff,(j>>3)-(OldX>>3));
               ImageNewData+=(j>>3)-(OldX>>3);
               if( (j&7)!=0)
                  (*ImageNewData)|=taildot[(j&7)];
                  //(*ImageNewData)|=~BitFill[7-(j&7)];
            }
         }
         OldX=j;
         OldColor=NewColor;
      }
  }

  if (!OldColor)
  {
     if ((OldX>>3)==(j>>3))
        ImageNewData++;
     else
     {
        ImageNewData+=(j>>3)-(OldX>>3);
        if (!(j&7))
           ImageNewData++;
     }
  }
  else
  {
     if ((OldX>>3)==(j>>3))
        (*(ImageNewData)++)|=headdot[(OldX&7)]&taildot[(j&7)];
        //(*(ImageNewData)++)|=BitFill[7-(OldX&7)]&(~BitFill[7-(j&7)]);
     else
     {
        if( (OldX&7)!=0 )
           (*(ImageNewData++))|=headdot[(OldX&7)];
        memset(ImageNewData,0xff,(j>>3)-(OldX>>3));
        ImageNewData+=(j>>3)-(OldX>>3);
        if (j&7)
           (*ImageNewData)|=taildot[(j&7)];
     }
  }
}

void ImageSlantRotateFillLine256(int x1,int x2,int y,LPDC lpdc)
{
  int j,OldX,TmpX,TmpY;
  unsigned char OldColor,NewColor;
  long Address;
  FIXED TmpFixedX,TmpFixedY,TmpFixed1,TmpFixed2,TmpFixed3;

  if (PrintingSign&&GlobalRorate90)
  ImageLineBufferSetLeft(ImageNewData,myDC.left);
  else
  ImageLineBufferSetLeft(ImageNewData,x1-lpdc->left);

  ImageLineBufferSetLength(ImageNewData,x2-x1);
  ImageNewData+=4;                     /* Save Image line left and width */

  Int2Fixed(TmpFixedY,(y-ImageAxisY));
  Int2Fixed(TmpFixedX,(x1-ImageAxisX));

  FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
  FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

  Address=(long)TmpY*RealWidth+TmpX;
  OldColor=ImageData[Address];
  OldX=x1;
  //for (j=x1+1;j<x2;j++)
  for (j=x1;j<x2;j++)           //By zjh 10.15
  {
      TmpFixedX.value++;
      FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
      FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

      Address=(long)TmpY*RealWidth+TmpX;
      NewColor=ImageData[Address];
      if (NewColor!=OldColor)
      {
         memset(ImageNewData,OldColor,j-OldX);
         ImageNewData+=j-OldX;
         OldX=j;
         OldColor=NewColor;
      }
  }
  memset(ImageNewData,OldColor,j-OldX);
  ImageNewData+=j-OldX;
  return;
}

void ImageSlantRotateFillLineTrueColor(int x1,int x2,int y,LPDC lpdc)
{
  int j,OldX,TmpX,TmpY;
  unsigned long OldColor,NewColor;
  long Address;
  FIXED TmpFixedX,TmpFixedY,TmpFixed1,TmpFixed2,TmpFixed3;

  if (PrintingSign&&GlobalRorate90)
  ImageLineBufferSetLeft(ImageNewData,x1-myDC.left);
  else
  ImageLineBufferSetLeft(ImageNewData,x1-lpdc->left);

  ImageLineBufferSetLength(ImageNewData,(x2-x1)*3);
  ImageNewData+=4;                     /* Save Image line left and width */

  Int2Fixed(TmpFixedY,(y-ImageAxisY));
  Int2Fixed(TmpFixedX,(x1-ImageAxisX));

  FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
  FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

  Address=((long)TmpY*RealWidth+TmpX)*3;
  OldColor=(ImageData[Address]<<16)|(ImageData[Address+1]<<8)|ImageData[Address];
  OldX=x1;
  //for (j=x1+1;j<x2;j++)
  for (j=x1;j<x2;j++)         //By zjh
  {
      TmpFixedX.value++;
      FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
      FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

      Address=((long)TmpY*RealWidth+TmpX)*3;
      NewColor=(ImageData[Address]<<16)|(ImageData[Address+1]<<8)|ImageData[Address];
      if (NewColor!=OldColor)
      {
         int k,l;

         for (k=0,l=j-OldX;k<l;k++)
         {
             *(ImageNewData++)=OldColor>>16;
             *(ImageNewData++)=OldColor>>8;
             *(ImageNewData++)=OldColor;
         }
         OldX=j;
         OldColor=NewColor;
      }
  }
  {
     int k,l;

     for (k=0,l=j-OldX;k<l;k++)
     {
         *(ImageNewData++)=OldColor>>16;
         *(ImageNewData++)=OldColor>>8;
         *(ImageNewData++)=OldColor;
     }
  }
  return;
}
#endif  //UNUSED


/*------- bellow functions are called only when painting on screen --------*/
static int AdjustImageNewData(int len)
{
    if(ImageNewData+len>ImageBufEnd)    // it need to be adjust
    {
       char *ImageBufStart,*pNewImage;
       int  OldLen,NewLen;
       HANDLE hNewHandle;

        //------- HandleRealloc ----------
       ImageBufStart=HandleLock(ImageHandle);
       if(ImageBufStart==0)
          return 0;

       OldLen = ImageNewData - ImageBufStart;
       //NewLen=OldLen+len+256;           // new buffer's length
       NewLen=OldLen+len+1024;            //By zjh for more speed
       hNewHandle=HandleAlloc(NewLen,0);
       if(hNewHandle==0)
       {
          HandleUnlock(ImageHandle);
          return 0;
       }
       pNewImage=HandleLock(hNewHandle);
       if (pNewImage==NULL)
       {
          HandleFree(hNewHandle);
          HandleUnlock(ImageHandle);
          return 0;
       }

       memset(pNewImage,0,NewLen);    // copy old buffer to new buffer
       //memcpy(pNewImage,ImageBufStart,OldLen);
       memcpy(pNewImage,ImageBufStart,OldLen+1);     //For lose point By zjh 10.16

       HandleUnlock(ImageHandle);
       HandleFree(ImageHandle);

       ImageHandle=hNewHandle;          // change all pointer and handle
       ImageBufEnd=pNewImage+NewLen-1;
       ImageNewData=pNewImage+OldLen;
    }

    return 1;
}

#define TEST_LINE(a,b,c,d)

#ifdef DEBUG
void _TEST_LINE(int sx,int ex,int y,int color)
{
   setviewport(0,0,639,479,0);
   if (y==0)
    {
        setfillstyle(1,0);
        bar(0,0,600,300);
    }
   setwritemode(COPY_PUT);
   //setcolor(color);
   setfillstyle(1,color);
   sx=sx%200;
   ex=ex%200;
   bar(sx*3,y*3,ex*3+2,y*3+2);
   //line(sx,y,ex,y);
   //if (getch()==27) ;
}
#endif

void ImageSlantRotateFillLine16(int x1,int x2,int y,LPDC lpdc)
{
  int j,OldX,TmpX,TmpY,len;
  unsigned char OldColor,NewColor;
  long Address;
  FIXED TmpFixedX,TmpFixedY,TmpFixed1,TmpFixed2,TmpFixed3;

  /*
  {
    setviewport(0,0,639,479,0);
    for (j=x1;j<=x2;j++)
      {
       Address=((long)y*RealWidth+j)>>1;
       OldColor=ImageData[Address];
       if ( (j&1)!=0 )
       OldColor&=0xf;
        else
       OldColor=(OldColor>>4)&0xf;
       setcolor(OldColor);
       line(j,y,j,y);
      }
  }
  */

  if(!AdjustImageNewData(4))    // Added ByHance, 96,4.14
      return;

  ImageLineBufferSetLeft(ImageNewData,x1);
  ImageLineBufferSetLength(ImageNewData,x2-x1+1);
  ImageNewData+=4;                     /* Save Image line left and width */

  Int2Fixed(TmpFixedY,(y-ImageAxisY));
  Int2Fixed(TmpFixedX,(x1-ImageAxisX));

  FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
  FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

  Address=((long)TmpY*RealWidth+TmpX)>>1;
  OldColor=ImageData[Address];
  OldX=x1;

  // if (TmpX>>1)               // ByHance, Error! 96,2.5
  if ( (TmpX&1)!=0 )
     OldColor&=0xf;
  else
     OldColor=(OldColor>>4)&0xf;

  //if (x2>x1)
  if (x2>=x1)    //By zjh
  {
     //for (j=x1+1;j<=x2;j++)
     for (j=x1;j<=x2;j++)             //By zjh 10.15,96
     {
         //TmpFixedX.value++;
         FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
         FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
         FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
         TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
         FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
         FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
         FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
         TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

         if (TmpY!=y||j!=TmpX)
          {
             //printf("\7error");
          }

         Address=((long)TmpY*RealWidth+TmpX)>>1;
         NewColor=ImageData[Address];
         // if (TmpX>>1)               // ByHance, Error! 96,2.5
         if ((TmpX&1)!=0)
            NewColor&=0xf;
         else
            NewColor=(NewColor>>4)&0xf;

         if (NewColor!=OldColor)
         {

            if( (OldX&1)!=0 )
            {
               if(!AdjustImageNewData(1))    // Added ByHance, 96,4.14
                   return;
               TEST_LINE(OldX,OldX,y,OldColor);
               *ImageNewData++ |= OldColor;
               //*ImageNewData++ |= 12;
               OldX++;
            }
            len=(j>>1)-(OldX>>1);        // ByHance,96,2.5
            if(len>0)
            {
               if(!AdjustImageNewData(len))    // Added ByHance, 96,4.14
                   return;

               memset(ImageNewData,((OldColor<<4)|OldColor),len);
               //memset(ImageNewData,0x11,len);
               TEST_LINE(OldX,(j&0xfffe),y,OldColor);
               ImageNewData+=len;
            }
            //else               //By zjh
            if( (j&1)!=0 )
               {
               TEST_LINE(j,j,y,OldColor);
               *ImageNewData |= (OldColor<<4);
               //*ImageNewData |= 0xe0;
               }
            OldX=j;
            OldColor=NewColor;
         }
         TmpFixedX.value++;
     } /*--- for j ----*/

     if( (OldX&1)!=0 )
     {
        if(!AdjustImageNewData(1))    // Added ByHance, 96,4.14
            return;

        *ImageNewData++ |= OldColor;
        OldX++;
     }
     len=(j>>1)-(OldX>>1);        // ByHance,96,2.5
     if(len>0)
     {
        if(!AdjustImageNewData(len))    // Added ByHance, 96,4.14
            return;

        memset(ImageNewData,((OldColor<<4)|OldColor),len);
        ImageNewData+=len;
     }
     if( (j&1)!=0 )
     {
        if(!AdjustImageNewData(1))    // Added ByHance, 96,4.14
            return;

        *ImageNewData++ |= (OldColor<<4);
     }
  }
  /*
  else
  {
     if(!AdjustImageNewData(1))    // Added ByHance, 96,4.14
         return;

     if( (OldX&1)!=0 )
        *ImageNewData++ |= OldColor;
     else
        *ImageNewData++ |= (OldColor<<4);
  }
  */
}
/*------------- end painting on screen functions -----------*/

/*------- bellow functions are called only when printing --------*/
void ImageDrawFillLineBW(int x1,int x2,int y,LPDC lpdc)
{
  int j,OldX,TmpX,TmpY;
  unsigned char OldColor,NewColor;
  long Address;
  FIXED TmpFixedX,TmpFixedY,TmpFixed1,TmpFixed2,TmpFixed3;

  Int2Fixed(TmpFixedY,(y-ImageOriginY-ZoomImageAxisY));
  Int2Fixed(TmpFixedX,(x1-ImageOriginX-ZoomImageAxisX));

  FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
  FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

  Address=(long)TmpY*((RealWidth+7)>>3)+(TmpX>>3);
  // Bits=7-TmpX%8;
  // OldColor=(ImageData[Address]&BitMatrixs[Bits])>>Bits;
  OldColor=0;
  if(ImageData[Address]&dot1tab[TmpX&7])
     OldColor=1;

  OldX=x1;
  //for (j=x1+1;j<x2;j++)
  for (j=x1;j<=x2;j++)    //By zjh for Rorate90
  {
      TmpFixedX.value++;
      FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
      FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

      Address=(long)TmpY*((RealWidth+7)>>3)+(TmpX>>3);
      //Bits=7-TmpX%8;
      //NewColor=(ImageData[Address]&BitMatrixs[Bits])>>Bits;
      NewColor=0;
      if(ImageData[Address]&dot1tab[TmpX&7])
         NewColor=1;

      if (NewColor!=OldColor)
      {
    #ifdef UNUSED   // ByHance, 96,1.30
         if ((OldColor&&!NegativeSign)||(!OldColor&&NegativeSign))
    #else
         if(OldColor)
    #endif
         {
            if (ColorSign && printer->DeviceType!=DEV_BW)
               SetDeviceColor(ColorSign,0);
            else
               SetDeviceColor(EGA_BLACK,0);

            RastLineClipLine(OldX,j,y);
         }

         OldX=j;
         OldColor=NewColor;
      }
  }

  if (OldX<j-1)
  {
  #ifdef UNUSED   // ByHance, 96,1.30
     if ((OldColor&&!NegativeSign)||(!OldColor&&NegativeSign))
  #else
     if(OldColor)
  #endif
     {
        if (ColorSign && printer->DeviceType!=DEV_BW)
           SetDeviceColor(ColorSign,0);
        else
           SetDeviceColor(EGA_BLACK,0);

        RastLineClipLine(OldX,j,y);
     }
  }
  return;
}

void ImageDrawFillLine16(int x1,int x2,int y,LPDC lpdc)
{
  int j,OldX,TmpX,TmpY;
  unsigned char OldColor,NewColor,color;
  int  R,G,B;
  long Address;
  FIXED TmpFixedX,TmpFixedY,TmpFixed1,TmpFixed2,TmpFixed3;

  Int2Fixed(TmpFixedY,(y-ImageOriginY-ZoomImageAxisY));
  Int2Fixed(TmpFixedX,(x1-ImageOriginX-ZoomImageAxisX));

  FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
  FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

  Address=((long)TmpY*RealWidth+TmpX)>>1;
  OldColor=ImageData[Address];
  OldX=x1;
  // if (TmpX>>1)               // ByHance, Error! 96,2.5
  if( (TmpX&1)!=0 )
     OldColor&=0xf;
  else
     OldColor=(OldColor>>4)&0xf;

#ifdef UNUSED   // ByHance, 96,1.30
  if (ContrastSign)
     OldColor=GetPosterizedGray(OldColor);
#endif

  //for (j=x1+1;j<x2;j++)
  for (j=x1;j<=x2;j++)   //By zjh
  {
      TmpFixedX.value++;
      FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
      FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

      Address=((long)TmpY*RealWidth+TmpX)>>1;
      NewColor=ImageData[Address];
      // if (TmpX>>1)               // ByHance, Error! 96,2.5
      if( (TmpX&1)!=0 )
         NewColor&=0xf;
      else
         NewColor=(NewColor>>4)&0xf;

    #ifdef UNUSED   // ByHance, 96,1.30
      if (ContrastSign)
         NewColor=GetPosterizedGray(NewColor);
    #endif

      if (NewColor!=OldColor)
      {
         color=OldColor;
    #ifdef UNUSED   // ByHance, 96,1.30
         if (NegativeSign)
            color=(~color)&0xff;
    #endif

         if (RGBPalatteSign)
         {
            R=(ByteRGBPalatte[3*color]);
            G=(ByteRGBPalatte[3*color+1]);
            B=(ByteRGBPalatte[3*color+2]);
         }
         else
            R=G=B=color*16;             // change to [0..255]

         printer->printSetRGBcolor(R,G,B);

         RastLineClipLine(OldX,j,y);
         OldX=j;
         OldColor=NewColor;
      }
  }

  if (OldX<j-1)
  {
     color=OldColor;
  #ifdef UNUSED   // ByHance, 96,1.30
     if (NegativeSign)
        color=(~color)&0xff;
   #endif

     if (RGBPalatteSign)
     {
        R=(ByteRGBPalatte[3*color]);
        G=(ByteRGBPalatte[3*color+1]);
        B=(ByteRGBPalatte[3*color+2]);
     }
     else
        R=G=B=color*16;             // change to [0..255]

     printer->printSetRGBcolor(R,G,B);

     RastLineClipLine(OldX,j,y);
  }
}

void ImageDrawFillLine256(int x1,int x2,int y,LPDC lpdc)
{
  int j,OldX,TmpX,TmpY;
  unsigned char OldColor,NewColor,color;
  int  R,G,B;
  long Address;
  FIXED TmpFixedX,TmpFixedY,TmpFixed1,TmpFixed2,TmpFixed3;

  Int2Fixed(TmpFixedY,(y-ImageOriginY-ZoomImageAxisY));
  Int2Fixed(TmpFixedX,(x1-ImageOriginX-ZoomImageAxisX));

  FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
  FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

  Address=(long)TmpY*RealWidth+TmpX;
  OldColor=ImageData[Address];

#ifdef UNUSED   // ByHance, 96,1.30
  if (ContrastSign)
     OldColor=GetPosterizedGray(OldColor);
#endif

  OldX=x1;
  //for (j=x1+1;j<x2;j++)
  for (j=x1;j<=x2;j++)   //By zjh
  {
      TmpFixedX.value++;
      FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
      FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

      Address=(long)TmpY*RealWidth+TmpX;
      NewColor=ImageData[Address];

    #ifdef UNUSED   // ByHance, 96,1.30
      if (ContrastSign)
         NewColor=GetPosterizedGray(NewColor);
    #endif

      if (NewColor!=OldColor)
      {
         color=OldColor;
    #ifdef UNUSED   // ByHance, 96,1.30
         if (NegativeSign)
             color=(~color)&0xff;
    #endif

         if (RGBPalatteSign)
         {
            R=(ByteRGBPalatte[3*color]);
            G=(ByteRGBPalatte[3*color+1]);
            B=(ByteRGBPalatte[3*color+2]);
         }
         else
            R=G=B=color;

         printer->printSetRGBcolor(R,G,B);

         RastLineClipLine(OldX,j,y);
         OldX=j;
         OldColor=NewColor;
      }
  }

  if (OldX<j-1)
  {
     color=OldColor;
   #ifdef UNUSED   // ByHance, 96,1.30
     if (NegativeSign)
        color=(~color)&0xff;
   #endif

     if (RGBPalatteSign)
     {
        R=(ByteRGBPalatte[3*color]);
        G=(ByteRGBPalatte[3*color+1]);
        B=(ByteRGBPalatte[3*color+2]);
     }
     else
        R=G=B=color;

     printer->printSetRGBcolor(R,G,B);

     RastLineClipLine(OldX,j,y);
  }
}

void ImageDrawFillLineTrueColor(int x1,int x2,int y,LPDC lpdc)
{
  int j,OldX,TmpX,TmpY;
 //  unsigned char OldColor,NewColor,color;
  int  R,G,B,OldR,OldG,OldB,r,g,b;
  long Address;
  FIXED TmpFixedX,TmpFixedY,TmpFixed1,TmpFixed2,TmpFixed3;

  Int2Fixed(TmpFixedY,(y-ImageOriginY-ZoomImageAxisY));
  Int2Fixed(TmpFixedX,(x1-ImageOriginX-ZoomImageAxisX));

  FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
  FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
  FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

  Address=((long)TmpY*RealWidth+TmpX)*3;
  //OldColor=((unsigned long)ImageData[Address]<<16)
    //       |((unsigned long)ImageData[Address+1]<<8)
      //     |(unsigned long)ImageData[Address+2];
  OldR=ImageData[Address];
  OldG=ImageData[Address+1];
  OldB=ImageData[Address+2];

  OldX=x1;
  //for (j=x1+1;j<x2;j++)
  for (j=x1;j<=x2;j++)   //By zjh
  {
      TmpFixedX.value++;
      FixedMul(&(ImageMatrix->eM11),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM12),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpX=IntofFixed(TmpFixed3)+ImageAxisX;
      FixedMul(&(ImageMatrix->eM21),&TmpFixedX,&TmpFixed1);
      FixedMul(&(ImageMatrix->eM22),&TmpFixedY,&TmpFixed2);
      FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
      TmpY=IntofFixed(TmpFixed3)+ImageAxisY;

      Address=((long)TmpY*RealWidth+TmpX)*3;
      // NewColor=((unsigned long)ImageData[Address]<<16)
         //       |((unsigned long)ImageData[Address+1]<<8)
           //     |(unsigned long)ImageData[Address+2];
      // if (NewColor!=OldColor)
      R=ImageData[Address];
      G=ImageData[Address+1];
      B=ImageData[Address+2];
      if(R!=OldR || G!=OldG || B!=OldB)
      {
         r=OldR;   g=OldG;  b=OldB;
    #ifdef UNUSED   // ByHance, 96,1.30
         if (NegativeSign)
         {
            r=(~r)&0xff;
            g=(~g)&0xff;
            b=(~b)&0xff;
         }
    #endif

         printer->printSetRGBcolor(r,g,b);

         RastLineClipLine(OldX,j,y);
         OldX=j;
         //OldColor=NewColor;
         OldR=R;  OldG=G;  OldB=B;
      }
  }

  if (OldX<j-1)
  {
     r=OldR;   g=OldG;  b=OldB;

   #ifdef UNUSED   // ByHance, 96,1.30
     if (NegativeSign)
     {
        r=(~r)&0xff;
        g=(~g)&0xff;
        b=(~b)&0xff;
     }
   #endif

     printer->printSetRGBcolor(r,g,b);

     RastLineClipLine(OldX,j,y);
  }
}
/*------------- end printing functions -----------*/

void SetImageNoClipLineFill(int x1,int x2,int y,LPDC lpdc)
{
  int TmpLength,TmpAddress;
  int byte1,byte2;

  if (x1>x2)
  {
     int tmp=x2;
     x2=x1;
     x1=tmp;
  }

  if (PrintingSign&&GlobalRorate90)
  {
  x1-=myDC.left;
  x2-=myDC.left;
  y-=myDC.top;
  byte1 = x1>>3;
  byte2 = x2>>3;
  TmpAddress=y*((myDC.right-myDC.left+7)/8)+byte1;
  }
  else
  {
  x1-=lpdc->left;
  x2-=lpdc->left;
  y-=lpdc->top;
  byte1 = x1>>3;
  byte2 = x2>>3;
  TmpAddress=y*RastWidthByte+byte1;
  }

  if (byte1==byte2)
  {
     ImageClipData[TmpAddress]|=headdot[(x1&7)]&taildot[(x2&7)];
     //ImageClipData[TmpAddress]|=BitFill[7-(x1&7)]&(~BitFill[7-(x2&7)]);
     return;
  }

  ImageClipData[TmpAddress]|=headdot[(x1&7)];
  TmpAddress++; byte1++;

  TmpLength=byte2-byte1;
  if (TmpLength>0)
  {
     memset(&ImageClipData[TmpAddress],0xff,TmpLength);
     TmpAddress+=TmpLength;
  }

  ImageClipData[TmpAddress]|=taildot[(x2&7)];
}

void SetImageClipLineFill(int x1,int x2,int y,LPDC lpdc)
{
  int TmpLength,TmpAddress;
  int byte1,byte2;

  if (x1>x2)
  {
     int tmp=x2;
     x2=x1;
     x1=tmp;
  }

  if (PrintingSign&&GlobalRorate90)
  {
  x1-=myDC.left;
  x2-=myDC.left;
  y-=myDC.top;
  byte1 = x1>>3;
  byte2 = x2>>3;
  TmpAddress=y*((myDC.right-myDC.left+7)/8)+byte1;
  }
  else
  {
  x1-=lpdc->left;
  x2-=lpdc->left;
  y-=lpdc->top;
  byte1 = x1>>3;
  byte2 = x2>>3;
  TmpAddress=y*RastWidthByte+byte1;
  }


  if (byte1==byte2)
  {
     ImageClipData[TmpAddress]&=~( headdot[(x1&7)]&taildot[(x2&7)] );
     //ImageClipData[TmpAddress]&=~((BitFill[7-(x1&7)])&(~BitFill[7-(x2&7)]));
     return;
  }

  //ImageClipData[TmpAddress]&=taildot[(x1&7)];
  ImageClipData[TmpAddress]&=~headdot[(x1&7)];
  TmpAddress++; byte1++;

  TmpLength=byte2-byte1;
  if (TmpLength>0)
  {
     memset(&ImageClipData[TmpAddress],0,TmpLength);
     TmpAddress+=TmpLength;
  }

  //ImageClipData[TmpAddress]&=headdot[(x2&7)];
  ImageClipData[TmpAddress]&=~taildot[(x2&7)];
}

