/*-------------------------------------------------------------------
* Name: findc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#ifdef OLD_VERSION
int find_str(Wchar *str,Wchar *sub_str,long len_str,int Sign)
{ long i;
  int j,len_sub;
  int next_val[200];
  int casesensitiveSign=!(Sign&F_CaseSensitive) ;

  i=0;  j=1;
  next_val[1]=0;
  len_sub=Wstrlen(sub_str);

  /*---- build sub_str's next_val -----*/
  do {
    if( i==0 || sub_str[j]==sub_str[i]
    || (casesensitiveSign&&Wtoupper(sub_str[j])==Wtoupper(sub_str[i])) )
    {
      i++; j++;
      if( sub_str[j]==sub_str[i]
      || (casesensitiveSign&&Wtoupper(sub_str[j])==Wtoupper(sub_str[i])) )
          next_val[j]=next_val[i];
      else
          next_val[j]=i;
    }
    else i=next_val[i];
  } while (j<len_sub);

  /*----- search sub_str according to next_val -------*/
  i=j=0;
  do {
    if( j==0 || str[i]==sub_str[j-1]
    || (casesensitiveSign&&Wtoupper(str[i])==Wtoupper(sub_str[j-1])) )
    {
      i++; j++;
    }
    else
     j=next_val[j];
  } while ((i<len_str)&&(j<=len_sub));

  if (j>len_sub) return(i-len_sub);
  return(-1);
}
#else                  // ByHance, 96,2.9
int find_str(Wchar *str,Wchar *sub_str,long len_str,int Sign)
{
  int i,j,len_sub;
  Wchar code,mcode;
  int caseSign=!(Sign&F_CaseSensitive) ;

  len_sub=Wstrlen(sub_str);
  i=0;
  code=sub_str[0];
  while(i<len_str && (mcode=str[i]) )
  {
     if( code==mcode
     || (caseSign&&Wtoupper(code)==Wtoupper(mcode)) )
     {          // found 1st letter
         for(j=1;j<len_sub;j++)
            if( sub_str[j]!=str[i+j]
            && !(caseSign&&Wtoupper(sub_str[j])==Wtoupper(str[i+j])) )
               goto search_next;
         return i;
     }
   search_next:
     i++;
  }

  return(-1);
}
#endif

int FindText(HBOX *iHBox,FindStructs *aFS,int *Position,
             int *BlockStart,int *BlockEnd)
{
   HBOX HBox;
   int X,Y;
   HBOX wasteHBox;
   TextBoxs *TextBox;
   Wchar *TextBlock;
   int startPos,endPos,findPos,SaveStart,SaveEnd,SavePos;
   int  ubreak,NumFound;
   Wchar FindString[MAXFINDLENGTH];
   Wchar ReplaceToString[MAXFINDLENGTH];

   HBox=GetFirstLinkBox(*iHBox);
   TextBox=HandleLock(ItemGetHandle(HBox));
   if (TextBox==NULL)
           return(OUTOFMEMORY);
   TextBlock=HandleLock(TextBoxGetTextHandle(TextBox)); //commented for test.
   if (TextBlock==NULL)
   {
           HandleUnlock(ItemGetHandle(HBox));
           return(OUTOFMEMORY);
   }

   MakeWchar((char*)aFS->ReplaceToString,ReplaceToString);
   MakeWchar((char*)aFS->FindString,FindString);

  /*---------------------------------  ByHance, 95,12.20
   if (FindFromHead(*aFS))
     if (!FindSelectRange(*aFS))
      startPos=0;
     else
      startPos=*BlockStart;
   else
     if (!FindSelectRange(*aFS))
      startPos=*Position;
     else
      startPos=(*Position<*BlockStart)? *BlockStart:*Position;
    --------------------------------*/
   //Locate the start Pos
   if (FindFromHead(*aFS))
      startPos=0;
   else
   if (FindSelectRange(*aFS))
   {
      if(*BlockStart>=*BlockEnd)        // if no block defined, error
      {
         MessageBox(GetTitleString(WARNINGINFORM),
                "请先定义文本块,否则,不要\n使用<选定范围>进行查找!",
                1,1);
         goto find_exit;
      }
      startPos=*BlockStart;     // search from block start
   }
   else // search from current position
     startPos=*Position;

   //Locate the end Pos
   if (!FindSelectRange(*aFS) || FindFromHead(*aFS))
      endPos=TextBox->TextLength;
   else
      endPos=*BlockEnd;


   findPos=startPos;
   SaveStart=GlobalTextBlockStart;
   SaveEnd=GlobalTextBlockEnd;
   SavePos=*Position;
   TextCursorOff();

   CancelBlock(HBox,&GlobalTextBlockStart,&GlobalTextBlockEnd);
   ubreak=NumFound=0;

   do
   {
     int oldPos=findPos;
     findPos=find_str(TextBlock+oldPos,FindString,endPos-oldPos,
                      FindCaseSensitive(*aFS)|FindWordOnly(*aFS) );
   /*---- findPos = offset from oldPos ---*/
     if (findPos==-1)
     {
       char str[24];
       if(NumFound)
       {
          strcpy(str, "替换已完成!");
       }
       else
          strcpy(str, "未找到所需字串!");
       MessageBox(GetTitleString(WARNINGINFORM),str,1,1);
       break;
     }

     NumFound++;

     if (FindDoReplace(*aFS))
        endPos+=Wstrlen(ReplaceToString)-Wstrlen(FindString);

     findPos+=oldPos;
     *Position=findPos;
     //CursorLocate(HBox,&wasteHBox,findPos,&X,&Y);

     if (FindDoReplace(*aFS))
     {
        int start,end;

        start=findPos;
        end =findPos +Wstrlen(FindString);

        GlobalTextBlockStart=start;
        GlobalTextBlockEnd=end;
        if (FindPrompt(*aFS))
        {
            // CancelBlock(HBox,&GlobalTextBlockStart,&GlobalTextBlockEnd);
            DisplayBlock(HBox,start,end);
            switch (MessageBox("替换","替换此处字串吗?",3,1))
            {
                case 0: //confirm OK
                     goto doreplace;

                case 1: //cancel
                     //CancelBlock(HBox,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                     ubreak=TRUE;
                     break;
                case 2://skip
                     CancelBlock(HBox,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                     break;
                default:  // "somethine wrong"
                    break;
            }
        }
        else
        {
          doreplace:
           TextBoxKey(HBox,&HBox,findPos,&findPos,
                      Wstrlen(ReplaceToString),
                      &GlobalTextBlockStart,&GlobalTextBlockEnd,ReplaceToString);
        }
     }//if we replace
     else   // only to find
        *Position+=Wstrlen(FindString);

   } while ( !ubreak && FindRepaceALL(*aFS));

   CancelBlock(HBox,&GlobalTextBlockStart,&GlobalTextBlockEnd);
   GlobalTextBlockStart=SaveStart;
   GlobalTextBlockEnd=SaveEnd;
   if(SaveStart<SaveEnd)
   {
      *Position=SavePos;
      DisplayBlock(HBox,SaveStart,SaveEnd);
   }
   CursorLocate(HBox,&wasteHBox,*Position,&X,&Y);

 find_exit:
   HandleUnlock(TextBoxGetTextHandle(TextBox));
   HandleUnlock(ItemGetHandle(HBox));

   return(NumFound);
}

void FindorReplaceNext(HBOX *HBox,FindStructs *FindStruct,int *Position,
                      int *BlockStart,int *BlockEnd)
{
  FindStructs aFS=*FindStruct;

  //ok we should start at char pos
  FindClearFromHeadSign(aFS);
  FindText(HBox,&aFS,Position,BlockStart,BlockEnd);
}
