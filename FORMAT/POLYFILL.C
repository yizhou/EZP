/*-------------------------------------------------------------------
* Name: polyfill.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static VOID _windfill(LPFILLP pfill);

void DefaultEdgeFillLine(LPDC lpdc,int x1,int y1,int x2,int y2)
{
  line(x1,y1,x2,y2);
}

void DefaultLineFillLine(int x1,int x2,int y,LPDC lpdc)
{
  line(x1,y,x2,y);
}

void buf_scanline(int x1,int x2,int y1,LPDC lpdc);
static VOID _windfill(pfill)
LPFILLP pfill;
{
  int i,ind,lastInd,nextInd,j,k,l,m;
  int top,bottom,left,right;           /* DC parameter */
  LPEDGE lpedg;
  int maxY,thisY,lastY,nextY,x0,x1;
  BOOL deleteFlag;

  if (PrintingSign&&GlobalRorate90&&CurrentLineFillLine!=buf_scanline)
  //if (PrintingSign&&GlobalRorate90)
  {
  left   = myDC.left;
  //if (left<0) left = 0;
  right  = myDC.right;
  top    = myDC.top;
  bottom = myDC.bottom;
  }
  else
  {
  left   = pfill->lpdc->left;
  right  = pfill->lpdc->right;
  top    = pfill->lpdc->top;
  bottom = pfill->lpdc->bottom;
  }

  lpedg =  pfill->edgeTable;

                                       /*
                                          Init edge table queue. set all
                                          edge items free, and set used
                                          queue empty.
                                        */
  pfill->free = 0;
  pfill->used = NEG_ONE;
  for (i=0;i<MAXEDGN-1;i++) lpedg[i].nextItem = i+1;
  lpedg[i].nextItem = NEG_ONE;
  if (pfill->dotCount==0) return;
                                       /* sort the dots by Y cordinate. */
                                       /* save the sort index value in  */
                                       /* sortYarray, that is, sortYarray[0] */
                                       /* has the dot's number which has the */
                                       /* lowest Y cordinate value.     */
  SortY(pfill->ybuffer,pfill->sortYarray,pfill->dotCount);

  i = 0;
  ind = pfill->sortYarray[i];
  thisY = maxY = pfill->ybuffer[ind];

  do   {        /* from lowest Y to highest Y */

      while (thisY < maxY) {                    /* in this case, the      */
                                               /* Y cordinate of the     */
                                               /* scanline has not reach */
                                               /* the next to-be-reached */
                                               /* dot's Y cordinate      */

           if (thisY>=bottom) return;          /* if thisY is higher than */
                                               /* device bottom, we do    */
                                               /* need to fill            */
           if ( thisY>=top ) {
                                               /* Yes, we can draw scanlines */
                                               /* now. pick the first edge   */
                                               /* table value,do draw   fill */
               j = pfill->used;
               k = 0;
               x0 = 0;
               while (j!=NEG_ONE) {
                   if (k==0) {
                      x0 = lpedg[j].xStart;
                      k += lpedg[j].direction;
                      j =  lpedg[j].nextItem;
                      continue;
                   }
                   x1 = lpedg[j].xStart;
                   k += lpedg[j].direction;
                   j =  lpedg[j].nextItem;
                   if (k==0) {
                      if (x1<left) continue;     /* we still have to check   */
                      if (x0<left) x0 = left;    /* horizontal border        */
                      if (x0>=right) continue;
                      if (x1>=right) x1 = right-1;
                      //if (x0==x1&&x0==right-1) continue;
                      //if (x1>right) x1 = right-1;
                                                 /* do a scan fill           */
                      CurrentLineFillLine(x0,x1,thisY,pfill->lpdc);
                   }
               } /* while */
            } /* if */


                                              /* Now, we're going to move */
                                              /* to the next scan line.   */
            j =  pfill->used;
                                              /* For each edge item,add  */
                                              /* the delta-X to xStart   */
                                              /* field.                  */
            if (j!=NEG_ONE) {
                                              /* the first edge item     */
                 lpedg[j].xStart+=lpedg[j].dxInt;
                 lpedg[j].halfOne+=lpedg[j].dxFrac;
                 if (lpedg[j].halfOne>MAXFRAC) {
                       lpedg[j].xStart++;
                       lpedg[j].halfOne -= MAXFRAC;
                 }
                 x1 = lpedg[j].xStart;

                                              /* the second edge item    */
                                              /* and so on...            */
                                              /* x0 is last x value      */
                                              /* x1 is current x value   */

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
                                              /* current X should be greater */
                                              /* than last X                 */
                     if (x1>=x0) {
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
                                             /* insert this item            */
                     if (m==NEG_ONE) {
                          lpedg[k].nextItem = l;
                          pfill->used = k;
                     } else {
                          lpedg[k].nextItem = l;
                          lpedg[m].nextItem = k;
                     }
                     x1 = lpedg[j].xStart;     /* !!*/
                }  /* while */
            } /* if j */                       /* end of adjusting edge    */
                                               /* table                    */
            thisY ++;                          /* the next scan line       */

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
}  /* _windfill  */

/*---------------------------------------------------------------------*/
VOID SortY(ybuf,indbuf,cnt)
LPINT ybuf,indbuf;
int cnt;
{
    register int i,j,k,y;

    for (i=0;i<cnt;i++) indbuf[i] = i;
 /*----------
    i = 0;  indbuf[i] = i;
    i++;
    -----*/
    i=1;
    while (i<cnt) {
           y = ybuf[i];
           for (j=i-1;j>=0;j--) {
               k = indbuf[j];
               if (y<ybuf[k]) indbuf[j+1] = k;
               else goto lbl1;
           }
          lbl1:
           indbuf[j+1] = i++;
    }
}

/*----------------------------InsertEdge-----------------------------*/
BOOL InsertEdge(pfill,x0,y0,x1,y1,flag)      /* y0 must > y1 */
LPFILLP pfill;
int x0,y0,x1,y1,flag;
{
  LPEDGE  lpedg;
  int i,j,k;
  int dx,dy;

  lpedg = pfill->edgeTable;
  if ((i = pfill->free) == NEG_ONE) return FALSE;       /* may be panic error ! */
  pfill->free = lpedg[i].nextItem;

  lpedg[i].yMax = y0;
  lpedg[i].xStart = x1;
  lpedg[i].halfOne = MAXFRAC / 2;
  lpedg[i].direction = flag;

  dy = y0-y1;
  dx = x0-x1;
  if (dx==0) {
      lpedg[i].dxInt = lpedg[i].dxFrac = 0;
  } else if (dx>0) {
      lpedg[i].dxInt = dx / dy;
      lpedg[i].dxFrac = (LONG)(dx%dy)*MAXFRAC /dy;
  } else {   /* dx < 0 */
      j = dx / dy;
      k = dx % dy;
      if (k) {
              j--;
              k = dy + k;
      }
      lpedg[i].dxInt = j;
      lpedg[i].dxFrac = (LONG)k*MAXFRAC /dy;
  }

  j = pfill->used;
  if (j==NEG_ONE) {
       lpedg[i].nextItem = j;
       pfill->used = i;
  } else {
       k = NEG_ONE;
       while (j!= NEG_ONE && x1>lpedg[j].xStart) {
            k = j;
            j = lpedg[j].nextItem;
       }
       if (k==NEG_ONE) {
            lpedg[i].nextItem = j;
            pfill->used = i;
       } else {
            lpedg[i].nextItem = j;
            lpedg[k].nextItem = i;
       }
  }
  return(TRUE);
} /* InsertEdge */

BOOL PolyFillPolygon(lpdc,lppoint,lpdotn,polyn)
LPDC  lpdc;
LPPOINT lppoint;
LPINT lpdotn;
int   polyn;
{
   LPFILLP pfill;
   int i,j,k,l,dotn;
   int x0,y0;

   SetIntSign();
   pfill = &fillp;

   dotn = 0;
   k = 0;
   for (i=0;i<polyn;i++) {
                  l = j = lpdotn[i];
                  dotn += j;
                  while (l--) {
                           pfill->xbuffer[k] = lppoint[k].x;
                           pfill->ybuffer[k] = lppoint[k].y;
                           pfill->lastDot[k] = k-1;
                           pfill->nextDot[k] = k+1;
                           k++;
                  }
                  pfill->nextDot[k-1] = k-j;
                  pfill->lastDot[k-j] = k-1;
   }
   pfill->dotCount = dotn;
   pfill->lpdc = lpdc;
   _windfill(pfill);

   if(CurrentEdgeFillLine!=NULL)
   {
       k = 0;
       for (i=0;i<polyn;i++) {
           x0 = lppoint[k].x;
           y0 = lppoint[k].y;
           for (j=0;j<lpdotn[i]-1;j++) {
             CurrentEdgeFillLine(lpdc,lppoint[k].x,lppoint[k].y,
                   lppoint[k+1].x,lppoint[k+1].y);
             k++;
           }
           CurrentEdgeFillLine(lpdc,lppoint[k].x,lppoint[k].y,x0,y0);
           k++;
       }
   }

   ClearIntSign();
   return TRUE;
}

BOOL FillPolygon(lpdc,lppoint,dotn)
LPDC lpdc;
LPPOINT lppoint;
int dotn;
{
   return PolyFillPolygon(lpdc,lppoint,&dotn,1);
}

/*----------
VOID PolyFillSetColor(lpdc,color)
LPDC lpdc;
int color;
{
  lpdc->color = color;
  setcolor(color);
}
-----------------*/
