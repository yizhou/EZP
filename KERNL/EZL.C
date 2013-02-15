#include "ezphead.h"

#define SYNTAX_ERROR  -100
#define LINEBUFFERLENGTH 1024

int mm2mil(double n)
{
  int mil=(int)(n*1000/25.4+0.5);
  return mil;
}

int getPointN(char *str,POINT point[],int *pN)
{
  float x,y;
  char *p=str;
  int  n=0;

  for(;;)
  {
     sscanf(p,"%f",&x);
     point[n].x=mm2mil((double)x);

     p=strchr(p,',');
     if(p==NULL)
        return SYNTAX_ERROR;
     p++;

     sscanf(p,"%f",&y);
     point[n].y=mm2mil((double)y);
     n++;

     p=strchr(p,',');
     if(p==NULL)
        break;
     p++;
  }

  *pN=n;
  return OpOK;
}

int getrealn(char *str,double val[],int n)
{
  int i;
  char *p=str;
  float v;

  for(i=0;i<n;i++)
  {
     sscanf(p,"%f",&v);
     val[i]=(double)v;

     p=strchr(p,',');
     if(p==NULL)
        break;
     p++;
  }
  if(i==n-1) return OpOK;
  else return SYNTAX_ERROR;
}

//  line x1,y1,x2,y2
int ParseLINE(char *str)
{
  char *p;
  int res;
  double val[4];
  int x1,y1,x2,y2;

  p = str;
  res = getrealn(p,val,4);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  MetaLine(x1,y1,x2,y2);
  return OpOK;
}

/// Triangle x1,y1,x2,y2,x3,y3
int ParseTRIANGLE(char *str)
{
  char *p;
  int res;
  double val[6];
  int x1,y1,x2,y2,x3,y3;

  p = str;
  res = getrealn(p,val,6);
  if (res != OpOK) return SYNTAX_ERROR;

  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  x3 = mm2mil(val[4]);
  y3 = mm2mil(val[5]);
  MetaTriangle(x1,y1,x2,y2,x3,y3);
  return OpOK;
}

/// FillTriangle x1,y1,x2,y2,x3,y3
int ParseFILLTRIANGLE(char *str)
{
  char *p;
  int res;
  double val[6];
  int x1,y1,x2,y2,x3,y3;

  p = str;
  res = getrealn(p,val,6);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  x3 = mm2mil(val[4]);
  y3 = mm2mil(val[5]);
  MetaFillTriangle(x1,y1,x2,y2,x3,y3);
  return OpOK;
}

// Rectangle x1,y1,x2,y2
int ParseRECTANGLE(char *str)
{
  char *p;
  int res;
  double val[4];
  int x1,y1,x2,y2;

  p = str;
  res = getrealn(p,val,4);
  if (res != OpOK) return SYNTAX_ERROR;

  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  MetaRectangle(x1,y1,x2,y2);
  return OpOK;
}

/// FillRectangle x1,y1,x2,y2
int ParseFILLRECTANGLE(char *str)
{
  char *p;
  int res;
  double val[4];
  int x1,y1,x2,y2;

  p = str;
  res = getrealn(p,val,4);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  MetaFillRectangle(x1,y1,x2,y2);
  return OpOK;
}

/// Square x,y,len
int ParseSQUARE(char *str)
{
  char *p;
  int res;
  double val[3];
  int x1,y1,len;

  p = str;
  res = getrealn(p,val,3);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  len = mm2mil(val[2]);
  MetaRectangle(x1,y1,x1+len-1,y1+len-1);
  return OpOK;
}

/// FillSquare x,y,len
int ParseFILLSQUARE(char *str)
{
  char *p;
  int res;
  double val[3];
  int x1,y1,len;

  p = str;
  res = getrealn(p,val,3);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  len = mm2mil(val[2]);
  MetaFillRectangle(x1,y1,x1+len-1,y1+len-1);
  return OpOK;
}

/// RoundRectangle x1,y1,x2,y2
int ParseROUNDRECTANGLE(char *str)
{
  char *p;
  int res;
  double val[4];
  int x1,y1,x2,y2;

  p = str;
  res = getrealn(p,val,4);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  MetaRoundRectangle(x1,y1,x2,y2);
  return OpOK;
}

/// FillRoundRectangle x1,y1,x2,y2
int ParseFILLROUNDRECTANGLE(char *str)
{
  char *p;
  int res;
  double val[4];
  int x1,y1,x2,y2;

  p = str;
  res = getrealn(p,val,4);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  MetaFillRoundRectangle(x1,y1,x2,y2);
  return OpOK;
}


/// CircleRectangle x1,y1,x2,y2
int ParseCIRCLERECTANGLE(char *str)
{
  char *p;
  int res;
  double val[4];
  int x1,y1,x2,y2;

  p = str;
  res = getrealn(p,val,4);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  MetaCircleRectangle(x1,y1,x2,y2);
  return OpOK;
}

/// FillCircleRectangle x1,y1,x2,y2
int ParseFILLCIRCLERECTANGLE(char *str)
{
  char *p;
  int res;
  double val[4];
  int x1,y1,x2,y2;

  p = str;
  res = getrealn(p,val,4);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  MetaFillCircleRectangle(x1,y1,x2,y2);
  return OpOK;
}

/// Circle xc,yc,r
int ParseCIRCLE(char *str)
{
  char *p;
  int res;
  double val[3];
  int x1,y1,r;

  p = str;
  res = getrealn(p,val,3);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  r  = mm2mil(val[2]);
  MetaEllipse(x1,y1,r,r);
  return OpOK;
}

/// FillCircle xc,yc,r
int ParseFILLCIRCLE(char *str)
{
  char *p;
  int res;
  double val[3];
  int x1,y1,r;

  p = str;
  res = getrealn(p,val,3);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  r  = mm2mil(val[2]);
  MetaFillEllipse(x1,y1,r,r);
  return OpOK;
}


/// Ellipse xc,yc,rx,ry
int ParseELLIPSE(char *str)
{
  char *p;
  int res;
  double val[4];
  int x1,y1,rx,ry;

  p = str;
  res = getrealn(p,val,4);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  rx = mm2mil(val[2]);
  ry = mm2mil(val[3]);
  MetaEllipse(x1,y1,rx,ry);
  return OpOK;
}

/// FillEllipse xc,yc,rx,ry
int ParseFILLELLIPSE(char *str)
{
  char *p;
  int res;
  double val[4];
  int x1,y1,rx,ry;

  p = str;
  res = getrealn(p,val,4);
  if (res != OpOK) return SYNTAX_ERROR;
  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  rx = mm2mil(val[2]);
  ry = mm2mil(val[3]);
  MetaFillEllipse(x1,y1,rx,ry);
  return OpOK;
}

/// Curve x1,y1,x2,y2,x3,y3,x4,y4
int ParseCURVE(char *str)
{
  char *p;
  int res;
  double val[8];
  int x1,y1,x2,y2,x3,y3,x4,y4;

  p = str;
  res = getrealn(p,val,8);
  if (res != OpOK) return SYNTAX_ERROR;

  x1 = mm2mil(val[0]);
  y1 = mm2mil(val[1]);
  x2 = mm2mil(val[2]);
  y2 = mm2mil(val[3]);
  x3 = mm2mil(val[4]);
  y3 = mm2mil(val[5]);
  x4 = mm2mil(val[6]);
  y4 = mm2mil(val[7]);
  MetaCurve(x1,y1,x2,y2,x3,y3,x4,y4);
  return OpOK;
}

/// Polygon x1,y1,x2,y2,x3,y3,...
int ParsePOLYGON(char *str)
{
  int res;
  int dotn;
  POINT point[MAXPOLYGONNUMBER];

  res = getPointN(str,point,&dotn);
  if (res != OpOK) return SYNTAX_ERROR;

  MetaPolygon(&point, dotn);
  return OpOK;
}

/// FillPolygon x1,y1,x2,y2,x3,y3,...
int ParseFILLPOLYGON(char *str)
{
  int res;
  int dotn;
  POINT point[MAXPOLYGONNUMBER];

  res = getPointN(str,point,&dotn);
  if (res != OpOK) return SYNTAX_ERROR;

  MetaFillPolygon(&point, dotn);
  return OpOK;
}

/// Arc xc,yc,xs,ys,angle
int ParseARC(char *str)
{
  char *p;
  int res;
  double val[5];
  int xc,yc,xs,ys;

  p = str;
  res = getrealn(p,val,5);
  if (res != OpOK) return SYNTAX_ERROR;
  xc = mm2mil(val[0]);
  yc = mm2mil(val[1]);
  xs = mm2mil(val[2]);
  ys = mm2mil(val[3]);
  MetaArc(xc,yc,xs,ys,val[4]);
  return OpOK;
}

/// LineWidth width
int ParseLINEWIDTH(char *str)
{
  char *p;
  int res;
  double val[1];
  int width;

  p = str;
  res = getrealn(p,val,1);
  if (res != OpOK) return SYNTAX_ERROR;
  width = mm2mil(val[0]);
  MetaSetLineWidth(width);
  return OpOK;
}

/// LineType type
int ParseLINETYPE(char *str)
{
  char *p;
  int res;
  double val[1];
  int type;

  p = str;
  res = getrealn(p,val,1);
  if (res != OpOK) return SYNTAX_ERROR;
  type = (int)val[0];
  if(type<0||type>4) return SYNTAX_ERROR;
  MetaSetLineType(type);
  return OpOK;
}

/// LineHead head
int ParseLINEHEAD(char *str)
{
  char *p;
  int res;
  double val[1];
  int head;

  p = str;
  res = getrealn(p,val,1);
  if (res != OpOK) return SYNTAX_ERROR;
  head = (int)val[0];
  if(head<0||head>5) return SYNTAX_ERROR;
  MetaSetLineHead(head);
  return OpOK;
}

/// LineEnd end
int ParseLINEEND(char *str)
{
  char *p;
  int res;
  double val[1];
  int end;

  p = str;
  res = getrealn(p,val,1);
  if (res != OpOK) return SYNTAX_ERROR;
  end = (int)val[0];
  if(end<0||end>5) return SYNTAX_ERROR;
  MetaSetLineEnd(end);
  return OpOK;
}

// color color_num(1..16)   or  color <COLOR_STRING>
int ParseCOLOR(char *str)
{
  int res;
  double val[1];
  int color;
  static char *ColorString[]={
        "黑",        "深蓝",      "深绿",      "深青",
        "深红",      "紫",        "橙",        "浅灰",
        "深灰",      "蓝",      "绿",      "青",
        "红",        "品红",      "黄",      "白"
  };

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;
  color=(int)val[0];
  if(color<=0 || color>16)    /*- it must be string, not int -*/
  {
      char cc[10];
      sscanf(str,"%s",cc);
      for(res=0;res<16;res++)
          if(stricmp(cc,ColorString[res])==0)
             break;
      if(res>=16) return SYNTAX_ERROR;
      color=res;
  }
  else
    color--;

  MetaColor(color);
  return OpOK;
}

// ColorRGB r,g,b
int ParseCOLORRGB(char *str)
{
  int res;
  double val[3];
  int r,g,b;

  res = getrealn(str,val,3);
  if (res != OpOK) return SYNTAX_ERROR;

  r=(int)val[0];
  g=(int)val[1];
  b=(int)val[2];
  if(r<0 || r>255 || g<0 || g>255 || b<0 || b>255)
     return SYNTAX_ERROR;

  MetaSetRGBColor(r,g,b);
  return OpOK;
}

// ColorCMYK c,m,y,k
int ParseCOLORCMYK(char *str)
{
  int res;
  double val[4];
  int c,m,y,k;

  res = getrealn(str,val,4);
  if (res != OpOK) return SYNTAX_ERROR;

  c=(int)val[0];
  m=(int)val[1];
  y=(int)val[2];
  k=(int)val[3];
  if(c<0 || c>255 || m<0 || m>255 || y<0 || y>255 || k<0 || k>255)
     return SYNTAX_ERROR;

  MetaSetCMYKColor(c,m,y,k);
  return OpOK;
}

// SetEEFont efont
int ParseSETEEFONT(char *str)
{
  int res;
  double val[1];
  int font;

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;

  font=(int)val[0];
  MetaSetEEFont(font);
  return OpOK;
}

// SetCCFont cfont
int ParseSETCCFONT(char *str)
{
  int res;
  double val[1];
  int font;

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;

  font=(int)val[0];
  MetaSetCCFont(font);
  return OpOK;
}


// FontSize size
int ParseFONTSIZE(char *str)
{
  int res;
  double val[1];
  int Size;

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;

  Size=(int)val[0];
  MetaSetCharSize(mm2mil(Size));
  return OpOK;
}

// FontHeight hh
int ParseFONTHEIGHT(char *str)
{
  int res;
  double val[1];
  int hh;

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;

  hh = mm2mil(val[0]);
  MetaSetCharH(hh);
  return OpOK;
}

// FontWidth ww
int ParseFONTWIDTH(char *str)
{
  int res;
  double val[1];
  int ww;

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;

  ww = mm2mil(val[0]);
  MetaSetCharW(ww);
  return OpOK;
}

// CharRotate angle
int ParseCHARROTATE(char *str)
{
  int res;
  double val[1];
  int angle;

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;

  angle=(int)val[0];
  MetaSetCharRotate(angle);
  return OpOK;
}

// CharSlant angle
int ParseCHARSLANT(char *str)
{
  int res;
  double val[1];
  int angle;

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;

  angle=(int)val[0];
  MetaSetCharSlant(angle);
  return OpOK;
}

// CharColor color
int ParseCHARCOLOR(char *str)
{
  int res;
  double val[1];
  int color;
  static char *ColorString[]={
        "黑",        "深蓝",      "深绿",      "深青",
        "深红",      "紫",        "橙",        "浅灰",
        "深灰",      "蓝",      "绿",      "青",
        "红",        "品红",      "黄",      "白"
  };

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;
  color=(int)val[0];
  if(color<=0 || color>16)    /*- it must be string, not int -*/
  {
      char cc[10];
      sscanf(str,"%s",cc);
      for(res=0;res<16;res++)
          if(stricmp(cc,ColorString[res])==0)
             break;
      if(res>=16) return SYNTAX_ERROR;
      color=res;
  }
  else
    color--;

  MetaSetCharColor(color);
  return OpOK;
}

// CharHollow <0,1>
int ParseCHARHOLLOW(char *str)
{
  int res;
  double val[1];
  int flag;

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;

  flag=(int)val[0];
  if(flag!=0 && flag!=1)
      return SYNTAX_ERROR;

  MetaSetCharHollow(flag);
  return OpOK;
}

// Char3D <0,1>
int ParseCHAR3D(char *str)
{
  int res;
  double val[1];
  int flag;

  res = getrealn(str,val,1);
  if (res != OpOK) return SYNTAX_ERROR;

  flag=(int)val[0];
  if(flag!=0 && flag!=1)
      return SYNTAX_ERROR;

  MetaSetChar3D(flag);
  return OpOK;
}

// ShowChar x,y,<char>
int ParseSHOWCHAR(char *str)
{
  int x,y;
  float fx,fy;
  unsigned int code=0;
  unsigned char code_str[10];

  sscanf(str,"%f,%f,0x%x",&fx,&fy,&code);
  if (code==0)
  {
     int len;
     sscanf(str,"%f,%f,%s",&fx,&fy,code_str);
     len=strlen(code_str);
     if(len!=1 && len!=2)
        return SYNTAX_ERROR;
     if(len==2 && code_str[0]>(unsigned char) 0x80)
        code=(unsigned int)code_str[0]*256 + code_str[1];
     else
        code=code_str[0];
  }

  x = mm2mil(fx);
  y = mm2mil(fy);
  MetaShowChar(code,x,y);
  return OpOK;
}


enum {
   UNKNOWN_COMMAND=-1,
   CMD_LINE=1,
   CMD_TRIANGLE,
   CMD_FILLTRIANGLE,
   CMD_RECTANGLE,
   CMD_FILLRECTANGLE,
   CMD_SQUARE,
   CMD_FILLSQUARE,
   CMD_ROUNDRECTANGLE,
   CMD_FILLROUNDRECTANGLE,
   CMD_CIRCLERECTANGLE,
   CMD_FILLCIRCLERECTANGLE,
   CMD_CIRCLE,
   CMD_FILLCIRCLE,
   CMD_ELLIPSE,
   CMD_FILLELLIPSE,
   CMD_CURVE,
   CMD_POLYGON,
   CMD_FILLPOLYGON,
   CMD_ARC,
   CMD_LINEWIDTH,
   CMD_LINETYPE,
   CMD_LINEHEAD,
   CMD_LINEEND,
   CMD_COLOR,
   CMD_COLORRGB,
   CMD_COLORCMYK,
   CMD_SETEEFONT,
   CMD_SETCCFONT,
   CMD_SETTTFONT,
   CMD_FONTSIZE,
   CMD_FONTHEIGHT,
   CMD_FONTWIDTH,
   CMD_CHARROTATE,
   CMD_CHARSLANT,
   CMD_CHARCOLOR,
   CMD_CHARHOLLOW,
   CMD_CHAR3D,
   CMD_SHOWCHAR,
};
static struct Token_S {
  unsigned char *szToken[3];
  short  szLen[3];
  short  token;
}
 TokenArray[]=
{
   {
     { "line", "zx", "线" },
     {  4,      2,    2   },
     CMD_LINE
   },
   {
     { "triangle", "sjx", "三角" },
     {  8,           3,    4   },
     CMD_TRIANGLE
   },
   {
     { "FillTriangle", "sxsj", "实心三角" },
     {  12,              4,      8   },
     CMD_FILLTRIANGLE
   },
   {
     { "rectangle", "jx", "矩形" },
     {  9,           2,     4   },
     CMD_RECTANGLE
   },
   {
     { "FillRectangle", "sxjx", "实心矩形" },
     {  13,               4,      8  },
     CMD_FILLRECTANGLE
   },
   {
     { "square", "zfx", "正方形" },
     {  6,        3,     6   },
     CMD_SQUARE
   },
   {
     { "FillSquare", "sxzfx", "实心正方形" },
     {  10,           5,        10   },
     CMD_FILLSQUARE
   },
   {
     { "RoundRectangle", "yjjx", "圆角矩形" },
     {  14,                4,      8   },
     CMD_ROUNDRECTANGLE
   },
   {
     { "FillRoundRectangle", "sxyjjx", "实心圆角矩形" },
     {  18,                     6,      12  },
     CMD_FILLROUNDRECTANGLE
   },
   {
     { "CircleRectangle", "ybjx", "圆边矩形" },
     {  15,                 4,     8   },
     CMD_CIRCLERECTANGLE
   },
   {
     { "FillCircleRectangle", "sxybjx", "实心圆边矩形" },
     {  19,                     6,        12   },
     CMD_FILLCIRCLERECTANGLE
   },
   {
     { "circle", "yuan", "圆" },
     {  6,         4,     2   },
     CMD_CIRCLE
   },
   {
     { "FillCircle", "sxy", "实心圆" },
     {  10,            3,     6   },
     CMD_FILLCIRCLE
   },
   {
     { "ellipse", "ty", "椭圆" },
     {  7,         2,     4   },
     CMD_ELLIPSE
   },
   {
     { "FillEllipse", "sxty", "实心椭圆" },
     {  11,             4,      8   },
     CMD_FILLELLIPSE
   },
   {
     { "curve", "qx", "曲线" },
     {  5,       2,     4   },
     CMD_CURVE
   },
   {
     { "polygon", "dbx", "多边形" },
     {  7,          3,     6   },
     CMD_POLYGON
   },
   {
     { "FillPolygon", "sxdbx", "实心多边形" },
     {  11,             5,       10   },
     CMD_FILLPOLYGON
   },
   {
     { "arc", "hu", "弧" },
     {  3,      2,    2   },
     CMD_ARC
   },

   {
     { "LineWidth", "xk", "线宽" },
     {  9,           2,    4   },
     CMD_LINEWIDTH
   },
   {
     { "LineType", "xx", "线型" },
     {  8,           2,    4   },
     CMD_LINETYPE
   },
   {
     { "LineHead", "xt", "线头" },
     {  8,           2,    4   },
     CMD_LINEHEAD
   },
   {
     { "LineEnd", "xw", "线尾" },
     {  7,           2,    4   },
     CMD_LINEEND
   },

   {
     { "color", "ys", "颜色" },
     {  5,        2,    4   },
     CMD_COLOR
   },
   {
     { "ColorRGB", "ysRGB", "颜色RGB" },
     {  8,           5,       7   },
     CMD_COLORRGB
   },
   {
     { "ColorCMYK", "ysCMYK", "颜色CMYK" },
     {  9,            6,        8   },
     CMD_COLORCMYK
   },

   {
     { "SetEEFont", "ywzt", "英文字体" },
     {  9,            4,        8   },
     CMD_SETEEFONT
   },
   {
     { "SetCCFont", "zwzt", "中文字体" },
     {  9,            4,        8   },
     CMD_SETCCFONT
   },
   {
     { "FontSize", "zh", "字号" },
     {  8,          2,     4   },
     CMD_FONTSIZE
   },
   {
     { "FontHeight", "zg", "字高" },
     {  10,           2,     4   },
     CMD_FONTHEIGHT
   },
   {
     { "FontWidth", "zk", "字宽" },
     {  9,           2,     4   },
     CMD_FONTWIDTH
   },

   {
     { "CharRotate", "zxz", "字旋转" },
     {  10,           3,     6   },
     CMD_CHARROTATE
   },
   {
     { "CharSlant", "zqx", "字倾斜" },
     {  9,           3,     6   },
     CMD_CHARSLANT,
   },
   {
     { "CharColor", "zys", "字颜色" },
     {  9,           3,     6   },
     CMD_CHARCOLOR
   },
   {
     { "CharHollow", "zkx", "字空心" },
     {  10,           3,     6   },
     CMD_CHARHOLLOW
   },
   {
     { "Char3D", "zlt", "字立体" },
     {  6,        3,     6   },
     CMD_CHAR3D
   },
   {
     { "ShowChar", "dz", "字" },
     {  8,          2,    2   },
     CMD_SHOWCHAR
   },
};
#define TOKEN_ARRAY_NUM    (sizeof(TokenArray)/sizeof(TokenArray[0]))

int CheckCommand(char cmd[])
{
  int i,k;
  int len=strlen(cmd);
  struct Token_S *pTokenArray;

  for(i=0;i<TOKEN_ARRAY_NUM;i++)
  {
      pTokenArray=&TokenArray[i];
      for(k=0;k<3;k++)
        if( len==pTokenArray->szLen[k]
         && !memicmp(pTokenArray->szToken[k], cmd, len) )
            return pTokenArray->token;
  }

  return UNKNOWN_COMMAND;
}

int ParseString(char *str)
{
  char *p;
  unsigned char cmd[40],ch;
  int i,token;

  p = str;
  // while ((ch=*p)!=0 && (ch<=0x20 || ch==0xa1a1) )  p++;
  while ((ch=*p)!=0 && ch<=0x20)  p++;
  if (!(*p)) return OpOK;

  i = 0;
  // while (i<32 && (ch=*p)!=0 && ch!=0x20 && ch!=0xa1a1 && ch!='\t')
  while (i<32 && (ch=*p)!=0 && ch!=0x20 && ch!='\t')
      cmd[i++] = *p++;
  cmd[i]=0;     /*-- i must great than 0 ------*/

  token = CheckCommand(cmd);
  switch(token)
  {
      case CMD_LINE:
           return ParseLINE(p);
      case CMD_TRIANGLE:
           return ParseTRIANGLE(p);
      case CMD_FILLTRIANGLE:
           return ParseFILLTRIANGLE(p);
      case CMD_RECTANGLE:
           return ParseRECTANGLE(p);
      case CMD_FILLRECTANGLE:
           return ParseFILLRECTANGLE(p);
      case CMD_SQUARE:
           return ParseSQUARE(p);
      case CMD_FILLSQUARE:
           return ParseFILLSQUARE(p);
      case CMD_ROUNDRECTANGLE:
           return ParseROUNDRECTANGLE(p);
      case CMD_FILLROUNDRECTANGLE:
           return ParseFILLROUNDRECTANGLE(p);
      case CMD_CIRCLERECTANGLE:
           return ParseCIRCLERECTANGLE(p);
      case CMD_FILLCIRCLERECTANGLE:
           return ParseFILLCIRCLERECTANGLE(p);
      case CMD_CIRCLE:
           return ParseCIRCLE(p);
      case CMD_FILLCIRCLE:
           return ParseFILLCIRCLE(p);
      case CMD_ELLIPSE:
           return ParseELLIPSE(p);
      case CMD_FILLELLIPSE:
           return ParseFILLELLIPSE(p);
      case CMD_CURVE:
           return ParseCURVE(p);
      case CMD_POLYGON:
           return ParsePOLYGON(p);
      case CMD_FILLPOLYGON:
           return ParseFILLPOLYGON(p);
      case CMD_ARC:
           return ParseARC(p);

      case CMD_LINEWIDTH:
           return ParseLINEWIDTH(p);
      case CMD_LINETYPE:
           return ParseLINETYPE(p);
      case CMD_LINEHEAD:
           return ParseLINEHEAD(p);
      case CMD_LINEEND:
           return ParseLINEEND(p);

      case CMD_COLOR:
           return ParseCOLOR(p);
      case CMD_COLORRGB:
           return ParseCOLORRGB(p);
      case CMD_COLORCMYK:
           return ParseCOLORCMYK(p);
      case CMD_SETEEFONT:
           return ParseSETEEFONT(p);
      case CMD_SETCCFONT:
           return ParseSETCCFONT(p);
      case CMD_FONTSIZE:
           return ParseFONTSIZE(p);
      case CMD_FONTHEIGHT:
           return ParseFONTHEIGHT(p);
      case CMD_FONTWIDTH:
           return ParseFONTWIDTH(p);
      case CMD_CHARROTATE:
           return ParseCHARROTATE(p);
      case CMD_CHARSLANT:
           return ParseCHARSLANT(p);
      case CMD_CHARCOLOR:
           return ParseCHARCOLOR(p);
      case CMD_CHARHOLLOW:
           return ParseCHARHOLLOW(p);
      case CMD_CHAR3D:
           return ParseCHAR3D(p);
      case CMD_SHOWCHAR:
           return ParseSHOWCHAR(p);
      default :
           return UNKNOWN_COMMAND;
  }
}

/*--------------------- meta file manage ----------------------*/
#define my_MAX_FILE_LEN (32*1024L)

#define MAX_META_FILE_NUM       5
typedef struct tagMetaFileStruct
{
   int  pBuf;
   int  filelen;         /*- it's flag also: >0 means it's be used -*/
   unsigned char *filebuf;
} MetaFile_s;
static MetaFile_s MetaFileArray[MAX_META_FILE_NUM];

static HFILENO AllocMetaFileHandle()
{
   HFILENO i;

   for(i=0;i<MAX_META_FILE_NUM;i++)
      if(MetaFileArray[i].filelen==0)
         return i;

   return (HFILENO)-1;
}
static void FreeMetaFileHandle(HFILENO hMetaFile)
{
      MetaFileArray[hMetaFile].filelen=0;
}

void CloseMetaFile(HFILENO hMetaFile)
{
   if(hMetaFile>=MAX_META_FILE_NUM || hMetaFile<0)
      return;

   if(MetaFileArray[hMetaFile].filebuf!=NULL)
   {
      free(MetaFileArray[hMetaFile].filebuf);
      MetaFileArray[hMetaFile].filebuf=NULL;
   }

   FreeMetaFileHandle(hMetaFile);
}
void CloseAllMetaFile()
{
   HFILENO i;

   for(i=0;i<MAX_META_FILE_NUM;i++)
      if(MetaFileArray[i].filelen)
          CloseMetaFile(i);
}

HFILENO InitMetaFile(char *filename)
{
   int hFile;
   HFILENO hMetaFile;
   long filelen;

   hMetaFile=AllocMetaFileHandle();
   if(hMetaFile<0)
       return hMetaFile;

   hFile=open(filename,O_RDONLY|O_BINARY);
   if(hFile==-1)
   {
    lbl_err_exit:
       FreeMetaFileHandle(hMetaFile);
       return (HFILENO)-1;
   }

   lseek(hFile,0,SEEK_END);
   filelen=tell(hFile);
   if(filelen>my_MAX_FILE_LEN)
   {
     lbl_err_file_exit:
        close(hFile);
        goto lbl_err_exit;
   }

   MetaFileArray[hMetaFile].filebuf=malloc(filelen);
   if(MetaFileArray[hMetaFile].filebuf==NULL)
        goto lbl_err_file_exit;

   lseek(hFile,0,SEEK_SET);
   if(read(hFile,MetaFileArray[hMetaFile].filebuf,filelen)==-1)
   {
       free(MetaFileArray[hMetaFile].filebuf);
       MetaFileArray[hMetaFile].filebuf=NULL;
       goto lbl_err_file_exit;
   }

   close(hFile);

   MetaFileArray[hMetaFile].filelen=filelen;
   return hMetaFile;
}

static void ReadALine(char *lbuf, HFILENO hMetaFile)
{
    long lcount;
    unsigned char lchar;
    int  first;
    int  pBuf=MetaFileArray[hMetaFile].pBuf;
    int  filelen=MetaFileArray[hMetaFile].filelen;
    char *filebuf=MetaFileArray[hMetaFile].filebuf;

    lcount=0;
    first=1;
    while (pBuf<filelen)
    {
        lchar=filebuf[pBuf++];
        if(lchar==0x1a) break;
        if ((lchar<0x21) && (lcount==0))
        {
            first=1;
            continue;  /* Skip control char*/
        }
        if (lchar=='%')         /*-- single line remark ---*/
        {
        lbl_ignore_to_eol:
            while (pBuf<filelen && (lchar=filebuf[pBuf])!='\r')
                pBuf++;
            if(pBuf>=filelen)
                break;
            first=1;
        }
        if (lchar=='\r')
        {
            if(filebuf[pBuf]=='\n') pBuf++;    /*-- ignore \n  ----*/
            break;
        }

        if(lchar>0xa0)          /*- for chinese -*/
        {
            if(!first)
            {
               Wchar code=lbuf[lcount-1]<<8;
               code |= lchar;
               first=1;
               if(code=='，') { lbuf[lcount-1]=','; continue; }
               else
               if(code==0xa1a1) { lbuf[lcount-1]=0x20; continue; }
               else
               if(code>=0xa3a1 && code<=0xa3fe)
               {
                   code -= 0xa3a1-0x21;
                   if(code=='%') goto lbl_ignore_to_eol;
                   lbuf[lcount-1]=code;
                   continue;
               }
            }
            else
               first=0;
        }

        lbuf[lcount++]=lchar;
        if(lcount>=LINEBUFFERLENGTH-1)
           break;
    }

    lbuf[lcount]=0;
    MetaFileArray[hMetaFile].pBuf = pBuf;
}

void MetaFileProc(HFILENO hMetaFile)
{
   char lbuf[LINEBUFFERLENGTH];
   int  filelen=MetaFileArray[hMetaFile].filelen;

   MetaFileArray[hMetaFile].pBuf=0;
   while(MetaFileArray[hMetaFile].pBuf<filelen)
   {
       ReadALine(lbuf,hMetaFile);
       ParseString(lbuf);
   }
}
void AllMetaFileProc()
{
   HFILENO i;

   for(i=0;i<MAX_META_FILE_NUM;i++)
      if(MetaFileArray[i].filelen)
          MetaFileProc(i);
}
