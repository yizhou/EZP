/*-------------------------------------------------------------------
* Name: undo.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static HANDLE UndoBufferHandle;    // ,RedoBufferHandle;
static int UndoSP=0;
int UndoInitial(void)
{
  UndoBufferHandle=HandleAlloc(MAXUNDOREDOBUFFER,0);
  if (!UndoBufferHandle)
     return(OUTOFMEMORY);
  ReturnOK();
}

void UndoFinish(void)
{
  HandleFree(UndoBufferHandle);
}

static int UndoPush(int Lengh)
{
  if (UndoSP+Lengh<MAXUNDOREDOBUFFER)
  {
     UndoSP+=Lengh;
     return(Lengh);
  }
  else
     return(0);
}

static int UndoPop(int Lengh)
{
  if (UndoSP>=Lengh)
  {
     UndoSP-=Lengh;
     return(Lengh);
  }
  else
     return(0);
}

void UndoClear(void)
{
  UndoPop(UndoSP);
}

static int UndoSeekLastItem(void)
{
  int LastPoint,UndoItemLength;
  unsigned char *UndoBuffer;
  //UndoItems UndoItem;
  int UndoComposeCount,UndoType,i,UndoComposePos;

  UndoBuffer=HandleLock(UndoBufferHandle);
  if (UndoBuffer==NULL)
     return(OUTOFMEMORY);
  LastPoint=UndoSP;
  while (LastPoint>=sizeof(UndoItems))
  {
    UndoType=((UndoItems *)(UndoBuffer+LastPoint-
                    sizeof(UndoItems)))->UndoType;
    if (UndoType==COMPOSEOPERATE)
    {
       UndoComposeCount=((UndoItems *)(UndoBuffer+LastPoint-
                            sizeof(UndoItems)))->UndoBufferSize;
       LastPoint-=sizeof(UndoItems);
       UndoComposePos=LastPoint;
       for (i=0;i<UndoComposeCount;i++)
       {
           UndoType=((UndoItems *)(UndoBuffer+LastPoint-
                            sizeof(UndoItems)))->UndoType;
           UndoItemLength=((UndoItems *)(UndoBuffer+LastPoint-
                            sizeof(UndoItems)))->UndoBufferSize;
           if (UndoType==COMPOSEOPERATE)
              LastPoint-=sizeof(UndoItems);
           else
              LastPoint-=sizeof(UndoItems)+UndoItemLength;
       }
       UndoItemLength=UndoComposePos-LastPoint;
       //LastPoint-=sizeof(UndoItems);
    }
    else
    {
       UndoItemLength=((UndoItems *)(UndoBuffer+LastPoint-sizeof(UndoItems)))->
                      UndoBufferSize;
       LastPoint-=sizeof(UndoItems)+UndoItemLength;
    }
  }
  HandleUnlock(UndoBufferHandle);
  return(sizeof(UndoItems)+UndoItemLength);
}

static int UndoAppend(UndoItems *UndoItem,void *UndoItemBuffer)
{
  unsigned char *UndoBuffer;
  int LastItemPosition;
  int UndoItemBufferLength;

  UndoItemBufferLength=UndoItem->UndoBufferSize;
  if (sizeof(UndoItems)+UndoItem->UndoBufferSize>MAXUNDOREDOBUFFER)
  {
     UndoSP=0;
     return(OUTOFMEMORY);
  }
  UndoBuffer=HandleLock(UndoBufferHandle);
  if (UndoBuffer==NULL)
     return(OUTOFMEMORY);
  while (UndoSP+sizeof(UndoItems)+UndoItem->UndoBufferSize>=MAXUNDOREDOBUFFER)
  {
    LastItemPosition=UndoSeekLastItem();
    memmove(UndoBuffer,&UndoBuffer[LastItemPosition],UndoSP-LastItemPosition);
    UndoSP-=LastItemPosition;
  }
  UndoOperateSum++;
  if (UndoItem->UndoType==COMPOSEOPERATE)
  {
     memcpy(&UndoBuffer[UndoSP],UndoItem,sizeof(UndoItems));
     HandleUnlock(UndoBufferHandle);
     return(UndoPush(sizeof(UndoItems)));
  }
  else
  {
     if (UndoItemBufferLength>0)
        memcpy(&UndoBuffer[UndoSP],UndoItemBuffer,UndoItemBufferLength);
     memcpy(&UndoBuffer[UndoSP+UndoItemBufferLength],UndoItem,sizeof(UndoItems));
  }
  HandleUnlock(UndoBufferHandle);
  return(UndoPush(UndoItemBufferLength+sizeof(UndoItems)));
}

static int UndoReadInformation(UndoItems *UndoItem)
{
  unsigned char *UndoBuffer;

  if (UndoSP<sizeof(UndoItems))
  {
     UndoItem->UndoType=0xff;
     UndoItem->UndoBufferSize=0;
     return(0);
  }
  UndoBuffer=HandleLock(UndoBufferHandle);
  if (UndoBuffer==NULL)
     return(OUTOFMEMORY);
  memcpy(UndoItem,(UndoBuffer+UndoSP-sizeof(UndoItems)),sizeof(UndoItems));
  HandleUnlock(UndoBufferHandle);
  return(sizeof(UndoItems));
}

static int UndoRead(UndoItems *UndoItem,void **UndoItemBuffer)
{
  unsigned char *UndoBuffer;

  if (UndoSP<sizeof(UndoItems))
  {
     UndoItem->UndoType=0xff;
     UndoItem->UndoBufferSize=0;
     return(0);
  }
  UndoBuffer=HandleLock(UndoBufferHandle);
  if (UndoBuffer==NULL)
     return(OUTOFMEMORY);
  memcpy(UndoItem,(UndoBuffer+UndoSP-sizeof(UndoItems)),sizeof(UndoItems));
  if (UndoItem->UndoType!=COMPOSEOPERATE)
     *UndoItemBuffer=(UndoBuffer+UndoSP-sizeof(UndoItems)-
                    UndoItem->UndoBufferSize);
  HandleUnlock(UndoBufferHandle);
  if (UndoItem->UndoType==COMPOSEOPERATE)
     return(sizeof(UndoItems));
  else
     return(sizeof(UndoItems)+UndoItem->UndoBufferSize);
}

static int UndoGet(UndoItems *UndoItem,void *UndoItemBuffer)
{
  int Result;
  void *TmpBuffer;

  Result=UndoRead(UndoItem,&TmpBuffer);
  if (Result>0)
  {
     memcpy(UndoItemBuffer,TmpBuffer,UndoItem->UndoBufferSize);
     UndoOperateSum--;
     return(UndoPop(Result));
  }
  else
     return(Result);
}

#ifdef UNUSED           // ByHance, 96,1.29
int UndoChangeBuffer(void *UndoItemBuffer,int UndoItemBufferLength)
{
  unsigned char *UndoBuffer;
  UndoItems UndoItem;

  if (UndoSP<sizeof(UndoItems)+UndoItemBufferLength)
     return(0);
  UndoReadInformation(&UndoItem);
  if (UndoItemBufferLength!=UndoItem.UndoBufferSize)
  {
     UndoPop(sizeof(UndoItems)+UndoItemBufferLength);
     UndoItem.UndoBufferSize=UndoItemBufferLength;
     return(UndoAppend(&UndoItem,UndoItemBuffer));
  }
  else
  {
     UndoBuffer=HandleLock(UndoBufferHandle);
     if (UndoBuffer==NULL)
        return(OUTOFMEMORY);
     memcpy(UndoBuffer+UndoSP-sizeof(UndoItems)-UndoItem.UndoBufferSize,
            UndoItemBuffer,UndoItem.UndoBufferSize);
     HandleUnlock(UndoBufferHandle);
  }
  return(UndoItemBufferLength);
}
#endif    // UNUSED           // ByHance, 96,1.29

static int UndoInsertOperate(char Operater,int SaveLength,void *SaveBuffer)
{
  UndoItems UndoItem;

  if (DoUndoSign)
     return(0);

  switch (Operater)
  {
    case TABLELINEMOVE:
    case TABLECOLMOVE:
    case TABLEINSERTLINE:
    case TABLEINSERTCOL:
    case TABLEDELETELINE:
    case TABLEDELETECOL:
    case TABLECHANGEHLINE:
    case TABLECHANGEVLINE:
    case TABLECHANGESLANT:
    case TABLECELLMERGE:
    case TABLEDISMERGE:

    case CURSORGOTO:
    case CURSORDEFINEBLOCK:
    case IMAGEROTATE:
    case IMAGESKEW:
    case IMAGEZOOM:
    case IMAGECOLOR:
    case IMAGECONTRAST:
    case BOXRESIZE:
    case BOXROTATE:
    case BOXCHANGEPARAMETER:
    case BOXMOVE:
    case BOXSELECT:                    // Compress operate
         if ((UndoReadInformation(&UndoItem)>0)&&(UndoItem.UndoType==Operater))
            return(SaveLength);
         goto insert_it;

    case IMAGEMOVE:
    case GROUPMOVE:                    // Compress operate
         if ((UndoReadInformation(&UndoItem)>0)&&(UndoItem.UndoType==Operater))
         {
            int OldMove[2];

            UndoGet(&UndoItem,&OldMove[0]);// Increase MoveX & MoveY
            OldMove[0]+=((int *)SaveBuffer)[0];
            OldMove[1]+=((int *)SaveBuffer)[1];
            return(UndoAppend(&UndoItem,&OldMove[0]));
         }
    case GROUPCREAT:
    case GROUPDELETE:
    case CURSORUNDEFINEBLOCK:
    case STRINGINSERT:
    case STRINGDELETE:
    //case STRINGEXCHANGE:
    case BOXCREAT:
    case BOXDELETE:
    case PAGEINSERT:
    case PAGEDELETE:
    case GROUPALIGN:
    case COMPOSEOPERATE:
    case IMAGEINSERT:
    case IMAGEDELETE:                  // Insert operate
    insert_it:
         UndoItem.UndoType=Operater;
         UndoItem.UndoBufferSize=SaveLength;
         return(UndoAppend(&UndoItem,SaveBuffer));
    case IMAGENEGATIVE:
    case BOXEXCHANGE:
    case PAGEEXCHANGE:                 // Merge operate
         if (UndoReadInformation(&UndoItem)>0 && UndoItem.UndoType==Operater)
         {
            UndoOperateSum--;
            return(UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize));
         }
         UndoItem.UndoType=Operater;
         UndoItem.UndoBufferSize=SaveLength;
         return(UndoAppend(&UndoItem,SaveBuffer));
    default:                           // Not implement, so clear undo buffer
      UndoSP=0;
      return(0);
  }
}

int UndoInsertCursorGoto(int OldPosition)
{
  int Buffer[2];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldPosition;
  return(UndoInsertOperate(CURSORGOTO,sizeof(OldPosition),Buffer));
}

int UndoInsertCursorDefineBlock(int OldBlockStart,int OldBlockEnd)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldBlockStart;
  Buffer[1]=OldBlockEnd;
  return(UndoInsertOperate(CURSORDEFINEBLOCK,2*sizeof(OldBlockStart),Buffer));
}

int UndoInsertCursorUndefineBlock(int OldStart,int OldEnd)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldStart;
  Buffer[1]=OldEnd;
  return(UndoInsertOperate(CURSORUNDEFINEBLOCK,2*sizeof(OldStart),Buffer));
}

int UndoInsertStringInsert(int Position,int StringLength)
{
  int Buffer[4];

  if (DoUndoSign)
     return(0);
  Buffer[0]=Position;
  Buffer[1]=StringLength;
  Buffer[2]=TotalPage;          // ByHance, 96,2.2
  return(UndoInsertOperate(STRINGINSERT,3*sizeof(StringLength),Buffer));
}

int UndoInsertStringDelete(int Position,int StringLength,Wchar *String)
{
  int Result;

  if (DoUndoSign)
     return(0);

  Result=UndoInsertOperate(STRINGDELETE,sizeof(unsigned short)*StringLength
                           +sizeof(Position),String);
  if (Result>0)
  {
     unsigned char *UndoBuffer;

     UndoBuffer=HandleLock(UndoBufferHandle);
     if (UndoBuffer==NULL)
        return(OUTOFMEMORY);
     *((int *)&(UndoBuffer[UndoSP-sizeof(UndoItems)-sizeof(Position)]))=Position;
     HandleUnlock(UndoBufferHandle);
  }
  ReturnOK();
}

int UndoInsertStringExchange(int Position,int InsertStringLength,int DeleteStringLength,
                             Wchar *DeleteString)
{
  int Buffer[2];

  if (DoUndoSign)
     return(0);
  Buffer[0]=InsertStringLength;
  UndoInsertStringDelete(Position,DeleteStringLength,DeleteString);
  return(UndoInsertStringInsert(Position,InsertStringLength));
}

int UndoInsertImageInsert(char *OldImageName)
{
  if (DoUndoSign)
     return(0);
  return(UndoInsertOperate(IMAGEINSERT,strlen(OldImageName)+1,OldImageName));
}

int UndoInsertImageDelete(char *OldImageName)
{
  if (DoUndoSign)
     return(0);
  return(UndoInsertOperate(IMAGEDELETE,strlen(OldImageName)+1,OldImageName));
}

int UndoInsertImageMove(int OldMoveX,int OldMoveY)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldMoveX;
  Buffer[1]=OldMoveY;
  return(UndoInsertOperate(IMAGEMOVE,2*sizeof(OldMoveX),Buffer));
}

int UndoInsertImageZoom(int OldZoomX,int OldZoomY)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldZoomX;
  Buffer[1]=OldZoomY;
  return(UndoInsertOperate(IMAGEZOOM,2*sizeof(OldZoomX),Buffer));
}

int UndoInsertImageRotate(int OldRotate)
{
  int Buffer[2];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldRotate;
  return(UndoInsertOperate(IMAGEROTATE,sizeof(OldRotate),Buffer));
}

int UndoInsertImageSkew(int OldAngle)
{
  int Buffer[2];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldAngle;
  return(UndoInsertOperate(IMAGESKEW,sizeof(OldAngle),Buffer));
}

int UndoInsertImageColor(int OldColor)
{
  int Buffer[2];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldColor;
  return(UndoInsertOperate(IMAGECOLOR,sizeof(OldColor),Buffer));
}

#ifdef UNUSED           // ByHance, 96,1.29
int UndoInsertImageNegative(void)
{
  if (DoUndoSign)
     return(0);
  return(UndoInsertOperate(IMAGENEGATIVE,0,NULL));
}

int UndoInsertImagePosterized(int OldPosterized)
{
  int Buffer[2];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldPosterized;
  return(UndoInsertOperate(IMAGECONTRAST,sizeof(OldPosterized),Buffer));
}
#endif   // UNUSED           // ByHance, 96,1.29

int UndoInsertTableSlip(int iCell,int type)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=iCell;
  Buffer[1]=type;
  return(UndoInsertOperate(TABLECHANGESLANT,2*sizeof(iCell),Buffer));
}

int UndoInsertTableCellMerge(int iMgLine,int iMgCol,int numMgLines,int numMgCols,HANDLE hUndoCellTable)
{
  int Buffer[6];

  if (DoUndoSign)
     return(0);
  Buffer[0]=iMgLine;
  Buffer[1]=iMgCol;
  Buffer[2]=numMgLines;
  Buffer[3]=numMgCols;
  Buffer[4]=hUndoCellTable;
  return(UndoInsertOperate(TABLECELLMERGE,5*sizeof(iMgLine),Buffer));
}

int UndoInsertTableDismerge(int iLineBegin,int iColBegin,int iLineNum,int iColNum)
{
  int Buffer[6];

  if (DoUndoSign)
     return(0);
  Buffer[0]=iLineBegin;
  Buffer[1]=iColBegin;
  Buffer[2]=iLineNum;
  Buffer[3]=iColNum;
  return(UndoInsertOperate(TABLEDISMERGE,4*sizeof(int),Buffer));
}

int UndoInsertTableInsertLine(int nLineIndex)
{
  int Buffer[4];

  if (DoUndoSign)
     return(0);
  Buffer[0]=nLineIndex;
  //Buffer[1]=OldPosY;
  Buffer[1]=TotalPage;
  return(UndoInsertOperate(TABLEINSERTLINE,2*sizeof(int),Buffer));
}

int UndoInsertTableDeleteLine(int nLineIndex,int OldPosY)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=nLineIndex;
  Buffer[1]=OldPosY;
  return(UndoInsertOperate(TABLEDELETELINE,2*sizeof(int),Buffer));
}

int UndoInsertTableInsertCol(int nColIndex)
{
  int Buffer[4];

  if (DoUndoSign)
     return(0);
  Buffer[0]=nColIndex;
  //Buffer[1]=OldPosX;
  Buffer[1]=TotalPage;
  return(UndoInsertOperate(TABLEINSERTCOL,2*sizeof(int),Buffer));
}

int UndoInsertTableDeleteCol(int nColIndex,int OldPosX)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=nColIndex;
  Buffer[1]=OldPosX;
  return(UndoInsertOperate(TABLEDELETECOL,2*sizeof(int),Buffer));
}

int UndoInsertTableChangeHline(int nLineIndex,int OldType)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=nLineIndex;
  Buffer[1]=OldType;
  return(UndoInsertOperate(TABLECHANGEHLINE,2*sizeof(OldType),Buffer));
}

int UndoInsertTableChangeVline(int nColIndex,int OldType)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=nColIndex;
  Buffer[1]=OldType;
  return(UndoInsertOperate(TABLECHANGEVLINE,2*sizeof(OldType),Buffer));
}

int UndoInsertTableChangeSlant(int iCell,int type)
{
  int Buffer[3];

  if (DoUndoSign)
     return(0);
  Buffer[0]=iCell;
  Buffer[1]=type;
  return(UndoInsertOperate(TABLECHANGESLANT,2*sizeof(iCell),Buffer));
}

int UndoInsertTableLineMove(int nLineIndex,int OldPosY)
{
  int Buffer[4];

  if (DoUndoSign)
     return(0);
  Buffer[0]=nLineIndex;
  Buffer[1]=OldPosY;
  Buffer[2]=TotalPage;
  return(UndoInsertOperate(TABLELINEMOVE,3*sizeof(int),Buffer));
}

int UndoInsertTableColMove(int nColIndex,int OldPosX)
{
  int Buffer[4];

  if (DoUndoSign)
     return(0);
  Buffer[0]=nColIndex;
  Buffer[1]=OldPosX;
  Buffer[2]=TotalPage;
  return(UndoInsertOperate(TABLECOLMOVE,3*sizeof(int),Buffer));
}

int UndoInsertBoxMove(int OldPosX,int OldPosY)
{
  int Buffer[4];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldPosX;
  Buffer[1]=OldPosY;
  Buffer[2]=TotalPage;
  return(UndoInsertOperate(BOXMOVE,3*sizeof(int),Buffer));
}

int UndoInsertBoxResize(int OldPosX,int OldPosY)
{
  int Buffer[4];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldPosX;
  Buffer[1]=OldPosY;
  Buffer[2]=TotalPage;
  return(UndoInsertOperate(BOXRESIZE,3*sizeof(int),Buffer));
}

int UndoInsertBoxRotate(int OldLeft,int OldTop,int OldRotateAngle,
                        int OldRotateAxisX,int OldRotateAxisY)
{
  int Buffer[7];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldLeft;
  Buffer[1]=OldTop;
  Buffer[2]=OldRotateAngle;
  Buffer[3]=OldRotateAxisX;
  Buffer[4]=OldRotateAxisY;
  Buffer[5]=TotalPage;
  return(UndoInsertOperate(BOXROTATE,6*sizeof(OldRotateAngle),Buffer));
}

int UndoInsertBoxSelect(HBOX OldHBox,int Position,int Cell)
{
  int Buffer[4];

  if (DoUndoSign)
     return(0);
  Buffer[0]=OldHBox;
  Buffer[1]=Position;
  Buffer[2]=Cell;
  return(UndoInsertOperate(BOXSELECT,3*sizeof(OldHBox),Buffer));
}

int UndoInsertBoxCreat(HBOX HBox,HBOX OldBox)
{
  int Buffer[4];

  if (DoUndoSign)
     return(0);
  Buffer[0]=HBox;
  Buffer[1]=OldBox;
  Buffer[2]=TotalPage;          // ByHance, 96,4.4
  return(UndoInsertOperate(BOXCREAT,3*sizeof(HBox),Buffer));
}

int UndoInsertBoxDelete(HBOX HBox)
{
 /*---------  delete ByHance, for error ------
  int Buffer[2];

  if (DoUndoSign)
     return(0);
  Buffer[0]=HBox;
  return(UndoInsertOperate(BOXDELETE,sizeof(HBox),Buffer));
 ------------*/
  return 0;
}

int UndoInsertCompose(int Number)
{
  Number=abs(Number);
  if (DoUndoSign||Number<1)     //<=1)  Change By DG in 1996,2
     return(0);
  return(UndoInsertOperate(COMPOSEOPERATE,Number,NULL));
}

void Undo(void)
{
  UndoItems UndoItem;
  int TmpX,TmpY;
  void *UndoBuffer;
  int *BlockPosition;
  int *ImageParameter;
  int *BoxParameter;
  int *InsertLength,Length,StartChangeLine,ChangeLines,Position;
  TextBoxs *TextBox;
  char *Name;
  int OldPosX,OldPosY;
  HBOX NewHBox,OldBox;
  int OldSign;
  int iLineBegin,iColBegin,iLineNum,iColNum;

 lbl_try_next:
  if(UndoRead(&UndoItem,&UndoBuffer)<=0)        // ByHance, 96,1.21
  {
       //Alarm();       // No more undo
       MessageBox(GetTitleString(WARNINGINFORM),"无法继续复原!",1,1);
       return;
  }

  switch (UndoItem.UndoType)
  {
    case CURSORDEFINEBLOCK:
    case CURSORGOTO:
    case CURSORUNDEFINEBLOCK:
    case STRINGINSERT:
    case STRINGDELETE:
         if(!BoxCanEditable(GlobalBoxHeadHandle))    // ByHance,96,1.31
         {              // can not undo, must be text box
         /*-----------------
            MessageBox(GetTitleString(WARNINGINFORM),
                       "先将文本框的边框变为实线\n黑框,再进行复原操作!",
                       1,1);
            return;
           ----------------*/
        lbl_skip_it:
            UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
            UndoOperateSum--;
            goto lbl_try_next;
         }
         break;
    case IMAGEMOVE:
    case IMAGEROTATE:
    case IMAGESKEW:
    case IMAGEZOOM:
    case IMAGECOLOR:
    case IMAGEINSERT:
    case IMAGEDELETE:
         if(!BoxIsPictureBox(GlobalBoxHeadHandle))    // ByHance,96,1.31
         {              // can not undo, must be picture box
            goto lbl_skip_it;
         }
         break;
    case TABLECELLMERGE:
    case TABLEDISMERGE:
    case TABLEINSERTLINE:
    case TABLEINSERTCOL:
    case TABLEDELETELINE:
    case TABLEDELETECOL:
    case TABLECHANGEHLINE:
    case TABLECHANGEVLINE:
    case TABLECHANGESLANT:
    case TABLELINEMOVE:
    case TABLECOLMOVE:
         if(!BoxIsTableBox(GlobalBoxHeadHandle))    // ByHance,96,1.31
            goto lbl_skip_it;
         break;
  } /* switch */

  OldSign=DoUndoSign;
  DoUndoSign=1;

  switch (UndoItem.UndoType)
  {
    case CURSORDEFINEBLOCK:
         CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,
                         &GlobalTextBlockEnd);
         BlockPosition=(int *)UndoBuffer;
         GlobalTextBlockStart=BlockPosition[0];
         GlobalTextBlockEnd=BlockPosition[1];
         TextCursorOff();
         if (GlobalTextBlockStart<GlobalTextBlockEnd)
            DisplayBlock(GlobalBoxHeadHandle,GlobalTextBlockStart,GlobalTextBlockEnd);
         UndoOperateSum--;
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         break;
    case CURSORGOTO:
          Position=*(int *)UndoBuffer;
          GlobalTextPosition=Position;
          CursorLocate(GlobalBoxHeadHandle,&NewHBox,Position,&TmpX,&TmpY);
          UndoOperateSum--;
          UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
          break;
    case CURSORUNDEFINEBLOCK:
         BlockPosition=(int *)UndoBuffer;
         GlobalTextBlockStart=BlockPosition[0];
         GlobalTextBlockEnd=BlockPosition[1];
         TextCursorOff();
         DisplayBlock(GlobalBoxHeadHandle,GlobalTextBlockStart,GlobalTextBlockEnd);
         UndoOperateSum--;
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         break;
    case STRINGINSERT:
         InsertLength=(int *)UndoBuffer;
         Position=InsertLength[0];
         Length=InsertLength[1];
         TotalPage=InsertLength[2];     // ByHance, 96,2.2

         if (GlobalTextBlockStart<GlobalTextBlockEnd)
            DisplayBlock(GlobalBoxHeadHandle,GlobalTextBlockStart,GlobalTextBlockEnd);

         Length=TextBoxDeleteString(GlobalBoxHeadHandle,Position,Length);
         if (Length)
         {
            FormatDeleteText(GlobalBoxHeadHandle,Position,
                             Length,&StartChangeLine,&ChangeLines,FALSE);
            TextBoxRedraw(GlobalBoxHeadHandle,StartChangeLine,ChangeLines, FALSE);
            CursorLocate(GlobalBoxHeadHandle,&GlobalBoxHeadHandle,
                         GlobalTextPosition,&TmpX,&TmpY);
            TellStatus();
         }

         if (GlobalTextBlockStart<GlobalTextBlockEnd)
            DisplayBlock(GlobalBoxHeadHandle,GlobalTextBlockStart,GlobalTextBlockEnd);

         UndoOperateSum--;
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         break;
    case STRINGDELETE:
         {
           Wchar *DeleteString;

           DeleteString=(Wchar *)UndoBuffer;
           Position=*((int *)(&DeleteString[(UndoItem.UndoBufferSize-sizeof(int))
                                 /sizeof(unsigned short)]));
           if (GlobalTextBlockStart<GlobalTextBlockEnd)
              DisplayBlock(GlobalBoxHeadHandle,GlobalTextBlockStart,GlobalTextBlockEnd);
           Length=TextBoxInsertString(GlobalBoxHeadHandle,Position,DeleteString,
                      (UndoItem.UndoBufferSize-sizeof(int))/sizeof(unsigned short));
           if (Length)
           {
              FormatDeleteText(GlobalBoxHeadHandle,Position,
                               Length,&StartChangeLine,&ChangeLines,FALSE);
              TextBoxRedraw(GlobalBoxHeadHandle,StartChangeLine,ChangeLines, FALSE);
              CursorLocate(GlobalBoxHeadHandle,&GlobalBoxHeadHandle,
                           GlobalTextPosition,&TmpX,&TmpY);
           }
           if (GlobalTextBlockStart<GlobalTextBlockEnd)
              DisplayBlock(GlobalBoxHeadHandle,GlobalTextBlockStart,GlobalTextBlockEnd);
           UndoOperateSum--;
           UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
           break;
         }
    case IMAGEMOVE:
         ImageParameter=(int *)UndoBuffer;
         OldPosX=ImageParameter[0];
         OldPosY=ImageParameter[1];
         PictureBoxMovePicture(GlobalBoxHeadHandle,-OldPosX,-OldPosY);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;
    case IMAGEROTATE:
         ImageParameter=(int *)UndoBuffer;
         TmpX=ImageParameter[0];      // rotate angle
         PictureBoxRotatePicture(GlobalBoxHeadHandle,TmpX);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;
    case IMAGESKEW:
         ImageParameter=(int *)UndoBuffer;
         TmpX=ImageParameter[0];        // angle
         PictureBoxSkewPicture(GlobalBoxHeadHandle,TmpX);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;
    case IMAGEZOOM:
         ImageParameter=(int *)UndoBuffer;
         TmpX=ImageParameter[0];      // zoom x
         TmpY=ImageParameter[1];      // zoom y
         PictureBoxZoomPicture(GlobalBoxHeadHandle,TmpX,TmpY);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;
    case IMAGECOLOR:
         ImageParameter=(int *)UndoBuffer;
         OldPosX=ImageParameter[0];     // color
         //OldPosY=ImageParameter[1];
         //PictureBoxMovePicture(GlobalBoxHeadHandle,-OldPosX,-OldPosY);
         PictureBoxSetPictureColor(GlobalBoxHeadHandle,OldPosX);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;

   #ifdef UNUSED        // ByHance, 96,1.30
    case IMAGECONTRAST:
         {
           int OldContrast;

           ImageParameter=(int *)UndoBuffer;
           OldContrast=ImageParameter[0];
           PictureBoxSetPictureNewContrast(GlobalBoxHeadHandle,OldContrast);
           UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
           UndoOperateSum--;
         }
         break;
    case IMAGENEGATIVE:
         if(!BoxIsPictureBox(GlobalBoxHeadHandle)    // ByHance,96,1.31
            break;

         PictureBoxSetPictureNegative(GlobalBoxHeadHandle);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;
   #endif  // UNUSED        // ByHance, 96,1.30

    case IMAGEINSERT:
         Name=(char *)UndoBuffer;
         if (!Name[0])
            PictureBoxClearImage(GlobalBoxHeadHandle);
         else
            PictureBoxImportTiff(Name,GlobalBoxHeadHandle);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;
    case IMAGEDELETE:
         Name=(char *)UndoBuffer;
         if (Name[0])
            PictureBoxImportTiff(Name,GlobalBoxHeadHandle);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;

    case TABLECELLMERGE:
         BoxParameter=(int *)UndoBuffer;
         iLineBegin=BoxParameter[0];
         iColBegin=BoxParameter[1];
         iLineNum=BoxParameter[2];
         iColNum=BoxParameter[3];
         FBDisMergeCells(GlobalBoxHeadHandle,iLineBegin,iColBegin,iLineNum,iColNum,(HANDLE)BoxParameter[4]);
         //ReFormatTableText(GlobalBoxHeadHandle,TRUE);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;
    case TABLEDISMERGE:
         BoxParameter=(int *)UndoBuffer;
         iLineBegin=BoxParameter[0];
         iColBegin=BoxParameter[1];
         iLineNum=BoxParameter[2];
         iColNum=BoxParameter[3];
         FBMergeCells(GlobalBoxHeadHandle,iLineBegin,iColBegin,iLineNum,iColNum);
         //ReFormatTableText(GlobalBoxHeadHandle,TRUE);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;
    case TABLEINSERTLINE:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         //OldPosY=BoxParameter[1];
         TotalPage=BoxParameter[1];
         FBDelALine(GlobalBoxHeadHandle,OldPosX,TRUE); // ByHance,96,4.8
         //FBDelALine(GlobalBoxHeadHandle,OldPosX,FALSE);
         //ReFormatTableText(GlobalBoxHeadHandle,TRUE);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;
    case TABLEINSERTCOL:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         //OldPosY=BoxParameter[1];
         TotalPage=BoxParameter[1];
         FBDelACol(GlobalBoxHeadHandle,OldPosX,FALSE); // ByHance,96,4.8
         //ReFormatTableText(GlobalBoxHeadHandle,TRUE);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;
    case TABLEDELETELINE:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         OldPosY=BoxParameter[1];
         FBInsALine(GlobalBoxHeadHandle,OldPosX,OldPosY,FALSE);
         ReFormatTableText(GlobalBoxHeadHandle,TRUE);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;
    case TABLEDELETECOL:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         OldPosY=BoxParameter[1];
         FBInsACol(GlobalBoxHeadHandle,OldPosX,OldPosY,FALSE);
         //ReFormatTableText(GlobalBoxHeadHandle,TRUE);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;

    case TABLECHANGEHLINE:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         OldPosY=BoxParameter[1];
         FBChangeHLineType(GlobalBoxHeadHandle,OldPosX,OldPosY);
         ReFormatTableText(GlobalBoxHeadHandle,TRUE);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;
    case TABLECHANGEVLINE:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         OldPosY=BoxParameter[1];
         FBChangeVLineType(GlobalBoxHeadHandle,OldPosX,OldPosY);
         ReFormatTableText(GlobalBoxHeadHandle,TRUE);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;
    case TABLECHANGESLANT:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         OldPosY=BoxParameter[1];
         FBSlipCell(GlobalBoxHeadHandle,OldPosX,OldPosY);
         ReFormatTableText(GlobalBoxHeadHandle,TRUE);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;

    case TABLELINEMOVE:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         OldPosY=BoxParameter[1];
         TotalPage=BoxParameter[2];
         TableChangeHortLine(GlobalBoxHeadHandle,OldPosX,OldPosY);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;
    case TABLECOLMOVE:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         OldPosY=BoxParameter[1];
         TotalPage=BoxParameter[2];
         TableChangeVertLine(GlobalBoxHeadHandle,OldPosX,OldPosY);
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         RedrawUserField();
         break;

    case BOXMOVE:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         OldPosY=BoxParameter[1];
         TotalPage=BoxParameter[2];
         TextBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
         if (TextBox==NULL)
            break;

         TextBoxSetBoxLeft(TextBox,OldPosX);
         TextBoxSetBoxTop(TextBox,OldPosY);
         BoxChange(PageGetBoxHead(GlobalCurrentPage),GlobalCurrentPage);
         RedrawUserField();
         HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;
    case BOXRESIZE:
         BoxParameter=(int *)UndoBuffer;
         OldPosX=BoxParameter[0];
         OldPosY=BoxParameter[1];
         TotalPage=BoxParameter[2];
         TextBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
         if (TextBox==NULL)
            break;

         TextBoxSetBoxWidth(TextBox,OldPosX);
         TextBoxSetBoxHeight(TextBox,OldPosY);
         BoxChange(PageGetBoxHead(GlobalCurrentPage),GlobalCurrentPage);
         RedrawUserField();
         HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;
    case BOXSELECT:
         {
           int OldPosition,OldCell;

           BoxParameter=(int *)UndoBuffer;
           OldBox=BoxParameter[0];
           OldPosition=BoxParameter[1];
           OldCell=BoxParameter[2];
           BoxDrawBorder(GlobalBoxHeadHandle,DRAWXORBORDER|DRAWBORDERWITHRECATNGLE);
           BoxDrawBorder(GlobalBoxHeadHandle,DRAWVIRTUALBORDOR);
           GlobalBoxHeadHandle=OldBox;
           BoxDrawBorder(GlobalBoxHeadHandle,DRAWBORDERWITHRECATNGLE);
           if (BoxIsTextBox(GlobalBoxHeadHandle))
              GlobalTextPosition=OldPosition;
           else
              if (BoxIsTableBox(GlobalBoxHeadHandle))
              {
                 GlobalTextPosition=OldPosition;
                 GlobalTableCell=OldCell;
              }
           SetNewCursor();
           UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
           UndoOperateSum--;
         }
         break;
    case BOXROTATE:
         {
           int OldAxisX,OldAxisY,OldAngle;

           BoxParameter=(int *)UndoBuffer;
           OldPosX=BoxParameter[0];
           OldPosY=BoxParameter[1];
           OldAxisX=BoxParameter[2];
           OldAxisY=BoxParameter[3];
           OldAngle=BoxParameter[4];
           TotalPage=BoxParameter[5];
           TextBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
           if (TextBox==NULL)
              break;

           TextBoxSetBoxLeft(TextBox,OldPosX);
           TextBoxSetBoxTop(TextBox,OldPosY);
           TextBoxSetRotateAxisX(TextBox,OldAxisX);
           TextBoxSetRotateAxisY(TextBox,OldAxisY);
           TextBoxSetRotateAngle(TextBox,OldAngle);
           BoxChange(PageGetBoxHead(GlobalCurrentPage),GlobalCurrentPage);
           RedrawUserField();
           SetNewCursor();      // ByHance, 96,1.22
           HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
           UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
           UndoOperateSum--;
         }
         break;
    case BOXCREAT:
         BoxParameter=(int *)UndoBuffer;
         NewHBox=BoxParameter[0];
         OldBox=BoxParameter[1];
         TotalPage=BoxParameter[2];     // ByHance, 96,2.2
         BoxDelete(NewHBox);
         GlobalBoxHeadHandle=OldBox;
         BoxChange(PageGetBoxHead(GlobalCurrentPage),GlobalCurrentPage);
         RedrawUserField();
         SetNewCursor();
         HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
         break;
    case COMPOSEOPERATE:
         UndoOperateSum--;
         UndoPop(sizeof(UndoItem));
         TmpX=UndoOperateSum-UndoItem.UndoBufferSize; // TotalCompose
         while (UndoOperateSum>TmpX)
           Undo();
       /*---------------------
           TmpY=GlobalNotDisplay;
           GlobalNotDisplay=1;
           while (UndoOperateSum>TmpX+1)
               Undo();
           GlobalNotDisplay=0;
           Undo();
           GlobalNotDisplay=TmpY;
       ---------------------*/
         break;
    case PAGEINSERT:
    case PAGEDELETE:
    case PAGEEXCHANGE:
    case GROUPMOVE:
    case GROUPCREAT:
    case GROUPDELETE:
    case GROUPALIGN:
    case BOXEXCHANGE:
    case BOXCHANGEPARAMETER:
    //case STRINGEXCHANGE:
    //case BOXDELETE:
    default:                           // Not implement, so clear undo buffer
         UndoPop(sizeof(UndoItem)+UndoItem.UndoBufferSize);
         UndoOperateSum--;
  }

  DoUndoSign=OldSign;
}
