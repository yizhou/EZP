#include <assert.h>
/*-------------------------------------------------------------------
* Name: form.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

/*------
#undef assert
#define assert(p) 0
-------*/

static void exchange(int *pi1,int *pi2)
{
   int tmp;
   tmp=*pi1;
   *pi1=*pi2;
   *pi2=tmp;
}

/*  draw a line with Box coordinate. */
static void Boxline(PTextBoxs pBox,int Style,int x0,int y0,int x1,int y1)
{
   int x0Win,x1Win,y0Win,y1Win;
   y0Win=BoxYToWindowY(y0,pBox);
   y1Win=BoxYToWindowY(y1,pBox);
   x0Win=BoxXToWindowX(x0,pBox);
   x1Win=BoxXToWindowX(x1,pBox);

   if(!PrintingSign)
   {
      switch(Style)
      {
        case LINE_NORMAL:        //Normal
          line(x0Win,y0Win,x1Win,y1Win);
          break;
        case LINE_BOLD:         // Bold
          line(x0Win,y0Win,x1Win,y1Win);
          if(y0==y1)             /*- Hori -*/
          {
            line(x0Win,y0Win+1,x1Win,y1Win+1);
          }
          else                   /*- Vert -*/
          {
            line(x0Win+1,y0Win,x1Win+1,y1Win);
          }
          break;
        case LINE_DBL:         //  Double
          if(y0==y1)             /*- Hori -*/
          {
            line(x0Win,y0Win-1,x1Win,y1Win-1);
            line(x0Win,y0Win+1,x1Win,y1Win+1);
          }
          else                   /*- Vert -*/
          {
            line(x0Win-1,y0Win,x1Win-1,y1Win);
            line(x0Win+1,y0Win,x1Win+1,y1Win);
          }
          break;
        default:        // None
          break;
      } /*- end of switch -*/
   }
   else       /*-- now, is printing --*/
   {
    /*------------------------
      if(y0==y1)  printer->printScanLine(x0Win,x1Win,y1Win,&SysDc);
      else  //if(x0==x1)       // x0 must be equ to x1
      {
         int y;
         for(y=y0Win;y<y1Win;y++) printer->printScanLine(x0Win,x0Win,y,&SysDc);
      }
     --------------------------*/
     int width=2;      // 2 pixel

      // if(!width) width=1;

      SetDeviceColor(EGA_BLACK,1);
      switch(Style)
      {
        case LINE_NORMAL:        //Normal
          WithWidthLine(&SysDc,x0Win,y0Win,x1Win,y1Win,width,0,0,0);
          break;
        case LINE_BOLD:         // Bold
          WithWidthLine(&SysDc,x0Win,y0Win,x1Win,y1Win,3*width,0,0,0);
          break;
        case LINE_DBL:         //  Double
          if(y0==y1)             /*- Hori -*/
          {
            WithWidthLine(&SysDc,x0Win,y0Win-width,x1Win,y1Win-width,width,0,0,0);
            WithWidthLine(&SysDc,x0Win,y0Win+width,x1Win,y1Win+width,width,0,0,0);
          }
          else                   /*- Vert -*/
          {
            WithWidthLine(&SysDc,x0Win-width,y0Win,x1Win-width,y1Win,width,0,0,0);
            WithWidthLine(&SysDc,x0Win+width,y0Win,x1Win+width,y1Win,width,0,0,0);
          }
          break;
        default:        // None
          break;
      } /*- end of switch -*/
   }
}

#ifdef UNUSED
static void Boxrectangle(PTextBoxs pBox,int x0,int y0,int x1,int y1)
{
   int x0Win,x1Win,y0Win,y1Win,y;

   y0Win=BoxYToWindowY(y0,pBox);
   y1Win=BoxYToWindowY(y1,pBox);
   x0Win=BoxXToWindowX(x0,pBox);
   x1Win=BoxXToWindowX(x1,pBox);

   if(!PrintingSign)
   {
      rectangle(x0Win,y0Win,x1Win,y1Win);
      rectangle(x0Win+1,y0Win+1,x1Win-1,y1Win-1);
   }
   else
   {
      SetDeviceColor(EGA_BLACK,1);
      printer->printScanLine(x0Win,x1Win,y0Win,&SysDc);
      printer->printScanLine(x0Win,x1Win,y0Win+1,&SysDc);
      printer->printScanLine(x0Win,x1Win,y1Win-1,&SysDc);
      printer->printScanLine(x0Win,x1Win,y1Win,&SysDc);
      for(y=y0Win+2;y<y1Win-1;y++)
      {
          printer->printScanLine(x0Win,x0Win+1,y,&SysDc);
          printer->printScanLine(x1Win-1,x1Win,y,&SysDc);
      }
   }
}

void DrawCellSlip(PBoxs pBox,int x0,int y0,int x1,int y1,int type)
{
   int x0Mid, y0Mid;
   int x0Win,x1Win,y0Win,y1Win;

   if(type==0 || type>2)
      return;

   y0Win=BoxYToWindowY(y0,pBox);
   y1Win=BoxYToWindowY(y1,pBox);
   x0Win=BoxXToWindowX(x0,pBox);
   x1Win=BoxXToWindowX(x1,pBox);
   switch (type)
   {
      case 1:
        line(x0Win,y0Win,x1Win,y1Win);
        break;
      case 2:
        x0Mid=(x0Win+x1Win)/2;
        y0Mid=(y0Win+y1Win)/2;
        line(x0Mid,y0Win,x1Win,y1Win);
        line(x0Win,y0Mid,x1Win,y1Win);
        break;
   }
}
#endif

/* find the first cell's number of the iCell. */
static int FirstCell(CELL * pCellTable,int iCell)
{
    if (pCellTable[iCell].iFirst==FIRSTCELL)
       return iCell;
    else
       return pCellTable[iCell].iFirst;
}

/* Initialize a cell.*/
static void InitACell(CELL * pCell,int hParentBox,int iSelf,int iFirst)
{
   // pCell->hParentBox=hParentBox;
   pCell->numLines=1;
   pCell->numCols=1;
   pCell->iFirst=iFirst;
   pCell->iSelf=iSelf;
   pCell->bSlip=0;
}

static void FBInitTexts(HBOX hFormBox)
{
    Wchar KeyString[2];
    TEXTTYPE Position=0,Length=1;
    int Result,StartChangeLine,ChangeLines;

    /*------ text block is null, so, we must cheat it with blank -----*/
    KeyString[0]=BLANK;
    KeyString[1]=0;
    Result=TextBoxInsertString(hFormBox,Position,&KeyString[0],Length);
    FormatInsertText(hFormBox,Position,Result,&StartChangeLine,&ChangeLines,FALSE);
    Result=TextBoxDeleteString(hFormBox,Position,Length);
    FormatDeleteText(hFormBox,Position,Result,&StartChangeLine,&ChangeLines,FALSE);
}

static void InitACellText(int iCell,HBOX hFormBox)
{
    int pos = TableCellGetTextHead(hFormBox,iCell);
    Wchar KeyString[2];
    int Result,StartChangeLine,ChangeLines;

    KeyString[0]=MakeATTRIBUTE(VPARAGRAPHALIGN,ALIGNVCENTRE);
    KeyString[1]=MakeATTRIBUTE(PARAGRAPHALIGN,ALIGNCENTRE);
    Result=TextBoxInsertString(hFormBox,pos,&KeyString[0],2);
    FormatInsertText(hFormBox,pos,Result,&StartChangeLine,&ChangeLines,FALSE);
}

/*
    Form Box initialize cells.
     Box's fields have been setted when called. This function
     is used to initialize the cells' data in a form box.
*/
int FBInitCells(HFormBoxs hFormBox,int numLines,int numCols)
{
   int i;
   PFormBoxs pFormBox;
   CELL * pCellTable;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

// Compute the hortline[] according to form box's height and number of lines
   assert(numLines<MAXFORMLINE&&numLines>0);
   pFormBox->numLines=numLines;
   for (i=0;i<=numLines;i++)
      pFormBox->hortline[i]=(long)pFormBox->BoxHeight*i/numLines;

// Compute the vertline[] according to form box's width and number of columns
   assert(numCols<MAXFORMLINE&&numCols>0);
   pFormBox->numCols=numCols;
   for (i=0;i<=numCols;i++)
      pFormBox->vertline[i]=(long)pFormBox->BoxWidth*i/numCols;

   pFormBox->hortlineType[0]=pFormBox->hortlineType[numLines]=
      pFormBox->vertlineType[0]=pFormBox->vertlineType[numCols]=LINE_BOLD;

// Allocate a cell table and get its pointer.
   pFormBox->hCellTable=HandleAlloc(numCols*numLines*sizeof(CELL),0);
   if (pFormBox->hCellTable==NULL)
      return(OUTOFMEMORY);
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// Initialize all the cells
   for (i=0;i<numLines*numCols;i++)
      InitACell(pCellTable+i,hFormBox,i,FIRSTCELL);

   // Init Cells' text
   FBInitTexts(hFormBox);
   for (i=0;i<numLines*numCols;i++)
      InitACellText(i,hFormBox);

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

//  Insert Text of a Column in the form.
//  NewPos is Insert Pos in Cell, nDistance is...,
//  nNum is the number of Insert Tab
static void FBInsAColText(HFormBoxs hFormBox,int nCell,int nDist, int nNum)
{
    Wchar KeyString[4];
    TEXTTYPE Position;
    int Result,StartChangeLine,ChangeLines;
    int old_undo_sign;


    nCell--;
    if (nCell<0)        // first cell
    {
        Position=0;
        KeyString[0]=MakeATTRIBUTE(VPARAGRAPHALIGN,ALIGNVCENTRE);
        KeyString[1]=MakeATTRIBUTE(PARAGRAPHALIGN,ALIGNCENTRE);
        KeyString[2]=TAB;
    }

    while (nNum>0)
    {
        if(nCell>=0)
        {
            KeyString[0]=TAB;
            KeyString[1]=MakeATTRIBUTE(VPARAGRAPHALIGN,ALIGNVCENTRE);
            KeyString[2]=MakeATTRIBUTE(PARAGRAPHALIGN,ALIGNCENTRE);
            Position=TableCellGetTextHead(hFormBox,nCell)+
                 TableCellGetTextLength(hFormBox,nCell);
        }

        old_undo_sign=DoUndoSign;
        DoUndoSign=1;           // ByDg, 96,4.12
        Result=TextBoxInsertString(hFormBox,Position,&KeyString[0],3);
        DoUndoSign=0;
        FormatInsertText(hFormBox,Position,Result,&StartChangeLine,&ChangeLines,FALSE);
        nCell+=nDist;
        nNum--;
    }
}

/*
   Insert a column in the form. New Column's place is specified
   by iNewCol;
*/
int FBInsACol(HFormBoxs hFormBox,int iNewCol,int CellWidth,BOOL bText)
{
   int iOld,iNew,i,j,k,wNewCol,i0,iOldCol;
   int * ConvertTable;
   PFormBoxs pFormBox;
   CELL * pCellTable;
   CELL * pOldCellTable;
   HANDLE hOldCellTable;
   int SaveUndoNumber;

   SaveUndoNumber=UndoOperateSum;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

// set iNewCol in appropriate range.
   if (iNewCol>pFormBox->numCols)
      iNewCol=pFormBox->numCols;
   else
   if (iNewCol<0)
      iNewCol=0;

// compute new coloumn's width.
   if(CellWidth<=0)
   {
      if (iNewCol==0)
      // if New col is the first Column, its width will be assigned as
      // the width of the original first column.
         wNewCol=pFormBox->vertline[1] - pFormBox->vertline[0];
      else
      // if it is not the first Column, its width will be assigned as
      // the width of its left column.
         wNewCol=pFormBox->vertline[iNewCol] - pFormBox->vertline[iNewCol-1];
   }
   else
      wNewCol=CellWidth;

   UndoInsertTableInsertCol(iNewCol);

// readjust vertline[] field and numCols field.
     // All the columns right to the New Col will just be moved.
     // Their width and linetype will not be changed.
   i=(++pFormBox->numCols);
   for (;i>iNewCol;i--)
   {
      pFormBox->vertline[i]=pFormBox->vertline[i-1]+wNewCol;
      pFormBox->vertlineType[i]=pFormBox->vertlineType[i-1];
   }

   if(iNewCol==0)
      pFormBox->vertlineType[1]=LINE_NORMAL;
   else
   if(iNewCol==pFormBox->numCols-1)
      pFormBox->vertlineType[iNewCol]=LINE_NORMAL;

  // Modify Box Frame  1996,3,2,     add condition ByHance, 96,4.6
   if(TableBoxGetBoxWidth(pFormBox)<pFormBox->vertline[pFormBox->numCols])
   {
      TableBoxSetBoxWidth(pFormBox,TableBoxGetBoxWidth(pFormBox)+wNewCol);
      //BoxChangeAll(GlobalCurrentPage);
      BoxChange(hFormBox,GlobalCurrentPage);
   }


// Get the pointer of old cell table.
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   pOldCellTable=pCellTable;
   hOldCellTable=pFormBox->hCellTable;

// allocate space for new cell table and get its pointer.
   pFormBox->hCellTable=HandleAlloc(pFormBox->numCols*pFormBox->numLines*sizeof(CELL),0);
   if (pFormBox->hCellTable==NULL)
      return(OUTOFMEMORY);
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// move the cells' data of old cell table into new cell table.
   iOld=0;
   ConvertTable=malloc((pFormBox->numCols-1)*pFormBox->numLines*sizeof(int));
   if (ConvertTable==NULL)
      return(OUTOFMEMORY);
   for (i=0;i<pFormBox->numLines;i++)
   {
      for (j=0;j<pFormBox->numCols;j++)
      {
          iNew=i*pFormBox->numCols+j;
          if (j!=iNewCol)
          // It's not a new cell
          {
             ConvertTable[iOld]=iNew;

             pCellTable[iNew]=pOldCellTable[iOld];
             pCellTable[iNew].iSelf=iNew;
             if (pCellTable[iNew].iFirst!=FIRSTCELL)
             {    // it's a merged cell
                pCellTable[iNew].iFirst=ConvertTable[pCellTable[iNew].iFirst];
             }
             else
             {    // it's the first cell of merged cells
                iOldCol=iOld%(pFormBox->numCols-1);
                if (iOldCol<iNewCol&&iOldCol+pCellTable[iNew].numCols>iNewCol)
                {     // New Column is through the merged cells
                   pCellTable[iNew].numCols++;
                }
             }
             iOld++;
          }
          else
          {     // It's a new cell
          // It's difficult to determine new cell's first cell by now,
          // so we set it as uncertain.
             InitACell(&pCellTable[iNew],hFormBox,iNew,UNCERTAINCELL);
          }
      }
   }
   //MemFree(ConvertTable);
   free(ConvertTable);

// free the old cell table.
   HandleUnlock(hOldCellTable);
   HandleFree(hOldCellTable);

// Fill back new cells' iFirst field.
   for (i=0;i<pFormBox->numCols*pFormBox->numLines;i++)
   {
      switch(pCellTable[i].iFirst)
      {
         case FIRSTCELL:         // This is a first cell.
            for (j=0;j<pCellTable[i].numLines;j++)
            {
               for (k=0;k<pCellTable[i].numCols;k++)
               {
                  i0=i+j*pFormBox->numCols+k;
                  if (pCellTable[i0].iFirst==UNCERTAINCELL)
                  // if a new cell belongs to i, correct its iFirst field.
                     pCellTable[i0].iFirst=i;
               }
            }
            break;

         case UNCERTAINCELL:
         // This is a new cell, and it does not belong to other merged cells
            pCellTable[i].iFirst=FIRSTCELL;
            break;
      }
   }

// readjust cells' text, implememted by DG in 1996,2
   if (bText)
       FBInsAColText(hFormBox,iNewCol,pFormBox->numCols,pFormBox->numLines);

   UndoInsertCompose(UndoOperateSum-SaveUndoNumber);

   GlobalTextPosition=0;
   GlobalTableCell=0;
   ReFormatTableText(hFormBox,TRUE);
   {
   HANDLE NewHBox;
   int CursorX, CursorY;
   CursorLocate(hFormBox,&NewHBox,GlobalTextPosition,&CursorX,&CursorY);
   }

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

//  Delete Text of a Col in the form.
//  NewPos is Delete Begin Pos in Cell, nNum is the number of Delete Tab
static int FBDelAColText(HFormBoxs hFormBox,int nCell,int nDist,int nNum)
{
  FormBoxs *FormBox;
  Wchar *FormTextBlock;
  TEXTTYPE Position,NewPosition,TextLength, DestLength, SumDelLength;
  int StartChangeLine,ChangeLines,Result,i;

  FormBox=HandleLock(ItemGetHandle(hFormBox));
  if (FormBox==NULL)
     return(OUTOFMEMORY);
  if (TableBoxGetBoxType(FormBox)!=TABLEBOX)
  {
     HandleUnlock(ItemGetHandle(hFormBox));
     return(OUTOFMEMORY);
  }

  FormTextBlock=HandleLock(TableBoxGetTextHandle(FormBox));
  if (FormTextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(hFormBox));
     return(OUTOFMEMORY);
  }

  TextLength=TableBoxGetTextLength(FormBox);
  SumDelLength=0;
  Position=0;
  while (nCell>0)
  {
      NewPosition=EditBufferSearchNextExtraChar(FormTextBlock,Position,
              TextLength,TAB);
      Position=NewPosition+1;
      nCell--;
  }

  while(nNum>0)
  {
      NewPosition=EditBufferSearchNextExtraChar(FormTextBlock,Position,
                                        TextLength,TAB);
      if (NewPosition>=Position)
          DestLength=NewPosition-Position+1;
      else
          DestLength=TextLength-Position;
      Result=EditBufferDeleteString(FormTextBlock,Position,DestLength,&TextLength);

      SumDelLength+=Result;
      nNum--;

      if(nNum>0)
       for (i=0;i<nDist;i++)
       {
          NewPosition=EditBufferSearchNextExtraChar(FormTextBlock,Position,
                                        TextLength,TAB);
          if (NewPosition>=Position)
              Position=NewPosition+1;
          else
              Position=TextLength-1;
       }
  }

  TableBoxSetTextLength(FormBox,TextLength);

  HandleUnlock(TableBoxGetTextHandle(FormBox));
  HandleUnlock(ItemGetHandle(hFormBox));
  FormatDeleteText(hFormBox,0,SumDelLength,&StartChangeLine,&ChangeLines,FALSE);
  return(NewPosition-Position);
}

/*
   Delete a column in the form.
*/
int FBDelACol(HFormBoxs hFormBox,int iDelCol,BOOL bText)
{
   int iOld,iOldCol,iNew,i,j,wDelCol;
   int iOldFirstCell,iNewFirstCell;
   int *ConvertTable;
   PFormBoxs pFormBox;
   CELL *pCellTable;
   CELL *pOldCellTable;
   HANDLE hOldCellTable;
   int SaveUndoNumber;

   SaveUndoNumber=UndoOperateSum;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

// Can't delete a column if there is only one.
   if (pFormBox->numCols<=1)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

// set iNewCol in appropriate range.
   if (iDelCol>=pFormBox->numCols)
      iDelCol=pFormBox->numCols-1;
   else
   if (iDelCol<0)
      iDelCol=0;

   wDelCol=pFormBox->vertline[iDelCol+1]-pFormBox->vertline[iDelCol];

   UndoInsertTableDeleteCol(iDelCol,wDelCol);

// Modify Box Frame  1996,3,2, deleted ByHance, 96,3.17
//   TableBoxSetBoxWidth(pFormBox,TableBoxGetBoxWidth(pFormBox)-wDelCol);

// readjust vertline[] field and numCols field.
   for (i=iDelCol+1;i<pFormBox->numCols;i++)
   {
   // All the columns right to the New Col will just be moved. Their width
   // will not be changed.
      pFormBox->vertline[i]=pFormBox->vertline[i+1]-wDelCol;
      pFormBox->vertlineType[i]=pFormBox->vertlineType[i+1];
   }
   if(iDelCol==pFormBox->numCols)
      pFormBox->vertlineType[iDelCol-1]=pFormBox->vertlineType[iDelCol];
   pFormBox->numCols--;

// Get the pointer of old cell table.
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }
   pOldCellTable=pCellTable;
   hOldCellTable=pFormBox->hCellTable;

// allocate space for new cell table and get its pointer.
   pFormBox->hCellTable=HandleAlloc(pFormBox->numCols*pFormBox->numLines*sizeof(CELL),0);
   if (pFormBox->hCellTable==NULL)
      return(OUTOFMEMORY);
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// generate convert Table
   ConvertTable=malloc((pFormBox->numCols+1)*pFormBox->numLines*sizeof(int));
   if (ConvertTable==NULL)
      return(OUTOFMEMORY);

   for (i=0;i<pFormBox->numLines;i++)
   {
      for (j=0;j<pFormBox->numCols+1;j++)
      {
         iOld=i*(pFormBox->numCols+1)+j;
         if (j<iDelCol)
            iNew=i*pFormBox->numCols+j;
         else if (j>iDelCol)
            iNew=i*pFormBox->numCols+j-1;
         else
            iNew=-1;

         ConvertTable[iOld]=iNew;
      }
   }

// move the cells' data of old cell table into new cell table.
   iOld=0;
   for (i=0;i<pFormBox->numLines;i++)
   {
      for (j=0;j<pFormBox->numCols;j++)
      {
          iNew=i*pFormBox->numCols+j;
          while(ConvertTable[iOld]==-1) iOld++;

          pCellTable[iNew]=pOldCellTable[iOld];
          pCellTable[iNew].iSelf=iNew;

          if (pCellTable[iNew].iFirst!=FIRSTCELL)
          {        // it's a merged cell
             iOldFirstCell=pCellTable[iNew].iFirst;
             iNewFirstCell=ConvertTable[iOldFirstCell];
             if (iNewFirstCell==-1)
             // its first cell is deleted,in this case,
             // we assign the deleted first cell's right cell as the new first cell.
             {
                iNewFirstCell=ConvertTable[iOldFirstCell+1];
                if (iNewFirstCell==iNew)
                {    // current cell is the first cell now.
                   pCellTable[iNew].iFirst=-1;
                   pCellTable[iNew].numLines=pOldCellTable[iOldFirstCell].numLines;
                   pCellTable[iNew].numCols=pOldCellTable[iOldFirstCell].numCols-1;
                }
                else
                {          // current cell is not the first cell
                   pCellTable[iNew].iFirst=iNewFirstCell;
                }
             }
             else
             {     // its first cell is not deleted
                pCellTable[iNew].iFirst=ConvertTable[iOldFirstCell];
             }
          }
          else
          {   // it's the first cell of merged cells
             iOldCol=iOld%(pFormBox->numCols+1);
             if (iOldCol<=iDelCol&&iOldCol+pCellTable[iNew].numCols-1>=iDelCol)
             {    // Deleted Column is through the merged cells
                pCellTable[iNew].numCols--;
             }
          }
          iOld++;
       }
   }
   //MemFree(ConvertTable);
   free(ConvertTable);

 // free the old cell table.
   HandleUnlock(hOldCellTable);
   HandleFree(hOldCellTable);

 // readjust cells' text, implememted by DG in 1996,2
   if (bText)
      FBDelAColText(hFormBox,iDelCol,pFormBox->numCols,pFormBox->numLines);

   UndoInsertCompose(UndoOperateSum-SaveUndoNumber);

   GlobalTextPosition=0;
   GlobalTableCell=0;
   ReFormatTableText(hFormBox,TRUE);
   {
   HANDLE NewHBox;
   int CursorX, CursorY;
   CursorLocate(hFormBox,&NewHBox,GlobalTextPosition,&CursorX,&CursorY);
   }

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

//  Insert Text of a line in the form.
//  NewPos is Insert Pos in Cell, nNum is the number of Insert Tab
static void FBInsALineText(HFormBoxs hFormBox,int nCell,int nNum)
{
    Wchar KeyString[MAXCLOUMNNUMBER*3+2];
    int i,n;
    TEXTTYPE Position;
    int Result,StartChangeLine,ChangeLines;

    n=3*nNum;

    if (nCell==0)       // first cell
    {
        Position=0;
        for(i=0;i<n;i+=3)
        {
           KeyString[i]=MakeATTRIBUTE(VPARAGRAPHALIGN,ALIGNVCENTRE);
           KeyString[i+1]=MakeATTRIBUTE(PARAGRAPHALIGN,ALIGNCENTRE);
           KeyString[i+2]=TAB;
        }
        Result=TextBoxInsertString(hFormBox,0,&KeyString[0],n);
    }
    else
    {
        for(i=0;i<n;i+=3)
        {
           KeyString[i]=TAB;
           KeyString[i+1]=MakeATTRIBUTE(VPARAGRAPHALIGN,ALIGNVCENTRE);
           KeyString[i+2]=MakeATTRIBUTE(PARAGRAPHALIGN,ALIGNCENTRE);
        }
        nCell--;
        Position=TableCellGetTextHead(hFormBox,nCell)+
                 TableCellGetTextLength(hFormBox,nCell);
        Result=TextBoxInsertString(hFormBox,Position,&KeyString[0],n);
    }
    FormatInsertText(hFormBox,Position,Result,&StartChangeLine,&ChangeLines,FALSE);
}

/*
   Insert a line in the form. New line's place is specified
   by iNewLine;
*/
int FBInsALine(HFormBoxs hFormBox,int iNewLine,int CellHeight,BOOL bText)
{
   int iOld,iNew,i,j,k,hNewLine,i0,iOldLine;
   int * ConvertTable;
   PFormBoxs pFormBox;
   CELL * pCellTable;
   CELL * pOldCellTable;
   HANDLE hOldCellTable;
   int SaveUndoNumber;

   SaveUndoNumber=UndoOperateSum;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

// set iNewLine in appropriate range.
   if (iNewLine>pFormBox->numLines)
      iNewLine=pFormBox->numLines;
   else
   if (iNewLine<0)
      iNewLine=0;

// compute new line's height.
   if(CellHeight<=0)
   {
      if (iNewLine==0)
      // if New line is the first line, its height will be assigned as
      // the height of the original first line.
         hNewLine=pFormBox->hortline[1]-pFormBox->hortline[0];
      else
      // if New line is not the first line, its height will be assigned as
      // the height of its up line.
         hNewLine=pFormBox->hortline[iNewLine]-pFormBox->hortline[iNewLine-1];
   }
   else
      hNewLine=CellHeight;

   UndoInsertTableInsertLine(iNewLine);

   // Modify Box Frame, ByHance, 96,4.6
   TableBoxSetBoxHeight(pFormBox,TableBoxGetBoxHeight(pFormBox)+hNewLine);
   //BoxChangeAll(GlobalCurrentPage);
   BoxChange(hFormBox,GlobalCurrentPage);

// readjust hortline[] field and numLines field.
   // All the Lines down to the New line will just be moved. Their height
   // will not be changed.
   i=(++pFormBox->numLines);
   for (;i>iNewLine;i--)
   {
      pFormBox->hortline[i]=pFormBox->hortline[i-1]+hNewLine;
      pFormBox->hortlineType[i]=pFormBox->hortlineType[i-1];
   }

   if(iNewLine==0)
      pFormBox->hortlineType[1]=LINE_NORMAL;
   else
   if(iNewLine==pFormBox->numLines-1)
      pFormBox->hortlineType[iNewLine]=LINE_NORMAL;


// Get the pointer of old cell table.
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   pOldCellTable=pCellTable;
   hOldCellTable=pFormBox->hCellTable;

// allocate space for new cell table and get its pointer.
   pFormBox->hCellTable=HandleAlloc(pFormBox->numCols*pFormBox->numLines*sizeof(CELL),0);
   if (pFormBox->hCellTable==NULL)
      return(OUTOFMEMORY);
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// move the cells' data of old cell table into new cell table.
   iOld=0;
   ConvertTable=malloc(pFormBox->numCols*(pFormBox->numLines-1)*sizeof(int));
   if (ConvertTable==NULL)
      return(OUTOFMEMORY);
   for (i=0;i<pFormBox->numLines;i++)
   {
      for (j=0;j<pFormBox->numCols;j++)
      {
          iNew=i*pFormBox->numCols+j;
          if (i!=iNewLine)
          {     // It's not a new cell
             ConvertTable[iOld]=iNew;

             pCellTable[iNew]=pOldCellTable[iOld];
             pCellTable[iNew].iSelf=iNew;
             if (pCellTable[iNew].iFirst!=FIRSTCELL)
             {  // it's a merged cell
                pCellTable[iNew].iFirst=ConvertTable[pCellTable[iNew].iFirst];
             }
             else
             {   // it's the first cell of merged cells
                iOldLine=iOld/pFormBox->numCols;
                if (iOldLine<iNewLine&&iOldLine+pCellTable[iNew].numLines>iNewLine)
                {    // New Line is through the merged cells
                   pCellTable[iNew].numLines++;
                }
             }
             iOld++;
          }
          else
          {   // It's a new cell
          // It's difficult to determine new cell's first cell by now
          // set it as uncertain.
             InitACell(&pCellTable[iNew],hFormBox,iNew,UNCERTAINCELL);
          }
      }
   }
   //MemFree(ConvertTable);
   free(ConvertTable);

// free the old cell table.
   HandleUnlock(hOldCellTable);
   HandleFree(hOldCellTable);

// Fill back new cells' iFirst field.
   for (i=0;i<pFormBox->numCols*pFormBox->numLines;i++)
   {
      switch(pCellTable[i].iFirst)
      {
         case FIRSTCELL:         // This is a first cell.
            for (j=0;j<pCellTable[i].numLines;j++)
            {
               for (k=0;k<pCellTable[i].numCols;k++)
               {
                  i0=i+j*pFormBox->numCols+k;
                  if (pCellTable[i0].iFirst==UNCERTAINCELL)
                  // if a new cell belongs to i, fill back its iFirst field.
                     pCellTable[i0].iFirst=i;
               }
            }
            break;

         case UNCERTAINCELL:
         // This is a new cell, and it does not belong to other merged cells
            pCellTable[i].iFirst=FIRSTCELL;
            break;
      }
   }

   //readjust cells' text, implememted by Dg 1996,2
   if (bText)
       FBInsALineText(hFormBox,iNewLine*pFormBox->numCols,pFormBox->numCols);

   GlobalTextPosition=0;
   GlobalTableCell=0;
   ReFormatTableText(hFormBox,TRUE);
   {
   HANDLE NewHBox;
   int CursorX, CursorY;
   CursorLocate(hFormBox,&NewHBox,GlobalTextPosition,&CursorX,&CursorY);
   }

   UndoInsertCompose(UndoOperateSum-SaveUndoNumber);

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

//  Delete Text of a line in the form.
//  NewPos is Delete Begin Pos in Cell, nNum is the number of Delete Tab
// Write By DG in 1996,2
static int FBDelALineText(HFormBoxs hFormBox,int nCell,int nNum)
{
  FormBoxs *FormBox;
  Wchar *FormTextBlock;
  TEXTTYPE Position,NewPosition,TextLength, DestLength, SumDelLength;
  int StartChangeLine,ChangeLines,Result;

  FormBox=HandleLock(ItemGetHandle(hFormBox));
  if (FormBox==NULL)
     return(OUTOFMEMORY);
  if (TableBoxGetBoxType(FormBox)!=TABLEBOX)
  {
     HandleUnlock(ItemGetHandle(hFormBox));
     return(OUTOFMEMORY);
  }

  FormTextBlock=HandleLock(TableBoxGetTextHandle(FormBox));
  if (FormTextBlock==NULL)
  {
     HandleUnlock(ItemGetHandle(hFormBox));
     return(OUTOFMEMORY);
  }

  TextLength=TableBoxGetTextLength(FormBox);
  SumDelLength=0;
  Position=0;
  while (nCell>0)
  {
      NewPosition=EditBufferSearchNextExtraChar(FormTextBlock,Position,
              TextLength,TAB);
      Position=NewPosition+1;
      nCell--;
  }

  while(nNum>0)
  {
      NewPosition=EditBufferSearchNextExtraChar(FormTextBlock,Position,
                                        TextLength,TAB);
      if (NewPosition>=Position)
          DestLength=NewPosition-Position+1;
      else
          DestLength=TextLength-Position;
      Result=EditBufferDeleteString(FormTextBlock,Position,DestLength,&TextLength);
      SumDelLength+=Result;
      nNum--;
  }

  TableBoxSetTextLength(FormBox,TextLength);

  HandleUnlock(TableBoxGetTextHandle(FormBox));
  HandleUnlock(ItemGetHandle(hFormBox));
  FormatDeleteText(hFormBox,Position,SumDelLength,&StartChangeLine,&ChangeLines,FALSE);
  return(NewPosition-Position);
}

/*
   Delete a line from the form.
*/
int FBDelALine(HFormBoxs hFormBox,int iDelLine,BOOL bText)
{
   int iOld,iNew,i,j,hDelLine,iOldLine;
   int iOldFirstCell,iNewFirstCell;
   int *ConvertTable;
   PFormBoxs pFormBox;
   CELL *pCellTable;
   CELL *pOldCellTable;
   HANDLE hOldCellTable;
   int SaveUndoNumber;

   SaveUndoNumber=UndoOperateSum;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

// Can't delete a line if there is only one.
   if (pFormBox->numLines<=1)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

// set iNewCol in appropriate range.
   if (iDelLine>=pFormBox->numLines)
      iDelLine=pFormBox->numLines-1;
   else
   if (iDelLine<0)
      iDelLine=0;

   hDelLine=pFormBox->hortline[iDelLine+1]-pFormBox->hortline[iDelLine];

   UndoInsertTableDeleteLine(iDelLine,hDelLine);

// readjust cells' text, implememted by DG in 1996,2
   if (bText)
       FBDelALineText(hFormBox,iDelLine*pFormBox->numCols,pFormBox->numCols);

// readjust vertline[] field and numCols field.
   for (i=iDelLine+1;i<pFormBox->numLines;i++)
   {
   // All the lines down to the delete line will just be moved. Their width
   // will not be changed.
      pFormBox->hortline[i]=pFormBox->hortline[i+1]-hDelLine;
      pFormBox->hortlineType[i]=pFormBox->hortlineType[i+1];
   }
   if(iDelLine==pFormBox->numLines)
      pFormBox->hortlineType[iDelLine-1]=pFormBox->hortlineType[iDelLine];
   pFormBox->numLines--;

// Get the pointer of old cell table.
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   pOldCellTable=pCellTable;
   hOldCellTable=pFormBox->hCellTable;

// allocate space for new cell table and get its pointer.
   pFormBox->hCellTable=HandleAlloc(pFormBox->numCols*pFormBox->numLines*sizeof(CELL),0);
   if (pFormBox->hCellTable==NULL)
      return(OUTOFMEMORY);
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// generate convert Table
   ConvertTable=malloc(pFormBox->numCols*(pFormBox->numLines+1)*sizeof(int));
   if (ConvertTable==NULL)
      return(OUTOFMEMORY);
   for (i=0;i<pFormBox->numLines+1;i++)
   {
      for (j=0;j<pFormBox->numCols;j++)
      {
         iOld=i*pFormBox->numCols+j;
         if (i<iDelLine)
         {
            iNew=i*pFormBox->numCols+j;
         }
         else if (i>iDelLine)
         {
            iNew=(i-1)*pFormBox->numCols+j;
         }
         else
            iNew=-1;

         ConvertTable[iOld]=iNew;
      }
   }

// move the cells' data of old cell table into new cell table.
   iOld=0;
   for (i=0;i<pFormBox->numLines;i++)
   {
      for (j=0;j<pFormBox->numCols;j++)
      {
          iNew=i*pFormBox->numCols+j;
          while(ConvertTable[iOld]==-1) iOld++;

          pCellTable[iNew]=pOldCellTable[iOld];
          pCellTable[iNew].iSelf=iNew;

          if (pCellTable[iNew].iFirst!=FIRSTCELL)
          // it's a merged cell
          {
             iOldFirstCell=pCellTable[iNew].iFirst;
             iNewFirstCell=ConvertTable[iOldFirstCell];
             if (iNewFirstCell==-1)
             // its first cell is deleted,in this case,
             // we assign the deleted first cell's down cell as the new first cell.
             {
                iNewFirstCell=ConvertTable[iOldFirstCell+pFormBox->numCols];
                if (iNewFirstCell==iNew)
                // current cell is the first cell now.
                {
                   pCellTable[iNew].iFirst=-1;
                   pCellTable[iNew].numLines=pOldCellTable[iOldFirstCell].numLines-1;
                   pCellTable[iNew].numCols=pOldCellTable[iOldFirstCell].numCols;
                }
                else
                // current cell is not the first cell
                {
                   pCellTable[iNew].iFirst=iNewFirstCell;
                }
             }
             else
             // its first cell is not deleted
             {
                pCellTable[iNew].iFirst=ConvertTable[iOldFirstCell];
             }
          }
          else
          // it's the first cell of merged cells
          {
             iOldLine=iOld/pFormBox->numCols;
             if (iOldLine<=iDelLine&&iOldLine+pCellTable[iNew].numLines-1>=iDelLine)
             // Deleted Column is through the merged cells
             {
                pCellTable[iNew].numLines--;
             }
          }
          iOld++;
       }
   }
   //MemFree(ConvertTable);
   free(ConvertTable);

// free the old cell table.
   HandleUnlock(hOldCellTable);
   HandleFree(hOldCellTable);

// Modify Box Frame  ByHance, 1996,4.6
   TableBoxSetBoxHeight(pFormBox,TableBoxGetBoxHeight(pFormBox)-hDelLine);
   //BoxChangeAll(GlobalCurrentPage);
   BoxChange(hFormBox,GlobalCurrentPage);

   GlobalTextPosition=0;
   GlobalTableCell=0;
   ReFormatTableText(hFormBox,TRUE);
   {
   HANDLE NewHBox;
   int CursorX, CursorY;
   CursorLocate(hFormBox,&NewHBox,GlobalTextPosition,&CursorX,&CursorY);
   }

   UndoInsertCompose(UndoOperateSum-SaveUndoNumber);

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

int FBMergeCellTexts(HFormBoxs hFormBox,int iMgLine,int iMgCol,int numMgLines,int numMgCols,int maxCols)
{
    TEXTTYPE Position,TextLength;
    int Result,StartChangeLine,ChangeLines;
    int nCell,nFirstCell,i,j;

    nFirstCell=iMgLine*maxCols+iMgCol;

    for (i=iMgLine;i<iMgLine+numMgLines;i++)
    {
        for (j=iMgCol;j<iMgCol+numMgCols;j++)
        {
            nCell=i*maxCols+j;
            if (nCell!=nFirstCell)
            {
                Position=TableCellGetTextHead(hFormBox,nCell);
                TextLength=TableCellGetTextLength(hFormBox,nCell);
                if (TextLength)
                {
                    Result=TextBoxDeleteString(hFormBox,Position,TextLength);
                    FormatDeleteText(hFormBox,Position,Result,&StartChangeLine,&ChangeLines,FALSE);
                }
            }
        }
    }
    ReturnOK();
}

/*
   Form box merge cells.
    iMgLine,iMgCol:First Cell's Line index and Col index.
    numMgLines,numMgCols:Lines and Columns to be merged.
    return value:
      0:succeed.
     -1:can not be merged because the new merged cell block will intersect
        with other merged cell block.
*/
int FBMergeCells(HFormBoxs hFormBox,int iMgLine,int iMgCol,int numMgLines,int numMgCols)
{
   int iFirst,iCur,iUndoCur;
   int i,j;
   int iExtremeLine,iExtremeCol;
   HANDLE hUndoCellTable;
   PFormBoxs pFormBox;
   CELL *pCellTable,*pUndoCellTable;
   int SaveUndoNumber,bSlip;

   if (numMgLines<=1 && numMgCols<=1)
      return(0);

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   if( pFormBox->numCols<iMgCol+numMgCols
   || pFormBox->numLines<iMgLine+numMgLines)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

 // check if can be merged
   for (i=iMgLine;i<iMgLine+numMgLines;i++)
   {
      for (j=iMgCol;j<iMgCol+numMgCols;j++)
      {
      // for every cell that will be merged
         iCur=i*pFormBox->numCols+j;
         if (pCellTable[iCur].iFirst==FIRSTCELL)
         // it's the first cell.
         {
            iExtremeLine=(i+pCellTable[iCur].numLines-1);
            iExtremeCol=j+pCellTable[iCur].numCols-1;
         }
         else
         // it's a merged cell,its first cell may most probabally out of new block.
         {
            iExtremeLine=pCellTable[iCur].iFirst/pFormBox->numCols;
            iExtremeCol=pCellTable[iCur].iFirst%pFormBox->numCols;
         }
         if (!(iExtremeLine>=iMgLine&&iExtremeLine<iMgLine+numMgLines
               &&iExtremeCol>=iMgCol&&iExtremeCol<iMgCol+numMgCols))
         {  // Old Block and New Block Intersect!
                // Merge failed
            HandleUnlock(pFormBox->hCellTable);
            HandleUnlock(ItemGetHandle(hFormBox));
            return -1;
         }
      }
   }

   SaveUndoNumber=UndoOperateSum;
   FBMergeCellTexts(hFormBox,iMgLine,iMgCol,numMgLines,numMgCols,pFormBox->numCols);

   hUndoCellTable=HandleAlloc((numMgCols*numMgLines+1)*sizeof(CELL),0);
   pUndoCellTable=HandleLock(hUndoCellTable);
   if (pUndoCellTable==NULL)
   {
      HandleUnlock(hUndoCellTable);
      return(OUTOFMEMORY);
   }
   for (i=0;i<numMgLines;i++)
   {
      for (j=0;j<numMgCols;j++)
      {
           iUndoCur=i*numMgCols+j;
           iCur=(i+iMgLine)*pFormBox->numCols+(j+iMgCol);
           pUndoCellTable[iUndoCur]=pCellTable[iCur];
      }
   }
   HandleUnlock(hUndoCellTable);
   UndoInsertTableCellMerge(iMgLine,iMgCol,numMgLines,numMgCols,hUndoCellTable);
   UndoInsertCompose(UndoOperateSum-SaveUndoNumber);

 // Reset Slip Flag
   bSlip=0;
   for (i=iMgLine;i<iMgLine+numMgLines;i++)
      for (j=iMgCol;j<iMgCol+numMgCols;j++)
      {
         iCur=i*pFormBox->numCols+j;
         if (pCellTable[iCur].bSlip)
         {
            bSlip=pCellTable[iCur].bSlip;
            pCellTable[iCur].bSlip=0;
         }
      }

  // Merge Cells
   iFirst=iMgLine*pFormBox->numCols+iMgCol;
   for (i=iMgLine;i<iMgLine+numMgLines;i++)
   {
      for (j=iMgCol;j<iMgCol+numMgCols;j++)
      {
         iCur=i*pFormBox->numCols+j;
         if (iCur==iFirst)         // it's the first cell
         {
            pCellTable[iCur].iFirst=FIRSTCELL;
            pCellTable[iCur].numLines=numMgLines;
            pCellTable[iCur].numCols=numMgCols;
            pCellTable[iCur].bSlip=bSlip;
         }
         else         // it's a merged cell
         {
            pCellTable[iCur].iFirst=iFirst;
            pCellTable[iCur].bSlip=0;
         }
      }
   }

   ReFormatTableText(hFormBox,TRUE);

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

int FBDdisMergeBlock(HFormBoxs hFormBox,int iCell)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int iLineBegin,iColBegin,iLineEnd,iColEnd,i,j,k;
   int retval;           //iCell,

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   retval=0;
   if( pCellTable[iCell].iFirst==FIRSTCELL
   && (pCellTable[iCell].numCols>1 || pCellTable[iCell].numLines>1))
   {      // DisMerge
        retval=1;
        // Compute Block's start line column and end line column.
        iLineBegin=iCell/pFormBox->numCols;
        iColBegin=iCell%pFormBox->numCols;
        iLineEnd=iLineBegin+pCellTable[iCell].numLines-1;
        iColEnd=iColBegin+pCellTable[iCell].numCols-1;

        UndoInsertTableDismerge(iLineBegin,iColBegin,pCellTable[iCell].numLines,pCellTable[iCell].numCols);

        for (i=iLineBegin;i<=iLineEnd;i++)
            for (j=iColBegin;j<=iColEnd;j++)
            {
                k=i*pFormBox->numCols+j;
                pCellTable[k].iFirst=FIRSTCELL;
                pCellTable[k].numCols=1;
                pCellTable[k].numLines=1;
                InitACellText(k,hFormBox);
            }
        ReFormatTableText(hFormBox,TRUE);
   }

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return retval;
}

int FBDisMergeCells(HFormBoxs hFormBox,int iMgLine,int iMgCol,int numMgLines,int numMgCols,HANDLE hUndoCellTable)
{
   int iUndoCur,iCur;
   int i,j;
   PFormBoxs pFormBox;
   CELL *pCellTable,*pUndoCellTable;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   pUndoCellTable=HandleLock(hUndoCellTable);
   if (pUndoCellTable==NULL)
   {
      HandleUnlock(hUndoCellTable);
      return(OUTOFMEMORY);
   }
   for (i=0;i<numMgLines;i++)
   {
      for (j=0;j<numMgCols;j++)
      {
           iUndoCur=i*numMgCols+j;
           iCur=(i+iMgLine)*pFormBox->numCols+(j+iMgCol);
           pCellTable[iCur]=pUndoCellTable[iUndoCur];
      }
   }
   ReFormatTableText(hFormBox,TRUE);

   HandleUnlock(hUndoCellTable);
   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

#ifdef UNUSED
/*
   Form Box disemble Cell
   return value:
      0:success
     -1:fail (when iCell is not first cell.)
*/
int FBDisembleCell(HFormBoxs hFormBox,int iCell)
{
   int iCur;
   int iDisLine,iDisCol,numDisLines,numDisCols;
   int i,j;
   PFormBoxs pFormBox;
   CELL * pCellTable;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// judge if can be disembled
   if (pCellTable[iCell].iFirst!=FIRSTCELL)
   // The cell is not a first cell, disemble failed.
   {
         HandleUnlock(pFormBox->hCellTable);
         HandleUnlock(ItemGetHandle(hFormBox));
         return -1;
   }

// Disemble the specified cell
   iDisLine=iCell/pFormBox->numCols;
   iDisCol=iCell%pFormBox->numCols;
   numDisLines=pCellTable[iCell].numLines;
   numDisCols=pCellTable[iCell].numCols;
   for (i=iDisLine;i<iDisLine+numDisLines;i++)
   {
      for (j=iDisCol;j<iDisCol+numDisCols;j++)
      {
         iCur=i*pFormBox->numCols+j;
         pCellTable[iCur].iFirst=FIRSTCELL;
         pCellTable[iCur].numLines=pCellTable[iCur].numCols=1;
      }
   }

   /*----------------------------------
   // Disemble cell's text, not implememted yet
     ...
     ...
    ----------------------------------*/

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}
#endif

/*
    Form Box (x,y) is in form
       (x,y) is user coordinate that related to Form Box.
      return value:
          0:(x,y) is not in the form.
          1:(x,y) is in the form.
*/
static int FBXYInForm(PFormBoxs pFormBox,int x,int y)
{
   if (x<pFormBox->vertline[0]||x>pFormBox->vertline[pFormBox->numCols])
      return 0;   // x is not in the form.

   if (y<pFormBox->hortline[0]||y>pFormBox->hortline[pFormBox->numLines])
      return 0;   // y is not in the form.

   return 1;
}

/*
      return value:
         -1:(x,y) is not in the form.
          0:(x,y) is in the form.
*/
static int FBXYToLineCol(HFormBoxs hFormBox,int x,int y,int *pLine,int *pCol)
{
   PFormBoxs pFormBox;
   int i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

// judge if (x,y) is in the form
   if (!FBXYInForm(pFormBox,x,y))
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

// Compute *pCol
   for (i=1;i<=pFormBox->numCols;i++)
      if (x<=pFormBox->vertline[i])
      {
         *pCol=i-1;
         break;
      }

// Compute *pLine
   for (i=1;i<=pFormBox->numLines;i++)
      if (y<=pFormBox->hortline[i])
      {
         *pLine=i-1;
         break;
      }

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

/*-------------------------------------------------------
   return value:
         is Col Number
------------------------------------------------------- */
int TableColOfX(HFormBoxs hFormBox,int x)
{
   PFormBoxs pFormBox;
   int i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

// Compute *pCol
   for (i=0;i<=pFormBox->numCols;i++)
      if (abs(x-pFormBox->vertline[i])<5)
         break;

// return successfully
   HandleUnlock(ItemGetHandle(hFormBox));
   return i;
}

int TableLineOfY(HFormBoxs hFormBox,int y)
{
   PFormBoxs pFormBox;
   int i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

// Compute *pLine
   for (i=0;i<=pFormBox->numLines;i++)
      if (abs(y-pFormBox->hortline[i])<5)
         break;

// return successfully
   HandleUnlock(ItemGetHandle(hFormBox));
   return i;
}

/*
    return value:
         -1:(x,y) is not in the form.
          0:(x,y) is in the form.
*/
int FBXYToNewXY(HFormBoxs hFormBox,int x,int y,int *xNew,int *yNew)
{
   PFormBoxs pFormBox;
   int i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

// judge if (x,y) is in the form
   if (!FBXYInForm(pFormBox,x,y))
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

// Compute *pCol
   for (i=1;i<=pFormBox->numCols;i++)
      if (x<=pFormBox->vertline[i])
      {
         //*xNew=pFormBox->vertline[i-1]+DEFAULTBOXTEXTDISTANT;
         *xNew=pFormBox->vertline[i-1]+pFormBox->TextDistantLeft;    //By zjh 12.5
         break;
      }

// Compute *pLine
   for (i=1;i<=pFormBox->numLines;i++)
      if (y<=pFormBox->hortline[i])
      {
         //*yNew=pFormBox->hortline[i-1]+DEFAULTBOXTEXTDISTANT;
         *yNew=pFormBox->hortline[i-1]+pFormBox->TextDistantTop;    //By zjh 12.5
         break;
      }

// return successfully
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

/*
   return value:
      the first cell's index of cell (iLine,iCol).
   note:
      when iLine,iCol is invalid, return value will have no meaning.
*/
int FBCellofLineCol(HFormBoxs hFormBox,int iLine,int iCol)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int iCell,iFirst;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// Get its first cell's index
   iCell=iLine*pFormBox->numCols+iCol;
   if (pCellTable[iCell].iFirst==FIRSTCELL)
      iFirst=iCell;
   else
      iFirst=pCellTable[iCell].iFirst;

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return iFirst;
}

int IsFBCellofLineCol(HFormBoxs hFormBox,int iLine,int iCol)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int iCell,iFirst;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// Get its first cell's index
   iCell=iLine*pFormBox->numCols+iCol;
   if (pCellTable[iCell].iFirst==FIRSTCELL)
      iFirst=1;
   else
      iFirst=0;

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return iFirst;
}

/*---------------------------
   coordinate (x,y) is relative to form box
   return value:
     -1:(x,y) is not in the form
     >=0:cell's index of (x,y).
-----------------------------*/
int FBCellofXY(HFormBoxs hFormBox,int x,int y)
{
   int iCol,iLine;

   if (FBXYToLineCol(hFormBox,x,y,&iLine,&iCol)==-1)
      return -1;   // (x,y) is not in the form.

   return FBCellofLineCol(hFormBox,iLine,iCol);
}

/*----------------------------
  iCell must be first cell.
  *px0,*py0,*px1,*py1 is relative to form box.
-----------------------------*/
int FBGetCellRect(HFormBoxs hFormBox,int iCell,
               int *px0,int *py0,int *px1,int *py1)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int iLine,iCol,slip;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

    slip=pCellTable[iCell].bSlip;                    //By zjh

   if (pCellTable[iCell].iFirst!=FIRSTCELL)
   {      // iCell is invalid
         HandleUnlock(pFormBox->hCellTable);
         HandleUnlock(ItemGetHandle(hFormBox));
         return -1;
   }

 // compute *x0,*y0,*x1,*y1;

   slip=pCellTable[iCell].bSlip;                    //By zjh
   iLine=iCell/pFormBox->numCols;
   iCol=iCell%pFormBox->numCols;
   *px0=pFormBox->vertline[iCol];
   *py0=pFormBox->hortline[iLine];
   *px1=pFormBox->vertline[iCol+pCellTable[iCell].numCols];
   *py1=pFormBox->hortline[iLine+pCellTable[iCell].numLines];

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

static int FBGetCellDistant(HFormBoxs hFormBox,
               int *px0,int *py0,int *px1,int *py1)
{
   PFormBoxs pFormBox;
   //int iLine,iCol,slip;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   *px0=pFormBox->TextDistantLeft;
   *py0=pFormBox->TextDistantTop;
   *px1=pFormBox->TextDistantRight;
   *py1=pFormBox->TextDistantBottom;

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

/*-------------------------------
      iCell must be first cell.
      *px0,*py0,*px1,*py1 is relative to form box.
-----------------------------*/
int FBPGetCellRect(PFormBoxs pFormBox,int iCell,int *px0,int *py0,int *px1,int *py1)
{
   CELL * pCellTable;
   int iLine,iCol,slip;

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
      return(OUTOFMEMORY);

   if (pCellTable[iCell].iFirst!=FIRSTCELL)
   {     // iCell is invalid
      HandleUnlock(pFormBox->hCellTable);
      return(-1);
   }

   slip=pCellTable[iCell].bSlip;                    //By zjh
// compute *x0,*y0,*x1,*y1;
   iLine=iCell/pFormBox->numCols;
   iCol=iCell%pFormBox->numCols;

   /*
   *px0=pFormBox->vertline[iCol]+DEFAULTBOXTEXTDISTANT;
   *py0=pFormBox->hortline[iLine]+DEFAULTBOXTEXTDISTANT;
   *px1=pFormBox->vertline[iCol+pCellTable[iCell].numCols]-DEFAULTBOXTEXTDISTANT;
   *py1=pFormBox->hortline[iLine+pCellTable[iCell].numLines]-DEFAULTBOXTEXTDISTANT;
   */
   *px0=pFormBox->vertline[iCol]+pFormBox->TextDistantLeft;
   *py0=pFormBox->hortline[iLine]+pFormBox->TextDistantTop;
   *px1=pFormBox->vertline[iCol+pCellTable[iCell].numCols]-pFormBox->TextDistantRight;
   *py1=pFormBox->hortline[iLine+pCellTable[iCell].numLines]-pFormBox->TextDistantBottom;
   HandleUnlock(pFormBox->hCellTable);
   return 0;
}

#ifdef NOTUSED
/*  Draw Cell(iCell)'s border. */
int FBDrawCellBorder(HFormBoxs hFormBox,int iCell)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int x0,x1,y0,y1;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// judge if need to draw
   if (pCellTable[iCell].iFirst!=FIRSTCELL)
   {  // do not need to draw a merged cell
      HandleUnlock(pFormBox->hCellTable);
      HandleUnlock(ItemGetHandle(hFormBox));
      return 0;
   }

// Draw a Cell
   FBGetCellRect(hFormBox,iCell,&x0,&y0,&x1,&y1);
   Boxrectangle((PTextBoxs)pFormBox,x0,y0,x1,y1);

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}
#endif     // NOTUSED


/*
   Form Box draw border
*/
int FBDrawBorder(HFormBoxs hFormBox)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int i,j,iCell;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// Draw border
//   Boxrectangle((PTextBoxs)pFormBox,pFormBox->vertline[0],pFormBox->hortline[0],
  //      pFormBox->vertline[pFormBox->numCols],pFormBox->hortline[pFormBox->numLines]);

// Draw every horizontal lines
   i=0;         //-- draw top line
   Boxline((PTextBoxs)pFormBox, pFormBox->hortlineType[i],
        pFormBox->vertline[0],   pFormBox->hortline[i],
        pFormBox->vertline[pFormBox->numCols],pFormBox->hortline[i]);
   i=pFormBox->numLines;    //-- draw bottom line
   Boxline((PTextBoxs)pFormBox, pFormBox->hortlineType[i],
        pFormBox->vertline[0],   pFormBox->hortline[i],
        pFormBox->vertline[pFormBox->numCols],pFormBox->hortline[i]);

   for (i=1;i<pFormBox->numLines;i++)
      for(j=0;j<pFormBox->numCols;j++)
      {
         iCell=i*pFormBox->numCols+j;
         if (pCellTable[iCell].iFirst==FIRSTCELL)
         {   // if iCell is a FirstCell,we draw its top border
            Boxline((PTextBoxs)pFormBox, pFormBox->hortlineType[i],
                 pFormBox->vertline[j],   pFormBox->hortline[i],
                 pFormBox->vertline[j+pCellTable[iCell].numCols],pFormBox->hortline[i]);
         }
      }

// Draw every vertical lines
   j=0;         //-- draw left line
   Boxline((PTextBoxs)pFormBox, pFormBox->vertlineType[j],
        pFormBox->vertline[j],pFormBox->hortline[0],
        pFormBox->vertline[j],pFormBox->hortline[pFormBox->numLines]);
   j=pFormBox->numCols;     //-- draw right line
   Boxline((PTextBoxs)pFormBox, pFormBox->vertlineType[j],
        pFormBox->vertline[j],pFormBox->hortline[0],
        pFormBox->vertline[j],pFormBox->hortline[pFormBox->numLines]);

   for (j=1;j<pFormBox->numCols;j++)
      for(i=0;i<pFormBox->numLines;i++)
      {
         iCell=i*pFormBox->numCols+j;
         if (pCellTable[iCell].iFirst==FIRSTCELL)
         {   // if iCell is a FirstCell,we draw its left border
            Boxline((PTextBoxs)pFormBox, pFormBox->vertlineType[j],
                 pFormBox->vertline[j],pFormBox->hortline[i],
                 pFormBox->vertline[j],pFormBox->hortline[i+pCellTable[iCell].numLines]);
         }
      }

   //Draw every slip lines
   for (j=0;j<pFormBox->numCols;j++)
      for(i=0;i<pFormBox->numLines;i++)
      {
         iCell=i*pFormBox->numCols+j;
         // if iCell has slip line,we draw it
         if (pCellTable[iCell].bSlip==1)
         {
            Boxline((PTextBoxs)pFormBox,0,
                pFormBox->vertline[j], pFormBox->hortline[i],
                pFormBox->vertline[j+pCellTable[iCell].numCols], pFormBox->hortline[i+pCellTable[iCell].numLines]);
         }
         else if (pCellTable[iCell].bSlip==2)
         {
            int x0,y0;
            x0=(pFormBox->vertline[j]+pFormBox->vertline[j+pCellTable[iCell].numCols])/2;
            y0=(pFormBox->hortline[i]+pFormBox->hortline[i+pCellTable[iCell].numLines])/2;
            Boxline((PTextBoxs)pFormBox,0,
                x0, pFormBox->hortline[i],
                pFormBox->vertline[j+pCellTable[iCell].numCols], pFormBox->hortline[i+pCellTable[iCell].numLines]);
            Boxline((PTextBoxs)pFormBox,0,
                pFormBox->vertline[j], y0,
                pFormBox->vertline[j+pCellTable[iCell].numCols], pFormBox->hortline[i+pCellTable[iCell].numLines]);
         }
      }

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

/*---------------------------------
     1 direction can be one of (LEFTCELL,RIGHTCELL,UPCELL,DOWNCELL).
     2 iCell need not be first cell.
     3 the return value is always a first cell's number.
----------------------------------------------*/
int FBFindNextCell(HFormBoxs hFormBox,int iCell,enum CELLDIRECTION direction)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int iFirst,iNewFirst;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// Find the first cell of iCell.
   iNewFirst=iFirst=FirstCell(pCellTable,iCell);

// Find the next cell.
   while(iCell>=0&&iCell<pFormBox->numLines*pFormBox->numCols)
   {
      iNewFirst=FirstCell(pCellTable,iCell);
      if (iNewFirst!=iFirst) break;
      switch(direction)
      {
         case LEFTCELL:
            iCell-=1;
            break;
         case RIGHTCELL:
            iCell+=1;
            break;
         case UPCELL:
            iCell-=pFormBox->numCols;
            break;
         case DOWNCELL:
            iCell+=pFormBox->numCols;
            break;
         default:
            assert("Unknown direction in Function FBFindNextCell\n"==0);
      }
   }

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return iNewFirst;
}

#ifdef NOT_USED
/*   Form Box Change column width. */
static int FBChangeColWidth(HFormBoxs hFormBox,int iCol,int wNew)
{
   PFormBoxs pFormBox;
   int wOld,i;

   if (wNew<0)
      return -1;
   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   wOld=pFormBox->vertline[iCol+1]-pFormBox->vertline[iCol];
   for (i=iCol+1;i<=pFormBox->numCols;i++)
      pFormBox->vertline[i]+=wNew-wOld;

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

/* Form Box Change line height. */
static int FBChangeLineHeight(HFormBoxs hFormBox,int iLine,int hNew)
{
   PFormBoxs pFormBox;
   int hOld,i;

   if (hNew<0)
      return -1;
   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   hOld=pFormBox->hortline[iLine+1]-pFormBox->vertline[iLine];
   for (i=iLine+1;i<=pFormBox->numLines;i++)
      pFormBox->hortline[i]+=hNew-hOld;

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}
#endif

int FBPlusHoriLine(HFormBoxs hFormBox,int iHoriLine,int yPlus)
{
   PFormBoxs pFormBox;
   int yOld,i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   assert(iHoriLine>=0&&iHoriLine<=pFormBox->numLines);
   if (iHoriLine!=0&&yPlus<=pFormBox->hortline[iHoriLine-1])
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

   yOld=pFormBox->hortline[iHoriLine];
   if (yPlus<0)
       yPlus = max(yPlus,-yOld);
   for (i=iHoriLine;i<=pFormBox->numLines;i++)
          pFormBox->hortline[i]+=yPlus;

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

int FBPlusVertLine(HFormBoxs hFormBox,int iVertLine,int xPlus)
{
   PFormBoxs pFormBox;
   int xOld,i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   assert(iVertLine>=0&&iVertLine<=pFormBox->numCols);
   if (iVertLine!=0&&xPlus<=pFormBox->vertline[iVertLine-1])
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

   xOld=pFormBox->vertline[iVertLine];
   if (xPlus<0)
       xPlus = max(xPlus,-xOld);
   for (i=iVertLine;i<=pFormBox->numCols;i++)
          pFormBox->vertline[i]+=xPlus;

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

static int SearchMaxFontSize(HFormBoxs hFormBox,PFormBoxs pFormBox,int ColNum)
{
    int LastPosition,pos,length;
    int CharSize,w;
    int numCol,i,k;

    if (ColNum<0)
        return (0);

    numCol=ColNum%pFormBox->numCols;
    for (w=k=0;k<pFormBox->numLines;k++)
    {
        pos=TableCellGetTextHead(hFormBox,numCol);
        length=TableCellGetTextLength(hFormBox,numCol);
        for (i=0;i<length;i++)
        {       //-- search max char size to w
            CharSize=TextSearchAttribute(hFormBox,pos+i,CHARSIZE,&LastPosition);
            if (w<CharSize)
                w=CharSize;
        }
        numCol+=pFormBox->numCols;      // point to next line
    }
    return (w);
}

/* Form Box change vertical lines postion. */
static int FBChangeVertLine(HFormBoxs hFormBox,int iVertLine,int xNew)
{
   PFormBoxs pFormBox;
   int xOld,i,w;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   assert(iVertLine>=0&&iVertLine<=pFormBox->numCols);
   if (iVertLine!=0&&xNew<=pFormBox->vertline[iVertLine-1])
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

   xOld=pFormBox->vertline[iVertLine];
 #ifdef OLD_VERSION
   for (i=iVertLine;i<=pFormBox->numCols;i++)
      pFormBox->vertline[i]+=xNew-xOld;

   if (xNew<xOld)
   {
       w=SearchMaxFontSize(hFormBox,pFormBox,iVertLine-1);
       for (i=1;i<=pFormBox->numCols;i++)
       {        // to prevent too small
          /*
          if (abs(pFormBox->vertline[i]-pFormBox->vertline[i-1])<(4*DEFAULTBOXTEXTDISTANT+w+10))
                pFormBox->vertline[i]=pFormBox->vertline[i-1]+4*DEFAULTBOXTEXTDISTANT+w+10;
                */   //By zjh 12.5/96
          if (abs(pFormBox->vertline[i]-pFormBox->vertline[i-1])<
              (pFormBox->TextDistantLeft+
              pFormBox->TextDistantRight+
              w+10))
          pFormBox->vertline[i]=pFormBox->vertline[i-1]+
                                pFormBox->TextDistantLeft+
                                pFormBox->TextDistantRight+
                                +w+10;
       }
   }
 #else          // ByHance, 97,6.1
   pFormBox->vertline[iVertLine]=xNew;
   for (i=iVertLine;i<=pFormBox->numCols;i++)
   {        // to prevent too small
      w=SearchMaxFontSize(hFormBox,pFormBox,i-1);
      if (abs(pFormBox->vertline[i]-pFormBox->vertline[i-1])<
          (pFormBox->TextDistantLeft+pFormBox->TextDistantRight+w+10))
      pFormBox->vertline[i]=pFormBox->vertline[i-1]+pFormBox->TextDistantLeft+
                            pFormBox->TextDistantRight+w+10;
   }
 #endif

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

/* Form Box change horizontal line's postion. */
static int FBChangeHortLine(HFormBoxs hFormBox,int iHortLine,int yNew)
{
   PFormBoxs pFormBox;
   int yOld,i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   assert(iHortLine>=0&&iHortLine<=pFormBox->numLines);
   if (iHortLine!=0&&yNew<=pFormBox->hortline[iHortLine-1])
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

   yOld=pFormBox->hortline[iHortLine];
   for (i=iHortLine;i<=pFormBox->numLines;i++)
      pFormBox->hortline[i]+=yNew-yOld;

   for (i=1;i<=pFormBox->numLines;i++)
   {            // to prevent too near
      /*
      if (abs(pFormBox->hortline[i]-pFormBox->hortline[i-1])<4*DEFAULTBOXTEXTDISTANT+10)
         pFormBox->hortline[i]=pFormBox->hortline[i-1]+4*DEFAULTBOXTEXTDISTANT+10;
         */

      if (abs(pFormBox->hortline[i]-pFormBox->hortline[i-1])<
               pFormBox->TextDistantTop+
               pFormBox->TextDistantBottom+
               +10)
      pFormBox->hortline[i]=pFormBox->hortline[i-1]+
               pFormBox->TextDistantTop+
               pFormBox->TextDistantBottom+
               +10;
   }

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

int FBChangeCellLeftLine(HFormBoxs hFormBox,int iCell,int xNew)
{
   PFormBoxs pFormBox;
   int iCol,retval;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   if (iCell<0)
      { iCell=0; GlobalTableCell=0; }

   iCol=iCell%pFormBox->numCols;
   retval=FBChangeVertLine(hFormBox,iCol,xNew);

   // Change Text in Table, Write By DG in 1996,2
   //ReFormatTableText(hFormBox,TRUE);

   HandleUnlock(ItemGetHandle(hFormBox));
   return retval;
}

int FBChangeCellTopLine(HFormBoxs hFormBox,int iCell,int yNew)
{
   PFormBoxs pFormBox;
   int iLine,retval;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   if (iCell<0)
      { iCell=0; GlobalTableCell=0; }

   iLine=iCell/pFormBox->numCols;
   retval=FBChangeHortLine(hFormBox,iLine,yNew);

   // Change Text in Table, Write By DG in 1996,2
   //ReFormatTableText(hFormBox,TRUE);

   HandleUnlock(ItemGetHandle(hFormBox));
   return retval;
}

int FBChangeCellRightLine(HFormBoxs hFormBox,int iCell,int xNew)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int iCol,retval;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   if (iCell<0)
      { iCell=0; GlobalTableCell=0; }
   //assert(pCellTable[iCell].iFirst==FIRSTCELL);

   iCol=iCell%pFormBox->numCols;
   iCol+=pCellTable[iCell].numCols;
   retval=FBChangeVertLine(hFormBox,iCol,xNew);

   // Change Text in Table, Write By DG in 1996,2
   //ReFormatTableText(hFormBox,TRUE);

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return retval;
}

int FBChangeCellBottomLine(HFormBoxs hFormBox,int iCell,int yNew)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int iLine,retval;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   if (iCell<0)
      { iCell=0; GlobalTableCell=0; }
   //assert(pCellTable[iCell].iFirst==FIRSTCELL);

   iLine=iCell/pFormBox->numCols;
   iLine+=pCellTable[iCell].numLines;
   retval=FBChangeHortLine(hFormBox,iLine,yNew);

   // Change Text in Table, Write By DG in 1996,2
   //ReFormatTableText(hFormBox,TRUE);

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return retval;
}

/*------------------------------------------------------
    Find the iPosInBlock'th Cell in the block(iCellBegin,iCellEnd).
    iCellBegin is the leftup cell of the block.
    iCellEnd is the rightbottom cell of the block.
    return the cell's number when success.
    return -1 when iPosInBlock'th cell is not in the block.
------------------------------------------------------*/
int FBFindBlockCell(HFormBoxs hFormBox,int iCellBegin,
                   int iCellEnd,int iPosInBlock)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int iLineBegin,iColBegin,iLineEnd,iColEnd,iCell,iPos;
   int i,j;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   //assert(pCellTable[iCellBegin].iFirst==FIRSTCELL);
   //assert(pCellTable[iCellEnd].iFirst==FIRSTCELL);

// Compute Block's start line column and end line column.
   iLineBegin=iCellBegin/pFormBox->numCols;
   iColBegin=iCellBegin%pFormBox->numCols;
   iLineEnd=iCellEnd/pFormBox->numCols+pCellTable[iCellEnd].numLines-1;
   iColEnd=iCellEnd%pFormBox->numCols+pCellTable[iCellEnd].numCols-1;
   if (iLineBegin>iLineEnd) exchange(&iLineBegin,&iLineEnd);
   if (iColBegin>iColEnd) exchange(&iColBegin,&iColEnd);

// search the required cell in block.
   iPos=0;
   for (i=iLineBegin;i<=iLineEnd;i++)
   {
      for (j=iColBegin;j<=iColEnd;j++)
      {
         iCell=i*pFormBox->numCols+j;
         if (pCellTable[iCell].iFirst==FIRSTCELL)
         {        // iCell is a First cell
            if (i+pCellTable[iCell].numLines-1<=iLineEnd&&
                j+pCellTable[iCell].numCols-1<=iColEnd)
            {    // iCell is within the block
               if (iPos==iPosInBlock)
               {     // Find the required cell,return successfully.
                  HandleUnlock(pFormBox->hCellTable);
                  HandleUnlock(ItemGetHandle(hFormBox));
                  return iCell;
               }
               else
                  iPos++;
            }
         }
      } /*---- j ----*/
   } /*--- i ----*/

  // have not find the required cell, return failed.
   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return -1;
}

/* Merge cells into a block. do the same thing as FBMergeCells. */
int FBMergeBlock(HFormBoxs hFormBox,int iCellBegin,int iCellEnd)
{
   PFormBoxs pFormBox;
   CELL * pCellTable;
   int iLineBegin,iColBegin,iLineEnd,iColEnd,retval,i,j;
   int iCell,FirstCell;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   //assert(pCellTable[iCellBegin].iFirst==FIRSTCELL);
   //assert(pCellTable[iCellEnd].iFirst==FIRSTCELL);

// Compute Block's start line column and end line column.
   iLineBegin=iCellBegin/pFormBox->numCols;
   iColBegin=iCellBegin%pFormBox->numCols;
   iLineEnd=iCellEnd/pFormBox->numCols+pCellTable[iCellEnd].numLines-1;
   iColEnd=iCellEnd%pFormBox->numCols+pCellTable[iCellEnd].numCols-1;
   if (iLineBegin>iLineEnd) exchange(&iLineBegin,&iLineEnd);
   if (iColBegin>iColEnd) exchange(&iColBegin,&iColEnd);
   //GlobalTableCell=min(iCellBegin,iCellEnd);  // ByHance, 96,5.6
   GlobalTableCell=
   FirstCell=FBCellofLineCol(hFormBox,iLineBegin,iColBegin); // ByHance, 96,4.7

   //----- if iCell has text, warning ----
   for (i=iLineBegin;i<=iLineEnd;i++)
       for (j=iColBegin;j<=iColEnd;j++)
       {
          Wchar *FormTextBlock,Tmp;
          int  pos,n,k;

          iCell=FBCellofLineCol(hFormBox,i,j);
          if(iCell==FirstCell)
              continue;

          pos=TableCellGetTextHead(hFormBox,iCell);
          n=TableCellGetTextLength(hFormBox,iCell);
          FormTextBlock=HandleLock(TableBoxGetTextHandle(pFormBox));
          for(k=0;k<n;k++)
          {
              Tmp=FormTextBlock[k+pos] & ATTRIBUTEPRECODE;
              if ((Tmp==HIGHENGLISHCHAR)||(Tmp>=HIGHCHINESECHARS))
                  break;
          }
          HandleUnlock(TableBoxGetTextHandle(pFormBox));
          if(k<n)
          {
              retval=MessageBox(GetTitleString(WARNINGINFORM),
                        "  欲合并的表元中有文字,合并后\n"
                        "文字将被删除。继续进行合并吗?",
                        2,1);
              if (retval==0)
                 goto lbl_MERGE;

                 // if cancel this OP, return
              HandleUnlock(pFormBox->hCellTable);
              HandleUnlock(ItemGetHandle(hFormBox));
              return 0;
          }
       }

// Merge
 lbl_MERGE:
   retval=FBMergeCells(hFormBox,iLineBegin,iColBegin,iLineEnd-iLineBegin+1,iColEnd-iColBegin+1);

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return retval;
}

int FBCellToLineCol(HFormBoxs hFormBox,int iCell,int *piCellLine,int *piCellCol)
{
   PFormBoxs pFormBox;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   *piCellLine=iCell/pFormBox->numCols;
   *piCellCol=iCell%pFormBox->numCols;

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

int AdjustTableBoxRightVertline(PFormBoxs pFormBox)
{
   int c,w=0;
   c=pFormBox->numCols;
   if (pFormBox->BoxWidth<pFormBox->vertline[c])
   {
       if ((pFormBox->vertline[c]-pFormBox->BoxWidth)<pFormBox->vertline[0])
       // Move vertlines
          w=pFormBox->vertline[c]-pFormBox->BoxWidth;
       else
          pFormBox->BoxWidth=pFormBox->vertline[c];
   }
   return w;
}

int AdjustTableBoxBottomHoriline(PFormBoxs pFormBox)
{
   int c,w=0;
   c=pFormBox->numLines;
   if (pFormBox->BoxHeight<pFormBox->hortline[c])
   {
       if ((pFormBox->hortline[c]-pFormBox->BoxHeight)<pFormBox->hortline[0])
       // Move hortlines
          w=pFormBox->hortline[c]-pFormBox->BoxHeight;
       else
          pFormBox->BoxHeight=pFormBox->hortline[c];
   }
   return w;
}

// Writed By Dg in 1996,2
int TableChangeHortLine(HFormBoxs hFormBox,int iHortLine,int yNew)
{
   PFormBoxs pFormBox;
   int y,i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   assert(iHortLine>=0&&iHortLine<=pFormBox->numLines);
   if (iHortLine!=0&&yNew<=pFormBox->hortline[iHortLine-1])
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return -1;
   }

   y=yNew - pFormBox->hortline[iHortLine];
   for (i=iHortLine;i<=pFormBox->numLines;i++)
      pFormBox->hortline[i]+=y;

   y=pFormBox->hortline[pFormBox->numLines];
   TextBoxSetBoxHeight(pFormBox,y);
   //BoxChangeAll(GlobalCurrentPage);     // ByHance, 96,4.6
   BoxChange(hFormBox,GlobalCurrentPage);

   // Change Text
   ReFormatTableText(hFormBox,FALSE);

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

int TableChangeVertLine(HFormBoxs hFormBox,int iVertLine,int xNew)
{
   PFormBoxs pFormBox;
   int x,i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);

   assert(iVertLine>=0&&iVertLine<=pFormBox->numCols);
   if (iVertLine!=0&&xNew<=pFormBox->vertline[iVertLine-1])
   {
       HandleUnlock(ItemGetHandle(hFormBox));
       return -1;
   }

   x=xNew - pFormBox->vertline[iVertLine];
   for (i=iVertLine;i<=pFormBox->numCols;i++)
      pFormBox->vertline[i]+=x;

   x=pFormBox->vertline[pFormBox->numCols];
   TextBoxSetBoxWidth(pFormBox,x);
   //BoxChangeAll(GlobalCurrentPage);     // ByHance, 96,4.6
   BoxChange(hFormBox,GlobalCurrentPage);

   // Change Text
   ReFormatTableText(hFormBox,FALSE);

   HandleUnlock(ItemGetHandle(hFormBox));
   return 0;
}

// Change Text in Table, Write By DG in 1996,2
void ReFormatTableText(HFormBoxs hFormBox,BOOL bEraseBk)
{
   PFormBoxs pFormBox;
   TEXTTYPE TextLength,Position=0;
   int StartChangeLine,ChangeLines;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return;

   TextLength=TableBoxGetTextLength(pFormBox);
   FormatInsertText(hFormBox,Position,TextLength,&StartChangeLine,&ChangeLines,bEraseBk);

   HandleUnlock(ItemGetHandle(hFormBox));
}

/* -------------------------------------------------------------
   Insert TYpe for a line in the form.
   New line's place is specified by iNewLine;
-------------------------------------------------------------- */
int FBChangeHLineType(HFormBoxs hFormBox,int iNewLine,int Type)
{
   PFormBoxs pFormBox;
   int retval;
   //int SaveUndoNumber;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return -1;

   if (iNewLine>pFormBox->numLines)
      iNewLine=pFormBox->numLines;
   else if (iNewLine<0) iNewLine=0;
   if (Type>3) Type=3;

   if(Type!=pFormBox->hortlineType[iNewLine])
   {
      UndoInsertTableChangeHline(iNewLine,pFormBox->hortlineType[iNewLine]);
      pFormBox->hortlineType[iNewLine]=Type;
      retval=1;
   } else retval=-1;

   HandleUnlock(ItemGetHandle(hFormBox));
   return retval;
}

int FBChangeVLineType(HFormBoxs hFormBox,int iNewCol,int Type)
{
   PFormBoxs pFormBox;
   int retval;
   //int SaveUndoNumber;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return -1;

   if (iNewCol>pFormBox->numCols)
      iNewCol=pFormBox->numCols;
   else if (iNewCol<0) iNewCol=0;
   if (Type>3) Type=3;

   if(Type!=pFormBox->vertlineType[iNewCol])
   {
      UndoInsertTableChangeVline(iNewCol,pFormBox->vertlineType[iNewCol]);
      pFormBox->vertlineType[iNewCol]=Type;
      retval=1;
   } else retval=-1;

   HandleUnlock(ItemGetHandle(hFormBox));
   return retval;
}

#ifdef UNUSED
/*-------------------------------------------------------------
   FBInsAHori(hFormBox,iNewLine,TRUE): Form Box Insert a HORI line.
      Insert a line in the form Cell.
      New line's place is specified by iNewLine, iNewLine>=1
-------------------------------------------------------------- */
int FBInsAHori(HFormBoxs hFormBox,int iNewLine,BOOL bText)
{
   int iOld,iNew,i,j,k,hNewLine,i0,iOldLine;
   int * ConvertTable;
   PFormBoxs pFormBox;
   CELL * pCellTable;
   CELL * pOldCellTable;
   HANDLE hOldCellTable;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return -1;

// set iNewLine in appropriate range.
   if (iNewLine<1 || iNewLine>pFormBox->numLines)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(0);
   }

 // compute new line's height.
   // if New line is not the first line, its height will be assigned as
   // the height of its up line.
   hNewLine=pFormBox->hortline[iNewLine]-pFormBox->hortline[iNewLine-1];

   UndoInsertTableInsertLine(iNewLine);

 // readjust hortline[] field and numLines field.
   for (i=pFormBox->numLines+1;i>iNewLine;i--)
   {
   // All the Lines down to the New line will just be moved. Their height
   // will not be changed.
      pFormBox->hortline[i]=pFormBox->hortline[i-1];
   }
   pFormBox->numLines++;
   pFormBox->hortline[iNewLine]-=hNewLine/2;

// Get the pointer of old cell table.
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   pOldCellTable=pCellTable;
   hOldCellTable=pFormBox->hCellTable;

// allocate space for new cell table and get its pointer.
   pFormBox->hCellTable=HandleAlloc(pFormBox->numCols*pFormBox->numLines*sizeof(CELL),0);
   if (pFormBox->hCellTable==NULL)
      return(OUTOFMEMORY);
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// move the cells' data of old cell table into new cell table.
   iOld=0;
   ConvertTable=malloc(pFormBox->numCols*(pFormBox->numLines-1)*sizeof(int));
   if (ConvertTable==NULL)
      return(OUTOFMEMORY);
   for (i=0;i<pFormBox->numLines;i++)
   {
      for (j=0;j<pFormBox->numCols;j++)
      {
          iNew=i*pFormBox->numCols+j;
          if (i!=iNewLine)
          {     // It's not a new cell
             ConvertTable[iOld]=iNew;

             pCellTable[iNew]=pOldCellTable[iOld];
             pCellTable[iNew].iSelf=iNew;
             if (pCellTable[iNew].iFirst!=FIRSTCELL)
             {     // it's a merged cell
                pCellTable[iNew].iFirst=ConvertTable[pCellTable[iNew].iFirst];
             }
             else
             {     // it's the first cell of merged cells
                iOldLine=iOld/pFormBox->numCols;
                if(iOldLine<iNewLine && iNewLine<iOldLine+pCellTable[iNew].numLines)
                {   // New Line is through the merged cells
                   pCellTable[iNew].numLines++;
                }
             }
             iOld++;
          }
          else
          {      // It's a new cell
          // It's difficult to determine new cell's first cell by now
          // set it as uncertain.
             InitACell(&pCellTable[iNew],hFormBox,iNew,UNCERTAINCELL);
          }
      }
   }
   free(ConvertTable);

// free the old cell table.
   HandleUnlock(hOldCellTable);
   HandleFree(hOldCellTable);

// Fill back new cells' iFirst field.
   for (i=0;i<pFormBox->numCols*pFormBox->numLines;i++)
   {
      switch(pCellTable[i].iFirst)
      {
         case FIRSTCELL:         // This is a first cell.
            for (j=0;j<pCellTable[i].numLines;j++)
            {
               for (k=0;k<pCellTable[i].numCols;k++)
               {
                  i0=i+j*pFormBox->numCols+k;
                  if (pCellTable[i0].iFirst==UNCERTAINCELL)
                  // if a new cell belongs to i, fill back its iFirst field.
                     pCellTable[i0].iFirst=i;
               }
            }
            break;

         case UNCERTAINCELL:
         // This is a new cell, and it does not belong to other merged cells
            pCellTable[i].iFirst=FIRSTCELL;
            break;
      }
   }

   //readjust cells' text, implememted by Dg 1996,2
   if (bText)
       FBInsALineText(hFormBox,iNewLine*pFormBox->numCols,pFormBox->numCols);

   GlobalTextPosition=0;
   GlobalTableCell=0;
   ReFormatTableText(hFormBox,TRUE);
   {
   HANDLE NewHBox;
   int CursorX, CursorY;
   CursorLocate(hFormBox,&NewHBox,GlobalTextPosition,&CursorX,&CursorY);
   }

   // return successfully.
   HandleUnlock(ItemGetHandle(hFormBox));
   return(0);
}

/*
   FBInsAVert(hFormBox,iNewCol): Form Box Insert a Vertical Line.
      Insert a Vert line in the form.
      Vert line is specified by iNewCol;
*/
int FBInsAVert(HFormBoxs hFormBox,int iNewCol,BOOL bText)
{
   int iOld,iNew,i,j,k,wNewCol,i0,iOldCol;
   int * ConvertTable;
   PFormBoxs pFormBox;
   CELL * pCellTable;
   CELL * pOldCellTable;
   HANDLE hOldCellTable;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return -1;

// set iNewCol in appropriate range.
   if (iNewCol<1 || iNewCol>pFormBox->numCols)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(-1);
   }

   // if New Col is not the first Column, its width will be assigned as
   // the width of its left column.
   wNewCol=pFormBox->vertline[iNewCol]-pFormBox->vertline[iNewCol-1];

   UndoInsertTableInsertCol(iNewCol);

// readjust vertline[] field and numCols field.
   for (i=pFormBox->numCols+1;i>iNewCol;i--)
   {
   // All the columns right to the New Col will just be moved. Their width
   // will not be changed.
      pFormBox->vertline[i]=pFormBox->vertline[i-1];
   }
   pFormBox->numCols++;
   pFormBox->vertline[iNewCol]-=wNewCol/2;

// Get the pointer of old cell table.
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }
   pOldCellTable=pCellTable;
   hOldCellTable=pFormBox->hCellTable;

// allocate space for new cell table and get its pointer.
   pFormBox->hCellTable=HandleAlloc(pFormBox->numCols*pFormBox->numLines*sizeof(CELL),0);
   if (pFormBox->hCellTable==NULL)
      return(OUTOFMEMORY);
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

// move the cells' data of old cell table into new cell table.
   iOld=0;
   ConvertTable=malloc((pFormBox->numCols-1)*pFormBox->numLines*sizeof(int));
   if (ConvertTable==NULL)
      return(OUTOFMEMORY);
   for (i=0;i<pFormBox->numLines;i++)
   {
      for (j=0;j<pFormBox->numCols;j++)
      {
          iNew=i*pFormBox->numCols+j;
          if (j!=iNewCol)
          {      // It's not a new cell
             ConvertTable[iOld]=iNew;

             pCellTable[iNew]=pOldCellTable[iOld];
             pCellTable[iNew].iSelf=iNew;
             if (pCellTable[iNew].iFirst!=FIRSTCELL)
             {    // it's a merged cell
                pCellTable[iNew].iFirst=ConvertTable[pCellTable[iNew].iFirst];
             }
             else
             {   // it's the first cell of merged cells
                iOldCol=iOld%(pFormBox->numCols-1);
                if(iOldCol<iNewCol && iNewCol<iOldCol+pCellTable[iNew].numCols)
                {    // New Column is through the merged cells
                   pCellTable[iNew].numCols++;
                }
             }
             iOld++;
          }
          else
          {      // It's a new cell
          // It's difficult to determine new cell's first cell by now,
          // so we set it as uncertain.
             InitACell(&pCellTable[iNew],hFormBox,iNew,UNCERTAINCELL);
          }
      }
   }
   free(ConvertTable);

// free the old cell table.
   HandleUnlock(hOldCellTable);
   HandleFree(hOldCellTable);

// Fill back new cells' iFirst field.
   for (i=0;i<pFormBox->numCols*pFormBox->numLines;i++)
   {
      switch(pCellTable[i].iFirst)
      {
         case FIRSTCELL:         // This is a first cell.
            for (j=0;j<pCellTable[i].numLines;j++)
            {
               for (k=0;k<pCellTable[i].numCols;k++)
               {
                  i0=i+j*pFormBox->numCols+k;
                  if (pCellTable[i0].iFirst==UNCERTAINCELL)
                  // if a new cell belongs to i, correct its iFirst field.
                     pCellTable[i0].iFirst=i;
               }
            }
            break;

         case UNCERTAINCELL:
         // This is a new cell, and it does not belong to other merged cells
            pCellTable[i].iFirst=FIRSTCELL;
            break;
      }
   }

// readjust cells' text, implememted by DG in 1996,2
   if (bText)
       FBInsAColText(hFormBox,iNewCol,pFormBox->numCols,pFormBox->numLines);

   GlobalTextPosition=0;
   GlobalTableCell=0;
   ReFormatTableText(hFormBox,TRUE);
   {
   HANDLE NewHBox;
   int CursorX, CursorY;
   CursorLocate(hFormBox,&NewHBox,GlobalTextPosition,&CursorX,&CursorY);
   }

// return successfully.
   HandleUnlock(ItemGetHandle(hFormBox));
   return(0);
}
#endif  // UNUSED

int FBSlipCell(HFormBoxs hFormBox,int iCell,int type)
{
   PFormBoxs pFormBox;
   CELL *pCellTable;
   int ret;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return(OUTOFMEMORY);
   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return(OUTOFMEMORY);
   }

   if(pCellTable[iCell].bSlip==type) ret=-1;
   else
   {
      UndoInsertTableChangeSlant(iCell,pCellTable[iCell].bSlip);
      pCellTable[iCell].bSlip=type;
      ret=0;
   }

   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
   return ret;
}

int TableBoxGetMinVertline(PFormBoxs pFormBox)
{
   return(pFormBox->vertline[0]);
}

//new
int TableBoxGetMinHortline(PFormBoxs pFormBox)
{
   return (pFormBox->hortline[0]);
}

//new
int TableBoxGetMinWidth(PFormBoxs pFormBox)
{
   return (pFormBox->vertline[pFormBox->numCols]-pFormBox->vertline[0]);
}

//new
int TableBoxGetMinHeight(PFormBoxs pFormBox)
{
   return (pFormBox->hortline[pFormBox->numLines]-pFormBox->hortline[0]);
}

int TableBoxGetMaxWidth(PFormBoxs pFormBox)
{
   return (pFormBox->vertline[pFormBox->numCols]);
}

int TableBoxGetMaxHeight(PFormBoxs pFormBox)
{
   return (pFormBox->hortline[pFormBox->numLines]);
}

int TableBoxGetLineFromCell(HFormBoxs hFormBox,int iCell)
{
   PFormBoxs pFormBox;
   int  num;

   if(iCell<0) return 0;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return 0;

   num=iCell/pFormBox->numCols;

   HandleUnlock(ItemGetHandle(hFormBox));
   return num;
}

int TableBoxGetColFromCell(HFormBoxs hFormBox,int iCell)
{
   PFormBoxs pFormBox;
   int  num;

   if(iCell<0) return 0;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return 0;

   num=iCell%pFormBox->numCols;

   HandleUnlock(ItemGetHandle(hFormBox));
   return num;
}

void TableBoxCalculate(HFormBoxs hFormBox,int iCellBegin,int iCellEnd)
{
   PFormBoxs pFormBox;
   Wchar *FormTextBlock;
   CELL *pCellTable;
   int iLineBegin,iColBegin,iLineEnd,iColEnd,iCell;
   int pos,i,j,str_i,ch,num;
   double sum;
   char str[64];
   Wchar TextString[64];

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return;

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return;
   }

   FormTextBlock=HandleLock(TableBoxGetTextHandle(pFormBox));
   if (FormTextBlock==NULL)
   {
      HandleUnlock(ItemGetHandle(HBox));   //I think HBox is error
                                           // the right is hFormBox
                                           //so i modify it. By zjh 10.23
      HandleUnlock(pFormBox->hCellTable);
      HandleUnlock(ItemGetHandle(hFormBox));
      return;
   }

 // Compute Block's start line column and end line column.
   iLineBegin=iCellBegin/pFormBox->numCols;
   iColBegin=iCellBegin%pFormBox->numCols;
   iLineEnd=iCellEnd/pFormBox->numCols+pCellTable[iCellEnd].numLines-1;
   iColEnd=iCellEnd%pFormBox->numCols+pCellTable[iCellEnd].numCols-1;
   if (iLineBegin>iLineEnd) exchange(&iLineBegin,&iLineEnd);
   if (iColBegin>iColEnd) exchange(&iColBegin,&iColEnd);

   sum=num=0;
   for (i=iLineBegin;i<=iLineEnd;i++)
   {
      for (j=iColBegin;j<=iColEnd;j++)
      {
         iCell=i*pFormBox->numCols+j;
         if (pCellTable[iCell].iFirst==FIRSTCELL)
         {        // iCell is a First cell
            pos=TableCellGetTextHead(hFormBox,iCell);

           /*--- get cell text, and change it to val --*/
            str_i=0;
            while ( pos<TableBoxGetTextLength(pFormBox)
            && (ch=FormTextBlock[pos])!=TAB  )
            {
               pos++;
               if(GetPreCode(ch)==ENGLISHCHAR)
               {
                  if(ch<=BLANK) continue;
                  if( ch!='.' && (ch<'0' || ch>'9') )
                     goto lbl_err_num;

                  str[str_i++]=ch;
               }
               else
               if(GetPreCode(ch)>=CHINESECHARS)
               {
                lbl_err_num:
                  MessageBox(GetTitleString(ERRORINFORM),
                             "欲计算的表元中有非法数字!",
                             1,1);
                  goto lbl_calc_exit;
               }
            } /*- while -*/

            str[str_i]=0;
            sum+=atof(str);
            num++;
         }
      } /*---- j ----*/
   } /*--- i ----*/

   if(!num) num=1;

   i=MakeDialogBox(1,TableCalculateDialog);
   if(i>10000)
   {
      if(i==10092)  // (a1+a2+...+an)/n
         sum=sum/num;

      // printf("num=%d  sum=%f\n",num, sum);
     // sprintf(str,"%.4g",sum);
      sprintf(str,"%.4f",sum);
     ///*-----------------
      str_i=strlen(str)-1;
      while(str[str_i]!='.')
      {
         if(str[str_i]!='0')
            break;
         str_i--;
      }

      if(str[str_i]=='.')  str[str_i]=0;     // only int_number
      else str[str_i+1]=0;         // trunc 0_string
      //-------------------------*/
      str_i=0;
      while((ch=str[str_i]))
         TextString[str_i++]=ch;

      ClipBoardInsert(str_i*sizeof(Wchar)+sizeof(ClipBoards));
      ClipBoardAppend(TextString,str_i*sizeof(Wchar),TEXTDATA);
   }

 lbl_calc_exit:
   //HandleUnlock(TableBoxGetTextHandle(FormBox));   //I think The FormBox is error
                                                     // so I modify it to pFormBox
                                                     //By zjh 10.23
   HandleUnlock(TableBoxGetTextHandle(pFormBox));
   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
}

int IsCellWidthValid(HBOX HBox,int width)
{
  if (BoxIsTableBox(HBox))
  {
      int Left,Top,Right,Bottom;
      int dLeft,dTop,dRight,dBottom;

      FBGetCellRect(HBox,GlobalTableCell,&Left,&Top,&Right,&Bottom);
      FBGetCellDistant(HBox,&dLeft,&dTop,&dRight,&dBottom);
      if (Right-Left<width+dLeft+dRight)
      {
           MessageBox(GetTitleString(WARNINGINFORM),
                "当前表元宽度太小, 无法排下\n"
                "当前字号的字, 请先用鼠标调\n"
                "整表元的宽度!",
                1,1);
           return 0;
      }
  }
  return 1;
}

void TableBoxGetTextStr(HFormBoxs hFormBox,int iCell,char *str,int max)
{
   PFormBoxs pFormBox;
   Wchar *FormTextBlock,ch;
   CELL *pCellTable;
   int len,pos,str_i;

   pFormBox=HandleLock(ItemGetHandle(hFormBox));
   if (pFormBox==NULL)
      return;

   pCellTable=HandleLock(pFormBox->hCellTable);
   if (pCellTable==NULL)
   {
      HandleUnlock(ItemGetHandle(hFormBox));
      return;
   }

   FormTextBlock=HandleLock(TableBoxGetTextHandle(pFormBox));

   if (FormTextBlock==NULL)
   {
      HandleUnlock(pFormBox->hCellTable);
      HandleUnlock(ItemGetHandle(hFormBox));
      return;
   }

   *str=0;
   if (pCellTable[iCell].iFirst==FIRSTCELL)
    {        // iCell is a First cell
        pos=TableCellGetTextHead(hFormBox,iCell);
        len=TableBoxGetTextLength(pFormBox);
        str_i=0;
        while ( str_i<max-1&&pos<len&&(ch=FormTextBlock[pos])!=TAB)
            {
               pos++;
               if (ch>' '&&ch<='z')
                {
                    str[str_i++]=ch;
                }
               else
               if (ch>0xa380+' '&&ch<=0xa380+'z')
                {
                    str[str_i++]=ch-0xa380;
                }
            }
    }
   str[str_i]=0;

   HandleUnlock(TableBoxGetTextHandle(pFormBox));
   HandleUnlock(pFormBox->hCellTable);
   HandleUnlock(ItemGetHandle(hFormBox));
}

