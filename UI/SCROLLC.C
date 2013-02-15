/*-------------------------------------------------------------------
* Name: scrollc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

HWND CreatHScroll(int Left,int Top,int Right,Function *HScrollProcedure,
                  HWND FatherWindow)
{
  Windows TobeCreatWindow;
  HWND MidWin;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));

  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Right;
  TobeCreatWindow.Bottom=Top+SYSSCROLLWIDTH+2;
  TobeCreatWindow.Procedure=(Function *)WindowDefaultProcedure;
  TobeCreatWindow.WindowStyle=WindowSetIsHScroll();
  MidWin=WindowAppend(&TobeCreatWindow,FatherWindow);

  // left button
  CreatButton(0,0,SYSSCROLLWIDTH,SYSSCROLLWIDTH,WindowSetIsLeftScroll(),
              WINDOWLEFTSCROLL,"",NULL,MidWin);

  // right button
  CreatButton(WindowGetWidth(MidWin)-SYSSCROLLWIDTH,0,
              WindowGetWidth(MidWin),SYSSCROLLWIDTH,
              WindowSetIsRightScroll(),WINDOWRIGHTSCROLL,"",NULL,MidWin);

  // scroll button
  CreatButton(SYSSCROLLWIDTH,0,2*SYSSCROLLWIDTH,SYSSCROLLWIDTH,
              WindowSetIsHHScroll(),WINDOWHHSCROLL,"",HScrollProcedure,
              MidWin);
  return(MidWin);
}

HWND CreatVScroll(int Left,int Top,int Bottom,Function *VScrollProcedure,
                  HWND FatherWindow)
{
  Windows TobeCreatWindow;
  HWND MidWin;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));

  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Left+SYSSCROLLWIDTH+1;
  TobeCreatWindow.Bottom=Bottom;
  TobeCreatWindow.Procedure=(Function *)WindowDefaultProcedure;
  TobeCreatWindow.WindowStyle=WindowSetIsVScroll();
  MidWin=WindowAppend(&TobeCreatWindow,FatherWindow);

  // up button
  CreatButton(0,0,SYSSCROLLWIDTH,SYSSCROLLWIDTH,
              WindowSetIsUpScroll(),WINDOWUPSCROLL,"",NULL,MidWin);

  // down button
  CreatButton(0,WindowGetHeight(MidWin)-SYSSCROLLWIDTH-2,
              SYSSCROLLWIDTH,WindowGetHeight(MidWin)-2,
              WindowSetIsDownScroll(),WINDOWDOWNSCROLL,"",NULL,MidWin);

  // scroll button
  CreatButton(0,SYSSCROLLWIDTH,SYSSCROLLWIDTH,2*SYSSCROLLWIDTH,
              WindowSetIsVVScroll(),WINDOWVVSCROLL,"",VScrollProcedure,
              MidWin);

  return(MidWin);
}

static long StatusProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case MOUSELEFTDOUBLE:
         MessageInsert(1,MENUCOMMAND,MENU_GOTOPAGE,0);
         return 0L;
    case WINDOWINIT:
    case WMPAINT:
    case REDRAWMESSAGE:
    case DRAWWINDOW:
         if (GlobalCurrentPage > 0)
         {
           struct viewporttype TmpViewPort;
           int Left,Top,Right,Bottom;
           char msg[80];
           int pn;

           pn = PageHandleToNumber(GlobalCurrentPage)+1;
           if(TotalPage<pn) TotalPage=pn;       // ByHance, 96,4.4
           sprintf(msg,"ÇáËÉÅÅ°æ: µÚ%dÒ³,¹²%dÒ³",pn,TotalPage);
           if(fEditor)
           {        //-- "±à¼­" ---
              msg[4]='±'; msg[5]='à'; msg[6]='¼'; msg[7]='­';
              sprintf(&msg[strlen(msg)],"  ÐÐ:%d",CurrentRow);
           }
           if (TextIsOverwrite())
              strcat(msg,"  ¸ÄÐ´");

           WindowDefaultProcedure(Window,Message,Param1,Param2);
           getviewsettings(&TmpViewPort);
           MouseHidden();
           WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
           setcolor(EGA_BLACK);
           rectangle(Left,Top,Right,Bottom);
           setviewport(Left+1,Top+1,Right-1,Bottom-1,1);
           setfillstyle(1,EGA_LIGHTGRAY);
           bar(0,0,Right-Left-3,Bottom-Top-3);
           ViewportDisplayString(msg,5,1,EGA_BLACK,EGA_LIGHTGRAY);
           setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                   TmpViewPort.bottom,TmpViewPort.clip);
           MouseShow();
         }
         break;
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return 0L;
}

static HWND StatWin=0;

void TellStatus()
{
 MessageInsert(StatWin,WINDOWINIT,0l,0l);
}

HWND CreatStatusWin(int Left,int Top,int Right,HWND FatherWindow)
{
  Windows TobeCreatWindow;
  HWND MidWin;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));

  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Right;
  TobeCreatWindow.Bottom=Top+20;
  TobeCreatWindow.Procedure=(Function *)StatusProcedure;
  TobeCreatWindow.WindowStyle=WindowSetIsUserWindow();
  MidWin=WindowAppend(&TobeCreatWindow,FatherWindow);
  //MessageGo(MidWin,WINDOWINIT,0l,0l);
  StatWin = MidWin;
  return(MidWin);
}

void TellFileName()             // ByHance, 95,12.18
{
 #define X      32
 #define Y      8
 #define MAXLEN 26

   char name[128];
   int  SaveColor=getcolor();
   struct viewporttype ViewInformation;

   if(!DebugFileName[0])
      strcpy(name,"ÎÄ¼þÎ´ÃüÃû");
   else
   {  int len;
      strcpy(name,DebugFileName);
      if( (len=strlen(name))>MAXLEN)        // only display 32 chars
        memmove(name,&name[len-MAXLEN],MAXLEN+1);
      strupr(name);
   }

   getviewsettings(&ViewInformation);
   MouseHidden();
   setviewport(0,0,getmaxx(),getmaxy(),1);

   setcolor(EGA_BLUE);
   bar(X,Y,X+MAXLEN*ASC16WIDTH,Y+ASC16HIGHT);
   DisplayString(name,X,Y,EGA_WHITE,EGA_BLUE);

   setcolor(SaveColor);
   setviewport(ViewInformation.left,ViewInformation.top,
               ViewInformation.right,ViewInformation.bottom,
               ViewInformation.clip);
   MouseShow();

 #undef Y
 #undef X
 #undef MAXLEN
} /* TellFileName */
