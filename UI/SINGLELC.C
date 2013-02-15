/*-------------------------------------------------------------------
* Name: singlelc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

void Alarm(void)
{
  _disable();
  outp( 0x61, 2^(0xfe & inp(0x61)) );
  _enable();
}

int IsInChineseChar(unsigned char *String,int Pos)
{
  int Length,i;
  unsigned char *TestPointer;

  Length=strlen(String);
  if (!Pos||Pos>Length||String[Pos-1]<=0xa0)
     return(FALSE);

  for (i=Pos,Length=0,TestPointer=String+Pos;i>0;i--,TestPointer--)
      if (*(TestPointer-1)>0xa0)
         Length++;
      else
         break;
  return(Length&1);
}

static BOOL bCursorOn,bBlockOn;
short CursorPosX;
static short CursorPosY,CursorHeight;
static short CursorBlockStart=-1, CursorBlockEnd=-1;
static void CursorDispBlk(int From);

static void CursorDisplay(void)
{
  int SaveColor;

  if(CursorBlockStart<CursorBlockEnd)            // ByHance, 96,3.4
  {
     if(!bBlockOn) { CursorDispBlk(0);  bBlockOn=TRUE; }
     return;
  }

  MouseHidden();
  SaveColor=getcolor();
  setcolor(EGA_WHITE);
  setwritemode(XOR_PUT);

/*--------
  if (bCursorOn)
     bCursorOn=FALSE;
  else
     bCursorOn=TRUE;
-----------*/
  bCursorOn=!bCursorOn;

  line(CursorPosX,CursorPosY-CursorHeight,CursorPosX,CursorPosY);
  setwritemode(COPY_PUT);
  setcolor(SaveColor);
  MouseShow();
}

static void CursorDispBlk(int From)
{
  int SaveColor;
  short x1,x2;
/*---------
  int Left,Top,Right,Bottom;
  struct viewporttype TmpViewPort;

  getviewsettings(&TmpViewPort);
  WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
  setviewport(Left+1,Top+1,Right-1,Bottom-1,1);
-----------*/
  if(CursorBlockStart>=CursorBlockEnd)
     return;

  SaveColor=getcolor();
  MouseHidden();

  setcolor(EGA_WHITE);
  setwritemode(XOR_PUT);

  x1=CursorBlockStart;
  if(x1<From) x1=0; else x1-=From;
  x1<<=3;     /*- x1*=8; -*/

  x2=CursorBlockEnd;
  if(x2<From) return;
  x2=(x2-From)*8 - 1;

  bar(x1,CursorPosY-CursorHeight,x2,CursorPosY);

  setwritemode(COPY_PUT);
  setcolor(SaveColor);
  MouseShow();
/*-----------
  setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
-----------*/
}

static void CancelCursorBlk(int From)
{
  if(CursorBlockStart>=CursorBlockEnd)
     return;

  CursorDispBlk(From);
  CursorBlockStart=CursorBlockEnd=-1;             // ByHance, 96,3.4
 // bBlockOn=FALSE;
}

static int CreatCursor(HWND Window,int Height)
{
  CursorHeight=Height;
  bCursorOn=FALSE;
  if(CursorBlockStart==CursorBlockEnd && CursorBlockStart==-1)
     bBlockOn=FALSE;
  // CursorBlockStart=CursorBlockEnd=-1;             // ByHance, 96,3.4
  return(TimerInsert(CURSORBLINKTIME,Window));
}

static void CursorOff(void)
{
  if (bCursorOn)
     CursorDisplay();
}

static int LineBufferInsertString(unsigned char *Source,int XPos,
                unsigned char *String, int *SourceLength,int RightBound)
{
  int DestLength;

  DestLength=strlen(String);
  if ((*SourceLength)+DestLength>=RightBound)
  {
     DestLength=RightBound-*SourceLength;
     if (!(IsInChineseChar(Source,*SourceLength)&&(DestLength==1)))
        if (IsInChineseChar(String,DestLength))
           DestLength--;
  }
  if (!DestLength)
     return(0);
  memmove(&Source[XPos+DestLength],&Source[XPos],(*SourceLength)-XPos);
  strncpy(&Source[XPos],String,DestLength);
  *SourceLength+=DestLength;
  Source[*SourceLength]=0;
  return(DestLength);
}

static int LineBufferDeleteString(unsigned char *Source,int XPos,int Length,
                           int *SourceLength)
{
//  int Mid;
  if (Length>*SourceLength)
     Length=*SourceLength;
  if (Length)
     memmove(&Source[XPos],&Source[XPos+Length],(*SourceLength)-XPos);
  *SourceLength-=Length;
  Source[*SourceLength]=0;

/*---------------
  //By zjh 1997.4.13
  Mid= *SourceLength;
  if (*(SourceLength+1)>Mid)
    {
    if (Mid-5>=0)
     *(SourceLength+1) = Mid-5;
    else
     *(SourceLength+1) = 0;
    }
  //end
--------*/
  return(Length);
}

static void DrawSingleLineText(HWND Window,int X)
{
  HANDLE LineHandle;

  LineHandle=WindowGetUserData(Window);
  if (   // X<GetLineBufferLeftStart(LineHandle)||      // ByHance, 96,3.14
      X>GetLineBufferLength(LineHandle)
      ||X-GetLineBufferLeftStart(LineHandle)>GetEditorWidth(LineHandle))
     return;
  else
  {
     char String[MAXSINGLETEXTLENGTH];
     int Count/*,Length*/;
     int Left,Top,Right,Bottom;
     struct viewporttype TmpViewPort;

     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
//////////By Jerry
     setviewport(Left+1,Top+1,Right-1,Bottom-1,1);
     CursorOff();

     Area3DDownColor(0,0,getmaxx(),getmaxy(),Left-2,Top-2,Right+2,Bottom+2,
              2,SINGLELEBKCOLOR);

     X =GetLineBufferLeftStart(LineHandle);
     setviewport(Left+1,Top+1,Right-1,Bottom-1,1);

     //Count=GetEditorWidth(LineHandle)-(X-GetLineBufferLeftStart(LineHandle))+1;
     Count=GetLineBufferLength(LineHandle)-X+1;      // ByHance, 96,3.14
    /*---------- delete ByHance, 3.14 --
     if (X+Count<=GetLineBufferLength(LineHandle)
     && IsInChineseChar(GetLineBufferString(LineHandle),X+Count) )
           Count++;
     -----------*/

     strncpy(String,GetLineBufferString(LineHandle)+X,Count);
   //  strcpy(String,GetLineBufferString(LineHandle));
    /*---------- delete ByHance, 3.14 --
     Length=strlen(String);
     if (Count>Length)
        memset(String+Length,' ',Count-Length);
     String[Count]=0;
     -----------*/

     ViewportDisplayString(String,8*(X-GetLineBufferLeftStart(LineHandle)),
                           WindowGetHeight(Window)-2-CHARHEIGHT,
                           SINGLELECOLOR,SINGLELEBKCOLOR);
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
  }
}

static int EditorChangeHScroll(HWND Window)
{
  HANDLE LineHandle;
  int DecCount;

  LineHandle=WindowGetUserData(Window);
  if (GetLineCursor(LineHandle)<=GetLineBufferLeftStart(LineHandle)
      &&GetLineBufferLeftStart(LineHandle))
  {
     if (GetLineCursor(LineHandle)>GetEditorWidth(LineHandle)-5)
     {
        DecCount=GetLineCursor(LineHandle)-(GetEditorWidth(LineHandle)-5);
        if (IsInChineseChar(GetLineBufferString(LineHandle),DecCount))
           DecCount--;
        SetLineBufferLeftStart(LineHandle,DecCount);
     }
     else
        SetLineBufferLeftStart(LineHandle,0);
     DrawSingleLineText(Window,GetLineBufferLeftStart(LineHandle));
     return(TRUE);
  }
  if (GetLineCursor(LineHandle)-GetLineBufferLeftStart(LineHandle)
      >GetEditorWidth(LineHandle))
  {
     DecCount=GetLineCursor(LineHandle)-(GetEditorWidth(LineHandle)-5);
     if (DecCount>=GetLineCursor(LineHandle))
        DecCount=GetLineCursor(LineHandle)-1;
     if (IsInChineseChar(GetLineBufferString(LineHandle),DecCount))
        DecCount--;
     SetLineBufferLeftStart(LineHandle,DecCount);
     DrawSingleLineText(Window,GetLineBufferLeftStart(LineHandle));
     return(TRUE);
  }
  else
     return(FALSE);
}

static void CursorMoveXTo(int MovetoX)
{
  CursorOff();
  CursorPosX=MovetoX;
}

void CursorMoveTo(int MovetoX,int MovetoY)
{
//  CursorMoveXTo(MovetoX);
  CursorOff();
  CursorPosX=MovetoX;
  CursorPosY=MovetoY;
}

void CursorSetHeight(int Height)
{
  CursorOff();
  CursorHeight=Height;
}

static void DestroyCursor(int Cursor)
{
  TimerDelete(Cursor);
  CursorOff();
}

unsigned long SingleLineEditorDefaultProcedure(HWND Window,HMSG Message,
                                      long Param1,long Param2)
{
  int Result,i;
  int Length,X,Y;
  int DisplayPos;
  char *String;
  unsigned char MidString[30];
  int DelLength;
  static int Cursor=-1;
  HANDLE LineHandle;

  switch(Message)
  {
    case GETFOCUS:
         if ((Result=WindowDefaultProcedure(Window,Message,Param1,Param2))
             ==TRUE)
         {
            Cursor=CreatCursor(Window,CHARHEIGHT);
            CursorMoveTo(8*GetLineCursor(WindowGetUserData(Window)),
                         WindowGetHeight(Window)-3);
            CursorBlockStart=0;            // ByHance, 96,3.4
            CursorBlockEnd=GetLineBufferLength(WindowGetUserData(Window));
            MessageInsert(Window,TIMERTRIGGER,Cursor,0L); // ByJerry, 96,3.18
         }
         else
            return(Result);
         break;
    case MOUSEMOVE:
         X=(short)MAKEHI(Param1);
         Y=(short)MAKELO(Param1);
         if(X<0 || X>WindowGetWidth(Window)
          ||Y<0 || Y>WindowGetHeight(Window))
         {
            DialogMouseMove(Window,Message,Param1,Param2);
            break;
         }
         MouseSetGraph(VCARETMOUSE);         // ByHance, 95,12.6
         break;
    case WINDOWQUIT:
         FreeLineBuffer(Window);
    case LOSTFOCUS:
         Result=WindowDefaultProcedure(Window,Message,Param1,Param2);
         if (Cursor>=0)
         {
            DestroyCursor(Cursor);
            Cursor=-1;
         }
         CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
         bBlockOn=FALSE;
         return(Result);
    case DRAWWINDOW:
         if ((Result=WindowDefaultProcedure(Window,Message,Param1,Param2))
             !=TRUE)
            return(Result);
    case WMPAINT:
         DrawSingleLineText(Window,
             GetLineBufferLeftStart(WindowGetUserData(Window)));
         break;
    case TIMERTRIGGER:
         if (Param1==Cursor)
            CursorDisplay();
         break;
    case SETLINEBUFFER:
         String=(char *)LONG2FP(Param1);
         SetLineBufferString(WindowGetUserData(Window),String);
         SetLineBufferLength(WindowGetUserData(Window),strlen(String));
         SetLineBufferLeftStart(WindowGetUserData(Window),0);
         SetLineCursor(WindowGetUserData(Window),0);
         CursorMoveXTo(0);
//         MessageInsert(Window,WMPAINT,0l,GetEditorWidth(WindowGetUserData(Window)));
         break;
    case GETLINEBUFFER:
         String=GetLineBufferString(WindowGetUserData(Window));
         return(FP2LONG(String));
    case KEYSTRING:
         if(CursorBlockStart<CursorBlockEnd)  // ByHance, 97,5.8
         {
           DelLength=CursorBlockEnd-CursorBlockStart;
           Length=LineBufferDeleteString(GetLineBufferString(WindowGetUserData(Window)),
                         CursorBlockStart,DelLength,
                         GetLineLengthAddress(WindowGetUserData(Window)));
           SetLineCursor(WindowGetUserData(Window),CursorBlockStart);
           CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
         }

         Length=LineBufferInsertString(GetLineBufferString(WindowGetUserData(Window)),
                GetLineCursor(WindowGetUserData(Window)),
                (char *)LONG2FP(Param1),
                GetLineLengthAddress(WindowGetUserData(Window)),
                MAXSINGLETEXTLENGTH);
         if (Length<Param2)
         {
            Alarm();
            if (Length>0&&(!IsInChineseChar(GetLineBufferString(
                 WindowGetUserData(Window)),GetLineBufferLength(
                 WindowGetUserData(Window)))))
            {
               Length--;
            }
         }
         if (Length>0)
         {
            IncLineCursor(WindowGetUserData(Window),Length);
            if(!EditorChangeHScroll(Window)
            && !IsInChineseChar(GetLineBufferString(WindowGetUserData(Window)),
                                GetLineCursor(WindowGetUserData(Window))) )
            {
               CursorOff();
               MouseHidden();
               DisplayPos=GetLineCursor(WindowGetUserData(Window))-Length;
               DrawSingleLineText(Window,DisplayPos);
               MouseShow();
            }
            CursorMoveXTo(8*(GetLineCursor(WindowGetUserData(Window))-
                          GetLineBufferLeftStart(WindowGetUserData(Window))));
         }
         break;
    case KEYDOWN:
         switch (MAKELO(Param1))
         {
           case UP:
           case LEFT:
                CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
                {
                  int LeftCount;

                  for (LeftCount=i=0;i<Param2;i++)
                  {
                      LeftCount++;
                      if (LeftCount>GetLineCursor(WindowGetUserData(Window)))
                         break;
                      if (GetLineBufferChar(WindowGetUserData(Window),
                          GetLineCursor(WindowGetUserData(Window))-LeftCount)>0xa0)
                         LeftCount++;
                  }
                  if (i<Param2)
                  {
                     Alarm();
                     SetLineCursor(WindowGetUserData(Window),0);
                  }
                  else
                     DecLineCursor(WindowGetUserData(Window),LeftCount);
                  EditorChangeHScroll(Window);
                  CursorMoveXTo(8*(GetLineCursor(WindowGetUserData(Window))-
                                   GetLineBufferLeftStart(WindowGetUserData(Window))));
                }
                break;
           case DOWN:
           case RIGHT:
                CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
                {
                  int RightCount;

                  for (RightCount=i=0;i<Param2;i++)
                  {
                      RightCount++;
                      if (RightCount+GetLineCursor(WindowGetUserData(Window))>
                          GetLineBufferLength(WindowGetUserData(Window)))
                         break;
                      if (GetLineBufferChar(WindowGetUserData(Window),
                          GetLineCursor(WindowGetUserData(Window))+RightCount-1)>0xa0)
                         RightCount++;
                  }
                  if (i<Param2)
                  {
                     Alarm();
                     SetLineCursor(WindowGetUserData(Window),
                                   GetLineBufferLength(WindowGetUserData(Window)));
                  }
                  else
                     IncLineCursor(WindowGetUserData(Window),RightCount);
                  EditorChangeHScroll(Window);
                  CursorMoveXTo(8*(GetLineCursor(WindowGetUserData(Window))-
                                   GetLineBufferLeftStart(WindowGetUserData(Window))));
                }
                break;
           case HOME:
                CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
                if (GetLineCursor(WindowGetUserData(Window))>0)
                {
                   SetLineCursor(WindowGetUserData(Window),0);
                   EditorChangeHScroll(Window);
                   CursorMoveXTo(8*(GetLineCursor(WindowGetUserData(Window))-
                                 GetLineBufferLeftStart(WindowGetUserData(Window))));
                }
                else
                   Alarm();
                break;
           case END:
                CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
                if (GetLineCursor(WindowGetUserData(Window))<GetLineBufferLength(WindowGetUserData(Window)))
                {
                   SetLineCursor(WindowGetUserData(Window),GetLineBufferLength(WindowGetUserData(Window)));
                   EditorChangeHScroll(Window);
                   CursorMoveXTo(8*(GetLineCursor(WindowGetUserData(Window))-
                                 GetLineBufferLeftStart(WindowGetUserData(Window))));
                }
                else
                   Alarm();
                break;
           case SHIFT_LEFT:
                if(CursorBlockStart>=CursorBlockEnd)
                {
                   CursorBlockEnd=GetLineCursor(WindowGetUserData(Window));
                   //CursorOff();
                }
                else              // hidden old
                   CursorDispBlk(GetLineBufferLeftStart(
                                   WindowGetUserData(Window)) );

                CursorBlockStart=GetLineCursor(WindowGetUserData(Window));
                if(CursorBlockStart==0)
                {
                   Alarm(); CursorDispBlk(0);  break;
                }

                --CursorBlockStart;
                if(GetLineBufferChar(WindowGetUserData(Window),CursorBlockStart)
                   >0xa0)
                     --CursorBlockStart;

                SetLineCursor(WindowGetUserData(Window),CursorBlockStart);
                EditorChangeHScroll(Window);
                CursorDispBlk(GetLineBufferLeftStart(
                                   WindowGetUserData(Window)) );
                break;
           case SHIFT_RIGHT:
                if(CursorBlockStart>=CursorBlockEnd)
                {
                   CursorBlockStart=GetLineCursor(WindowGetUserData(Window));
                   //CursorOff();
                }
                else            // hidden old
                   CursorDispBlk(GetLineBufferLeftStart(
                                   WindowGetUserData(Window)) );

                CursorBlockEnd=GetLineCursor(WindowGetUserData(Window));
                if(CursorBlockEnd>=GetLineBufferLength(WindowGetUserData(Window)))
                {
                   Alarm();
                   CursorDispBlk(GetLineBufferLeftStart(
                                   WindowGetUserData(Window)) );
                   break;
                }

                ++CursorBlockEnd;
                if(GetLineBufferChar(WindowGetUserData(Window),CursorBlockStart)
                   >0xa0)
                     ++CursorBlockEnd;

                SetLineCursor(WindowGetUserData(Window),CursorBlockEnd);
                EditorChangeHScroll(Window);
                CursorDispBlk(GetLineBufferLeftStart(
                                   WindowGetUserData(Window)) );
                break;

           case SHIFT_HOME:
                if(CursorBlockStart>=CursorBlockEnd)
                {
                   CursorBlockEnd=GetLineCursor(WindowGetUserData(Window));
                   //CursorOff();
                }
                else            // hidden old
                   CursorDispBlk(GetLineBufferLeftStart(
                                   WindowGetUserData(Window)) );

                CursorBlockStart=0;
                SetLineCursor(WindowGetUserData(Window),0);
                EditorChangeHScroll(Window);
                CursorDispBlk(GetLineBufferLeftStart(
                                   WindowGetUserData(Window)) );
                break;
           case SHIFT_END:
                if(CursorBlockStart>=CursorBlockEnd)
                {
                   CursorBlockStart=GetLineCursor(WindowGetUserData(Window));
                   //CursorOff();
                }
                else            // hidden old
                   CursorDispBlk(GetLineBufferLeftStart(
                                   WindowGetUserData(Window)) );

                CursorBlockEnd=GetLineBufferLength(WindowGetUserData(Window));
                SetLineCursor(WindowGetUserData(Window),CursorBlockEnd);
                EditorChangeHScroll(Window);
                CursorDispBlk(GetLineBufferLeftStart(
                                   WindowGetUserData(Window)) );
                break;
           case DEL:
                if(CursorBlockStart<CursorBlockEnd)
                {               // ByHance, 96,3.4
           del_blk:
                  CursorOff();
                  LineHandle=WindowGetUserData(Window);
                  DelLength=CursorBlockEnd-CursorBlockStart;
                  Length=LineBufferDeleteString(GetLineBufferString(LineHandle),
                               CursorBlockStart,DelLength,
                               GetLineLengthAddress(LineHandle));
                  SetLineCursor(LineHandle,CursorBlockStart);

                  if (CursorBlockStart <= GetEditorWidth(LineHandle))
                     SetLineBufferLeftStart(LineHandle,0);

                  EditorChangeHScroll(Window);
                  DrawSingleLineText(Window,GetLineBufferLeftStart(LineHandle));
                  // CursorMoveXTo(CursorBlockStart*8);
                  CursorMoveXTo(8*
                       (GetLineCursor(LineHandle)-GetLineBufferLeftStart(LineHandle)) );

                  CursorBlockStart=CursorBlockEnd=-1;
                  break;
                }

                if (GetLineCursor(WindowGetUserData(Window))>=
                    GetLineBufferLength(WindowGetUserData(Window)))
                {
                   Alarm();
                   break;
                }
                for (i=DelLength=0;i<Param2;i++)
                {
                    DelLength++;
                    if (DelLength+GetLineCursor(WindowGetUserData(Window))>=
                        GetLineBufferLength(WindowGetUserData(Window)))
                       break;
                    if (GetLineBufferChar(WindowGetUserData(Window),
                        GetLineCursor(WindowGetUserData(Window))+DelLength-1)>0xa0)
                       DelLength++;
                }
                if ((i<Param2)&&(DelLength<Param2))
                   Alarm();

                Length=LineBufferDeleteString(GetLineBufferString(
                              WindowGetUserData(Window)),
                              GetLineCursor(WindowGetUserData(Window)),
                              DelLength,GetLineLengthAddress(WindowGetUserData(Window)));
                if (Length>0)
                   DrawSingleLineText(Window,GetLineCursor(WindowGetUserData(Window)));

                if (Length<DelLength)
                   Alarm();
                break;
           case INS:
                break;
           case BACKSPACE:
                if(CursorBlockStart<CursorBlockEnd)
                     goto del_blk;

                if (GetLineCursor(WindowGetUserData(Window))==0)
                {
                   Alarm();
                   break;
                }
                for (i=DelLength=0;i<Param2;i++)
                {
                    DelLength++;
                    if (DelLength>GetLineCursor(WindowGetUserData(Window)))
                       break;
                    if (GetLineBufferChar(WindowGetUserData(Window),
                        GetLineCursor(WindowGetUserData(Window))-DelLength)>0xa0)
                       DelLength++;
                }
                if (i<Param2)
                   Alarm();

                if (DelLength>GetLineCursor(WindowGetUserData(Window)))
                {
                   Alarm();
                   DelLength=GetLineCursor(WindowGetUserData(Window));
                }

                DecLineCursor(WindowGetUserData(Window),DelLength);
                Length=LineBufferDeleteString(GetLineBufferString(WindowGetUserData(Window)),
                                       GetLineCursor(WindowGetUserData(Window)),
                                       DelLength,GetLineLengthAddress(WindowGetUserData(Window)));
                if (Length>0)
                {
                   if (!EditorChangeHScroll(Window))
                      DrawSingleLineText(Window,GetLineCursor(WindowGetUserData(Window)));
                   CursorMoveXTo(8*(GetLineCursor(WindowGetUserData(Window))-
                                    GetLineBufferLeftStart(WindowGetUserData(Window))));
                }

                if (Length<DelLength)
                   Alarm();
                break;
           case ENTER:
                if (Cursor>=0)
                {
                   DestroyCursor(Cursor);
                   Cursor=-1;
                }
                CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
                MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
                break;
           case SHIFT_TAB:
                WindowTableOrderPrev(Window);
                break;
           case TAB:
                WindowTableOrderNext(Window);
                break;
           case ESC:
                MessageInsert(WindowGetFather(Window),Message,Param1,Param2);
                break;
           default:
                if (Param1<32||Param1>255)
                   break;
                memset(MidString,0,sizeof(MidString));
                if(Param2>30) Param2=30;             // for Overflow

                if( (Wchar)(MAKELO(Param1)&0xff)>0xa0
                 && (Wchar)(MAKEHI(Param1)&0xff00)>0xa000 )   // Chinese
                {
                   for (Length=0;Length<Param2;Length++)
                       ((Wchar *)MidString)[2*Length]=MAKELO(Param1);
                }
                else                                    // English
                   for (Length=0;Length<Param2;Length++)
                       MidString[Length]=MAKELO(Param1);
                MidString[Length]=0;

                if(CursorBlockStart<CursorBlockEnd)
                {               // ByHance, 96,3.4
                  DelLength=CursorBlockEnd-CursorBlockStart;
                  Length=LineBufferDeleteString(GetLineBufferString(WindowGetUserData(Window)),
                                CursorBlockStart,DelLength,
                                GetLineLengthAddress(WindowGetUserData(Window)));
                /*-------------------------
                  if( GetLineCursor(WindowGetUserData(Window)) >
                    GetLineBufferLength(WindowGetUserData(Window)) )
                  {
                     SetLineCursor(WindowGetUserData(Window),
                              GetLineBufferLength(WindowGetUserData(Window)));
                  }
                  -----------------------*/
                  SetLineCursor(WindowGetUserData(Window),CursorBlockStart);
                  CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
                }

                Length=LineBufferInsertString(GetLineBufferString(WindowGetUserData(Window)),
                        GetLineCursor(WindowGetUserData(Window)),
                        MidString,
                        GetLineLengthAddress(WindowGetUserData(Window)),
                        MAXSINGLETEXTLENGTH);
                if (Length<Param2)
                {
                   Alarm();
                   if(Length>0
                   && !IsInChineseChar(GetLineBufferString(WindowGetUserData(Window)),
                                GetLineBufferLength(WindowGetUserData(Window)))
                   ) {
                      Length--;
                   }
                }

                if (Length>0)
                {
                   IncLineCursor(WindowGetUserData(Window),Length);
                   if( !EditorChangeHScroll(Window)
                   && !IsInChineseChar(GetLineBufferString(WindowGetUserData(Window)),
                                     GetLineCursor(WindowGetUserData(Window))) )
                   {
                      CursorOff();
                      MouseHidden();
                      DisplayPos=GetLineCursor(WindowGetUserData(Window))-Length;
                      if (*(GetLineBufferString(WindowGetUserData(Window))+
                           GetLineCursor(WindowGetUserData(Window))-1)>0xa0)
                         DisplayPos--;
                      DrawSingleLineText(Window,DisplayPos);
                      MouseShow();
                   }
                   CursorMoveXTo(8*(GetLineCursor(WindowGetUserData(Window))-
                            GetLineBufferLeftStart(WindowGetUserData(Window))));
                }
                break;
         }
         break;
    case MOUSELEFTDOWN:
         {
           int XCursor;

           // XCursor=(MAKEHI(Param1)-1)/8;
           XCursor=(MAKEHI(Param1)+4)/8;
           if (XCursor+GetLineBufferLeftStart(WindowGetUserData(Window))
               >GetLineBufferLength(WindowGetUserData(Window)))
           {
                XCursor=GetLineBufferLength(WindowGetUserData(Window))
                       -GetLineBufferLeftStart(WindowGetUserData(Window));
           }

           if (IsInChineseChar(GetLineBufferString(WindowGetUserData(Window)),
             XCursor+GetLineBufferLeftStart(WindowGetUserData(Window))))
               XCursor--;


           if(bBlockOn==TRUE)
               CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
           CursorBlockStart=CursorBlockEnd=0;
           bBlockOn=TRUE;
           SetLineCursor(WindowGetUserData(Window),XCursor
                         +GetLineBufferLeftStart(WindowGetUserData(Window)));
           CursorMoveXTo(8*(GetLineCursor(WindowGetUserData(Window))-
                         GetLineBufferLeftStart(WindowGetUserData(Window))));
         }
         break;
    case MOUSELEFTDOUBLE:
         if(CursorBlockStart<CursorBlockEnd)    // hide old
             CancelCursorBlk(GetLineBufferLeftStart(WindowGetUserData(Window)));
         CursorOff();
         CursorBlockStart=0;
         CursorBlockEnd=GetLineBufferLength(WindowGetUserData(Window));
         SetLineCursor(WindowGetUserData(Window),0);
         EditorChangeHScroll(Window);
         CursorDispBlk(GetLineBufferLeftStart(WindowGetUserData(Window)) );
         break;
    case MOUSELEFTDROP:
         break;
    case GETTEXTWIDTH:
         return((WindowGetWidth(Window)-2)*2/CHARWIDTH);
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

int CreatSingleLineEditor(int Left,int Top,int Right,int Bottom,
            Function *SingleLineEditorProcedure, HWND FatherWindow)
{
  Windows TobeCreatWindow;

  memset(&TobeCreatWindow,0,sizeof(TobeCreatWindow));
  TobeCreatWindow.Left=Left;
  TobeCreatWindow.Top=Top;
  TobeCreatWindow.Right=Right;
  TobeCreatWindow.Bottom=Bottom;
  if (SingleLineEditorProcedure==NULL)
     TobeCreatWindow.Procedure=(Function *)SingleLineEditorDefaultProcedure;
  else
     TobeCreatWindow.Procedure=SingleLineEditorProcedure;

    //TobeCreatWindow.UserData=AllocLineBuffer();
    //assert(CurrentAllocLine<MAXSINGLELINEBUFFER);
  TobeCreatWindow.UserData=CurrentAllocLine++;

  SetLineBuffer(TobeCreatWindow.UserData,0);
  SetEditorWidth(TobeCreatWindow.UserData,(Right-Left-2)/8);
  TobeCreatWindow.WindowStyle=WindowSetIsUserWindow()
                              |WindowSetCanTabOrder()
                              |WindowSetResizeable()
                              |WindowSetMoveable();

  return(WindowAppend(&TobeCreatWindow,FatherWindow));
}

void SingleLineEditorInitial(void)
{
  memset(TestLineBuffer,0,sizeof(SingleLine)*MAXSINGLELINEBUFFER);
}

void SingleLineEditorEnd(void)
{
}
