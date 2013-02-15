/*-------------------------------------------------------------------
* Name: framec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

void DrawFrame(HWND Window,int DrawLeft,int DrawTop,
               int DrawRight,int DrawBottom)
{
  int SaveColor;
  struct viewporttype TmpViewPort;
  int Left,Top,Right,Bottom;

  MouseHidden();
  SaveColor=getcolor();
  getviewsettings(&TmpViewPort);

  WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);

  DrawLeft+=Left;
  DrawTop+=Top;
  DrawRight+=Left;
  DrawBottom+=Top;

  setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);

  Left-=DrawLeft;
  Top-=DrawTop;
  Right-=DrawLeft;
  Bottom-=DrawTop;

  setfillstyle(1,FRAMEBKCOLOR);
  bar(Left,Top,Right,Bottom);
  setcolor(FRAMECOLOR);

  Left+=2;
  Top+=2+CHARHEIGHT/2;
  Right-=2;
  Bottom-=2;

  if (Left<=2)
     Vline3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,Left,Top,Bottom);
  if (Right>=Right-Left-3)
     Vline3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,Right-1,Top,Bottom);
  if (Top<=2+CHARHEIGHT/2)
     Hline3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,Left+1,Right,Top);
  if (DrawBottom>=Bottom-Top-3)
     Hline3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,Left+2,Right-1,Bottom);

  ViewportDisplayString(WindowGetTitle(Window),Left+8,Top-CHARHEIGHT/2,
                        FRAMECOLOR,FRAMEBKCOLOR);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
  return;
}

static long FrameDefaultProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case DRAWWINDOW:
         DrawFrame(Window,(short)MAKEHI(Param1),(short)MAKELO(Param1),
                   (short)MAKEHI(Param2),(short)MAKELO(Param2));
         break;
    case MOUSEMOVE:
         DialogMouseMove(Window,Message,Param1,Param2); // ByHance, 95,12.6
         break;
    case KEYDOWN:
         if (Param1==ESC||Param1==ENTER)
         {
            MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
            break;
         }
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

int CreatFrameWindow(int Left,int Top,int Right,int Bottom,
                     char *Title,Function *FrameProcedure,
                     HWND FatherWindow)
{
  Windows TobeCreatWindow;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));

  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Right;
  TobeCreatWindow.Bottom=Bottom;
  if (FrameProcedure==NULL)
     TobeCreatWindow.Procedure=(Function *)FrameDefaultProcedure;
  else
     TobeCreatWindow.Procedure=FrameProcedure;
  strcpy(TobeCreatWindow.Title,Title);
//  TobeCreatWindow.WindowStyle=3|WindowSetCanTabOrder();
  TobeCreatWindow.WindowStyle=3;      // ByHance, 95,11.22

  return(WindowAppend(&TobeCreatWindow,FatherWindow));
}

