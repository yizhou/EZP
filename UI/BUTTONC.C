#include "ezpHead.h"

static int backdoor_x, /*backdoor_y,*/ backdoor_status=0;
int CreatButton(int Left,int Top,int Right,int Bottom,long WindowStyle,
                int ButtonReturn,char *ButtonTitle,Function *ButtonProcedure,
                HWND FatherWindow)
{
  Windows ButtonWindow;

  memset(&ButtonWindow,0,sizeof(ButtonWindow));
  ButtonWindow.Left=Left;
  ButtonWindow.Top=Top;
  ButtonWindow.Right=Right;
  ButtonWindow.Bottom=Bottom;
  strcpy(ButtonWindow.Title,ButtonTitle);
  ButtonWindow.UserData=ButtonReturn;
  ButtonWindow.WindowStyle=WindowStyle;
  if (WindowStyle&WindowSetIsUserButton())
     ButtonWindow.WindowStyle|=WindowSetCanTabOrder();
  if (ButtonProcedure==NULL)
     ButtonWindow.Procedure=(Function *)ButtonDefaultProc;
  else
     ButtonWindow.Procedure=ButtonProcedure;
  return(WindowAppend(&ButtonWindow,FatherWindow));
}

long ButtonDefaultProc(HWND Window,HMSG Message,long Param1,long Param2)
{
  int X,Y;
  HWND MidWin;

  switch (Message)
  {
    case KEYDOWN:
         if (WindowGetStatus(Window)&BUTTONISDOWN)      // ByHance, 95,11.25
             break;
         if (WindowGetUserData(Window)==WINDOWLEFTSCROLL
             ||WindowGetUserData(Window)==WINDOWRIGHTSCROLL
             ||WindowGetUserData(Window)==WINDOWUPSCROLL
             ||WindowGetUserData(Window)==WINDOWDOWNSCROLL)
         {
            MidWin=WindowGetFather(Window);
            if(MidWin && WindowGetFather(MidWin)==1)  // ByHance, 96,3.24
                 MessageGo(WindowGetFather(MidWin),GETFOCUS,0,0);

            return(MessageGo(WindowGetFather(WindowGetFather(Window)),
                   Message,Param1,Param2));
         }
         if (WindowIsVVScroll(Window)||WindowIsHHScroll(Window))
         {
            MessageGo(WindowGetFather(Window),GETFOCUS,0,0);
            return(MessageGo(WindowGetFather(Window),Message,Param1,Param2));
         }

         if (!WindowCanTabOrder(Window))
            break;

         switch (MAKELO(Param1))
         {
           case TAB:
                WindowTableOrderNext(Window);
                break;
           case SHIFT_TAB:
                WindowTableOrderPrev(Window);
                break;
           case BLANK:
           case ENTER:
                WindowSetStatus(Window,BUTTONISDOWN|BUTTONGETFOCUS);
                MessageInsert(Window,REDRAWMESSAGE,0l,
                    MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
                break;
           case ESC:
                if (WindowGetStatus(Window)&BUTTONISDOWN)
                {
                   WindowSetStatus(Window,BUTTONGETFOCUS);
                   MessageInsert(Window,REDRAWMESSAGE,0l,
                     MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
                }
                else
                   MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
                break;
           default:
                return(WindowDefaultProcedure(Window,Message,Param1,Param2));
         }
         break;
    case KEYUP:
         if (!WindowIsUserButton(Window))
            break;
         switch (MAKELO(Param1))
         {
           case BLANK:
           case ENTER:
                if (WindowGetStatus(Window)&BUTTONISDOWN)
                {
                   WindowSetStatus(Window,BUTTONGETFOCUS);
                   MessageInsert(Window,REDRAWMESSAGE,0l,
                        MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
                   MessageInsert(WindowGetFather(Window),
                        WindowGetUserData(Window),Window,0l);
                }
                break;
           default:
                return(WindowDefaultProcedure(Window,Message,Param1,Param2));
         }
         break;

    case WINDOWCLOSE:
         MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
         WindowDefaultProcedure(Window,Message,Param1,Param2);
         break;
    case GETFOCUS:
         if (WindowDefaultProcedure(Window,Message,Param1,Param2)!=TRUE)  {
                    return(FALSE);
         }
         if (WindowIsUserButton(Window)) {
            WindowSetStatus(Window,BUTTONGETFOCUS);
            MessageInsert(Window,REDRAWMESSAGE,0l,
                  MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)));
         }
            // it is IconButton, don't do it now (MouseLeft down will do it)
         //if (WindowGetStyle(Window) & WindowSetIsIcon()) break;
  /*------------- ByHance 95.9,16 --------
         if (WindowDefaultProcedure(Window,Message,Param1,Param2)!=TRUE)  {
                    return(FALSE);
         }
   ---------------------------*/
         break;

    case LOSTFOCUS:
         if (!WindowIsUserButton(Window))
            break;
         if( WindowGetStatus(Window)&(BUTTONISDOWN|BUTTONGETFOCUS) )
         {
            WindowSetStatus(Window,0);
            MessageInsert(Window,REDRAWMESSAGE,0l,
                   MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
         }
         break;
    case MOUSELEFTDOWN_ALT:             // added ByHance, for backdoor
              /* 1. press mouse left key on OK button, but release outside
                 2. press left key(with ALT pressed) 3 times,
                    but mouse(x,y) increases 16 dots(char width) everytime
                 3. now, we can see Hance and Jerry Name in window
               */
         X=(short)MAKEHI(Param1);
         // Y=MAKELO(Param1);
         switch (backdoor_status) {
            case 0:          // first time
            first_time:
                backdoor_x=X; //     backdoor_y=Y;
                backdoor_status=1;
                break;
            case 1:
            case 2:
                if( X-backdoor_x<16+16-4 && X-backdoor_x>10 )
                // it must be OK, Cancel ... button (width>30)
                 // && Y-backdoor_y<16+12 && Y-backdoor_y>4 )
                {
                   backdoor_status++;
                   backdoor_x=X;  //    backdoor_y=Y;
                   if(backdoor_status>2)      // 3 times
                   {
                      HWND win=WindowGetFather(Window);
                      char title[MAXTITLELENGTH];
                      //X=WindowGetLeft(Window);
                      //Y=WindowGetTop(Window);

                      win=WindowGetChild(win);
                      if(!win) {
                         Alarm();
                         break;
                      }
                      strcpy(title,WindowGetTitle(win));
                      { unsigned char midstr[24];
                        midstr[0]='周'>>8; midstr[1]='周'&0xff;
                        midstr[2]='奕'>>8; midstr[3]='奕'&0xff;
                        midstr[4]=',';
                        midstr[5]='韩'>>8; midstr[6]='韩'&0xff;
                        midstr[7]='兆'>>8; midstr[8]='兆'&0xff;
                        midstr[9]='强'>>8; midstr[10]='强'&0xff;
                        midstr[11]=0;
                        WindowSetTitle(win,midstr);
                      }
                      MessageGo(win,DRAWWINDOW,0l,
                        MAKELONG(WindowGetWidth(win),WindowGetHeight(win)));
                      WindowSetTitle(win,title);
                   }
                } else {
                   goto first_time;
                }
            default:
                break;
         }  // switch backdoor_status
         break;
    case MOUSELEFTDOWN:
         {                  // added ByHance
            X=(short)MAKEHI(Param1);
            Y=(short)MAKELO(Param1);
            if(X<0 || X>WindowGetWidth(Window))
                break;
            if(Y<0 || Y>WindowGetHeight(Window))
                break;
            backdoor_status=0;      // global var, in ..\kernl\knlenv.h
         }

         if (WindowIsUserButton(Window))
         {
            WindowSetStatus(Window,BUTTONISDOWN|BUTTONGETFOCUS);
         }
         else
         {
            WindowSetStatus(Window,BUTTONISDOWN);    // button now is down
         }
         MessageInsert(Window,REDRAWMESSAGE,0l,
                   MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
         break;
    case MOUSEMOVE:
         if( WindowGetProcedure(Window)==(Function *)ToolBarProcedure
         || WindowGetProcedure(Window)==(Function *)ToolBarProcedure1)
             ToolBarMouseMove(Window,Message,Param1,Param2);
         else
         {
            X=(short)MAKEHI(Param1);
            Y=(short)MAKELO(Param1);
            if( X<0 || X>WindowGetWidth(Window)
            || Y<0 || Y>WindowGetHeight(Window))
              DialogMouseMove(Window,Message,Param1,Param2); // ByHance, 95,12.6
            else
              MouseSetGraph(FINGERMOUSE);
         }
         break;
    case MOUSELEFTDOWNON:
         if (WindowGetUserData(Window)!=WINDOWLEFTSCROLL
             &&WindowGetUserData(Window)!=WINDOWRIGHTSCROLL
             &&WindowGetUserData(Window)!=WINDOWUPSCROLL
             &&WindowGetUserData(Window)!=WINDOWDOWNSCROLL)
            break;
         GlobalNotDisplay=1;        // ByHance, 95,12.8
    case MOUSELEFTUP:          // 3.23
         if (WindowGetStatus(Window)&BUTTONISDOWN)
         {
            long SendClick;

            if (Message!=MOUSELEFTDOWNON)
            {
               if (WindowIsUserButton(Window))
               {
                  WindowSetStatus(Window,BUTTONGETFOCUS);
               }
               else
               {
                  WindowSetStatus(Window,0);
               }

               /*------- added ByHance ---------*/
               if (WindowGetStyle(Window) & WindowSetIsIcon())
               {   // it is IconButton, up it, then send Message
                  int idx;
                  MessageGo(Window,DRAWWINDOW,0l,
                     MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)));

                  idx=WindowGetUserData(Window);
                  MessageGo(WindowGetFather(WindowGetFather(Window)),
                         MENUCOMMAND,IconMenuIdxArr[idx],0L);

                  if( ActiveMenu<=0 &&
                  ActiveWindow!=WindowGetFather(WindowGetFather(Window)) )
                      MessageGo(WindowGetFather(WindowGetFather(Window)),
                          GETFOCUS,0,0);

                  return(TRUE);
               }

               MessageInsert(Window,REDRAWMESSAGE,0l,
                   MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)));

               if( (WindowIsVVScroll(Window)||WindowIsHHScroll(Window)
                ||WindowGetUserData(Window)==WINDOWLEFTSCROLL
                ||WindowGetUserData(Window)==WINDOWRIGHTSCROLL
                ||WindowGetUserData(Window)==WINDOWUPSCROLL
                ||WindowGetUserData(Window)==WINDOWDOWNSCROLL)
                &&GlobalNotDisplay) {
                   MidWin=WindowGetFather(Window);
                   if(MidWin && WindowGetFather(MidWin)==1)  // ByHance, 96,3.24
                   {
                       GlobalNotDisplay=0;        // ByHance, 95,12.8
                       RedrawUserField();
                       CloseTextCursor();
                       MessageInsert(1,GETFOCUS,0,0);
                   }
                   break;
               }

               SendClick=1;             // when click, only 1 time
            }
            else            // (Message==MOUSELEFTDOWNON)
               SendClick=Param2*10;        // mouse down delay times

            switch (WindowGetUserData(Window))
            {
               case WINDOWLEFTSCROLL:
                    SendClick=-SendClick;
               case WINDOWRIGHTSCROLL:
                    MidWin=WindowGetChild(WindowGetFather(Window));
                    while (MidWin)      // search H_Scroll
                    {
                      if (WindowIsHHScroll(MidWin))
                         break;
                      else
                         MidWin=WindowGetNext(MidWin);
                    }

                    if (MidWin)         // found it
                       MessageInsert(MidWin,HHSCROLLMOVE,SendClick,0l);
                    break;
               case WINDOWUPSCROLL:
                    SendClick=-SendClick;
               case WINDOWDOWNSCROLL:
                    MidWin=WindowGetChild(WindowGetFather(Window));
                    while (MidWin)      // search V_Scroll
                    {
                      if (WindowIsVVScroll(MidWin))
                         break;
                      else
                         MidWin=WindowGetNext(MidWin);
                    }

                    if (MidWin)         // found it
                       MessageInsert(MidWin,VVSCROLLMOVE,SendClick,0l);
                    break;
               default:
                    MessageInsert(WindowGetFather(Window),
                                  WindowGetUserData(Window),Window,0l);
            }   // switch WinType
         }      // if button is pressed and is down
         else
         {
            MidWin=WindowGetFather(Window);
            if(MidWin && WindowGetFather(MidWin)==1)  // ByHance, 96,3.24
                 MessageInsert(WindowGetFather(MidWin),GETFOCUS,0,0);
         }
         break;
    case MOUSELEFTDROP:
         {
           int DropY,DropX;

           DropX=(short)MAKEHI(Param2);
           DropY=(short)MAKELO(Param2);
           if (WindowIsVVScroll(Window))
           {
              GlobalNotDisplay=1;               // ByHance, 95,12.8
              MessageInsert(Window,VVSCROLLMOVE,DropY-(short)MAKELO(Param1),0l);
              break;
           }
           if (WindowIsHHScroll(Window))
           {
              GlobalNotDisplay=1;               // ByHance, 95,12.8
              MessageInsert(Window,HHSCROLLMOVE,DropX-(short)MAKEHI(Param1),0l);
              break;
           }
           if (DropX<0||DropY<0||DropX>=WindowGetWidth(Window)
               ||DropY>=WindowGetHeight(Window))
           {                            // leave the window
              if (WindowGetStatus(Window)&BUTTONISDOWN)
              {
                 if (WindowIsUserButton(Window))
                 {                      // when drop, don't lost focus
                    WindowSetStatus(Window,BUTTONGETFOCUS);
                 }
                 else
                 {
                    WindowSetStatus(Window,0);
                 }
                 MessageInsert(Window,REDRAWMESSAGE,0l,
                   MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
              }
           }
           else
           {            // (x,y) in the window, so, it is droping
              if (!(WindowGetStatus(Window)&BUTTONISDOWN))
              {
                 if (WindowIsUserButton(Window))
                 {
                    WindowSetStatus(Window,BUTTONISDOWN|BUTTONGETFOCUS);
                 }
                 else
                 {
                    WindowSetStatus(Window,BUTTONISDOWN);
                 }
                 MessageInsert(Window,REDRAWMESSAGE,0l,
                   MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
              }
           }
         }
         break;
    case MOUSELEFTDOUBLE:
         if (WindowIsSystemMenuButton(Window)) {
            if(ActiveMenu>0)
                MenuAllClose(ActiveMenu);       // ByHance
            MessageInsert(WindowGetFather(Window),WINDOWCLOSE,
                          Window,0l);
         }
         break;
    case HHSCROLLMOVE:
         if (!WindowIsHHScroll(Window))
            break;
         if (WindowGetLeft(Window)+(int)Param1<SYSSCROLLWIDTH)
            Param1=SYSSCROLLWIDTH-WindowGetLeft(Window);
         if (WindowGetRight(Window)+(int)Param1>
             WindowGetWidth(WindowGetFather(Window))-SYSSCROLLWIDTH)
            Param1=WindowGetWidth(WindowGetFather(Window))-SYSSCROLLWIDTH
                   -WindowGetRight(Window);
         /*if (WindowGetLeft(Window)+(int)Param1>=SYSSCROLLWIDTH
             &&WindowGetRight(Window)+(int)Param1<
               WindowGetWidth(WindowGetFather(Window))-SYSSCROLLWIDTH)*/
         {
            MessageInsert(Window,WINDOWMOVE,Param1,0l);
            MessageInsert(WindowGetFather(WindowGetFather(Window)),Message,
                          WindowGetLeft(Window)+Param1-SYSSCROLLWIDTH,
                          WindowGetWidth(WindowGetFather(Window))
                          -3*SYSSCROLLWIDTH);
         }
         break;
    case VVSCROLLMOVE:
         if (!WindowIsVVScroll(Window))
            break;

         if (WindowGetTop(Window)+(int)Param1<SYSSCROLLWIDTH)
            Param1=SYSSCROLLWIDTH-WindowGetTop(Window);
         if (WindowGetBottom(Window)+(int)Param1>
             WindowGetHeight(WindowGetFather(Window))-SYSSCROLLWIDTH)
            Param1=WindowGetHeight(WindowGetFather(Window))-SYSSCROLLWIDTH
                   -WindowGetBottom(Window);
         /* if (WindowGetTop(Window)+(int)Param1>=SYSSCROLLWIDTH
             &&WindowGetBottom(Window)+(int)Param1<
               WindowGetHeight(WindowGetFather(Window))-SYSSCROLLWIDTH) */
         {
            MessageInsert(Window,WINDOWMOVE,0l,Param1);
            MessageInsert(WindowGetFather(WindowGetFather(Window)),Message,
                          WindowGetTop(Window)+Param1-SYSSCROLLWIDTH,
                          WindowGetHeight(WindowGetFather(Window))
                          -3*SYSSCROLLWIDTH);
         }
         break;
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

/////////added by jerry////////////////////////////////////////////////////////
void ToolBarMouseMove(HWND Window,HMSG Message,ULONG Param1,ULONG Param2)
{
   HWND win1;
   short MouseX,MouseY;
   int Left,Top,Right,Bottom;

   if(ActiveWindow!=1)                  // ByHance
   {
         MouseSetGraph(ARRAWMOUSE);
         return;
   }

   WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
   MouseX = (short)MAKEHI(Param1)+Left;
   MouseY = (short)MAKELO(Param1)+Top;

//   MouseX = (short)MAKEHI(Param1);
//   MouseY = (short)MAKELO(Param1);
   if ((win1=WindowGetNumber(MouseX,MouseY,ActiveWindow,2))<OpOK)
     return ;
   if (win1 != Window) {
         WindowGetRealRect(win1,&Left,&Top,&Right,&Bottom);
         MouseX-=Left;
         MouseY-=Top;
         Param1=MAKELONG(MouseX,MouseY);
         MessageGo(win1,MOUSEMOVE,Param1,0L);
         return ;
   }
   MouseSetGraph(FINGERMOUSE);
}

void DialogMouseMove(HWND Window,HMSG Message,ULONG Param1,ULONG Param2)
{
   HWND win1;
   short MouseX,MouseY;
   int Left,Top,Right,Bottom;

   WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
   MouseX = (short)MAKEHI(Param1)+Left;
   MouseY = (short)MAKELO(Param1)+Top;
   if ((win1=WindowGetNumber(MouseX,MouseY,Window,2))<OpOK) {
      MouseSetGraph(ARRAWMOUSE);
      return;
   }
   if (win1 != Window) {
         WindowGetRealRect(win1,&Left,&Top,&Right,&Bottom);
         MouseX-=Left;
         MouseY-=Top;
         Param1=MAKELONG(MouseX,MouseY);
         MessageGo(win1,MOUSEMOVE,Param1,0L);
         return ;
   }

//   if (WindowIsUserButton(Window))
      MouseSetGraph(ARRAWMOUSE);
}

