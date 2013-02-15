/***************************************************
     META.C: written By Jerry Zhou
             for displaying graphics and chars
     units: 0.001 inch
***************************************************/
#include "ezpHead.h"

#define USEINDEX16      0               //16 color
#define USERGB          10              // use RGB color
#define USECMYK         20              // use CMYK color

static  int metaColorType = USEINDEX16;
static  int metaIndexcolor = EGA_BLACK;
static  int metaRed,metaGreen,metaBlue;
static  int metaCyan,metaMagenta,metaYellow,metaBlack;
static  int metaLineWidth = 0;              // linewidth
static  int metaLineType = 0;
static  int metaLineHeadEnd= 0;
// static  int metaPointNum = 0;
// static  POINT metaPoint[1000];

///////Font and Char operations....
static int metaEEFont = 1;
static int metaCCFont = 1;

static int metaCharW = 900;
static int metaCharH = 900;
static int metaCharSlant = 0;
static int metaCharRotate = 0;
static int metaCharHollow = 0;
static int metaChar3D = 0;
static int metaCharColor = EGA_BLACK;
static int metaCharDim3DBorder = 70;
static int metaCharDim3DDir = 0;
static int RGBTable[16][3] = {
        {0,0,0},               //BLACK
        {0,0,128},             //BLUE
        {0,128,0},             //GREEN
        {0,128,128},           //CYAN
        {128,0,0},             //RED
        {128,0,128},           //MAGENTA
        {128,128,0},           //BROWN
        {192,192,192},         //LIGHTGRAY
        {128,128,128},            //DARKGRAY
        {0,0,255},               //LIGHTBLUE
        {0,255,0},               //LIGHTGREEN
        {0,255,255},            //LIGHTCYAN
        {255,0,0},              //LIGHTRED
        {255,0,255},            //LIGHTMAGENTA
        {255,255,0},            //YELLOW
        {255,255,255}
   };
// RGBtoEGA(r,g,b)
// convert RGB color to EGA's 16 color
int RGBtoEGA(int r,int g,int b)
{
   int i,j,r1,g1,b1;
   long t1,t2;
   j = 0;
   t1 = r*r+g*g+b*b;
   for (i=1;i<16;i++) {
     r1 = RGBTable[i][0] - r;
     g1 = RGBTable[i][1] - g;
     b1 = RGBTable[i][2] - b;
     t2 = r1*r1+g1*g1+b1*b1;
     if (t2<t1) { t1 = t2; j = i; }
   }
   return j;
}

// CMYKtoEGA(c,m,y,k)
// convert CMYK color to EGA's 16 color
int CMYKtoEGA(int c,int m,int y, int k)
{
   static int CMYKTable[16][4] = {
        {255,255,255,255},                 //BLACK
        {128,128,0,128},                   //BLUE
        {128,0,128,128},                   //GREEN
        {128,0,0,128},                     //CYAN
        {0,128,128,128},                   //RED
        {0,128,0,128},                     //MAGENTA
        {0,0,128,128},                     //BROWN
        {0,0,0,80},                        //LIGHTGRAY
        {0,0,0,140},                      //DARKGRAY
        {255,255,0,0},                     //LIGHTBLUE
        {255,0,255,0},                     //LIGHTGREEN
        {255,0,0,0},                       //LIGHTCYAN
        {0,255,255,0},                     //LIGHTRED
        {0,255,0,0},                       //LIGHTMAGENTA
        {0,0,255,0},                       //YELLOW
        {0,0,0,0}
   };
   int i,j,c1,m1,y1,k1;
   long t1,t2;
   j = 0;
   t1 = c*c+m*m+y*y+k*k;
   for (i=1;i<16;i++) {
     c1 = CMYKTable[i][0] - c;
     m1 = CMYKTable[i][1] - m;
     y1 = CMYKTable[i][2] - y;
     k1 = CMYKTable[i][3] - k;
     t2 = c1*c1+m1*m1+y1*y1+k1*k1;
     if (t2<t1) { t1 = t2; j = i; }
   }
   return j;
}
// MetaSetRGBColor( int r,g,b)
// Note: rgb is color;
void MetaSetRGBColor(int r, int g, int b)
{
   if (PrintingSign)
   {
        (printer->printSetRGBcolor)(r,g,b);
   }
   else
        setcolor(RGBtoEGA(r,g,b));

   metaRed = r;
   metaGreen = g;
   metaBlue = b;
   metaColorType = USERGB;
}

// MetaSetCMYKColor( int c,m,y,k)
// Note: cmyk is color;
void MetaSetCMYKColor(int c, int m, int y, int k)
{
   if (PrintingSign)
   {
        (printer->printSetCMYKcolor)(c,m,y,k);
   }
   else
        setcolor(CMYKtoEGA(c,m,y,k));

   metaCyan = c;
   metaMagenta = m;
   metaYellow = y;
   metaBlack = k;
   metaColorType = USECMYK;
}

void MetaColor (int color)
{
    MetaSetRGBColor(RGBTable[color][0],
                    RGBTable[color][1],
                    RGBTable[color][2]);
}

void MetaSetLineWidth(int linewidth)  { metaLineWidth = linewidth;}
void MetaSetLineType(int type) { metaLineType = type; }

void MetaSetLineHead(int var)
{
  metaLineHeadEnd &= 0x0f;
  metaLineHeadEnd |= (var<<4);
}

void MetaSetLineEnd(int var)
{
  metaLineHeadEnd &= 0xf0;
  metaLineHeadEnd |= var;
}

static int UserWidthToWindowWidth(int wid)
{
  int tt;
  tt =  myUserXToWindowX(wid);
  if (tt<1) tt =  1;
  return tt;
}


static int UserYLToWindowYL(int len)
{
  return  myUserYToWindowY(len);
}

static int UserXLToWindowXL(int len)
{
  return  myUserXToWindowX(len);
}

static void fillrectangle(int x1,int y1,int x2,int y2)
{
  int min_x,min_y,max_x,max_y;
  int i;

  min_x=min(x1,x2);
  max_x=max(x1,x2);
  min_y=min(y1,y2);
  max_y=max(y1,y2);
  for (i = min_y; i<=max_y; i++)
       CurrentLineFillLine(min_x,max_x,i,&SysDc);
}

void MetaLine(int x1,int y1 ,int x2, int y2)
{
   int Left,Top,Right,Bottom;
   struct viewporttype TmpViewPort;

   if (!PrintingSign)
   {
     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
   }

   WithWidthLine(&SysDc,UserXToWindowX(x1),UserYToWindowY(y1),
                UserXToWindowX(x2),UserYToWindowY(y2),
                UserWidthToWindowWidth(metaLineWidth),
                metaLineType, metaLineHeadEnd,ORMODE);

   if (!PrintingSign)
   {
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
   }
}

void MetaMakeLineJoint(int x1,int y1,int x2,int y2, int x3,int y3)
{
   int Left,Top,Right,Bottom;
   struct viewporttype TmpViewPort;
   if (!PrintingSign)
   {
     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
   }
   makejoint(&SysDc,UserXToWindowX(x1),UserYToWindowY(y1),
                UserXToWindowX(x2),UserYToWindowY(y2),
                UserXToWindowX(x3),UserYToWindowY(y3),
                UserWidthToWindowWidth(metaLineWidth));
   if (!PrintingSign)
   {
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
   }
}

void MetaTriangle(int x1,int y1,int x2,int y2, int x3,int y3)
{
  MetaLine(x1,y1,x2,y2);
  MetaLine(x2,y2,x3,y3);
  MetaLine(x3,y3,x1,y1);
  MetaMakeLineJoint(x1,y1,x2,y2,x3,y3);
  MetaMakeLineJoint(x2,y2,x3,y3,x1,y1);
  MetaMakeLineJoint(x3,y3,x1,y1,x2,y2);
}

void MetaLines(LPPOINT lppoint, int dotn)
{
  int i;
  if (dotn<2) return;
  for (i=0;i<dotn-1;i++)
    MetaLine(lppoint[i].x,lppoint[i].y,lppoint[i+1].x,lppoint[i+1].y);

  for (i=0;i<dotn-2;i++)
    MetaMakeLineJoint(lppoint[i].x,lppoint[i].y,
                      lppoint[i+1].x,lppoint[i+1].y,
                      lppoint[i+2].x,lppoint[i+2].y);
}

void MetaPolygon(LPPOINT lppoint,int dotn)
{
  int i;
  if (dotn<3) return;
  for (i=0;i<dotn-1;i++)
    MetaLine(lppoint[i].x,lppoint[i].y,lppoint[i+1].x,lppoint[i+1].y);
  MetaLine(lppoint[i].x,lppoint[i].y,lppoint[0].x,lppoint[0].y);

  for (i=0;i<dotn-2;i++)
    MetaMakeLineJoint(lppoint[i].x,lppoint[i].y,
                      lppoint[i+1].x,lppoint[i+1].y,
                      lppoint[i+2].x,lppoint[i+2].y);

  MetaMakeLineJoint(lppoint[i].x,lppoint[i].y,
                    lppoint[i+1].x,lppoint[i+1].y,
                    lppoint[0].x,lppoint[0].y);
  i++;
  MetaMakeLineJoint(lppoint[i].x,lppoint[i].y,
                    lppoint[0].x,lppoint[0].y,
                    lppoint[1].x,lppoint[1].y);
}

void MetaRectangle(int x1, int y1, int x2, int y2)
{
  POINT p[4];
  p[0].x = x1; p[0].y = y1;
  p[1].x = x1; p[1].y = y2;
  p[2].x = x2; p[2].y = y2;
  p[3].x = x2; p[3].y = y1;
  MetaPolygon((LPPOINT) &p,4);
}

void MetaFillPolygon(LPPOINT lppnt,int dotn)
{
  int i;
  LPPOINT lppnt1;
  int Left,Top,Right,Bottom;
  struct viewporttype TmpViewPort;

  lppnt1 = malloc(sizeof(POINT) * dotn);
  if (lppnt1 == NULL) return;

  for (i=0;i<dotn;i++) {
    lppnt1[i].x = UserXToWindowX(lppnt[i].x);
    lppnt1[i].y = UserYToWindowY(lppnt[i].y);
  }

  if (!PrintingSign)
   {
     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
   }

  FillPolygon(&SysDc,lppnt1,dotn);

  if (!PrintingSign)
   {
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
   }
  free(lppnt1);
}

void MetaFillRectangle(int x1,int y1,int x2, int y2)
{
  POINT p[4];
  p[0].x = x1; p[0].y = y1;
  p[1].x = x1; p[1].y = y2;
  p[2].x = x2; p[2].y = y2;
  p[3].x = x2; p[3].y = y1;
  MetaFillPolygon((LPPOINT) &p,4);
}

void MetaFillTriangle(int x1,int y1,int x2, int y2,int x3,int y3)
{
  POINT p[3];
  p[0].x = x1; p[0].y = y1;
  p[1].x = x2; p[1].y = y2;
  p[2].x = x3; p[2].y = y3;
  MetaFillPolygon((LPPOINT) &p,3);
}

void MetaEllipse(int xc, int yc, int rx, int ry)
{
  POINT midpoint;
  int rx1,ry1;
  int Left,Top,Right,Bottom;
  struct viewporttype TmpViewPort;


  if (!PrintingSign)
   {
     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
   }

  midpoint.x=UserXToWindowX(xc);
  midpoint.y=UserYToWindowY(yc);
  rx1 = UserXLToWindowXL(rx);
  ry1 = UserYLToWindowYL(ry);

  WithArcTypeArc(&SysDc,midpoint,(float)rx1,(float)ry1,(float)0,2*PI,
                 ORMODE,
                 UserWidthToWindowWidth(metaLineWidth),
                 metaLineType);

  if (!PrintingSign)
   {
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
   }
}

void MetaCircle(int xc,int yc,int r)
{
   MetaEllipse(xc,yc,r,r);
}

void MetaArc(int xc,int yc, int xs, int ys, double angle)
{
   POINT MiddlePoint;
   float StartAngle;
   double goang;
   long r;
   int ox,oy, x,y;

   int Left,Top,Right,Bottom;
   struct viewporttype TmpViewPort;

   ox = UserXToWindowX(xc);
   oy = UserYToWindowY(yc);
   x = UserXToWindowX(xs);
   y = UserYToWindowY(ys);

   goang = -(double)angle*PI/180;

   MiddlePoint.x=ox;
   MiddlePoint.y=oy;
   r=sqrt((double)(x-ox)*(double)(x-ox)+(double)(y-oy)*(double)(y-oy))+0.5;
   if (r<=0) return;

   if (x==ox)
   {
      if (y<oy) StartAngle=PI*0.5;
      else StartAngle=PI*1.5;
   }
   else
   {
      if (y==oy)
         if (x>ox) StartAngle=0;
         else StartAngle=PI;
      else
      {
         float tg;

         tg = -(float)(y-oy)/(float)(x-ox);
         StartAngle=atan(tg);
         if (tg>0)
         {
            if (y>oy) StartAngle+=PI;
         }
         else
            if (y<oy) StartAngle+=PI;
      }
   }

  if (!PrintingSign)
   {
     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
   }

   StartAngle = 2*PI-StartAngle;
   if (goang>=0) {
       WithArcTypeArc(&SysDc,MiddlePoint,(float)r,(float)r,StartAngle,
                         StartAngle+goang,ORMODE,
                         UserWidthToWindowWidth(metaLineWidth),
                         metaLineType);
   } else {
       WithArcTypeArc(&SysDc,MiddlePoint,(float)r,(float)r,StartAngle+goang,
                         StartAngle,ORMODE,
                         UserWidthToWindowWidth(metaLineWidth),
                         metaLineType);
   }
  if (!PrintingSign)
   {
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
   }
}


void MetaRoundRectangle(int x1, int y1, int x2, int y2)
{
  int min_x,min_y,max_x,max_y;
  int xc,yc;
  int minworh;
  min_x=min(x1,x2);
  max_x=max(x1,x2);
  min_y=min(y1,y2);
  max_y=max(y1,y2);
  minworh=(min(max_x-min_x,max_y-min_y)+5)/10;
  MetaLine(min_x,min_y+minworh,min_x,max_y-minworh);
  MetaLine(max_x,min_y+minworh,max_x,max_y-minworh);
  MetaLine(min_x+minworh,min_y,max_x-minworh,min_y);
  MetaLine(min_x+minworh,max_y,max_x-minworh,max_y);
  xc=min_x+minworh;
  yc=min_y+minworh;
  MetaArc(xc,yc,min_x,yc,-90);
  xc=max_x-minworh;
  yc=min_y+minworh;
  MetaArc(xc,yc,xc,min_y,-90);
  xc=min_x+minworh;
  yc=max_y-minworh;
  MetaArc(xc,yc,min_x,yc,90);
  xc=max_x-minworh;
  yc=max_y-minworh;
  MetaArc(xc,yc,max_x,yc,-90);
}

void MetaCircleRectangle(int x1,int y1,int x2, int y2)
{
  int min_x,min_y,max_x,max_y;
  int xc,yc,r;

  min_x=min(x1,x2);
  max_x=max(x1,x2);
  min_y=min(y1,y2);
  max_y=max(y1,y2);

  r = (max_y - min_y+1)/2;
  xc = min_x+r;
  yc = (min_y+max_y)/2;
  MetaArc(xc,yc,xc,yc+r,-180);
  xc = max_x-r;
  MetaArc(xc,yc,xc,yc+r,180);
  MetaLine(min_x+r,min_y,max_x-r,min_y);
  MetaLine(min_x+r,max_y,max_x-r,max_y);
}

void MetaFillEllipse(int xc, int yc, int rx, int ry)
{
   int ox,oy,rrx,rry,x,y,LastX,LastY;
   double b,r;
   int Left,Top,Right,Bottom;
   struct viewporttype TmpViewPort;

  if (!PrintingSign)
   {
     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
   }

  ox = UserXToWindowX(xc);
  oy = UserYToWindowY(yc);
  rrx = UserXLToWindowXL(rx);
  rry = UserYLToWindowYL(ry);

  b=(double)rry*(double)rry;     r=(double)rrx/(double)rry;
  LastX=0; LastY=rry;
  for (y = rry; y>=0; y--) {
      if( (x=0.5+(double)r*sqrt(b-(double)y*y)) == LastX ) continue;
      fillrectangle(ox-LastX,oy-LastY,ox+LastX,oy-(y+1));
      fillrectangle(ox-LastX,oy+LastY,ox+LastX,oy+(y+1));
      LastX=x; LastY=y;
  }
  fillrectangle(ox-LastX,oy-LastY,ox+LastX,oy-(y+1));
  fillrectangle(ox-LastX,oy+LastY,ox+LastX,oy+(y+1));

  if (!PrintingSign)
   {
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
   }
}

void MetaFillCircle(int xc, int yc, int r)
{
  MetaFillEllipse(xc,yc,r,r);
}

void MetaFillRoundRectangle(int x1, int y1, int x2, int y2)
{
  int min_x,min_y,max_x,max_y;
  int xc,yc;
  int minworh;
  min_x=min(x1,x2);
  max_x=max(x1,x2);
  min_y=min(y1,y2);
  max_y=max(y1,y2);
  minworh=(min(max_x-min_x,max_y-min_y)+5)/10;
  MetaFillRectangle(min_x,min_y+minworh,max_x,max_y-minworh);
  MetaFillRectangle(min_x+minworh,min_y,max_x-minworh,min_y+minworh);
  MetaFillRectangle(min_x+minworh,max_y-minworh,max_x-minworh,max_y);
  xc=min_x+minworh;
  yc=min_y+minworh;
  MetaFillCircle(xc,yc,minworh);
  xc=max_x-minworh;
  yc=min_y+minworh;
  MetaFillCircle(xc,yc,minworh);
  xc=min_x+minworh;
  yc=max_y-minworh;
  MetaFillCircle(xc,yc,minworh);
  xc=max_x-minworh;
  yc=max_y-minworh;
  MetaFillCircle(xc,yc,minworh);
}

void MetaFillCircleRectangle(int x1, int y1, int x2, int y2)
{
  int min_x,min_y,max_x,max_y;
  int xc,yc,r;
  min_x=min(x1,x2);
  max_x=max(x1,x2);
  min_y=min(y1,y2);
  max_y=max(y1,y2);

  r = (max_y - min_y+1)/2;

  MetaFillRectangle(min_x+r,min_y,max_x-r,max_y);
  xc = min_x+r;
  yc = (min_y+max_y)/2;
  MetaFillCircle(xc,yc,r);
  xc = max_x-r;
  MetaFillCircle(xc,yc,r);
}


/////////////////////////Bezier curve flatten///////////////////
static int IsFlat (int x0,int y0,int x1,int y1,int x2,int y2,
                   int  x3,int y3)
{
        float flatness = 1;
        float sa, ca, y, O = y3 - y0, A = x3 - x0, H = sqrt (O*O + A*A);
        if (H == 0)
                return TRUE;

        sa = O / H, ca = A / H;
        y = - sa * (x1 - x0) + ca * (y1 - y0);
        if (y > flatness || y < -flatness)
                return FALSE;
        y =  - sa * (x2 - x0) + ca * (y2 - y0);
        return y < flatness && y > -flatness;
}

static int Bezier ( int x0,int  y0,int  x1,int y1,int  x2,int  y2,
         int  x3, int y3)
{
        if (IsFlat (x0, y0, x1, y1, x2, y2, x3, y3)) {
                MetaLine(x0,y0,x3,y3);
//              InsertPoint(x3,y3);
                return 1;
        }
        return Bezier (x0,                                      y0,
                (x0 + x1) / 2,                          (y0 + y1) / 2,
                (x0 + x2) / 4 + x1 / 2,                 (y0 + y2) / 4 + y1 / 2,
                (x0 + x3) / 8 + 3 * (x1 + x2) / 8,      (y0 + y3) / 8 + 3 * (y1 + y2) / 8)
        &&
        Bezier ((x0 + x3) / 8 + 3 * (x1 + x2) / 8,      (y0 + y3) / 8 + 3 * (y1 + y2) / 8,
                (x1 + x3) / 4 + x2 / 2,                 (y1 + y3) / 4 + y2 / 2,
                (x2 + x3) / 2,                          (y2 + y3) / 2,
                x3,                                     y3);
}

/*-----------
static void InsertPoint(int x, int y)
{
  metaPoint[metaPointNum].x = x;
  metaPoint[metaPointNum++].y = y;
}
--------------*/

void MetaCurve(int x0,int y0 ,int x1, int y1, int x2, int y2, int x3, int y3)
{
//  metaPointNum = 0;
//InsertPoint(x0,y0);
  Bezier(x0,y0,x1,y1,x2,y2,x3,y3);
//MetaLines(&metaPoint,metaPointNum);
}

//////////////////////Char and Font operation////////////////////
void MetaSetEEFont(int fnt) { metaEEFont = fnt; }
void MetaSetCCFont(int fnt) { metaCCFont = fnt; }
void MetaSetCharSize(int size) { metaCharW = metaCharH = size; }
void MetaSetCharW(int w) { metaCharW = w; }
void MetaSetCharH(int h) { metaCharH = h; }
void MetaSetCharSlant(int ang) { metaCharSlant = ang; }
void MetaSetCharRotate(int ang) {metaCharRotate = ang; }
void MetaSetCharHollow( int v) { metaCharHollow = v; }
void MetaSetChar3D( int v) { metaChar3D = v; }
void MetaSetCharColor(int color) { metaCharColor = color; }

void MetaShowChar(unsigned short code, int x, int y)
{
   int fnt;
   int ww,hh;
   unsigned short chflag;
   unsigned int pttn;
   int old_dir;
   int old_bdr;

   int Left,Top,Right,Bottom;
   struct viewporttype TmpViewPort;

  pttn = 0;
  old_dir = GetDimDir();
  old_bdr = GetDimBorder();
  if (!PrintingSign)
   {
     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
   }

   if (code<256) fnt = metaEEFont;
   else fnt = metaCCFont;
   ww = UserXLToWindowXL(metaCharW);
   hh = UserYLToWindowYL(metaCharH);

   chflag = 0;
   if (metaCharHollow) chflag |= HOLLOWBIT;
   if (metaChar3D) {
            chflag |= DIM3BIT;
            SetDimDir(metaCharDim3DDir);
            SetDimBorder(UserXLToWindowXL(metaCharDim3DBorder));
            pttn = 12;
   }
   if (metaCharSlant) chflag  |= RITALICBIT;
   if (metaCharRotate) chflag |= ROTATEBIT;

   BuildChar(UserXToWindowX(x),UserYToWindowY(y),
             code,fnt,ww,hh,metaCharSlant,
             metaCharRotate,metaCharColor,0,chflag,pttn) ;

  SetDimDir(old_dir);
  SetDimBorder(old_bdr);

  if (!PrintingSign)
   {
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                 TmpViewPort.bottom,TmpViewPort.clip);
     MouseShow();
   }

}

#define LACEWW  100
void MetaShowLaceChar(unsigned short code, int x, int y)
{
   int fnt;
   int ww,hh;
   unsigned short chflag;

   fnt = LACEFONT;
   ww = UserXLToWindowXL(LACEWW);
   hh = UserYLToWindowYL(LACEWW);
   chflag = 0;

   BuildChar(UserXToWindowX(x),UserYToWindowY(y),
             code,fnt,ww,hh,0,0,metaCharColor,LACELIB,0,0) ;
}


void MetaLace(int x1,int y1,int x2,int y2,int lacen)
{
  int x,y,xn,yn,i,w;
  int lr_flag =0;
  int ud_flag = 0;

  w = LACEWW;

  if (x1>x2) {x=x1;x1=x2;x2=x;lr_flag = 1;}
  if (y1>y2) {y=y1;y1=y2;y2=y;ud_flag = 1;}
  xn=(x2-x1+1+w/2)/w+1;
  yn=(y2-y1+1+w/2)/w+1;
  x2=x1-w/2+(xn-1)*w;
  y2=y1+w/2+(yn-1)*w;
  x=x1-w/2;
  y=y1+w/2;
  if (xn>2 && yn>2)
  {
    MetaShowLaceChar(lacen*8+7,x,y);
    MetaShowLaceChar(lacen*8+4,x2,y);
    MetaShowLaceChar(lacen*8+6,x,y2);
    MetaShowLaceChar(lacen*8+5,x2,y2);


    x+=w;
    y+=w;
    xn-=2;
    yn-=2;
    for (i=0;i<xn;i++)
    {
      MetaShowLaceChar(lacen*8,x,y1+w/2);
      MetaShowLaceChar(lacen*8+2,x,y2);
      x += w;
    }
    for (i=0;i<yn;i++)
    {
      MetaShowLaceChar(lacen*8+3,x1-w/2,y);
      MetaShowLaceChar(lacen*8+1,x2,y);
      y += w;
    }
  }
  else
  {
    if (xn >= yn)
    {
      x=x1;
      for (i=0;i<xn-1;i++)
      {
        if (yn>1)    /* yn == 2 */
            {
              MetaShowLaceChar(lacen*8,x,y);
              MetaShowLaceChar(lacen*8+2,x,y2);
            }
        else
            MetaShowLaceChar(lacen*8+2*ud_flag,x,y);
        x+=w;
      }
    }
    else
    {
      y=y1+w;
      for (i=0;i<yn-1;i++)
      {
        if (xn>1) /* xn == 2 */
          {
           MetaShowLaceChar(lacen*8+3,x,y);
           MetaShowLaceChar(lacen*8+1,x2,y);
          }
        else
          MetaShowLaceChar(lacen*8+3-2*lr_flag,x,y);
        y+=w;
      }
    }
  }
}

void RunMetaFile()
{
 if (GlobalMetaFile<0) return;
 switch(metaColorType) {
   case USEINDEX16:
        MetaColor(metaIndexcolor);
        break;
   case USERGB:
        MetaSetRGBColor(metaRed,metaGreen,metaBlue);
        break;
   case USECMYK:
        MetaSetCMYKColor(metaCyan,metaMagenta,metaYellow,metaBlack);
        break;
 }
 metaLineWidth = 14;
 MetaFileProc(GlobalMetaFile);
}

