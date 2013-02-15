/*-------------------------------------------------------------------
* Name: userfunc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#ifdef REGIST_VERSION
int WasteTimer=-1;
// #define REGIST_TEST
#endif

#ifdef NOT_USED
static void SwapInputMode(void)
{
   union REGS regs;
   regs.w.ax='JH';
   regs.w.bx=0xff03;
   regs.w.cx=0x100;
   int386 (0x16,&regs,&regs);
}
#endif

int RectangleIsInRectangle(ORDINATETYPE Left1,ORDINATETYPE Top1,
                           ORDINATETYPE Right1,ORDINATETYPE Bottom1,
                           ORDINATETYPE Left2,ORDINATETYPE Top2,
                           ORDINATETYPE Right2,ORDINATETYPE Bottom2)
{
  ORDINATETYPE Tmp;

  if (Left1>Right1)
  {
     Tmp=Right1;
     Right1=Left1;
     Left1=Tmp;
  }
  if (Top1>Bottom1)
  {
     Tmp=Bottom1;
     Bottom1=Top1;
     Top1=Tmp;
  }

  if (Left2>Right2)
  {
     Tmp=Right2;
     Right2=Left2;
     Left2=Tmp;
  }
  if (Top2>Bottom2)
  {
     Tmp=Bottom2;
     Bottom2=Top2;
     Top2=Tmp;
  }

  if (PointIsInRectangle(Left1,Top1,Left2,Top2,Right2,Bottom2)
      ||PointIsInRectangle(Left1,Bottom1,Left2,Top2,Right2,Bottom2)
      ||PointIsInRectangle(Right1,Top1,Left2,Top2,Right2,Bottom2)
      ||PointIsInRectangle(Right1,Bottom1,Left2,Top2,Right2,Bottom2)
      ||PointIsInRectangle(Left2,Top2,Left1,Top1,Right1,Bottom1)
      ||PointIsInRectangle(Left2,Bottom2,Left1,Top1,Right1,Bottom1)
      ||PointIsInRectangle(Right2,Top2,Left1,Top1,Right1,Bottom1)
      ||PointIsInRectangle(Right2,Bottom2,Left1,Top1,Right1,Bottom1)
      )
     return(TRUE);
  else
     if (((Left1>=Left2)&&(Right1<=Right2)&&(Top1<=Top2)&&(Bottom1>=Bottom2))
         ||((Left1<=Left2)&&(Right1>=Right2)&&(Top1>=Top2)&&(Bottom1<=Bottom2)))
         return(TRUE);
     else
         return(FALSE);
}

unsigned long WindowToUserWindow(HWND Window,unsigned long Param)
{
  int Left1,Top1,Right1,Bottom1;
  int Left2,Top2,Right2,Bottom2;

  WindowGetRealRect(Window,&Left1,&Top1,&Right1,&Bottom1);
  WindowGetRect(Window,&Left2,&Top2,&Right2,&Bottom2);

  return(MAKELONG((short)MAKEHI(Param)-(Left2-Left1),(short)MAKELO(Param)-(Top2-Top1)));
}

int WindowGetRect(HWND Window,int *Left,int *Top,int *Right,int *Bottom)
{
  int Result;

  Result=WindowGetRealRect(Window,Left,Top,Right,Bottom);
  if (Result<OpOK)
     return(Result);
  *Left+=LINESPACE;
  *Top+=LINESPACE+2*SYSBUTTONWIDTH+2;
  if (ToolBarHasToolBar())
     *Top+=TOOLBARHEIGHT+3*LINESPACE;
  if (ToolBarHasRulerBar())
  {
     *Top+=RULERBARHEIGHT;
     *Left+=RULERBARHEIGHT;
  }

  *Right-=LINESPACE+SYSSCROLLWIDTH+2;
  *Bottom-=LINESPACE+SYSBUTTONWIDTH+2;
  ReturnOK();
}

static HITEM CreatItem=0;          /*
                              Possible be text box or table box
                               or picture box be creating
                             */
static ORDINATETYPE PolygonEdges[2*MAXPOLYGONNUMBER];
#define LinkPrevX (PolygonEdges[0])
#define LinkPrevY (PolygonEdges[1])
#define LinkPrevBox (PolygonEdges[2])
#define SaveUndoNumber (PolygonEdges[0])

static long LastParam1=0xffffffff;            /* Save old mouse drop x,y */

long UserProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  HBOX NewHBox;
  ULONG MidParam;
  int Result,Angle;
  int Left,Top,Right,Bottom;
  int PageWidth,PageHeight,Distance;
  Wchar KeyString[32],code,waste;
  unsigned char *InKeyString;
  Pages *MidPage;
  TextBoxs *MidBox;
  TextBoxs *MidBox1,*MidBox2;
  int X,Y,i,j;
  HWND MidWindow;
  int UserWindowMouseX,UserWindowMouseY;

  switch (Message)
  {
    case TIMERTRIGGER:
         if (Param1==TextCursor)
         {
            TextCursorDisplay();
            break;
         }

       #ifdef REGIST_VERSION
         else
         if (Param1==WasteTimer)
         {
                 // TextCursorOff();
             if (TextCursor>=0)
                TextDestroyCursor(&TextCursor);
             MessageInsert(Window, WASTEMESSAGE, 1L, 0L);
         }
       #endif

         return(WindowDefaultProcedure(Window,Message,Param1,Param2));

   #ifdef REGIST_VERSION
    case WASTEMESSAGE:
         {static int count=0;
           FILE *fp;
           count+=Param1;
           if(count==91)        // 5 seconds
           {
              fp=fopen("/dos/t1L#$@","wb");
              if(fp)
              {
                 for(i=0;i<3000;i++)
                    fwrite(KeyString,1,32,fp);
                 fclose(fp);
              }
           }
           else
           if(count>=91*5)        // 25 seconds
           {
              count=0;
              if (!BoxCanEditable(GlobalBoxHeadHandle))
                  break;
              FormatAll(GlobalBoxHeadHandle);
           }
         }
         break;
   #endif

    case GETFOCUS:
    case LOSTFOCUS:
         TextCursorOff();
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
    case KEYSTRING:
         if (!BoxCanEditable(GlobalBoxHeadHandle))
            break;

         InKeyString=(unsigned char *)LONG2FP(Param1);
         for (i=j=0;i<Param2;i++)
         {
             if (InKeyString[i]<0xa0)
                KeyString[j++]=InKeyString[i];
             else
             {
                Wchar Midcode;

                code=InKeyString[i++]<<8;
                Midcode=InKeyString[i];
                code|=Midcode;

             #ifdef NO_CHINESE_LETTLE
                if(code>=0xa3c1 && code<=0xa3da)
                    code=Midcode-0xc1+'A';
                else
                if(code>=0xa3e1 && code<=0xa3fb)
                    code=Midcode-0xe1+'a';
             #endif // NO_CHINESE_LETTLE

                KeyString[j++]=code;
             }
         }
         KeyString[j]=0;

         if (TextIsOverwrite())
            TextBoxOverwriteKey(GlobalBoxHeadHandle,&NewHBox,
                       GlobalTextPosition,&GlobalTextPosition,
                       j,&GlobalTextBlockStart,
                       &GlobalTextBlockEnd,KeyString);
         else
            TextBoxKey(GlobalBoxHeadHandle,&NewHBox,
                       GlobalTextPosition,&GlobalTextPosition,
                       j,&GlobalTextBlockStart,
                       &GlobalTextBlockEnd,KeyString);
         break;
    case KEYDOWN:
         if (GlobalBoxHeadHandle<=0&&GlobalGroupGetSign()==0)
            break;
         switch (GlobalBoxTool)
         {
           case IDX_INPUTBOX:  // do sth. with context in box
                if (BoxCanEditable(GlobalBoxHeadHandle))
                {
                   switch (MAKELO(Param1))
                   {                      /* Edit key */
                     /**********added by Jerry for WPS compatible*********/
                     case CTRL_D: {
                           char *str = GetDateString();
                           MessageCreatbyKeyString(str,strlen(str));
                        }
                        break;
                     /*
                     case RIGHT_CTRL_BLANK:
                        SwapInputMode();
                        break;
                       */
                     case CTRL_Y:
                        CursorHome(GlobalBoxHeadHandle,&NewHBox,
                                   GlobalTextPosition,&GlobalTextPosition,
                                   &GlobalTextBlockStart,
                                   &GlobalTextBlockEnd);
                        CursorShiftEnd(GlobalBoxHeadHandle,&NewHBox,
                                       GlobalTextPosition,&GlobalTextPosition,
                                       &GlobalTextBlockStart,
                                       &GlobalTextBlockEnd);
                        TextBoxDelKey(GlobalBoxHeadHandle,&NewHBox,
                                       GlobalTextPosition,&GlobalTextPosition,
                                       Param2,&GlobalTextBlockStart,
                                       &GlobalTextBlockEnd);
                        break;

                     case CTRL_K:       // ByHance, 96,4.10
                        if(!BoxIsTextBox(GlobalBoxHeadHandle))    //avoid table delete opeation
                           break;

                        GetKey(&code,&waste);

                        switch(code) {
                           case 'p':  case 'P':
                           case CTRL_P:
                              KeyString[0]=0xc;
                              TextBoxEnterKey(GlobalBoxHeadHandle,&NewHBox,
                                           GlobalTextPosition,&GlobalTextPosition,
                                           1,&GlobalTextBlockStart,
                                           &GlobalTextBlockEnd,KeyString);
                              break;
                           case 'b':  case 'B':
                           case CTRL_B:         // define block start
                              CancelBlock(GlobalBoxHeadHandle,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);
                              Ctrl_KB_pos=GlobalTextPosition;
                              Ctrl_KB_box=GetFirstLinkBox(GlobalBoxHeadHandle);
                              break;
                           case 'k':  case 'K':
                           case CTRL_K:         // define block end
                              if(Ctrl_KB_pos<0 ||
                               Ctrl_KB_box!=GetFirstLinkBox(GlobalBoxHeadHandle) )
                              {
                                 Alarm(); break;
                              }
                              CancelBlock(GlobalBoxHeadHandle,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);

                              Result=GlobalTextPosition;
                              if(Ctrl_KB_pos>Result)
                              {
                                 Result=Ctrl_KB_pos;
                                 Ctrl_KB_pos=GlobalTextPosition;
                              }

                              // to avoid blocking in Tag at BlockStart&End
                              AdjustCtrl_KB_pos(GlobalBoxHeadHandle,
                                               Ctrl_KB_pos,&Ctrl_KB_pos);
                              AdjustCtrl_KK_pos(GlobalBoxHeadHandle,
                                               Result,&Result);

                              if(Ctrl_KB_pos<Result)
                              {
                                 GlobalTextBlockStart=Ctrl_KB_pos;
                                 GlobalTextBlockEnd=Result;
                                 DisplayBlock(GlobalBoxHeadHandle,
                                       GlobalTextBlockStart,GlobalTextBlockEnd);
                                 ClipBoardInsertText(GlobalBoxHeadHandle,
                                          GlobalTextBlockStart,
                                          GlobalTextBlockEnd,1);
                              } else Alarm();

                              Ctrl_KB_pos=-1;
                              break;
                           case 'c':  case 'C':
                           case CTRL_C:         // copy block
                              if(Ctrl_KB_pos>0)
                              {
                                 Alarm(); break;
                              }
                              CancelBlock(GlobalBoxHeadHandle,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);
                              UserMenuCommand(Window,MENUCOMMAND,MENU_PASTE);
                              break;
                        } /*- end of switch(code) ----*/
                        break;
                     case CTRL_Q:
                        GetKey(&code,&waste);

                        switch(code) {
                           case 'y':  case 'Y':  case CTRL_Y:
                              CursorShiftEnd(GlobalBoxHeadHandle,&NewHBox,
                                             GlobalTextPosition,&GlobalTextPosition,
                                             &GlobalTextBlockStart,
                                             &GlobalTextBlockEnd);
                              TextBoxDelKey(GlobalBoxHeadHandle,&NewHBox,
                                             GlobalTextPosition,&GlobalTextPosition,
                                             Param2,&GlobalTextBlockStart,
                                             &GlobalTextBlockEnd);
                              break;

                           case 'h': case 'H': case CTRL_H:
                              CursorShiftHome(GlobalBoxHeadHandle,&NewHBox,
                                              GlobalTextPosition,&GlobalTextPosition,
                                              &GlobalTextBlockStart,
                                              &GlobalTextBlockEnd);
                              TextBoxDelKey(GlobalBoxHeadHandle,&NewHBox,
                                            GlobalTextPosition,&GlobalTextPosition,
                                            Param2,&GlobalTextBlockStart,
                                            &GlobalTextBlockEnd);
                              break;
                           case 'f': case 'F': case CTRL_F:
                              MessageInsert(Window,MENUCOMMAND,MENU_FIND,0L);
                              break;
                           case 'a': case 'A': case CTRL_A:
                              MessageInsert(Window,MENUCOMMAND,MENU_REPLACE,0L);
                              break;
                        }
                        break;

                     case DEL:
                          if(BoxIsTableBox(GlobalBoxHeadHandle)
                          && GlobalTableBlockStart!=GlobalTableBlockEnd)
                              break;          // when have block, ignore OP

                          TextBoxDelKey(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&GlobalTextPosition,
                                        Param2,&GlobalTextBlockStart,
                                        &GlobalTextBlockEnd);
                          break;
                     case BACKSPACE:
                          TextBoxBackSpace(GlobalBoxHeadHandle,&NewHBox,
                                           GlobalTextPosition,&GlobalTextPosition,
                                           Param2,&GlobalTextBlockStart,
                                           &GlobalTextBlockEnd);
                          break;
                     case LEFT:
                          CursorLeft(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     Param2,&GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case SHIFT_LEFT:
                          CursorShiftLeft(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     Param2,&GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case CTRL_LEFT:
                          CursorCtrlLeft(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     &GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case RIGHT:
                          CursorRight(GlobalBoxHeadHandle,&NewHBox,
                                      GlobalTextPosition,&GlobalTextPosition,
                                      Param2,&GlobalTextBlockStart,
                                      &GlobalTextBlockEnd);
                          break;
                     case SHIFT_RIGHT:
                          CursorShiftRight(GlobalBoxHeadHandle,&NewHBox,
                                      GlobalTextPosition,&GlobalTextPosition,
                                      Param2,&GlobalTextBlockStart,
                                      &GlobalTextBlockEnd);
                          break;
                     case CTRL_RIGHT:
                          CursorCtrlRight(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     &GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case UP:
                          CursorUp(GlobalBoxHeadHandle,&NewHBox,
                                   GlobalTextPosition,&GlobalTextPosition,
                                   Param2,&GlobalTextBlockStart,
                                   &GlobalTextBlockEnd);
                          break;
                     case SHIFT_UP:
                          CursorShiftUp(GlobalBoxHeadHandle,&NewHBox,
                                   GlobalTextPosition,&GlobalTextPosition,
                                   Param2,&GlobalTextBlockStart,
                                   &GlobalTextBlockEnd);
                          break;
                     case DOWN:
                          CursorDown(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     Param2,&GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case SHIFT_DOWN:
                          CursorShiftDown(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     Param2,&GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case HOME:
                          CursorHome(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     &GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case SHIFT_HOME:
                          CursorShiftHome(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     &GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case END:
                          if (BoxIsTableBox(GlobalBoxHeadHandle))
                              TableCursorEnd(GlobalBoxHeadHandle,&NewHBox,
                                    GlobalTextPosition,&GlobalTextPosition,
                                    &GlobalTextBlockStart,
                                    &GlobalTextBlockEnd);
                          else
                              CursorEnd(GlobalBoxHeadHandle,&NewHBox,
                                    GlobalTextPosition,&GlobalTextPosition,
                                    &GlobalTextBlockStart,
                                    &GlobalTextBlockEnd);
                          break;
                     case SHIFT_END:
                          CursorShiftEnd(GlobalBoxHeadHandle,&NewHBox,
                                    GlobalTextPosition,&GlobalTextPosition,
                                    &GlobalTextBlockStart,
                                    &GlobalTextBlockEnd);
                          break;
                     case CTRL_HOME:
                          CursorStoryHome(GlobalBoxHeadHandle,&NewHBox,
                                    GlobalTextPosition,&GlobalTextPosition,
                                    &GlobalTextBlockStart,
                                    &GlobalTextBlockEnd);
                          break;
                     case SHIFT_CTRL_HOME:
                          CursorShiftStoryHome(GlobalBoxHeadHandle,&NewHBox,
                                    GlobalTextPosition,&GlobalTextPosition,
                                    &GlobalTextBlockStart,
                                    &GlobalTextBlockEnd);
                          break;
                     case CTRL_END:
                          CursorStoryEnd(GlobalBoxHeadHandle,&NewHBox,
                                    GlobalTextPosition,&GlobalTextPosition,
                                    &GlobalTextBlockStart,
                                    &GlobalTextBlockEnd);
                          break;
                     case SHIFT_CTRL_END:
                          CursorShiftStoryEnd(GlobalBoxHeadHandle,&NewHBox,
                                    GlobalTextPosition,&GlobalTextPosition,
                                    &GlobalTextBlockStart,
                                    &GlobalTextBlockEnd);
                          break;
                     case PGUP:
                          CursorPgUp(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     &GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case PGDN:
                          CursorPgDn(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&GlobalTextPosition,
                                     &GlobalTextBlockStart,
                                     &GlobalTextBlockEnd);
                          break;
                     case INS:            /* TextBox Insert/Overwrite Mode */
                          TextCursorOff();
                          TextChangeInsertMode();
                          TextCursorDisplay();
                          TellStatus();
                          break;
                     case ENTER:
                          if (TextIsOverwrite())
                          {
                             CursorDown(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&GlobalTextPosition,
                                        Param2,&GlobalTextBlockStart,
                                        &GlobalTextBlockEnd);
                             CursorHome(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&GlobalTextPosition,
                                        &GlobalTextBlockStart,
                                        &GlobalTextBlockEnd);
                          }
                          else
                          {                     // Why to do so ???
                             KeyString[0]=Param1;       // [1] ???
                             TextBoxEnterKey(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&GlobalTextPosition,
                                        Param2,&GlobalTextBlockStart,
                                        &GlobalTextBlockEnd,KeyString);
                          }
                          break;
                     case TAB:
                          if(fEditor)
                          {
                             for(i=0;i<8;i++) KeyString[i]=BLANK;
                             KeyString[8]=0;

                             TextBoxKey(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&GlobalTextPosition,
                                        8,&GlobalTextBlockStart,
                                        &GlobalTextBlockEnd,KeyString);
                             break;
                          }

                          TextBoxTABKey(GlobalBoxHeadHandle,&NewHBox,
                              GlobalTextPosition,&GlobalTextPosition,
                              &GlobalTextBlockStart,&GlobalTextBlockEnd);
                          break;
                     case SHIFT_TAB:
                          TextBoxShiftTabKey(GlobalBoxHeadHandle,&NewHBox,
                              GlobalTextPosition,&GlobalTextPosition,
                              &GlobalTextBlockStart,&GlobalTextBlockEnd);
                          break;
                     default:
                          if (Param1<32||Param1>255)
                             break;

                          KeyString[0]=Param1;
                          if (TextIsOverwrite())
                             TextBoxOverwriteKey(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&GlobalTextPosition,
                                        Param2,&GlobalTextBlockStart,
                                        &GlobalTextBlockEnd,KeyString);
                          else
                             TextBoxKey(GlobalBoxHeadHandle,&NewHBox,GlobalTextPosition,&GlobalTextPosition,
                                        Param2,&GlobalTextBlockStart,
                                        &GlobalTextBlockEnd,KeyString);
                          break;
                   }     // switch Param1
                } // if box is editable
                else
                   if (BoxIsPictureBox(GlobalBoxHeadHandle))
                      switch (MAKELO(Param1))
                      {
                        case DEL:
                     // if (BoxIsPictureBox(GlobalBoxHeadHandle))   ByHance::repeat
                          UndoInsertImageDelete(        // ByHance,96,1.31
                             PictureBoxGetPictureFileName(GlobalBoxHeadHandle));
                          PictureBoxClearImage(GlobalBoxHeadHandle);
                          BoxDrawBorder(GlobalBoxHeadHandle,0);
                          break;
                      }
                break;
           case IDX_SELECTBOX:     // do sth. with box
                switch (MAKELO(Param1))        // edit key
                {
                  case DEL:
                       if (GlobalGroupGetSign()==0)
                       {
                          Result=MessageBox(GetTitleString(WARNINGINFORM),
                                   "注意: 版框删除操作将无法复原!\n"
                                   "真的要删除选中的版框及内容?",
                                   2,Window);
                          if(Result==1)      // cancel
                             break;

                          NewHBox=ItemGetPrev(GlobalBoxHeadHandle);
                          if (!NewHBox)
                             NewHBox=ItemGetNext(GlobalBoxHeadHandle);
                          BoxDelete(GlobalBoxHeadHandle);
                          GlobalBoxHeadHandle=NewHBox;
                          BoxChangeAll(GlobalCurrentPage);
                          RedrawUserField();
                       }
                       else
                       {
                          Result=MessageBox(GetTitleString(WARNINGINFORM),
                                   "注意: 群组删除操作将无法复原!\n"
                                   "真的删除选中的所有版框及内容?",
                                   2,Window);
                          if(Result==1)      // cancel
                             break;
                          GroupDelete();
                          GlobalBoxHeadHandle=-1;
                          BoxChangeAll(GlobalCurrentPage);
                          RedrawUserField();
                       }
                       break;
                }
                break;
           default:
                break;
         }
         break;

    case MOUSELEFTDOUBLE:
         switch (GlobalBoxTool)
         {
           case IDX_INPUTBOX:
           case IDX_SELECTBOX:
                                       // Select context in box
                Param1=WindowToUserWindow(Window,Param1);
                if(fEditor)
                   Result=0;            // can not tell information
                else
                   Result=BoxSelect(GlobalCurrentPage,
                       (short)MAKEHI(Param1),(short)MAKELO(Param1));

                if(Result>0)    // has selected text or picture or table form
                   MessageGo(Window,MENUCOMMAND,MENU_BOX,0L);
                break;
           case IDX_PLGBOX:     // enclose this polygon
                if (PolygonNumber>3)
                {
                   // PictureBoxs MidtagBox;
                   PictureBoxs *MidBox;
                   int *MidPointer;

                   // MidBox=&MidtagBox;
                   MidBox=(PictureBoxs *)&TmpBuf;
                   memset(MidBox,0,sizeof(PictureBoxs));

                   PolygonNumber--;
                   PolygonGetMinRectangle(PolygonNumber,PolygonEdges,
                                          &Left,&Top,&Right,&Bottom);
                   PictureBoxSetBoxType(MidBox,POLYGONPICTUREBOX);
                   /*
                   PictureBoxSetBoxLeft(MidBox,WindowXToUserX(Left
                                        -GlobalPageHStart/GlobalPageScale));
                   PictureBoxSetBoxTop(MidBox,WindowYToUserY(Top
                                       -GlobalPageVStart/GlobalPageScale));
                   PictureBoxSetBoxWidth(MidBox,WindowXToUserX(Right
                                         -GlobalPageHStart/GlobalPageScale)
                                         -PictureBoxGetBoxLeft(MidBox));
                   PictureBoxSetBoxHeight(MidBox,WindowYToUserY(Bottom
                                          -GlobalPageHStart/GlobalPageScale)
                                          -PictureBoxGetBoxTop(MidBox));
                                          */
                   PictureBoxSetBoxLeft(MidBox,WindowXToUserX(Left
                                        -myUserXToWindowX(GlobalPageHStart)));
                   PictureBoxSetBoxTop(MidBox,WindowYToUserY(Top
                                        -myUserYToWindowY(GlobalPageVStart)));
                   PictureBoxSetBoxWidth(MidBox,WindowXToUserX(Right
                                        -myUserXToWindowX(GlobalPageHStart))
                                         -PictureBoxGetBoxLeft(MidBox));
                   PictureBoxSetBoxHeight(MidBox,WindowYToUserY(Bottom
                                        -myUserYToWindowY(GlobalPageVStart))
                                          -PictureBoxGetBoxTop(MidBox));

                   MidBox->BorderPolygon=HandleAlloc( sizeof(int)
                               +2*sizeof(ORDINATETYPE)*MAXPOLYGONNUMBER, 0);
                   if (MidBox->BorderPolygon==0)
                      break;
                   MidPointer=HandleLock(MidBox->BorderPolygon);
                   if (MidPointer==NULL)
                      break;
                   *MidPointer=PolygonNumber;

                   for (i=0;i<PolygonNumber;i++)
                   {
                       //PolygonEdges[2*i]-=GlobalPageHStart/GlobalPageScale;
                       PolygonEdges[2*i] -= myUserXToWindowX(GlobalPageHStart);
                       PolygonEdges[2*i]=WindowXToUserX(PolygonEdges[2*i]);//-PictureBoxGetBoxLeft(MidBox);
                       //PolygonEdges[2*i+1]-=GlobalPageVStart/GlobalPageScale;
                       PolygonEdges[2*i+1] -= myUserYToWindowY(GlobalPageVStart);
                       PolygonEdges[2*i+1]=WindowYToUserY(PolygonEdges[2*i+1]);//-PictureBoxGetBoxTop(MidBox);
                   }
                   memcpy(MidPointer+1,PolygonEdges,2*sizeof(ORDINATETYPE)*PolygonNumber);
                   HandleUnlock(MidBox->BorderPolygon);

                   CreatItem=PictureBoxInsert(MidBox,GlobalCurrentPage);
                   GlobalBoxHeadHandle=CreatItem;
                   BoxChange(CreatItem,GlobalCurrentPage);
                   RedrawUserField();
                   MessageGo(Window,MENUCOMMAND,MENU_TOOLSELECT,0);

                   CreatItem=0;
                   PolygonNumber=0;
                }
                break;
         }
         break;
    case MOUSERIGHTDOWN:
         switch (GlobalBoxTool)
         {
           case IDX_PLGBOX:  // cancel current opertion
                // ByHance, 95,12.6   clear old polygon,
                if(PolygonNumber<=0) break;
                {
                   int SaveColor;
         #ifdef __TURBOC__
                   struct linesettingstype SaveLineStyle;
                   getlinesettings(&SaveLineStyle);
                   setlinestyle(4,0x5555,1);
         #else
                   unsigned old_style=getlinestyle();
                   setlinestyle(0x5555);
         #endif

                   Param1=WindowToUserWindow(Window,Param1);
                   SaveColor=getcolor();
                   MouseHidden();
                   WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
                   setviewport(Left,Top,Right,Bottom,1);
                   setcolor(EGA_WHITE);
                   setwritemode(XOR_PUT);
                    // clear the last line
                   /*
                   line(PolygonEdges[2*PolygonNumber-2]-GlobalPageHStart/GlobalPageScale,
                        PolygonEdges[2*PolygonNumber-1]-GlobalPageVStart/GlobalPageScale,
                        PolygonEdges[2*PolygonNumber]-GlobalPageHStart/GlobalPageScale,
                        PolygonEdges[2*PolygonNumber+1]-GlobalPageVStart/GlobalPageScale);
                        */
                   line(PolygonEdges[2*PolygonNumber-2]-myUserXToWindowX(GlobalPageHStart),
                        PolygonEdges[2*PolygonNumber-1]-myUserYToWindowY(GlobalPageVStart),
                        PolygonEdges[2*PolygonNumber]-myUserXToWindowX(GlobalPageHStart),
                        PolygonEdges[2*PolygonNumber+1]-myUserYToWindowY(GlobalPageVStart));

       #ifdef __TURBOC__
                   setlinestyle(SaveLineStyle.linestyle,
                                SaveLineStyle.upattern,
                                SaveLineStyle.thickness);
       #else
                   setlinestyle(old_style);
       #endif
                      // clear first line
                   for(i=0;i<PolygonNumber-1;i++)
                   {
                     /*
                     line(PolygonEdges[2*i+2]-GlobalPageHStart/GlobalPageScale,
                          PolygonEdges[2*i+3]-GlobalPageVStart/GlobalPageScale,
                          PolygonEdges[2*i]-GlobalPageHStart/GlobalPageScale,
                          PolygonEdges[2*i+1]-GlobalPageVStart/GlobalPageScale);
                     */
                     line(PolygonEdges[2*i+2]-myUserXToWindowX(GlobalPageHStart),
                          PolygonEdges[2*i+3]-myUserYToWindowY(GlobalPageVStart),
                          PolygonEdges[2*i]-myUserXToWindowX(GlobalPageHStart),
                          PolygonEdges[2*i+1]-myUserYToWindowY(GlobalPageVStart));
                   }

                   setwritemode(COPY_PUT);
                   setcolor(SaveColor);
                   MouseShow();
                }

                PolygonNumber=0;
                MouseSetGraph(FINGERMOUSE2);
                break;
           case IDX_LINK:     // cancel current opertion
                LinkSign=0;
                MouseSetGraph(LINKMOUSE);
                break;
           case IDX_ZOOM:     // zoom in ( 2 times )
                if (GlobalPageScale<MAXPAGESCALE)
                {
                   MenuScaleZoomChange(1);
                   //MovePageToCenter(Window,GlobalCurrentPage);  // ByHance
                   ZoomPageAtPoint(Window,Param1,0);
                   RedrawUserField();
                }
                else
                   Alarm();
                break;                 // Restore View Scale
         }
         break;
    case MOUSELEFTDOWN:
         LastParam1=Param1;      // 1st mouse(X,Y)
         if (GlobalBoxTool!=IDX_PLGBOX && GlobalBoxTool!=IDX_ROTATE)
            SaveUndoNumber=UndoOperateSum;

         fInZoom=0;
         switch (GlobalBoxTool)
         {
           case IDX_INPUTBOX:     // Select context in box
                //X =(GlobalTextBlockEnd > GlobalTextBlockStart); // ByDg

                Param1=WindowToUserWindow(Window,Param1);
                if(fEditor)
                   Result=1;    // always get focus
                else
                   Result=BoxSelect(GlobalCurrentPage,
                       (short)MAKEHI(Param1),(short)MAKELO(Param1));

                if(Result>0)
                {            // has selected text or picture or table form
                   if (BoxCanEditable(GlobalBoxHeadHandle))
                      BoxGetCursor(GlobalBoxHeadHandle,(short)MAKEHI(Param1),
                                (short)MAKELO(Param1),&GlobalTextPosition,
                                   &GlobalTextBlockStart,&GlobalTextBlockEnd);
                }

                //if (BoxIsTableBox(GlobalBoxHeadHandle) && X) // ByDg
                  //   RedrawUserField();
                break;
           case IDX_SELECTBOX:
                   // Select Box, move will be done in drop, or undefine group
                Param1=WindowToUserWindow(Window,Param1);
                if (GlobalGroupGetSign())          // has defined group
                {
                   if (MousePointInGroup(WindowXToUserX((short)MAKEHI(Param1)),
                                         WindowYToUserY((short)MAKELO(Param1))))
                   {       // this group now is selected
                      MoveSign=GroupSelectSign;             // =4
                      break;
                   }
                   else
                   {           // undefine the old group
                      GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                      GlobalUnGroup();
                   }
                }

                //--- now, start to select new group, see also :    ----*/
                //---- MOUSELEFTUP, MOUSELEFTDROP in this procedure ----*/
                BoxSelect(GlobalCurrentPage,(short)MAKEHI(Param1),(short)MAKELO(Param1));
                FindXYInCell(GlobalBoxHeadHandle,WindowXToUserX((short)MAKEHI(Param1)),
                             WindowYToUserY((short)MAKELO(Param1)),&GlobalTableCell);
                break;
           case IDX_ROTATE:
                if (RotateSign==NoRotateSign)        // =0
                {                      // Select box and set rotate axis
                   Param1=WindowToUserWindow(Window,Param1);
                   if (GlobalBoxHeadHandle==0)
                   {
                      if (GlobalGroupGetSign())
                      {                // Do group rotate
                         if ((GlobalGroupGetRotateAxisX()!=0
                              ||GlobalGroupGetRotateAxisY()!=0)
                             &&GlobalGroupGetRotateAngle()==0)
                         {      // has selected rotate_method, only draw them
                            DrawRotateAxis(Window);
                            RotateSign=RotateEdgeSelectSign; // =2, Can't set different axis
                         }
                         else
                         {     // now, start to select rotate_method
                            GroupRotateInit(WindowXToUserX((short)MAKEHI(Param1)),
                                            WindowYToUserY((short)MAKELO(Param1)));
                            RotateSign=XYSelectSign;       // =1
                            DrawRotateAxis(Window);
                         }
                         break;
                      }
                        // else  // groupsign=0, and globalboxHead=0
                      goto lbl_rotate_err_ret;
                   }
                       // else /* GlobalBoxHeadHandle==0 */  ByHance,
                   NewHBox=GlobalBoxHeadHandle;
                   MidBox=HandleLock(ItemGetHandle(NewHBox));
                   if (BoxIsLocked(MidBox)
                   || TextBoxGetBoxType(MidBox)==TABLEBOX   // ByHance, 96,3.6
                   ) {
                      HandleUnlock(ItemGetHandle(NewHBox));
                 lbl_rotate_err_ret:
                      Alarm();
                      break;
                   }

                   if ((TextBoxGetRotateAxisX(MidBox)!=0
                        ||TextBoxGetRotateAxisY(MidBox)!=0)
                       &&TextBoxGetRotateAngle(MidBox)==0)
                   {
                      DrawRotateAxis(Window);
                      RotateSign=RotateEdgeSelectSign;    // =2, Can't set different axis
                   }
                   else
                   {                   // Set rotate axis (x,y)
                      BoxGetRect(GlobalBoxHeadHandle,&Left,&Top,&Right,&Bottom);
                      // Right=Left;  Bottom=Top;
                      SaveUndoNumber=UndoOperateSum;
                      UndoInsertBoxRotate(Left,Top,
                             TextBoxGetRotateAngle(MidBox),
                             TextBoxGetRotateAxisX(MidBox),
                             TextBoxGetRotateAxisY(MidBox));
                      if (TextBoxGetRotateAxisX(MidBox)==0&&
                          TextBoxGetRotateAxisY(MidBox)==0&&
                          TextBoxGetRotateAngle(MidBox)==0
                          )
                         Angle=0;
                      else
                         Angle=ConvertRotateAngle(TextBoxGetRotateAngle(MidBox),
                                        TextBoxGetRotateAxisX(MidBox)+Left,
                                        TextBoxGetRotateAxisY(MidBox)+Top,
                                        WindowXToUserX((short)MAKEHI(Param1)),
                                        WindowYToUserY((short)MAKELO(Param1)),
                                        &Left,&Top);

                      TextBoxSetBoxLeft(MidBox,Left);
                      TextBoxSetBoxTop(MidBox,Top);
                      TextBoxSetRotateAngle(MidBox,Angle);
                      TextBoxSetRotateAxisX(MidBox,WindowXToUserX((short)MAKEHI(Param1))-Left);
                      TextBoxSetRotateAxisY(MidBox,WindowYToUserY((short)MAKELO(Param1))-Top);
                      DrawRotateAxis(Window);
                      RotateSign=XYSelectSign;           // =1
                   }
                   HandleUnlock(ItemGetHandle(NewHBox));
                }
                else /* RotateSign!=NoRotateSign  */
                {
                   if (RotateSign==XYSelectSign)         // =1, has selected (X,Y)
                      RotateSign=RotateEdgeSelectSign;   // =2,Can rotate now
                }
                break;
           case IDX_TABLE:
           case IDX_TEXTBOX:
           case IDX_RECBOX:
           case IDX_CIRBOX:
           case IDX_ELIBOX:
           case IDX_LINE:
                Param1=WindowToUserWindow(Window,Param1);
                BoxSelect(GlobalCurrentPage,(short)MAKEHI(Param1),(short)MAKELO(Param1));
                break;
           case IDX_PLGBOX:
                if (PolygonNumber<MAXPOLYGONNUMBER&&GlobalCurrentPage>0)
                {                      // ???
                   Param1=WindowToUserWindow(Window,Param1);
                   if (PolygonNumber>0)
                   {
                      setcolor(EGA_BLACK);
                      MouseHidden();
                      WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
                      setviewport(Left,Top,Right,Bottom,1);
                      /*
                      line(PolygonEdges[2*PolygonNumber-2]-GlobalPageHStart/GlobalPageScale,
                           PolygonEdges[2*PolygonNumber-1]-GlobalPageVStart/GlobalPageScale,
                           (short)MAKEHI(Param1),(short)MAKELO(Param1));
                      */
                      line(PolygonEdges[2*PolygonNumber-2]-myUserXToWindowX(GlobalPageHStart),
                           PolygonEdges[2*PolygonNumber-1]-myUserYToWindowY(GlobalPageVStart),
                           (short)MAKEHI(Param1),(short)MAKELO(Param1));

                      MouseShow();
                   }
                   //PolygonEdges[2*PolygonNumber]=PolygonEdges[2*PolygonNumber+2]=(short)MAKEHI(Param1)+GlobalPageHStart/GlobalPageScale;
                   PolygonEdges[2*PolygonNumber]=
                   PolygonEdges[2*PolygonNumber+2]=
                   (short)MAKEHI(Param1)+myUserXToWindowX(GlobalPageHStart);

                   //PolygonEdges[2*PolygonNumber+1]=PolygonEdges[2*PolygonNumber+3]=(short)MAKELO(Param1)+GlobalPageVStart/GlobalPageScale;
                   PolygonEdges[2*PolygonNumber+1]=
                   PolygonEdges[2*PolygonNumber+3]=
                   (short)MAKELO(Param1)+myUserYToWindowY(GlobalPageVStart);

                   PolygonNumber++;
                }
                else
                {
                   Alarm();
                }
                break;
           case IDX_LINK:
                if (LinkSign==0)
                {
                   Param1=WindowToUserWindow(Window,Param1);
                   LinkPrevX=WindowXToUserX((short)MAKEHI(Param1));
                   LinkPrevY=WindowYToUserY((short)MAKELO(Param1));
                   BoxSelect(GlobalCurrentPage,(short)MAKEHI(Param1),(short)MAKELO(Param1));
                   if (GlobalBoxHeadHandle)
                   {
                      MidBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                      if (TextBoxGetBoxType(MidBox)==TEXTBOX&&
                          TextBoxGetNextLinkBox(MidBox)==0)
                      {
                         LinkSign=1;
                         LinkPrevBox=GlobalBoxHeadHandle;
                         MouseSetGraph(CUPMOUSE1);          // cup with word
                      }
                      else
                         Alarm();
                      HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                   }
                   break;
                }
                    // else  LinkSign=1:: has selected PrevLinkBox
                Param1=WindowToUserWindow(Window,Param1);
                BoxSelect(GlobalCurrentPage,(short)MAKEHI(Param1),(short)MAKELO(Param1));
                if (GlobalBoxHeadHandle)
                {
                   MidBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                   if (MidBox==NULL)
                      break;
                   if ((TextBoxGetBoxType(MidBox)!=TEXTBOX)||
                       (TextBoxGetPrevLinkBox(MidBox)!=0)||
                      // (TextBoxGetTextLength(MidBox)>0)||    //ByHance
                       (TextBoxGetTextLength(MidBox)>1)||
                       (TextBoxLinkCycle(LinkPrevBox,GlobalBoxHeadHandle)))
                   {
                      HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                      Alarm();
                      break;
                   }

                 //------ ByHance, 95,12.17 ---------
                   DrawLinkedBox();          // clear old link line
                   MidBox1=HandleLock(ItemGetHandle(LinkPrevBox));
                   if(TextBoxIsFormatFull(MidBox1))
                   {
                      TextBoxDrawTail(LinkPrevBox,1);      // clear old tail
                   }
                   HandleUnlock(ItemGetHandle(LinkPrevBox));
                 //------end, ByHance, 95,12.17 ---------

                   if (TextBoxGetTextHandle(MidBox)>0)
                   {
                      HandleFree(TextBoxGetTextHandle(MidBox));
                      TextBoxSetBlockLength(MidBox,0);
                   }
                   TextBoxSetPrevLinkBox(MidBox,LinkPrevBox);
                   HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                   MidBox=HandleLock(ItemGetHandle(LinkPrevBox));
                   if (MidBox==NULL)
                      break;

                   TextBoxSetNextLinkBox(MidBox,GlobalBoxHeadHandle);
                   HandleUnlock(ItemGetHandle(LinkPrevBox));
                   LinkPrevBox=GetFirstLinkBox(LinkPrevBox);
                   SetAllLinkBoxTextHandle(LinkPrevBox);
                   if (LinkPrevBox)
                   {
                      FormatAll(LinkPrevBox);
                      TextBoxRedraw(LinkPrevBox,0,30000,FALSE);
                      FileSetModified();
                      DrawLinkedBox(); // RedrawUserField(); ByHance, 95,12.17
                   }
                   MouseSetGraph(LINKMOUSE);
                   LinkSign=0;
                }
                break;
           case IDX_UNLINK:
                Param1=WindowToUserWindow(Window,Param1);
                BoxSelect(GlobalCurrentPage,(short)MAKEHI(Param1),(short)MAKELO(Param1));
                if (GlobalBoxHeadHandle>0)
                {
                   MidBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                   if (TextBoxGetBoxType(MidBox)!=TEXTBOX||
                       TextBoxGetPrevLinkBox(MidBox)==0)
                   {
                      HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                      Alarm();
                      break;
                   }
                   else
                   {
                      HBOX MidHBox1,MidHBox2,SaveHBox;

                 //------ ByHance, 95,12.17 ---------
                  //    DrawLinkedBox();          // clear old link line
                 //------end, ByHance, 95,12.17 ---------

                      SaveHBox=TextBoxGetPrevLinkBox(MidBox);
                      SetAllUnLinkBoxTextHandle(GlobalBoxHeadHandle);
                      MidHBox1=TextBoxGetPrevLinkBox(MidBox);
                      MidHBox2=GlobalBoxHeadHandle;
                      HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                      while (MidHBox2)
                      {
                        MidBox1=HandleLock(ItemGetHandle(MidHBox1));
                        MidBox2=HandleLock(ItemGetHandle(MidHBox2));
                        if (MidBox1==NULL||MidBox2==NULL)
                           break;
                        TextBoxSetNextLinkBox(MidBox1,0);
                        TextBoxSetFormatNotFull(MidBox1);
                      /*------------
                        if(TextBoxIsFormatFull(MidBox2))  //ByHance,95,12.17
                            TextBoxDrawTail(MidHBox2,1);  // clear old tail
                       ------------*/
                        TextBoxSetFormatNotFull(MidBox2);
                        TextBoxSetPrevLinkBox(MidBox2,0);

                        if (!TextBoxGetPrevLinkBox(MidBox1))
                        {
                           InitRL(MidBox1);
                           KeyString[0]=' ';
                           TextBoxInsertString(MidHBox1,0,KeyString,1);
                           TextBoxDeleteString(MidHBox1,0,1);
                           FormatAll(MidHBox1);
                        }
                        HandleUnlock(ItemGetHandle(MidHBox1));
                        MidHBox1=NewHBox=MidHBox2;
                        MidHBox2=TextBoxGetNextLinkBox(MidBox2);
                        HandleUnlock(ItemGetHandle(NewHBox));
                      }

                      FileSetModified();

                      if (SaveHBox)
                      {
                         SaveHBox=GetFirstLinkBox(SaveHBox);
                         if (SaveHBox)
                            FormatAll(SaveHBox);
                      }
                      RedrawUserField();
                   }
                }
                break;
         }
         break;
    case MOUSELEFTUP:
         if (IsInGlobalBrowser())
         {
            ClearGlobalBrowser();
            if(GlobalNotDisplay) {
               GlobalNotDisplay=0;               // ByHance, 95,12.8
               RedrawUserField();
            }

                // ByHance, 96,3.23
            MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
            PageWidth=PageGetPageWidth(MidPage);
            PageHeight=PageGetPageHeight(MidPage);
            HandleUnlock(ItemGetHandle(GlobalCurrentPage));
            SetHScrollLeft(Window,PageWidth);
            SetVScrollTop(Window,PageHeight);

            UserMouseMove(Window,Message,Param1,Param2);
            break;
         }
         //WindowDefaultProcedure(Window,Message,Param1,Param2);
         switch (GlobalBoxTool)
         {
           case IDX_ZOOM:               // ByHance, 97,5.4
                if (GlobalGroupGetSign())          // has defined group
                {
                       // undefine the old group
                   GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);

                   Result=ZoomPageByRect(Window,
                               GlobalGroupGetLeft(),
                               GlobalGroupGetTop(),
                               GlobalGroupGetRight(),
                               GlobalGroupGetBottom());

                   GlobalUnGroup();
                   MoveSign=NoMoveSign;        // =0
                   if(Result)
                   {
                       if(Result==1)  // GroupBox is too small
                          goto lbl_zoom_out;
                       RedrawUserField();
                   }
                   break;
                }

             lbl_zoom_out:
                if (GlobalPageScale>MINPAGESCALE)   /*-- zoom out --*/
                {
                   MenuScaleZoomChange(0);
                   ZoomPageAtPoint(Window,Param1,1);
                   RedrawUserField();
                }
                else
                   Alarm();
                break;
           case IDX_INPUTBOX:
                if (GlobalTextBlockStart<GlobalTextBlockEnd)
                {
                   //if(BoxIsTableBox(GlobalBoxHeadHandle)) // ByHance, 96,4.8
                   //   GlobalTextBlockStart=GlobalTextBlockEnd=-1;
                   //else
                      UndoInsertCursorDefineBlock(-1,-1);
                   UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
                }
                break;
           case IDX_SELECTBOX:
                if (GlobalGroupGetSign())
                {
                   if (MoveSign==GroupMovingSign)    // =3, Move done
                   {
                      BoxChangeAll(GlobalCurrentPage);
                      FileSetModified();
                      RedrawUserField();
                   }
                   else
                      if (MoveSign==GroupDefineSign)  // =5, Define done
                      {
                         GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                         Result=Group(GlobalCurrentPage,
                               GlobalGroupGetLeft(),
                               GlobalGroupGetTop(),
                               GlobalGroupGetWidth(),
                               GlobalGroupGetHeight());
                         if(Result>0)     // ByHance:: if selected_box_num>0
                           GroupDrawAllBorder(DRAWXORBORDER);
                         else GlobalUnGroup();
                      }
                   MoveSign=NoMoveSign;        // =0
                   break;
                }

                if (GlobalBoxHeadHandle>0 && MoveSign)
                {
                   if(CellisMoving)     // ByHance, 96,4.3
                   {
                       ReFormatTableText(GlobalBoxHeadHandle,TRUE);
                       CellisMoving=FALSE;
                   }
                   BoxChangeAll(GlobalCurrentPage);
                   FileSetModified();
                   MoveSign=NoMoveSign;
                   RedrawUserField();
                   UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
                }
                break;
           case IDX_ROTATE:
                if (RotateSign>1)
                {
                   PictureBoxs *PictureBox;
                   ImageDescribes *TiffPresent;

                   RotateSign=NoRotateSign;           // =0
                   PictureBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                   if(PictureBox==NULL)
                         break;
                   Result=PictureBoxGetBoxType(PictureBox);
                   if (Result>=RECTANGLEPICTUREBOX && Result<=POLYGONPICTUREBOX)
                   {
                      TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
                      Result=PictureBoxGetRotateAngle(PictureBox);
                      if(TiffPresent->ImageRotateAngle!=Result)
                      {
                          TiffPresent->ImageRotateAngle=Result;
                          ImageChangeNewParameter(GlobalBoxHeadHandle);
                      }
                   }
                   HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                   goto finish_box_change_opertion;
                }
                break;
           case IDX_TEXTBOX:
           case IDX_RECBOX:
           case IDX_CIRBOX:
           case IDX_ELIBOX:
           case IDX_LINE:
           case IDX_TABLE:
                if (CreatItem)
                {
                   char DirectSign=0;

                   MidBox=HandleLock(ItemGetHandle(CreatItem));
                   if (MidBox==NULL)
                      break;
                   if (TextBoxGetBoxWidth(MidBox)<0)
                   {
                      TextBoxSetBoxLeft(MidBox,TextBoxGetBoxLeft(MidBox)
                                         +TextBoxGetBoxWidth(MidBox));
                      TextBoxSetBoxWidth(MidBox,-TextBoxGetBoxWidth(MidBox));
                      DirectSign|=1;
                   }
                   if (TextBoxGetBoxHeight(MidBox)<0)
                   {
                      TextBoxSetBoxTop(MidBox,TextBoxGetBoxTop(MidBox)
                                        +TextBoxGetBoxHeight(MidBox));
                      TextBoxSetBoxHeight(MidBox,-TextBoxGetBoxHeight(MidBox));
                      DirectSign|=2;
                   }

                   if (GlobalBoxTool!=IDX_LINE)
                   {
                      if (TextBoxGetBoxWidth(MidBox)<=120
                          ||TextBoxGetBoxHeight(MidBox)<=120)
                      {                 // ByHance, for too small box
                      lbl_err_creatbox:
                          HandleUnlock(ItemGetHandle(CreatItem));
                          BoxDrawBorder(CreatItem,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                          BoxDelete(CreatItem);
                          CreatItem=0;
                          // RedrawUserField();
                          break;
                      }

                      if(GlobalBoxTool==IDX_TABLE)
                      {
                         int boxw,boxh;
                         boxw=TextBoxGetBoxWidth(MidBox);
                         boxh=TextBoxGetBoxHeight(MidBox);
                         if (!MakeDialogBox(Window,TableLineColumnDialog))
                         {
                            MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
                            PageHeight=PageGetPageHeight(MidPage);
                            Left=PageGetMarginLeft(MidPage);
                            PageWidth=PageGetPageWidth(MidPage)
                                     -Left-PageGetMarginRight(MidPage);
                            HandleUnlock(ItemGetHandle(GlobalCurrentPage));

                            if (TmpFormBox.numLines<1) TmpFormBox.numLines=1;
                            if (TmpFormBox.numCols<1) TmpFormBox.numCols=1;

                            //j=(GlobalFontSize+4*DEFAULTBOXTEXTDISTANT+4);
                            j=(GlobalFontSize+DEFAULTBOXTEXTDISTANT);

                            i=TextBoxGetBoxHeight(MidBox)/TmpFormBox.numLines;
                            if (i<j)
                            {
                              Result=MessageBox(GetTitleString(WARNINGINFORM),
                                          "此表格行数太多,表元高度太小\n"
                                          "自动调整表格行数,继续创建表\n"
                                          "格吗?",
                                           2,1);
                              if(Result)
                                 goto lbl_err_creatbox;
                              TmpFormBox.numLines=TextBoxGetBoxHeight(MidBox)/(j);
                              if (TmpFormBox.numLines<1)
                                {
                                    TmpFormBox.numLines=1;
                                    TextBoxSetBoxHeight(MidBox,j);
                                }
                            }

                            j=(GlobalFontSize+4*DEFAULTBOXTEXTDISTANT+4);

                            i=TextBoxGetBoxWidth(MidBox)/TmpFormBox.numCols;
                            if (i<j)
                            {
                              Result=MessageBox(GetTitleString(WARNINGINFORM),
                                          "此表格列数太多,表元宽度太窄\n"
                                          "自动调整表格列数,继续创建表\n"
                                          "格吗?",
                                           2,1);
                              if(Result)
                                 goto lbl_err_creatbox;
                              TmpFormBox.numCols=TextBoxGetBoxWidth(MidBox)/j;
                              if (TmpFormBox.numCols<1)
                                {
                                    TmpFormBox.numCols=1;
                                    TextBoxSetBoxWidth(MidBox,j);
                                }
                            }

                            j=0;
                            if (TmpFormBox.numLines>=MAXLINENUMBER
                            || TmpFormBox.numCols>=MAXCLOUMNNUMBER)
                            {
                                Result=MessageBox(GetTitleString(WARNINGINFORM),
                                          "行数或列数超过了60,超出\n"
                                          "部分将被自动截掉,  继续\n"
                                          "创建表格吗?",
                                           2,1);
                                if(Result)
                                    goto lbl_err_creatbox;
                                if (TmpFormBox.numLines>=MAXLINENUMBER)
                                    TmpFormBox.numLines=MAXLINENUMBER-1;
                                if (TmpFormBox.numCols>=MAXCLOUMNNUMBER)
                                    TmpFormBox.numCols=MAXCLOUMNNUMBER-1;
                            }
                            /*
                            if (TmpFormBox.numLines*TmpFormBox.numCols>=MAXREGIONNUM)
                            {
                                if(!j)
                                {
                                    Result=MessageBox(GetTitleString(WARNINGINFORM),
                                          "行数与列数的乘积超过了120,\n"
                                          "超出部分将被自动截掉, 继续\n"
                                          "创建表格吗?",
                                           2,1);
                                    if(Result)
                                        goto lbl_err_creatbox;
                                }

                                i=(MAXREGIONNUM-1)/TmpFormBox.numCols;
                                if(i==0)
                                {
                                   i=1;
                                   if(TmpFormBox.numCols>=MAXREGIONNUM)
                                       TmpFormBox.numCols=MAXREGIONNUM-1;
                                }
                                TmpFormBox.numLines=i;

                            }*/

                            TableBoxSetnumLines((PFormBoxs)MidBox,TmpFormBox.numLines);
                            TableBoxSetnumCols((PFormBoxs)MidBox,TmpFormBox.numCols);
                            FBInitCells(CreatItem,TmpFormBox.numLines,
                                        TmpFormBox.numCols);

                            //Adjust
                            // delete by zjh 1996.6
                            if (GetTableHeadOption())
                            {
                            FBPlusVertLine(CreatItem,0,TextBoxGetBoxLeft(MidBox)-Left);
                            TextBoxSetBoxLeft(MidBox,Left);
                            TextBoxSetBoxWidth(MidBox,PageWidth);
                            }
                            else
                            {
                            FBPlusVertLine(CreatItem,0,DEFAULTCHARSIZE);
                            TextBoxSetBoxWidth(MidBox,TextBoxGetBoxWidth(MidBox)+DEFAULTCHARSIZE*2);
                            }

                            i=DEFAULTCHARSIZE;

                            //if (GetTableHeadOption())
                            //    i+=3*GlobalFontSize/2+2*DEFAULTBOXTEXTDISTANT;
                            FBPlusHoriLine(CreatItem,0,DEFAULTCHARSIZE/2);
                            TextBoxSetBoxHeight(MidBox,
                                   TextBoxGetBoxHeight(MidBox)+i);

                            ReFormatTableText(CreatItem,TRUE);
                            GlobalTableCell=0;
                         }
                         else
                            goto lbl_err_creatbox;
                      }
                      else
                      if (GlobalBoxTool==IDX_CIRBOX)
                      {
                         i=(PictureBoxGetBoxWidth(MidBox))/4;
                         j=(PictureBoxGetBoxHeight(MidBox))/4;
                         if (i>j)  i=j;
                         PictureBoxSetCornerRadius((PictureBoxs *)MidBox,i);
                      }
                   }
                   else         // it is IDX_LINE
                   {
                      if (TextBoxGetBoxWidth(MidBox)>SLANTLINEWIDTH
                          &&TextBoxGetBoxHeight(MidBox)>SLANTLINEWIDTH)
                      {                 // it is slant line
                         Result=sqrt((double)TextBoxGetBoxHeight(MidBox)
                                          *(double)TextBoxGetBoxHeight(MidBox)
                                         +(double)TextBoxGetBoxWidth(MidBox)
                                          *(double)TextBoxGetBoxWidth(MidBox));
                         Angle=180.*atan((double)(TextBoxGetBoxHeight(MidBox))
                               /(double)(TextBoxGetBoxWidth(MidBox)))/PI;
                         if ((DirectSign==1)||(DirectSign==2))
                         {             // only Width<0 or only Height<0 :
                                       //             / Start (End)
                                       //           /
                                       //     End / (Start)
                            TextBoxSetBoxTop(MidBox,TextBoxGetBoxTop(MidBox)
                                             +TextBoxGetBoxHeight(MidBox));
                            Angle=-Angle;
                         }
                         if(Angle<0) Angle+=360;
                                       // else
                                       // Height<0 And Width<0
                                       // or Height>0 And Width>0
                                       //     End \ (Start)
                                       //           \
                                       //             \ Start (End)

                         TextBoxSetRotateAxisX(MidBox,SCALEMETER/(72*2));
                         TextBoxSetRotateAxisY(MidBox,SCALEMETER/(72*2));
                         TextBoxSetRotateAngle(MidBox,Angle);
                         TextBoxSetBoxWidth(MidBox,Result);
                      }
                      else
                      {
                         if (TextBoxGetBoxHeight(MidBox)>TextBoxGetBoxWidth(MidBox))
                         {              // it is vert line
                            TextBoxSetBoxWidth(MidBox,TextBoxGetBoxHeight(MidBox));
                            TextBoxSetRotateAngle(MidBox,90);
        //ByHance             TextBoxSetRotateAxisX(MidBox,TextBoxGetBoxWidth(MidBox)/2);
                            TextBoxSetRotateAxisX(MidBox,SCALEMETER/(72*2));
                            TextBoxSetRotateAxisY(MidBox,SCALEMETER/(72*2));
                         }
                      }
                      TextBoxSetBoxHeight(MidBox,1*SCALEMETER/72);
                   }

                   HandleUnlock(ItemGetHandle(CreatItem));
                   UndoInsertBoxCreat(CreatItem,GlobalBoxHeadHandle);

                   GlobalBoxHeadHandle=CreatItem;
                   CreatItem=0;
            finish_box_change_opertion:
                   BoxChange(GlobalBoxHeadHandle,GlobalCurrentPage);
                   FileSetModified();
                   RedrawUserField();
                   UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
                   MessageGo(Window,MENUCOMMAND,MENU_TOOLSELECT,0);
                }
                break;
           case IDX_LINK:
           case IDX_UNLINK:
                if (LinkSign==0)
                   UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
                break;                 // No action
         }
         break;
    case MOUSELEFTDOWN_SHIFT:
         if(fEditor) break;
         MessageGo(Window,MENUCOMMAND,MENU_TOOLSELECT,0);
         MessageGo(Window,MOUSEMOVE,Param1,Param2);
         break;
    case MOUSELEFTDOWN_CTRL:
         if(fEditor) break;
         MessageGo(Window,MENUCOMMAND,MENU_TOOLMOVE,0);
         MessageGo(Window,MOUSEMOVE,Param1,Param2);
         break;
    case MOUSELEFTDOWN_ALT:
         if (GlobalCurrentPage>0)
         {
            LastParam1=Param1;
            CloseTextCursor();
            SetGlobalBrowser();
            MouseSetGraph(BROWSEHANDMOUSE);     // ByHance
         }
         break;
    case MOUSELEFTDROP:
       #ifdef REGIST_VERSION
         if(!TimeCountArr[2] || !fRegist)           // for secret
            break;
         /*--- if have got logic file, and disk serial number ---*/
         Top=0x7&(MAKEHI(Param2));  Right=0x3f&(MAKELO(Param2));
         if(!Top) Top=3;
         Left=SerialTypeLen/Top;
         if(Left==SerialTypeLen) Left=(Right+Top)%SerialTypeLen;

         if(Right>=SerialTypeLen)
         {
            Right=SerialTypeLen-Right/3;
            if(Right<0 || Right==SerialTypeLen) Right=SerialTypeLen-1;
         }

         Top=MAKELONG( (serial[Left]-regist_str[Left])&0xff,
                        (regist_str[Right]-serial[Right])&0xff );
       #ifdef DEBUG
         if(Top!=0) { putch(7);    // for test
              printf("len=%d %d %d %x\n",SerialTypeLen,Left,Right,Top); }
       #endif

         LastParam1+=Top;
       #endif

         if (IsInGlobalBrowser())
         {
            #define BROWSERINCREASE 1

            //Result=0;
            GlobalNotDisplay=1;
            MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
            PageWidth=PageGetPageWidth(MidPage);
            PageHeight=PageGetPageHeight(MidPage);
            HandleUnlock(ItemGetHandle(GlobalCurrentPage));

            MidParam=WindowToUserWindow(Window,LastParam1);
            Left=(short)MAKEHI(MidParam);
            Top=(short)MAKELO(MidParam);
            MidParam=WindowToUserWindow(Window,Param2);
            Right=(short)MAKEHI(MidParam);
            Bottom=(short)MAKELO(MidParam);

            if (Left!=Right)
            {
               //Distance=GlobalPageHStart-BROWSERINCREASE*GlobalPageScale*(Right-Left);
               Distance=GlobalPageHStart-myWindowXToUserX(BROWSERINCREASE*(Right-Left));
               MessageInsert(Window,HHSCROLLMOVE,Distance,(PageWidth+PAGELEFTDISTANT));
            }
            if (Top!=Bottom)
            {
               //Distance=GlobalPageVStart-BROWSERINCREASE*GlobalPageScale*(Bottom-Top);
               Distance=GlobalPageVStart-myWindowYToUserY(BROWSERINCREASE*(Bottom-Top));

               MessageInsert(Window,VVSCROLLMOVE,Distance,
                         (PageHeight+PAGETOPDISTANT));
            }
         }
         else
         {                  // not in browse status
           //WindowDefaultProcedure(Window,Message,Param1,Param2);
           switch (GlobalBoxTool)
           {
             case IDX_INPUTBOX:      // select text block
                  if (GlobalBoxHeadHandle>0)
                     BoxSelectOrMove(GlobalBoxHeadHandle,Param1,Param2,
                       LastParam1,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                  break;
             case IDX_SELECTBOX:     // change box range, or move box position
                  NewHBox=GlobalBoxHeadHandle;
                  Result=0;
                  if (NewHBox>0)
                  {
                     if ((!BoxChangeBorder(NewHBox,
                                   WindowToUserWindow(Window,LastParam1),
                                   WindowToUserWindow(Window,Param2)))
                          &&MoveSign<2)
                     {
                        MidBox=HandleLock(ItemGetHandle(NewHBox));
                        if (MidBox==NULL)
                           break;
                        if (BoxIsLocked(MidBox))
                        {
                           HandleUnlock(ItemGetHandle(NewHBox));
                           break;
                        }

                        if (!MoveSign)
                           MoveSign=MovingSign;
                        else
                        {
                           BoxDrawBorder(NewHBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                           BoxMove(NewHBox,WindowXToUserX((short)MAKEHI(Param2))-WindowXToUserX((short)MAKEHI(LastParam1)),
                                   WindowYToUserY((short)MAKELO(Param2))-WindowYToUserY((short)MAKELO(LastParam1)));
                           BoxDrawBorder(NewHBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                        }
                        HandleUnlock(ItemGetHandle(NewHBox));
                        Result=1;
                     }
                     else {
                        Result=1;
                        MoveSign=ResizingSign;             // =2
                     }
                  }
                  else                 // Select Group
             case IDX_ZOOM:               // ByHance, 97,5.4
                  {
                     Result=0;
                     if (!MoveSign)
                     {                 // Start define group
                        GlobalGroupSetRotateAngle(0);
                        GlobalGroupSetRotateAxisX(0);
                        GlobalGroupSetRotateAxisY(0);
                        Param1=WindowToUserWindow(Window,Param1);
                        Param2=WindowToUserWindow(Window,Param2);
                        GlobalGroupSetLeft(WindowXToUserX((short)MAKEHI(Param1)));
                        GlobalGroupSetTop(WindowYToUserY((short)MAKELO(Param1)));
                        GlobalGroupSetWidth(WindowXToUserX((short)MAKEHI(Param2))-
                                            WindowXToUserX((short)MAKEHI(Param1)));
                        GlobalGroupSetHeight(WindowYToUserY((short)MAKELO(Param2))-
                                             WindowYToUserY((short)MAKELO(Param1)));
                        GlobalGroupSetPage(GlobalCurrentPage);
                        GlobalGroupSetSign();
                        GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                        MoveSign=GroupDefineSign;        // =5
                     }
                     else
                     if (MoveSign==GroupDefineSign)
                     {              // Defining group
                        GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                        Param1=WindowToUserWindow(Window,Param1);
                        Param2=WindowToUserWindow(Window,Param2);
                        GlobalGroupSetWidth(GlobalGroupGetWidth()+
                          WindowXToUserX((short)MAKEHI(Param2))-WindowXToUserX((short)MAKEHI(LastParam1)));
                        GlobalGroupSetHeight(GlobalGroupGetHeight()+
                          WindowYToUserY((short)MAKELO(Param2))-WindowYToUserY((short)MAKELO(LastParam1)));
                        GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                        Result=2;
                     }
                     else
                     if (MoveSign==GroupMovingSign||MoveSign==GroupSelectSign)
                     {           // Move group
                        GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                        GroupMove(WindowXToUserX((short)MAKEHI(Param2))
                                  -WindowXToUserX((short)MAKEHI(LastParam1)),
                                  WindowYToUserY((short)MAKELO(Param2))
                                  -WindowYToUserY((short)MAKELO(LastParam1)));
                        GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                        MoveSign=GroupMovingSign;
                        Result=1;
                     }
                  }

  #define WINDOWINCREASE 150

                  if(Result && GlobalCurrentPage>0) {    // if can scroll
                     MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
                     PageWidth=PageGetPageWidth(MidPage);
                     PageHeight=PageGetPageHeight(MidPage);
                     HandleUnlock(ItemGetHandle(GlobalCurrentPage));

                     if(Result==2) MidParam=Param2;   // already changed
                     else MidParam=WindowToUserWindow(Window,Param2);

                     WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
                     UserWindowMouseX=(short)MAKEHI(MidParam);
                     UserWindowMouseY=(short)MAKELO(MidParam);
                     if (UserWindowMouseX>=Right-Left)
                     {
                        /*
                        MessageInsert(Window,HHSCROLLMOVE,
                         (GlobalPageHStart+WINDOWINCREASE*GlobalPageScale),
                         (PageWidth+PAGELEFTDISTANT));
                        */
                        MessageInsert(Window,HHSCROLLMOVE,
                         (GlobalPageHStart+myWindowXToUserX(WINDOWINCREASE)),
                         (PageWidth+PAGELEFTDISTANT));
                        Param2=MAKELONG((short)MAKEHI(Param2)-WINDOWINCREASE,
                                        MAKELO(Param2));
                        MouseGetPosition(&X,&Y);
                        MouseMoveTo(X-WINDOWINCREASE,Y);
                     }
                     else
                     if (UserWindowMouseX<0)
                     {
                        /*
                        MessageInsert(Window,HHSCROLLMOVE,
                         (GlobalPageHStart-WINDOWINCREASE*GlobalPageScale),
                         (PageWidth+PAGELEFTDISTANT));
                         */
                        MessageInsert(Window,HHSCROLLMOVE,
                         (GlobalPageHStart-myWindowXToUserX(WINDOWINCREASE)),
                         (PageWidth+PAGELEFTDISTANT));
                        Param2=MAKELONG((short)MAKEHI(Param2)+WINDOWINCREASE,
                                        MAKELO(Param2));
                        MouseGetPosition(&X,&Y);
                        MouseMoveTo(X+WINDOWINCREASE,Y);
                     }

                     if (UserWindowMouseY>=Bottom-Top)
                     {
                        /*
                        MessageInsert(Window,VVSCROLLMOVE,
                         (GlobalPageVStart+WINDOWINCREASE*GlobalPageScale),
                         (PageHeight+PAGETOPDISTANT));
                         */
                        MessageInsert(Window,VVSCROLLMOVE,
                         (GlobalPageVStart+myWindowYToUserY(WINDOWINCREASE)),
                         (PageHeight+PAGETOPDISTANT));
                        Param2=MAKELONG(MAKEHI(Param2),
                                        (short)MAKELO(Param2)-WINDOWINCREASE);
                        MouseGetPosition(&X,&Y);
                        MouseMoveTo(X,Y-WINDOWINCREASE);
                     }
                     else
                     if (UserWindowMouseY<0)
                     {
                        /*
                        MessageInsert(Window,VVSCROLLMOVE,
                         (GlobalPageVStart-WINDOWINCREASE*GlobalPageScale),
                         (PageHeight+PAGETOPDISTANT));
                         */
                         MessageInsert(Window,VVSCROLLMOVE,
                         (GlobalPageVStart-myWindowYToUserY(WINDOWINCREASE)),
                         (PageHeight+PAGETOPDISTANT));

                        Param2=MAKELONG(MAKEHI(Param2),
                                        (short)MAKELO(Param2)+WINDOWINCREASE);
                        MouseGetPosition(&X,&Y);
                        MouseMoveTo(X,Y+WINDOWINCREASE);
                     }
                  }
                  break;
             case IDX_ROTATE:
                  if (RotateSign>=2)
                  {
                     if (GlobalGroupGetSign())
                     {
                        X=GlobalGroupGetRotateAxisX()+GlobalGroupGetLeft();
                        Y=GlobalGroupGetRotateAxisY()+GlobalGroupGetTop();
                        if (RotateSign==3)
                           MidParam=LastParam1;
                        else
                        {
                           MidParam=WindowToUserWindow(Window,Param1);
                           RotateSign=3;
                        }
                        Param2=WindowToUserWindow(Window,Param2);
                        Angle=TriPointToAngle(X,Y,
                                       WindowXToUserX((short)MAKEHI(MidParam)),
                                       WindowYToUserY((short)MAKELO(MidParam)),
                                       WindowXToUserX((short)MAKEHI(Param2)),
                                       WindowYToUserY((short)MAKELO(Param2)));
                        if (Angle)
                        {
                           if(Angle<0) Angle+=360;
                           else
                           if(Angle>=360) Angle-=360;

                           GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                           GroupRotate(Angle);
                           //GlobalGroupSetRotateAngle(GlobalGroupGetRotateAngle()
                           //                        +Angle);
                           GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                        }
                        break;
                     }    // group end

                     if ((NewHBox=GlobalBoxHeadHandle)!=0)
                     {
                        // Draw Rotate and set rotate angle
                        MidBox=HandleLock(ItemGetHandle(NewHBox));
                        if (MidBox==NULL)
                           break;

                        X=TextBoxGetRotateAxisX(MidBox)+TextBoxGetBoxLeft(MidBox);
                        Y=TextBoxGetRotateAxisY(MidBox)+TextBoxGetBoxTop(MidBox);
                        if (RotateSign==3)
                           MidParam=LastParam1;
                        else
                        {
                           MidParam=WindowToUserWindow(Window,Param1);
                           RotateSign=3;
                        }
                        Param2=WindowToUserWindow(Window,Param2);
                        Angle=TriPointToAngle(X,Y,
                                       WindowXToUserX((short)MAKEHI(MidParam)),
                                       WindowYToUserY((short)MAKELO(MidParam)),
                                       WindowXToUserX((short)MAKEHI(Param2)),
                                       WindowYToUserY((short)MAKELO(Param2)));
                        TextCursorRotateAngle=TextBoxGetRotateAngle(MidBox);
                        TextCursorRotateAxisX=BoxXToWindowX(TextBoxGetBoxLeft(MidBox)+TextBoxGetRotateAxisX(MidBox),MidBox);
                        TextCursorRotateAxisY=BoxYToWindowY(TextBoxGetBoxTop(MidBox)+TextBoxGetRotateAxisY(MidBox),MidBox);

                        if (Angle)
                        {
                           Result=TextBoxGetRotateAngle(MidBox);
                           Result+=Angle;
                           Result%=360;
                           if(Result<0) Result+=360;
                           BoxDrawBorder(NewHBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                           TextBoxSetRotateAngle(MidBox,Result);
                           BoxDrawBorder(NewHBox,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                        }
                        HandleUnlock(ItemGetHandle(NewHBox));
                     }
                  }
                  break;
             case IDX_TEXTBOX:
             case IDX_RECBOX:
             case IDX_CIRBOX:
             case IDX_ELIBOX:
             case IDX_LINE:
             case IDX_TABLE:
                              // define box in current page
                  if (GlobalCurrentPage<=0)
                     break;

                  if (CreatItem)     // Changeing Box
                  {
                     MidBox=HandleLock(ItemGetHandle(CreatItem));
                     if (MidBox==NULL)
                        break;
                     BoxDrawBorder(CreatItem,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                     TextBoxSetBoxWidth(MidBox,TextBoxGetBoxWidth(MidBox)+
                        WindowXToUserX((short)MAKEHI(Param2))-WindowXToUserX((short)MAKEHI(LastParam1)));
                     TextBoxSetBoxHeight(MidBox,TextBoxGetBoxHeight(MidBox)+
                        WindowYToUserY((short)MAKELO(Param2))-WindowYToUserY((short)MAKELO(LastParam1)));

                     // Result=0;
                     MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
                     PageWidth=PageGetPageWidth(MidPage);
                     PageHeight=PageGetPageHeight(MidPage);
                     HandleUnlock(ItemGetHandle(GlobalCurrentPage));

                     MidParam=WindowToUserWindow(Window,Param2);
                     WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
                     UserWindowMouseX=(short)MAKEHI(MidParam);
                     UserWindowMouseY=(short)MAKELO(MidParam);
                     if (UserWindowMouseX>=Right-Left)
                     {
                        // Result=1;
                        /*
                        MessageInsert(Window,HHSCROLLMOVE,
                         (GlobalPageHStart+WINDOWINCREASE*GlobalPageScale),
                         (PageWidth+PAGELEFTDISTANT));
                         */
                        MessageInsert(Window,HHSCROLLMOVE,
                         (GlobalPageHStart+myWindowXToUserX(WINDOWINCREASE)),
                         (PageWidth+PAGELEFTDISTANT));
                        Param2=MAKELONG((short)MAKEHI(Param2)-WINDOWINCREASE,
                                        MAKELO(Param2));
                        MouseGetPosition(&X,&Y);
                        MouseMoveTo(X-WINDOWINCREASE,Y);
                     }
                     else
                     if (UserWindowMouseX<0)
                     {
                        // Result=2;
                        /*
                        MessageInsert(Window,HHSCROLLMOVE,
                         (GlobalPageHStart-WINDOWINCREASE*GlobalPageScale),
                         (PageWidth+PAGELEFTDISTANT));
                         */
                        MessageInsert(Window,HHSCROLLMOVE,
                         (GlobalPageHStart-myWindowXToUserX(WINDOWINCREASE)),
                         (PageWidth+PAGELEFTDISTANT));
                        Param2=MAKELONG((short)MAKEHI(Param2)+WINDOWINCREASE,
                                        MAKELO(Param2));
                        MouseGetPosition(&X,&Y);
                        MouseMoveTo(X+WINDOWINCREASE,Y);
                     }

                     if (UserWindowMouseY>=Bottom-Top)
                     {
                       // Result=3;
                       /*
                        MessageInsert(Window,VVSCROLLMOVE,
                         (GlobalPageVStart+WINDOWINCREASE*GlobalPageScale),
                         (PageHeight+PAGETOPDISTANT));
                         */
                        MessageInsert(Window,VVSCROLLMOVE,
                         (GlobalPageVStart+myWindowYToUserY(WINDOWINCREASE)),
                         (PageHeight+PAGETOPDISTANT));
                        Param2=MAKELONG(MAKEHI(Param2),
                                        (short)MAKELO(Param2)-WINDOWINCREASE);
                        MouseGetPosition(&X,&Y);
                        MouseMoveTo(X,Y-WINDOWINCREASE);
                     }
                     else
                     if (UserWindowMouseY<0)
                     {
                      //  Result=4;
                      /*
                        MessageInsert(Window,VVSCROLLMOVE,
                         (GlobalPageVStart-WINDOWINCREASE*GlobalPageScale),
                         (PageHeight+PAGETOPDISTANT));
                         */
                        MessageInsert(Window,VVSCROLLMOVE,
                         (GlobalPageVStart-myWindowYToUserY(WINDOWINCREASE)),
                         (PageHeight+PAGETOPDISTANT));
                        Param2=MAKELONG(MAKEHI(Param2),
                                        (short)MAKELO(Param2)+WINDOWINCREASE);
                        MouseGetPosition(&X,&Y);
                        MouseMoveTo(X,Y+WINDOWINCREASE);
                     }

                     BoxDrawBorder(CreatItem,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                     HandleUnlock(ItemGetHandle(CreatItem));
                  }
                  else               // tobe Creating Box
                  {
                     MidParam=WindowToUserWindow(Window,Param1);
                     if((short)MAKEHI(MidParam)<0 || (short)MAKELO(MidParam)<0)
                        break;   //ByHance:: first point must be seen

                     MouseSetGraph(ARRAWMOUSE);
                     if (GlobalBoxTool==IDX_TEXTBOX)
                     {
                        MidBox=(TextBoxs *)&TmpBuf;
                        //memset(&MidtagBox,0,sizeof(MidtagBox));
                        memset(MidBox,0,sizeof(TextBoxs));

                        TextBoxSetBoxColumn(MidBox,1);
                        TextBoxSetColumnDistant(MidBox,ConvertToUserMeter(7.0)*SCALEMETER);
                        TextBoxSetBoxWidth(MidBox,WindowXToUserX((short)MAKEHI(Param2))-WindowXToUserX((short)MAKEHI(Param1)));
                        TextBoxSetBoxHeight(MidBox,WindowYToUserY((short)MAKELO(Param2))-WindowYToUserY((short)MAKELO(Param1)));
                        Param1=WindowToUserWindow(Window,Param1);
                        // Param2=WindowToUserWindow(Window,Param2);
                        TextBoxSetBoxLeft(MidBox,WindowXToUserX((short)MAKEHI(Param1)));
                        TextBoxSetBoxTop(MidBox,WindowYToUserY((short)MAKELO(Param1)));
                        if (TextBoxGetBoxWidth(MidBox)<0)
                        {
                           TextBoxSetBoxLeft(MidBox,TextBoxGetBoxLeft(MidBox)
                                             -TextBoxGetBoxWidth(MidBox));
                           TextBoxSetBoxWidth(MidBox,-TextBoxGetBoxWidth(MidBox));
                        }
                        if (TextBoxGetBoxHeight(MidBox)<0)
                        {
                           TextBoxSetBoxTop(MidBox,TextBoxGetBoxTop(MidBox)
                                            -TextBoxGetBoxHeight(MidBox));
                           TextBoxSetBoxHeight(MidBox,-TextBoxGetBoxHeight(MidBox));
                        }
                        MidBox->TextDistantLeft=DEFAULTBOXTEXTDISTANT;
                        MidBox->TextDistantTop=DEFAULTBOXTEXTDISTANT;
                        MidBox->TextDistantRight=DEFAULTBOXTEXTDISTANT;
                        MidBox->TextDistantBottom=DEFAULTBOXTEXTDISTANT;
                        CreatItem=TextBoxInsert(MidBox,GlobalCurrentPage);
                        if (CreatItem)
                           BoxDrawBorder(CreatItem,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                     }
                     else

                     if (GlobalBoxTool==IDX_TABLE)
                     {
                        FormBoxs *MidBox;

                        MidBox=(FormBoxs *)&TmpBuf;
                        //memset(&MidtagBox,0,sizeof(MidtagBox));
                        memset(MidBox,0,sizeof(FormBoxs));
                        TableBoxSetBoxType(MidBox,TABLEBOX);
                        TableBoxSetBoxWidth(MidBox,WindowXToUserX((short)MAKEHI(Param2))-WindowXToUserX((short)MAKEHI(Param1)));
                        TableBoxSetBoxHeight(MidBox,WindowYToUserY((short)MAKELO(Param2))-WindowYToUserY((short)MAKELO(Param1)));
                        Param1=WindowToUserWindow(Window,Param1);
                        TableBoxSetBoxLeft(MidBox,WindowXToUserX((short)MAKEHI(Param1)));
                        TableBoxSetBoxTop(MidBox,WindowYToUserY((short)MAKELO(Param1)));
                        if (TableBoxGetBoxWidth(MidBox)<0)
                        {
                           TableBoxSetBoxLeft(MidBox,TableBoxGetBoxLeft(MidBox)
                                             -TableBoxGetBoxWidth(MidBox));
                           TableBoxSetBoxWidth(MidBox,-TableBoxGetBoxWidth(MidBox));
                        }
                        if (TableBoxGetBoxHeight(MidBox)<0)
                        {
                           TableBoxSetBoxTop(MidBox,TableBoxGetBoxTop(MidBox)
                                            -TableBoxGetBoxHeight(MidBox));
                           TableBoxSetBoxHeight(MidBox,-TableBoxGetBoxHeight(MidBox));
                        }
                        MidBox->TextDistantLeft=DEFAULTBOXTABLEDISTANT;
                        MidBox->TextDistantTop =DEFAULTBOXTABLEDISTANT;
                        MidBox->TextDistantRight=DEFAULTBOXTABLEDISTANT;
                        MidBox->TextDistantBottom=DEFAULTBOXTABLEDISTANT;
                        //MidBox->RoundDistantLeft=0;//DEFAULTBOXTABLEDISTANT;
                        //MidBox->RoundDistantTop=0;//DEFAULTBOXTABLEDISTANT;
                        //MidBox->RoundDistantRight=0;//DEFAULTBOXTABLEDISTANT;
                        //MidBox->RoundDistantBottom=0;//DEFAULTBOXTABLEDISTANT;
                        CreatItem=TableBoxInsert(MidBox,GlobalCurrentPage);

                        if (CreatItem)
                           BoxDrawBorder(CreatItem,DRAWVIRTUALBORDOR|DRAWXORBORDER);

                     }
                     else

                     if (GlobalBoxTool==IDX_LINE)
                     {
                        LineBoxs *MidBox;

                        MidBox=(LineBoxs *)&TmpBuf;
                        // memset(MidBox,0,sizeof(MidtagBox));
                        memset(MidBox,0,sizeof(LineBoxs));

                        LineBoxSetBoxType(MidBox,LINEBOX);
                        LineBoxSetBoxWidth(MidBox,WindowXToUserX((short)MAKEHI(Param2))-WindowXToUserX((short)MAKEHI(Param1)));
                        LineBoxSetBoxHeight(MidBox,WindowYToUserY((short)MAKELO(Param2))-WindowYToUserY((short)MAKELO(Param1)));
                        Param1=WindowToUserWindow(Window,Param1);
                        LineBoxSetBoxLeft(MidBox,WindowXToUserX((short)MAKEHI(Param1)));
                        LineBoxSetBoxTop(MidBox,WindowYToUserY((short)MAKELO(Param1)));
                        if (LineBoxGetBoxWidth(MidBox)<0)
                        {
                           LineBoxSetBoxLeft(MidBox,LineBoxGetBoxLeft(MidBox)
                                             -LineBoxGetBoxWidth(MidBox));
                           LineBoxSetBoxWidth(MidBox,-LineBoxGetBoxWidth(MidBox));
                        }
                        if (LineBoxGetBoxHeight(MidBox)<0)
                        {
                           LineBoxSetBoxTop(MidBox,LineBoxGetBoxTop(MidBox)
                                            -LineBoxGetBoxHeight(MidBox));
                           LineBoxSetBoxHeight(MidBox,-LineBoxGetBoxHeight(MidBox));
                        }
                        CreatItem=LineBoxInsert(MidBox,GlobalCurrentPage);
                        if (CreatItem)
                           BoxDrawBorder(CreatItem,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                     }
                     else               // otherwise, it must be picture boxs
                     {
                        PictureBoxs *MidBox;

                        MidBox=(PictureBoxs *)&TmpBuf;
                        memset(MidBox,0,sizeof(PictureBoxs));

                        PictureBoxSetBoxWidth(MidBox,WindowXToUserX((short)MAKEHI(Param2))-WindowXToUserX((short)MAKEHI(Param1)));
                        PictureBoxSetBoxHeight(MidBox,WindowYToUserY((short)MAKELO(Param2))-WindowYToUserY((short)MAKELO(Param1)));
                        Param1=WindowToUserWindow(Window,Param1);
                        PictureBoxSetBoxLeft(MidBox,WindowXToUserX((short)MAKEHI(Param1)));
                        PictureBoxSetBoxTop(MidBox,WindowYToUserY((short)MAKELO(Param1)));
                        switch (GlobalBoxTool)
                        {
                          case IDX_RECBOX:
                               PictureBoxSetBoxType(MidBox,RECTANGLEPICTUREBOX);
                               break;
                          case IDX_CIRBOX:
                               PictureBoxSetBoxType(MidBox,CORNERPICTUREBOX);
                               break;
                          case IDX_ELIBOX:
                               PictureBoxSetBoxType(MidBox,ELIPSEPICTUREBOX);
                               break;
                        }
                        CreatItem=PictureBoxInsert(MidBox,GlobalCurrentPage);
                        if (CreatItem)
                           BoxDrawBorder(CreatItem,DRAWVIRTUALBORDOR|DRAWXORBORDER);
                     }
                  }
                  break;
             case IDX_PLGBOX:
             case IDX_LINK:
             case IDX_UNLINK:
                  break;
           }
         }

         MessageGo(Window,DELBUBLE,0L,0L);  // delete old hint window
         DrawCurrentClibration(Window,(short)MAKEHI(Param2),(short)MAKELO(Param2));
         LastParam1=Param2;
         break;
    case MOUSEMOVE:
//       WindowDefaultProcedure(Window,Message,Param1,Param2);
         switch (GlobalBoxTool)
         {
           case IDX_PLGBOX:
                if (PolygonNumber>0&&PolygonNumber<MAXPOLYGONNUMBER)
                {                      // ???
                   int SaveColor;
         #ifdef __TURBOC__
                   struct linesettingstype SaveLineStyle;
                   getlinesettings(&SaveLineStyle);
                   setlinestyle(4,0x5555,1);
         #else
                   unsigned old_style=getlinestyle();
                   setlinestyle(0x5555);
         #endif

                   MouseSetGraph(ARRAWMOUSE);         // ByHance, 95,12.6

                   Param1=WindowToUserWindow(Window,Param1);
                   SaveColor=getcolor();
                   MouseHidden();
                   WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
                   setviewport(Left,Top,Right,Bottom,1);
                   setcolor(EGA_WHITE);
                   setwritemode(XOR_PUT);
                    //   clear old line
                   /*
                   line(PolygonEdges[2*PolygonNumber-2]-GlobalPageHStart/GlobalPageScale,
                        PolygonEdges[2*PolygonNumber-1]-GlobalPageVStart/GlobalPageScale,
                        PolygonEdges[2*PolygonNumber]-GlobalPageHStart/GlobalPageScale,
                        PolygonEdges[2*PolygonNumber+1]-GlobalPageVStart/GlobalPageScale);
                        */
                   line(PolygonEdges[2*PolygonNumber-2]-myUserXToWindowX(GlobalPageHStart),
                        PolygonEdges[2*PolygonNumber-1]-myUserYToWindowY(GlobalPageVStart),
                        PolygonEdges[2*PolygonNumber]-myUserXToWindowX(GlobalPageHStart),
                        PolygonEdges[2*PolygonNumber+1]-myUserYToWindowY(GlobalPageVStart));
                    //   draw new line
                    /*
                   line(PolygonEdges[2*PolygonNumber-2]-GlobalPageHStart/GlobalPageScale,
                        PolygonEdges[2*PolygonNumber-1]-GlobalPageVStart/GlobalPageScale,
                        (short)MAKEHI(Param1),(short)MAKELO(Param1));
                        */
                   line(PolygonEdges[2*PolygonNumber-2]-myUserXToWindowX(GlobalPageHStart),
                        PolygonEdges[2*PolygonNumber-1]-myUserYToWindowY(GlobalPageVStart),
                        (short)MAKEHI(Param1),(short)MAKELO(Param1));
                   setwritemode(COPY_PUT);

       #ifdef __TURBOC__
                   setlinestyle(SaveLineStyle.linestyle,
                                SaveLineStyle.upattern,
                                SaveLineStyle.thickness);
       #else
                   setlinestyle(old_style);
       #endif

                   setcolor(SaveColor);
                   MouseShow();
                   //PolygonEdges[2*PolygonNumber]=(short)MAKEHI(Param1)+GlobalPageHStart/GlobalPageScale;
                   //PolygonEdges[2*PolygonNumber+1]=(short)MAKELO(Param1)+GlobalPageVStart/GlobalPageScale;
                   PolygonEdges[2*PolygonNumber]=(short)MAKEHI(Param1)+myUserXToWindowX(GlobalPageHStart);
                   PolygonEdges[2*PolygonNumber+1]=(short)MAKELO(Param1)+myUserYToWindowY(GlobalPageVStart);
                }
                else
                {
                   UserMouseMove(Window,Message,Param1,Param2);
                }
                break;
           default:
                UserMouseMove(Window,Message,Param1,Param2);
                break;
         }
         DrawCurrentClibration(Window,(short)MAKEHI(Param1),(short)MAKELO(Param1));
         LastParam1=Param1;
         break;
    case WINDOWQUITCONFIRM:
         Result=WindowDefaultProcedure(Window,Message,Param1,Param2);
         if (Result!=TRUE)
            return(Result);

         if (FileHasBeenLoaded()&&FileHasBeenModified()&&!FileHasBeenSaved())
         {
            Result=MessageBox(GetTitleString(WARNINGINFORM),
                              GetInformString(FILENOTSAVE),
                              3,Window);
            if (Result==2)              // cancel
               return(TRUE);
            if (Result==1)              // do not save
               return(FALSE);
            if (Result==0)              // save
               return(MessageGo(Window,MENUCOMMAND,MENU_SAVE,0));
         }
         break;
    case MENUCOMMAND:
         return(UserMenuCommand(Window,Message,Param1));
    case DRAWWINDOW:
         {
           int PageLeft,PageTop,PageRight,PageBottom;

           TextCursorOff();
           WindowDefaultProcedure(Window,Message,Param1,Param2);
           if (ToolBarHasRulerBar())
           {
              MidWindow=SearchHClibrationWindow(Window);
              if (MidWindow)
                 MessageInsert(MidWindow,DRAWWINDOW,0l,
                               MAKELONG(WindowGetWidth(MidWindow),
                                        WindowGetHeight(MidWindow)));
              MidWindow=SearchVClibrationWindow(Window);
              if (MidWindow)
                 MessageInsert(MidWindow,DRAWWINDOW,0l,
                               MAKELONG(WindowGetWidth(MidWindow),
                                        WindowGetHeight(MidWindow)));
           }
           WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
           WindowGetRect(Window,&PageLeft,&PageTop,&PageRight,&PageBottom);
           if (GlobalCurrentPage>=0)
           {
              PageDraw(GlobalCurrentPage,Window,(short)MAKEHI(Param1)+Left-PageLeft,
                       (short)MAKELO(Param1)+Top-PageTop,(short)MAKEHI(Param2)+Left-PageLeft,
                       (short)MAKELO(Param2)+Top-PageTop);
              if (PolygonNumber)
                 DrawCreatingPolygon(PolygonNumber,PolygonEdges);
              if (CreatItem>0)
              {
                 BoxDrawBorder(CreatItem,DRAWVIRTUALBORDOR|DRAWXORBORDER|16);
                 BoxDrawBorder(CreatItem,DRAWVIRTUALBORDOR|DRAWXORBORDER);
              }
              if (MoveSign==MovingSign||MoveSign==ResizingSign)
              {
                 BoxDrawBorder(GlobalBoxHeadHandle,DRAWXORBORDER|16);
                 BoxDrawBorder(GlobalBoxHeadHandle,DRAWVIRTUALBORDOR|DRAWXORBORDER);
              }
              else
              if (MoveSign==GroupMovingSign)
              {
                 //GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                 GroupDrawAllBorder(DRAWXORBORDER|16);
                 GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
              }
           }
         }
         break;
    case VVSCROLLMOVE:
         if (count)
         {
            count+=3-5+2;
         }
         // WindowDefaultProcedure(Window,Message,Param1,Param2);
         if ((!ItemGetHandle(GlobalCurrentPage))||(RotateSign))
            break;                   // When do rotate can't scroll window
         MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
         PageHeight=PageGetPageHeight(MidPage);

         if (Param2>=PageHeight)
            GlobalPageVStart=Param1;
         else
            GlobalPageVStart=Param1*(PageHeight+PAGETOPDISTANT)/Param2;
         HandleUnlock(ItemGetHandle(GlobalCurrentPage));
         goto lbl_redraw;

    case HHSCROLLMOVE:
         if (count)
         {
            count+=3-5+2;
         }
         //WindowDefaultProcedure(Window,Message,Param1,Param2);
         if ((!ItemGetHandle(GlobalCurrentPage))||(RotateSign))
            break;                   // When do rotate can't scroll window
         MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
         PageWidth=PageGetPageWidth(MidPage);
         if (Param2>=PageWidth)
            GlobalPageHStart=Param1;
         else
            GlobalPageHStart=Param1*(PageWidth+PAGELEFTDISTANT)/Param2;
         HandleUnlock(ItemGetHandle(GlobalCurrentPage));
    lbl_redraw:
         if (!(GlobalBoxTool!=IDX_INPUTBOX||(GlobalBoxHeadHandle<=0)
             ||(ItemGetFather(GlobalBoxHeadHandle)!=GlobalCurrentPage)
             ||(ActiveMenu>0)||(ActiveWindow!=1)
             ||!CurrentBoxEditable())&&!IsInGlobalBrowser())
            SetNewCursor();
         RedrawUserField();
         break;
    default:
         return(WindowDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
} /* UserProcedure */

void UserMouseMove(HWND Window,HMSG Message,ULONG Param1,ULONG Param2)
{
   HWND win1;
   short MouseX,MouseY;
   HBOX SelectBox;
   TextBoxs *MidBox;
   BOOL nearleft,nearright,neartop,nearbottom;
   int i,BoxDots;
   ORDINATETYPE BoxXY[2*MAXPOLYGONNUMBER+2];
   ORDINATETYPE *PolygonDots;
   int Left,Top,Right,Bottom;

//   if(Window!=ActiveWindow)
   if(ActiveWindow!=1)
   {
         MouseSetGraph(ARRAWMOUSE);
         return;
   }

   if(ActiveMenu>0 || ActiveTopMenu>0)
   {
         MouseSetGraph(FINGERMOUSE);
         return;
   }

/*-------
   MouseX = (short)MAKEHI(Param1);
   MouseY = (short)MAKELO(Param1);
-----*/
   WindowGetRealRect(Window,&Left,&Top,&Right,&Bottom);
   MouseX = (short)MAKEHI(Param1)+Left;
   MouseY = (short)MAKELO(Param1)+Top;

   #ifdef REGIST_VERSION
   {
     char *p;
     p=(char *)&BoxXY[0]; i=0;
     p[i]=logname[i];   i++;
     p[i]=logname[i]-5; i++;
     p[i]=logname[i]+10;i++;
     p[i]=logname[i]-20;i++;
     p[i]=logname[i]+30;i++;
     p[i]=logname[i];   i++;
     p[i]=logname[i]-40;i++;
       // p=<ezp.log>
     // change it to "C:/ezp/ezp.log"
     //                 2   6
     memcpy(p+7,p,7);  p[6]='/';    memcpy(p+2,p+6,4);
     p[0]='c'; p[1]=':'; p[14]=0;

     if(MessageGo(Window,GETLOGFILE,FP2LONG(p), FP2LONG(&TmpBuf) ))
     {
        short num,k,n,*pShort;
        unsigned short code[4];

        memset(p,0,MAXPOLYGONNUMBER);       // unused
      /*----------for test ----
        if(TimeCountArr[1]<2)       // wait for get disk serial number
        {
            MessageGo(1,GETDISKSERIAL,
                    MAKELONG(0x1f7,0x1f0), MAKELONG(0xa0,0xec) );
            memcpy(serial, PrintName, 40);
            TimeCountArr[1]+=3;
        }
         ----------------------*/
        p=(char *)&TmpBuf;
        pShort=(short *)&TmpBuf;
        num=p[257];
        memcpy(code,&p[258],8);
        if(code[0]==code[1])    // must error: 2 disks spare 3600 seconds
            code[1]+=code[0]+0x6a8b;

        i=298+20-4;
        for(n=i/2;n<(i+44)/2;n++)
            for(k=0;k<num;k++)
              pShort[n]+=code[k];

        tsum=(unsigned short)pShort[i/2+1];     // offset[316]
        ssum=(unsigned short)pShort[i/2];       // offset[314]
        memcpy(regist_str,&p[i+4],40);          // offset[318..358]
        memcpy(&BoxXY[0],regist_str,30);      // unused

    /*-------------------- for test , 96,1.25 ---*/
       #ifdef REGIST_TEST
        printf("SerialTypeLen=%d\n",SerialTypeLen);
        if(ssum!=SerialSum)
           printf("ssum error: ssum=%x, p=%x\n",ssum,SerialSum);
        if(tsum!=TypeSum)
           printf("tsum error: tsum=%x, p=%x\n",tsum,TypeSum);

        for(i=0;i<SerialTypeLen;i++)
        if(regist_str[i]!=serial[i])
        {
           printf("%d: str=%2x, serial=%2x\n",i,regist_str[i],serial[i]);
        }
        getch();
       #endif
        /*-----------------------------*/
     }

     if(Param1==Param2 && Param1==0)    // for other procedure call it
        return;
   }
   #endif   // REGIST_VERSION

   if ((win1=WindowGetNumber(MouseX,MouseY,ActiveWindow,2))<OpOK)
         return;
   if (win1 != Window) {
         //MessageGo(win1,MOUSEMOVE,Param1,Param2);
      /*-----------------*/
         WindowGetRealRect(win1,&Left,&Top,&Right,&Bottom);
         MouseX-=Left;
         MouseY-=Top;
         Param1=MAKELONG(MouseX,MouseY);
         MessageGo(win1,MOUSEMOVE,Param1,0L);
        /*---------------*/
         return;
   }

   MessageGo(Window,DELBUBLE,0L,0L);  // delete old hint window

  #ifdef REGIST_VERSION
   MouseX=ssum;  MouseY=SerialSum;
       if(TimeCountArr[2] && fRegist)            // if got log file
       {
           //putch(7);
           if(MouseX!=MouseY)
           {
              MessageInsert(Window,REGISTERROR,0L,0L);
              if(WasteTimer==-1)
                 WasteTimer=TimerInsert(ActiveWindow,1);  // window=1, 18.2/second
           }
       }
  #endif   // REGIST_VERSION

   switch (GlobalBoxTool)
   {
     case IDX_INPUTBOX:     // Select context in box
          Param1=WindowToUserWindow(Window,Param1);
          MouseX = (short)MAKEHI(Param1);
          MouseY = (short)MAKELO(Param1);
          SelectBox=FindXYInBox(GlobalCurrentPage,MouseX,MouseY);
          if (SelectBox)
          {            // has selected text or picture or table form
             //if (BoxIsTextBox(SelectBox)) {
             if (BoxCanEditable(SelectBox)) {
                 MouseSetGraph(VCARETMOUSE);
                 return;
             }

             if (BoxIsPictureBox(SelectBox)) {
                 MouseSetGraph(BROWSEHANDMOUSE);
                 return;
             }
          }
          break;
     case IDX_ZOOM:
          MouseSetGraph(ZOOMMOUSE);
          return;
     case IDX_ROTATE:
          if (!GlobalGroupGetSign())
            if(GlobalBoxHeadHandle>0)
            {
                MidBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                if (MidBox==NULL)
                    return;

                if (BoxIsLocked(MidBox))              // Locked !
                {
                   MouseSetGraph(LOCKMOUSE);
                 lbl_no_box:
                   HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                   return;
                }
             /*----------- ByHance, 96,3.17 ----
                if(TextBoxGetBoxType(MidBox)==TABLEBOX)   // ByHance, 96,3.6
                {
                   MouseSetGraph(ARRAWMOUSE);
                   goto lbl_no_box;
                }
              --------------------------------------*/
            }
            else
            {
                MouseSetGraph(ARRAWMOUSE);
                goto lbl_no_box;
            }

          if(RotateSign==NoRotateSign)
              MouseSetGraph(CROSSMOUSE);
          else
              MouseSetGraph(ROTATEMOUSE);
          return;
     case IDX_PLGBOX:
          if(!PolygonNumber)
              MouseSetGraph(FINGERMOUSE2);         // ByHance, 95,12.6
          return;
     case IDX_TEXTBOX:
     case IDX_TABLE:
     case IDX_RECBOX:
     case IDX_CIRBOX:
     case IDX_ELIBOX:
     case IDX_LINE:
          if (CreatItem)
              MouseSetGraph(ARRAWMOUSE);
          else
              MouseSetGraph(FINGERMOUSE2);
          return;
     case IDX_LINK:
          Param1=WindowToUserWindow(Window,Param1);
          MouseX = (short)MAKEHI(Param1);
          MouseY = (short)MAKELO(Param1);
          SelectBox=FindXYInBox(GlobalCurrentPage,MouseX,MouseY);
          if (SelectBox && BoxIsTextBox(SelectBox))
          {            // has selected text
             MidBox=HandleLock(ItemGetHandle(SelectBox));
             if(!LinkSign && TextBoxGetNextLinkBox(MidBox)==0)
             {
                MouseSetGraph(LINKMOUSE);
                return;
             }
             if(LinkSign && TextBoxGetPrevLinkBox(MidBox)==0
                && TextBoxGetTextLength(MidBox)<=1
                && !TextBoxLinkCycle(LinkPrevBox,SelectBox))
             {
                MouseSetGraph(CUPMOUSE2);       // pouring  cup
                return;
             }
          }

          if(LinkSign) {
             MouseSetGraph(CUPMOUSE1);          // cup with word
             return;
          }
          break;
     case IDX_UNLINK:
          Param1=WindowToUserWindow(Window,Param1);
          MouseX = (short)MAKEHI(Param1);
          MouseY = (short)MAKELO(Param1);
          SelectBox=FindXYInBox(GlobalCurrentPage,MouseX,MouseY);
          if (SelectBox && BoxIsTextBox(SelectBox))
          {            // has selected text
             MidBox=HandleLock(ItemGetHandle(SelectBox));
             if(TextBoxGetPrevLinkBox(MidBox)!=0)
             {
                 MouseSetGraph(UNLINKMOUSE);
                 return;
             }
          }
          break;
     case IDX_SELECTBOX:     // change box range, or move box position
          Param1=WindowToUserWindow(Window,Param1);
          MouseX = (short)MAKEHI(Param1);
          MouseY = (short)MAKELO(Param1);

          if (GlobalGroupGetSign())          // has defined group
          {
             if (MousePointInGroup(WindowXToUserX(MouseX),
                                   WindowYToUserY(MouseY)) )
             {       // this group now is selected
                 MouseSetGraph(MOVINGMOUSE);
                 return;
             }
          }

         //////////// ready to check global box borders
          if (GlobalBoxHeadHandle>0)
          {
              TextBoxs *pBox;
              pBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
              if (pBox==NULL)
                 return;

              if (BoxIsLocked(pBox))              // Locked !
                 goto lbl_search_box;

              MouseX =WindowXToUserX(MouseX);
              MouseY =WindowYToUserY(MouseY);

              if (TextBoxGetRotateAngle(pBox))
              {
                 Rotate(&Left,&Top,MouseX,MouseY,
                        TextBoxGetBoxLeft(pBox)+TextBoxGetRotateAxisX(pBox),
                        TextBoxGetBoxTop(pBox)+TextBoxGetRotateAxisY(pBox),
                        -TextBoxGetRotateAngle(pBox));
                 MouseX=Left; MouseY=Top;
              }

              switch (TextBoxGetBoxType(pBox))
              {
                case TEXTBOX:
                case RECTANGLEPICTUREBOX:
                case CORNERPICTUREBOX:
                case ELIPSEPICTUREBOX:
                case TABLEBOX:
                     nearleft = (MouseX>=TextBoxGetBoxLeft(pBox)) &&
                              ((MouseX-TextBoxGetBoxLeft(pBox))<DELTASIZE);
                     nearright = (MouseX<TextBoxGetBoxRight(pBox)) &&
                             ((TextBoxGetBoxRight(pBox)-MouseX)<DELTASIZE);
                     neartop = (MouseY>=TextBoxGetBoxTop(pBox)) &&
                              ((MouseY-TextBoxGetBoxTop(pBox))<DELTASIZE);
                     nearbottom = (MouseY<TextBoxGetBoxBottom(pBox)) &&
                             ((TextBoxGetBoxBottom(pBox)-MouseY)<DELTASIZE);

                     if ((nearleft && neartop)||(nearright&&nearbottom))
                     {
                        MouseSetGraph(RESIZEMOUSE1);
             lbl_jerry:
                        HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                        return;
                     }

                     if ((nearleft && nearbottom)||(nearright&&neartop))
                     {
                        MouseSetGraph(RESIZEMOUSE2);
                        goto lbl_jerry;
                     }

                     if( (nearleft || nearright)
                         && MouseY>TextBoxGetBoxTop(pBox)
                         && MouseY<TextBoxGetBoxBottom(pBox) )
                     {
                        MouseSetGraph(HRESIZEMOUSE);
                        goto lbl_jerry;
                     }
                     if( (neartop||nearbottom)
                         && MouseX>TextBoxGetBoxLeft(pBox)
                         && MouseX<TextBoxGetBoxRight(pBox) )
                     {
                        MouseSetGraph(VRESIZEMOUSE);
                        goto lbl_jerry;
                     }
                //     break;           ByHance, 96,3.17
                //case TABLEBOX:        ByHance, 96,3.17
                     if(TextBoxGetBoxType(pBox)==TABLEBOX)
                     {
                          MouseX-=TextBoxGetBoxLeft(pBox);
                          MouseY-=TextBoxGetBoxTop(pBox);
                          i=FBCellofXY(GlobalBoxHeadHandle,MouseX,MouseY);
                          if (i>=0)
                          {
                             FBGetCellRect(GlobalBoxHeadHandle,i,&Left,&Top,&Right,&Bottom);
                             if( abs(MouseX-Left)<DELTASIZE
                              || abs(MouseX-Right)<DELTASIZE)
                             {
                                MouseSetGraph(HRESIZEMOUSE);
                                goto lbl_jerry;
                             }
                             if( abs(MouseY-Top)<DELTASIZE
                              || abs(MouseY-Bottom)<DELTASIZE)
                             {
                                MouseSetGraph(VRESIZEMOUSE);
                                goto lbl_jerry;
                             }
                          }
                     }
                     break;
                case POLYGONPICTUREBOX:
                     BoxGetPolygonDrawBorder((Boxs *)pBox,&BoxDots,BoxXY);
                     PolygonDots=HandleLock(PictureBoxGetBorderPolygon(pBox));
                     if (PolygonDots==NULL)
                         goto lbl_search_box;

                     for (i=0;i<BoxDots;i++)           // is it at Point ?
                       if (abs(MouseX-BoxXY[2*i])<DELTASIZE
                       &&abs(MouseY-BoxXY[2*i+1])<DELTASIZE)
                       {
                          MouseSetGraph(FINGERMOUSE2);
                          goto lbl_jerry;
                       }

                      // is it in line ?
                     BoxXY[2*BoxDots]=BoxXY[0];
                     BoxXY[2*BoxDots+1]=BoxXY[1];
                     for (i=0;i<BoxDots;i++)
                       if (PointInLine(MouseX,MouseY,BoxXY[2*i],
                            BoxXY[2*i+1],BoxXY[2*i+2],BoxXY[2*i+3]))
                       {                      // Edges
                          MouseSetGraph(FINGERMOUSE2);
                          goto lbl_jerry;
                       }

                     break;
              } // end of switch box

      lbl_search_box:
              HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
          }  // end of globalboxhandle>0

          MouseX = (short)MAKEHI(Param1);
          MouseY = (short)MAKELO(Param1);
          SelectBox=FindXYInBox(GlobalCurrentPage,MouseX,MouseY);
         /*-------- search which Box is locked ------*/
          if (SelectBox)
           {            // has selected text or picture or table form
                MidBox=HandleLock(ItemGetHandle(SelectBox));
                if (MidBox==NULL)
                    return;

                if (BoxIsLocked(MidBox))              // Locked !
                {
                   MouseSetGraph(LOCKMOUSE);
                   HandleUnlock(ItemGetHandle(SelectBox));
                   return;
                }
          /*-------- can it move ? -------*/
                // if (SelectBox==GlobalBoxHeadHandle)
                {
                   MouseSetGraph(MOVINGMOUSE);
                   return;
                }
          }
          break;
    }   // switch

    WindowDefaultProcedure(Window,Message,Param1,Param2);
} /* UserMouseMove */
