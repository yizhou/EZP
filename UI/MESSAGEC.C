/*-------------------------------------------------------------------
* Name: messagec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#pragma off (check_stack)
/* Following is Message Code */

int MessageInitial(void)
{
  GlobalMessageHandle=HandleAlloc(sizeof(struct tagMessages)*MAXMESSAGES,0);
  if (GlobalMessageHandle==0)
          return(OUTOFMEMORY);
  DataofMessages=HandleLock(GlobalMessageHandle);
  if (DataofMessages==NULL)
  {
     HandleFree(GlobalMessageHandle);
     GlobalMessageHandle=0;
     return(OUTOFMEMORY);
  }
  memset(DataofMessages,0,sizeof(struct tagMessages)*MAXMESSAGES);
  ReturnOK();
}

int MessageFinish(void)
{
  if (GlobalMessageHandle>0)
  {
     HandleUnlock(GlobalMessageHandle);
     HandleFree(GlobalMessageHandle);
  }
  ReturnOK();
}

static HMSG MessageConstruct(void)
{
  HMSG i;
  for (i=1;i<MAXMESSAGES;i++)
      if (MessageIsEmpty(i)&&i!=MessageGetQueueTail())
         return(i);
  Error(TOOMANYMESSAGE);
}

static int MessageDestruct(HMSG MessageNumber)
{
  if(MessageNumber>=MAXMESSAGES)
      return(INVAILEDPARAM);
  ReturnOK();
}

unsigned long MessageGo(HWND Window,HMSG Message,ULONG Data,ULONG ExtraData)
{
//  if(Window<0)
//       Window=1;                // ByHance, for test ...
  if (Window>MAXWINDOWS||WindowCanUse(Window))
     return(TRUE);              /* No Window or Window has no procedure */

  if (ActiveMenu>0)
     return(MenuDefaultProc(Window,Message,Data,ExtraData));

  return(DataofWindows[Window].Procedure(Window,Message,Data,ExtraData));
}

int MessageGet(HWND *Window,HMSG *Message,ULONG *Data,ULONG *ExtraData)
{
  int i,RedrawSign=-1,SaveIntSign;

  if (MessageQueueIsEmpty()) {
     return(MAXMESSAGES);
  }

  SaveIntSign=GetIntSign();
  SetIntSign();
  for (i=MessageGetQueueHead();i!=MessageGetQueueTail();i=MessageGetNext(i))
    if (!MessageIsEmpty(i))
    {
       if (ActiveMenu<=0&&MessageGetMessage(i)==REDRAWMESSAGE)
       {                               /* Redraw Message is the lastest option */
          if (RedrawSign==-1)
             RedrawSign=i;
       }
       else
          break;
    }

  if (i!=MessageGetQueueTail())
  {
     *Window=MessageGetWindow(i);
     *Message=MessageGetMessage(i);
     *Data=MessageGetData(i);
     *ExtraData=MessageGetExtra(i);
     MessageDelete(i);
     if (!SaveIntSign)
        ClearIntSign();
     return(i);
  }
  else
     if (RedrawSign!=-1)
     {
        *Window=MessageGetWindow(RedrawSign);
        *Message=MessageGetMessage(RedrawSign);
        *Data=MessageGetData(RedrawSign);
        *ExtraData=MessageGetExtra(RedrawSign);
        MessageDelete(RedrawSign);
        if (!SaveIntSign)
           ClearIntSign();
        return(RedrawSign);
     }
     else
     {
        if (!SaveIntSign)
           ClearIntSign();
        return(MAXMESSAGES);
     }
}

#ifdef UNUSED
int MessageRead(HWND *Window,HMSG *Message,ULONG *Data,ULONG *ExtraData)
{
  int i,RedrawSign=-1;
  int SaveIntSign;

  if (MessageQueueIsEmpty())
     return(MAXMESSAGES);

  SaveIntSign=GetIntSign();
  SetIntSign();

  for (i=MessageGetQueueHead();i!=MessageGetQueueTail();i=MessageGetNext(i))
    if (!MessageIsEmpty(i))
    {
       if (MessageGetMessage(i)==REDRAWMESSAGE)
       {                               /* Redraw Message is the lastest option */
          if (RedrawSign==-1)
             RedrawSign=i;
       }
       else
          break;
    }

  if (i!=MessageGetQueueTail())
  {
     *Window=MessageGetWindow(i);
     *Message=MessageGetMessage(i);
     *Data=MessageGetData(i);
     *ExtraData=MessageGetExtra(i);
     if (!SaveIntSign)
        ClearIntSign();
     return(i);
  }
  else
     if (RedrawSign!=-1)
     {
        *Window=MessageGetWindow(RedrawSign);
        *Message=MessageGetMessage(RedrawSign);
        *Data=MessageGetData(RedrawSign);
        *ExtraData=MessageGetExtra(RedrawSign);
        if (!SaveIntSign)
           ClearIntSign();
        return(RedrawSign);
     }
     else
     {
        if (!SaveIntSign)
           ClearIntSign();
        return(MAXMESSAGES);
     }
}
#endif // UNUSED

void WaitMessageEmpty()                 // added ByHance
{
  int Result;
  HWND Window;
  HMSG Message;
  ULONG Param1,Param2;

  while(1)
  {
      // KeyToMessage(); don't access key or mouse
    if (MessageQueueIsEmpty())
         break;
    if ((Result=MessageGet(&Window,&Message,&Param1,&Param2))<OpOK)
       break;
    if (Result<MAXMESSAGES)
       MessageGo(Window,Message,Param1,Param2);
    else
       MessageGo(1,SYSTEMIDLE,Param1,Param2);
  }
} /* WaitMessageEmpty */

static int MessageDelete(HMSG MessageNumber)
{
  int Result,SaveIntSign;
  HMSG MidMsg;

  if(MessageNumber>=MAXMESSAGES)
      return(INVAILEDPARAM);

  if ((Result=MessageDestruct(MessageNumber))<OpOK)
     Error(Result);
  if (MessageQueueIsEmpty()||MessageNumber==MessageGetQueueTail())
     ReturnOK();

  SaveIntSign=GetIntSign();
  SetIntSign();
  if (MessageGetQueueHead()==MessageNumber)
     MessageSetQueueHead(MessageGetNext(MessageNumber));
  else
  {
     for (MidMsg=MessageGetQueueHead();MidMsg!=MessageGetQueueTail()
          &&MessageGetNext(MidMsg)!=MessageNumber;MidMsg=MessageGetNext(MidMsg));
     if (MidMsg==MessageGetQueueTail())
     {
        if (!SaveIntSign)
           ClearIntSign();
        ReturnOK();
     }
     else
        MessageSetNext(MidMsg,MessageGetNext(MessageNumber));
  }
  MessageSetWindow(MessageNumber,0);
  MessageSetMessage(MessageNumber,0);

  if (!SaveIntSign)
     ClearIntSign();
  ReturnOK();
}



//void MessageBeginProc(void) {}
static HMSG MouseMessageTranslate(HMSG Message)         // ByHance
{
    HMSG msg=Message;
//    unsigned int KeyShiftStatus=bioskey(0x2);
    unsigned int KeyShiftStatus=_bios_keybrd(0x2);

    if(KeyShiftStatus&0x8) {              // Alt
         switch(msg) {
             case MOUSELEFTDOWN:
                  msg=MOUSELEFTDOWN_ALT;
                  break;
          /*
             case MOUSELEFTDROP:
                  msg=MOUSELEFTDOWN_ALT;
                  break;
             case MOUSELEFTDOWNON:
                  msg=MOUSELEFTDOWN_ALT;
                  break;
             case MOUSELEFTUP:
                  msg=MOUSELEFTDOWN_ALT;
                  break;
           */
         }  // switch
    } else
    if(KeyShiftStatus&0x4) {             // Ctrl
         switch(msg) {
             case MOUSELEFTDOWN:
                  msg=MOUSELEFTDOWN_CTRL;
                  break;
           /*--
             case MOUSELEFTDROP:
                  msg=MOUSELEFTDOWN_CTRL;
                  break;
             case MOUSELEFTDOWNON:
                  msg=MOUSELEFTDOWN_CTRL;
                  break;
             case MOUSELEFTUP:
                  msg=MOUSELEFTDOWN_CTRL;
                  break;
             ----*/
         }  // switch
    } else
    if(KeyShiftStatus&0x3) {             // Shift
         switch(msg) {
             case MOUSELEFTDOWN:
                  msg=MOUSELEFTDOWN_SHIFT;
                  break;
            /*--
             case MOUSELEFTDROP:
                  msg=MOUSELEFTDOWN_SHIFT;
                  break;
             case MOUSELEFTDOWNON:
                  msg=MOUSELEFTDOWN_SHIFT;
                  break;
             case MOUSELEFTUP:
                  msg=MOUSELEFTDOWN_SHIFT;
                  break;
             ---*/
         }  // switch
    }

    return msg;
} /* MouseMessageTranslate */

static int MessageMerge(HWND Window,HMSG Message,ULONG Data,ULONG ExtraData)
{
  int i;
  int Left,Top,Right,Bottom,
       DrawLeft,DrawTop,DrawRight,DrawBottom;

  if (Message>MAXMERGEMESSAGE)
     Error(INVAILEDPARAM);
  if (MessageQueueIsEmpty())
     Error(INVAILEDPARAM);
  for (i=MessageGetQueueHead();i!=MessageGetQueueTail();i=MessageGetNext(i))
      if ((MessageGetWindow(i)==Window) && (MessageGetMessage(i)==Message))
      {
         switch (Message)
         {
           case DRAWWINDOW:
           case REDRAWMESSAGE:
                {
                   UCHAR RedrawSign=FALSE;

                   Left=(short)MAKEHI(Data);
                   if (Left<0)
                      Left=0;
                   if (Left>=WindowGetWidth(Window))
                      ReturnOK();

                   Top=(short)MAKELO(Data);
                   if (Top<0)
                      Top=0;
                   if (Top>=WindowGetHeight(Window))
                      ReturnOK();

                   Right=(short)MAKEHI(ExtraData);
                   if (Right<=0)
                      ReturnOK();
                   if (Right>WindowGetWidth(Window))
                      Right=WindowGetWidth(Window);

                   Bottom=(short)MAKELO(ExtraData);
                   if (Bottom<=0)
                      ReturnOK();
                   if (Bottom>WindowGetHeight(Window))
                      Bottom=WindowGetHeight(Window);

                   DrawLeft=(short)MAKEHI(MessageGetData(i));
                   DrawTop=(short)MAKELO(MessageGetData(i));
                   DrawRight=(short)MAKEHI(MessageGetExtra(i));
                   DrawBottom=(short)MAKELO(MessageGetExtra(i));

                   if (Left<=DrawLeft)
                   {
                      DrawLeft=Left;
                      RedrawSign=TRUE;
                   }

                   if (Top<=DrawTop)
                   {
                      DrawTop=Top;
                      RedrawSign=TRUE;
                   }

                   if (Right>=DrawRight)
                   {
                      DrawRight=Right;
                      RedrawSign=TRUE;
                   }

                   if (Bottom>=DrawBottom)
                   {
                      DrawBottom=Bottom;
                      RedrawSign=TRUE;
                   }
                   if (RedrawSign==TRUE)
                   {
                      MessageSetData(i,MAKELONG(DrawLeft,DrawTop));
                      MessageSetExtra(i,MAKELONG(DrawRight,DrawBottom));
                   }
                   ReturnOK();
                }
           case KEYSTRING:
           case MOUSEMOVE:
                MessageSetData(i,Data);
                MessageSetExtra(i,ExtraData);
                ReturnOK();
           case MOUSELEFTDROP:
           case MOUSERIGHTDROP:
           case MOUSELRDROP:
            /*------------------
                if (MessageGetData(i)==Data)
                {
                   MessageSetExtra(i,ExtraData);
                   ReturnOK();
                }
                break;
              ----------------*/
                if (MessageGetData(i)==Data)
                   MessageSetExtra(i,ExtraData);
                ReturnOK();
           case MOUSELEFTDOWNON:
           case TIMERTRIGGER:
                if (MessageGetData(i)!=Data)
                   break;        // Error(INVAILEDPARAM);
                MessageSetExtra(i,MessageGetExtra(i)+ExtraData);
           case WINDOWCLOSE:
           case WINDOWQUIT:
                ReturnOK();
          /*----------------
           case HHSCROLLMOVE:
                if (MessageGetExtra(i)!=ExtraData)
                     break;
                else {
                    Left=Data-GlobalPageHStart;
                    Left+=MessageGetData(i)-GlobalPageHStart;
                    if(Left<0) Left=0;
                    MessageSetData(i,Left);
                    ReturnOK();
                }
           case VVSCROLLMOVE:
                if (MessageGetExtra(i)!=ExtraData)
                     break;
                else {
                    Top=Data-GlobalPageVStart;
                    Top+=MessageGetData(i)-GlobalPageVStart;
                    if(Top<0) Top=0;
                    MessageSetData(i,Top);
                    ReturnOK();
                }
          ----------------*/
         } // switch
      } // ------ for i -------
  Error(INVAILEDPARAM);
} /* MouseMerge */

static int MessageInterruptInsert(HWND Window,HMSG Message,ULONG Data,ULONG ExtraData)
{
  int Result;

  Message=MouseMessageTranslate(Message);         // ByHance

  if (MessageMerge(Window,Message,Data,ExtraData)==OpOK)
     ReturnOK();
  if ((Result=MessageConstruct())<OpOK)
     Error(Result);
  MessageSetWindow(MessageGetQueueTail(),Window);
  MessageSetMessage(MessageGetQueueTail(),Message);
  MessageSetData(MessageGetQueueTail(),Data);
  MessageSetExtra(MessageGetQueueTail(),ExtraData);
  MessageSetNext(MessageGetQueueTail(),Result);
  MessageSetQueueTail(Result);
  ReturnOK();
}

int MessageInsert(HWND Window,HMSG Message,ULONG Data,ULONG ExtraData)
{
  int Result,SaveIntSign;

  SaveIntSign=GetIntSign();
  SetIntSign();
  Result=MessageInterruptInsert(Window,Message,Data,ExtraData);
  if (!SaveIntSign)
     ClearIntSign();

  return(Result);
}


#ifdef UNUSED
int MessageCreatbySerial(void)
{
  ReturnOK();
}

int MessageCreatbyParallel(void)
{
  ReturnOK();
}

int MessageCreatbyOther(void)
{
  ReturnOK();
}

int MessageCreatbyJoyStick(void)
{
  ReturnOK();
}
#endif

int MessageCreatbyKeyboard(unsigned Key,unsigned Shift)
{
  /*--- because not set new Int9, so, can't get keyUp message,
       we must insert keyUp message by ourself
    --------------------------------------------------*/
  MessageInsert(ActiveWindow,KEYDOWN,MAKELONG(0,Key),1l);
  return(MessageInsert(ActiveWindow,KEYUP,MAKELONG(0,Key),1l));
}

int MessageCreatbyKeyString(unsigned char *KeyString,unsigned Length)
{
  return(MessageInsert(ActiveWindow,KEYSTRING,FP2LONG(KeyString),Length));
}

int MessageCreatbyTimer(HWND WindowNumber,int TimerNumber)
{
  return(MessageInterruptInsert(WindowNumber,TIMERTRIGGER,TimerNumber,1l));
}

/*-- called by NewInt8, but must first SetMouseDownOnTime(long timeCount) --*/
void MouseDownOnTrigger(short MouseLRState,short MouseX,short MouseY)
{
  #define LDOWNON 1
  #define RDOWNON 2
  #define LRDOWNON 3

#ifdef OLD_VERSION
  int Left,Top,Right,Bottom;
  unsigned long MouseWindowXY;

  WindowGetRealRect(ActiveWindow,&Left,&Top,&Right,&Bottom);
  MouseWindowXY=MAKELONG(MouseX-Left,MouseY-Top);
  MouseWindowXY=MAKELONG(MouseX,MouseY);

  if (MouseLRState==LDOWNON)
  {
     MessageInterruptInsert(ActiveWindow,MOUSELEFTDOWNON,
            MouseWindowXY,1l);
  }
  else
  if (MouseLRState==LRDOWNON)
  {
     MessageInterruptInsert(ActiveWindow,MOUSELRDOWNON,
            MouseWindowXY,1l);
  }
  else
  if (MouseLRState==RDOWNON)
  {
     MessageInterruptInsert(ActiveWindow,MOUSERIGHTDOWNON,
            MouseWindowXY,1l);
  }
#else
  HMSG msg;
  switch(MouseLRState)
  {
     case LDOWNON: msg=MOUSELEFTDOWNON; break;
     case RDOWNON: msg=MOUSELRDOWNON; break;
     //case LRDOWNON:
     default:
        msg=MOUSERIGHTDOWNON; break;
  }
  MessageInterruptInsert(ActiveWindow,msg,0,1L);
#endif
}

#define MUSTEQU                 // ByHance

int MessageCreatbyMouse(unsigned MouseStatus,unsigned MouseButton,
                        unsigned MouseX,unsigned MouseY)
{
  #define MouseMoveSign (MouseStatus&1)
  #define MouseLeftPressSign (MouseStatus&2)
  #define MouseLeftReleaseSign (MouseStatus&4)
  #define MouseRightPressSign (MouseStatus&8)
  #define MouseRightReleaseSign (MouseStatus&16)

  #define MouseLeftIsDown (MouseButton&1)
  #define MouseRightIsDown (MouseButton&2)
  #define MouseLeftKey          0
  #define MouseRightKey         1

  #define DOUBLECLICKTIME 8

  static UCHAR MouseDropSign=0; /*---   bit        meaning(==1) ----
                                         0         is droping now(Left/Right)
                                         1         LeftKey  is down
                                         2         RightKey is down
                                 -------------------------------------*/
  static int MouseDropStartX,MouseDropStartY;
  static ULONG MousePrevClickTime=0;
#ifdef MUSTEQU
  static unsigned PrevX=0,PrevY=0;          // ByHance
#endif
  int Result;
  //UINT WindowNumber,MouseWindowX,MouseWindowY;
  int WindowNumber,MouseWindowX,MouseWindowY;
  ULONG MouseClickTime;
  int Left,Top,Right,Bottom;

  //_bios_timeofday(0, (long *)&MouseClickTime);
  MouseClickTime=*(ULONG *)0x46c;
  //if((MouseButton&3)!=0)
   SetMouseDownOnTime(MouseClickTime,MouseButton);

  if (MouseLeftPressSign||MouseRightPressSign) //ByHance
  {
     ClearMainTimer();                  // in windeftc.c
     MessageGo(1,DELBUBLE,0L,0L);       // delete old hint window
  }

  if (MouseLeftPressSign&&!MouseRightPressSign) // &&!MouseMoveSign) //ByHance
  {
#ifdef MUSTEQU
     if( (labs(MouseClickTime-MousePrevClickTime)>=DOUBLECLICKTIME)
        || (PrevX!=MouseX) || (PrevY!=MouseY) )
#else
     if (labs(MouseClickTime-MousePrevClickTime)>=DOUBLECLICKTIME)
#endif

     {
        if (ActiveMenu>0)      // click at menu
        {
           Result=MenuGetFocus(ActiveMenu,MouseX,MouseY);
           if (Result<=0)       // not at menu area
              MenuClose(MenuGetTopest(ActiveMenu));
           else
           {
              MouseDropSign|=2;     // left key is down now
              return(MessageInterruptInsert(ActiveWindow,MOUSELEFTDOWN,
                     MAKELONG(MouseX,MouseY),0l));
           }
        }
     }

    //---------- Is it at some window ? -----------
     if ((Result=WindowGetNumber(MouseX,MouseY,ActiveWindow,2))<OpOK)
        Error(Result);                 /* Search Tree UP and DOWN */
     else
        WindowNumber=Result;
      /*---------- added ByHance ---*/
     if (WindowNumber!=ActiveWindow)     // &&WindowNumber>0)
     {
        if (MessageGo(WindowNumber,GETFOCUS,0l,0l)!=TRUE)
           ReturnOK();
        MouseDropSign=0;
     }
     /*-------------*/
  }
  else          // LeftKey not Pressed, and mouse not move
     WindowNumber=ActiveWindow;


  MouseWindowX=MouseX;
  MouseWindowY=MouseY;
  if (ActiveMenu<=0)           // no menu active
  {
     WindowGetRealRect(WindowNumber,&Left,&Top,&Right,&Bottom);
     MouseWindowX-=Left;
     MouseWindowY-=Top;
  }

  if (MouseMoveSign)
  {
     if ((MouseLeftIsDown||MouseRightIsDown)&&((MouseDropSign&1)==0))
     {                // now, start to drop
        MouseDropSign|=1;
        MouseDropStartX=MouseWindowX;
        MouseDropStartY=MouseWindowY;
     }

     if (MouseDropSign&1)         // if mouse is now droping
     {
        if (MouseLeftIsDown&&MouseRightIsDown&&((MouseDropSign&6)!=6))
        {                  /* now, LR_key is down */
           MouseDropSign|=6;
           MessageInterruptInsert(ActiveWindow,MOUSELRDOWN,
                         MAKELONG(MouseWindowX,MouseWindowY),0l);
        }
        else
        if (MouseLeftIsDown&&((MouseDropSign&2)!=2))
        {               /* now, L_key is down */
           MouseDropSign|=2;
           MessageInterruptInsert(ActiveWindow,MOUSELEFTDOWN,
                         MAKELONG(MouseWindowX,MouseWindowY),0l);
        }
        else
        if (MouseRightIsDown&&((MouseDropSign&4)!=4))
        {            /* now, R_key is down */
           MouseDropSign|=4;
           MessageInterruptInsert(ActiveWindow,MOUSERIGHTDOWN,
                         MAKELONG(MouseWindowX,MouseWindowY),0l);
        }

        if (MouseDropStartX==MouseWindowX&&MouseDropStartY==MouseWindowY)
           ReturnOK();

        if (MouseLeftIsDown&&MouseRightIsDown)
           return(MessageInterruptInsert(ActiveWindow,MOUSELRDROP,
                         MAKELONG(MouseDropStartX,MouseDropStartY),
                         MAKELONG(MouseWindowX,MouseWindowY)));

        if (MouseLeftIsDown)
           return(MessageInterruptInsert(ActiveWindow,MOUSELEFTDROP,
                      MAKELONG(MouseDropStartX,MouseDropStartY),
                      MAKELONG(MouseWindowX,MouseWindowY)));

        if (MouseRightIsDown)
           return(MessageInterruptInsert(ActiveWindow,MOUSERIGHTDROP,
                      MAKELONG(MouseDropStartX,MouseDropStartY),
                      MAKELONG(MouseWindowX,MouseWindowY)));

        /*---------- now, no key is down ---------*/
    NoKeyIsDown:
        if ((MouseDropSign&6)==6) // if LR_key was down, now must be up
        {
           MessageInterruptInsert(ActiveWindow,MOUSELRUP,MAKELONG(MouseWindowX,MouseWindowY),0l);
        }
        else
        if ((MouseDropSign&2)==2)
        {
           MessageInterruptInsert(ActiveWindow,MOUSELEFTUP,MAKELONG(MouseWindowX,MouseWindowY),0l);
        }
        else
        if ((MouseDropSign&4)==4) {
           MessageInterruptInsert(ActiveWindow,MOUSERIGHTUP,MAKELONG(MouseWindowX,MouseWindowY),0l);
        }

        MouseDropSign=0;
        return(MessageInterruptInsert(ActiveWindow,MOUSEMOVE,MAKELONG(MouseWindowX,MouseWindowY),0l));
     }
     else  /* MouseDropSign&1==0  , mouse is not droping now */
     {                     // ByHance
        goto  NoKeyIsDown;
     }
  }
  else  //  Mouse is not moving
  {
#ifdef MUSTEQU
     if( (labs(MouseClickTime-MousePrevClickTime)>=DOUBLECLICKTIME)
        || (PrevX!=MouseX) || (PrevY!=MouseY) )
#else
     if (labs(MouseClickTime-MousePrevClickTime)>=DOUBLECLICKTIME)
#endif
     {
        if (!MouseLeftPressSign&&MouseLeftIsDown)
        {
           if (!MouseRightPressSign&&MouseRightIsDown)
              return(MessageInterruptInsert(ActiveWindow,MOUSELRDOWNON,
                     MAKELONG(MouseWindowX,MouseWindowY),0l));
           else
              return(MessageInterruptInsert(ActiveWindow,MOUSELEFTDOWNON,
                     MAKELONG(MouseWindowX,MouseWindowY),0l));
        }
        if (!MouseRightPressSign&&MouseRightIsDown)
           return(MessageInterruptInsert(ActiveWindow,MOUSERIGHTDOWNON,
                  MAKELONG(MouseWindowX,MouseWindowY),0l));
     }
  }

  if (MouseLeftPressSign)
  {
      /*---------- added ByHance ---
     if (WindowNumber!=ActiveWindow&&WindowNumber!=0&&!MouseMoveSign)
     {
        //if (ActiveWindow>0&&MessageGo(ActiveWindow,LOSTFOCUS,0l,0l)==FALSE)
        //   ReturnOK();

        if (MessageGo(ActiveWindow,GETFOCUS,0l,0l)!=TRUE)
           ReturnOK();
        {                        // added ByHance  95.9,15
        MouseDropSign=0;
        }
     }
     ---------------------*/

     if ((MouseDropSign&1)==0)          // not drag last time
     {
#ifdef MUSTEQU
        if( (labs(MouseClickTime-MousePrevClickTime)<DOUBLECLICKTIME)
           && (PrevX==MouseX) && (PrevY==MouseY)  )      // ByHance
#else
        if (labs(MouseClickTime-MousePrevClickTime)<DOUBLECLICKTIME)
#endif
        {                          // double click
           if (MouseRightPressSign)
              return(MessageInterruptInsert(ActiveWindow,MOUSELRDOUBLE,
                     MAKELONG(MouseWindowX,MouseWindowY),0l));
           else
              return(MessageInterruptInsert(ActiveWindow,MOUSELEFTDOUBLE,
                     MAKELONG(MouseWindowX,MouseWindowY),0l));
        }
        else
        {
           MousePrevClickTime=MouseClickTime;
#ifdef MUSTEQU
           PrevX=MouseX;     PrevY=MouseY;
#endif
           if (MouseRightPressSign)
           {
              MouseDropSign|=6;   // L_key is already pressed, so LR_key is down
              return(MessageInterruptInsert(ActiveWindow,MOUSELRDOWN,
                            MAKELONG(MouseWindowX,MouseWindowY),0l));
           }
           else
           {
              MouseDropSign|=2;
              return(MessageInterruptInsert(ActiveWindow,MOUSELEFTDOWN,
                            MAKELONG(MouseWindowX,MouseWindowY),0l));
           }
        }
     }
  }
  else          // L_key is not pressed

  if (MouseRightPressSign)
  {
     if ((MouseDropSign&1)==0)
     {
#ifdef MUSTEQU
        if( (labs(MouseClickTime-MousePrevClickTime)<DOUBLECLICKTIME)
           && (PrevX==MouseX) && (PrevY==MouseY)  )      // ByHance
#else
        if (labs(MouseClickTime-MousePrevClickTime)<DOUBLECLICKTIME)
#endif
           return(MessageInterruptInsert(ActiveWindow,MOUSERIGHTDOUBLE,
                                MAKELONG(MouseWindowX,MouseWindowY),0l));
        else
        {
           MousePrevClickTime=MouseClickTime;
#ifdef MUSTEQU
           PrevX=MouseX;     PrevY=MouseY;
#endif
           MouseDropSign|=4;   // L_key is not pressed,so only R_key down
           return(MessageInterruptInsert(ActiveWindow,MOUSERIGHTDOWN,
                         MAKELONG(MouseWindowX,MouseWindowY),0l));
        }
     }
  }

  else   // LR_key is not pressed
  {
     MouseDropSign=0;
     if (MouseLeftReleaseSign&&MouseRightReleaseSign)
        return(MessageInterruptInsert(ActiveWindow,MOUSELRUP,MAKELONG(MouseWindowX,MouseWindowY),0l));

     if (MouseLeftReleaseSign)
        return(MessageInterruptInsert(ActiveWindow,MOUSELEFTUP,MAKELONG(MouseWindowX,MouseWindowY),0l));

     if (MouseRightReleaseSign)
        return(MessageInterruptInsert(ActiveWindow,MOUSERIGHTUP,MAKELONG(MouseWindowX,MouseWindowY),0l));
  }

  ReturnOK();
} /* MessageCreatbyMouse */

//void MessageEndProc(void) {}
