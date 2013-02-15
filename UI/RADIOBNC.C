/*-------------------------------------------------------------------
* Name: radiobnc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static void DrawRadioButton(HWND Window,int DrawLeft,int DrawTop,
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

  setfillstyle(1,RADIOBUTTONBKCOLOR);
  bar(Left,Top,Right,Bottom);
//  setcolor(RADIOBUTTONCOLOR);
//  circle(Left+CIRCLERADIUS,(Top+Bottom)/2,CIRCLERADIUS);

  setcolor(EGA_DARKGRAY);
//  arc(Left+CIRCLERADIUS,(Top+Bottom)/2,45,225,CIRCLERADIUS);
  _arc(Left,(Top+Bottom)/2-CIRCLERADIUS,
        Left+2*CIRCLERADIUS,(Top+Bottom)/2+CIRCLERADIUS,
        Left+CIRCLERADIUS+1,(Top+Bottom)/2-1,
        Left+CIRCLERADIUS-1,(Top+Bottom)/2+1 );

  setcolor(EGA_WHITE);
//  arc(Left+CIRCLERADIUS,(Top+Bottom)/2,225,45,CIRCLERADIUS);
  _arc(Left,(Top+Bottom)/2-CIRCLERADIUS,
        Left+2*CIRCLERADIUS,(Top+Bottom)/2+CIRCLERADIUS,
        Left+CIRCLERADIUS-1,(Top+Bottom)/2+1,
        Left+CIRCLERADIUS+1,(Top+Bottom)/2-1 );

  ViewportDisplayString(WindowGetTitle(Window),Left+CIRCLERADIUS*2+6,
      Top+(SYSBUTTONWIDTH-CHARHEIGHT)-1,RADIOBUTTONCOLOR,RADIOBUTTONBKCOLOR);

  if (WindowGetUserData(Window)&RADIOBUTTONSELECT)
  {
      int X=Left+CIRCLERADIUS, Y=(Top+Bottom)/2;
      int R=FOCUSRADIUS;
//     setfillstyle(1,EGA_RED);
     setcolor(EGA_RED);
//     pieslice(X,Y,0,360,R);
     _ellipse(_GFILLINTERIOR,X-R,Y-R,X+R,Y+R);
  }

  if (WindowGetStatus(Window)&BUTTONISDOWN) {
      setcolor(EGA_DARKGRAY);
//      arc(Left+CIRCLERADIUS,(Top+Bottom)/2,45,225,CIRCLERADIUS);
      _arc(Left,(Top+Bottom)/2-CIRCLERADIUS,
            Left+2*CIRCLERADIUS,(Top+Bottom)/2+CIRCLERADIUS,
            Left+CIRCLERADIUS+1,(Top+Bottom)/2-1,
            Left+CIRCLERADIUS-1,(Top+Bottom)/2+1 );

      setcolor(EGA_WHITE);
//      arc(Left+CIRCLERADIUS,(Top+Bottom)/2,225,45,CIRCLERADIUS);
      _arc(Left,(Top+Bottom)/2-CIRCLERADIUS,
            Left+2*CIRCLERADIUS,(Top+Bottom)/2+CIRCLERADIUS,
            Left+CIRCLERADIUS-1,(Top+Bottom)/2+1,
            Left+CIRCLERADIUS+1,(Top+Bottom)/2-1 );
  }
     //  circle(Left+CIRCLERADIUS,(Top+Bottom)/2,CIRCLERADIUS-1);

  if (WindowGetStatus(Window)&BUTTONGETFOCUS)
  {
#ifdef __TURBOC__
     struct linesettingstype SaveLineStyle;
     getlinesettings(&SaveLineStyle);
     setlinestyle(1,0,1);
#else
     unsigned old_style=getlinestyle();
     setlinestyle(0x5555);
#endif

     setcolor(FRAMECOLOR);
     rectangle(Left+CIRCLERADIUS*2+3,Top,Left+CIRCLERADIUS*2+8
               +strlen(WindowGetTitle(Window))*CHARWIDTH/2,Bottom);

     #ifdef __TURBOC__
       setlinestyle(SaveLineStyle.linestyle,SaveLineStyle.upattern,
                    SaveLineStyle.thickness);
     #else
       setlinestyle(old_style);
     #endif
  }

  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
  return;
}

static void DrawFrameButton(HWND Window,int DrawLeft,int DrawTop,
                     int DrawRight,int DrawBottom)
{
  int SaveColor;
  struct viewporttype TmpViewPort;
  int Left,Top,Right,Bottom;
  int chk_x1,chk_y1,chk_x2,chk_y2;

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

  setfillstyle(1,RADIOBUTTONBKCOLOR);
  bar(Left,Top,Right,Bottom);
  setcolor(RADIOBUTTONCOLOR);

  #define CHECKWIDTH 12

  chk_x1 = Left+1;
  chk_x2 = Left+1+CHECKWIDTH;

  chk_y1 = (Top+Bottom)/2-CHECKWIDTH/2;
  chk_y2 = chk_y1+CHECKWIDTH;

//  rectangle(Left+1,(Top+Bottom)/2-CIRCLERADIUS+1,Left+2*(CIRCLERADIUS-1)+1,
//          (Top+Bottom)/2+CIRCLERADIUS-1);


  Area3DDown(DrawLeft,DrawTop,DrawRight,DrawBottom,
             chk_x1,chk_y1,chk_x2,chk_y2,1);

  ViewportDisplayString(WindowGetTitle(Window),Left+CHECKWIDTH+5,
      Top+(SYSBUTTONWIDTH-CHARHEIGHT)-1,RADIOBUTTONCOLOR,RADIOBUTTONBKCOLOR);

  if (WindowGetUserData(Window)&RADIOBUTTONSELECT)
  {
     setcolor(EGA_RED);
     line(chk_x1+2,chk_y1+5,chk_x1+3,chk_y1+9);
     line(chk_x1+3,chk_y1+9,chk_x1+10,chk_y1+2);

     line(chk_x1+3,chk_y1+5,chk_x1+4,chk_y1+9);
     line(chk_x1+4,chk_y1+9,chk_x1+11,chk_y1+2);

//     line(Left+1,(Top+Bottom)/2-CIRCLERADIUS+1,Left+2*(CIRCLERADIUS-1)+1,
//        (Top+Bottom)/2+CIRCLERADIUS-1);
//     line(Left+2*(CIRCLERADIUS-1)+1,(Top+Bottom)/2-CIRCLERADIUS+1,
//        Left+1,(Top+Bottom)/2+CIRCLERADIUS-1);
  }

  if (WindowGetStatus(Window)&BUTTONISDOWN) {
     setcolor(FRAMECOLOR);
     rectangle(chk_x1,chk_y1,chk_x2,chk_y2);
  }


  if (WindowGetStatus(Window)&BUTTONGETFOCUS)
  {
#ifdef __TURBOC__
     struct linesettingstype SaveLineStyle;
     getlinesettings(&SaveLineStyle);
     setlinestyle(1,0,1);
#else
     unsigned old_style=getlinestyle();
     setlinestyle(0x5555);
#endif

     setcolor(FRAMECOLOR);
     rectangle(Left+CHECKWIDTH+3,Top,Left+CHECKWIDTH+6
               +strlen(WindowGetTitle(Window))*CHARWIDTH/2,Bottom);

     #ifdef __TURBOC__
       setlinestyle(SaveLineStyle.linestyle,SaveLineStyle.upattern,
                    SaveLineStyle.thickness);
     #else
       setlinestyle(old_style);
     #endif
  }

  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  setcolor(SaveColor);
  MouseShow();
  return;
}

long RadioDefaultProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  int DropY,DropX;

  switch (Message)
  {
    case DRAWWINDOW:
         if ((WindowGetUserData(Window)&RADIOBUTTON)==RADIOBUTTON)
            DrawRadioButton(Window,MAKEHI(Param1),MAKELO(Param1),
                            MAKEHI(Param2),MAKELO(Param2));
         else
            DrawFrameButton(Window,MAKEHI(Param1),MAKELO(Param1),
                            MAKEHI(Param2),MAKELO(Param2));
         break;
    case SETSTATUS:
         //if (Param1!=MessageInsert(Window,GETSTATUS,0l,0l))
         if (Param1!=MessageGo(Window,GETSTATUS,0l,0l))      //By zjh 12.6 for test
         {
            if (Param1)
               WindowSetUserData(Window,(unsigned short)
                                 (WindowGetUserData(Window))|RADIOBUTTONSELECT);
            else
               WindowSetUserData(Window,(unsigned short)
                                 (WindowGetUserData(Window))&0x7fff);
            MessageInsert(Window,REDRAWMESSAGE,0l,
                          MAKELONG(WindowGetWidth(Window),
                          WindowGetHeight(Window)));
         }
         break;
    case GETSTATUS:
         if (WindowGetUserData(Window)&RADIOBUTTONSELECT)
            return(TRUE);
         else
            return(FALSE);
    case GETFOCUS:
         if (WindowDefaultProcedure(Window,Message,Param1,Param2)!=TRUE)
            return(FALSE);
         WindowSetStatus(Window,2);
        /*------ added ByHance, 95,11.22 -----*/
         MessageInsert(Window,REDRAWMESSAGE,0l,
                       MAKELONG(WindowGetWidth(Window),
                                WindowGetHeight(Window)));
        /*------ end, 95,11.22 -----*/
         break;
    case LOSTFOCUS:
         if( WindowGetStatus(Window) & (BUTTONISDOWN|BUTTONGETFOCUS) )
         {
            WindowSetStatus(Window,0);
            MessageInsert(Window,REDRAWMESSAGE,0l,
                          MAKELONG(WindowGetWidth(Window),
                                   WindowGetHeight(Window)));
         }
         break;
    case MOUSELEFTDOWN:
    /*--------
         DropX=(short)MAKEHI(Param1);   // ByHance, 96,3.7
         DropY=(short)MAKELO(Param1);
         if (DropX<0||DropY<0||DropX>=WindowGetWidth(Window)
          ||DropY>=WindowGetHeight(Window))
             break;
    -----------*/
         WindowSetStatus(Window,3);
         if (!(WindowGetUserData(Window)&RADIOBUTTONSELECT))
         {
            if (((WindowGetUserData(Window)&RADIOBUTTON)==RADIOBUTTON))
            {
               int MidGroup;
               HWND MidWindow;

               MidGroup=RadioGetGroup(Window);
               MidWindow=WindowGetChild(WindowGetFather(Window));
               while (MidWindow)
               {
                 if ((RadioGetGroup(MidWindow)==MidGroup)
                     &&(WindowGetUserData(MidWindow)&RADIOBUTTONSELECT))
                 {
                    WindowSetUserData(MidWindow,(unsigned short)
                                 (WindowGetUserData(MidWindow))&0x7fff);
                    MessageInsert(MidWindow,REDRAWMESSAGE,0l,
                                  MAKELONG(WindowGetWidth(MidWindow),
                                  WindowGetHeight(MidWindow)));
                    MessageInsert(MidWindow,SELECTUNSELECTED,
                                  RadioGetGroup(MidWindow),
                                  RadioGetOrder(MidWindow));
                    break;
                 }
                 else
                    MidWindow=WindowGetNext(MidWindow);
               }
            }
            WindowSetUserData(Window,(unsigned short)
                              (WindowGetUserData(Window))|RADIOBUTTONSELECT);
         }
         else
            if (!((WindowGetUserData(Window)&RADIOBUTTON)==RADIOBUTTON))
               WindowSetUserData(Window,(unsigned short)
                                 (WindowGetUserData(Window))&0x7fff);
         MessageInsert(Window,REDRAWMESSAGE,0l,
                       MAKELONG(WindowGetWidth(Window),
                                WindowGetHeight(Window)));
         MessageInsert(Window,SELECTSELECTED,
                       RadioGetGroup(Window),
                       RadioGetOrder(Window));
         break;
    case MOUSELEFTUP:
     /*--------------
         DropX=(short)MAKEHI(Param1);   // ByHance, 96,3.7
         DropY=(short)MAKELO(Param1);
         if (DropX<0||DropY<0||DropX>=WindowGetWidth(Window)
          ||DropY>=WindowGetHeight(Window))
             break;
     ----------------*/
         if (WindowGetStatus(Window)&BUTTONISDOWN)
         {
            WindowSetStatus(Window,2);
            MessageInsert(Window,REDRAWMESSAGE,0l,
                          MAKELONG(WindowGetWidth(Window),
                                   WindowGetHeight(Window)));
         }
         break;
    case MOUSELEFTDROP:
         DropX=(short)MAKEHI(Param2);
         DropY=(short)MAKELO(Param2);
         if (DropX<0||DropY<0||DropX>=WindowGetWidth(Window)
             ||DropY>=WindowGetHeight(Window))
         {
            if (WindowGetStatus(Window)&BUTTONISDOWN)
            {
               WindowSetStatus(Window,2);
               MessageInsert(Window,REDRAWMESSAGE,0l,
                             MAKELONG(WindowGetWidth(Window),
                                      WindowGetHeight(Window)));
            }
         }
         else
         {
            if (!(WindowGetStatus(Window)&BUTTONISDOWN))
            {
               WindowSetStatus(Window,3);
               MessageInsert(Window,REDRAWMESSAGE,0l,
                             MAKELONG(WindowGetWidth(Window),
                                      WindowGetHeight(Window)));
            }
         }
         break;
    case KEYDOWN:
         switch (MAKELO(Param1))
         {
           case TAB:
                WindowTableOrderNext(Window);
                break;
           case SHIFT_TAB:
                WindowTableOrderPrev(Window);
                break;
           case ENTER:
                MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
                break;
           case BLANK:
                MessageGo(Window,MOUSELEFTDOWN,0l,0l);
                break;
           case ESC:
                if (WindowGetStatus(Window)&BUTTONISDOWN)
                {
                   WindowSetStatus(Window,2);
                   MessageInsert(Window,REDRAWMESSAGE,0l,
                                 MAKELONG(WindowGetWidth(Window),
                                          WindowGetHeight(Window)));
                }
                else
                   MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
                break;
      /*------------ deleted ByHance, 95,11.22 -----
           case LEFT:
           case UP:
                {
                  int Order,Group;
                  HWND MidWindow,SearchWindow;

                  Group=RadioGetGroup(Window);
                  Order=RadioGetOrder(Window);
                  MidWindow=WindowGetChild(WindowGetFather(Window));
                  SearchWindow=0;
                  if (!Order)
                  {
                     while (MidWindow)
                     {
                       if ((RadioGetGroup(MidWindow)==Group)
                           &&(RadioGetOrder(MidWindow)>Order))
                       {
                          Order=WindowGetUserData(MidWindow)&0xff;
                          SearchWindow=MidWindow;
                       }
                       MidWindow=WindowGetNext(MidWindow);
                     }
                  }
                  else
                  {
                     while (MidWindow)
                     {
                       if ((RadioGetGroup(MidWindow)==Group)
                           &&(RadioGetOrder(MidWindow)==Order-1))
                       {
                          SearchWindow=MidWindow;
                          break;
                       }
                       else
                          MidWindow=WindowGetNext(MidWindow);
                     }
                  }
                  if (SearchWindow)
                  {
                     MessageInsert(Window,LOSTFOCUS,0l,0l);
                     MessageInsert(SearchWindow,GETFOCUS,0l,0l);
                  }
                }
                break;
           case RIGHT:
           case DOWN:
                {
                  int Order,Group;
                  HWND MidWindow,SearchWindow;

                  Group=RadioGetGroup(Window);
                  Order=RadioGetOrder(Window);
                  MidWindow=WindowGetChild(WindowGetFather(Window));
                  SearchWindow=0;
                  while (MidWindow)
                  {
                    if (RadioGetGroup(MidWindow)==Group)
                    {
                       if (RadioGetOrder(MidWindow)==Order+1)
                       {
                          SearchWindow=MidWindow;
                          break;
                       }
                       if (RadioGetOrder(MidWindow)==0)
                          SearchWindow=MidWindow;
                    }
                    MidWindow=WindowGetNext(MidWindow);
                  }
                  if (SearchWindow)
                  {
                     MessageInsert(Window,LOSTFOCUS,0l,0l);
                     MessageInsert(SearchWindow,GETFOCUS,0l,0l);
                  }
                }
                break;
         ----------- end deleted, 95,11.22 -----*/
           default:
                return(WindowDefaultProcedure(Window,Message,Param1,Param2));
         }
         break;
    case KEYUP:
         switch (MAKELO(Param1))
         {
           case ENTER:
                MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
                break;
           case BLANK:
                if (WindowGetStatus(Window)&BUTTONISDOWN)
                {
                   WindowSetStatus(Window,2);
                   MessageInsert(Window,REDRAWMESSAGE,0l,
                                 MAKELONG(WindowGetWidth(Window),
                                          WindowGetHeight(Window)));
                }
                break;
           default:
                return(WindowDefaultProcedure(Window,Message,Param1,Param2));
         }
         break;
    case MOUSEMOVE:
         {
            int X=(short)MAKEHI(Param1);
            int Y=(short)MAKELO(Param1);
            if( X<0 || X>WindowGetWidth(Window)
            || Y<0 || Y>WindowGetHeight(Window))
              DialogMouseMove(Window,Message,Param1,Param2); // ByHance, 95,12.6
            else
              MouseSetGraph(FINGERMOUSE);
         }
         break;
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static HWND CreatSelectButton(int Left,int Top,int Right,int Order,int SelectSign,
                      int SelectType,char *Title,int Group,
                      Function *SelectButtonProcedure,
                      HWND FatherWindow)
{
  Windows TobeCreatWindow;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));

  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Right;
  TobeCreatWindow.Bottom=Top+SYSBUTTONWIDTH;
  if (SelectButtonProcedure==NULL)
     TobeCreatWindow.Procedure=(Function *)RadioDefaultProcedure;
  else
     TobeCreatWindow.Procedure=SelectButtonProcedure;
  strcpy(TobeCreatWindow.Title,Title);
  TobeCreatWindow.UserData=(unsigned char)(Order);
  if (SelectSign)
     TobeCreatWindow.UserData|=RADIOBUTTONSELECT;
  if (SelectType==RADIOBUTTON)
     TobeCreatWindow.UserData|=RADIOBUTTON;
  TobeCreatWindow.UserData|=(Group<<8)&0x3f00;
//  TobeCreatWindow.WindowStyle=3;      // ByHance, 95,11.22
  TobeCreatWindow.WindowStyle=3|WindowSetCanTabOrder();

  return(WindowAppend(&TobeCreatWindow,FatherWindow));
}

HWND CreatRadioButton(int Left,int Top,int Right,int Order,int SelectSign,
                     char *Title,int Group,Function *RadioButtonProcedure,
                     HWND FatherWindow)
{
  return(CreatSelectButton(Left,Top,Right,Order,SelectSign,
         RADIOBUTTON,Title,Group,RadioButtonProcedure,FatherWindow));
}

HWND CreatFrameButton(int Left,int Top,int Right,int Order,int SelectSign,
                     char *Title,int Group,Function *FrameButtonProcedure,
                     HWND FatherWindow)
{
  return(CreatSelectButton(Left,Top,Right,Order,SelectSign,
         FRAMEBUTTON,Title,Group,FrameButtonProcedure,FatherWindow));
}
