/*-------------------------------------------------------------------
* Name: lines.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"


/////////////defines//////////////////////////

////////////////////////////////////////////



#define LineFillBuffer(lpdc,y,x1,x2,FillMode) CurrentLineFillLine(x1,x2,y,lpdc)
 static int MakeLineArrow(LPDC lpdc,POINT point1,POINT point2,
                  UINT LineWidth,UCHAR ArrowType);
 static int  ArcToArea(LPDC lpdc,POINT MiddlePoint,float Xlen,float Ylen,
    float StartAngle,float FinishAngle,UCHAR FillMode,
    UINT LineWidth);

void LineToPath(LPDC lpdc,POINT point1,POINT point2,UINT LineWidth)
{
  POINT midpoint1,midpoint2;
  POINT multiploy1[4];

  if (LineWidth==0) return;

  memcpy(&midpoint1,&point1,sizeof(POINT));
  memcpy(&midpoint2,&point2,sizeof(POINT));

  if (LineWidth>1)
  {
     double dx,dy,tg;

     tg=1.0;
     if (point2.x==point1.x)
     {
        dx=(LineWidth+1)/2;
        dy=0;
     }
     else
     if (point2.y==point1.y)
     {
        dx=0;
        dy=(LineWidth+1)/2;
     }
     else
     {
        tg=(double)(point2.y-point1.y)/(double)(point2.x-point1.x);
        dx=(double)LineWidth*0.5/sqrt(1.+1./(tg*tg))+0.5;
        dy=(double)LineWidth*0.5/sqrt(1.+(tg*tg))+0.5;
     }
     if (tg>0)
     {
        point1.x-=dx;
        point2.x-=dx;
        midpoint1.x+=dx;
        midpoint2.x+=dx;
        point1.y+=dy;
        point2.y+=dy;
        midpoint1.y-=dy;
        midpoint2.y-=dy;
     }
     else
     {
        point1.x+=dx;
        point2.x+=dx;
        midpoint1.x-=dx;
        midpoint2.x-=dx;
        point1.y+=dy;
        point2.y+=dy;
        midpoint1.y-=dy;
        midpoint2.y-=dy;
     }
  }

  memcpy(&multiploy1[0],&point1,sizeof(POINT));
  memcpy(&multiploy1[1],&point2,sizeof(POINT));
  memcpy(&multiploy1[2],&midpoint2,sizeof(POINT));
  memcpy(&multiploy1[3],&midpoint1,sizeof(POINT));

  FillPolygon(lpdc,&multiploy1[0],4);
}

void WithWidthLine(LPDC lpdc,int x1,int y1,int x2,int y2,
                  UINT LineWidth,UINT LineType,
                  unsigned int ArrowType,UCHAR FillMode)
{
  static LineTypes[MaxLineType]={12,8,4,1};

  POINT point1,point2;
  long TotalX,TotalY,StepX,StepY;

  point1.x=x1;
  point1.y=y1;
  point2.x=x2;
  point2.y=y2;

  if (ArrowType&0x0f)
     MakeLineArrow(lpdc,point1,point2,LineWidth,ArrowType&0x0f);
  if ((ArrowType>>4)&0x0f)
     MakeLineArrow(lpdc,point2,point1,LineWidth,(ArrowType>>4)&0x0f);

  if (x2<x1)
  {
     int tmp;

     tmp=x1;
     x1=x2;
     x2=tmp;
     tmp=y1;
     y1=y2;
     y2=tmp;

     point1.x=x1;
     point1.y=y1;
  }

  if (LineType>MaxLineType) LineType=NoLineType;

  switch(LineType)
  {
    case NoLineType:
         StepX=TotalX=x2-x1;
         StepY=TotalY=y2-y1;
         break;
    case LineType1:
    case LineType2:
    case LineType3:
    case LineType4:
         if (x2!=x1)
         {
             if (y2!=y1)
             {
                float tgx,sinx,cosx;

                tgx=(float)(y2-y1)/(float)(x2-x1);
                if (tgx>0)
                {
                   tgx*=tgx;
                   cosx=1./sqrt(1+tgx);
                   sinx=1./sqrt(1+1./tgx);
                }
                else
                {
                   tgx*=tgx;
                   cosx=1./sqrt(1+tgx);
                   sinx=-1./sqrt(1+1./tgx);
                }
                StepX=(double)LineTypes[LineType1-1]*cosx+0.5*((cosx>0)?1:-1);
                TotalX=(double)MaxLineTypeLength*cosx+0.5*((cosx>0)?1:-1);
                StepY=(double)LineTypes[LineType1-1]*sinx+0.5*((sinx>0)?1:-1);
                TotalY=(double)MaxLineTypeLength*sinx+0.5*((sinx>0)?1:-1);
             }
             else
             {
                StepX=LineTypes[LineType1-1];
                TotalX=MaxLineTypeLength;
                StepY=TotalY=0;
             }
         }
         else
         {
             StepX=TotalX=0;
             if (y2!=y1)
             {
                StepY=LineTypes[LineType1-1]*((y2>y1)?1:-1);
                TotalY=MaxLineTypeLength*((y2>y1)?1:-1);
             }
             else StepY=TotalY=0;
         }
         break;
  }

  if (x1!=x2)
  {
     for (;point1.x<x2;)
     {
         point2.x=point1.x+StepX;
         point2.y=point1.y+StepY;
         if (point2.x>x2) break;

         LineToPath(lpdc,point1,point2,LineWidth);

         point1.x+=TotalX;
         point1.y+=TotalY;
     }
  }
  else
  {
     for (point2.x=point1.x;(y1<y2)?(point1.y<y2):(point1.y>y2);)
     {
         point2.y=point1.y+StepY;
         if (((y1<y2)&&(point2.y>y2))||((y1>y2)&&(point2.y<y2))) break;

         LineToPath(lpdc,point1,point2,LineWidth);

         point1.y+=TotalY;
     }
  }
}

static int MakeLineJoint(LPDC lpdc,POINT point1,POINT point2,POINT point3,
                 UINT LineWidth)
{
  POINT midpoint1,midpoint2,midpoint3,midpoint4,midpoint5,midpoint6;
  POINT multiploy1[4];
  double dx,dy,tg1,tg2,tg3,l;
  UCHAR Sign1,Sign2,Sign3,Sign4;

  /*
    Sign1=1 ==> Point2.x=Point1.x
    Sign2=1 ==> Point3.x=Point2.x
    Sign3=1 ==> Point2.y=Point1.y
    Sign4=1 ==> Point3.y=Point2.y
  */
  Sign1=Sign2=Sign3=Sign4=0;
  tg1=-1e20;
  tg2=-1e20;

  if (LineWidth<=1) return(OpOK);

  if (point2.x==point1.x) Sign1=1;
  else tg1=(double)(point2.y-point1.y)/(double)(point2.x-point1.x);

  if (point3.x==point2.x) Sign2=1;
  else tg2=(double)(point3.y-point2.y)/(double)(point3.x-point2.x);

  if (point2.y==point1.y) Sign3=1;
  if (point3.y==point2.y) Sign4=1;

  if ((Sign1&&Sign3)||(Sign2&&Sign4)||
      (Sign1&&Sign2)||(Sign3&&Sign4)||fabs(tg2-tg1)<0.0001) return(OpOK);

  if ((Sign1&&Sign4)||(Sign2&&Sign3))
  {
     LineWidth=(LineWidth+1)/2;

     if (point2.x>point1.x)
     {
        midpoint1.y=point1.y;
        midpoint1.x=point2.x+LineWidth;
        midpoint5.x=midpoint1.x;
        if (point3.y>point2.y)
           midpoint5.y=point2.y-LineWidth;
        else
           midpoint5.y=point2.y+LineWidth;
        midpoint6.x=point2.x;
        midpoint6.y=midpoint5.y;
     }
     else
     if (point2.x<point1.x)
     {
        midpoint1.y=point2.y;
        midpoint1.x=point2.x-LineWidth;
        midpoint5.x=midpoint1.x;
        if (point3.y>point2.y)
           midpoint5.y=point2.y-LineWidth;
        else
           midpoint5.y=point2.y+LineWidth;
        midpoint6.x=point2.x;
        midpoint6.y=midpoint5.y;
     }
     else
     if (point2.y>point1.y)
     {
        midpoint1.x=point2.x;
        midpoint1.y=point2.y+LineWidth;
        midpoint5.y=midpoint1.y;
        if (point3.x>point2.x)
           midpoint5.x=point2.x-LineWidth;
        else
           midpoint5.x=point2.x+LineWidth;
        midpoint6.x=midpoint5.x;
        midpoint6.y=point2.y;
     }
     else
     if (point2.y<point1.y)
     {
        midpoint1.x=point2.x;
        midpoint1.y=point2.y-LineWidth;
        midpoint5.y=midpoint1.y;
        if (point3.x>point2.x)
           midpoint5.x=point2.x-LineWidth;
        else
           midpoint5.x=point2.x+LineWidth;
        midpoint6.x=midpoint5.x;
        midpoint6.y=point2.y;
     }

     memcpy(&multiploy1[0],&point2,sizeof(POINT));
     memcpy(&multiploy1[1],&midpoint1,sizeof(POINT));
     memcpy(&multiploy1[2],&midpoint5,sizeof(POINT));
     memcpy(&multiploy1[3],&midpoint6,sizeof(POINT));

     return(FillPolygon(lpdc,&multiploy1[0],4));
  }

  if (Sign3) /* point2.y==point1.y */
  {
     tg3=tg2;
     l=LineWidth*sqrt(1+1./(tg3*tg3));
     dx=l*0.5+0.5;
     dy=0;
  }
  else
  if (Sign1) /* point2.x==point1.x */
  {
     tg3=1./tg2;
     l=LineWidth*sqrt(1+1./(tg3*tg3));
     dx=0;
     dy=l*0.5+0.5;
  }
  else
  {
     if (fabs(tg1*tg2+1)>=0.0001)
     {
        tg3=(tg2-tg1)/(1+tg1*tg2);
        l=LineWidth*sqrt(1+1./(tg3*tg3));
     }
     else l=LineWidth;

     dx=l*0.5/sqrt(1.+(tg1*tg1))+0.5;
     dy=l*0.5/sqrt(1.+1./(tg1*tg1))+0.5;
  }

  /*if (tg1>0)
  {
     midpoint1.x=point2.x+dx;
     midpoint1.y=point2.y+dy;
  }
  else
  {
     midpoint1.x=point2.x-dx;
     midpoint1.y=point2.y+dy;
  }

  if (Sign4) *//* point3.y==point2.y */
  /*{
     tg3=tg1;
     l=LineWidth*sqrt(1+1./(tg3*tg3));
     dx=l*0.5+0.5;
     dy=0;
  }
  else
  if (Sign2) *//* point3.x==point2.x */
  /*{
     tg3=1./tg1;
     l=LineWidth*sqrt(1+1./(tg3*tg3));
     dx=0;
     dy=l*0.5+0.5;
  }
  else
  {
     if (fabs(tg1*tg2+1)>=0.0001)
     {
        tg3=(tg2-tg1)/(1+tg1*tg2);
        l=LineWidth*sqrt(1+1./(tg3*tg3));
     }
     else l=LineWidth;

     dx=l*0.5/sqrt(1.+(tg2*tg2))+0.5;
     dy=l*0.5/sqrt(1.+1./(tg2*tg2))+0.5;
  }

  if (tg2>0)
  {
     midpoint2.x=point2.x+dx;
     midpoint2.y=point2.y+dy;
     midpoint5.x=midpoint1.x+dx;
     midpoint5.y=midpoint1.y+dy;
  }
  else
  {
     midpoint2.x=point2.x-dx;
     midpoint2.y=point2.y+dy;
     midpoint5.x=midpoint1.x-dx;
     midpoint5.y=midpoint1.y+dy;
  }

  memcpy(&multiploy1[0],&point2,sizeof(POINT));
  memcpy(&multiploy1[1],&midpoint1,sizeof(POINT));
  memcpy(&multiploy1[2],&midpoint5,sizeof(POINT));
  memcpy(&multiploy1[3],&midpoint2,sizeof(POINT));*/

  if (tg1>0)
  {
     midpoint1.x=point2.x+dx;
     midpoint1.y=point2.y+dy;
     midpoint2.x=point2.x-dx;
     midpoint2.y=point2.y-dy;
  }
  else
  {
     midpoint1.x=point2.x-dx;
     midpoint1.y=point2.y+dy;
     midpoint2.x=point2.x+dx;
     midpoint2.y=point2.y-dy;
  }

  if (Sign4) /* point3.y==point2.y */
  {
     tg3=tg1;
     l=LineWidth*sqrt(1+1./(tg3*tg3));
     dx=l*0.5+0.5;
     dy=0;
  }
  else
  if (Sign2) /* point3.x==point2.x */
  {
     tg3=1/tg1;
     l=LineWidth*sqrt(1+1./(tg3*tg3));
     dx=0;
     dy=l*0.5+0.5;
  }
  else
  {
     if (fabs(tg1*tg2+1)>=0.0001)
     {
        tg3=(tg2-tg1)/(1+tg1*tg2);
        l=LineWidth*sqrt(1+1./(tg3*tg3));
     }
     else l=LineWidth;

     dx=l*0.5/sqrt(1.+(tg2*tg2))+0.5;
     dy=l*0.5/sqrt(1.+1./(tg2*tg2))+0.5;
  }

  if (tg2>0)
  {
     midpoint3.x=midpoint2.x-dx;
     midpoint3.y=midpoint2.y-dy;
     midpoint4.x=midpoint1.x-dx;
     midpoint4.y=midpoint1.y-dy;
     midpoint5.x=midpoint1.x+dx;
     midpoint5.y=midpoint1.y+dy;
     midpoint6.x=midpoint2.x+dx;
     midpoint6.y=midpoint2.y+dy;
  }
  else
  {
     midpoint3.x=midpoint2.x+dx;
     midpoint3.y=midpoint2.y-dy;
     midpoint4.x=midpoint1.x+dx;
     midpoint4.y=midpoint1.y-dy;
     midpoint5.x=midpoint1.x-dx;
     midpoint5.y=midpoint1.y+dy;
     midpoint6.x=midpoint2.x-dx;
     midpoint6.y=midpoint2.y+dy;
  }

  memcpy(&multiploy1[0],&midpoint3,sizeof(POINT));
  memcpy(&multiploy1[1],&midpoint4,sizeof(POINT));
  memcpy(&multiploy1[2],&midpoint5,sizeof(POINT));
  memcpy(&multiploy1[3],&midpoint6,sizeof(POINT));

  return(FillPolygon(lpdc,&multiploy1[0],4));
}

int makejoint(LPDC lpdc,int x1,int y1,int x2,int y2,int x3,int y3,
                 UINT LineWidth)
{
  POINT point1,point2,point3;

  point1.x=x1;
  point1.y=y1;
  point2.x=x2;
  point2.y=y2;
  point3.x=x3;
  point3.y=y3;

  return(MakeLineJoint(lpdc,point1,point2,point3,LineWidth));
}

static int MakeLineArrow(LPDC lpdc,POINT point1,POINT point2,
                  UINT LineWidth,UCHAR ArrowType)
{
  POINT midpoint1,midpoint2,midpoint3;
  POINT multiploy1[4];
  double dx,dy,tg,dx3,dy3;

  if ((LineWidth<=1)||(ArrowType>MAXARROWTYPE)||(ArrowType<=NOARROW))
     return OpOK;

  if (ArrowType==ARROWARC)
  {
       float startangle;

       if (point2.x==point1.x)
       {
          if (point2.y>point1.y)
             startangle=0;
          else
             startangle=PI;
       }
       else
       if (point2.y==point1.y)
       {
          if (point2.x>point1.x)
             startangle=PI*1.5;
          else
             startangle=PI*0.5;
       }
       else
       {
          startangle=atan((float)(point2.y-point1.y)/(float)(point2.x-point1.x));
          if (startangle>0)
             if (point2.y>point1.y)
                 startangle+=PI*1.5;
             else
                 startangle+=PI*0.5;
          else
             if (point2.y>point1.y)
                 startangle+=PI/2.;
             else
                 startangle+=PI*1.5;
       }
       ArcToArea(lpdc,point2,(LineWidth+2)/4,(LineWidth+2)/4,
                 startangle,startangle+PI,ORMODE,(LineWidth+1)/2);
       return OpOK;
  }

  tg=1.0;
  if (point2.x==point1.x)
  {
     if ((ArrowType==ARROWHOR)||(ArrowType==ARROWVER)) return(OpOK);

     dy=dx3=0;
     dx=(float)(LineWidth)*0.5;
     dy3=LineWidth*1.732;
  }
  else
  if (point2.y==point1.y)
  {
     if ((ArrowType==ARROWHOR)||(ArrowType==ARROWVER)) return(OpOK);

     dy=(float)(LineWidth)*0.5;
     dx3=LineWidth*1.732;
     dy3=dx=tg=0;
  }
  else
  {
     tg=(double)(point2.y-point1.y)/(double)(point2.x-point1.x);
     dx=(double)LineWidth*0.5/sqrt(1.+1./(tg*tg));
     dy=(double)LineWidth*0.5/sqrt(1.+(tg*tg));
     dx3=(double)LineWidth*1.732/sqrt(1.+(tg*tg));
     dy3=(double)LineWidth*1.732/sqrt(1.+1./(tg*tg));
  }
  if (tg>=0)
  {
     switch (ArrowType)
     {
       case ARROWHOR:
            if (point2.y>point1.y)
            {
               midpoint3.y=point2.y+(dy+0.5);
               midpoint3.x=point2.x+(dx+2.*dy/tg+0.5);
            }
            else
            {
               midpoint3.y=point2.y-(dy+0.5);
               midpoint3.x=point2.x-(dx+2.*dy/tg+0.5);
            }

            goto Point12;

       case ARROWVER:
            if (point2.y>point1.y)
            {
               midpoint3.x=point2.x+(dx+0.5);
               midpoint3.y=point2.y+(dy+2.*dx*tg+0.5);
            }
            else
            {
               midpoint3.x=point2.x-(dx+0.5);
               midpoint3.y=point2.y-(dy+2.*dx*tg+0.5);
            }

       Point12:

       case ARROWTRI1:
            midpoint1.x=point2.x-dx;
            midpoint2.x=point2.x+dx;
            midpoint1.y=point2.y+dy;
            midpoint2.y=point2.y-dy;
            if (ArrowType!=ARROWTRI1) break;
            if (point2.y==point1.y)
            {
               midpoint3.x=point2.x+(dx3*0.5+0.5)*((point2.x-point1.x>0)?1:-1);
               midpoint3.y=point2.y+(dy3*0.5+0.5);
            }
            else
            if (point2.y>point1.y)
            {
               midpoint3.x=point2.x+(dx3*0.5+0.5);
               midpoint3.y=point2.y+(dy3*0.5+0.5);
            }
            else
            {
               midpoint3.x=point2.x-(dx3*0.5+0.5);
               midpoint3.y=point2.y-(dy3*0.5+0.5);
            }
            break;
       case ARROWTRI2:
            midpoint1.x=point2.x-(2.*dx+0.5);
            midpoint2.x=point2.x+(2.*dx+0.5);
            midpoint1.y=point2.y+(2.*dy+0.5);
            midpoint2.y=point2.y-(2.*dy+0.5);
            if (point2.y==point1.y)
            {
               midpoint3.x=point2.x+(dx3+0.5)*((point2.x-point1.x>0)?1:-1);
               midpoint3.y=point2.y+(dy3+0.5);
            }
            else
            if (point2.y>point1.y)
            {
               midpoint3.x=point2.x+(dx3+0.5);
               midpoint3.y=point2.y+(dy3+0.5);
            }
            else
            {
               midpoint3.x=point2.x-(dx3+0.5);
               midpoint3.y=point2.y-(dy3+0.5);
            }
            break;
     }
  }
  else
  {
     switch (ArrowType)
     {
       case ARROWHOR:
            if (point2.y>point1.y)
            {
               midpoint3.y=point2.y+(dy+0.5);
               midpoint3.x=point2.x+(2.*dy/tg-dx+0.5);
            }
            else
            {
               midpoint3.y=point2.y-(dy+0.5);
               midpoint3.x=point2.x+(dx-2.*dy/tg+0.5);
            }

            goto Point21;

       case ARROWVER:
            if (point2.y>point1.y)
            {
               midpoint3.x=point2.x-(dx+0.5);
               midpoint3.y=point2.y+dy-(2.*dx*tg+0.5);
            }
            else
            {
               midpoint3.x=point2.x+(dx+0.5);
               midpoint3.y=point2.y+(2.*dx*tg-dy+0.5);
            }

       Point21:

       case ARROWTRI1:
            midpoint1.x=point2.x+(dx+0.5);
            midpoint2.x=point2.x-(dx+0.5);
            midpoint1.y=point2.y+(dy+0.5);
            midpoint2.y=point2.y-(dy+0.5);
            if (ArrowType!=ARROWTRI1) break;
            if (point2.y>point1.y)
            {
               midpoint3.x=point2.x-(dx3*0.5+0.5);
               midpoint3.y=point2.y+(dy3*0.5+0.5);
            }
            else
            {
               midpoint3.x=point2.x+(dx3*0.5+0.5);
               midpoint3.y=point2.y-(dy3*0.5+0.5);
            }
            break;
       case ARROWTRI2:
            midpoint1.x=point2.x+(2.*dx+0.5);
            midpoint2.x=point2.x-(2.*dx+0.5);
            midpoint1.y=point2.y+(2*dy+0.5);
            midpoint2.y=point2.y-(2*dy+0.5);
            if (point2.y>point1.y)
            {
               midpoint3.x=point2.x-(dx3+0.5);
               midpoint3.y=point2.y+(dy3+0.5);
            }
            else
            {
               midpoint3.x=point2.x+(dx3+0.5);
               midpoint3.y=point2.y-(dy3+0.5);
            }
            break;
     }
  }

  memcpy(&multiploy1[0],&midpoint1,sizeof(POINT));
  memcpy(&multiploy1[1],&midpoint2,sizeof(POINT));
  memcpy(&multiploy1[2],&midpoint3,sizeof(POINT));

  FillPolygon(lpdc,&multiploy1[0],3);
  return OpOK;
}

int makearrow(LPDC lpdc,int x1,int y1,int x2,int y2,
              UINT LineWidth,UCHAR ArrowType)
{
  POINT point1,point2;

  point1.x=x1;
  point1.y=y1;
  point2.x=x2;
  point2.y=y2;

  return(MakeLineArrow(lpdc,point1,point2,LineWidth,ArrowType));
}


////////Here I am adding some meta codes
/* ------- Followed for meta draw -------- */

int DrawUpTriangle(LPDC lpdc,int x1,int y1,int x2,int y2,
                   UCHAR FillMode,UINT LineWidth,
                   UCHAR LineType)
{
  long min_x,min_y,max_x,max_y;

  min_x=min(x1,x2);
  max_x=max(x1,x2);
  min_y=min(y1,y2);
  max_y=max(y1,y2);

  WithWidthLine(lpdc,(x1+x2)/2,min_y,min_x,max_y,LineWidth,LineType,
                NOARROW,FillMode);
  WithWidthLine(lpdc,(x1+x2)/2,min_y,max_x,max_y,LineWidth,LineType,
                NOARROW,FillMode);
  WithWidthLine(lpdc,min_x,max_y,max_x,max_y,LineWidth,LineType,
                NOARROW,FillMode);
  makejoint(lpdc,min_x,max_y,max_x,max_y,(x1+x2)/2,min_y,LineWidth);
  makejoint(lpdc,max_x,max_y,(x1+x2)/2,min_y,min_x,max_y,LineWidth);
  makejoint(lpdc,(x1+x2)/2,min_y,min_x,max_y,max_x,max_y,LineWidth);
  return(OpOK);
}

int DrawDownTriangle(LPDC lpdc,int x1,int y1,int x2,int y2,
                     UCHAR FillMode,UINT LineWidth,
                     UCHAR LineType)
{
  long min_x,min_y,max_x,max_y;

  min_x=min(x1,x2);
  max_x=max(x1,x2);
  min_y=min(y1,y2);
  max_y=max(y1,y2);

  WithWidthLine(lpdc,(x1+x2)/2,max_y,min_x,min_y,LineWidth,LineType,
                NOARROW,FillMode);
  WithWidthLine(lpdc,(x1+x2)/2,max_y,max_x,min_y,LineWidth,LineType,
                NOARROW,FillMode);
  WithWidthLine(lpdc,min_x,min_y,max_x,min_y,LineWidth,LineType,
                NOARROW,FillMode);
  makejoint(lpdc,min_x,min_y,max_x,min_y,(x1+x2)/2,max_y,LineWidth);
  makejoint(lpdc,max_x,min_y,(x1+x2)/2,max_y,min_x,min_y,LineWidth);
  makejoint(lpdc,(x1+x2)/2,max_y,min_x,min_y,max_x,min_y,LineWidth);
  return(OpOK);
}

int DrawRectangle(LPDC lpdc,int x1,int y1,int x2,int y2,
                  UCHAR FillMode,UINT LineWidth,
                  UCHAR LineType,UCHAR LineArrow)
{
  long min_x,min_y,max_x,max_y;

  min_x=min(x1,x2);
  max_x=max(x1,x2);
  min_y=min(y1,y2);
  max_y=max(y1,y2);

  switch (LineArrow) {
      case ARROWARC:
         {
            POINT MiddlePoint;
            int minworh;

            minworh=(min(max_x-min_x,max_y-min_y)+5)/10;

            WithWidthLine(lpdc,min_x,min_y+minworh,min_x,max_y-minworh,LineWidth,
                          LineType,NOARROW,FillMode);
            WithWidthLine(lpdc,max_x,min_y+minworh,max_x,max_y-minworh,LineWidth,
                          LineType,NOARROW,FillMode);
            WithWidthLine(lpdc,min_x+minworh,min_y,max_x-minworh,min_y,LineWidth,
                          LineType,NOARROW,FillMode);
            WithWidthLine(lpdc,min_x+minworh,max_y,max_x-minworh,max_y,LineWidth,
                          LineType,NOARROW,FillMode);

            MiddlePoint.x=min_x+minworh;
            MiddlePoint.y=min_y+minworh;
            ArcToArea(lpdc,MiddlePoint,(float)minworh,(float)minworh,PI,PI*1.5,
                      FillMode,LineWidth);
            MiddlePoint.x=max_x-minworh;
            ArcToArea(lpdc,MiddlePoint,(float)minworh,(float)minworh,PI*1.5,PI*2.,
                      FillMode,LineWidth);
            MiddlePoint.y=max_y-minworh;
            ArcToArea(lpdc,MiddlePoint,(float)minworh,(float)minworh,(float)0,PI/2.,
                      FillMode,LineWidth);
            MiddlePoint.x=min_x+minworh;
            ArcToArea(lpdc,MiddlePoint,(float)minworh,(float)minworh,PI/2.,PI,
                      FillMode,LineWidth);
         }
         break;
      case HALFARROWARC:
           {
              POINT MiddlePoint;
              int r = (max_y - min_y+1)/2;

              MiddlePoint.x=min_x+r;
              MiddlePoint.y=(min_y+max_y+1)/2;
              ArcToArea(lpdc,MiddlePoint,(float) r,
                        (float)r,PI*0.5,PI*1.5,
                        FillMode,LineWidth);
              MiddlePoint.x=max_x-r;
              ArcToArea(lpdc,MiddlePoint,(float)(max_y-min_y+1)/2,
                        (float)(max_y-min_y+1)/2,PI*1.5,PI*0.5,
                        FillMode,LineWidth);
              WithWidthLine(lpdc,max_x-r,min_y,min_x+r,min_y,LineWidth,LineType,
                   NOARROW,FillMode);
              WithWidthLine(lpdc,min_x+r,max_y,max_x-r,max_y,LineWidth,LineType,
                   NOARROW,FillMode);
          }
          break;
      default:
        WithWidthLine(lpdc,max_x,min_y,min_x,min_y,LineWidth,LineType,
                   NOARROW,FillMode);
        WithWidthLine(lpdc,min_x,max_y,max_x,max_y,LineWidth,LineType,
                   NOARROW,FillMode);
        WithWidthLine(lpdc,min_x,min_y,min_x,max_y,LineWidth,LineType,
                   NOARROW,FillMode);
        WithWidthLine(lpdc,max_x,max_y,max_x,min_y,LineWidth,LineType,
                   NOARROW,FillMode);
        makejoint(lpdc,min_x,min_y,min_x,max_y,max_x,max_y,LineWidth);
        makejoint(lpdc,min_x,max_y,max_x,max_y,max_x,min_y,LineWidth);
        makejoint(lpdc,max_x,max_y,max_x,min_y,min_x,min_y,LineWidth);
        makejoint(lpdc,max_x,min_y,min_x,min_y,min_x,max_y,LineWidth);
        break;
  } /* switch */
  return(OpOK);
}


//////////////JJJJJ@@@@@


/* ------- Followed for arc restore ----- */
static void ArcToArea(LPDC lpdc,POINT MiddlePoint,float Xlen,float Ylen,
    float StartAngle,float FinishAngle,UCHAR FillMode,
    UINT LineWidth)
{
   float xlen1,ylen1,xlen2,ylen2,angle11,angle12,angle21,angle22,atob1,atob2;
   float ctgs,ctgf;
   float Crossy1,Crossx2,Crossy2,Crossy3,Crossx4,Crossy4,
         y,x11,x12,x21,x22;
   int ymin,ymax;

   if (!LineWidth) return;

   for (;StartAngle<0;StartAngle+=2*PI);
   for (;StartAngle>2*PI+0.005;StartAngle-=2*PI);
   for (;FinishAngle<0;FinishAngle+=2*PI);
   for (;FinishAngle>2*PI+0.005;FinishAngle-=2*PI);

   if (StartAngle>FinishAngle)
   {
      ArcToArea(lpdc,MiddlePoint,Xlen,Ylen,StartAngle,2*PI,FillMode,LineWidth);
      ArcToArea(lpdc,MiddlePoint,Xlen,Ylen,0,FinishAngle,FillMode,LineWidth);
   }

   if (LineWidth==1)
   {
      xlen1=Xlen;
      ylen1=Ylen;
      xlen2=Xlen-1;
      ylen2=Ylen-1;
      atob1=(double)xlen1/(double)ylen1;
      if (ylen2>0) atob2=(double)xlen2/(double)ylen2;
      else atob2=0;
   }
   else
   {
      xlen1=Xlen+(LineWidth+1)/2;
      ylen1=Ylen+(LineWidth+1)/2;
      xlen2=Xlen-(LineWidth+1)/2;
      if (xlen2<0) xlen2=0;
      ylen2=Ylen-(LineWidth+1)/2;
      atob1=(double)xlen1/(double)ylen1;
      if (ylen2>0) atob2=(double)xlen2/(double)ylen2;
      else atob2=0;
   }

   Crossy1=ylen1*sin(StartAngle);
   Crossx2=xlen2*cos(StartAngle);
   Crossy2=ylen2*sin(StartAngle);
   Crossy3=ylen1*sin(FinishAngle);
   Crossx4=xlen2*cos(FinishAngle);
   Crossy4=ylen2*sin(FinishAngle);

   if ((fabs(StartAngle)>0.0001)&&(fabs(StartAngle-PI)>0.0001))
      ctgs=1/tan(StartAngle);
   if ((fabs(FinishAngle-PI)>0.0001)&&(fabs(FinishAngle-2*PI)>0.0001))
      ctgf=1/tan(FinishAngle);

   /*for (y=-ylen1;y<ylen1;y++)*/ /* <--- No Clip */

   ymin=max(-ylen1,lpdc->top-MiddlePoint.y);
   ymax=min(ylen1,lpdc->bottom-MiddlePoint.y);
   for (y=ymin;y<ymax;y++) /* <---  Only for Clip */
   {
       long x1,x2,x3,x4;

       if (!y)
       {
          if ((StartAngle<=0)&&(FinishAngle>=0))
             CurrentLineFillLine(max(lpdc->left,xlen1+MiddlePoint.x),
                            min(lpdc->right,xlen2+MiddlePoint.x),
                            MiddlePoint.y,lpdc);

          if ((StartAngle<=PI)&&(FinishAngle>=PI))
             CurrentLineFillLine(max(lpdc->left,-xlen2+MiddlePoint.x),
                            min(lpdc->right,-xlen1+MiddlePoint.x),
                            MiddlePoint.y,lpdc);

          continue;
       }

       angle11=asin((double)y/(double)ylen1);

           x12=atob1*sqrt((long)ylen1*ylen1-(long)y*y);
       x11=-x12;

       if (y<0) /* be sure if angle12>=angle11 */
       {
          angle12=2*PI+angle11;
          angle11=PI-angle11;
       }
       else
       {
          angle12=PI-angle11;
       }

       if (fabs(y)>=fabs(ylen2))
       {
          if (StartAngle<=angle11)
             if (FinishAngle>=angle11)
                if (y>0) x2=x12; else x1=x11;
             else continue;
          else
          {
             if (y>0&&Crossy2<=y&&y<=Crossy1)
             {
                x2=Crossx2+(double)(y-Crossy2)*ctgs;
             }
             else
             if (y<0&&Crossy1<=y&&y<=Crossy2)
             {
                x1=Crossx2+(double)(y-Crossy2)*ctgs;
             }
             else continue;
          }

          if (StartAngle<=angle12)
             if (FinishAngle>=angle12)
                if (y>0) x1=x11; else x2=x12;
             else
             {
                if (y>0&&Crossy4<=y&&y<=Crossy3)
                {
                   x1=Crossx4+(double)(y-Crossy4)*ctgf;
                }
                else
                if (y<0&&Crossy3<=y&&y<=Crossy4)
                {
                   x2=Crossx4+(double)(y-Crossy4)*ctgf;
                }
                else continue;
             }
          else continue;

          /*LineFillBuffer(lpdc,y+MiddlePoint.y,x1+MiddlePoint.x,
                         x2+MiddlePoint.x,FillMode);*/ /* <--- No Clip */

          LineFillBuffer(lpdc,y+MiddlePoint.y,
                           max(lpdc->left,x1+MiddlePoint.x),
                           min(lpdc->right,x2+MiddlePoint.x),
                           FillMode);  /* ^--- Only for Clip */

          continue;
       }

       angle21=asin((double)y/(double)ylen2);

           x22=atob2*sqrt((long)ylen2*ylen2-(long)y*y);
       x21=-x22;

       if (y<0) /* be sure if angle22>=angle21 */
       {
          angle22=2*PI+angle21;
          angle21=PI-angle21;
       }
       else
       {
          angle22=PI-angle21;
       }

       if (y>0)
       {
          if (StartAngle<=angle11)
             if (FinishAngle>=angle21)
             {
                x1=x22;
                x2=x12;
             }
             else goto LineTwo;
          else
          {
             if (Crossy2<y&&y<=Crossy1)
             {
                                if (Crossy4<=y&&y<Crossy3)
                                {
                                   if (StartAngle<PI*1.5-0.000001)
                                   {
                                          x1=Crossx2+(double)(y-Crossy2)*ctgs;
                                          x2=Crossx4+(double)(y-Crossy4)*ctgf;
                                   }
                                   else
                                   {
                                          x2=Crossx2+(double)(y-Crossy2)*ctgs;
                                          x1=Crossx4+(double)(y-Crossy4)*ctgf;
                                   }
                                }
                                else
                                {
                                   if (StartAngle<PI*0.5-0.000001)
                                   {
                                          x1=x22;
                                          x2=Crossx2+(double)(y-Crossy2)*ctgs;
                                          if(x2<x1) goto LineTwo;;
                                   }
                                   else
                                   {
                                          x1=x11;
                                          x2=Crossx2+(double)(y-Crossy2)*ctgs;
                                          if(x2<x1) goto LineTwo;;
                                   }
                }
             }
             else goto LineTwo;
          }
       }
       else
       {
          if (StartAngle<=angle11)
             if (FinishAngle>=angle21)
             {
                x1=x21;
                x2=x11;
             }
             else goto LineTwo;
          else
          {
             if (Crossy1<=y&&y<Crossy2)
             {
                                if (Crossy3<=y&&y<Crossy4)
                                {
                                   if (StartAngle<PI*1.5-0.000001)
                                   {
                                          x1=Crossx2+(double)(y-Crossy2)*ctgs;
                                          x2=Crossx4+(double)(y-Crossy4)*ctgf;
                                   }
                                   else
                                   {
                                          x2=Crossx2+(double)(y-Crossy2)*ctgs;
                                          x1=Crossx4+(double)(y-Crossy4)*ctgf;
                                   }
                                }
                                else
                                {
                                   if (StartAngle<PI*1.5-0.000001)
                                   {
                                          x1=Crossx2+(double)(y-Crossy2)*ctgs;
                                          x2=x21;
                                          if(x2<x1) goto LineTwo;
                                   }
                                   else
                                   {
                                          x1=Crossx2+(double)(y-Crossy2)*ctgs;
                                          x2=x12;
                                          if(x2<x1) goto LineTwo;
                                   }
                }
             }
             else goto LineTwo;
          }
       }

       /*LineFillBuffer(lpdc,y+MiddlePoint.y,x1+MiddlePoint.x,
                      x2+MiddlePoint.x,FillMode);*/ /* <--- No Clip */

       LineFillBuffer(lpdc,y+MiddlePoint.y,
                        max(lpdc->left,x1+MiddlePoint.x),
                        min(lpdc->right,x2+MiddlePoint.x),
                        FillMode); /* ^--- Only for Clip */

       LineTwo:

       if (y>0)
       {
          if (StartAngle<=angle22)
             if (FinishAngle>=angle12)
             {
                x3=x21;
                x4=x11;
             }
             else
             {
                                if (Crossy4<y&&y<=Crossy3)
                {
                                   if (Crossy2<=y&&y<Crossy1) continue;
                   if (FinishAngle<PI*0.5+0.0001)
                   {
                      x3=Crossx4+(double)(y-Crossy4)*ctgf;
                      x4=x12;
                                          if(x4<x3) continue;
                   }
                   else
                   {
                      x3=Crossx4+(double)(y-Crossy4)*ctgf;
                                          x4=x21;
                                          if(x4<x3) continue;
                   }
                }
                   else continue;
             }
          else continue;
       }
       else
       {
          if (StartAngle<=angle22)
             if (FinishAngle>=angle12)
             {
                 x3=x22;
                 x4=x12;
             }
             else
             {
                                if (Crossy3<=y&&y<Crossy4)        /*--- !!! -------*/
                {
                                   if (Crossy1<=y&&y<Crossy2) continue;
                   if (FinishAngle<PI*1.5+0.0001)
                   {
                      x3=x11;
                                          x4=Crossx4+(double)(y-Crossy4)*ctgf;
                                          if(x4<x3) continue;
                   }
                   else
                   {
                      x3=x22;
                                          x4=Crossx4+(double)(y-Crossy4)*ctgf;
                                          if(x4<x3) continue;
                   }
                }
                else continue;
             }
          else continue;
       }

       /*LineFillBuffer(lpdc,y+MiddlePoint.y,x3+MiddlePoint.x,
                      x4+MiddlePoint.x,FillMode);*/ /* <--- No Clip */

       LineFillBuffer(lpdc,y+MiddlePoint.y,
                        max(lpdc->left,x3+MiddlePoint.x),
                        min(lpdc->right,x4+MiddlePoint.x),
                        FillMode); /* ^--- Only for Clip */
   }
}

int WithArcTypeArc(LPDC lpdc,POINT MiddlePoint,float Xlen,float Ylen,
    float StartAngle,float FinishAngle,UCHAR FillMode,
    UINT ArcWidth,UINT ArcType)
{
  static ArcTypes[MaxArcType]={12,8,4,1};

  float MiddleStartAngle,MiddleFinishAngle;
  float ArcLengthAngle,StepAngle;

  for (;StartAngle<0;StartAngle+=2*PI);
  for (;StartAngle>2*PI+0.005;StartAngle-=2*PI);
  for (;FinishAngle<0;FinishAngle+=2*PI);
  for (;FinishAngle>2*PI+0.005;FinishAngle-=2*PI);

  if (StartAngle>FinishAngle)
  {
     WithArcTypeArc(lpdc,MiddlePoint,Xlen,Ylen,StartAngle,2*PI,
                     FillMode,ArcWidth,ArcType);
     WithArcTypeArc(lpdc,MiddlePoint,Xlen,Ylen,0,FinishAngle,
                     FillMode,ArcWidth,ArcType);
  }

  if (ArcType>MaxArcType) ArcType=NoArcType;

  switch(ArcType)
  {
    case NoArcType:
         ArcLengthAngle=StepAngle=FinishAngle-StartAngle;
         break;
    case ArcType1:
    case ArcType2:
    case ArcType3:
    case ArcType4:
         StepAngle=ArcTypes[ArcType1-1]*MaxArcTypeAngle/MaxArcTypeLength;
         ArcLengthAngle=MaxArcTypeAngle;
         break;
  }

  for (MiddleStartAngle=StartAngle;MiddleStartAngle<FinishAngle;)
  {
      MiddleFinishAngle=MiddleStartAngle+StepAngle;
      if (MiddleFinishAngle>FinishAngle) MiddleFinishAngle=FinishAngle;
      ArcToArea(lpdc,MiddlePoint,Xlen,Ylen,MiddleStartAngle,MiddleFinishAngle,
                FillMode,ArcWidth);
      MiddleStartAngle+=ArcLengthAngle;
  }
  return(OpOK);
}
