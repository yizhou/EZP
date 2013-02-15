/*-------------------------------------------------------------------
* Name: statictc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static void DrawStaticText(HWND Window,int DrawLeft,int DrawTop,int DrawRight,
                    int DrawBottom,int BackgroundColor)
{
  int SaveColor;
  struct viewporttype TmpViewPort;
  int Left,Top,Right,Bottom;

  MouseHidden();
  SaveColor=getcolor();
  getviewsettings(&TmpViewPort);

  WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
  WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
  DrawLeft+=Left;
  DrawRight+=Left;
  DrawTop+=Top;
  DrawBottom+=Top;
  setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);
  Left-=DrawLeft;
  Right-=DrawLeft;
  Top-=DrawTop;
  Bottom-=DrawTop;
  setfillstyle(1,BackgroundColor);
  bar(Left,Top,Right,Bottom);
  ViewportDisplayString(WindowGetTitle(Window),0,0,STATICTEXTCOLOR,
                        BackgroundColor);
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
}

static unsigned long StaticTextDefaultProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case DRAWWINDOW:
         DrawStaticText(Window,MAKEHI(Param1),MAKELO(Param1),
                        MAKEHI(Param2),MAKELO(Param2),
                        MessageGo(Window,STATICTEXTBKCOLOR,0l,0l));
         break;
    case STATICTEXTBKCOLOR:
         return(EGA_LIGHTGRAY);
    case KEYDOWN:
         if (Param1==ESC)
         {
            MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
            break;
         }
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

#ifdef UNUSED           // ByHance, 96,1.30
static unsigned long WhiteStaticTextDefaultProcedure(HWND Window,HMSG Message,
              long Param1,long Param2)
{
  switch (Message)
  {
    case STATICTEXTBKCOLOR:
         return(EGA_WHITE);
    default:
         return(StaticTextDefaultProcedure(Window,Message,Param1,Param2));
  }
}
#endif      // UNUSED           // ByHance, 96,1.30

HWND CreatStaticText(int Left,int Top,int Right,char *Title,
                     Function *StaticTextProcedure,HWND FatherWindow)
{
  Windows TobeCreatWindow;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));

  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Right;
  TobeCreatWindow.Bottom=Top+SYSBUTTONWIDTH;
  if (StaticTextProcedure==NULL)
     TobeCreatWindow.Procedure=(Function *)StaticTextDefaultProcedure;
  else
     TobeCreatWindow.Procedure=StaticTextProcedure;
  strcpy(TobeCreatWindow.Title,Title);
  TobeCreatWindow.WindowStyle=3;

  return(WindowAppend(&TobeCreatWindow,FatherWindow));
}
