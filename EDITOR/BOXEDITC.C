/*-------------------------------------------------------------------
* Name: boxeditc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static int TableTextInitial(HBOX HTableBox);

static int EditBufferInsertString(Wchar *Source,TEXTTYPE Position,Wchar *String,
         TEXTTYPE *SourceLength,TEXTTYPE DestLength,TEXTTYPE RightBound)
{
  long TmpDestLength;

  if (Position>=*SourceLength)
     Position=*SourceLength-1;
  if (Position<0)
     Position=0;
  TmpDestLength=DestLength;
  if ((long)(*SourceLength)+TmpDestLength>=RightBound)
     TmpDestLength=RightBound-*SourceLength;
  if (!TmpDestLength)
     return(0);
  UndoInsertStringInsert(Position,TmpDestLength);
  memmove(&Source[Position+TmpDestLength],&Source[Position],
          ((*SourceLength)-Position)*sizeof(short));
  memcpy(&Source[Position],String,TmpDestLength*sizeof(short));
  *SourceLength+=TmpDestLength;
  *(Source+*SourceLength-1)=*(Source+*SourceLength)=0;
  return(TmpDestLength);
}

int EditBufferDeleteString(Wchar *Source,TEXTTYPE Position,
                         TEXTTYPE Length,TEXTTYPE *SourceLength)
{
  if (Position>=*SourceLength)
     Position=*SourceLength-1;
  if (Position<0)
     Position=0;
  if (Length>*SourceLength-Position)
     Length=*SourceLength-Position;
  UndoInsertStringDelete(Position,Length,&Source[Position]);
  if (Length)
     memmove(&Source[Position],&Source[Position+Length],
             ((*SourceLength)-Position-Length)*sizeof(short));
  *SourceLength-=Length;
  *(Source+*SourceLength-1)=*(Source+*SourceLength)=0;
  return(Length);
}

static int EditBufferExchangeString(Wchar *Source,TEXTTYPE Position,
            TEXTTYPE ExchangeLength,Wchar *String,TEXTTYPE *SourceLength,
            TEXTTYPE DestLength,TEXTTYPE RightBound)
{
  if (Position>=*SourceLength)
     Position=*SourceLength-1;
  if (Position<0)
     Position=0;
  if ((long)*SourceLength+(long)DestLength-ExchangeLength>RightBound)
     DestLength=RightBound-(*SourceLength-ExchangeLength);
  if (!DestLength)
     return(0);
  UndoInsertStringExchange(Position,DestLength,ExchangeLength,&Source[Position]);
  if (ExchangeLength>DestLength)
  {
     memmove(&Source[Position+DestLength],&Source[Position+ExchangeLength],
             ((*SourceLength)-ExchangeLength)*sizeof(short));
  }
  else
  {
     if (ExchangeLength<DestLength)
     {
        memmove(&Source[Position+ExchangeLength],&Source[Position+DestLength],
                ((*SourceLength)-ExchangeLength)*sizeof(short));
     }
  }
  memcpy(&Source[Position],String,sizeof(Wchar)*DestLength);
  *SourceLength+=DestLength-ExchangeLength;
  *(Source+*SourceLength-1)=*(Source+*SourceLength)=0;
  return(DestLength);
}

static int EditBufferSearchAttribute(Wchar *Source,TEXTTYPE Position,
                unsigned char SearchAttribute,TEXTTYPE *AttributePosition)
{
  TEXTTYPE i,Attribute;

  Attribute=(SearchAttribute<<ATTRIBUTEBITS);
//  for (i=Position-1;i>=0;i--)
  for (i=Position;i>=0;i--)
  {
      if ((Source[i]&ATTRIBUTEPRECODE)==Attribute)
      {
         *AttributePosition=i;
         //return(Source[i]&ATTRIBUTEPATTERN);
         return (GetAttribute(Source[i]));      //By zjh 10.29
      }
  }
  *AttributePosition=-1;
  return(-1);
}

int EditBufferSearchNextAttribute(Wchar *Source,TEXTTYPE Position,
            TEXTTYPE RightBound,unsigned char SearchAttribute,
            TEXTTYPE *AttributePosition)
{
  TEXTTYPE i,Attribute;

  Attribute=(SearchAttribute<<ATTRIBUTEBITS);
  //for (i=Position;i<RightBound;i++)
  for (i=Position+1;i<RightBound;i++)
  {
      if ((Source[i]&ATTRIBUTEPRECODE)==Attribute)
      {
         *AttributePosition=i;
         //return(Source[i]&ATTRIBUTEPATTERN);
         return (GetAttribute(Source[i]));      //By zjh 10.29
      }
  }
  *AttributePosition=-1;
  return(-1);
}

#ifdef UNUSED           // ByHance, 96,1.29
static int EditBufferChangeAttribute(Wchar *Source,TEXTTYPE Position,
               TEXTTYPE ChangeItem, TEXTTYPE Attribute,
               TEXTTYPE *SourceLength,TEXTTYPE RightBound)
{
  unsigned short tmp[2];

  tmp[0]=MakeATTRIBUTE(ChangeItem,Attribute);
  return(EditBufferInsertString(Source,Position,tmp,SourceLength,1,RightBound));
}

static int EditBufferSearchCharPosition(Wchar *Source,TEXTTYPE Position)
{
  TEXTTYPE i;
  unsigned short Tmp;

  for (i=Position-1;i>=0;i--)
  {
      Tmp=Source[i]&ATTRIBUTEPRECODE;
      if ((Tmp==HIGHENGLISHCHAR)||(Tmp>=HIGHCHINESECHARS))
         return(i);
  }
  return(-1);
}
#endif  // UNUSED

static int EditBufferSeekPosition(Wchar *Source,TEXTTYPE Position,
                 TEXTTYPE Length,TEXTTYPE *NewPosition,TEXTTYPE RightBound)
{
  TEXTTYPE i,SeekLength;
  Wchar Tmp;

  SeekLength=0;
  if (Length<0)
  {
     for (i=Position-1;(i>=0)&&(SeekLength<-Length);i--)
     {
         Tmp=Source[i]&ATTRIBUTEPRECODE;
         if ((Tmp==HIGHENGLISHCHAR)||(Tmp>=HIGHCHINESECHARS)
             ||(Tmp==(INSERTBOX<<ATTRIBUTEBITS)))
            SeekLength++;
     }
     *NewPosition=i+1;
     return(SeekLength);
  }
  else
  {
     for (i=Position;(i<RightBound)&&(SeekLength<=Length);i++)
     {
         Tmp=Source[i]&ATTRIBUTEPRECODE;
         if ((Tmp==HIGHENGLISHCHAR)||(Tmp>=HIGHCHINESECHARS)
             ||(Tmp==(INSERTBOX<<ATTRIBUTEBITS)))
            SeekLength++;
     }
     if (i>RightBound)
     {
        *NewPosition=i;
        return(SeekLength);
     }
     else
     {
        if (RightBound==0)
        {
           *NewPosition=0;
           return(0);
        }
        *NewPosition=i-1;
        return(SeekLength-1);
     }
  }
}

static int EditBufferSearchExtraChar(Wchar *Source,TEXTTYPE Position,
                                Wchar CharCode)
{
  TEXTTYPE i;

  for (i=Position-1;i>=0;i--)
  {
      if (Source[i]==CharCode)
         return(i);
  }
  return(-1);
}

int EditBufferSearchNextExtraChar(Wchar *Source,TEXTTYPE Position,
                             TEXTTYPE RightBound,Wchar CharCode)
{
  TEXTTYPE i;

  for (i=Position;i<RightBound;i++)
  {
      if (Source[i]==CharCode)
         return(i);
  }
  return(-1);
}

int TextBoxInsertString(HBOX HTextBox,TEXTTYPE Position,Wchar *String,
                        TEXTTYPE DestLength)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  if (TextBoxGetTextHandle(TextBox)==0)
  {
     TextBoxSetTextHandle(TextBox,HandleAlloc(TEXTBLOCKSIZE*sizeof(Wchar),0));
     TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
     if (TextBlock==NULL)
     {
        HandleFree(TextBoxGetTextHandle(TextBox));
        return(OUTOFMEMORY);
     }
     else
     {
        if (TextBoxGetBoxType(TextBox)==TEXTBOX)
        {
           *TextBlock=0;
           TextBoxSetTextLength(TextBox,1);
        }
        else
        if (TextBoxGetBoxType(TextBox)==TABLEBOX)
           TableTextInitial(HTextBox);
     }
     HandleUnlock(TextBoxGetTextHandle(TextBox));
     TextBoxSetBlockLength(TextBox,TEXTBLOCKSIZE);
     SetAllLinkBoxTextHandle(HTextBox);
  }
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }

  if (TextBoxGetTextLength(TextBox)+DestLength>=TextBoxGetBlockLength(TextBox))
  {                                    /* Realloc text block */
     HANDLE NewHandle;
     unsigned int NewSize;

     NewSize=(TextBoxGetTextLength(TextBox)+DestLength+TEXTBLOCKSIZE-1);
   /*------------    ByHance, for test, 95.9,26 -- */
     if (NewSize>MAXBUFFERLENGTH)
        NewSize=MAXBUFFERLENGTH;
     else
     /*--------------*/
//        NewSize=(NewSize/TEXTBLOCKSIZE)*TEXTBLOCKSIZE;
     NewSize=(NewSize/TEXTBLOCKSIZE+1)*TEXTBLOCKSIZE;
     HandleUnlock(TextBoxGetTextHandle(TextBox));
     if (NewSize<=TextBoxGetTextLength(TextBox)+DestLength)
     {
        HandleUnlock(ItemGetHandle(HTextBox));
        return(0);
     }
     NewHandle=HandleRealloc(TextBoxGetTextHandle(TextBox),sizeof(short)*NewSize);
     if (NewHandle)
     {
        TextBoxSetTextHandle(TextBox,NewHandle);
        TextBoxSetBlockLength(TextBox,NewSize);
        TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
        if (TextBlock==NULL)
        {
           HandleUnlock(ItemGetHandle(HTextBox));
           return(0);
        }
        SetAllLinkBoxTextHandle(HTextBox);
     }
     else
     {
        HandleUnlock(ItemGetHandle(HTextBox));
        return(OUTOFMEMORY);
     }
  }

  DestLength=EditBufferInsertString(TextBlock,Position,String,
             &(TextBoxGetTextLength(TextBox)),DestLength,MAXBUFFERLENGTH);
  SetAllLinkBoxTextHandle(HTextBox);
  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));
  FileSetModified();
  return(DestLength);
}

int TextBoxDeleteString(HBOX HTextBox,TEXTTYPE Position,TEXTTYPE DestLength)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;
  int i;
  HBOX InsertHBox;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }

  i=Position;
  do
  {
     InsertHBox=EditBufferSearchNextAttribute(TextBlock,i,
                TextBoxGetTextLength(TextBox),INSERTBOX,&i);
     if (i>0&&i<Position+DestLength&&InsertHBox)
     {
        BoxDelete(InsertHBox);
        i++;
     } else break;
  }  while( i<Position+DestLength && i<TextBoxGetTextLength(TextBox) );

/* Dg Changed in 1996,2. MayBy Tempture
  if (TextBoxGetBoxType(TextBox)==TABLEBOX)
  {
     i=Position;
     TABCount=0;
     do
     {
        i=EditBufferSearchNextExtraChar(TextBlock,i,
              TextBoxGetTextLength(TextBox),TAB);
  //    if (i>0)              Why? Dg
        if (Position<=i && i<Position+DestLength)     // Dg Changed in 1996,2
           TABCount++;
        else
           break;
     } while( i<Position+DestLength && i<TextBoxGetTextLength(TextBox) );
  }
*/

  if (DestLength>=TextBoxGetTextLength(TextBox))
     DestLength=TextBoxGetTextLength(TextBox)-1;
  DestLength=EditBufferDeleteString(TextBlock,Position,DestLength,
                                    &(TextBoxGetTextLength(TextBox)));

/* Dg Change This in 1996,2, Maybe Tempture
  if (TextBoxGetBoxType(TextBox)==TABLEBOX&&TABCount>0)
  {
     Wchar *TABKeys;

     TABKeys=(Wchar *)malloc(sizeof(short)*TABCount);
     for (i=0;i<TABCount;i++)
         TABKeys[i]=TAB;
     TABKeys[i]=0;
     EditBufferInsertString(TextBlock,Position,TABKeys,
             &(TextBoxGetTextLength(TextBox)),TABCount,MAXBUFFERLENGTH);
     free(TABKeys);
  }
*/

  if (TextBoxGetBlockLength(TextBox)-TextBoxGetTextLength(TextBox)>=TEXTBLOCKSIZE)
  {                                    /* Realloc text block */
     HANDLE NewHandle;
     unsigned int NewSize;

     NewSize=TextBoxGetTextLength(TextBox)+TEXTBLOCKSIZE-1;
     if (NewSize>MAXBUFFERLENGTH)
        NewSize=MAXBUFFERLENGTH;
     else
     if (NewSize<TEXTBLOCKSIZE)
        NewSize=TEXTBLOCKSIZE;
//     NewSize=(NewSize/TEXTBLOCKSIZE)*TEXTBLOCKSIZE;
     NewSize=(NewSize/TEXTBLOCKSIZE+1)*TEXTBLOCKSIZE;
     HandleUnlock(TextBoxGetTextHandle(TextBox));
     NewHandle=HandleRealloc(TextBoxGetTextHandle(TextBox),sizeof(short)*NewSize);
     if (NewHandle)
     {
        TextBoxSetTextHandle(TextBox,NewHandle);
        TextBoxSetBlockLength(TextBox,NewSize);
        TextBox=HandleLock(TextBoxGetTextHandle(TextBox));
        if (TextBox==NULL)
        {
           HandleUnlock(ItemGetHandle(HTextBox));
           return(0);
        }
        SetAllLinkBoxTextHandle(HTextBox);
     }
     else
     {
        HandleUnlock(ItemGetHandle(HTextBox));
        return(0);
     }
  }
  else
     HandleUnlock(TextBoxGetTextHandle(TextBox));
  SetAllLinkBoxTextHandle(HTextBox);
  HandleUnlock(ItemGetHandle(HTextBox));
  FileSetModified();
  return(DestLength);
}

int TextBoxExchangeString(HBOX HTextBox,TEXTTYPE Position,Wchar *String,
                          TEXTTYPE ExchangeLength,TEXTTYPE DestLength)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;
  TEXTTYPE ChangeLength;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }

  ChangeLength=DestLength-ExchangeLength;
  if ((ChangeLength>0)&&(TextBoxGetTextLength(TextBox)+ChangeLength>=
      TextBoxGetBlockLength(TextBox)))
  {                                    /* Realloc text block */
     HANDLE NewHandle;
     unsigned int NewSize;

     NewSize=(TextBoxGetTextLength(TextBox)+ChangeLength+TEXTBLOCKSIZE-1);
     if (NewSize>MAXBUFFERLENGTH)
        NewSize=MAXBUFFERLENGTH;
     else
     if (NewSize<TEXTBLOCKSIZE)
        NewSize=TEXTBLOCKSIZE;

//     NewSize=(NewSize/TEXTBLOCKSIZE)*TEXTBLOCKSIZE;
     NewSize=(NewSize/TEXTBLOCKSIZE+1)*TEXTBLOCKSIZE;
     HandleUnlock(TextBoxGetTextHandle(TextBox));
     NewHandle=HandleRealloc(TextBoxGetTextHandle(TextBox),sizeof(short)*NewSize);
     if (NewHandle)
     {
        TextBoxSetTextHandle(TextBox,NewHandle);
        TextBoxSetBlockLength(TextBox,NewSize);
        TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
        if (TextBlock==NULL)
        {
           HandleUnlock(ItemGetHandle(HTextBox));
           return(0);
        }
        SetAllLinkBoxTextHandle(HTextBox);
     }
     else
     {
        HandleUnlock(ItemGetHandle(HTextBox));
        return(0);
     }
  }

  DestLength=EditBufferExchangeString(TextBlock,Position,
                                      ExchangeLength,String,
                                      &(TextBoxGetTextLength(TextBox)),
                                      DestLength,MAXBUFFERLENGTH);

  if ((ChangeLength<0)&&(TextBoxGetBlockLength(TextBox)
      -TextBoxGetTextLength(TextBox)>=TEXTBLOCKSIZE))
  {                                    /* Realloc text block */
     HANDLE NewHandle;
     unsigned int NewSize;

     NewSize=(TextBoxGetTextLength(TextBox)+TEXTBLOCKSIZE-1);
     if (NewSize>MAXBUFFERLENGTH)
        NewSize=MAXBUFFERLENGTH;
     else
     if (NewSize<TEXTBLOCKSIZE)
        NewSize=TEXTBLOCKSIZE;

//     NewSize=(NewSize/TEXTBLOCKSIZE)*TEXTBLOCKSIZE;
     NewSize=(NewSize/TEXTBLOCKSIZE+1)*TEXTBLOCKSIZE;
     HandleUnlock(TextBoxGetTextHandle(TextBox));
     NewHandle=HandleRealloc(TextBoxGetTextHandle(TextBox),sizeof(short)*NewSize);
     if (NewHandle)
     {
        TextBoxSetTextHandle(TextBox,NewHandle);
        TextBoxSetBlockLength(TextBox,NewSize);
        TextBox=HandleLock(TextBoxGetTextHandle(TextBox));
        if (TextBox==NULL)
        {
           HandleUnlock(ItemGetHandle(HTextBox));
           return(0);
        }
        SetAllLinkBoxTextHandle(HTextBox);
     }
     else
     {
        HandleUnlock(ItemGetHandle(HTextBox));
        return(0);
     }
  }
  else
     HandleUnlock(TextBoxGetTextHandle(TextBox));

  SetAllLinkBoxTextHandle(HTextBox);
  HandleUnlock(ItemGetHandle(HTextBox));
  FileSetModified();
  return(DestLength);
}

int TextSearchAttribute(HBOX HTextBox,TEXTTYPE Position,
                        char SearchAttribute,TEXTTYPE *AttributePosition)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;
  int Result;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     // if (SearchAttribute!=PARAGRAPHALIGN)    ByHance,96,1.18
     //if (SearchAttribute!=PARAGRAPHALIGN && SearchAttribute!=ROWGAP)
     //By zjh  add last condition
     if (SearchAttribute!=VPARAGRAPHALIGN &&SearchAttribute!=PARAGRAPHALIGN && SearchAttribute!=ROWGAP && SearchAttribute!=COLGAP )
        *AttributePosition=0;
     if (SearchAttribute==CHARSIZE || SearchAttribute==CHARHSIZE)
        if(fEditor)
            return(160);
        else
            return(DEFAULTCHARSIZE);

     return(0);
  }

  if (Position<0) Position=0;
  Result=EditBufferSearchAttribute(TextBlock,Position,
                                   SearchAttribute,AttributePosition);
  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));

  if (Result==-1)
  {
     // if (SearchAttribute!=PARAGRAPHALIGN)
     //if (SearchAttribute!=PARAGRAPHALIGN && SearchAttribute!=ROWGAP)
     // By zjh add last condition
     if (SearchAttribute!=VPARAGRAPHALIGN && SearchAttribute!=PARAGRAPHALIGN && SearchAttribute!=ROWGAP && SearchAttribute!=COLGAP)
        *AttributePosition=0;
     switch (SearchAttribute)
     {
       case CHARSIZE:
            if(fEditor)
                Result=160;
            else
                Result=DEFAULTCHARSIZE;
            break;
       case CHARHSIZE:
            Result=DEFAULTCHARHSIZE;
            break;
       case CHARFONT:
            Result=DEFAULTCHARFONT;
            break;
       case CHARSLANT:
            Result=DEFAULTCHARSLANT;
            break;
       case CHARCOLOR:
            Result=DEFAULTCHARCOLOR;
            break;
       case PARAGRAPHALIGN:
            Result=DEFAULTPARAGRAPHALIGN;
            break;
       case VPARAGRAPHALIGN:
            Result=DEFAULTVPARAGRAPHALIGN;
            break;
       case ROWGAP:
            Result=DEFAULTROWGAP;
            break;
       case COLGAP:                     //By zjh
            Result=DEFAULTCOLGAP;
            break ;
       case UPDOWN:
            Result=DEFAULTUPDOWN;
            break;
       case SUBLINE:                    //ATTRADD
            Result=DEFAULTSUBLINE;
            break;                      //End
       case SUPERSCRIPT:
            Result=DEFAULTSUPERSCRIPT;
            break;
       case SUBSCRIPT:
            Result=DEFAULTSUBSCRIPT;
            break;
     }
  }

 #ifdef USE_FONTSIZE_FACT
  else
  {
     if ((SearchAttribute==CHARSIZE)||(SearchAttribute==CHARHSIZE))
        Result*=FONTSIZEFACT;
  }
 #endif

  return(Result);
}

int NewTextSearchAttribute(HBOX HTextBox,TEXTTYPE Position)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;
  int Result,i;

  if (Position<0) Position=0;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(0);
  }

  for (i=Position;i>=0;i--)
  {
     if (Cisctext(TextBlock[i]))  {Result=1; goto bye; }
     if (Cisascii(TextBlock[i]))  {Result=2; goto bye; }
  }

  i=Position;
  while (TextBlock[i])
  {
     if (Cisctext(TextBlock[i]))  {Result=1; goto bye; }
     if (Cisascii(TextBlock[i]))  {Result=2; goto bye; }
     i++;
  }
  Result=0;

  bye:
  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));

  return(Result);
}

int TextSearchCFont(HBOX HTextBox,TEXTTYPE Position,TEXTTYPE *AttributePosition)
{
  int SearchAttribute=0;

  if (Position<0) Position=0;

  *AttributePosition=Position;
  while (*AttributePosition>=0)
  {
    SearchAttribute=TextSearchAttribute(HTextBox,*AttributePosition,CHARFONT,AttributePosition);
    if (SearchAttribute<1024)
       break;
    else
       (*AttributePosition)--;
  }
  if (SearchAttribute>=1024||*AttributePosition<0)
  {
     *AttributePosition=0;
     SearchAttribute=0;
  }
  return(SearchAttribute);
}

int TextSearchEFont(HBOX HTextBox,TEXTTYPE Position,TEXTTYPE *AttributePosition)
{
  int SearchAttribute=1024;

  if (Position<0) Position=0;

  *AttributePosition=Position;
  while (*AttributePosition>=0)
  {
    SearchAttribute=TextSearchAttribute(HTextBox,*AttributePosition,CHARFONT,AttributePosition);
    if (SearchAttribute>=1024)
       break;
    else
       (*AttributePosition)--;
  }
  if (SearchAttribute<1024||*AttributePosition<0)
  {
     *AttributePosition=0;
     SearchAttribute=1024;
  }
  return(SearchAttribute);
}

int TextChangeAttribute(HBOX HTextBox,TEXTTYPE Position,int ChangeLength,
                        int ChangeItem,int Attribute,TEXTTYPE *NewPosition,
                        int *BlockStart,int *BlockEnd)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;
  unsigned short AttributeString[2];
  int LastPosition,NextPosition,MiddlePosition,LastAttribute,NextAttribute,
      MiddleAttribute,LastCharPosition,tmpPosition;
  int Result,TextChangeLength,BlockStartChange,BlockEndChange;
  int /*SaveUndoNumber,*/ChangeLineStart,ChangeLineEnd;
  int bChanged=0,ModifyLen;
  int bHaveDeleteBefore=0;

  if (ChangeLength<0)
       return(0);

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }

  //if (ChangeItem==PARAGRAPHALIGN || ChangeItem==ROWGAP)
  // By zjh add last condition
  if (ChangeItem==VPARAGRAPHALIGN || ChangeItem==PARAGRAPHALIGN || ChangeItem==ROWGAP || ChangeItem==COLGAP)
  {
//     if (TextBoxGetTextLength(TextBox)==0)
     if (TextBoxGetTextLength(TextBox)<=1)
     {
      /*- changed byHance -----*/
       Result=0;
       goto error_exit;
        // ReturnOK();
     }

     //LastPosition=EditBufferSearchExtraChar(TextBlock,Position,ENTER)+1;
     LastPosition=EditBufferSearchExtraChar(TextBlock,Position,ENTER)+1;
     tmpPosition=EditBufferSearchExtraChar(TextBlock,Position,TAB)+1;
     if(tmpPosition>LastPosition)
        LastPosition=tmpPosition;

     //if (Position+ChangeLength==0&&TextBlock[0]==ENTER)  // only at begin
     if (Position+ChangeLength==0
     && (TextBlock[0]==ENTER || TextBlock[0]==TAB) )
        NextPosition=0;
     else
     if(ChangeLength>0)
     {
        tmpPosition=Position+ChangeLength-1;
        while (tmpPosition>=0)
        {
          if ((GetPreCode(TextBlock[tmpPosition])==ENGLISHCHAR)
              ||(GetPreCode(TextBlock[tmpPosition])>=CHINESECHARS))
             break;        // if it is not attribute char , break;
          tmpPosition--;    // else, search the prev char
        }
        if(tmpPosition<0) tmpPosition=0;

        if (TextBlock[tmpPosition]==ENTER || TextBlock[tmpPosition]==TAB)
           NextPosition=tmpPosition;
        else
           goto lbl_find_next;
     }
     else                              // Search next ENTER or TAB
     {
     lbl_find_next:
        NextPosition=EditBufferSearchNextExtraChar(TextBlock,Position+ChangeLength,
                                  TextBoxGetTextLength(TextBox),ENTER);
        tmpPosition=EditBufferSearchNextExtraChar(TextBlock,Position+ChangeLength,
                                  TextBoxGetTextLength(TextBox),TAB);

        if (NextPosition==-1 || (tmpPosition!=-1 && tmpPosition<NextPosition) )
            NextPosition=tmpPosition;
     }

     if (NextPosition<0)
        NextPosition=TextBoxGetTextLength(TextBox)-1;
     else
        NextPosition++;
     Position=LastPosition;
     ChangeLength=NextPosition-LastPosition;
     bChanged=1;
  }

  //SaveUndoNumber=UndoOperateSum;
  UndoInsertCursorGoto(GlobalTextPosition);
  UndoInsertCursorDefineBlock(GlobalTextBlockStart,GlobalTextBlockEnd);

  if (ChangeItem==CHARFONT)
  {
     if (Attribute>=1024)
     {
        LastAttribute=TextSearchEFont(HTextBox,Position,&LastPosition);
        NextAttribute=TextSearchEFont(HTextBox,Position+ChangeLength,
                                      &NextPosition);
     }
     else
     {
        LastAttribute=TextSearchCFont(HTextBox,Position,&LastPosition);
        NextAttribute=TextSearchCFont(HTextBox,Position+ChangeLength,
                                      &NextPosition);
     }
  }
  else
  {
     LastAttribute=TextSearchAttribute(HTextBox,Position,ChangeItem,
                   &LastPosition);
     NextAttribute=TextSearchAttribute(HTextBox,Position+ChangeLength,
                   ChangeItem,&NextPosition);
  }

  if (ChangeLength==0) { ///////////Jerry, for init when begining of inset
     if (Attribute==LastAttribute)
     {
       Result=0;
       goto error_exit;
     }

     if (ChangeItem==CHARSIZE||ChangeItem==CHARHSIZE) {
        AttributeString[0]=MakeATTRIBUTE(ChangeItem,Attribute/FONTSIZEFACT);
        AttributeString[1]=MakeATTRIBUTE(ChangeItem,LastAttribute/FONTSIZEFACT);
     } else {
        AttributeString[0]=MakeATTRIBUTE(ChangeItem,Attribute);
        AttributeString[1]=MakeATTRIBUTE(ChangeItem,LastAttribute);
     }

     /*--- added ByHance 96,1.19, when at tail, only need 1 tag ---*/
     // if(Position==TextBoxGetTextLength(TextBox)-3)
       //   TextBoxDeleteString(HTextBox,Position+1,1);
     ModifyLen=2;
     if(Position==TextBoxGetTextLength(TextBox)-1)
          ModifyLen=1;
      /*----------- end --------*/
     // Result=TextBoxInsertString(HTextBox,Position,AttributeString,2);
     Result=TextBoxInsertString(HTextBox,Position,AttributeString,ModifyLen);
     if (Result<1)
        goto error_exit;

     *NewPosition = Position+1;
       /*- added byHance -----*/
      Result=1;
             //goto error_exit;
      TextChangeLength=1;
      goto lbl_reformatit;
       /*- end adding, byHance -----*/
  }

  TextChangeLength=BlockStartChange=BlockEndChange=0;
  if (LastAttribute!=Attribute)        /* Change attribute before String */
  {
     if (ChangeItem==CHARSIZE||ChangeItem==CHARHSIZE)
        AttributeString[0]=MakeATTRIBUTE(ChangeItem,Attribute/FONTSIZEFACT);
     else
        AttributeString[0]=MakeATTRIBUTE(ChangeItem,Attribute);

     Result=TextBoxInsertString(HTextBox,Position,AttributeString,1);
     if (Result<1)
        goto error_exit;
      //  return(Result);
     if (Position<=*NewPosition)
        TextChangeLength++;
     if (Position<=*BlockStart)
        BlockStartChange++;
     if (Position<=*BlockEnd)
        BlockEndChange++;
  }

  while (1)                            /* Delete attribute at middle of string */
  {
    if (ChangeItem==CHARFONT)
    {
       if (Attribute>=1024)
          MiddleAttribute=TextSearchEFont(HTextBox,Position+ChangeLength,
                          &MiddlePosition);
       else
          MiddleAttribute=TextSearchCFont(HTextBox,Position+ChangeLength,
                          &MiddlePosition);
    }
    else
       MiddleAttribute=TextSearchAttribute(HTextBox,Position+ChangeLength,
                    //+((LastAttribute!=Attribute)?1:0),
                    ChangeItem,&MiddlePosition);

    //if ((MiddlePosition>Position+((LastAttribute!=Attribute)?1:0))
      //  &&(MiddlePosition<Position+ChangeLength))
    if ((MiddlePosition>Position)&&(MiddlePosition<Position+ChangeLength))
    {
       Result=TextBoxDeleteString(HTextBox,MiddlePosition,1);
       if (Result<1)
          goto error_exit;        //  return(Result);

       bChanged=1;
       if(MiddlePosition==Position+1)
             bHaveDeleteBefore=1;

       ChangeLength--;
       if (MiddlePosition<=*NewPosition)
          TextChangeLength--;
       if (MiddlePosition<=*BlockStart)
          BlockStartChange--;
       if (MiddlePosition<=*BlockEnd)
          BlockEndChange--;
    }
    else
       break;
  }

  //  if(!GlobalNotDisplay)         // hide old block
  DisplayBlock(HTextBox,*BlockStart,*BlockEnd);

  if (LastAttribute==Attribute && NextAttribute==Attribute && !bChanged)
      if(GlobalNotDisplay)
      {       //- added byHance , 95,12.17 --
          Result=0;
          goto error_exit;
           /*- end adding, byHance -----*/
      }
      else
      {
         HandleUnlock(TextBoxGetTextHandle(TextBox));
         HandleUnlock(ItemGetHandle(HTextBox));
         goto lbl_reformatit;
      }

  ModifyLen=0;
  if(LastAttribute!=Attribute)
      ModifyLen=1;

  /*-- Added ByHance: when at tail, need not add tag --*/
  if(Position+ChangeLength+ModifyLen==TextBoxGetTextLength(TextBox)-1)
      goto lbl_merge_head;
    /*--- end added ---*/

  /*-- Restore attribute after string --*/
  if (ChangeItem==CHARSIZE||ChangeItem==CHARHSIZE)
     AttributeString[0]=MakeATTRIBUTE(ChangeItem,NextAttribute/FONTSIZEFACT);
  else
     AttributeString[0]=MakeATTRIBUTE(ChangeItem,NextAttribute);


 /*---------- adjust Block start & end position -----------*/
  Result=TextBoxInsertString(HTextBox,Position+ChangeLength+ModifyLen,
                   AttributeString,1);
  if (Result<1)  {
   error_exit:
     HandleUnlock(TextBoxGetTextHandle(TextBox));
     HandleUnlock(ItemGetHandle(HTextBox));
     return(Result);
  }
  if (Position+ChangeLength<=*NewPosition)
     TextChangeLength++;
  if (Position+ChangeLength<=*BlockStart)
     BlockStartChange++;
  if (Position+ChangeLength<=*BlockEnd)
     BlockEndChange++;

                                       /* Merge same attribute */
  MiddleAttribute=EditBufferSearchNextAttribute(TextBlock,Position+ChangeLength
                      //+1+((LastAttribute!=Attribute)?1:0),
                      +ModifyLen,
                      TextBoxGetTextLength(TextBox)-1,
                      ChangeItem,&MiddlePosition);

  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));

  if( MiddleAttribute==NextAttribute
   //&& MiddlePosition>Position+ChangeLength+1+((LastAttribute!=Attribute)?1:0) )
   && MiddlePosition>Position+ChangeLength+ModifyLen )
  {
     Result=TextBoxDeleteString(HTextBox,MiddlePosition,1);
     if (Result<1)
        return(Result);
     if (MiddlePosition<=*NewPosition)
        TextChangeLength--;
     if (MiddlePosition<=*BlockStart)
        BlockStartChange--;
     if (MiddlePosition<=*BlockEnd)
        BlockEndChange--;
  }

lbl_merge_head:                 // 96,2.2
  if(TextBoxSeekTextPosition(HTextBox,Position,-1,&LastCharPosition)>0
  || (LastCharPosition==0) )
  {                                    /* Merge attribute before string */
     //if (LastCharPosition<LastPosition)
     if (LastCharPosition<LastPosition)
     if( !(bHaveDeleteBefore && LastCharPosition+1==LastPosition) )
     if(LastAttribute!=Attribute)       // 96,2.2
     {
        Result=TextBoxDeleteString(HTextBox,LastPosition,1);
        if (LastPosition<=*NewPosition)
           TextChangeLength--;
        if (LastPosition<=*BlockStart)
           BlockStartChange--;
        if (LastPosition<=*BlockEnd)
           BlockEndChange--;
     }
  }

  *NewPosition+=TextChangeLength;
  if (*BlockStart<*BlockEnd)
  {
     *BlockStart+=BlockStartChange;
     *BlockEnd+=BlockEndChange;
  }

 lbl_reformatit:
  FileSetModified();
  FormatChangeText(HTextBox,Position,TextChangeLength,ChangeLength,
         &ChangeLineStart,&ChangeLineEnd,FALSE);

  if(!GlobalNotDisplay)
  {
     HBOX NewHBox;
     int CursorX,CursorY;

     TextBoxRedraw(HTextBox,ChangeLineStart,ChangeLineEnd, FALSE);

     CursorLocate(HTextBox,&NewHBox,*NewPosition,&CursorX,&CursorY);
  }

  if (*BlockStart<*BlockEnd)
     DisplayBlock(HTextBox,*BlockStart,*BlockEnd);

  //UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  if(!ChangeLength) ChangeLength=1;
  return(ChangeLength);
}

#ifdef UNUSED           // ByHance, 96,1.29
int TextSearchCharPosition(HBOX HTextBox,TEXTTYPE Position)
{
  HBOX MidHBox;
  TextBoxs *TextBox;
  Wchar *TextBlock;
  int Result;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }
  Result=EditBufferSearchCharPosition(TextBlock,
                                                      Position);
  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));
  return(Result);
}

int TextBoxGetAllTextStyle(HBOX HTextBox,TEXTTYPE Position,TextStyles *BlockTextStyle)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;
  int EditBufferSearchPosition;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }

  BlockTextStyle->CharSize=EditBufferSearchAttribute(TextBlock,Position,CHARSIZE,
                                              &EditBufferSearchPosition);
  if (BlockTextStyle->CharSize==-1)
     BlockTextStyle->CharSize=DEFAULTCHARSIZE;

  BlockTextStyle->CharHSize=EditBufferSearchAttribute(TextBlock,Position,CHARHSIZE,
                                              &EditBufferSearchPosition);
  if (BlockTextStyle->CharHSize==-1)
     BlockTextStyle->CharHSize=DEFAULTCHARHSIZE;

  BlockTextStyle->CharSlant=EditBufferSearchAttribute(TextBlock,Position,CHARSLANT,
                                              &EditBufferSearchPosition);
  if (BlockTextStyle->CharSlant==-1)
     BlockTextStyle->CharSlant=DEFAULTCHARSLANT;

  BlockTextStyle->CharFont=EditBufferSearchAttribute(TextBlock,Position,CHARFONT,
                                              &EditBufferSearchPosition);
  if (BlockTextStyle->CharFont==-1)
     BlockTextStyle->CharFont=DEFAULTCHARFONT;

  BlockTextStyle->CharColor=EditBufferSearchAttribute(TextBlock,Position,CHARCOLOR,
                                              &EditBufferSearchPosition);
  if (BlockTextStyle->CharColor==-1)
     BlockTextStyle->CharColor=DEFAULTCHARCOLOR;

  BlockTextStyle->ParagraphAlign=EditBufferSearchAttribute(TextBlock,Position,
                                               PARAGRAPHALIGN,
                                               &EditBufferSearchPosition);
  if (BlockTextStyle->ParagraphAlign==-1)
     BlockTextStyle->ParagraphAlign=DEFAULTPARAGRAPHALIGN;

  //By zjh  9.12
  BlockTextStyle->VParagraphAlign=EditBufferSearchAttribute(TextBlock,Position,
                                               VPARAGRAPHALIGN,
                                               &EditBufferSearchPosition);
  if (BlockTextStyle->VParagraphAlign==-1)
     BlockTextStyle->VParagraphAlign=DEFAULTVPARAGRAPHALIGN;

  BlockTextStyle->RowGap=EditBufferSearchAttribute(TextBlock,Position,
                                               ROWGAP,
                                               &EditBufferSearchPosition);
  if (BlockTextStyle->RowGap==-1)
     BlockTextStyle->RowGap=DEFAULTROWGAP;

  // By zjh ----------start------------
  BlockTextStyle->ColGap=EditBufferSearchAttribute(TextBlock,Position,
                                               COLGAP,
                                               &EditBufferSearchPosition);
  if (BlockTextStyle->ColGap==-1)
     BlockTextStyle->ColGap=DEFAULTCOLGAP;


  BlockTextStyle->SubLine=EditBufferSearchAttribute(TextBlock,Position,
                                               SUBLINE,
                                               &EditBufferSearchPosition);
  if (BlockTextStyle->SuLine==-1)                   //ATTRADD
     BlockTextStyle->SubLine=DEFAULTSUBLINE;


  BlockTextStyle->UpDown=EditBufferSearchAttribute(TextBlock,Position,
                                               UPDOWN,
                                               &EditBufferSearchPosition);
  if (BlockTextStyle->UpDown==-1)
     BlockTextStyle->UpDown=DEFAULTUPDOWN;

  BlockTextStyle->SubLine=EditBufferSearchAttribute(TextBlock,Position,
                                               SUBLINE,
                                               &EditBufferSearchPosition);
  if (BlockTextStyle->SubLine==-1)
     BlockTextStyle->SubLine=DEFAULTSUBLINE;

  //----------add end ------------

  // New Add By Dg in 1996,3
  BlockTextStyle->Superscript=EditBufferSearchAttribute(TextBlock,Position,
                                              SUPERSCRIPT,
                                              &EditBufferSearchPosition);
  if (BlockTextStyle->Superscript==-1)
     BlockTextStyle->Superscript=DEFAULTSUPERSCRIPT;

  BlockTextStyle->Subscript=EditBufferSearchAttribute(TextBlock,Position,
                                              SUBSCRIPT,
                                              &EditBufferSearchPosition);
  if (BlockTextStyle->Subscript==-1)
     BlockTextStyle->Subscript=DEFAULTSUBSCRIPT;
 // Add end

  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));
  ReturnOK();
}
#endif     // UNUSED           // ByHance, 96,1.29

int TextBoxSeekTextPosition(HBOX HTextBox,TEXTTYPE Position,int Length,
                            TEXTTYPE *NewPosition)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;
  int Result,VisualLength;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }
  VisualLength=TextBoxGetTextLength(TextBox);

  if(Position>=VisualLength)
      Position=VisualLength-1;
  Result=EditBufferSeekPosition(TextBlock,Position,
                           Length,NewPosition,VisualLength);
  if(Result<=0) *NewPosition=Position;        // Add ByHance, 96,2.3

  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));
  return(Result);
}

Wchar TextPositionChar(HBOX HTextBox,TEXTTYPE Position)
{
  TextBoxs *TextBox;
  Wchar *TextBlock,ReturnValue;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }
  if (Position<0||Position>=TableBoxGetTextLength(TextBox))
     ReturnValue=0xffff;
  else
     ReturnValue=*(TextBlock+Position);
  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));
  return(ReturnValue);
}

/*
   Initial table box text, insert TABLE in it's text block
*/
static int TableTextInitial(HBOX HTableBox)
{
  PFormBoxs FormBox;
  Wchar *FormTextBlock;
  int i,j;

  FormBox=HandleLock(ItemGetHandle(HTableBox));
  if (FormBox==NULL)
     return(OUTOFMEMORY);
  if (TableBoxGetBoxType(FormBox)!=TABLEBOX)
  {
     HandleUnlock(ItemGetHandle(HTableBox));
     return(OUTOFMEMORY);
  }

  FormTextBlock=HandleLock(TableBoxGetTextHandle(FormBox));
  if (FormTextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTableBox));
     return(OUTOFMEMORY);
  }
  for (i=0;i<TableBoxGetnumLines(FormBox);i++)
      for (j=0;j<TableBoxGetnumCols(FormBox);j++)
      *(FormTextBlock++)=TAB;          // !!! If modify future,
                                       // must let TEXTBLOCKSIZE>
                                       // MAXLINENUMBER*MAXCLOUMNNUMBER
  *(FormTextBlock-1)=*(FormTextBlock)=0;
  TableBoxSetTextLength(FormBox,TableBoxGetnumLines(FormBox)
                        *TableBoxGetnumCols(FormBox));
  HandleUnlock(TableBoxGetTextHandle(FormBox));
  HandleUnlock(ItemGetHandle(HTableBox));
  ReturnOK();
}

/*
   Return:
     >=0 : TextPosition of CellNumber ( Position 0 <==> Cell 0 )
     else : Error or search not found
*/
int TableCellGetTextHead(HBOX HTableBox,int CellNumber)
{
  PFormBoxs FormBox;
  Wchar *FormTextBlock;
  int CellLine,CellColumn,Result,SeekCount,i,SeekPos;

  Result=FBCellToLineCol(HTableBox,CellNumber,&CellLine,&CellColumn);
  if (Result<0)
     return(Result);
  FormBox=HandleLock(ItemGetHandle(HTableBox));
  if (FormBox==NULL)
     return(OUTOFMEMORY);
  if (TableBoxGetBoxType(FormBox)!=TABLEBOX)
  {
     HandleUnlock(ItemGetHandle(HTableBox));
     return(OUTOFMEMORY);
  }

  FormTextBlock=HandleLock(TableBoxGetTextHandle(FormBox));
  if (FormTextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTableBox));
     return(OUTOFMEMORY);
  }

  SeekCount=CellLine*TableBoxGetnumCols(FormBox)+CellColumn;
  for (i=0,SeekPos=0;i<SeekCount;i++)
  {
      Result=EditBufferSearchNextExtraChar(FormTextBlock,SeekPos,
              TableBoxGetTextLength(FormBox),TAB);
      if (Result>=0)
         SeekPos=Result+1;
      else
      {
         SeekPos=Result;
         break;
      }
  }

  HandleUnlock(TableBoxGetTextHandle(FormBox));
  HandleUnlock(ItemGetHandle(HTableBox));
/*   DG Change 1996,1,5
  if (SeekPos>0)
     return(SeekPos+1);
  else
     return(SeekPos);
*/
  return(SeekPos);
}

// New, Wirte By Dg
int TrueCellIsFirstCell(HBOX HTableBox,int TextPosition)
{
  PFormBoxs FormBox;
  Wchar *FormTextBlock;
  int CellLine,CellColumn,Result,SeekCount,SeekPos;

  if (TextPosition==0)
     return (IsFBCellofLineCol(HTableBox,0,0));
  FormBox=HandleLock(ItemGetHandle(HTableBox));
  if (FormBox==NULL)
     return(OUTOFMEMORY);
  if (TableBoxGetBoxType(FormBox)!=TABLEBOX)
  {
     HandleUnlock(ItemGetHandle(HTableBox));
     return(OUTOFMEMORY);
  }

  FormTextBlock=HandleLock(TableBoxGetTextHandle(FormBox));
  if (FormTextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTableBox));
     return(OUTOFMEMORY);
  }

  //SeekPos=TextPosition-1;     By DG
  SeekPos=TextPosition;
  SeekCount=0;
  do
  {
     Result=EditBufferSearchExtraChar(FormTextBlock,SeekPos,TAB);
     if (Result>0)
        SeekPos=Result;
     if (Result>=0)
        SeekCount++;
  }  while (Result>0);

  CellLine=SeekCount/TableBoxGetnumCols(FormBox);
  CellColumn=SeekCount%TableBoxGetnumCols(FormBox);

  HandleUnlock(TableBoxGetTextHandle(FormBox));
  HandleUnlock(ItemGetHandle(HTableBox));

  return(IsFBCellofLineCol(HTableBox,CellLine,CellColumn));
}
/*--------- added end -----------------*/

/*
   Return:
     >=0 : CellNumber included TextPosition ( Position 0 <==> Cell 0 )
     else : Error or search not found
*/
int TableTextGetCellNumber(HBOX HTableBox,int TextPosition)
{
  PFormBoxs FormBox;
  Wchar *FormTextBlock;
  int CellLine,CellColumn,Result,SeekCount,SeekPos;

  if (TextPosition==0)
     return(FBCellofLineCol(HTableBox,0,0));
  FormBox=HandleLock(ItemGetHandle(HTableBox));
  if (FormBox==NULL)
     return(OUTOFMEMORY);
  if (TableBoxGetBoxType(FormBox)!=TABLEBOX)
  {
     HandleUnlock(ItemGetHandle(HTableBox));
     return(OUTOFMEMORY);
  }

  FormTextBlock=HandleLock(TableBoxGetTextHandle(FormBox));
  if (FormTextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTableBox));
     return(OUTOFMEMORY);
  }

  SeekPos=TextPosition;   // SeekPos=TextPosition-1;     By DG
  SeekCount=0;
  do
  {
     Result=EditBufferSearchExtraChar(FormTextBlock,SeekPos,TAB);
     if (Result>0)
        SeekPos=Result;
     if (Result>=0)
        SeekCount++;
  }  while (Result>0);

  CellLine=SeekCount/TableBoxGetnumCols(FormBox);
  CellColumn=SeekCount%TableBoxGetnumCols(FormBox);

  HandleUnlock(TableBoxGetTextHandle(FormBox));
  HandleUnlock(ItemGetHandle(HTableBox));

  return(FBCellofLineCol(HTableBox,CellLine,CellColumn));
}

/*
   Return:
     >0 : Table CellNumber included Text Length
     else : Error or search not found
*/

int TableCellGetTextLength(HBOX HBox,int TableCell)
{
  PFormBoxs FormBox;
  Wchar *FormTextBlock;
  TEXTTYPE Position,NewPosition;

  FormBox=HandleLock(ItemGetHandle(HBox));
  if (FormBox==NULL)
     return(OUTOFMEMORY);
  if (TableBoxGetBoxType(FormBox)!=TABLEBOX)
  {
     HandleUnlock(ItemGetHandle(HBox));
     return(OUTOFMEMORY);
  }

  FormTextBlock=HandleLock(TableBoxGetTextHandle(FormBox));
  if (FormTextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HBox));
     return(OUTOFMEMORY);
  }

  Position=TableCellGetTextHead(HBox,TableCell);
  NewPosition=EditBufferSearchNextExtraChar(FormTextBlock,Position,
              TableBoxGetTextLength(FormBox),TAB);
  if (NewPosition<0)
     NewPosition=TableBoxGetTextLength(FormBox)-1;
  HandleUnlock(TableBoxGetTextHandle(FormBox));
  HandleUnlock(ItemGetHandle(HBox));
  return(NewPosition-Position);
}

void DisplayCellBlock(HBOX HTableBox,int StartCell,int EndCell)
{
  int DisplayCell,i;

  if (StartCell<0||EndCell<0)
     return;

  i=0;
  do
  {
    DisplayCell=FBFindBlockCell(HTableBox,StartCell,EndCell,i++);
    if (DisplayCell>=0)
       XorPutCell(HTableBox,DisplayCell);
  } while (DisplayCell>=0);
}

void CancelCellBlock(HBOX HTableBox)
{
  if (GlobalTableBlockStart<GlobalTableBlockEnd)
  {
     DisplayCellBlock(HTableBox,GlobalTableBlockStart,GlobalTableBlockEnd);
     GlobalTableBlockStart=GlobalTableBlockEnd=-1;
  }
}

/*--- added ByHance, 95,12.13 --*/
static void BoxXYLocateCursor(HBOX HBox,int BoxX,int BoxY,TEXTTYPE *Position)
{
   struct mark_rec *eptr;
   HBOX NewHBox;
   int CursorX,CursorY;

   eptr=BoxXYToPos(HBox,&NewHBox,BoxX,BoxY,Position);
   if (eptr->type == E_LINEFEED) {
     NewHBox=HBox;
     CursorX=eptr->x;
     CursorY=eptr->y-GetLineBottom(eptr);
     CursorLocate2(HBox,&NewHBox,*Position,&CursorX,&CursorY);
     bAtLineFeed=1;
   }
   else
   {
     bAtLineFeed=0;
     CursorLocate(HBox,&NewHBox,*Position,&CursorX,&CursorY);
   }
}
/*--- end,  ByHance, 95,12.13 --*/

int BoxGetCursor(HBOX HBox,int WindowX,int WindowY,TEXTTYPE *Position,
                 int *BlockStart,int *BlockEnd)
{
  TextBoxs *TextBox;
  ORDINATETYPE BoxX,BoxY;
  int CursorX,CursorY;
  HBOX NewHBox;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  switch (TextBoxGetBoxType(TextBox))
  {
   case TEXTBOX:
        if ((TextBoxGetTextHandle(TextBox)>0)
        && (TextBoxGetTextLength(TextBox)>0))
        {
           CancelBlock(HBox,BlockStart,BlockEnd);
           BoxX=WindowXToUserX(WindowX);
           BoxY=WindowYToUserY(WindowY);
           if (TextBoxGetRotateAngle(TextBox))
              Rotate(&BoxX,&BoxY,BoxX,BoxY,
                     TextBoxGetRotateAxisX(TextBox)+TextBoxGetBoxLeft(TextBox),
                     TextBoxGetRotateAxisY(TextBox)+TextBoxGetBoxTop(TextBox),
                     -TextBoxGetRotateAngle(TextBox));
           BoxX-=TextBoxGetBoxLeft(TextBox);
           BoxY-=TextBoxGetBoxTop(TextBox);
           UndoInsertCursorGoto(GlobalTextPosition);
           /*--- ByHance, 95,12.13 --*/
           //BoxXYToPos(HBox,&NewHBox,BoxX,BoxY,Position);
           //CursorLocate(NewHBox,&NewHBox,*Position,&CursorX,&CursorY);
           BoxXYLocateCursor(HBox,BoxX,BoxY,Position);
        }
        else
        {
           *Position=0;
           CursorLocate(HBox,&NewHBox,*Position,&CursorX,&CursorY);
        }
        break;
   case TABLEBOX:
        CancelCellBlock(HBox);
        BoxX=WindowXToUserX(WindowX);
        BoxY=WindowYToUserY(WindowY);
        if (TextBoxGetRotateAngle(TextBox))
           Rotate(&BoxX,&BoxY,BoxX,BoxY,
                  TextBoxGetRotateAxisX(TextBox)+TextBoxGetBoxLeft(TextBox),
                  TextBoxGetRotateAxisY(TextBox)+TextBoxGetBoxTop(TextBox),
                  -TextBoxGetRotateAngle(TextBox));
        BoxX-=TableBoxGetBoxLeft(TextBox);
        BoxY-=TableBoxGetBoxTop(TextBox);
        if (BoxX<=TableBoxGetMinVertline((PFormBoxs)TextBox))
            BoxX=TableBoxGetMinVertline((PFormBoxs)TextBox)+1;
        if (BoxY<=TableBoxGetMinHortline((PFormBoxs)TextBox))
            BoxY=TableBoxGetMinHortline((PFormBoxs)TextBox)+1;
        GlobalTableCell=FBCellofXY(HBox,BoxX,BoxY);
        if (GlobalTableCell<0) GlobalTableCell=0;

        if(TableBoxGetTextHandle((PFormBoxs)TextBox)>0
        && TableBoxGetTextLength((PFormBoxs)TextBox)>0 )
        {
           int textlen,NewPos;

           CancelBlock(HBox,BlockStart,BlockEnd);
           UndoInsertCursorGoto(GlobalTextPosition);
           *Position=TableCellGetTextHead(HBox,GlobalTableCell);

           textlen=TableCellGetTextLength(HBox,GlobalTableCell);
           if (textlen<=0)
               CursorLocate(HBox,&NewHBox,*Position,&BoxX,&BoxY);
           else
           {
               BoxXYLocateCursor(HBox,BoxX,BoxY,&NewPos);
               if(NewPos>*Position+textlen)   // ByHance, 96,4.6
               {        // this cell may be merged,so put it at text_end
                  *Position+=textlen;
                  CursorLocate(HBox,&NewHBox,*Position,&BoxX,&BoxY);
               } else *Position=NewPos;
           }
        }
        else
        {
           *Position=0;
           CursorLocate(HBox,&NewHBox,*Position,&CursorX,&CursorY);
        }
        break;
  }     // switch

  HandleUnlock(ItemGetHandle(HBox));
  ReturnOK();
}

int BoxGetSelectBlock(HBOX HBox,int WindowX,int WindowY,TEXTTYPE *Position)
{
  TextBoxs *TextBox;
  ORDINATETYPE BoxX,BoxY;
  //int CursorX,CursorY;
  HBOX NewHBox;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  BoxX=WindowXToUserX(WindowX);
  BoxY=WindowYToUserY(WindowY);
  if (TextBoxGetRotateAngle(TextBox))
     Rotate(&BoxX,&BoxY,BoxX,BoxY,
            TextBoxGetRotateAxisX(TextBox)+TextBoxGetBoxLeft(TextBox),
            TextBoxGetRotateAxisY(TextBox)+TextBoxGetBoxTop(TextBox),
            -TextBoxGetRotateAngle(TextBox));
  BoxX-=TextBoxGetBoxLeft(TextBox);
  BoxY-=TextBoxGetBoxTop(TextBox);

  switch (TextBoxGetBoxType(TextBox))
  {
   case TEXTBOX:
        if( (TextBoxGetTextHandle(TextBox)>0)
        && (TextBoxGetTextLength(TextBox)>0))
        {
           BoxXYToPos(HBox,&NewHBox,BoxX,BoxY,Position);
        }
        else
           *Position=0;
        break;
   case TABLEBOX:
        if (BoxX<=TableBoxGetMinVertline((PFormBoxs)TextBox))
            BoxX=TableBoxGetMinVertline((PFormBoxs)TextBox)+1;
        if (BoxY<=TableBoxGetMinHortline((PFormBoxs)TextBox))
            BoxY=TableBoxGetMinHortline((PFormBoxs)TextBox)+1;
        GlobalTableCell=FBCellofXY(HBox,BoxX,BoxY);
        if (GlobalTableCell<0) GlobalTableCell=0;

        if(TableBoxGetTextHandle((PFormBoxs)TextBox)>0
        && TableBoxGetTextLength((PFormBoxs)TextBox)>0 )
        {
           int textlen,NewPos;

           *Position=TableCellGetTextHead(HBox,GlobalTableCell);
           textlen=TableCellGetTextLength(HBox,GlobalTableCell);
           if (textlen>0)
           {
               BoxXYToPos(HBox,&NewHBox,BoxX,BoxY,&NewPos);
               if(NewPos>*Position+textlen)   // ByHance, 96,4.6
               {        // this cell may be merged,so put it at text_end
                  *Position += textlen;
               } else *Position=NewPos;
           }
        }
        else
           *Position=0;
        break;
  } /*- end of switch -*/

  HandleUnlock(ItemGetHandle(HBox));
  ReturnOK();
}

int TextBoxSeekNextWord(HBOX HTextBox,TEXTTYPE Position,TEXTTYPE *NewPosition)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;
  int Result,RightBound;
  TEXTTYPE i;
  Wchar Tmp,code;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }
  RightBound=TextBoxGetTextLength(TextBox);

  //---------- xxxxx<sp><sp>xxxxx ------------
  //  position-^
  //  tmp--------^          ^
  //  or  tmp--------^      |
  //  will search to this ==

  i=Position;
  Tmp=TextBlock[i]&ATTRIBUTEPRECODE;
  if(Tmp==HIGHENGLISHCHAR)
  {
     Result=0;
     if(TextBlock[i]==BLANK) Result=1; // space
     i++;
     while(i<RightBound)
     {
         code=TextBlock[i];
         Tmp=code&ATTRIBUTEPRECODE;
         if(Tmp==HIGHENGLISHCHAR)
         {
             if(Result==0)     // last char is char
             {
                code=toupper(code);
                if(code==BLANK) Result=1; // skip blank(s), search next char
                else
                if( (code<'A'||code>'Z') && (code<'0'||code>'9') )
                   break;
             }
             else // Result==1, first char is blank, only stop when find a char
                if(code!=BLANK) break;
         }
         else
         if( Tmp==(INSERTBOX<<ATTRIBUTEBITS) )
             break;
         else
         if( Tmp>=HIGHCHINESECHARS )
         {
             if( Result==0 || code!=0xa1a1 )
                 break;
         }

         i++;
     }
     *NewPosition=i;
     Result=i-Position;
  }
  else
  if(Tmp>=HIGHCHINESECHARS)
  {
     Result=0;
     if(TextBlock[i]==0xa1a1) Result=1; // space
     i++;
     while(i<RightBound)
     {
         code=TextBlock[i];
         Tmp=code&ATTRIBUTEPRECODE;
         if( Tmp==(INSERTBOX<<ATTRIBUTEBITS) )
             break;

         if(Tmp>=HIGHCHINESECHARS)
         {
             if(Result==0)     // last char is char
             {
                if(code==0xa1a1) Result=1; // skip blank(s), search next char
                else
                if( CNoEnd(code) || CNoBegin(code) )
                   break;
             }
             else // Result==1, last char is blank, only stop when find a char
                if(code!=0xa1a1) break;
         }
         else
         if( Tmp==HIGHENGLISHCHAR )
         {
             if(Result==0 || code!=BLANK)
                break;
         }

         i++;
     }

     *NewPosition=i;
     Result=i-Position;
  }
  else
  {
     Result=EditBufferSeekPosition(TextBlock,Position,
                               1,NewPosition,RightBound);
     if(Result<=0) *NewPosition=Position;
  }

  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));
  return(Result);
}

int TextBoxSeekPrevWord(HBOX HTextBox,TEXTTYPE Position,TEXTTYPE *NewPosition)
{
  TextBoxs *TextBox;
  Wchar *TextBlock;
  int Result;
  TEXTTYPE i;
  Wchar Tmp,code;

  TextBox=HandleLock(ItemGetHandle(HTextBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
  if (TextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(HTextBox));
     return(OUTOFMEMORY);
  }

  /*---- search prev first char ----*/
  Result=EditBufferSeekPosition(TextBlock,Position,
                      -1,NewPosition,0);
  if(Result<=0)
     goto err_exit;

  //---------- xxxxx<sp><sp>xxxxx ------------
  //                             ^--- position
  //              ^---tmp
  //   or                 ^-------- tmp
  //           ^------ must search to this position

  i=*NewPosition;
  Tmp=TextBlock[i]&ATTRIBUTEPRECODE;
  if(Tmp==HIGHENGLISHCHAR)
  {
     Result=0;
     if(TextBlock[i]==BLANK) Result=1; // space
     i--;
     while(i>0)
     {
         code=TextBlock[i];
         Tmp=code&ATTRIBUTEPRECODE;
         if(Tmp==HIGHENGLISHCHAR)
         {
             if(Result==0)     // last char is english char
             {    // may be symbol
                code=toupper(code);
                if( (code<'A'||code>'Z') && (code<'0'||code>'9') )
                   break;
             }
             else     // Result==1, last char is blank
                if(code!=BLANK) //break;
                   Result=0;    // skip blank(s), go on find prev word
         }
         else
         if( Tmp==(INSERTBOX<<ATTRIBUTEBITS) )
             break;
         else
         if( Tmp>=HIGHCHINESECHARS )
         {
             if( Result==0 || code!=0xa1a1 )
                 break;
         }

         i--;
     } /*- while -*/
  }
  else
  if(Tmp>=HIGHCHINESECHARS)
  {
     Result=0;
     if(TextBlock[i]==0xa1a1) Result=1; // space
     i--;
     while(i>0)
     {
         code=TextBlock[i];
         Tmp=code&ATTRIBUTEPRECODE;
         if( Tmp==(INSERTBOX<<ATTRIBUTEBITS) )
             break;

         if(Tmp>=HIGHCHINESECHARS)
         {
             if(Result==0)     // last char is Chinese char
             {            // may be symbol
                if( code==0xa1a1 || CNoEnd(code) || CNoBegin(code) )
                   break;
             }
             else     // Result==1, last char is blank
                if(code!=0xa1a1)
                   Result=0;    // skip blank(s), go on find prev word
         }
         else
         if( Tmp==HIGHENGLISHCHAR )
         {
             if(Result==0 || code!=BLANK)
                break;
         }

         i--;
     } /*- while -*/
  }

  if(i>0)       // move to first char's position
      Result=EditBufferSeekPosition(TextBlock,i,1,NewPosition,Position);
  else
  {
      *NewPosition=0;
      Result=1;
  }

 err_exit:
  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HTextBox));
  return(Result);
}
