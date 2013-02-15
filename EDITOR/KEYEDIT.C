/*-------------------------------------------------------------------
* Name: keyedit.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define REDRAWDELTA 1

/* TextBox Cursor Key Deal Procedure */
int BoxXToWindowX(ORDINATETYPE X,TextBoxs *TextBox)
{
  return(UserXToWindowX(TextBoxGetBoxLeft(TextBox)+X));
}

int BoxIsModule(char *s,HBOX HBox)
{
#ifdef DEBUG_VERSION
   int attr;
   TextBoxs *TextBox;

   TextBox=HandleLock(ItemGetHandle(HBox));
   attr=TextBox->BoxAttr;
   HandleUnlock(ItemGetHandle(HBox));

   if (BoxCanNotEdit(TextBox))
     {
        char str[100];
        sprintf(str,"Module Box is :%d,from Program:%s",HBox,s);
        MessageBox("Debug",str,1,0);
     }

   return attr;
#else
   return 0;
#endif
}

int BoxYToWindowY(ORDINATETYPE Y,TextBoxs *TextBox)
{
  return(UserYToWindowY(TextBoxGetBoxTop(TextBox)+Y));
}

static int SetCurrentTextBox(HBOX NewHBox)
{               // Return 0 : No adjust page, 1 : Adjust page
  int Result;

  BoxIsModule("SetCurrentTextBox",NewHBox);

  if (BoxGetPage(NewHBox)!=GlobalCurrentPage)
  {
     PageGotoHandle(BoxGetPage(NewHBox));
     Result=1;
  }
  else
     Result=0;

  if (NewHBox!=GlobalBoxHeadHandle)
  {
     GlobalBoxHeadHandle=NewHBox;
     SetTextBoxTextCursor(NewHBox);
  }
  return(Result);
}

// -------------- ByHance ----------
// if at lineFeed, call it directly, otherwise, call CursorLocate
// -------------- ByHance ----------
int CursorLocate2(HBOX HBox,HBOX *pNewBox,int Position,int *X,int *Y)
{                        // Return 0 : To redraw,  1 : Had Redrawed
  TextBoxs *TextBox;
  int BoxInWindowLeft,BoxInWindowTop;
  int Result1,Result2;

  if (count)
  {
  return 0;
  }
  count++;

  BoxInWindowTop=TextSearchAttribute(HBox,Position,CHARSIZE,&BoxInWindowLeft);
  //TextCursorSetHeight(BoxInWindowTop/GlobalPageScale);
  TextCursorSetHeight(myUserYToWindowY(BoxInWindowTop));
  OpenTextCursor();

  TextBox=HandleLock(ItemGetHandle(*pNewBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
                                       /* Set new current text box */
  Result1=SetCurrentTextBox(*pNewBox);

  if(fEditor)
  {
      Pmark_rec eptr=LocateMarkbyPos(TextBox,Position);
      if(eptr && CurrentRow!=eptr->line_number)
      {
          CurrentRow=eptr->line_number;
          TellStatus();
      }
  }

  if (TextBoxGetRotateAngle(TextBox))
  {
     Rotate(&BoxInWindowLeft,&BoxInWindowTop,
            TextBoxGetBoxLeft(TextBox)+(*X),
            TextBoxGetBoxTop(TextBox)+(*Y),
            TextBoxGetRotateAxisX(TextBox)+TextBoxGetBoxLeft(TextBox),
            TextBoxGetRotateAxisY(TextBox)+TextBoxGetBoxTop(TextBox),
            TextBoxGetRotateAngle(TextBox));
     BoxInWindowLeft=UserXToWindowX(BoxInWindowLeft);
     BoxInWindowTop=UserYToWindowY(BoxInWindowTop);
  }
  else
  {
     BoxInWindowLeft=UserXToWindowX(TextBoxGetBoxLeft(TextBox)+(*X));
     BoxInWindowTop=UserYToWindowY(TextBoxGetBoxTop(TextBox)+(*Y));
  }

  do
  {
    Result2=CursorAdjusttoWindow(BoxInWindowLeft,BoxInWindowTop);
    if (!TextBoxGetRotateAngle(TextBox))
    {
       if (Result2&1)
          BoxInWindowLeft=UserXToWindowX(TextBoxGetBoxLeft(TextBox)+(*X));
       if (Result2&2)
          BoxInWindowTop=UserYToWindowY(TextBoxGetBoxTop(TextBox)+(*Y));
    }
    else
    {
       if (Result2)
       {
          Rotate(&BoxInWindowLeft,&BoxInWindowTop,
                 TextBoxGetBoxLeft(TextBox)+(*X),
                 TextBoxGetBoxTop(TextBox)+(*Y),
                 TextBoxGetRotateAxisX(TextBox)+TextBoxGetBoxLeft(TextBox),
                 TextBoxGetRotateAxisY(TextBox)+TextBoxGetBoxTop(TextBox),
                 TextBoxGetRotateAngle(TextBox));
          BoxInWindowLeft=UserXToWindowX(BoxInWindowLeft);
          BoxInWindowTop=UserYToWindowY(BoxInWindowTop);
       }
    }

    if (Result2)
       Result1=1;
  }
  while (Result2);

  if (TextBoxGetRotateAngle(TextBox))
  {
     BoxInWindowLeft=UserXToWindowX(TextBoxGetBoxLeft(TextBox)+(*X));
     BoxInWindowTop=UserYToWindowY(TextBoxGetBoxTop(TextBox)+(*Y));
  }
  TextCursorMoveTo(BoxInWindowLeft,BoxInWindowTop);
  HandleUnlock(ItemGetHandle(*pNewBox));

  count--;
  if (Result1)
     return(1);
  else
     return(0);
}

int CursorLocate(HBOX HBox,HBOX *pNewBox,int Position,int *X,int *Y)
{
  HBOX HNewBox;
  int  Result;

  if (count)
   {
    Result=0;
   }
  PosToBoxCursorXY(HBox,&HNewBox,Position,X,Y);
  Result=CursorLocate2(HBox,&HNewBox,Position,X,Y);
  *pNewBox=HNewBox;
  return Result;
}

int CancelBlock(HBOX HBox,int *BlockStart,int *BlockEnd)
{
  if (*BlockStart<*BlockEnd)
  {
     if (BoxIsTableBox(HBox) && (GlobalTableBlockStart<GlobalTableBlockEnd))
        CancelCellBlock(HBox);
     else
        DisplayBlock(HBox,*BlockStart,*BlockEnd);
     UndoInsertCursorUndefineBlock(*BlockStart,*BlockEnd);
     *BlockEnd=*BlockStart=-1;
  }
  ReturnOK();
}

int CursorCtrlLeft(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                   int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result;

  CancelBlock(HBox,BlockStart,BlockEnd);
  bAtLineFeed=0;

  TextCursorOff();

  Result=TextBoxSeekPrevWord(HBox,Position,NewPosition);
  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }
  else
     Alarm();

  TextCursorDisplay();

  ReturnOK();
}

int CursorCtrlRight(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                   int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result;

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextCursorOff();

  if(bAtLineFeed)
  {
     Position--;     // to prev mark
     bAtLineFeed=0;
  }

  Result=TextBoxSeekNextWord(HBox,Position,NewPosition);
  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }
  else
     Alarm();

  TextCursorDisplay();

  ReturnOK();
}

int CursorLeft(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
               int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result;

  CancelBlock(HBox,BlockStart,BlockEnd);
  bAtLineFeed=0;                // ByHance, 95,12.14

  TextCursorOff();

  Result=TextBoxSeekTextPosition(HBox,Position,-Length,NewPosition);
  if( BoxIsTableBox(HBox)
  && TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell )
     TextBoxShiftTabKey(HBox,NewHBox,*NewPosition,NewPosition,BlockStart,BlockEnd);

  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }
  else
     Alarm();

  TextCursorDisplay();

  if (Result!=Length)
     return(Result);
  ReturnOK();
}

int CursorRight(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
                int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result;
  //int SaveTableCell;

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextCursorOff();

  if(bAtLineFeed)               // ByHance, 95,12.14
  {
     Position--;     // to prev mark
     bAtLineFeed=0;
  }

  Result=TextBoxSeekTextPosition(HBox,Position,Length,NewPosition);
  if (BoxIsTableBox(HBox)) {
     while (!TrueCellIsFirstCell(HBox,*NewPosition))
         Result=TextBoxTABKey(HBox,NewHBox,Position,NewPosition,
                        BlockStart,BlockEnd);
     if (TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell)
         Result=TextBoxTABKey(HBox,NewHBox,Position,NewPosition,
                        BlockStart,BlockEnd);
  }

  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }
  else
  {
     *NewPosition=Position;
     Alarm();
  }

  TextCursorDisplay();

  if (Result!=Length)
     return(Result);
  ReturnOK();
}

int CursorUp(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
             int *BlockStart,int *BlockEnd)
{
  //int Result;
  int CursorX,CursorY;

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextCursorOff();

  if(bAtLineFeed)               // ByHance, 95,12.14
  {
     Position--;     // to prev mark
     bAtLineFeed=0;
  }

  BoxCursorPosDown(HBox,NewHBox,Position,NewPosition,-Length);
  if( BoxIsTableBox(HBox)
  && ( TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell
     || Position==*NewPosition) )
  {
     GlobalTableCell=FBFindNextCell(HBox,GlobalTableCell,UPCELL);
     *NewPosition=TableCellGetTextHead(HBox,GlobalTableCell)+
                  TableCellGetTextLength(HBox,GlobalTableCell);
  }

  if (Position>*NewPosition)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(*NewHBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }
  else
  {
     *NewPosition=Position;     // DG in 1996,3.12
     Alarm();
  }

  TextCursorDisplay();

  ReturnOK();
}

int CursorDown(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
               int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY;

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextCursorOff();

  if(bAtLineFeed)               // ByHance, 95,12.14
  {
     Position--;     // to prev mark
     bAtLineFeed=0;
  }

  BoxCursorPosDown(HBox,NewHBox,Position,NewPosition,Length);
  if (BoxIsTableBox(HBox))
  {
      while (!TrueCellIsFirstCell(HBox,*NewPosition))
          TextBoxTABKey(HBox,NewHBox,Position,NewPosition,BlockStart,BlockEnd);

      if( TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell
       || Position==*NewPosition)  // DG Change
      {
         GlobalTableCell=FBFindNextCell(HBox,GlobalTableCell,DOWNCELL);
         *NewPosition=TableCellGetTextHead(HBox,GlobalTableCell);
      }
  }

  if (Position<*NewPosition)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }
  else
  {
     *NewPosition=Position;     // DG in 1996,3.12
     Alarm();
  }

  TextCursorDisplay();

  ReturnOK();
}

int CursorGotoLine(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
               int NewLine, int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY;

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextCursorOff();

  bAtLineFeed=0;

  BoxLineColumnToPos(HBox,NewHBox,NewLine,0,NewPosition);

  if (Position==*NewPosition)
     Alarm();
  else
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }

  TextCursorDisplay();

  ReturnOK();
}

int CursorHome(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
               int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result;

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextCursorOff();

  if(bAtLineFeed)               // ByHance, 95,12.14
  {
     Position--;     // to prev mark
     bAtLineFeed=0;
  }

  PosToBoxLineColumn(HBox,NewHBox,Position,&CursorY,&CursorX);
  BoxLineColumnToPos(HBox,NewHBox,CursorY,0,NewPosition);

  Result=Position-*NewPosition;
  //Result=TextBoxSeekTextPosition(HBox,Position,*NewPosition-Position,NewPosition);
  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     Result=0;
  }
  else
     Alarm();

  TextCursorDisplay();

  return Result;
}

int TableCursorEnd(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
              int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result;

  CancelBlock(HBox,BlockStart,BlockEnd);

  if(bAtLineFeed)               // ByHance, 95,12.14
  {
     *NewPosition=Position;
     Alarm();
     ReturnOK();
  }

  TextCursorOff();

  PosToBoxLineColumn(HBox,NewHBox,Position,&CursorY,&CursorX);
  BoxLineColumnToPos(HBox,NewHBox,CursorY+1,0,NewPosition);

  //(*NewPosition)--;
  TextBoxSeekTextPosition(HBox,*NewPosition,-1,NewPosition);

  Result=*NewPosition-Position;
  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     Result=0;
  }
  else
     Alarm();

  TextCursorDisplay();

  return Result;
}

int CursorEnd(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
              int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY; // ,Result;

  CancelBlock(HBox,BlockStart,BlockEnd);
  if(bAtLineFeed)               // ByHance, 95,12.14
  {
     *NewPosition=Position;
     Alarm();
     ReturnOK();
  }

  TextCursorOff();

/*------------------ changed ByHance, 95,12.14 -----------
  PosToBoxLineColumn(HBox,NewHBox,Position,&CursorY,&CursorX);
  Result=BoxLineColumnToPos(HBox,NewHBox,CursorY+1,0,NewPosition);

  //(*NewPosition)--;                      // ??? Has LF
  if(Result==OpOK)                      // ByHance, 95,12.13
    TextBoxSeekTextPosition(HBox,*NewPosition,-1,NewPosition);

  Result=*NewPosition-Position;
  if (Result<=0)
  {
     Alarm();
     return(Result);
  }
  else
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }
  ReturnOK();
 -------------------------------*/

  *NewHBox=HBox;
  bAtLineFeed=PosToBoxRowEnd(HBox,Position,NewPosition,&CursorY,&CursorX);

  if(bAtLineFeed)
     CursorLocate2(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  else
  {
     if (*NewPosition>Position)
        CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     else
        Alarm();
  }

  TextCursorDisplay();

  ReturnOK();
/*-------- end changed ByHance, 95,12.14 -----------*/
}

int CursorPgUp(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
              int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY; // ,Result;

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextCursorOff();

  bAtLineFeed=0;                // ByHance, 95,12.14

  if(BoxIsTableBox(HBox))
     *NewPosition=0;
  else
  {
     PosToBoxLineColumn(HBox,NewHBox,Position,&CursorY,&CursorX);
     BoxLineColumnToPos(HBox,NewHBox,CursorY-10,CursorX,NewPosition);
  }

  // Result=Position-*NewPosition;
  // if (Result<=0)
  if (Position<=*NewPosition)
  {
     Alarm();
     // return(Result);
  }
  else
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }

  TextCursorDisplay();

  ReturnOK();
}

int CursorPgDn(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
              int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result;

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextCursorOff();

  bAtLineFeed=0;                // ByHance, 95,12.14

  if(BoxIsTableBox(HBox))
  {
     TextBoxs *TextBox;

     TextBox=HandleLock(ItemGetHandle(HBox));
     if (TextBox==NULL)
        return(OUTOFMEMORY);
     *NewPosition=TextBoxGetVisualLength(TextBox);
     HandleUnlock(ItemGetHandle(HBox));
  }
  else
  {
     PosToBoxLineColumn(HBox,NewHBox,Position,&CursorY,&CursorX);
     BoxLineColumnToPos(HBox,NewHBox,CursorY+10,CursorX,NewPosition);
  }

  Result=*NewPosition-Position;
  if (Result<=0)
  {
     Alarm();
     // return(Result);
  }
  else
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }

  TextCursorDisplay();

  ReturnOK();
}

int CursorStoryHome(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                    int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result;

  if (BoxIsTableBox(HBox))
     return(0);

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextCursorOff();

  bAtLineFeed=0;                // ByHance, 95,12.14

  TextBoxSeekTextPosition(HBox,0,1,NewPosition);
  TextBoxSeekTextPosition(HBox,*NewPosition,-1,NewPosition);
  //BoxXYToPos(HBox,NewHBox,0,0,NewPosition);
                                       // ??? Box LeftTop
  //Result=TextBoxSeekTextPosition(HBox,Position,*NewPosition-Position,NewPosition);
  Result=Position-*NewPosition;
  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }
  else
  {
     Alarm();
     // return(Result);
  }

  TextCursorDisplay();

  ReturnOK();
}

int CursorStoryEnd(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                   int *BlockStart,int *BlockEnd)
{
  TextBoxs *TextBox;
  int CursorX,CursorY,Result;

  if (BoxIsTableBox(HBox))
     return(0);

  CancelBlock(HBox,BlockStart,BlockEnd);

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  TextCursorOff();

  if(bAtLineFeed)               // ByHance, 95,12.14
  {
     Position--;     // to prev mark
     bAtLineFeed=0;
  }

  Result=TextBoxSeekTextPosition(HBox,Position,TextBoxGetVisualLength(TextBox)-Position,NewPosition);
  if (Result<=0)
  {
     Alarm();
     // return(Result);
  }
  else
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
  }
  HandleUnlock(ItemGetHandle(HBox));

  TextCursorDisplay();

  ReturnOK();
}

/* TextBox SHIFT_Cursor Key Deal Procedure*/
#ifdef OLD
int CursorShiftLeft(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                    int Length,int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result,SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;
  bAtLineFeed=0;                // ByHance, 95,12.14

  Result=TextBoxSeekTextPosition(HBox,Position,-Length,NewPosition);
  if (BoxIsTableBox(HBox)
   &&TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell)
  {
     Result=GlobalTableBlockEnd;
     CancelBlock(HBox,BlockStart,BlockEnd);
     if (Result<0)
        GlobalTableBlockEnd=GlobalTableCell;
     else
        GlobalTableBlockEnd=Result;
     GlobalTableCell=GlobalTableBlockStart=
                     FBFindNextCell(HBox,GlobalTableCell,LEFTCELL);
     *BlockStart=*NewPosition=TableCellGetTextHead(HBox,GlobalTableBlockStart);
     *BlockEnd=TableCellGetTextHead(HBox,GlobalTableBlockEnd)+
               TableCellGetTextLength(HBox,GlobalTableBlockEnd);
     DisplayCellBlock(HBox,GlobalTableBlockStart,GlobalTableBlockEnd);
     ReturnOK();
  }

  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=*NewPosition;
        *BlockEnd=Position;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd-(Position-*NewPosition),*BlockEnd);
           *BlockEnd-=(Position-*NewPosition);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart-(Position-*NewPosition),*BlockStart);
           *BlockStart-=(Position-*NewPosition);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
  {
     *NewPosition=Position;
     Alarm();
  }

  if (Result!=Length)
     return(Result);
  ReturnOK();
}

int CursorShiftRight(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                    int Length,int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result,SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;

  bAtLineFeed=0;                // ByHance, 95,12.14

  Result=TextBoxSeekTextPosition(HBox,Position,Length,NewPosition);
  if (BoxIsTableBox(HBox)
  &&TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell)
  {
     Result=GlobalTableBlockStart;
     CancelBlock(HBox,BlockStart,BlockEnd);
     if (Result<0)
        GlobalTableBlockStart=GlobalTableCell;
     else
        GlobalTableBlockStart=Result;
     GlobalTableCell=GlobalTableBlockEnd=
                     FBFindNextCell(HBox,GlobalTableCell,RIGHTCELL);
     *BlockStart=TableCellGetTextHead(HBox,GlobalTableBlockStart);
     *BlockEnd=*NewPosition=TableCellGetTextHead(HBox,GlobalTableBlockEnd)+
               TableCellGetTextLength(HBox,GlobalTableBlockEnd);
     DisplayCellBlock(HBox,GlobalTableBlockStart,GlobalTableBlockEnd);
     ReturnOK();
  }

  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=Position;
        *BlockEnd=*NewPosition;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd,*BlockEnd+(*NewPosition-Position));
           *BlockEnd+=(*NewPosition-Position);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart,*BlockStart+(*NewPosition-Position));
           *BlockStart+=(*NewPosition-Position);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
  {
     *NewPosition=Position;
     Alarm();
  }

  if (Result!=Length)
     return(Result);
  ReturnOK();
}

int CursorShiftUp(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
             int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result,SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;

  bAtLineFeed=0;                // ByHance, 95,12.14

  BoxCursorPosDown(HBox,NewHBox,Position,NewPosition,-Length);
  if (BoxIsTableBox(HBox)
   &&TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell)
  {
     Result=GlobalTableBlockEnd;
     CancelBlock(HBox,BlockStart,BlockEnd);
     if (Result<0)
        GlobalTableBlockEnd=GlobalTableCell;
     else
        GlobalTableBlockEnd=Result;
     GlobalTableCell=GlobalTableBlockStart=
                     FBFindNextCell(HBox,GlobalTableCell,UPCELL);
     *BlockStart=*NewPosition=TableCellGetTextHead(HBox,GlobalTableBlockStart);
     *BlockEnd=TableCellGetTextHead(HBox,GlobalTableBlockEnd)+
               TableCellGetTextLength(HBox,GlobalTableBlockEnd);
     DisplayCellBlock(HBox,GlobalTableBlockStart,GlobalTableBlockEnd);
     GlobalTableCell=FBFindNextCell(HBox,GlobalTableCell,UPCELL);
     *NewPosition=TableCellGetTextHead(HBox,GlobalTableCell)+
                  TableCellGetTextLength(HBox,GlobalTableCell);
     ReturnOK();
  }

  if (Position>*NewPosition)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(*NewHBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=*NewPosition;
        *BlockEnd=Position;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd-(Position-*NewPosition),*BlockEnd);
           *BlockEnd-=(Position-*NewPosition);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart-(Position-*NewPosition),*BlockStart);
           *BlockStart-=(Position-*NewPosition);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
  {
     *NewPosition=Position;
     Alarm();
  }

  ReturnOK();
}

int CursorShiftDown(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
               int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result,SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;

  bAtLineFeed=0;                // ByHance, 95,12.14

  BoxCursorPosDown(HBox,NewHBox,Position,NewPosition,Length);
  if (BoxIsTableBox(HBox)
  &&TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell)
  {
     Result=GlobalTableBlockStart;
     CancelBlock(HBox,BlockStart,BlockEnd);
     if (Result<0)
        GlobalTableBlockStart=GlobalTableCell;
     else
        GlobalTableBlockStart=Result;
     GlobalTableCell=GlobalTableBlockEnd=
                     FBFindNextCell(HBox,GlobalTableCell,DOWNCELL);
     *BlockStart=TableCellGetTextHead(HBox,GlobalTableBlockStart);
     *BlockEnd=*NewPosition=TableCellGetTextHead(HBox,GlobalTableBlockEnd)+
               TableCellGetTextLength(HBox,GlobalTableBlockEnd);
     DisplayCellBlock(HBox,GlobalTableBlockStart,GlobalTableBlockEnd);
     ReturnOK();
  }

  if (*NewPosition>Position)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=Position;
        *BlockEnd=*NewPosition;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd,*BlockEnd+(*NewPosition-Position));
           *BlockEnd+=(*NewPosition-Position);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart,*BlockStart+(*NewPosition-Position));
           *BlockStart+=(*NewPosition-Position);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
  {
     *NewPosition=Position;     // DG in 1996,3.12
     Alarm();
  }

  ReturnOK();
}
#endif  // Old_version

int CursorShiftLeft(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                    int Length,int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result,SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;
  bAtLineFeed=0;                // ByHance, 95,12.14

  Result=TextBoxSeekTextPosition(HBox,Position,-Length,NewPosition);
  if( BoxIsTableBox(HBox)
  && TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell )
  {
     *NewPosition=Position;     // DG in 1996,3.12
     Alarm();
     ReturnOK();
  }

  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=*NewPosition;
        *BlockEnd=Position;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd-(Position-*NewPosition),*BlockEnd);
           *BlockEnd-=(Position-*NewPosition);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart-(Position-*NewPosition),*BlockStart);
           *BlockStart-=(Position-*NewPosition);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
     Alarm();

  if (Result!=Length)
     return(Result);
  ReturnOK();
}

int CursorShiftRight(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                    int Length,int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result,SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;

  bAtLineFeed=0;                // ByHance, 95,12.14

  Result=TextBoxSeekTextPosition(HBox,Position,Length,NewPosition);
  if( BoxIsTableBox(HBox)
  && TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell )
  {
     *NewPosition=Position;     // DG in 1996,3.12
     Alarm();
     ReturnOK();
  }

  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=Position;
        *BlockEnd=*NewPosition;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd,*BlockEnd+(*NewPosition-Position));
           *BlockEnd+=(*NewPosition-Position);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart,*BlockStart+(*NewPosition-Position));
           *BlockStart+=(*NewPosition-Position);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
     Alarm();

  if (Result!=Length)
     return(Result);
  ReturnOK();
}

int CursorShiftUp(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
             int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;
  bAtLineFeed=0;                // ByHance, 95,12.14

  BoxCursorPosDown(HBox,NewHBox,Position,NewPosition,-Length);
  if( BoxIsTableBox(HBox)
   &&TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell )
  {
     Alarm();
     *NewPosition=Position;
     ReturnOK();
  }

  if (Position>*NewPosition)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(*NewHBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=*NewPosition;
        *BlockEnd=Position;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd-(Position-*NewPosition),*BlockEnd);
           *BlockEnd-=(Position-*NewPosition);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart-(Position-*NewPosition),*BlockStart);
           *BlockStart-=(Position-*NewPosition);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
  {
     *NewPosition=Position;     // DG in 1996,3.12
     Alarm();
  }

  ReturnOK();
}

int CursorShiftDown(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
               int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;
  bAtLineFeed=0;                // ByHance, 95,12.14

  BoxCursorPosDown(HBox,NewHBox,Position,NewPosition,Length);
  if( BoxIsTableBox(HBox)
   &&TableTextGetCellNumber(HBox,*NewPosition)!=GlobalTableCell )
  {
     *NewPosition=Position;
     Alarm();
     ReturnOK();
  }

  if (*NewPosition>Position)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=Position;
        *BlockEnd=*NewPosition;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd,*BlockEnd+(*NewPosition-Position));
           *BlockEnd+=(*NewPosition-Position);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart,*BlockStart+(*NewPosition-Position));
           *BlockStart+=(*NewPosition-Position);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
  {
     *NewPosition=Position;     // DG in 1996,3.12
     Alarm();
  }

  ReturnOK();
}

int CursorShiftHome(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
               int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result,SaveUndoNumber;

  if(bAtLineFeed)                // ByHance, 95,12.14
     return(CursorShiftUp(HBox,NewHBox,Position,NewPosition,1,BlockStart,BlockEnd) );

  SaveUndoNumber=UndoOperateSum;
  PosToBoxLineColumn(HBox,NewHBox,Position,&CursorY,&CursorX);
  BoxLineColumnToPos(HBox,NewHBox,CursorY,0,NewPosition);

  Result=Position-*NewPosition;
  //Result=TextBoxSeekTextPosition(HBox,Position,*NewPosition-Position,NewPosition);
  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);

     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=*NewPosition;
        *BlockEnd=Position;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd-(Position-*NewPosition),*BlockEnd);
           *BlockEnd-=(Position-*NewPosition);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart-(Position-*NewPosition),*BlockStart);
           *BlockStart-=(Position-*NewPosition);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
     Alarm();

  ReturnOK();
}

int CursorShiftEnd(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
              int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result,SaveUndoNumber;

  if(bAtLineFeed)               // ByHance, 95,12.14
  {
     Alarm();
     return 0;
  }

  SaveUndoNumber=UndoOperateSum;
  PosToBoxLineColumn(HBox,NewHBox,Position,&CursorY,&CursorX);
  BoxLineColumnToPos(HBox,NewHBox,CursorY+1,0,NewPosition);

  //(*NewPosition)--;                      // ??? Has LF
  {
    TextBoxs *TextBox;

    TextBox=HandleLock(ItemGetHandle(HBox));
    if (TextBox==NULL)
       return(OUTOFMEMORY);

    if (TextBoxGetBoxType(TextBox)==TABLEBOX)   // ByHance, 97,3.20
    {
        int txt_len,start_pos;
        start_pos=TableCellGetTextHead(HBox,GlobalTableCell);
        txt_len=TableCellGetTextLength(HBox,GlobalTableCell);
        if(*NewPosition>start_pos+txt_len)
           *NewPosition=start_pos+txt_len;
    }
    else
    if (*NewPosition>=TextBoxGetTextLength(TextBox))
       *NewPosition=TextBoxGetTextLength(TextBox)-1;
//       *NewPosition=TextBoxGetTextLength(TextBox);   // ByHance
    HandleUnlock(ItemGetHandle(HBox));
  }
  Result=*NewPosition-Position;
  //Result=TextBoxSeekTextPosition(HBox,*NewPosition,-1,NewPosition);
//  if (Result>0||Position==*NewPosition)        // ByHance
  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=Position;
        *BlockEnd=*NewPosition;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd,*BlockEnd+(*NewPosition-Position));
           *BlockEnd+=(*NewPosition-Position);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart,*BlockStart+(*NewPosition-Position));
           *BlockStart+=(*NewPosition-Position);
        }
        if (*BlockStart>*BlockEnd)
        {
           int Tmp;
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
     Alarm();

  ReturnOK();
}

int CursorShiftStoryHome(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                    int *BlockStart,int *BlockEnd)
{
  int CursorX,CursorY,Result,SaveUndoNumber;

  if (BoxIsTableBox(HBox))
     return(0);
  SaveUndoNumber=UndoOperateSum;

  bAtLineFeed=0;                // ByHance, 95,12.14

  TextBoxSeekTextPosition(HBox,0,1,NewPosition);
  TextBoxSeekTextPosition(HBox,*NewPosition,-1,NewPosition);
  //BoxXYToPos(HBox,NewHBox,0,0,NewPosition);
                                       // ??? Box LeftTop
  //Result=TextBoxSeekTextPosition(HBox,Position,*NewPosition-Position,NewPosition);
  Result=Position-*NewPosition;
  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=*NewPosition;
        *BlockEnd=Position;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd-(Position-*NewPosition),*BlockEnd);
           *BlockEnd-=(Position-*NewPosition);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart-(Position-*NewPosition),*BlockStart);
           *BlockStart-=(Position-*NewPosition);
        }
        if (*BlockStart>*BlockEnd)
        {
           int Tmp;
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
     Alarm();
  ReturnOK();
}

int CursorShiftStoryEnd(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                        int *BlockStart,int *BlockEnd)
{
  TextBoxs *TextBox;
  int CursorX,CursorY,Result,SaveUndoNumber;

  if (BoxIsTableBox(HBox))
     return(0);
  SaveUndoNumber=UndoOperateSum;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  bAtLineFeed=0;                // ByHance, 95,12.14

  Result=TextBoxSeekTextPosition(HBox,Position,TextBoxGetVisualLength(TextBox)-Position,NewPosition);
  if (Result>0)
  {
     UndoInsertCursorGoto(Position);
     CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);
     if (*BlockStart>=*BlockEnd)
     {
        UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
        *BlockStart=Position;
        *BlockEnd=*NewPosition;
        DisplayBlock(*NewHBox,*BlockStart,*BlockEnd);
     }
     else
     {
        int Tmp;

        if (Position>*BlockStart)
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockEnd,*BlockEnd+(*NewPosition-Position));
           *BlockEnd+=(*NewPosition-Position);
        }
        else
        {
           UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
           DisplayBlock(*NewHBox,*BlockStart,*BlockStart+(*NewPosition-Position));
           *BlockStart+=(*NewPosition-Position);
        }
        if (*BlockStart>*BlockEnd)
        {
           Tmp=*BlockStart;
           *BlockStart=*BlockEnd;
           *BlockEnd=Tmp;
        }
     }
     UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  }
  else
     Alarm();
  HandleUnlock(ItemGetHandle(HBox));
  ReturnOK();
}

/* TextBox Delete Key Deal Procedure */

int TextBoxDel(HBOX HBox,int *NewPosition,int *BlockStart,int *BlockEnd)
{
  TextStyles BlockTextStyle;
  unsigned char CharSizeModifySign,CharFontModifySign,
                CharSlantModifySign,CharHSizeModifySign,
                CharColorModifySign,SuperscriptModifySign,
                SubscriptModifySign;
  TextBoxs *TextBox;
  Wchar *TextBlock;
  unsigned short CharAttributeString[6];
  int i;

  if (*BlockStart<*BlockEnd)
  {
     TextBox=HandleLock(ItemGetHandle(HBox));
     if (TextBox==NULL)
        return(OUTOFMEMORY);
     TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
     if (TextBlock==NULL)
     {
        HandleUnlock(ItemGetHandle(HBox));
        return(OUTOFMEMORY);
     }

     CharSizeModifySign=CharFontModifySign=CharSlantModifySign
     =CharColorModifySign=CharHSizeModifySign=SuperscriptModifySign
     =SubscriptModifySign=0;

     *NewPosition=*BlockStart;
     UndoInsertCursorDefineBlock(*BlockStart,*BlockEnd);
     for (i=*BlockStart;i<*BlockEnd;i++)
     {                                 /*
                                          When delete block data, must save
                                          their char attribute information
                                        */
         switch (GetPreCode(TextBlock[i]))
         {
           case CHARSIZE:
                CharSizeModifySign=1;
                BlockTextStyle.CharSize=GetAttribute(TextBlock[i]);
                break;
           case CHARFONT:
                CharFontModifySign=1;
                BlockTextStyle.CharFont=GetAttribute(TextBlock[i]);
                break;
           case CHARSLANT:
                CharSlantModifySign=1;
                BlockTextStyle.CharSlant=GetAttribute(TextBlock[i]);
                break;
           case SUPERSCRIPT:
                SuperscriptModifySign=1;
                BlockTextStyle.Superscript=GetAttribute(TextBlock[i]);
                break;
           case SUBSCRIPT:
                SubscriptModifySign=1;
                BlockTextStyle.Subscript=GetAttribute(TextBlock[i]);
                break;
           case CHARHSIZE:
                CharHSizeModifySign=1;
                BlockTextStyle.CharHSize=GetAttribute(TextBlock[i]);
                break;
           case CHARCOLOR:
                CharColorModifySign=1;
                BlockTextStyle.CharColor=GetAttribute(TextBlock[i]);
                break;
         }
     }
     HandleUnlock(TextBoxGetTextHandle(TextBox));
     HandleUnlock(ItemGetHandle(HBox));
     DisplayBlock(HBox,*BlockStart,*BlockEnd);
     TextBoxDeleteString(HBox,*NewPosition,(*BlockEnd)-(*NewPosition));
     *BlockEnd=*BlockStart=-1;
     i=0;
     if (CharSizeModifySign)
        CharAttributeString[i++]=MakeCHARSIZE(BlockTextStyle.CharSize);
     if (CharFontModifySign)
        CharAttributeString[i++]=MakeCHARFONT(BlockTextStyle.CharFont);
     if (CharSlantModifySign)
        CharAttributeString[i++]=MakeCHARSLANT(BlockTextStyle.CharSlant);
     if (SuperscriptModifySign)
        CharAttributeString[i++]=MakeSUPERSCRIPT(BlockTextStyle.Superscript);
     if (SubscriptModifySign)
        CharAttributeString[i++]=MakeSUBSCRIPT(BlockTextStyle.Subscript);
     if (CharHSizeModifySign)
        CharAttributeString[i++]=MakeCHARHSIZE(BlockTextStyle.CharHSize);
     if (CharColorModifySign)
        CharAttributeString[i++]=MakeCHARCOLOR(BlockTextStyle.CharColor);
     if (i)
     {
        i=TextBoxInsertString(HBox,*NewPosition,CharAttributeString,i);
        *NewPosition+=i;
     }
     return(*BlockEnd-*BlockStart-i);
  }
  else
     return(0);
}

int TextBoxDelKey(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
                  int *BlockStart,int *BlockEnd)
{
  int DeleteResult,CursorX,CursorY,StartChangeLine,ChangeLines;
  int SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;
  UndoInsertCursorGoto(GlobalTextPosition);
  bAtLineFeed=0;                // ByHance, 95,12.14

  if (*BlockStart<*BlockEnd)
  {
     int BlockSaveLength;

     Position=*BlockStart;
     BlockSaveLength=*BlockEnd-*BlockStart;
     DeleteResult=TextBoxDel(HBox,NewPosition,BlockStart,BlockEnd);
     FormatChangeText(HBox,Position,DeleteResult,BlockSaveLength,
                      &StartChangeLine,&ChangeLines, FALSE);
  }
  else
  {
     // Delete String
     Length=TextBoxSeekTextPosition(HBox,Position,Length,NewPosition);
     if( Length<=0
     || (BoxIsTableBox(HBox)&&TextPositionChar(HBox,Position)==TAB))
     {
        Alarm();
        return(0);
     }
     DeleteResult=TextBoxDeleteString(HBox,Position,Length);
     TextBoxSeekTextPosition(HBox,Position,1,NewPosition);
     TextBoxSeekTextPosition(HBox,*NewPosition,-1,NewPosition);
     FormatDeleteText(HBox,*NewPosition,DeleteResult,&StartChangeLine,&ChangeLines, FALSE);
  }

  if (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
     TextBoxRedraw(*NewHBox,StartChangeLine,ChangeLines, FALSE);
  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  ReturnOK();
}

int TextBoxBackSpace(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                     int Length,int *BlockStart,int *BlockEnd)
{
  int DeleteResult,CursorX,CursorY,StartChangeLine,ChangeLines;
  int SaveUndoNumber;

  bAtLineFeed=0;                // ByHance, 95,12.14
  if (*BlockStart<*BlockEnd)
     return(TextBoxDelKey(HBox,NewHBox,Position,NewPosition,Length,
            BlockStart,BlockEnd));

  SaveUndoNumber=UndoOperateSum;

  Length=TextBoxSeekTextPosition(HBox,Position,-Length,NewPosition);
  if( Length<=0
  || (BoxIsTableBox(HBox)&&TextPositionChar(HBox,*NewPosition)==TAB))
  {
     Alarm();
     return(0);
  }

  UndoInsertCursorGoto(GlobalTextPosition);
  DeleteResult=TextBoxDeleteString(HBox,*NewPosition,Length);
  FormatDeleteText(HBox,*NewPosition,DeleteResult,&StartChangeLine,&ChangeLines,FALSE);

  if (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
        TextBoxRedraw(*NewHBox,StartChangeLine,ChangeLines, FALSE);
  if (!DeleteResult)
        return(0);
  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  ReturnOK();
}

int TextBoxEnterKey(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
               int *BlockStart,int *BlockEnd,Wchar *KeyString)
{
  int Result,CursorX,CursorY,StartChangeLine,ChangeLines;
  int SaveUndoNumber,InsLen;

  if (BoxIsTableBox(HBox))
  {
      return(TextBoxKey(HBox,NewHBox,Position,NewPosition,Length,
               BlockStart,BlockEnd,KeyString));
  }

  SaveUndoNumber=UndoOperateSum;
  UndoInsertCursorGoto(Position);

#ifdef OLD_VERSION
  if (*BlockStart<*BlockEnd)
  {                        /* Position == BlockStart or BlockEnd */
     int BlockSaveLength;

     BlockSaveLength=TextBoxDelKey(HBox,NewHBox,Position,NewPosition,Length,
                                   BlockStart,BlockEnd);
     Result=TextBoxInsertString(*NewHBox,*NewPosition,KeyString,Length);
     if (Result>0)
        *NewPosition+=Result;
  }
  else
#else
  CancelBlock(HBox,BlockStart,BlockEnd);
#endif

  {
     bAtLineFeed=0;                // ByHance, 95,12.14

     Result=TextBoxInsertString(HBox,Position,KeyString,Length);
     if (Result>0)
        *NewPosition=Position+Result;
     else
     {
        *NewPosition=Position;
        return Result;
     }
  }
  InsLen=Result;

  if(!fEditor)
  {     //-- ByHance, 96,5.6 ---for paragrah's FirstSpace ---
     Wchar InsStr[3];
     char  attribute=TextSearchAttribute(HBox,Position,PARAGRAPHALIGN,&Result);
     if(attribute==ALIGNLEFT || attribute==ALIGNLEFTRIGHT)
     {
         InsStr[0]=InsStr[1]=0xa1a1;
         Result=TextBoxInsertString(HBox,*NewPosition,InsStr,2);
         if (Result>0)
         {
            *NewPosition+=Result;
            InsLen+=Result;
         }
     }
  }

  FormatInsertText(HBox,Position,InsLen,&StartChangeLine,&ChangeLines, FALSE);

  if (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
     TextBoxRedraw(*NewHBox,StartChangeLine,ChangeLines,FALSE);

  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  return(InsLen);
}

int TextBoxKey(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,int Length,
               int *BlockStart,int *BlockEnd,Wchar *KeyString)
{
  int Result,CursorX,CursorY,StartChangeLine,ChangeLines;
  int SaveUndoNumber;
  int BlockSaveLength;

  SaveUndoNumber=UndoOperateSum;
  UndoInsertCursorGoto(Position);

  if (BoxIsTableBox(HBox))
  {
    {           // ByDG, 96,4.12
     TextBoxs *TextBox;
     int BoxBottom,PageBottom;
     Pages *MidPage;

         TextBox=HandleLock(ItemGetHandle(HBox));
         if (TextBox==NULL)
           return(OUTOFMEMORY);
         BoxBottom=TextBoxGetBoxBottom(TextBox);
         HandleUnlock(ItemGetHandle(HBox));

         MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
         if (MidPage==NULL)
           return(OUTOFMEMORY);
         PageBottom=PageGetPageHeight(MidPage);
         HandleUnlock(ItemGetHandle(GlobalCurrentPage));

         if (BoxBottom>=PageBottom)
         {
             Alarm();
             return (0);
         }
    }
    {           // DG Add in 1996,4.13        Control Font Size
        int CharWidth;
        CharWidth=TextSearchAttribute(HBox,Position,CHARHSIZE,&Result);
        if(!IsCellWidthValid(HBox,CharWidth))
            return(0);
    } /*------ end -----*/

    if (*BlockStart<*BlockEnd)
    {                   /* Position = BlockStart or BlockEnd */
       BlockSaveLength=TextBoxDelKey(HBox,NewHBox,Position,NewPosition,Length,
                                     BlockStart,BlockEnd);
       Position=*NewPosition;           // ByHance,97,5.6
       Result=TextBoxInsertString(*NewHBox,*NewPosition,KeyString,Length);
       if(Result>0)                     // ByHance,97,5.6
          *NewPosition += Result;
    }
    else
    {
       bAtLineFeed=0;                // ByHance, 95,12.14
       Result=TextBoxInsertString(HBox,Position,KeyString,Length);
       if (Result>0)
          *NewPosition=Position+Result;
       else
          *NewPosition=Position;
    }
    FormatInsertText(HBox,Position,Result,&StartChangeLine,&ChangeLines,FALSE);

    AdjustTableCells(HBox);     // ByHance, 96,4.8

    if (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY))
       TextBoxRedraw(*NewHBox,StartChangeLine,ChangeLines,FALSE);
  }
  else {
    if (*BlockStart<*BlockEnd)
    {                                    /* Must Position = BlockStart or BlockEnd */
       BlockSaveLength=TextBoxDelKey(HBox,NewHBox,Position,NewPosition,Length,
                                     BlockStart,BlockEnd);
    lbl_only_insert:
       Result=TextBoxInsertString(*NewHBox,*NewPosition,KeyString,Length);
       if (Result>0)
          *NewPosition+=Result;

       FormatInsertText(HBox,Position,Result,&StartChangeLine,&ChangeLines, TRUE);
       if (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
          TextBoxRedraw(*NewHBox,StartChangeLine,ChangeLines, FALSE);  // Old
    }
    else
    {
       int SaveCursorX, SaveCursorY;
       int SaveStartChangeLine, SaveChangeLines;
       int DeleteResult;
       int LastCursorX=0xff00, LastCursorY=0xff00, LastPosition;
       BOOL bReturn=FALSE;

       bAtLineFeed=0;
       if(Length>20)                  // ByHance, 96,4.3
       {        // may be inset file
            *NewPosition=Position;
            goto lbl_only_insert;
       }

       //CursorLocate(HBox,NewHBox,Position,&SaveCursorX,&SaveCursorY);
       PosToBoxCursorXY(HBox,NewHBox,Position,&SaveCursorX,&SaveCursorY);
       for (LastPosition=Position; LastPosition>0; LastPosition--) {
          PosToBoxCursorXY(HBox,NewHBox,LastPosition,&LastCursorX,&LastCursorY);
          if(HBox!=*NewHBox)    // ByHance, 96,4.5
             break;
          if (LastCursorY<SaveCursorY) {
             bReturn=TRUE;
             break;
          }
       }

      /*----------   ByHance, 96,3.1
       if(bReturn)
          PosToBoxCursorXY(HBox,NewHBox,LastPosition,&LastCursorX,&LastCursorY);
       -------------------*/

       Result=TextBoxInsertString(HBox,Position,KeyString,Length);
       if (Result>0)
          *NewPosition=Position+Result;
       else
          *NewPosition=Position;

       FormatInsertText(HBox,Position,Result,&SaveStartChangeLine,&SaveChangeLines,TRUE);

       DoUndoSign=1;
       //CursorLocate(HBox,NewHBox,Position,&CursorX,&CursorY);
       PosToBoxCursorXY(HBox,NewHBox,Position,&CursorX,&CursorY);
       //SaveCursorX/=GlobalPageScale;
       SaveCursorX = myUserXToWindowX(SaveCursorX);
       //SaveCursorY/=GlobalPageScale;
       SaveCursorY = myUserYToWindowY(SaveCursorY);

       //CursorX/=GlobalPageScale;
       CursorX = myUserXToWindowX(CursorX);
       //CursorY/=GlobalPageScale;
       CursorY = myUserYToWindowY(CursorY);

       if (abs(CursorX-SaveCursorX)<=REDRAWDELTA &&
           (CursorY==SaveCursorY))   // Position Not Change
       {
          // Delete String & Format & UnDraw
          DeleteResult=TextBoxDeleteString(HBox,Position,Length);
          FormatDeleteText(HBox,Position,DeleteResult,&StartChangeLine,&ChangeLines, TRUE);
          TextBoxRedrawPart(HBox,SaveStartChangeLine,SaveChangeLines, Position, TRUE);

          // Add String & Format & Redraw
          Result=TextBoxInsertString(HBox,Position,KeyString,Length);
          if (Result>0)
             *NewPosition=Position+Result;
          else
             *NewPosition=Position;
          FormatInsertText(HBox,Position,Result,&StartChangeLine,&ChangeLines, TRUE);
          if  (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
             TextBoxRedrawPart(HBox,StartChangeLine,ChangeLines, Position, FALSE);
       }
       else
       if(bReturn)
       {   /*----- Last Line is changed ? ByHance, 96,1.23 --------*/
           PosToBoxCursorXY(HBox,NewHBox,LastPosition,&CursorX,&CursorY);
           //LastCursorX/=GlobalPageScale;
           //LastCursorY/=GlobalPageScale;
           LastCursorX = myUserXToWindowX(LastCursorX);
           LastCursorY = myUserYToWindowY(LastCursorY);
           //CursorX/=GlobalPageScale;
           //CursorY/=GlobalPageScale;
           CursorX = myUserXToWindowX(CursorX);
           CursorY = myUserYToWindowY(CursorY);
           if (abs(CursorX-LastCursorX)<=REDRAWDELTA &&
               (CursorY==LastCursorY))   // Position Not Change
           {                      /*--- No! -----*/
                  // Delete String & Format & UnDraw
                  DeleteResult=TextBoxDeleteString(HBox,Position,Length);
                  FormatDeleteText(HBox,Position,DeleteResult,&StartChangeLine,&ChangeLines, TRUE);
                  TextBoxRedrawPart(HBox,SaveStartChangeLine,SaveChangeLines, LastPosition, TRUE);

                  // Add String & Format & Redraw
                  Result=TextBoxInsertString(HBox,Position,KeyString,Length);
                  if (Result>0)
                          *NewPosition=Position+Result;
                  else
                          *NewPosition=Position;

                  FormatInsertText(HBox,Position,Result,&StartChangeLine,&ChangeLines, TRUE);
                  if  (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
                          TextBoxRedrawPart(HBox,StartChangeLine,ChangeLines, LastPosition, FALSE);
           }
           else
           {                     /* Yes, redraw Last line */
           lbl_use_old_method:
              if (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
              {
                 DeleteResult=TextBoxDeleteString(HBox,Position,Length);
                 FormatDeleteText(HBox,Position,DeleteResult,&StartChangeLine,&ChangeLines, TRUE);
                 Result=TextBoxInsertString(HBox,Position,KeyString,Length);
                 FormatInsertText(HBox,Position,Result,&StartChangeLine,&ChangeLines, FALSE);
                 TextBoxRedraw(*NewHBox,StartChangeLine,ChangeLines, FALSE);    // Dg Add End
              }
           }
       }
       else
          goto lbl_use_old_method;
       DoUndoSign=0;
    }
  }

  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  return(Result);
}

int TextBoxOverwriteKey(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                        int Length,int *BlockStart,int *BlockEnd,
                        Wchar *KeyString)
{
  int Result,CursorX,CursorY,StartChangeLine,ChangeLines;
  int BlockSaveLength,i;
  int SaveUndoNumber;
  TextBoxs *TextBox;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  if(Position==TextBoxGetTextLength(TextBox)-1)  // when at text end, == insert
  {
     HandleUnlock(ItemGetHandle(HBox));
     return( TextBoxKey(HBox,NewHBox,Position,NewPosition,Length,
                        BlockStart,BlockEnd,KeyString)
           );
   }

  SaveUndoNumber=UndoOperateSum;
  UndoInsertCursorGoto(Position);

  if (*BlockStart<*BlockEnd)
  {                                    /* Must Position = BlockStart or BlockEnd */
     BlockSaveLength=TextBoxDelKey(HBox,NewHBox,Position,NewPosition,Length,
                                   BlockStart,BlockEnd);
  }
  else
     BlockSaveLength=0;

  bAtLineFeed=0;                // ByHance, 95,12.14

  *NewPosition=Position;
  for (i=0;i<Length;i++)
  {
     if (BoxIsTableBox(HBox)&&TextPositionChar(HBox,*NewPosition)==TAB)
        break;
      Result=TextBoxExchangeString(HBox,*NewPosition,&KeyString[i],1,1);
      if (!Result)
         break;
      Result=TextBoxSeekTextPosition(HBox,*NewPosition,1,NewPosition);
      if (Result<=0)
         break;
  }

  FormatChangeText(HBox,Position,*NewPosition-Position,*NewPosition-Position+
                   BlockSaveLength,&StartChangeLine,&ChangeLines, FALSE);
  if (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
     TextBoxRedraw(*NewHBox,StartChangeLine,ChangeLines, FALSE);
  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  return(Result);
}

int TextBoxInsertBox(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                     int *BlockStart,int *BlockEnd,HBOX InsertHBox)
{
  unsigned short InsertString[2];
  int Result,CursorX,CursorY,StartChangeLine,ChangeLines;
  int SaveUndoNumber;

  SaveUndoNumber=UndoOperateSum;
  UndoInsertCursorGoto(Position);

  InsertString[0]=MakeINSERTBOX(InsertHBox);
  InsertString[1]=0;
  if (*BlockStart<*BlockEnd)
  {                                    /* Must Position = BlockStart or BlockEnd */
     int BlockSaveLength;

     BlockSaveLength=TextBoxDelKey(HBox,NewHBox,Position,NewPosition,1,
                                   BlockStart,BlockEnd);
     Result=TextBoxInsertString(*NewHBox,*NewPosition,InsertString,1);
     FormatChangeText(HBox,Position,Result,BlockSaveLength,
                      &StartChangeLine,&ChangeLines, FALSE);
  }
  else
  {
     Result=TextBoxInsertString(HBox,Position,InsertString,1);
     if (Result>0)
        *NewPosition=Position+Result;
     else
        *NewPosition=Position;
     FormatInsertText(HBox,Position,Result,&StartChangeLine,&ChangeLines, FALSE);
  }
  if (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
     TextBoxRedraw(*NewHBox,StartChangeLine,ChangeLines, FALSE);

  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  return(Result);
}

int TextBoxTABKey(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                  int *BlockStart,int *BlockEnd)
{
  int Result,CursorX,CursorY;

  if (BoxIsTableBox(HBox))
  {
     Result=FBFindNextCell(HBox,GlobalTableCell,RIGHTCELL);
     if (Result>=0)
     {
        CancelBlock(HBox,BlockStart,BlockEnd);

        TextCursorOff();

        if (GlobalTableCell!=Result) GlobalTableCell=Result;
          else GlobalTableCell=0;

        *NewPosition=TableCellGetTextHead(HBox,GlobalTableCell);
        CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);

        TextCursorDisplay();
     }
     return(Result);        // return(Result+1);  By DG
  }
  return(0);
}

int TextBoxShiftTabKey(HBOX HBox,HBOX *NewHBox,int Position,int *NewPosition,
                  int *BlockStart,int *BlockEnd)
{
  int Result,CursorX,CursorY;

  if (BoxIsTableBox(HBox))
  {
     Result=FBFindNextCell(HBox,GlobalTableCell,LEFTCELL);
     if (Result>=0)
     {
        CancelBlock(HBox,BlockStart,BlockEnd);
        TextCursorOff();

        GlobalTableCell=Result;
        *NewPosition=TableCellGetTextHead(HBox,GlobalTableCell);
        CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY);

        TextCursorDisplay();
     }
     return(Result+1);
  }
  return(0);
}

int calculate(HBOX HBox,int Position,int *NewPosition,int fPerson)
{
  int CursorX,CursorY,Result,len,bEqu;
  int StartChangeLine,ChangeLines,i;
  HBOX NewHBox;
  char str[512],ch,*p;
  Wchar code;
  Wchar TextString[512];
  double val;
  int SaveUndoNumber;

  if(GlobalTextBlockEnd>GlobalTextBlockStart)
  {
     TextBoxs *TextBox;
     Wchar *TextBlock;
     int k;

     TextBox=HandleLock(ItemGetHandle(HBox));
     if (TextBox==NULL)
        return(OUTOFMEMORY);
     TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
     if (TextBlock==NULL)
     {
        HandleUnlock(ItemGetHandle(HBox));
        return(OUTOFMEMORY);
     }

     k=GlobalTextBlockStart;
     while(TextBlock[k]<=BLANK || TextBlock[k]==0xa1a1) k++;

     i=0;
     for(;k<GlobalTextBlockEnd;k++)
     {
         Wchar code;
         code=TextBlock[k];
         if(code<BLANK)
             continue;

         if(code==0xa1a1)
             code=BLANK;
         else
         if(code>=0xa000 || i>510)
         {
             HandleUnlock(TextBoxGetTextHandle(TextBox));
             HandleUnlock(ItemGetHandle(HBox));
             return -1;
         }
         else
         if( (code&ATTRIBUTEPRECODE)!=HIGHENGLISHCHAR )
             continue;

         str[i++]=code;
     }

     HandleUnlock(TextBoxGetTextHandle(TextBox));
     HandleUnlock(ItemGetHandle(HBox));
     if(i==0) return -1;
     str[i]=0;
  }
  else    /*- no block -*/
  if(GetLineText(HBox,Position,str))
     return -1;


 /*---- ignore BLANK at string tail ----*/
  len=strlen(str);
  while(str[len-1]==' ') len--;

  bEqu=0;
  if(str[len-1]=='=') { len--; bEqu=1; }
  str[len]=0;

 /*---- check str is valid string ----*/
  i=len=0;
  while((ch=str[len]))
  {
     if(ch==BLANK)
     {
         if(i==0) i=1;
     }
     else
     if( (ch>='0' && ch<='9') || ch=='.' )
     {
         if(i==1) str[len-1]='+';
         i=0;
     }
     else
         i=2;

     len++;
  } /*-- while --*/

  // Result=evaluate(str, &val);
  Result=get_exp(str, &val);
  if(Result!=0)
      return(Result);

  p=&str[0];
  ch=str[len-1];
 //  if(ch!='=')
  if(fPerson==1)
  {
      val -= 840;

      if(val>=100000) val=(val*0.45) - 15375;
      else
      if(val>=80000) val=(val*0.40) - 10375;
      else
      if(val>=60000) val=(val*0.35) - 6375;
      else
      if(val>=40000) val=(val*0.30) - 3375;
      else
      if(val>=20000) val=(val*0.25) - 1375;
      else
      if(val>= 5000) val=(val*0.20) - 375;
      else
      if(val>= 2000) val=(val*0.15) - 125;
      else
      if(val>=  500) val=(val*0.10) - 25;
      else
      if(val>0) val=(val*0.05) - 0;
      else
         val=0;

      sprintf(p,"\n个人所得税=%.2f元",val);       /* only 2 floating */
  }
  else
  if(fPerson==2)
  {
      double r17,r6;
      static unsigned char fmt[]="\n税率为%d%%时的销售额=%.2f  税额=%.2f";
      r17=val/1.17;       /* only 2 floating */
      r6=val/1.06;
      sprintf(p,fmt,17, r17, val-r17);
      p += strlen(p);
      sprintf(p,fmt, 6,  r6, val-r6  );
  }
  else
  {
      if(!bEqu)
         *p++= '=';

      //sprintf(p,"%.3g",val);       /* float, divide, using floating */
      sprintf(p,"%.4f",val);       /* float, divide, using floating */
  }

  len=strlen(str)-1;
  while(str[len]!='.')
  {
     if(str[len]!='0')
        break;
     len--;
  }

  if(str[len]=='.')  str[len]=0;     // only int_number
  else str[len+1]=0;         // trunc 0_string

 /*--------
  MakeWchar(str, TextString);
  len=Wstrlen(TextString);
  ------------*/
  i=len=0;
  while((code=str[i++]))
  {
     if(code>0xa0) code=(code<<8)|str[i++];
     TextString[len++]=code;
  }

  SaveUndoNumber=UndoOperateSum;
  UndoInsertCursorGoto(Position);
  if(GlobalTextBlockEnd>GlobalTextBlockStart)
  {
     *NewPosition=GlobalTextBlockEnd;
     CancelBlock(HBox,&GlobalTextBlockStart,&GlobalTextBlockEnd);
  }
  else
     PosToBoxRowEnd(HBox,Position,NewPosition,&CursorY,&CursorX);

  len=TextBoxInsertString(HBox,*NewPosition,TextString,len);

  FormatInsertText(HBox,*NewPosition,len,&StartChangeLine,&ChangeLines,FALSE);
  *NewPosition+=len;
  if(!CursorLocate(HBox,&NewHBox,*NewPosition,&CursorX,&CursorY))
      TextBoxRedraw(HBox,StartChangeLine,ChangeLines, FALSE);

  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  ReturnOK();
}

void AdjustCtrl_KB_pos(HBOX HBox,int Position,int *NewPosition)
{
  bAtLineFeed=0;
  TextBoxSeekTextPosition(HBox,Position,1,NewPosition);  // move right
  TextBoxSeekTextPosition(HBox,*NewPosition,-1,NewPosition); // move left
}

void AdjustCtrl_KK_pos(HBOX HBox,int Position,int *NewPosition)
{
  if(bAtLineFeed)
  {
     Position--;     // to prev mark
     bAtLineFeed=0;
  }

  TextBoxSeekTextPosition(HBox,Position,-1,NewPosition);   // move left
  TextBoxSeekTextPosition(HBox,*NewPosition,1,NewPosition); // move right
}

long TextBoxGetRemainSize(HBOX HBox)
{
  TextBoxs *TextBox;
  long ReturnLength;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  ReturnLength=MAXBUFFERLENGTH-TextBoxGetTextLength(TextBox);
  HandleUnlock(ItemGetHandle(HBox));
  return(ReturnLength);
}

////////////////////By zjh for Dbase ///////////////////////////
int TableBoxInsertText(HBOX HBox,int iCell,char *buff,BOOL fErase)
{
  long BoxTextSize;
  #define  MAXFIELDLEN    10000
  #define  MaxReadLen     0x4000       // can't be too large, for stack
  Wchar MidTextBlock[MaxReadLen],ReadChar;
  long i,j,Result;
  int StartChangeLine,ChangeLines,MidReadChar;
  int SaveUndoNumber;
  int Position,TextLength;

  Position=TableCellGetTextHead(HBox,iCell);

  SaveUndoNumber=UndoOperateSum;
  UndoInsertCursorGoto(Position);

  if (fErase)
  {
    TextLength=TableCellGetTextLength(HBox,iCell);
    Result=TextBoxDeleteString(HBox,Position,TextLength);
    FormatDeleteText(HBox,Position,Result,&StartChangeLine,&ChangeLines,FALSE);
  }

  BoxTextSize=TextBoxGetRemainSize(HBox);

  i=j=0;

  while (*buff&&i<MAXFIELDLEN&&j<BoxTextSize)
    {
       MidReadChar=*buff++;
       i++;

       if (MidReadChar<0x20 && MidReadChar!=ENTER)
           continue;
       if(MidReadChar>=0x7f && MidReadChar<=0xa0)
           continue;
       ReadChar=MidReadChar;
       if (ReadChar>0xa0)
           {
            MidReadChar=*buff++;
            i++;
            if (!MidReadChar) break;
            if (MidReadChar<=0xa0) continue;

            ReadChar<<=8;
            ReadChar|=MidReadChar;
           }

       MidTextBlock[j++]=ReadChar;
    }

    MidTextBlock[j]=0;
    Result=TextBoxInsertString(HBox,Position,MidTextBlock,j);
    if (Result<j)  Alarm();

    FormatInsertText(HBox,Position,Result,&StartChangeLine,&ChangeLines, FALSE);

    UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
    return(Result);
}


#if  1
int TextBoxInsertTextFile(char *FileName,HBOX HBox,int *NewHBox,int Position,
                          int *NewPosition,int *BlockStart,int *BlockEnd)
{
  FILE *TextFp;
  int WpsVer;
  long FileSize,BoxTextSize;
#define  MaxReadLen     0x4000       // can't be too large, for stack
  Wchar MidTextBlock[MaxReadLen],ReadChar;
  long i,j,Result,total_len,InsertLen;
  int StartChangeLine,ChangeLines,MidReadChar;
  int CursorX,CursorY;
  int SaveUndoNumber;

  if ((TextFp=fopen(FileName,"rb"))==NULL)
     return(FILEOPEN);

  SaveUndoNumber=UndoOperateSum;
  UndoInsertCursorGoto(Position);

  WpsVer=0;
  fseek(TextFp,0,SEEK_END);
  FileSize=ftell(TextFp);                  /* Get file size */
  fseek(TextFp,0,SEEK_SET);

  i=fread((char *)MidTextBlock,1,0x400,TextFp);
  if(MidTextBlock[0]>=0xff00 && i>0x300)
  {
      WpsVer=MidTextBlock[0]-0xff00+2;
      if(WpsVer==2)            /* ver 2.0 */
      {
         fseek(TextFp,0x300L,SEEK_SET);
         FileSize-=0x300;
      }
      else
         FileSize-=0x400;
  }
  else
  if(i>=0x80 && !memcmp(MidTextBlock,"\x31\xbe\x0\x0\x0\xab\x0\x0",8) )
  {     /*---------- it is a Window_Write file ------------*/
    fseek(TextFp,0x80L,SEEK_SET);
    FileSize=*(long *)((long)MidTextBlock+0xe);
    FileSize-=0x80;
  }
  else
    fseek(TextFp,0,SEEK_SET);

  BoxTextSize=TextBoxGetRemainSize(HBox);
  if (FileSize/2>BoxTextSize)   /* char_type will be changed to Wchar_type */
     FileSize=2*BoxTextSize;

  *NewHBox=HBox;
  total_len=InsertLen=i=0;
  do {
            for (j=0; i<FileSize && j<MaxReadLen; i++)
            {
                   MidReadChar=fgetc(TextFp);
                   if (MidReadChar==EOF)
                   {
                    err1:
                   #ifdef OLD_VERSION
                      fclose(TextFp);
                      return(FILEREAD);
                   #else
                      i=FileSize;
                      break;
                   #endif
                   }

                   if (MidReadChar<0x20 && MidReadChar!=ENTER)
                      continue;
                   /*---------------
                   if(MidReadChar==0x7f
                    || MidReadChar==0x8d    // Chinese (WordStar) CR
                    || MidReadChar==0x8a    // Chinese (WordStar) LF
                    ---------------------*/
                   if(MidReadChar>=0x7f && MidReadChar<=0xa0)
                      continue;

                   ReadChar=MidReadChar;
                   if (ReadChar>0xa0)
                   {
                      MidReadChar=fgetc(TextFp);
                      if (MidReadChar==EOF)
                         goto err1;

                      if (MidReadChar<=0xa0)
                         continue;

                      ReadChar<<=8;
                      ReadChar|=MidReadChar;

                  #ifdef NO_CHINESE_LETTLE
                      //----- change Chinese('A'..'Z') to English, 96,5.20
                      if(ReadChar>=0xa3c1 && ReadChar<=0xa3da)
                          ReadChar=MidReadChar-0xc1+'A';
                      else
                      if(ReadChar>=0xa3e1 && ReadChar<=0xa3fb)
                          ReadChar=MidReadChar-0xe1+'a';
                  #endif // NO_CHINESE_LETTLE

                      i++;
                   }

                   MidTextBlock[j++]=ReadChar;
            }

            if (*BlockStart<*BlockEnd)
            {
             #ifdef OLD_VERSION
               TextBoxDelKey(HBox,NewHBox,Position,NewPosition,1,BlockStart,BlockEnd);
               Position=*NewPosition;
             #else
               CancelBlock(HBox,BlockStart,BlockEnd);
             #endif
            }

         #ifdef OLD_VERSION
            else
               *NewPosition=Position;
         #endif

            Result=TextBoxInsertString(HBox,Position+InsertLen,MidTextBlock,j);
            if (Result<j) {
                 Alarm();   break;
            }

            total_len+=j;
            InsertLen+=Result;
            if(total_len<j || InsertLen<Result)      // overflow
               break;
  }   while (total_len<BoxTextSize && i<FileSize) ;

  fclose(TextFp);

 #ifndef OLD_VERSION
    *NewPosition=Position;
 #endif

  FormatInsertText(HBox,Position,InsertLen,&StartChangeLine,&ChangeLines, FALSE);
  if (!CursorLocate(HBox,NewHBox,*NewPosition,&CursorX,&CursorY) && ChangeLines)
       TextBoxRedraw(HBox,StartChangeLine,ChangeLines, FALSE);

  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  return(Result);
}

#endif

