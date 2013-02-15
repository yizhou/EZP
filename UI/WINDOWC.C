/*-------------------------------------------------------------------
* Name: windowc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static int WindowConstruct(void)
{
  HWND MidWindow;

  for (MidWindow=0;MidWindow<MAXWINDOWS;MidWindow++)
      if (WindowCanUse(MidWindow)) break;
  if (MidWindow>=MAXWINDOWS)
     Error(TOOMANYWINDOWS);
  return(MidWindow);
}

static int WindowDestruct(HWND WindowNumber)
{
  if(WindowNumber>=MAXWINDOWS)
       return(INVAILEDPARAM);
  ReturnOK();
}

int WindowGetRealRect(HWND WindowNumber,int *Left,int *Top,
                      int *Right,int *Bottom)
{
  HWND MidWindow;

  *Left=WindowGetLeft(WindowNumber);
  *Top=WindowGetTop(WindowNumber);
  *Right=WindowGetRight(WindowNumber);
  *Bottom=WindowGetBottom(WindowNumber);

  MidWindow=WindowGetFather(WindowNumber);

  while(MidWindow)
  {
    *Left+=WindowGetLeft(MidWindow);
    *Top+=WindowGetTop(MidWindow);
    *Right+=WindowGetLeft(MidWindow);
    *Bottom+=WindowGetTop(MidWindow);
    MidWindow=WindowGetFather(MidWindow);
  }
  ReturnOK();
}

int WindowGetFocus(int WindowNumber)
{
  HWND TmpWindow;

  if(WindowNumber>=MAXWINDOWS)
       return(INVAILEDPARAM);
  if (!WindowNumber)    // NULL_WIN
     ReturnOK();
  if (WindowGetFather(WindowNumber)!=0)
     WindowGetFocus(WindowGetFather(WindowNumber));

  if (WindowGetNext(WindowNumber)==0)   // if no brother
  {
     /*---- if father's_active_win is itself, need not active it ---*/
     if( WindowGetActive(WindowGetFather(WindowNumber)) == WindowNumber )
        ReturnOK();    // !!!!
     /*----------- set father's active win with this win ---*/
     WindowSetActive(WindowGetFather(WindowNumber),WindowNumber);
     return(WindowNumber);
  }

  /*--- if brother is top win(alway visible, and windows after this top win
        are visible too), then it is already active, need not active it  -*/
  if (WindowIsTopWindow(WindowGetNext(WindowNumber)))
     return(WindowNumber);

 /*----------- Now, active this win: move it to the place, which is before
         the 1st top window -------------------------------------*/
  /*---------- find 1st top window -----------*/
  TmpWindow=WindowGetActive(WindowGetFather(WindowNumber)); // the active win
  while (TmpWindow&&WindowIsTopWindow(TmpWindow))  // may be prev is top too
    TmpWindow=WindowGetPrev(TmpWindow);
  if (!TmpWindow)
     return(WindowNumber);

  /*---- pick up this win ----*/
  if (WindowGetPrev(WindowNumber)==0)    // it is 1st child
     WindowSetChild(WindowGetFather(WindowNumber),WindowGetNext(WindowNumber));
  else
     WindowSetNext(WindowGetPrev(WindowNumber),WindowGetNext(WindowNumber));
   /*------ double direction cross_linker, so set next's pointer -----------*/
  WindowSetPrev(WindowGetNext(WindowNumber),WindowGetPrev(WindowNumber));

  /*---- insert this win to correct place: tmp is prev of top win!! --*/
  WindowSetPrev(WindowNumber,TmpWindow);
  if(WindowGetNext(TmpWindow))
     WindowSetPrev(WindowGetNext(TmpWindow),WindowNumber);
  WindowSetNext(WindowNumber,WindowGetNext(TmpWindow));
  WindowSetNext(TmpWindow,WindowNumber);
  WindowSetActive(WindowGetFather(WindowNumber),WindowNumber);
  return(WindowNumber);
}

int WindowResize(HWND Window,int Left,int Top,int Right,int Bottom)
{
  HWND MidWin;
  int LeftSpace,RightSpace,TopSpace,BottomSpace;

  WindowSetLeft(Window,Left);
  WindowSetTop(Window,Top);
  WindowSetRight(Window,Right);
  WindowSetBottom(Window,Bottom);

  LeftSpace=TopSpace=LINESPACE;
  RightSpace=Right-Left-LINESPACE;
  BottomSpace=Bottom-Top-LINESPACE;

  MidWin=WindowGetChild(Window);
  // adjust right_upper's two buttons( max, min button )
  if (WindowCanMoveable(Window)&&WindowCanMiniumable(Window))
  {
     MidWin=WindowGetChild(Window);
     while (MidWin)
     {
       if (WindowIsMiniumButton(MidWin))
          break;
       else
          MidWin=WindowGetNext(MidWin);
     }

     if (MidWin!=0)
     {
        RightSpace-=SYSBUTTONWIDTH;
        WindowSetLeft(MidWin,RightSpace);
        WindowSetRight(MidWin,RightSpace+SYSBUTTONWIDTH);
     }
  }

  if (WindowCanMoveable(Window)&&WindowCanMaxiumable(Window))
  {
     MidWin=WindowGetChild(Window);
     while (MidWin)
     {
       if (WindowIsMaxiumButton(MidWin))
          break;
       else
          MidWin=WindowGetNext(MidWin);
     }
     if (MidWin!=0)
     {
        RightSpace-=SYSBUTTONWIDTH;
        WindowSetLeft(MidWin,RightSpace);
        WindowSetRight(MidWin,RightSpace+SYSBUTTONWIDTH);
     }
  }

  /*--- adjust title bar ----------*/
  if (WindowCanMoveable(Window))
  {
     MidWin=WindowGetChild(Window);
     while (MidWin)
     {
       if (WindowIsTitleBar(MidWin))
          break;
       else
          MidWin=WindowGetNext(MidWin);
     }
     if (MidWin!=0)
     {
        if (WindowHasSystemMenu(Window))
           LeftSpace+=SYSBUTTONWIDTH;
        WindowSetLeft(MidWin,LeftSpace);
        WindowSetRight(MidWin,RightSpace);
     }
  }

  /*-------- adjust menu place : only need adjust left and right ---*/
  if (WindowHasMenu(Window))
  {
     MidWin=WindowGetChild(Window);
     while (MidWin)
     {
       if (WindowIsMenuWindow(MidWin))
          break;
       else
          MidWin=WindowGetNext(MidWin);
     }
     if (MidWin!=0)
     {
        HMENU MidMenu;
        int MidLeft,MidTop;

        if (WindowCanMoveable(Window))
           LeftSpace=LINESPACE;
        else
           LeftSpace=1;
//      WindowSetRight(MidWin,WindowGetWidth(Window)-LeftSpace-1);     ???
        WindowSetRight(MidWin,WindowGetWidth(Window)-LeftSpace);
        MidMenu=WindowGetUserData(MidWin);
        while (MenuGetNext(MidMenu))
          MidMenu=MenuGetNext(MidMenu);
        MenuGetTopHeight(MidMenu,&MidLeft,&MidTop);
        WindowSetBottom(MidWin,MidTop+SYSBUTTONWIDTH+LeftSpace);
     }
  }

  /*------ adjust hor scroll bar ------------------*/
  if (WindowHasHScroll(Window))
  {
     MidWin=WindowGetChild(Window);
     while (MidWin)
     {
       if (WindowIsHScroll(MidWin))
          break;
       else
          MidWin=WindowGetNext(MidWin);
     }
     if (MidWin!=0)
     {
        RightSpace=Right-Left-LINESPACE;
        if (WindowHasVScroll(Window))
           RightSpace-=SYSSCROLLWIDTH;
        WindowSetRight(MidWin,RightSpace);
        WindowSetTop(MidWin,BottomSpace-SYSSCROLLWIDTH);
        WindowSetBottom(MidWin,BottomSpace);

        BottomSpace-=SYSSCROLLWIDTH;

        MidWin=WindowGetChild(MidWin);
        while (MidWin)
        {
          if (WindowIsRightScroll(MidWin))
             break;
          else
             MidWin=WindowGetNext(MidWin);
        }
        if (MidWin!=0)
        {
           WindowSetLeft(MidWin,WindowGetWidth(WindowGetFather(MidWin))-SYSSCROLLWIDTH-1);
           WindowSetRight(MidWin,WindowGetLeft(MidWin)+SYSSCROLLWIDTH);
        }
        MidWin=WindowGetChild(WindowGetFather(MidWin));
        while (MidWin)
        {
          if (WindowIsHHScroll(MidWin))
             break;
          else
             MidWin=WindowGetNext(MidWin);
        }
        if (MidWin!=0)
        {
           WindowSetLeft(MidWin,SYSSCROLLWIDTH);
           WindowSetRight(MidWin,2*SYSSCROLLWIDTH);
        }
     }
  }

  /*------ adjust ver scroll bar ------------------*/
  if (WindowHasVScroll(Window))
  {
     MidWin=WindowGetChild(Window);
     while (MidWin)
     {
       if (WindowIsVScroll(MidWin))
          break;
       else
          MidWin=WindowGetNext(MidWin);
     }
     if (MidWin!=0)
     {
        if (WindowCanResizeable(MidWin))
           TopSpace=LINESPACE;
        else
           TopSpace=1;
        if (WindowHasSystemMenu(MidWin)||WindowCanMoveable(MidWin))
           TopSpace+=SYSSCROLLWIDTH;
        if (WindowHasMenu(Window))
        {
           HWND MidWin2;

           MidWin2=WindowGetChild(Window);

           while (MidWin2)
           {
             if (WindowIsMenuWindow(MidWin2))
                break;
             else
                MidWin2=WindowGetNext(MidWin2);
           }
           if (MidWin2)
              TopSpace+=WindowGetHeight(MidWin2);
        }

        WindowSetLeft(MidWin,Right-Left-LINESPACE-SYSSCROLLWIDTH);
        WindowSetRight(MidWin,WindowGetLeft(MidWin)+SYSSCROLLWIDTH);
        WindowSetTop(MidWin,TopSpace);
        WindowSetBottom(MidWin,BottomSpace);

        MidWin=WindowGetChild(MidWin);
        while (MidWin)
        {
          if (WindowIsDownScroll(MidWin))
             break;
          else
             MidWin=WindowGetNext(MidWin);
        }
        if (MidWin!=0)
        {
           WindowSetTop(MidWin,WindowGetHeight(WindowGetFather(MidWin))-SYSSCROLLWIDTH-1);
           WindowSetBottom(MidWin,WindowGetTop(MidWin)+SYSSCROLLWIDTH);
        }
        MidWin=WindowGetChild(WindowGetFather(MidWin));
        while (MidWin)
        {
          if (WindowIsVVScroll(MidWin))
             break;
          else
             MidWin=WindowGetNext(MidWin);
        }
        if (MidWin!=0)
        {
           WindowSetTop(MidWin,SYSSCROLLWIDTH);
           WindowSetBottom(MidWin,2*SYSSCROLLWIDTH);
        }
     }
  }
  ReturnOK();
} /* WindowResize */


int WindowDraw(HWND WindowNumber,int DrawLeft,int DrawTop,int DrawRight,
               int DrawBottom)
{
  int SaveColor;
  struct viewporttype TmpViewPort;
  int Left,Top,Right,Bottom;

  if(WindowNumber>=MAXWINDOWS)
       return(INVAILEDPARAM);
  MouseHidden();
  SaveColor=getcolor();
  getviewsettings(&TmpViewPort);
  setviewport(0,0,getmaxx(),getmaxy(),1);

  WindowGetRealRect(WindowNumber,&Left,&Top,&Right,&Bottom);

  if (DrawLeft>=WindowGetWidth(WindowNumber)
      ||DrawTop>=WindowGetHeight(WindowNumber)
      ||DrawRight<0||DrawBottom<0)
     ReturnOK();

  if (DrawLeft<0)     DrawLeft=0;
  if (DrawTop<0)     DrawTop=0;

  if (DrawRight>WindowGetWidth(WindowNumber)-1)
     DrawRight=WindowGetWidth(WindowNumber)-1;
  if (DrawBottom>WindowGetHeight(WindowNumber)-1)
     DrawBottom=WindowGetHeight(WindowNumber)-1;

  DrawLeft+=Left;
  DrawTop+=Top;
  DrawRight+=Left;
  DrawBottom+=Top;
  if (!WindowIsUserWindow(WindowNumber))
  {
     Left-=DrawLeft;
     Top-=DrawTop;
     Right-=DrawLeft;
     Bottom-=DrawTop;
  }

/*-------------
  if (WindowNumber==0)
  {
     setfillstyle(1,BLACK);
     bar(DrawLeft,DrawTop,DrawRight,DrawBottom);
  }
-------*/

#ifdef UNUSED
  if (WindowIsMiniumButton(WindowNumber))
     DrawMinButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                   Left,Top,Right,Bottom,WindowGetStatus(WindowNumber));
  if (WindowIsMaxiumButton(WindowNumber))
     DrawMaxButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                   Left,Top,Right,Bottom,WindowGetStatus(WindowNumber));
#endif // UNUSED

  if (WindowIsSystemMenuButton(WindowNumber))
     DrawMenuButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                    Left,Top,Right,Bottom,WindowGetStatus(WindowNumber));
  if (WindowIsVScroll(WindowNumber)||WindowIsHScroll(WindowNumber))
//     DrawScroll(DrawLeft,DrawTop,DrawRight,DrawBottom);
     DrawScroll(DrawLeft,DrawTop,DrawRight,DrawBottom,
                Left,Top,Right,Bottom);

  if (WindowIsVVScroll(WindowNumber)||WindowIsHHScroll(WindowNumber))
     DrawScrollButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                      Left,Top,Right,Bottom,WindowGetStatus(WindowNumber));
  if (WindowIsLeftScroll(WindowNumber))
     DrawLeftScrollButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                          Left,Top,Right,Bottom,WindowGetStatus(WindowNumber));
  if (WindowIsRightScroll(WindowNumber))
     DrawRightScrollButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                           Left,Top,Right,Bottom,WindowGetStatus(WindowNumber));
  if (WindowIsUpScroll(WindowNumber))
     DrawUpScrollButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                        Left,Top,Right,Bottom,WindowGetStatus(WindowNumber));
  if (WindowIsDownScroll(WindowNumber))
     DrawDownScrollButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                          Left,Top,Right,Bottom,WindowGetStatus(WindowNumber));
  if (WindowIsTitleBar(WindowNumber))
     DrawTitleBar(DrawLeft,DrawTop,DrawRight,DrawBottom,
                  Left,Top,Right,Bottom,WindowGetTitle(WindowNumber));
/*---------------
  if (WindowIsUserButton(WindowNumber))
     DrawUserButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                    Left,Top,Right,Bottom,WindowGetStatus(WindowNumber),
                    WindowGetTitle(WindowNumber));
 ----------------*/
  if (WindowIsUserButton(WindowNumber))  {
     int style=WindowGetStatus(WindowNumber);
     style|=(WindowGetStyle(WindowNumber)&0x180);      // ByHance, for icon style
     DrawUserButton(DrawLeft,DrawTop,DrawRight,DrawBottom,
                    Left,Top,Right,Bottom,style,
                    WindowGetTitle(WindowNumber));
  }

  if (WindowIsUserWindow(WindowNumber))
     DrawWindow(DrawLeft,DrawTop,DrawRight,DrawBottom,Left,Top,Right,Bottom,
                WindowCanMoveable(WindowNumber)?LINESPACE:1);

  if (WindowIsMenuWindow(WindowNumber))
  {
     HMENU MidMenu;
     int MidLeft,MidTop;

     Hline3DDown(0,0,getmaxx(),getmaxy(),DrawLeft,DrawRight,DrawBottom-1);

     setcolor(MENUCOLOR);
     setviewport(DrawLeft,DrawTop,DrawRight,DrawBottom,1);

     MidMenu=WindowGetUserData(WindowGetFather(WindowNumber));
     if (WindowHasSystemMenu(WindowGetFather(WindowNumber)))
        MidMenu=MenuGetNext(MidMenu);
     while (MidMenu)
     {
       MenuGetRealLeftTop(MidMenu,&MidLeft,&MidTop);
       MidLeft-=DrawLeft;
       MidTop-=DrawTop+SYSBUTTONWIDTH;
       /* ViewportDisplayString(MenuGetName(MidMenu),MidLeft+16,MidTop+2,
                  MENUCOLOR,MENUBKCOLOR);
        ---------- ByHance ---------------*/
       ViewportDisplayString(MenuGetName(MidMenu),MidLeft+16,MidTop+2,
                     MENUCOLOR,EGA_LIGHTGRAY);
       if (MenuGetShortChar(MidMenu))  // draw under_line under shortChar
          line(MidLeft+15+(MenuGetShortChar(MidMenu)-1)*CHARWIDTH/2,
               MidTop+2+CHARHEIGHT,
               MidLeft+15+MenuGetShortChar(MidMenu)*CHARWIDTH/2,
               MidTop+2+CHARHEIGHT);
       /*MenuDraw(MidMenu,0,0);*/
       MidMenu=MenuGetNext(MidMenu);
     }
  }

  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
  ReturnOK();
}

int WindowGetLastChild(HWND WindowNumber)
{
  HWND MidWindow;

  MidWindow=WindowGetChild(WindowNumber);
  if (MidWindow==0)
     return(0);
  while (!WindowIsNextless(MidWindow))
  {
    MidWindow=WindowGetNext(MidWindow);
  }
  return(MidWindow);
}

/*------- find (X,Y) in which window, return win number ----*/
int WindowGetNumber(int X,int Y,HWND WindowNumber,int SearchDirection)
{
  HWND MidWindow;
  int X1,Y1,X2,Y2;

  #define DOWNTREE 1
  #define UPTREE 2

  MidWindow=WindowNumber;
  if (MidWindow==0)
  {
     if (SearchDirection==DOWNTREE)
        Error(INVAILEDPARAM);
     else
        SearchDirection=DOWNTREE;

     MidWindow=WindowGetLastChild(MidWindow);
     if (WindowGetPrev(MidWindow)==0)
        Error(INVAILEDPARAM);
     MidWindow=WindowGetPrev(MidWindow);
  }

  while (MidWindow)
  {
    WindowGetRealRect(MidWindow,&X1,&Y1,&X2,&Y2);
    if (X>=X1&&X<X2&&Y>=Y1&&Y<Y2)
    {
       HWND MidWindow2;
       int Result;

       if (WindowIsChildless(MidWindow))
          return(MidWindow);
       MidWindow2=WindowGetLastChild(MidWindow);
       Result=WindowGetNumber(X,Y,MidWindow2,DOWNTREE);
       if (Result<=OpOK)
          return(MidWindow);
       else
          return(Result);
    }
    MidWindow=WindowGetPrev(MidWindow);
  }

  if (SearchDirection==DOWNTREE)
     Error(INVAILEDPARAM);
  return(WindowGetNumber(X,Y,WindowGetFather(WindowNumber),SearchDirection));
}

static HWND WindowInsert(Windows *InsertWindow,HWND FatherWindow,HWND PrevWindow)
{
  int Result;
  HWND WindowNumber;

  if ((Result=WindowConstruct())<OpOK)
     Error(Result);
  else
     WindowNumber=Result;

  memcpy(&DataofWindows[WindowNumber],InsertWindow,sizeof(Windows));
  WindowSetFather(WindowNumber,FatherWindow);
  WindowSetPrev(WindowNumber,PrevWindow);
  if (PrevWindow!=0)
  {
     WindowSetNext(WindowNumber,WindowGetNext(PrevWindow));
     WindowSetNext(PrevWindow,WindowNumber);
  }
  else
  {       // it is first child
     WindowSetNext(WindowNumber,WindowGetChild(FatherWindow));
     WindowSetChild(FatherWindow,WindowNumber);
  }

  if (WindowGetNext(WindowNumber)!=0)
     WindowSetPrev(WindowGetNext(WindowNumber),WindowNumber);
  else
     WindowSetActive(WindowGetFather(WindowNumber),WindowNumber);

  return(WindowNumber);
}

int WindowAppend(Windows *AppendWindow,HWND FatherWindow)
{
  return(WindowInsert(AppendWindow,FatherWindow,WindowGetLastChild(FatherWindow)));
}

int WindowDelete(HWND WindowNumber)
{
  int Result;
  HWND MidWindow;

  if ((Result=WindowDestruct(WindowNumber))!=OpOK)
     Error(Result);
  WindowSetNoUse(WindowNumber);

  if (!WindowIsChildless(WindowNumber))
  {
     MidWindow=WindowGetChild(WindowNumber);
     while (MidWindow)
     {
       WindowDelete(MidWindow);
       MidWindow=WindowGetNext(MidWindow);
     }
  }

  if (WindowHasSystemMenu(WindowNumber)||WindowHasMenu(WindowNumber))
  {
     HMENU MidMenu;

     MidMenu=WindowGetUserData(WindowNumber);
     while (MidMenu)
     {
       HMENU SaveMenu;

       SaveMenu=MidMenu;
       MidMenu=MenuGetNext(MidMenu);
       MenuDelete(SaveMenu);
     }
  }

  if (WindowGetPrev(WindowNumber)==0)  // it is first child
     WindowSetChild(WindowGetFather(WindowNumber),WindowGetNext(WindowNumber));
  else    // only need change next,prev link
     WindowSetNext(WindowGetPrev(WindowNumber),WindowGetNext(WindowNumber));

  if (WindowGetNext(WindowNumber)!=0)
     WindowSetPrev(WindowGetNext(WindowNumber),WindowGetPrev(WindowNumber));

  ReturnOK();
}

static void SetWindowBase(Windows *SetWindow,int Left,int Top,
                   int Right,int Bottom)
{
  SetWindow->Left=Left;
  SetWindow->Right=Right;
  SetWindow->Top=Top;
  SetWindow->Bottom=Bottom;
}

int CreatWindow(int Left,int Top,int Right,int Bottom,
                HWND FatherWindow,Function *WindowProcedure,
                unsigned long CreatStyle,char *UserTitle,
                LoadMenus UserMenu[])
{
  int LineSpace,Result;
  HWND MidWin,PrevWin;
  Windows TobeCreatWindow;

  if ((Result=WindowConstruct())<OpOK)
     Error(Result);
  else
     MidWin=Result;

  PrevWin=0;
  TobeCreatWindow.NextChildWindow=0;
  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Right;
  TobeCreatWindow.Bottom=Bottom;
  TobeCreatWindow.FatherWindow=FatherWindow;
  TobeCreatWindow.NextBrotherWindow=TobeCreatWindow.PrevBrotherWindow=0;
  TobeCreatWindow.ActiveChildWindow=0;
  TobeCreatWindow.UserData=0;
  TobeCreatWindow.WindowStyle=CreatStyle;
  TobeCreatWindow.Procedure=WindowProcedure;
  strcpy(TobeCreatWindow.Title,UserTitle);

  memcpy(&DataofWindows[MidWin],&TobeCreatWindow,sizeof(Windows));

  if (TobeCreatWindow.Procedure==NULL)
     WindowSetProcedure(MidWin,(Function *)WindowDefaultProcedure);
  /*--------- ???? -------------*/
  TobeCreatWindow.Procedure=(Function *)WindowDefaultProcedure;

  if (WindowCanResizeable(MidWin))
     LineSpace=LINESPACE+1;
  else
     LineSpace=1;

  if (WindowCanMoveable(MidWin))
  {
     int LeftSpace,RightSpace;

     if (WindowHasSystemMenu(MidWin))
        LeftSpace=SYSBUTTONWIDTH;
     else
        LeftSpace=0;
     RightSpace=0;
     if (WindowCanMiniumable(MidWin))
        RightSpace+=SYSBUTTONWIDTH;
     if (WindowCanMaxiumable(MidWin))
        RightSpace+=SYSBUTTONWIDTH;

     SetWindowBase(&TobeCreatWindow,LineSpace+LeftSpace,
                   LineSpace,Right-Left-LineSpace-RightSpace,
                   LineSpace+SYSBUTTONWIDTH);
     TobeCreatWindow.WindowStyle=WindowSetIsTitle();
     PrevWin=WindowInsert(&TobeCreatWindow,MidWin,PrevWin);
  }

  TobeCreatWindow.Title[0]=0;
  if (WindowHasSystemMenu(MidWin))
  {
     PrevWin=CreatButton(LineSpace,LineSpace,SYSBUTTONWIDTH+LineSpace,
                         SYSBUTTONWIDTH+LineSpace,WindowSetIsSystemMenuButton(),
                         WINDOWSYSMENU,"",NULL,MidWin);
     CreatSystemMenu(MidWin);
  }

  if (WindowCanMiniumable(MidWin)&&WindowCanResizeable(MidWin))
     PrevWin=CreatButton(Right-Left-SYSBUTTONWIDTH-LineSpace,LineSpace,
                         Right-Left-LineSpace,SYSBUTTONWIDTH+LineSpace,
                         WindowSetIsMiniumButton(),WINDOWMINIUM,"",
                         NULL,MidWin);

  if (WindowCanMaxiumable(MidWin)&&WindowCanResizeable(MidWin))
  {
     int MinButtonSpace;

     if (WindowCanMiniumable(MidWin))
        MinButtonSpace=SYSBUTTONWIDTH;
     else
        MinButtonSpace=0;

     PrevWin=CreatButton(Right-Left-SYSBUTTONWIDTH-MinButtonSpace-LineSpace,
                         LineSpace,Right-Left-MinButtonSpace-LineSpace,
                         SYSBUTTONWIDTH+LineSpace,WindowSetIsMaxiumButton(),
                         WINDOWSIZE,"",NULL,MidWin);
  }

  if (WindowHasMenu(MidWin))
  {
     CreatMenuWindow(MidWin,UserMenu);
  }


  if (WindowHasHScroll(MidWin))
  {
     int RightSpace;

     if (WindowHasVScroll(MidWin))
        RightSpace=SYSSCROLLWIDTH;
     else
        RightSpace=0;

     //CreatHScroll(LineSpace-1,Bottom-Top-SYSSCROLLWIDTH-LineSpace+2,
     CreatHScroll(0,Bottom-Top-SYSSCROLLWIDTH-LineSpace-1,
                   (Right-Left-RightSpace-LineSpace)/2,NULL,MidWin);
     CreatStatusWin((Right-Left-RightSpace-LineSpace)/2+5,Bottom-Top-SYSSCROLLWIDTH-LineSpace-3,
                    Right-Left-RightSpace-LineSpace,MidWin);
  }
  if (WindowHasVScroll(MidWin))
  {
     int TopButtonHeight,BottomButtonHeight;

     if (WindowCanMoveable(MidWin))
        TopButtonHeight=SYSBUTTONWIDTH;
     else
        TopButtonHeight=0;
     if (WindowHasMenu(MidWin))
     {
        HWND MidWin2;

        MidWin2=WindowGetChild(MidWin);

        while (MidWin2)
        {
          if (WindowIsMenuWindow(MidWin2))
             break;
          else
             MidWin2=WindowGetNext(MidWin2);
        }
        if (MidWin2)
           TopButtonHeight+=WindowGetHeight(MidWin2);
     }

     if (WindowHasHScroll(MidWin))
        BottomButtonHeight=SYSSCROLLWIDTH;
     else
        BottomButtonHeight=0;

     CreatVScroll(Right-Left-SYSSCROLLWIDTH-LineSpace,LineSpace+TopButtonHeight,
                  Bottom-Top-LineSpace-BottomButtonHeight-4,NULL,MidWin);
  }

  WindowSetFather(MidWin,FatherWindow);
  PrevWin=WindowGetLastChild(FatherWindow);

  if (PrevWin==0)
     WindowSetChild(FatherWindow,MidWin);
  else
     WindowSetNext(PrevWin,MidWin);
  WindowSetPrev(MidWin,PrevWin);
  WindowSetNext(MidWin,0);
  WindowSetActive(FatherWindow,MidWin);
  WindowSetStyle(MidWin,WindowGetStyle(MidWin)|WindowSetIsUserWindow());
  ActiveWindow=MidWin;

  return(MidWin);
}

int WindowInitial(int MaxX,int MaxY)
{
  int Result;       // Left,Top,i;
  LoadMenus *Menu;
  char str[40];
//  Windows TopWindow;

  GlobalWindowHandle=HandleAlloc(MAXWINDOWS*sizeof(Windows),0);
  if (GlobalWindowHandle==0)
     return(OUTOFMEMORY);
  DataofWindows=HandleLock(GlobalWindowHandle);
  if (DataofWindows==NULL)
  {
     HandleFree(GlobalWindowHandle);
     GlobalWindowHandle=0;
     return(OUTOFMEMORY);
  }
  memset(DataofWindows,0,MAXWINDOWS*sizeof(Windows));

  if ((Result=MenuInitial())<0)
     return(Result);
  if ((Result=MessageInitial())<0)
     return(Result);

  SingleLineEditorInitial();
  WindowSetLeft(0,0);
  WindowSetTop(0,0);
  WindowSetRight(0,MaxX);
  WindowSetBottom(0,MaxY);
  WindowSetProcedure(0,(Function *)WindowDefaultProcedure);

  // strcpy(str,"ÇáËÉ");
  str[0]='Ç';   str[1]='á';
  str[2]='Ë';   str[3]='É';
  if(fEditor)
  {
      //strcat(str,"±à¼­");
      str[4]='±';   str[5]='à';
      str[6]='¼';   str[7]='­';
      Menu=EditorMenu;
  }
  else
  {
      //strcat(str,"ÅÅ°æ");
      str[4]='Å';   str[5]='Å';
      str[6]='°';   str[7]='æ';
      Menu=UserMenu;
  }

  str[8]=0;
  strcat(str," V2.0");
/*---------------
  str[8]=0x20;
  {
    double tt;
    char buf[20];

    evaluate("123456*89", &tt);         // tt==10987584.0000
    sprintf(buf,"%.0f",tt);
    Result=strlen(buf)-1;
    str[9]='V' +4 -(buf[Result]&0xf);     // 'V'
    tt=(buf[Result]&0xf)+(buf[Result-1]&0xf);       // 12
    sprintf(&str[10],"%.1f",tt/10);
  }
 -----------------*/

  ActiveWindow=CreatWindow(1,1,MaxX,MaxY,0,(Function *)UserProcedure,
                           WindowSetHasSystemMenu()|
                           WindowSetHasVScroll()|
                           WindowSetHasHScroll()|
                           WindowSetHasMenu(),
                           str,Menu);

  UserFunctionInitial(ActiveWindow);
  MessageInsert(0,REDRAWMESSAGE,0l,MAKELONG(getmaxx(),getmaxy()));
//  MessageInsert(0,DRAWWINDOW,0l,MAKELONG(getmaxx(),getmaxy()));
  ReturnOK();
}

int WindowEnd(void)
{
  UserFunctionFinish();
  SingleLineEditorEnd();
  MessageFinish();
  MenuEnd();
  if (GlobalWindowHandle>0)
  {
     HandleUnlock(GlobalWindowHandle);
     HandleFree(GlobalWindowHandle);
     GlobalWindowHandle=0;
  }
  ReturnOK();
}
