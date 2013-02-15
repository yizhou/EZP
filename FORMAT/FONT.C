/*-------------------------------------------------------------------
* Name: font.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

//LPVOID jj_alloc_dotn();
//LPVOID jj_alloc_points();

static VOID  ShiftPoly(int x,int y,
                       LPPOINT lppoint,
                       LPINT lpdotn,
                       int polyn);

static BOOL GetChOutline(LPDC lpdc,
                  LPPOINT lppoint,
                  LPINT   lpdotn,
                  LPINT   lppolyn,
                  LPRCH   lprch,
                  LPMAT2  lpmatrix);
static BOOL VecOutline(LPDC lpdc,
                       Wchar code,
                       int font,
                       int ww,
                       int hh,
                       LPPOINT lppoint,
                       LPINT lpdotn,
                       LPINT lppolyn,
                       LPMAT2 lpmatrix);

static Wchar SymbolToGB(Wchar code);
static Wchar LaceCode(Wchar code);
static Wchar HorCodeToGB(Wchar code);
static Wchar VerCodeToGB(Wchar code);

//VOID SetFillMode (LPDC lpdc, int flag);
//int GetFillMode (LPDC lpdc);

int FontInitial(void)
{
  LPVECFNT Lpvfnt;

  cfnHandle=HandleAlloc(sizeof(VECFNT),0);
  if (cfnHandle==0)
     return(OUTOFMEMORY);

  efnHandle=HandleAlloc(sizeof(TTFINFO),0);
  if (efnHandle==0)
     return(OUTOFMEMORY);

  SysDc.left=SysDc.top=0;
  SysDc.right=SysDc.bottom=2048;
  //SysDc.polyFillMode=WINDING;
  if ((Lpvfnt=HandleLock(cfnHandle))==NULL)
     return(OUTOFMEMORY);
  strcpy (Lpvfnt->cfnPath,VectLibPath);      //By zjh 9.11
  Lpvfnt->cfnNumber = -1;
  Lpvfnt->cfnFile = NULL;
  HandleUnlock(cfnHandle);

  if ((SysDc.lpttf=HandleLock(efnHandle))==NULL)
  {
     HandleFree(cfnHandle);
     return(OUTOFMEMORY);
  }
  strcpy (SysDc.lpttf->ttPath,TrueTypeLibPath);           //By zjh 9.11

  SysDc.lpttf->ttfnNumber = -1;
  SysDc.lpttf->httf = -1;
  SysDc.lpttf->lpnumTables = (LPUSHORT)&SysDc.lpttf->ttfHeadBuf[NUMTABS];
  SysDc.lpttf->lpTableDirectory =(LPTABDIR)&SysDc.lpttf->ttfHeadBuf[TABLEDIR];

  HandleUnlock(efnHandle);

  ReturnOK();
}

int FontEnd(void)
{
  if (efnHandle)
     HandleFree(efnHandle);
  if (cfnHandle)
     HandleFree(cfnHandle);

  ReturnOK();
}

void set_buf_param(int w,int h,char *ptr);
void buf_sline(LPDC lpdc,int x1,int y1,int x2,int y2);
void buf_scanline(int x1,int x2,int y1,LPDC lpdc);

/******************************************************
     tmpbuffer bitmap builder  added by jerry
*******************************************************/
static int buf_w,buf_h,buf_wbyte;
static char *buf_ptr;

void set_buf_param(int w,int h,char *ptr)
{
 buf_w = w; buf_h = h; buf_ptr = ptr;
 buf_wbyte = (buf_w+7)>>3;
}

#define  buf_putdot(lpdc,x,y)     (buf_scanline(x,x,y,lpdc))
void buf_sline(LPDC lpdc,int x1,int y1,int x2,int y2)
{
    int x,y,xend,yend;
    int dx,dy,d,incr1,incr2;
    int flag1,flag2;
    int left,right,top,bottom;

    if (abs(x1-x2)<=abs(y1-y2)) return;
    if(y1==y2)
    {
        if(x1>x2) { x=x1; x1=x2; x2=x; }
        buf_scanline(x1,x2,y1,lpdc);
        return;
    }
    /*----------------clip the line -----------*/
    left = 0;
    right = buf_w;
    top = 0;
    bottom = buf_h;

    flag1 = flag2 = 0;
    dx =x2-x1;
    dy = y2-y1;

    if (x1<left) flag1 |= 1;
    if (x1>=right) flag1 |= 2;
    if (y1<top) flag1 |= 4;
    if (y1>=bottom) flag1 |=8;

    if (x2<left) flag2 |= 1;
    if (x2>=right) flag2 |= 2;
    if (y2<top) flag2 |= 4;
    if (y2>=bottom) flag2 |=8;

    if (flag1|flag2) {
         if (flag1&flag2) return;
         if (flag1&1){
              y1 = y1+dy*(left-x1)/dx;
              x1 = left;
         } else if (flag1&2) {
              y1 = y1+dy*(right-x1-1)/dx;
              x1 = right-1;
         } else if (flag1&4) {
              x1 = x1+dx*(top-y1)/dy;
              y1 = top;
         } else if (flag1&8) {
              x1 = x1+dx*(bottom-y1-1)/dy;
              y1 = bottom-1;
         }

         if (flag2&1){
              y2 = y2+dy*(left-x2)/dx;
              x2 = left;
         } else if (flag2&2) {
              y2 = y2+dy*(right-x2-1)/dx;
              x2 = right-1;
         } else if (flag2&4) {
              x2 = x2+dx*(top-y2)/dy;
              y2 = top;
         } else if (flag2&8) {
              x2 = x2+dx*(bottom-y2-1)/dy;
              y2 = bottom-1;
         }
    }

    if (x1<x2) {
           x = x1; y=y1;
           xend = x2; yend=y2;
    } else {
           x = x2; y=y2;
           xend = x1; yend=y1;
    }

    dx = abs(xend-x);
    dy = abs(yend-y);
    d = (dy<<1)-dx;
    incr1 = dy<<1;
    incr2 = (dy-dx)<<1;

    buf_putdot(lpdc,x,y);
    if (y<yend)
         while (x++<xend) {
            if (d<0) d+=incr1;
            else {
               y++;
               d+=incr2;
            }
            buf_putdot(lpdc,x,y);
         }
    else
         while (x++<xend) {
            if (d<0) d+=incr1;
            else {
               y--;
               d+=incr2;
            }
            buf_putdot(lpdc,x,y);
         }
}


void buf_scanline(int x1,int x2,int y1,LPDC lpdc)
{
   LONG byteoff;
   int headbit,tailbit,byte1,byte2;
   LPBYTE p;

   if (x1>x2) {
      int tmp=x2;
      x2=x1;
      x1=tmp;
   }

   if ((x1>=buf_w)||(x2<0)) return;
   if ((y1>=buf_h)||(y1<0)) return;

   if (x1<0) x1=0;
   if (x2>=buf_w) x2=buf_w-1;

   byte1 = x1>>3;
   byte2 = x2>>3;
   headbit = x1 & 7;
   tailbit = x2 & 7;

   byteoff = (y1 )*buf_wbyte+byte1;
   p = buf_ptr+byteoff;

   if (byte1==byte2) {
      *p |= headdot[headbit]&taildot[tailbit];
   } else {
      *p |= headdot[headbit];
      p++; byte1++;
      while (byte1++<byte2) *p++ = 0xff;
      *p |=taildot[tailbit];
   }

} /* buf_scanfill */


static void cachebuildch(lpdc,x,y,lprch,Color)
LPDC lpdc;
int x;
int y;
LPRCH lprch;
int Color;
{
     MAT2 /*matrix1,matrix2,*/matrix;
     POINT Points[1600];
     int Edges[40];
     LPPOINT lppoint;
     LPINT   lpdotn;
     int     polyn;
     int ww,hh,tmp_top,tmp_bottom,tmp_left,tmp_right;
     unsigned short code;
     int font,imagelen,wbyte;
     char bmpbuf[4096];
     EdgeFillLine *SaveEdgeFillLine;
     LineFillLine *SaveLineFillLine;

//   FILE *fp;
//   fp = fopen("ch.bin","wb");

     memset(bmpbuf,0,4096);
     ww = lprch->chSize.width;
     wbyte = (ww+7)>>3;
     hh = lprch->chSize.height;

     imagelen = wbyte*hh;
     code = lprch->code;
     if (lprch->codeType == EE_CHAR)
         font = lprch->cStyle.eefont;
     else font = lprch->cStyle.ccfont;

     if (GetCacheData(code,font,ww,bmpbuf,imagelen)!=OpOK) {
           SaveEdgeFillLine=CurrentEdgeFillLine;
           SaveLineFillLine=CurrentLineFillLine;

           set_buf_param(ww,hh,bmpbuf);
           tmp_top = lpdc->top;
           tmp_left = lpdc->left;
           tmp_right = lpdc->right;
           tmp_bottom = lpdc->bottom;
           //tmp_print = PrintingSign;

           lpdc->top = lpdc->left = 0;
           lpdc->right = lpdc->bottom = 128;
           //PrintingSign = 0;

           lppoint = (LPPOINT) Points;
           lpdotn =  (LPINT) Edges;
           Long2Fixed(matrix.eM11,0x10000);
           Long2Fixed(matrix.eM12,0x0000);
           Long2Fixed(matrix.eM21,0x0000);
           Long2Fixed(matrix.eM22,0x10000);

           if (!GetChOutline(lpdc,lppoint,lpdotn,&polyn,lprch,&matrix))
              {
                lpdc->top = tmp_top;        // By zjh add
                lpdc->left = tmp_left;
                lpdc->right = tmp_right;
                lpdc->bottom= tmp_bottom;   //By zjh add
                return ;
              }

           CurrentEdgeFillLine=buf_sline;
           CurrentLineFillLine=buf_scanline;
           PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);

           CurrentEdgeFillLine=SaveEdgeFillLine;
           CurrentLineFillLine=SaveLineFillLine;
           lpdc->top = tmp_top;
           lpdc->left = tmp_left;
           lpdc->right = tmp_right;
           lpdc->bottom= tmp_bottom;
           //PrintingSign = tmp_print;
           PutCacheData(code,font,ww,bmpbuf,imagelen);
     }
     if(!PrintingSign)
        copymono(bmpbuf,x,y-hh,wbyte<<3,hh,Color);
     else
        BuildImage(bmpbuf,x,y,ww,hh,0,0,Color);
}

static void _buildCh(lpdc,x,y,lprch)
LPDC lpdc;
int x;
int y;
LPRCH lprch;
{
     MAT2 matrix1,matrix2,matrix;

     POINT Points[1600];
     int Edges[40];
     LPPOINT lppoint;
     LPINT   lpdotn;
     int     polyn,i,j;
     int w;

/*
     lpdotn =  jj_alloc_dotn();
     lppoint = jj_alloc_points();
*/
/*     lpdotn =(LPINT)((char *)(lpdc->lpvfnt)+sizeof(FILLP)+sizeof(VECFNT));
     lppoint =(LPPOINT)((char *)lpdotn+400);
*/
     lppoint = (LPPOINT) Points;
     lpdotn = (LPINT) Edges;

//   if (lppoint==NULL||lpdotn==NULL) return;

     if (lprch->Slant.degree&&lprch->Slant.direction)
         if (lprch->Slant.direction == SLANT_LEFT) {
                /*
                     / 1     -sin(ang) \
                    |                   |
                    |                   |
                     \ 0         1     /
                */
                Long2Fixed(matrix1.eM11,0x10000);
                Long2Fixed(matrix1.eM12,(-LSin(lprch->Slant.degree)));
                Long2Fixed(matrix1.eM21,0x0000);
                Long2Fixed(matrix1.eM22,0x10000);
         } else {
                /*
                     / 1      sin(ang) \
                    |                   |
                    |                   |
                     \ 0         1     /
                */
                Long2Fixed(matrix1.eM11,0x10000);
                Long2Fixed(matrix1.eM12,LSin(lprch->Slant.degree));
                Long2Fixed(matrix1.eM21,0x0000);
                Long2Fixed(matrix1.eM22,0x10000);
      } else {
                /*
                     / 1         1  \
                    |                |
                    |                |
                     \ 0         1  /
                */
         Long2Fixed(matrix1.eM11,0x10000);
         Long2Fixed(matrix1.eM12,0x0000);
         Long2Fixed(matrix1.eM21,0x0000);
         Long2Fixed(matrix1.eM22,0x10000);
     }

     if (lprch->Rotate) {
         Long2Fixed(matrix2.eM11,LCos(lprch->Rotate));
         Long2Fixed(matrix2.eM12,-LSin(lprch->Rotate));
         Long2Fixed(matrix2.eM21,LSin(lprch->Rotate));
         Long2Fixed(matrix2.eM22,LCos(lprch->Rotate));

     } else {
         Long2Fixed(matrix2.eM11,0x10000);
         Long2Fixed(matrix2.eM12,0x0000);
         Long2Fixed(matrix2.eM21,0x0000);
         Long2Fixed(matrix2.eM22,0x10000);
     }

     MAT2Mul(&matrix1,&matrix2,&matrix);

     if (!GetChOutline(lpdc,lppoint,lpdotn,&polyn,lprch,&matrix)) return ;

         if (lprch->Hollow.toDo) {
                  ShiftPoly(x,y,lppoint,lpdotn,polyn);
                  //SetFillMode(lpdc,WINDING);          //By zjh 9.17
                  //SetColor(lpdc,sysColor);

                  w = lprch->Hollow.border;
                  if (w) {
                     for (j=0;j<w;j++)
                     {
                      ShiftPoly(-1,-1,lppoint,lpdotn,polyn);
                      PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }
                     for (j=0;j<w;j++)
                     {
                      ShiftPoly(1 , 0,lppoint,lpdotn,polyn);
                      PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }
                     for (j=0;j<w;j++)
                     {
                      ShiftPoly(1 , 0,lppoint,lpdotn,polyn);
                      PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }
                     for (j=0;j<w;j++)
                     {
                      ShiftPoly(0 , 1,lppoint,lpdotn,polyn);
                      PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }
                     for (j=0;j<w;j++)
                     {
                      ShiftPoly(0 , 1,lppoint,lpdotn,polyn);
                      PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }
                     for (j=0;j<w;j++)
                     {
                      ShiftPoly(-1, 0,lppoint,lpdotn,polyn);
                      PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }
                     for (j=0;j<w;j++)
                     {
                      ShiftPoly(-1, 0,lppoint,lpdotn,polyn);
                      PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }
                     for (j=0;j<w;j++)
                     {
                      ShiftPoly(0 ,-1,lppoint,lpdotn,polyn);
                      PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }
                     ShiftPoly(w ,0 ,lppoint,lpdotn,polyn);
                  }
                  SetDeviceColor(EGA_WHITE,1);
                  //SetDeviceColor(GetHollowColor(),1);         //By zjh 11.5
                  PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);

                  //for (j=0;j<w;j++)
                  //{
                  // ShiftPoly(1 ,0 ,lppoint,lpdotn,polyn);
                  // PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                  //}
         } else
         if (lprch->dim.toDo) {
                  ShiftPoly(x,y,lppoint,lpdotn,polyn);
                  //SetFillMode(lpdc,WINDING);                 //By zjh 9.17
                  //SetColor(lpdc,sysColor);
                  w = lprch->dim.border;
                  if (w) {
                     ShiftPoly(-w,-w,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     ShiftPoly(w , 0,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     ShiftPoly(w , 0,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     ShiftPoly(0 , w,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     ShiftPoly(0 , w,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     ShiftPoly(-w, 0,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     ShiftPoly(-w, 0,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     ShiftPoly(0 ,-w,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     ShiftPoly(w ,0 ,lppoint,lpdotn,polyn);
                  }
           switch (lprch->dim.direction) {
                case UPPERLEFT_BLOCK:
                     for (i=0;i<lprch->dim.width;i+=lprch->dim.step) {
                        ShiftPoly(-1,-1,lppoint,lpdotn,polyn);
                        PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }
                     ShiftPoly(i-1,i-1,lppoint,lpdotn,polyn);

                     SetDeviceColor(EGA_WHITE,1);
                     //SetDeviceColor(GetDimColor(),1);         //By zjh 11.5
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     break;
                case UPPERRIGHT_BLOCK:
                     for (i=0;i<lprch->dim.width;i+=lprch->dim.step) {
                        ShiftPoly(1,-1,lppoint,lpdotn,polyn);
                        PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }

                     SetDeviceColor(EGA_WHITE,1);
                     //SetDeviceColor(GetDimColor(),1);         //By zjh 11.5
                     ShiftPoly(-i+1,i-1,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     break;
                case LOWERLEFT_BLOCK:
                     for (i=0;i<lprch->dim.width;i+=lprch->dim.step) {
                        ShiftPoly(-1,1,lppoint,lpdotn,polyn);
                        PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }

                     SetDeviceColor(EGA_WHITE,1);
                     //SetDeviceColor(GetDimColor(),1);         //By zjh 11.5
                     ShiftPoly(i-1,-i+1,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     break;

                case LOWERRIGHT_BLOCK:
                     for (i=0;i<lprch->dim.width;i+=lprch->dim.step) {
                        ShiftPoly(1,1,lppoint,lpdotn,polyn);
                        PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     }

                     SetDeviceColor(EGA_WHITE,1);
                     //SetDeviceColor(GetDimColor(),1);         //By zjh 11.5
                     ShiftPoly(-i+1,-i+1,lppoint,lpdotn,polyn);
                     PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
                     break;
                default:
                     printf("unknow direction in buildChar()\n");
                     return  ;
          } /* switch */
     } else {
        ShiftPoly(x,y,lppoint,lpdotn,polyn);
        //SetFillMode(lpdc,WINDING);
        //SetColor(lpdc,sysColor);
        PolyFillPolygon(lpdc,lppoint,lpdotn,polyn);
     }
}

static VOID ShiftPoly(x,y,lppoint,lpdotn,polyn)
int x,y;
LPPOINT lppoint;
LPINT lpdotn;
int polyn;
{
     int i,j,k;
     k = 0;
     for (i=0;i<polyn;i++) {
          for (j=0;j<lpdotn[i];j++) {
            lppoint[k].x += x;
            lppoint[k].y += y;
            k++;
          }
     }
}

#ifdef UNUSED   // ByHance, 96,1.30
static BOOL MathOutline(lpdc,code,ww,hh,lppoint,lpdotn,lppolyn,lpmatrix)
LPDC lpdc;
Wchar code;
int  ww,hh;
LPPOINT lppoint;
LPINT lpdotn,lppolyn;
LPMAT2 lpmatrix;
{
  return(TRUE);
}
#else
   #define MathOutline(lpdc,code,ww,hh,lppoint,lpdotn,lppolyn,lpmatrix)  (1)
#endif


static BOOL GetChOutline(lpdc,lppoint,lpdotn,lppolyn,lprch,lpmatrix)
LPDC lpdc;
LPPOINT lppoint;
LPINT lpdotn,lppolyn;
LPRCH lprch;
LPMAT2 lpmatrix;
{
  Wchar nCode,t1;
  SHORT nFont;
  int ww,hh;

  ww = lprch->chSize.width;
  hh = lprch->chSize.height;

  switch (lprch->codeType) {
     case LACE_CHAR:
         nFont = LACEFONT;
         nCode = LaceCode(lprch->code);
         return VecOutline(lpdc,nCode,nFont,ww,hh,lppoint,lpdotn,lppolyn,lpmatrix);
     case CC_CHAR:
         nCode = lprch->code;
         t1 =nCode & 0xff00;

         if (!(nCode & 0xff00)) {
                           /*************** SBCS char ************/
                nFont = lprch->cStyle.eefont;
                nCode &= 0xff;
                return TTOutline(lpdc,nCode,nFont,ww,hh,lppoint,lpdotn,lppolyn,
                       lpmatrix);
         }

         if (t1==MATHTYPE)  /**********Math Char************************/
                return MathOutline(lpdc,nCode&0xff,ww,hh,lppoint,lpdotn,lppolyn,
                       lpmatrix);

         if (nCode>=GBCHAR) { /*********DBCS Character*****************/
                nFont = lprch->cStyle.ccfont;
                if (nFont>=100) {
                    nFont = (nFont)|0x8000;        // for Chinese TTF flag
                    return TTOutline(lpdc,nCode,nFont,ww,hh,lppoint,lpdotn,lppolyn,
                    lpmatrix);
                } else
                return VecOutline(lpdc,nCode,nFont,ww,hh,lppoint,lpdotn,lppolyn,
                       lpmatrix);
         }

         switch(t1) {
              case QUE:
                nCode = VerCodeToGB(nCode);
                return VecOutline(lpdc,nCode,SYMBOLFONT,ww,hh,lppoint,lpdotn,lppolyn,
                       lpmatrix);
              case QUF:
                nCode = HorCodeToGB(nCode);
                return VecOutline(lpdc,nCode,SYMBOLFONT,ww,hh,lppoint,lpdotn,lppolyn,
                       lpmatrix);
              case QU1:         /************GB symbols****************/
              case QU2:
              case QU4:
              case QU5:
              case QU6:
              case QU7:
              case QU8:
              default:
                nCode = SymbolToGB(nCode);
                return VecOutline(lpdc,nCode,SYMBOLFONT,ww,hh,lppoint,lpdotn,lppolyn,
                       lpmatrix);
                // return(FALSE);
         } /* switch t1 */
     case EE_CHAR:
         nCode = lprch->code&0xff;
         nFont = lprch->cStyle.eefont;
         return TTOutline(lpdc,nCode,nFont,ww,hh,lppoint,lpdotn,lppolyn,
                lpmatrix);

     case HG_SYMBOL:     /* for BD HG and FZ */
         nFont = SYMBOLFONT;
         nCode = SymbolToGB(lprch->code);
         return VecOutline(lpdc,nCode,nFont,ww,hh,lppoint,lpdotn,lppolyn,lpmatrix);
     default:
         return FALSE;
  } /* switch codetype */
}

/***********code convertion********************/
static Wchar LaceCode(Wchar code)
{
  USHORT i,q,w;

  if (code<NEWLACE*8||(code & 7)>3) {
      i =(((FIRSTLACE & 0xff00) >> 8) - 0x80)*0x80+(FIRSTLACE & 0xff) - 0x80;
      i += code;
      q = (i / 94)+0xb0; w = i % 94+0xa1;
      return ( (q<<8)|w);
  } else {
      q = (((code / 8)-LINELACE) & 7)+LINELACE;
      q *= 8;
      return (LaceCode(q+(code & 7)));
  }
}

static Wchar SymbolToGB(Wchar code)
{
/*****************
  USHORT i,q,w;
  i =(((code & 0xff00) >> 8) - 0x80)*0x80+(code & 0xff) - 0x80;
  q = (i / 94)+0xb0; w = i % 94+0xa1;
  return ( (q<<8)|w);
***************/
  USHORT hi,lo;

  hi = code /256;
  lo = code % 256;
  return ((hi+15)*256+lo);
}

static Wchar HorCodeToGB(Wchar code)
{
 USHORT i;
 static Wchar HorCodeTab[32]=
        { 0xa3a1,       /* ! */
          0xa1a2,       /* DunHao */
          0xa1a3,       /* Full Stop */
          0xa3ae,       /* . */
          0xa1bc,       /* [' */
          0xa1bd,       /* ]' */
          0xa3bf,       /* ? */
          0xa3a8,       /* ( */
          0xa3a9,       /* ) */
          0xa3ba,       /* : */
          0xa3bb,       /* ; */
          0xa3ac,       /* , */
          0xa1a8,       /* " */
          0xa1ae,       /* ` */
          0xa1af,       /* ' */
          0xa1b0,       /* " */
          0xa1b1,       /* " */
          0xa1b2,       /* [ */
          0xa1b3,       /* ] */
          0xa1b4,       /* < */
          0xa1b5,       /* > */
          0xa1b6,       /* <<*/
          0xa1b7,       /* >>*/
          0xa1be,       /* [ */
          0xa1bf,       /* ] */
          0xa3fb,       /* { */
          0xa3fd,       /* } */
          0xa3a7,       /* ' */
          0xa1ba,       /* " */
          0xa1bb,       /* " */
          0xa1b8,       /* ` */
          0xa1b9        /* , */
        };
  i = ((code & 0xff)-0xa1)%32;
  return(SymbolToGB(HorCodeTab[i]));
}

static Wchar VerCodeToGB(Wchar code)
{
 static Wchar VerCodeTab[32]=
        { 0xa594,       /* ! */
          0xa58f,       /* DunHao */
          0xa591,       /* Full Stop */
          0xa596,       /* . */
          0xa585,       /* [' */
          0xa58c,       /* ]' */
          0xa593,       /* ? */
          0xa580,       /* ( */
          0xa587,       /* ) */
          0xa595,       /* : */
          0xa592,       /* ; */
          0xa590,       /* , */
          0xa1a8,       /* " */
          0xa1b8,       /* ` */
          0xa1b9,       /* ' */
          0xa1ba,       /* " */
          0xa1bb,       /* " */
          0xa584,       /* [ */
          0xa58b,       /* ] */
          0xa581,       /* < */
          0xa588,       /* > */
          0xa582,       /* <<*/
          0xa589,       /* >>*/
          0xa586,       /* [ */
          0xa58d,       /* ] */
          0xa583,       /* { */
          0xa58a,       /* } */
          0xa3a7,       /* ' */
          0xa1ba,       /* " */
          0xa1bb,       /* " */
          0xa1b8,       /* ` */
          0xa1b9        /* , */
        };

  USHORT i;
  i = ((code & 0xff)-0xa1)%32;
  return(SymbolToGB(VerCodeTab[i]));
}

/*------------------low level font support --------*/
/* vec font... */

#define cfnPath   lpvfnt->cfnPath
#define cfnNumber lpvfnt->cfnNumber
#define cfnFile   lpvfnt->cfnFile
#define cfnHead   lpvfnt->cfnHead
#define cfnIndex  lpvfnt->cfnIndex
#define cbuffer   lpvfnt->cbuffer
#define libstart  lpvfnt->libstart
#define emsize    lpvfnt->emsize

static int cLastErrFont=-1;
static BOOL OpenCFont(lpvfnt,cfnt)
LPVECFNT lpvfnt;
int cfnt;
{
  char ext[4];
  char name[40];
  char *cp1, *cp2;
  USHORT i,j,k,l;
  USHORT crypt;
  USHORT key1,tmp;
  FILE *ff;

  if (cfnNumber == cfnt|| cfnt <= 0 || cfnt>999) return(TRUE);
  if(cfnNumber==-1 && cfnt==cLastErrFont)
       return(FALSE);

  ext[0]  = '0'+cfnt / 100;
  ext[1]  = '0'+(cfnt % 100)/10;
  ext[2]  = '0'+cfnt % 10;
  ext[3] = '\0';
  ff = NULL;

  cp1 = cfnPath;

  while (*cp1) {                        /* SEARCH FOR ALL PATH */
     cp2 = name;
     while(*cp1 != ';' && *cp1 != '\0')
                 *cp2++ = *cp1++;
     if (*(cp2-1) !='/' && *(cp2-1) != '\\') *cp2++ = '\\';
     if (*cp1 == ';') cp1++;
     strcpy(cp2,cfnName);       // THVECT.
     strcat(cp2,ext);
    // printf("name=%s\n",name);
     if ((ff = fopen(name,"rb")) != NULL) break;
  }

  if (ff == NULL) return FALSE;
  if (cfnNumber != -1 && cfnFile !=NULL) {
      fclose(cfnFile);
  }
  cfnFile = ff;
  cfnNumber = cfnt;

  /* init data */

  // added By Jerry  97/5/5
  if (fread((void*)&(cfnHead),1,256,ff)!=256)
  {
   lbl_read_err:
       fclose(ff);
       cfnNumber = -1;
       cLastErrFont = cfnt;
       return FALSE;
  }

  emsize = (USHORT) *(LPUSHORT)&cfnHead[128+10];
  libstart = (LONG) *(LPLONG)&cfnHead[128+12];
  crypt = (USHORT)*(LPUSHORT)&cfnHead[128+16];
  // fread((void *)&(cfnIndex),1,0x6c00,ff);
  { long readlen=0x6c00;
    if( readlen>libstart-0x100 )
         readlen=libstart-0x100;

    //added by Jerry  97/5/5
    if (fread((void *)&(cfnIndex),1,readlen,ff)!=readlen)
        goto lbl_read_err;
  }

  if (crypt == 1) {
       i = (libstart - 256)/2;
       if (i>0x3400) i = 0x3400;
       l = i;
       j = 0;
       key1 = 0x6467;
       for (k=0;k<l/2;k++) {
          tmp = (USHORT) *(LPUSHORT)&cfnIndex[j];
          tmp += 1122;
          tmp ^= 3344;
          tmp = (tmp/256)+(tmp%256)*256;
          *(LPUSHORT)&cfnIndex[j] = tmp;

          tmp = (USHORT) *(LPUSHORT)&cfnIndex[i];
          tmp += 5566;
          tmp ^= 7788;
          tmp = (tmp/256)+(tmp%256)*256;
          *(LPUSHORT)&cfnIndex[i] = tmp;

           *(LPUSHORT)&cfnIndex[i] ^= key1;
           *(LPUSHORT)&cfnIndex[j] ^= key1;

          tmp = (USHORT) *(LPUSHORT)&cfnIndex[i];
           *(LPUSHORT)&cfnIndex[i] =(USHORT) *(LPUSHORT)&cfnIndex[j];
           *(LPUSHORT)&cfnIndex[j] = tmp;
          j +=2; i+=2;
        }
  }
  return(TRUE);
}

/* close font function */
#ifdef UNUSED
static VOID CloseCFont(lpvfnt)
LPVECFNT lpvfnt;
{
 if (cfnFile !=NULL) fclose(cfnFile);
 cfnNumber = -1;
}
#endif

static BOOL VecOutline(lpdc,code,font,ww,hh,lppoint,lpdotn,lppolyn,lpmatrix)
LPDC lpdc;
Wchar code;
int font,ww,hh;
LPPOINT lppoint;
LPINT lpdotn,lppolyn;
LPMAT2 lpmatrix;
{
  LPLONG s,s1;
  USHORT ind,num,q,w;
  int len,i,j,k,dotsn,polyn;
  LONG x,y;
  FIXED fx,fy,f1,f2,f3,f4;
  LPVECFNT lpvfnt = lpdc->lpvfnt;

  if (!OpenCFont(lpvfnt,font)) return FALSE;
  if (cfnFile == NULL) return(FALSE);

  q  = ((code & 0x7f7f)>> 8)  - 0x20;
  w  = (code & 0x7f)  - 0x20;
  num = (q-16)*94+w-1;

  ind = num*4;

  s = (LPLONG)&cfnIndex[ind];
  s1 = (LPLONG)&cfnIndex[ind+4];
  len = (int)(*s1 - *s);
  if (len <=0)
  {
   lbl_err:
        fclose(cfnFile);
        return(FALSE);
  }

  /* read raw data */
  x=(long)(*s)+libstart;

//  added by Jerry 97/5/5
  if (fseek(cfnFile,x,SEEK_SET)!=0)
        goto lbl_err;
  if (len != fread(cbuffer,1,len,cfnFile))
        goto lbl_err;

  i = 0;
  j = 0;
  polyn = 0;

  dotsn = (unsigned int)cbuffer[i] + (unsigned int)cbuffer[i+1]*256;
  i+=2;
  while (dotsn) {
        *lpdotn++ = dotsn;
        polyn++;
        for (k = 0;k<dotsn;k++) {
             x = cbuffer[i++];
             y = cbuffer[i++];
             Long2Fixed(fx,(LONG)((x*ww/emsize)<<16));
             Long2Fixed(fy,(LONG)((y*hh/emsize)<<16));
             FixedMul(&fx,&(lpmatrix->eM11),&f1);
             FixedMul(&fy,&(lpmatrix->eM12),&f2);
             FixedMul(&fx,&(lpmatrix->eM21),&f3);
             FixedMul(&fy,&(lpmatrix->eM22),&f4);
             FixedAdd(&f1,&f2,&fx);
             FixedAdd(&f3,&f4,&fy);
             lppoint->x = IntofFixed(fx);
             lppoint->y = IntofFixed(fy);
             /*
             lppoint->x = x;
             lppoint->y = y;
             */
             lppoint++;
        }
        dotsn = (unsigned int)cbuffer[i] + (unsigned int)cbuffer[i+1]*256;
        i+=2;
  }
  *lppolyn = polyn;
///////////////************************//////////
  //fclose(cfnFile);
  return(TRUE);
}

/*--------------High level routines for REDTEK--------*/
int BuildChar(x,y,code,fnt,ww,hh,Slant,rota,Color,lib,chflag,pattern)
//USHORT x,y;    /* position */  //By zjh 10.29
int x,y;
Wchar code;
SHORT fnt;
//USHORT ww,hh;              //By zjh 10.29
int ww,hh;
SHORT Slant;
SHORT rota;
LONG Color;
SHORT lib;
USHORT chflag;
USHORT pattern;
{
   SHORT rotaten=rota;
   RCH  rch;
   USHORT t1;
   long dx,dy,sina,cosa,dxx,dyy;
   //SHORT rx,ry;
   int rx,ry;
   SHORT font;
   USHORT shadow_d,shadow_w;
   BOOL cacheable_char = TRUE;

   if (GlobalHollow)    //By zjh 1997.3.18 for big font
     {
       chflag |= HOLLOWBIT;
       SetHollowBorder(2);
     }

   /*
   if (GlobalExtFormat.hollow)
     {
       chflag |= HOLLOWBIT;
     }

   if (GlobalExtFormat.dim3)
     {
       chflag |= DIM3BIT;
     }
   */

   if (code>0xa0)
   {
      if ((SysDc.lpvfnt=HandleLock(cfnHandle))==NULL)
         return(OUTOFMEMORY);
   }
   else
   {
      if ((SysDc.lpttf=HandleLock(efnHandle))==NULL)
         return(OUTOFMEMORY);
   }
   /*if ((SysDc.lpvfnt=HandleLock(cfnHandle))==NULL)
      return(OUTOFMEMORY);
   if ((SysDc.lpttf=HandleLock(efnHandle))==NULL)
   {
      HandleUnlock(cfnHandle);
      return(OUTOFMEMORY);
   }*/

   rch.code = code;
   font = fnt;//+1;
   if (lib==LACELIB)  rch.codeType = LACE_CHAR;
   else {
         t1 =code & 0xff00;
         if (!(code & 0xff00)) {
               rch.code &= 0xff;
               rch.codeType = EE_CHAR;
               rch.cStyle.eefont = font;
          } else {
               rch.codeType = CC_CHAR;
               rch.cStyle.ccfont = font;
          }
   }

   rch.chSize.width = ww;
   rch.chSize.height = hh;

   if (ww>127 || hh>127 ||ww != hh) cacheable_char = FALSE;

/*-----------italic and rotate ------*/

   if (Slant&&chflag&RITALICBIT) {          /*redtek right italic */
         rch.Slant.degree = Slant;
         rch.Slant.direction = SLANT_LEFT;
         dx =(LSin(rch.Slant.degree)*hh)>>16;
         cacheable_char = FALSE;
   } else if (Slant&&chflag&LITALICBIT) {
         rch.Slant.degree = Slant;
         rch.Slant.direction = SLANT_RIGHT;
         dx = (-LSin(rch.Slant.degree)*hh)>>16;
         cacheable_char = FALSE;
   } else {
         rch.Slant.degree = 0;
         dx = 0;
   }

   dy = hh;
   if (rotaten&&chflag&ROTATEBIT) {
         rch.Rotate=rotaten;
         sina = LSin(-rotaten);  cosa = LCos(-rotaten);
         dxx = dx*cosa-dy*sina;
         dyy = dx*sina+dy*cosa;
         dx = dxx>>16;
         dy = dyy>>16;
         cacheable_char = FALSE;
   } else {
         rch.Rotate = 0;
   }
   rx = x+dx;
   ry = y-dy;

/*-----------hollow and dim3 ------*/
   if (chflag&HOLLOWBIT) {
         rch.Hollow.toDo = TRUE;
         //rch.Hollow.border = HOLLOWBORDER;
         rch.Hollow.border = GetHollowBorder();       //By zjh 11.5
         rch.dim.toDo = rch.Outline.toDo = FALSE;
         cacheable_char = FALSE;
   } else if (chflag&DIM3BIT) {
         rch.dim.toDo = TRUE;
         cacheable_char = FALSE;
         rch.dim.step = DIM3STEP;
         //rch.dim.border = DIM3BORDER;
         //shadow_w = CHAR_SHADOW_W(pattern);
         //shadow_d = CHAR_SHADOW_D(pattern);
         rch.dim.border = 0;
         shadow_w = GetDimBorder();                     //By zjh 11.5
         shadow_d = GetDimDir();                        //By zjh 11.5
         rch.dim.width = shadow_w*SHADOW_ASPECT;
         switch(shadow_d) {
              case 1:
                     rch.dim.direction = UPPERRIGHT_BLOCK;
                     break;
              case 2:
                     rch.dim.direction = UPPERLEFT_BLOCK;
                     break;
              case 3:
                     rch.dim.direction = LOWERLEFT_BLOCK;
                     break;
              default:
                     rch.dim.direction = LOWERRIGHT_BLOCK;
                     break;
         }
         rch.Hollow.toDo = rch.Outline.toDo = FALSE;
   } else {
         rch.Hollow.toDo = rch.dim.toDo = rch.Outline.toDo = FALSE;
   }

   SetDeviceColor(Color,1);
   if (cacheable_char) cachebuildch(&SysDc,x,y,&rch,Color);
   else
     _buildCh(&SysDc,rx,ry,&rch);
   if (code>0xa0)
      HandleUnlock(cfnHandle);
   else
      HandleUnlock(efnHandle);
   ReturnOK();
}
