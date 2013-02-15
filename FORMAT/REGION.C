//This is backup by ZJH 1997.01.17
#include <assert.h>
/*-------------------------------------------------------------------
* Name: region.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

/*------
#undef assert
#define assert(p) 0
-------*/

#define REDUCESCALE 10
#define REDUCE(size) ((size+REDUCESCALE/2)/REDUCESCALE)
#define ENLARGE(size) (size*REDUCESCALE)
/*----------------      ByHance


void * mymalloc(size_t size)
{
        void *ptr = malloc(size);
        if (ptr == NULL)
        {
          HANDLE waste;
          waste=HandleAlloc(10000,0);
          if (HandleLock(waste)==0)
          {
           //     printf("Cannot allocate space for hxg. hu\n");
          }
          HandleUnlock(waste);
          HandleFree(waste);
          ptr = malloc(size);
        }
        return ptr;
}
-------------*/


///////////////By zjh 10.29//////////////////////////////
int MaxRL(HBOX hBox,int *w,int *h)
{
     int uwCol,uhCol;
     PTextBoxs pBox;

      pBox=HandleLock(ItemGetHandle(hBox));
      if (pBox==NULL) return (OUTOFMEMORY);

      if (pBox->BoxColumn<=0) pBox->BoxColumn=1;
      uwCol=pBox->BoxWidth
              - pBox->ColumnDistant*((int)pBox->BoxColumn-1);

      uwCol=uwCol/pBox->BoxColumn-12;

      uhCol=pBox->BoxHeight - pBox->TextDistantTop - pBox->TextDistantBottom-12;
      *w=uwCol;
      *h=uhCol;

      HandleUnlock(ItemGetHandle(hBox));
      return 0;
}

int Y0ofRg(REGIONITEM *pRgItem)
{
/*-----------  ByHance, for speed --
  int xyDots[8];
  GetRgXY(pRgItem,xyDots);
  return xyDots[1];
-----------*/
   if (pRgItem->type==RT_RECT)
   {
      RRECT *p = (RRECT *)pRgItem->pRg;
      return (p->y0);
   } else
   // if (pRgItem->type==RT_TIXIN)
   {
      RTIXIN *p = (RTIXIN *)pRgItem->pRg;
      return (p->y0Top);
   }
}

/**********************************************************************
   xyDots is a TIXIN's coordinate. When a horizental line y=yCut cuts
   the TIXIN, the funciton will return the its intersection with the
   TIXIN in *x0 and *x1.
**********************************************************************/
void GetTXX0X1(int xyDots[],int yCut,int *x0,int *x1)
{
     float fX0,fX1;
     if(xyDots[0]==xyDots[6])           // xLeftTop==xLeftBottom
          *x0=xyDots[0];
     else
     {
         fX0=xyDots[0]+(float)(xyDots[6]-xyDots[0])/(xyDots[7]-xyDots[1])*(yCut-xyDots[1]);
         *x0=(int)(fX0+0.5);
     }

     if(xyDots[2]==xyDots[4])           // xRightTop==xRightBottom
          *x1=xyDots[2];
     else
     {
         fX1=xyDots[2]+(float)(xyDots[4]-xyDots[2])/(xyDots[5]-xyDots[3])*(yCut-xyDots[3]);
         *x1=(int)(fX1+0.5);
     }
}

/*******************************************************************
    From iRgFrom, find the next region include y and return its
    region number. if not found return -1.
*******************************************************************/
#ifdef UNUSED           // ByHance, 96,1.29
int NextRgWithY(REGIONITEM rgList[],int numRg,int iRgFrom,int y)
{
    int iRg;
    int xyDots[8];
    for (iRg=iRgFrom;iRg<numRg;iRg++)
    {
        GetRgXY(&(rgList[iRg]),xyDots);
        if (xyDots[1]<=y&&y<=xyDots[5])
           return iRg;
    }
    return -1;
}

int BoxGetRegionNo(PTextBoxs pBox,int x,int y)
{
  int xyDots[8];
  int iRg;

  for (iRg=0;iRg<pBox->numRg;iRg++)
  {
     GetRgXY(&(pBox->rgList[iRg]),xyDots);
     if (y>=xyDots[1]&&y<=xyDots[5])
     {
        int x0,x1;
        GetTXX0X1(xyDots,y,&x0,&x1);
        if (x0<=x&&x<=x1) return iRg;
     }
  }
  return -1;
}
#endif   //UNUSED           // ByHance, 96,1.29

/***********************************************************************
   judge if two regions are adjacent, We request iRg1+1==iRg2.
***********************************************************************/
static BOOL IsRgAdjacent(PTextBoxs pBox,int iRg1,int iRg2)
{
   int xyDots1[8],xyDots2[8];
   int x0,x1;
   REGIONITEM *rgList;

   assert(iRg1+1==iRg2);
  /*-----------------
   if (iRg1+1!=iRg2)                    // index must be adjacent
      return  FALSE;
    ------------------*/

   rgList=pBox->rgList;
   GetRgXY(&(rgList[iRg2]),xyDots2);
   GetRgXY(&(rgList[iRg1]),xyDots1);
   if (xyDots1[5]!=xyDots2[1])      // y2Top!=y1Bottom, y value must be adjacent
      return FALSE;

   //x0=max(xyDots1[6],xyDots2[0])+pBox->TextDistantLeft;    // left side
   //x1=min(xyDots1[4],xyDots2[2])-pBox->TextDistantRight;    // right side
   x0=max(xyDots1[6],xyDots2[0]);    // By zjh 1997.01.07
   x1=min(xyDots1[4],xyDots2[2]);    // By zjh 1997.01.07
   /*------
   if (x1-x0<=0)
      return FALSE;
   else
      return TRUE;
    ------*/
   return (x1>x0);
}

int BoxGetLine(PTextBoxs pBox,int iRg,int y,int RequiredHeight,
               int *x0Line,int *wLine,int *yNew)
{
   int xyDots[8];
   int x0Top,x1Top,x0Bot,x1Bot,x0,x1;
   int hCur,hNext;
   int x0Cur,x1Cur;
   //BOOL FirstRg=0;
   //BOOL Depend=0;

   //if (iRg==0&&y==Y0ofRg(&(pBox->rgList[0]))) FirstRg=1;   //By zjh 10.29
   //if (pBox->numRg==pBox->BoxColumn&&TextBoxDependPage(pBox)) Depend=1;   //By zjh 10.29

try_next:
    if (iRg>=pBox->numRg)
      {

       return -1;
      }

    GetRgXY(&(pBox->rgList[iRg]),xyDots);

    //if(y<xyDots[1]+pBox->TextDistantTop)  // ByHance, 96,3.18, for error
    //    y=xyDots[1]+pBox->TextDistantTop;
    if(y<xyDots[1])  // By Zjh 1997.01.07
        y=xyDots[1];

    if (y<=xyDots[5])     // yTop<= y <=yBottom
    {
       if (y+RequiredHeight<=xyDots[5])
       {        // This region has the requied height
          GetTXX0X1(xyDots,y,&x0Top,&x1Top);
          GetTXX0X1(xyDots,y+RequiredHeight,&x0Bot,&x1Bot);
          //x0=max(x0Top,x0Bot)+pBox->TextDistantLeft;
          //x1=min(x1Top,x1Bot)-pBox->TextDistantRight;
          x0=max(x0Top,x0Bot);   //By zjh 1997.01.07
          x1=min(x1Top,x1Bot);   //By zjh 1997.01.07
          // *x0Line=x0; *wLine=x1-x0;       // ByHance, 96,3.18, for distant
          if(x0<x1) {
             *x0Line=x0;
             *wLine=x1-x0;
             *yNew=y;
             return(iRg);
          }
          //-- else --
          y+=RequiredHeight;
          goto try_next;
       }
       else // Current region has not the required height
       {
       // look at ajacent regions for more height.
          if (iRg+1>=pBox->numRg)
            {
             iRg++;              //By zjh 10.29
             goto try_next;      //By zjh 10.29
             //return -1;
            }

          if (IsRgAdjacent(pBox,iRg,iRg+1))
          {   // Current region and Next region are adjacent
             *yNew=y;   hNext=RequiredHeight;
             x0Cur=xyDots[6];    x1Cur=xyDots[4];
             while(1)
             {
                 GetTXX0X1(xyDots,y,&x0Top,&x1Top);
                 x0Cur=max(x0Top,x0Cur);
                 x1Cur=min(x1Top,x1Cur);

                 hCur=xyDots[5]-y;          // LeftHeight of current region
                 hNext-=hCur;              // wanted height in next region
                 y=xyDots[5];
                 if (iRg+1>=pBox->numRg)
                   {
                    iRg++;              //By zjh 10.29
                    goto try_next;      //By zjh 10.29
                    //return -1;
                   }

                 iRg++;
                 GetRgXY(&(pBox->rgList[iRg]),xyDots);
                 if(y+hNext<=xyDots[5])
                 {
                    GetTXX0X1(xyDots,y+hNext,&x0Bot,&x1Bot);
                    //x0=max(x0Cur,x0Bot)+pBox->TextDistantLeft;
                    //x1=min(x1Cur,x1Bot)-pBox->TextDistantRight;
                    x0=max(x0Cur,x0Bot);   //By zjh 1997.01.07
                    x1=min(x1Cur,x1Bot);   //By zjh 1997.01.07
                    if(x0>=x1)
                    {
                         if (iRg+1>=pBox->numRg)
                          {
                            iRg++;              //By zjh 10.29
                            goto try_next;      //By zjh 10.29
                            //return -1;
                          }
                    lbl_skip_region:
                         iRg++;
                         y=Y0ofRg(&(pBox->rgList[iRg]));
                         goto try_next;
                    }

                    *x0Line=x0;
                    *wLine=x1-x0;
                    return iRg;
                 }
                 //----- else, try next region ---
                 if (iRg+1>=pBox->numRg)
                   {
                    iRg++;              //By zjh 10.29
                    goto try_next;      //By zjh 10.29
                    //return -1;
                   }
                 if(!IsRgAdjacent(pBox,iRg,iRg+1))
                    goto lbl_skip_region;
             } /*--- end of while ----*/

          }
          else   // Current region and next region are not adjacent.
             goto lbl_skip_region;
       }
    }
    else
    // y is not within current region. try to find the height required
    // from the begin of the next region.
    {
        if (iRg+1>=pBox->numRg)
           {
            iRg++;              //By zjh 10.29
            goto try_next;      //By zjh 10.29
            //return -1;
           }

        if (IsRgAdjacent(pBox,iRg,iRg+1))
        {            // next region yTop maybe large than y
           iRg++;
           goto try_next;
        }
        //- Current region and next region are not adjacent.
        goto lbl_skip_region;
    }
}

void GetRgXY(REGIONITEM *pRgItem,int xyDots[])
{
   if (pRgItem->type==RT_RECT)
   {
      RRECT *p = (RRECT *)pRgItem->pRg;
      xyDots[0]=p->x0;
      xyDots[1]=p->y0;
      xyDots[2]=p->x1;
      xyDots[3]=p->y0;
      xyDots[4]=p->x1;
      xyDots[5]=p->y1;
      xyDots[6]=p->x0;
      xyDots[7]=p->y1;
   } else
   // if (pRgItem->type==RT_TIXIN)
   {
      RTIXIN *p = (RTIXIN *)pRgItem->pRg;
      xyDots[0]=p->x0Top;
      xyDots[1]=p->y0Top;
      xyDots[2]=p->x1Top;
      xyDots[3]=p->y0Top;
      xyDots[4]=p->x1Bot;
      xyDots[5]=p->y1Bot;
      xyDots[6]=p->x0Bot;
      xyDots[7]=p->y1Bot;
   }
}

static void InsertRegion(REGIONITEM rgList[],int *numRg,
                         int posInsert,REGIONITEM *prgInsert)
{
   int num=*numRg;
   if (num>=MAXREGIONNUM)
   {
        MessageBox(GetTitleString(WARNINGINFORM),
                   GetInformString(TOOMUCHREGION),
                   1,1);
        return;
   }

   if(posInsert>num) posInsert=num;     // add ByHance

// for (i=num-1;i>=posInsert;i--) rgList[i+1]=rgList[i];  // ByHance
   memmove((void *)&rgList[posInsert+1],(void *)&rgList[posInsert],
               sizeof(rgList[0])*(num-posInsert) );
   rgList[posInsert]=*prgInsert;
   (*numRg)++;
}

void InsertTXRg(REGIONITEM rgList[],int * numRg,int posIns,
                int x0Top,int x1Top,int y0Top,
                int x0Bot,int x1Bot,int y1Bot)

{
   REGIONITEM rgTmp;
   RRECT * prrectTmp;
   RTIXIN * prtixinTmp;

   if (x0Top==x0Bot&&x1Top==x1Bot)
   {
   // it's a rectangle region
      prrectTmp=(RRECT *)malloc(sizeof(RRECT));
      //if(prrectTmp==NULL)
      if(prrectTmp<0x1000)
      {
         ReportMemoryError("insertrgrect");
         return;
      }

      prrectTmp->x0=x0Top;
      prrectTmp->x1=x1Top;
      prrectTmp->y0=y0Top;
      prrectTmp->y1=y1Bot;
      rgTmp.type=RT_RECT;
      rgTmp.pRg=(void *)prrectTmp;
   }
   else
   {
   // it's a tixin
      prtixinTmp=(RTIXIN *)malloc(sizeof(RTIXIN));
      //if(prtixinTmp==NULL)
      if(prtixinTmp<0x1000)
      {
         ReportMemoryError("insertrgTx");
         return;
      }

      prtixinTmp->x0Top=x0Top;
      prtixinTmp->x1Top=x1Top;
      prtixinTmp->y0Top=y0Top;
      prtixinTmp->x0Bot=x0Bot;
      prtixinTmp->x1Bot=x1Bot;
      prtixinTmp->y1Bot=y1Bot;
      rgTmp.type=RT_TIXIN;
      rgTmp.pRg=(void *)prtixinTmp;
   }
   InsertRegion(rgList,numRg,posIns,&rgTmp);
}


static void DeleteRegion(REGIONITEM rgList[],int * numRg,int posDel)
{
  int num=*numRg;

  if(posDel>=num) return;

  //MemFree(rgList[posDel].pRg);
  if(rgList[posDel].pRg!=NULL)
  {
      free(rgList[posDel].pRg);
      rgList[posDel].pRg=NULL;
  }

//for (i=posDel+1;i<num;i++) rgList[i-1]=rgList[i];  // ByHance
  memmove((void *)&rgList[posDel],(void *)&rgList[posDel+1],
               sizeof(rgList[0])*(num-posDel) );
  (*numRg)--;
}

/***************************************************************************
    Detect if a quadriple is a rectangle.
***************************************************************************/
#ifdef UNUSED   // ByHance, 96,1.30
static int IsRect(ORDINATETYPE xyList[])
{
   int x1,y1,x2,y2;
   x1=xyList[0];
   y1=xyList[1];
   x2=xyList[4];
   y2=xyList[5];

   if( xyList[2]==x2 && xyList[3]==y1 &&
       xyList[6]==x1 && xyList[7]==y2 )
      return 1;
   if( xyList[2]==x1 && xyList[3]==y2 &&
       xyList[6]==x2 && xyList[7]==y1 )
      return 1;

   return 0;
}
#endif  // UNUSED   // ByHance, 96,1.30

/***************************************************************************
     clear all region's data and free the memory allocated by the
     Region List.
***************************************************************************/
void FreeRL(PTextBoxs pBox)
{
   int i,num=pBox->numRg;
   for (i=0;i<num;i++) {
      //MemFree(pBox->rgList[i].pRg);
      free(pBox->rgList[i].pRg);
      pBox->rgList[i].pRg=NULL;
   }

   pBox->numRg=0;
}

// hlList is used to save information of the lines clipped out
//  from the current scan line by the polypolygons.
#define EDGELEFT 10000
#define EDGERIGHT 20000
/*--------------
static struct tagHORLINE
{
   int x0,x1;
   int iEdge0,iEdge1; //Index of the Edge Table.
} hlList[20];
static int numhlList;
-------------------------*/
typedef struct tagHORLINE
{
   int x0,x1;
   int iEdge0,iEdge1; //Index of the Edge Table.
} HORLINELIST;

/*************************************************************************
     Duplicate fillp's edgetable's entry iedge's content into pedgeNew.
     There are two exceptions,iedge==EDGELEFT or iedge==EDGERIGHT, at this
     time pedgeNew should be set as the Left border or the Right border of
     the current rect region clipped.
**************************************************************************/
static void DupEdge(EDGE *pedgeNew,FILLP *pfill,int iedge)
{
   LPEDGE lpedg;

   if (iedge==EDGELEFT)
   {
       pedgeNew->xStart=pfill->lpdc->left;
       pedgeNew->halfOne=0;
       pedgeNew->dxInt=0;
       pedgeNew->dxFrac=0;
       return;
   }

   if (iedge==EDGERIGHT)
   {
       pedgeNew->xStart=pfill->lpdc->right;
       pedgeNew->halfOne=0;
       pedgeNew->dxInt=0;
       pedgeNew->dxFrac=0;
       return;
   }

   lpedg = pfill->edgeTable;
   *pedgeNew=lpedg[iedge];
}

static void CleanRL(REGIONITEM rgList[],int * pnumRg)
{
   int iRg=0;
   int xyDots[8];

   while(iRg<*pnumRg)           // *pnumRg may be change when DeleteRegion
   {
      GetRgXY(&rgList[iRg],xyDots);
      if (xyDots[5]==xyDots[1])         // if region height=0, delete it
         DeleteRegion(rgList,pnumRg,iRg);
      else
         iRg++;
   }
}

static void RestoreReducedRgs(REGIONITEM rgList[],int numRg)
{
  int iRg;

  for (iRg=0;iRg<numRg;iRg++)
  {
       if (rgList[iRg].type==RT_RECT)
       {
          RRECT * p = (RRECT *)rgList[iRg].pRg;
          p->x0=ENLARGE(p->x0);
          p->x1=ENLARGE(p->x1);
          p->y0=ENLARGE(p->y0);
          p->y1=ENLARGE(p->y1);
       } else
       //if (rgList[iRg].type==RT_TIXIN)
       {
          RTIXIN * p = (RTIXIN *)rgList[iRg].pRg;
          p->x0Top=ENLARGE(p->x0Top);
          p->x1Top=ENLARGE(p->x1Top);
          p->y0Top=ENLARGE(p->y0Top);
          p->x0Bot=ENLARGE(p->x0Bot);
          p->x1Bot=ENLARGE(p->x1Bot);
          p->y1Bot=ENLARGE(p->y1Bot);
       }
  }
}

/**********************************************************************************
 This function is based on the code in function windfill() in paint.c
 which is an polypolygon filling algorithm. I added some new code to make
 it suit for generating a region list by clip a rectangle with the polypolygons.
**********************************************************************************/
static void GenNewRgListFromFillP(FILLP *pfill,REGIONITEM rgList[],int * numRg)
{
  int i,ind,lastInd,nextInd,j,k,l,m;
  int top,bottom,left,right;    /* RRect's position */
  int top1,bottom1,left1,right1;    /* RRect's position */
  LPEDGE lpedg;
  int maxY,thisY,lastY,nextY,x0,x1;
  BOOL deleteFlag;
  HORLINELIST hlList[20];
  int numhlList=0;

// variable added for region generation
// x0 means left x coordinate of a line,x1 means right coordinate.
// CL means Current Line. TX means TiXin.
  int wCLMax;
  int x0TXTop,x1TXTop,y0TXTop;
  int x0TXBot,x1TXBot,y1TXBot;
  int iLine,iLineMax;
  EDGE edge0,edge1;
  BOOL bTXTopFound;

// Init new added variables
  *numRg=0;
  bTXTopFound=FALSE;

// Init clipped rectangle;

  left1   = pfill->lpdc->left;
  right1  = pfill->lpdc->right;
  top1    = pfill->lpdc->top;
  bottom1 = pfill->lpdc->bottom;
 if (GlobalRorate90&&PrintingSign&&0)           //By zjh 9.7
   {
    left=top1;
    right=bottom1;
    top=PageHightDot-right1;
    bottom=PageHightDot-left1;
   }
   else
   {
    left=left1;
    right=right1;
    top=top1;
    bottom=bottom1;
    }

  lpedg =  pfill->edgeTable;

/* Init edge table queue: set all edge items free, and set used queue empty. */
  pfill->free = 0;
  pfill->used = NEG_ONE;

  for (i=0;i<MAXEDGN-1;i++) lpedg[i].nextItem = i+1;
  lpedg[i].nextItem = NEG_ONE;
  if (pfill->dotCount==0) return;

/* sort the dots by Y cordinate. save the sort index value in sortYarray, */
/* that is, sortYarray[0] has the dot's number which has the */
/* lowest Y cordinate value. */
  SortY(pfill->ybuffer,pfill->sortYarray,pfill->dotCount);

  i = 0;
  ind = pfill->sortYarray[i];
  thisY = maxY = pfill->ybuffer[ind];

  if (thisY>=bottom) return;
  if (thisY>top)
  {
  // Duplicate EdgeTable's information into edge0 and edge1
     DupEdge(&edge0,pfill,EDGELEFT);
     DupEdge(&edge1,pfill,EDGERIGHT);
  // Initialize New region's coordinate data
     x0TXTop=x0TXBot=left;
     x1TXTop=x1TXBot=right;
     y0TXTop=top;
     y1TXBot=thisY;
     bTXTopFound=TRUE;
  }

  do   {        /* from lowest Y to highest Y */
      while (thisY < maxY) {      /* in this case, the Y cordinate of the */
                                  /* scanline has not reach the next */
                                  /* to-be-reached_dot's Y cordinate */
           if (thisY>bottom)
           {
              if (bTXTopFound)
              {    // Last region exists.
              // Insert Last region at the end of the region list.
                 int posIns=*numRg;
                 y1TXBot=(thisY>bottom)?bottom:thisY;
                 InsertTXRg(rgList,numRg,posIns,
                               x0TXTop,x1TXTop,y0TXTop,
                               x0TXBot,x1TXBot,y1TXBot);
              }
              return;
             /* if thisY is higher than device bottom, we don't need to fill */
           }

           if (thisY>=top)
           {   /* draw scanlines now. pick the first edge table value,do draw fill */
               j = pfill->used;
               k = 0;
               x0 = 0;

            // Init hlList
               numhlList=0;
               hlList[numhlList].x0=left;
               hlList[numhlList].iEdge0=EDGELEFT;

               while (j!=NEG_ONE)
               {
                   if (k==0)
                   {     // save the right point's edge index.
                      hlList[numhlList].iEdge1=j;

                      x0 = lpedg[j].xStart;
                      k += lpedg[j].direction;
                      j =  lpedg[j].nextItem;
                      continue;
                   }
                // save the next line's left point's edge index.
                   hlList[numhlList+1].iEdge0=j;

                   x1 = lpedg[j].xStart;
                   k += lpedg[j].direction;
                   j =  lpedg[j].nextItem;
                   if (k==0)
                   {
                      if (x1<left) continue;     /* we still have to check   */
                      if (x0<left) x0 = left;    /* horizontal border        */
                      if (x0>=right) continue;
                      if (x1>right) x1 = right;
                                                 /* do a scan fill           */
                   // save current Lines right x coordinate
                   // and next Lines left x coordinate to the hlList.
                       hlList[numhlList].x1=x0;
                       hlList[numhlList+1].x0=x1;
                       numhlList++;
                   }
               } /* while */

            // set the last lines right edge as the right border.
               hlList[numhlList].x1=right;
               hlList[numhlList].iEdge1=EDGERIGHT;
               numhlList++;

            // find the lines with the max width.
               wCLMax=0;
               for (iLine=0;iLine<numhlList;iLine++)
               {  int ww=hlList[iLine].x1-hlList[iLine].x0;
                  if (ww>wCLMax)
                  {
                      wCLMax=ww;
                      iLineMax=iLine;
                  }
               }

       // if last region ends or a new region should start or both. process them
               if (bTXTopFound)
               { // Old region existed
                  if (edge1.xStart-edge0.xStart!=wCLMax || wCLMax==0)
                  {    // Last region ended.
                         // Insert Last region at the end of the region list.
                     int posIns=*numRg;
                     y1TXBot=(thisY>bottom)?bottom:thisY;
                     InsertTXRg(rgList,numRg,posIns,
                               x0TXTop,x1TXTop,y0TXTop,
                               x0TXBot,x1TXBot,y1TXBot);

                     if (wCLMax>0)
                     {      // New region exists. Start a new region.
                     // Duplicate EdgeTable's information into edge0 and edge1
                        DupEdge(&edge0,pfill,hlList[iLineMax].iEdge0);
                        DupEdge(&edge1,pfill,hlList[iLineMax].iEdge1);

                     // Initialize New region's coordinate data
                        x0TXTop=x0TXBot=hlList[iLineMax].x0;
                        x1TXTop=x1TXBot=hlList[iLineMax].x1;
                        y0TXTop=y1TXBot=thisY;

                        // bTXTopFound=TRUE;
                     }
                     else      // There is not new region
                        bTXTopFound=FALSE;
                  }
               }
               else
               {
               // Old region doesn't exists.
                  if (wCLMax>0)
                  // Find a new region,start it.
                  {
                  // Duplicate EdgeTable's information into edge0 and edge1
                     DupEdge(&edge0,pfill,hlList[iLineMax].iEdge0);
                     DupEdge(&edge1,pfill,hlList[iLineMax].iEdge1);

                  // Initialize New region's coordinate data
                     x0TXTop=x0TXBot=hlList[iLineMax].x0;
                     x1TXTop=x1TXBot=hlList[iLineMax].x1;
                     y0TXTop=y1TXBot=thisY;

                     bTXTopFound=TRUE;
                  }
               }
           } /* if */

              /* Now, we're going to move to the next scan line.   */
            j =  pfill->used;
              /* For each edge item,add the delta-X to xStart field. */
            if (j!=NEG_ONE) {

              // if TIXIN region exists, adjust TIXIN's bottom and edge0,edge1 too
                 if (bTXTopFound)
                 {           // adjust TX's bottom
                    x0TXBot=edge0.xStart;
                    x1TXBot=edge1.xStart;
                    y1TXBot=thisY;

                 // adjust edge0
                    edge0.xStart+=edge0.dxInt;
                    edge0.halfOne+=edge0.dxFrac;
                    if (edge0.halfOne>MAXFRAC)
                    {
                       edge0.xStart++;
                       edge0.halfOne -= MAXFRAC;
                    }

                 // adjust edge1
                    edge1.xStart+=edge1.dxInt;
                    edge1.halfOne+=edge1.dxFrac;
                    if (edge1.halfOne>MAXFRAC)
                    {
                       edge1.xStart++;
                       edge1.halfOne -= MAXFRAC;
                    }
                 }

                       /* the first edge item */
                 lpedg[j].xStart+=lpedg[j].dxInt;
                 lpedg[j].halfOne+=lpedg[j].dxFrac;
                 if (lpedg[j].halfOne>MAXFRAC) {
                       lpedg[j].xStart++;
                       lpedg[j].halfOne -= MAXFRAC;
                 }
                 x1 = lpedg[j].xStart;

               /* the 2nd edge item: x0 is last x value, x1 is current value */
                 while ((k = lpedg[j].nextItem)!=NEG_ONE) {
                     x0 = x1;
                           /* calculate out current X */
                     lpedg[k].xStart+=lpedg[k].dxInt;
                     lpedg[k].halfOne+=lpedg[k].dxFrac;
                     if (lpedg[k].halfOne>MAXFRAC) {
                         lpedg[k].xStart++;
                         lpedg[k].halfOne -= MAXFRAC;
                     }
                     x1 = lpedg[k].xStart;
                     if (x1>=x0)
                     {       /* current X should be greater than last X */
                            j = k;
                            continue;
                     }
                               /* delete this item from queue */
                     lpedg[j].nextItem = lpedg[k].nextItem;
                                              /* find the correct position   */
                                              /* of this item                */
                     m = NEG_ONE;
                     l = pfill->used;
                     while (l!=NEG_ONE && x1>=lpedg[l].xStart){
                             m = l;
                             l = lpedg[l].nextItem;
                     }
                                             /* insert this item  */
                     if (m==NEG_ONE) {
                          lpedg[k].nextItem = l;
                          pfill->used = k;
                     } else {
                          lpedg[k].nextItem = l;
                          lpedg[m].nextItem = k;
                     }
                     x1 = lpedg[j].xStart;     /* !!*/
                }  /* while */
            } /* if j */                 /* end of adjusting edge  table */

            thisY ++;                   /* the next scan line       */
      } /* while thisY ... */

                                               /* Now, the scan line reach */
                                               /* maxY,that means, we meet */
                                               /* a new dot. We have to    */
                                               /* delete the lower edge and */
                                               /* add in some higher edges. */

                                            /* if last dot is higher than   */
                                            /* this dot,add edge(last,this) */
                                            /* to used edge queue.          */
                                            /* if (lastY==thisY) do nothing */

      deleteFlag = FALSE;                   /* indicate whether we have to  */
                                            /* delete edge from used queue. */
      lastInd = pfill->lastDot[ind];
      lastY = pfill->ybuffer[lastInd];
      if (lastY>=thisY) {
         if (lastY >thisY)
           InsertEdge(pfill,pfill->xbuffer[lastInd],lastY,pfill->xbuffer[ind],\
                      thisY,UPFLAG);
      } else deleteFlag = TRUE;

                                            /* if next dot is higher than   */
                                            /* this dot,add edge(this,next) */
                                            /* to used edge queue.          */
                                            /* if (nextY==thisY) do nothing */
      nextInd = pfill->nextDot[ind];
      nextY = pfill->ybuffer[nextInd];

      if (nextY>=thisY) {
             if (nextY > thisY)
                  InsertEdge(pfill,pfill->xbuffer[nextInd],nextY,pfill->\
                             xbuffer[ind], thisY,DOWNFLAG);
      }  else deleteFlag = TRUE;
                                           /* if deleteFlag == TRUE we have  */
                                           /* to delete the lower edges from */
                                           /* used queue.                    */
      if (deleteFlag) {
             j = pfill->used;
             k = NEG_ONE;
             while (j!=NEG_ONE) {
                 if (thisY>=lpedg[j].yMax) {
                       if (k==NEG_ONE){
                           pfill->used = lpedg[j].nextItem;
                           lpedg[j].nextItem = pfill->free;
                           pfill->free = j;
                           j = pfill->used;
                       } else {
                           lpedg[k].nextItem = lpedg[j].nextItem;
                           lpedg[j].nextItem = pfill->free;
                           pfill->free = j;
                           j = lpedg[k].nextItem;
                      }
                 } else {
                       k = j;
                       j = lpedg[j].nextItem;
                 }
             }
      }

      i++;                               /* the next sorted dot index   */

      if (i<pfill->dotCount) {
           ind = pfill->sortYarray[i];
           maxY = pfill->ybuffer[ind];
      }
  } while (i<pfill->dotCount);

  if (thisY<bottom)
  {
     int posIns=*numRg;
     if (bTXTopFound)
     {
        y1TXBot=thisY;
        InsertTXRg(rgList,numRg,posIns,
                   x0TXTop,x1TXTop,y0TXTop,
                   x0TXBot,x1TXBot,y1TXBot);

     }

     x0TXTop=x0TXBot=left;
     x1TXTop=x1TXBot=right;
     y0TXTop=thisY;
     y1TXBot=bottom;
     posIns=*numRg;
     InsertTXRg(rgList,numRg,posIns,
               x0TXTop,x1TXTop,y0TXTop,
               x0TXBot,x1TXBot,y1TXBot);
  }
}

/***************************************************************************
 void InitRL(PTextBoxs pBox) : Using the Coloumn data and Invalid
    Polygons data in TextBox to generate a Reglion List.
***************************************************************************/
void InitRL(PTextBoxs pBox)
{
   int i,j,k,l,dotn;
   struct tagDC dcBuf;       // ByHance,   struct tagDC dcBuf[2];
   LPFILLP pfillP;
   REGIONITEM rgListNew[MAXREGIONNUM];
   int numRgListNew;
   int iRect;
   RRECT *prrectTmp;

// 1.Free the old Region List.
   FreeRL(pBox);

// 2.Divide box into serval rectangle region, each region represents a coloumn.
   {
      unsigned int uwCol,uhCol;
      ORDINATETYPE xCol,yCol;

   // Compute Width of Coloumn.
      if (pBox->BoxColumn<=0) pBox->BoxColumn=1;
      /*-------- ByHance, 96,3.18 ----
      uwCol=pBox->BoxWidth - pBox->TextDistantLeft - pBox->TextDistantRight
              - pBox->ColumnDistant*((int)pBox->BoxColumn-1);
       -------------------------------*/
      //uwCol=pBox->BoxWidth
      //        - pBox->ColumnDistant*((int)pBox->BoxColumn-1);
      //By zjh 1997.01.07
      uwCol=pBox->BoxWidth - pBox->TextDistantLeft - pBox->TextDistantRight
              - pBox->ColumnDistant*((int)pBox->BoxColumn-1);

      uwCol/=pBox->BoxColumn;

   // Compute Height of Coloumn.
      uhCol=pBox->BoxHeight - pBox->TextDistantTop - pBox->TextDistantBottom;

    // Compute Left Top Corner's (x,y) Cordinate of Coloumns.
      yCol=pBox->TextDistantTop;

   // Generate Coloumns' Reglion List.
      for (i=0;i<pBox->BoxColumn;i++)
      {
         //By zjh 1997.01.07
         xCol=pBox->TextDistantLeft+i*(uwCol+pBox->ColumnDistant);
         //xCol=i*(uwCol+pBox->ColumnDistant);
         pBox->rgList[i].type=RT_RECT;
         pBox->rgList[i].pRg=malloc(sizeof(RRECT));
         //if(pBox->rgList[i].pRg==NULL)
         if(pBox->rgList[i].pRg<0x1000)
         {
            ReportMemoryError("initrl");
            return;
         }

         prrectTmp=(RRECT *)pBox->rgList[i].pRg;
         //prrectTmp->x0=xCol;
         prrectTmp->x0=xCol;
         prrectTmp->y0=yCol;
         prrectTmp->x1=xCol+uwCol;
         prrectTmp->y1=yCol+uhCol;
      }
      pBox->numRg=i;
   }

// 3.Initialize the data in fillP,Reduce the size of polypolygon in order to
//     lower the computing cost in polypolygon fill algorithm.
   fillp.lpdc=&dcBuf;      //  ByHance, fillp.lpdc=&(dcBuf[0]);
   pfillP = &fillp;

   i=dotn=k=0;
   for (;i<pBox->InvalidPolygons;i++)
   {
      l = j = pBox->InvalidEdges[i];
      dotn += j;
      while (l--)
      {
         pfillP->xbuffer[k] = REDUCE(pBox->InvalidBoxXY[2*k]);
         pfillP->ybuffer[k] = REDUCE(pBox->InvalidBoxXY[2*k+1]);
         pfillP->lastDot[k] = k-1;
         pfillP->nextDot[k] = k+1;
         k++;
      }
      pfillP->nextDot[k-1] = k-j;
      pfillP->lastDot[k-j] = k-1;
   }
   pfillP->dotCount = dotn;

// 4. Using Polypolygons to clip each rectangle region in region List.
   iRect=0;
   while(iRect<pBox->numRg)
   {
      prrectTmp=(RRECT *)pBox->rgList[iRect].pRg;
      fillp.lpdc->left=REDUCE(prrectTmp->x0);
      fillp.lpdc->right=REDUCE(prrectTmp->x1);
      fillp.lpdc->top=REDUCE(prrectTmp->y0);
      fillp.lpdc->bottom=REDUCE(prrectTmp->y1);

   // Generate a new region List from currect rectangle region and cliping
   //    polypolygons. The new regions' size is reduced too.
      numRgListNew=0;
      GenNewRgListFromFillP(&fillp,rgListNew,&numRgListNew);

      if (numRgListNew>0)
      {
        // restore the reduced regions to their orignal size
           RestoreReducedRgs(rgListNew,numRgListNew);
           CleanRL(rgListNew,&numRgListNew);

        // Delete Old rect region in region List.
           DeleteRegion(pBox->rgList,&(pBox->numRg),iRect);

        // Insert new regions in pBox's region List;
         /*------- must be changed !!! ByHance ----
           for (i=0;i<numRgListNew;i++)
           {
              InsertRegion(pBox->rgList,&(pBox->numRg),iRect,&(rgListNew[i]));
              iRect++;
           }
          ------- must be changed !!! ByHance -*/
          {  REGIONITEM *rg=&(pBox->rgList[0]);
             //-- leave some item space --
             memmove((void *)&rg[iRect+numRgListNew], (void *)&rg[iRect],
                    sizeof(rg[0])*(pBox->numRg-iRect) );
              //-- insert new item --
             memmove((void *)&rg[iRect], (void *)&rgListNew[0],
                    sizeof(rg[0])*numRgListNew );
             pBox->numRg+=numRgListNew;
             iRect+=numRgListNew;
          }
          /*---------*/
      }
      else
        iRect++;
   } // while
}

#ifdef REGION_DEBUG
PrintBoxRgList(PTextBoxs pBox)
{
  int iRg;
  int x0Top,x1Top,y0Top,x0Bot,x1Bot,y1Bot;

  printf("\nRegion List is:\n");

  for (iRg=0;iRg<pBox->numRg;iRg++)
  {
    if (pBox->rgList[iRg].type==RT_RECT)
    {
       RRECT * p = (RRECT *)pBox->rgList[iRg].pRg;
       x0Top=x0Bot=p->x0;
       x1Top=x1Bot=p->x1;
       y0Top=p->y0;
       y1Bot=p->y1;
    } else
    // if (pBox->rgList[iRg].type==RT_TIXIN)
    {
       RTIXIN * p = (RTIXIN *)pBox->rgList[iRg].pRg;
       x0Top=p->x0Top;
       x1Top=p->x1Top;
       y0Top=p->y0Top;
       x0Bot=p->x0Bot;
       x1Bot=p->x1Bot;
       y1Bot=p->y1Bot;
    }
    printf("iRg=%d\n",iRg);
    printf("    (%d,%d),%d\n",x0Top,x1Top,y0Top);
    printf("    (%d,%d),%d\n",x0Bot,x1Bot,y1Bot);
  }
}
#endif
