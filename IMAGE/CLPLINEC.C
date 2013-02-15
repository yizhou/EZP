/*-------------------------------------------------------------------
* Name: clplinec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static HANDLE ScreenHandle=0;
static char  *ScreenBuffer;
//static int BoundaryLeft=0,BoundaryTop=0;

//static ScreenModes ScreenMode=MODE640X480X16;
static int BoundaryRight=640,BoundaryBottom=480;
static unsigned short ScreenWidthDots[]={ 640, 800, 1024, 1024 };
static unsigned short ScreenWidth[]={ 640/8, 800/8, 1024/8, 1024/8 };
static unsigned short ScreenHeight[]={ 480, 600, 768, 768 };
static unsigned long ScreenSizes[]=
    { 640l/8*480, 800l/8*600, 1024l/8*768, 1024l/8*768 };

int getmaxx() { return (ScreenWidthDots[ScreenMode]-1); }
int getmaxy() { return (ScreenHeight[ScreenMode]-1); }
int getmaxcolor() { return(16-1); }
/*-------------------------------------*/

void Init_dc(void)
{
/*--------------
  int MaxX,MaxY,MaxColor;

  MaxX=getmaxx();
  MaxY=getmaxy();
  MaxColor=getmaxcolor();

  if ((MaxX==639)&&(MaxY==479)&&(MaxColor==15))
     ScreenMode=MODE640X480X16;
  else
  if ((MaxX==799)&&(MaxY==5990)&&(MaxColor==15))
     ScreenMode=MODE800X600X16;
  else
  if ((MaxX==1023)&&(MaxY==767)&&(MaxColor==15))
     ScreenMode=MODE1024X768X16;
  else
  if ((MaxX==1023)&&(MaxY==767)&&(MaxColor==255))
     ScreenMode=MODE1024X768X256;
-------------------------*/
}

int Init_template(void)
{
  if (ScreenHandle==0)
  {
     ScreenHandle=HandleAlloc(ScreenSizes[ScreenMode],0);
     if (ScreenHandle!=0)
        ScreenBuffer=HandleLock(ScreenHandle);
     if (ScreenBuffer==NULL)
     {
        HandleFree(ScreenHandle);
        ScreenHandle=0;
     }
     else
        memset(ScreenBuffer,0,ScreenSizes[ScreenMode]);
  }
  return(ScreenHandle);
}

void FinishTemplate(void)
{
  if (ScreenHandle!=0)
  {
     if (ScreenBuffer)
        HandleUnlock(ScreenHandle);
     HandleFree(ScreenHandle);
     ScreenHandle=0;
  }
}

void SetDeviceBoundary(/*int Left,int Top,*/ int Right,int Bottom)
{
  //BoundaryLeft=Left;
  //BoundaryTop=Top;
  BoundaryRight=Right;
  BoundaryBottom=Bottom;
  //setviewport(Left,Top,Right,Bottom,1);
}

void SetViewportPolygon(int BoxDots,int *BoxXY)
{
  unsigned char *SaveImageData;
  int SaveSysDCWidth,SaveLeft,SaveTop,SaveRight,SaveBottom;
  LineFillLine *SaveLineFillLine;
  EdgeFillLine *SaveEdgeFillLine;
  struct viewporttype GetVp;

  SaveImageData=ImageClipData;
  ImageClipData=ScreenBuffer;
  SaveLineFillLine=CurrentLineFillLine;
  SaveEdgeFillLine=CurrentEdgeFillLine;

  CurrentLineFillLine=SetImageNoClipLineFill;
  CurrentEdgeFillLine=NULL;         // PolyEdgeFillLine; ByHance, 96,1.29
  SaveSysDCWidth=RastWidthByte;
  RastWidthByte=ScreenWidth[ScreenMode];
  SaveLeft=SysDc.left;
  SaveTop=SysDc.top;
  SaveRight=SysDc.right;
  SaveBottom=SysDc.bottom;
  getviewsettings(&GetVp);
  SysDc.left=GetVp.left;
  SysDc.top=GetVp.top;
  SysDc.right=GetVp.right;
  SysDc.bottom=GetVp.bottom;

  FillPolygon(&SysDc,(LPPOINT)BoxXY,BoxDots);

  SysDc.left=SaveLeft;
  SysDc.top=SaveTop;
  SysDc.right=SaveRight;
  SysDc.bottom=SaveBottom;
  RastWidthByte=SaveSysDCWidth;
  CurrentLineFillLine=SaveLineFillLine;
  CurrentEdgeFillLine=SaveEdgeFillLine;
  ImageClipData=SaveImageData;
}

void SetClipportPolygon(int BoxDots,int *BoxXY)
{
  unsigned char *SaveImageData;
  int SaveSysDCWidth,SaveLeft,SaveTop,SaveRight,SaveBottom;
  LineFillLine *SaveLineFillLine;
  EdgeFillLine *SaveEdgeFillLine;
  struct viewporttype GetVp;

  SaveImageData=ImageClipData;
  ImageClipData=ScreenBuffer;
  SaveLineFillLine=CurrentLineFillLine;
  SaveEdgeFillLine=CurrentEdgeFillLine;

  CurrentLineFillLine=SetImageClipLineFill;
  CurrentEdgeFillLine=NULL;        // PolyEdgeFillLine; ByHance, 96,1.29
  SaveSysDCWidth=RastWidthByte;
  RastWidthByte=ScreenWidth[ScreenMode];
  SaveLeft=SysDc.left;
  SaveTop=SysDc.top;
  SaveRight=SysDc.right;
  SaveBottom=SysDc.bottom;
  getviewsettings(&GetVp);
  SysDc.left=GetVp.left;
  SysDc.top=GetVp.top;
  SysDc.right=GetVp.right;
  SysDc.bottom=GetVp.bottom;

  FillPolygon(&SysDc,(LPPOINT)BoxXY,BoxDots);

  SysDc.left=SaveLeft;
  SysDc.top=SaveTop;
  SysDc.right=SaveRight;
  SysDc.bottom=SaveBottom;
  RastWidthByte=SaveSysDCWidth;
  CurrentLineFillLine=SaveLineFillLine;
  CurrentEdgeFillLine=SaveEdgeFillLine;
  ImageClipData=SaveImageData;
}

void HLine(int X1,int Y,int X2)
{                                      /* Need input X1<=X2 */
#define SLOW_VER
#ifdef SLOW_VER
  char FHUGE *LineClipLine;             /* Record the OR matrix line of the Y */
  int x,StartX,fLine;
#endif
  int color = getcolor();

  if( Y<0||Y>ScreenHeight[ScreenMode]
  || X1>=BoundaryRight
  || X2<0)
  //|| X2<BoundaryLeft)
     return;

  //line(X1,Y,X2,Y);
  //return ;

  // if (X1<BoundaryLeft)  X1=BoundaryLeft;
  if (X1<0)             // ByHance, 96,4.15
     X1=0;
  if (X2>=BoundaryRight)
     X2=BoundaryRight-1;

#ifdef SLOW_VER
  LineClipLine=&ScreenBuffer[Y*ScreenWidth[ScreenMode]];
  x=X1;  fLine=0;
  while (x<=X2)
  {
    if (LineClipLine[x>>3]&dot1tab[x&7])
    {
       if (!fLine)
       {
          StartX=x;
          fLine=1;
       }
    }
    else
    {
       if (fLine)
       {
          scan_line(StartX,x-1,Y,color);
//        line(StartX,Y,x-1,Y);
          StartX=x;
          fLine=0;
       }
    }
    x++;
  }

  if (fLine)
       scan_line(StartX,X2,Y,color);
#else
  copymono(&ScreenBuffer[Y*ScreenWidth[ScreenMode]],X1,Y,X2-X1+1,1,color);
#endif
}

#ifdef UNUSED
void SLine(int X1,int Y1,int X2,int Y2)
{
  if (Y1==Y2)
     HLine(X1,Y1,X2);
  /* else ...  Needn't implement */
}
#endif


/////////////add by zjh for more speedly in redrawing picture////////
/////////////////////////////////96.10.19////////////////////////////
char FillData[2048]=
{
     0,0,0,0,0,0,0,0,
     8,0,0,0,0,0,0,0,
     7,0,0,0,0,0,0,0,
     7,8,0,0,0,0,0,0,
     6,0,0,0,0,0,0,0,
     6,8,0,0,0,0,0,0,
     6,7,0,0,0,0,0,0,
     6,7,8,0,0,0,0,0,
     5,0,0,0,0,0,0,0,
     5,8,0,0,0,0,0,0,
     5,7,0,0,0,0,0,0,
     5,7,8,0,0,0,0,0,
     5,6,0,0,0,0,0,0,
     5,6,8,0,0,0,0,0,
     5,6,7,0,0,0,0,0,
     5,6,7,8,0,0,0,0,
     4,0,0,0,0,0,0,0,
     4,8,0,0,0,0,0,0,
     4,7,0,0,0,0,0,0,
     4,7,8,0,0,0,0,0,
     4,6,0,0,0,0,0,0,
     4,6,8,0,0,0,0,0,
     4,6,7,0,0,0,0,0,
     4,6,7,8,0,0,0,0,
     4,5,0,0,0,0,0,0,
     4,5,8,0,0,0,0,0,
     4,5,7,0,0,0,0,0,
     4,5,7,8,0,0,0,0,
     4,5,6,0,0,0,0,0,
     4,5,6,8,0,0,0,0,
     4,5,6,7,0,0,0,0,
     4,5,6,7,8,0,0,0,
     3,0,0,0,0,0,0,0,
     3,8,0,0,0,0,0,0,
     3,7,0,0,0,0,0,0,
     3,7,8,0,0,0,0,0,
     3,6,0,0,0,0,0,0,
     3,6,8,0,0,0,0,0,
     3,6,7,0,0,0,0,0,
     3,6,7,8,0,0,0,0,
     3,5,0,0,0,0,0,0,
     3,5,8,0,0,0,0,0,
     3,5,7,0,0,0,0,0,
     3,5,7,8,0,0,0,0,
     3,5,6,0,0,0,0,0,
     3,5,6,8,0,0,0,0,
     3,5,6,7,0,0,0,0,
     3,5,6,7,8,0,0,0,
     3,4,0,0,0,0,0,0,
     3,4,8,0,0,0,0,0,
     3,4,7,0,0,0,0,0,
     3,4,7,8,0,0,0,0,
     3,4,6,0,0,0,0,0,
     3,4,6,8,0,0,0,0,
     3,4,6,7,0,0,0,0,
     3,4,6,7,8,0,0,0,
     3,4,5,0,0,0,0,0,
     3,4,5,8,0,0,0,0,
     3,4,5,7,0,0,0,0,
     3,4,5,7,8,0,0,0,
     3,4,5,6,0,0,0,0,
     3,4,5,6,8,0,0,0,
     3,4,5,6,7,0,0,0,
     3,4,5,6,7,8,0,0,
     2,0,0,0,0,0,0,0,
     2,8,0,0,0,0,0,0,
     2,7,0,0,0,0,0,0,
     2,7,8,0,0,0,0,0,
     2,6,0,0,0,0,0,0,
     2,6,8,0,0,0,0,0,
     2,6,7,0,0,0,0,0,
     2,6,7,8,0,0,0,0,
     2,5,0,0,0,0,0,0,
     2,5,8,0,0,0,0,0,
     2,5,7,0,0,0,0,0,
     2,5,7,8,0,0,0,0,
     2,5,6,0,0,0,0,0,
     2,5,6,8,0,0,0,0,
     2,5,6,7,0,0,0,0,
     2,5,6,7,8,0,0,0,
     2,4,0,0,0,0,0,0,
     2,4,8,0,0,0,0,0,
     2,4,7,0,0,0,0,0,
     2,4,7,8,0,0,0,0,
     2,4,6,0,0,0,0,0,
     2,4,6,8,0,0,0,0,
     2,4,6,7,0,0,0,0,
     2,4,6,7,8,0,0,0,
     2,4,5,0,0,0,0,0,
     2,4,5,8,0,0,0,0,
     2,4,5,7,0,0,0,0,
     2,4,5,7,8,0,0,0,
     2,4,5,6,0,0,0,0,
     2,4,5,6,8,0,0,0,
     2,4,5,6,7,0,0,0,
     2,4,5,6,7,8,0,0,
     2,3,0,0,0,0,0,0,
     2,3,8,0,0,0,0,0,
     2,3,7,0,0,0,0,0,
     2,3,7,8,0,0,0,0,
     2,3,6,0,0,0,0,0,
     2,3,6,8,0,0,0,0,
     2,3,6,7,0,0,0,0,
     2,3,6,7,8,0,0,0,
     2,3,5,0,0,0,0,0,
     2,3,5,8,0,0,0,0,
     2,3,5,7,0,0,0,0,
     2,3,5,7,8,0,0,0,
     2,3,5,6,0,0,0,0,
     2,3,5,6,8,0,0,0,
     2,3,5,6,7,0,0,0,
     2,3,5,6,7,8,0,0,
     2,3,4,0,0,0,0,0,
     2,3,4,8,0,0,0,0,
     2,3,4,7,0,0,0,0,
     2,3,4,7,8,0,0,0,
     2,3,4,6,0,0,0,0,
     2,3,4,6,8,0,0,0,
     2,3,4,6,7,0,0,0,
     2,3,4,6,7,8,0,0,
     2,3,4,5,0,0,0,0,
     2,3,4,5,8,0,0,0,
     2,3,4,5,7,0,0,0,
     2,3,4,5,7,8,0,0,
     2,3,4,5,6,0,0,0,
     2,3,4,5,6,8,0,0,
     2,3,4,5,6,7,0,0,
     2,3,4,5,6,7,8,0,
     1,0,0,0,0,0,0,0,
     1,8,0,0,0,0,0,0,
     1,7,0,0,0,0,0,0,
     1,7,8,0,0,0,0,0,
     1,6,0,0,0,0,0,0,
     1,6,8,0,0,0,0,0,
     1,6,7,0,0,0,0,0,
     1,6,7,8,0,0,0,0,
     1,5,0,0,0,0,0,0,
     1,5,8,0,0,0,0,0,
     1,5,7,0,0,0,0,0,
     1,5,7,8,0,0,0,0,
     1,5,6,0,0,0,0,0,
     1,5,6,8,0,0,0,0,
     1,5,6,7,0,0,0,0,
     1,5,6,7,8,0,0,0,
     1,4,0,0,0,0,0,0,
     1,4,8,0,0,0,0,0,
     1,4,7,0,0,0,0,0,
     1,4,7,8,0,0,0,0,
     1,4,6,0,0,0,0,0,
     1,4,6,8,0,0,0,0,
     1,4,6,7,0,0,0,0,
     1,4,6,7,8,0,0,0,
     1,4,5,0,0,0,0,0,
     1,4,5,8,0,0,0,0,
     1,4,5,7,0,0,0,0,
     1,4,5,7,8,0,0,0,
     1,4,5,6,0,0,0,0,
     1,4,5,6,8,0,0,0,
     1,4,5,6,7,0,0,0,
     1,4,5,6,7,8,0,0,
     1,3,0,0,0,0,0,0,
     1,3,8,0,0,0,0,0,
     1,3,7,0,0,0,0,0,
     1,3,7,8,0,0,0,0,
     1,3,6,0,0,0,0,0,
     1,3,6,8,0,0,0,0,
     1,3,6,7,0,0,0,0,
     1,3,6,7,8,0,0,0,
     1,3,5,0,0,0,0,0,
     1,3,5,8,0,0,0,0,
     1,3,5,7,0,0,0,0,
     1,3,5,7,8,0,0,0,
     1,3,5,6,0,0,0,0,
     1,3,5,6,8,0,0,0,
     1,3,5,6,7,0,0,0,
     1,3,5,6,7,8,0,0,
     1,3,4,0,0,0,0,0,
     1,3,4,8,0,0,0,0,
     1,3,4,7,0,0,0,0,
     1,3,4,7,8,0,0,0,
     1,3,4,6,0,0,0,0,
     1,3,4,6,8,0,0,0,
     1,3,4,6,7,0,0,0,
     1,3,4,6,7,8,0,0,
     1,3,4,5,0,0,0,0,
     1,3,4,5,8,0,0,0,
     1,3,4,5,7,0,0,0,
     1,3,4,5,7,8,0,0,
     1,3,4,5,6,0,0,0,
     1,3,4,5,6,8,0,0,
     1,3,4,5,6,7,0,0,
     1,3,4,5,6,7,8,0,
     1,2,0,0,0,0,0,0,
     1,2,8,0,0,0,0,0,
     1,2,7,0,0,0,0,0,
     1,2,7,8,0,0,0,0,
     1,2,6,0,0,0,0,0,
     1,2,6,8,0,0,0,0,
     1,2,6,7,0,0,0,0,
     1,2,6,7,8,0,0,0,
     1,2,5,0,0,0,0,0,
     1,2,5,8,0,0,0,0,
     1,2,5,7,0,0,0,0,
     1,2,5,7,8,0,0,0,
     1,2,5,6,0,0,0,0,
     1,2,5,6,8,0,0,0,
     1,2,5,6,7,0,0,0,
     1,2,5,6,7,8,0,0,
     1,2,4,0,0,0,0,0,
     1,2,4,8,0,0,0,0,
     1,2,4,7,0,0,0,0,
     1,2,4,7,8,0,0,0,
     1,2,4,6,0,0,0,0,
     1,2,4,6,8,0,0,0,
     1,2,4,6,7,0,0,0,
     1,2,4,6,7,8,0,0,
     1,2,4,5,0,0,0,0,
     1,2,4,5,8,0,0,0,
     1,2,4,5,7,0,0,0,
     1,2,4,5,7,8,0,0,
     1,2,4,5,6,0,0,0,
     1,2,4,5,6,8,0,0,
     1,2,4,5,6,7,0,0,
     1,2,4,5,6,7,8,0,
     1,2,3,0,0,0,0,0,
     1,2,3,8,0,0,0,0,
     1,2,3,7,0,0,0,0,
     1,2,3,7,8,0,0,0,
     1,2,3,6,0,0,0,0,
     1,2,3,6,8,0,0,0,
     1,2,3,6,7,0,0,0,
     1,2,3,6,7,8,0,0,
     1,2,3,5,0,0,0,0,
     1,2,3,5,8,0,0,0,
     1,2,3,5,7,0,0,0,
     1,2,3,5,7,8,0,0,
     1,2,3,5,6,0,0,0,
     1,2,3,5,6,8,0,0,
     1,2,3,5,6,7,0,0,
     1,2,3,5,6,7,8,0,
     1,2,3,4,0,0,0,0,
     1,2,3,4,8,0,0,0,
     1,2,3,4,7,0,0,0,
     1,2,3,4,7,8,0,0,
     1,2,3,4,6,0,0,0,
     1,2,3,4,6,8,0,0,
     1,2,3,4,6,7,0,0,
     1,2,3,4,6,7,8,0,
     1,2,3,4,5,0,0,0,
     1,2,3,4,5,8,0,0,
     1,2,3,4,5,7,0,0,
     1,2,3,4,5,7,8,0,
     1,2,3,4,5,6,0,0,
     1,2,3,4,5,6,8,0,
     1,2,3,4,5,6,7,0,
     1,2,3,4,5,6,7,8
};

int FindRowFillData(int Row,short *buff)
{
   char *p;
   char *data;
   int len=ScreenWidth[ScreenMode];
   register short i,last,ret=0;
   short curr;
   p=&ScreenBuffer[Row*len];
   last=-2;
   for (i=0;i<len;i++)
     if (*p)
       {
         if (*p==255)
         {
           if (!((*(p-1))&1))
            buff[ret++]=(i<<3);
           last=(i<<3)+7;
           if (!((*(p+1))&0x80))
            buff[ret++]=last;
           p++;
         }
         else
         {
           data=FillData+((*p++)<<3);
           while (*data)
            {
             curr=(i<<3)+(*data++)-1;
             if (curr!=last+1)
               buff[ret++]=curr;
             if ((*(data-1)+1!=(*data)&&*(data-1)!=8)||(*(data-1)==8&&(((*p)&0x80)==0)))
               buff[ret++]=curr;
             if (ret>1&&buff[ret-1]==buff[ret-2]) ret-=2;
             last=curr;
            }  /* while */
         }
       }
       else
        p++;
  return ret;
}


