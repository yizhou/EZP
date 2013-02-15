
typedef short int SHORT;
typedef unsigned short Wchar,WORD,USHORT;
typedef unsigned char UCHAR,BYTE;
typedef unsigned int  UINT;
typedef unsigned long ULONG,DWORD;
typedef int BOOL;

typedef int HFILENO;

typedef BYTE lpFAR  *LPBYTE;
typedef char lpFAR  *LPCHAR;
typedef UCHAR lpFAR *LPUCHAR;
typedef SHORT lpFAR *LPSHORT;
typedef USHORT lpFAR *LPUSHORT;
typedef int lpFAR  *LPINT;
typedef long lpFAR  *LPLONG;
typedef ULONG lpFAR *LPULONG;

typedef int ORDINATETYPE;
typedef int HANDLE;
typedef HANDLE HWND;
typedef HANDLE HMSG;
typedef HANDLE HMENU;
typedef HANDLE HGROUP;
typedef HANDLE HITEM;
typedef HANDLE HTEXT;
typedef HANDLE HLINE;
typedef HANDLE HPAGE;
typedef HANDLE HBOX;
typedef HBOX   HTEXTBOX;
typedef HBOX   HFormBoxs;
typedef HBOX   HLineBoxs;
typedef HBOX   HPICTUREBOX;

typedef int TEXTTYPE;



/* Follow is define of Table Boxs */

// used for CELL's iFirst field
#define FIRSTCELL -1
#define UNCERTAINCELL -2

typedef struct tagCELL
{
   HFormBoxs hParentBox;
   int numLines,numCols;
   int iFirst;
   int iSelf;
   int bSlip;           // Add By DG
} CELL;

typedef struct tagFormBoxs
{
  unsigned char BoxStatus; // see doc.txt
  unsigned char BoxType;   // see doc.txt
  ORDINATETYPE BoxLeft,BoxTop,BoxWidth,BoxHeight;
  WORD RotateAngle;        /* 0-359 */          //By zjh  9.12  /96
  WORD BoxAttr;                                 //By zjh  9.12  /96
  ORDINATETYPE RotateAxisX,RotateAxisY;
  HANDLE BoxBackColor;
  int BoxBorderType;
  int BoxBorderWidth;
  short RoundDistantLeft,RoundDistantTop,RoundDistantRight,RoundDistantBottom;
  short TextDistantLeft,TextDistantTop,TextDistantRight,TextDistantBottom;
  unsigned char TextAlign; // see doc.txt
  short numLines,numCols;  /* in TextBox, it is BoxColumn and ColumnDistant */
  int TextLength;
  int TextBlockLength;
  HANDLE TextHandle;
  HANDLE PrevLinkBox,NextLinkBox;      // Reserved for implement

  Wchar  *text;
  Pmark_rec formatted_elements,elements_end;

  int numRg;
  REGIONITEM rgList[MAXREGIONNUM];

  int reserved[10];

  int InvalidPolygons;
  short InvalidEdges[30];
  short InvalidBoxXY[10*MAXPOLYGONNUMBER];

    #define MAXFORMLINE 60
  int vertline[MAXFORMLINE],hortline[MAXFORMLINE];
  int vertlineType[MAXFORMLINE],hortlineType[MAXFORMLINE]; //By Dg, 1996,3
  HANDLE hCellTable;
} FormBoxs,*PFormBoxs;



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



/*------------------------- init ------------------*/
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
   pCell->hParentBox=hParentBox;
   pCell->numLines=1;
   pCell->numCols=1;
   pCell->iFirst=iFirst;
   pCell->iSelf=iSelf;
   pCell->bSlip=0;
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







/*------------------------------ ins a col ------------------*/
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

