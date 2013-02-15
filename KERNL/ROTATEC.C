/*-------------------------------------------------------------------
* Name: rotatec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

void RotatePoint(int *RotateX,int *RotateY, int OldX,int OldY,
                 int RotateAxisX,int RotateAxisY,int RotateAngle)
{
  /*float Angle,SinXX,CosXX;
  ORDINATETYPE TmpX,TmpY;

  Angle=RotateAngle*PI/180.;
  SinXX=sin(Angle);
  CosXX=cos(Angle);

  TmpX=OldX-RotateAxisX;
  TmpY=OldY-RotateAxisY;
  *RotateX=CosXX*TmpX-SinXX*TmpY+RotateAxisX;
  *RotateY=SinXX*TmpX+CosXX*TmpY+RotateAxisY;*/



  FIXED SinXX,CosXX,TmpX,TmpY,TmpF1,TmpF2,TmpF3;

  if (RotateAngle%360==0)      //By zjh 12.24/96
  {
    *RotateX=OldX;
    *RotateY=OldY;
    return ;
  }

  Long2Fixed(SinXX,LSin(RotateAngle));
  Long2Fixed(CosXX,LCos(RotateAngle));
  Int2Fixed(TmpX,(OldX-RotateAxisX));
  Int2Fixed(TmpY,(OldY-RotateAxisY));

  FixedMul(&CosXX,&TmpX,&TmpF1);
  FixedMul(&SinXX,&TmpY,&TmpF2);
  FixedSub(&TmpF1,&TmpF2,&TmpF3);
  *RotateX=RotateAxisX+IntofFixed(TmpF3);
  FixedMul(&SinXX,&TmpX,&TmpF1);
  FixedMul(&CosXX,&TmpY,&TmpF2);
  FixedAdd(&TmpF1,&TmpF2,&TmpF3);
  *RotateY=RotateAxisY+IntofFixed(TmpF3);

  return;
}

void Rotate(ORDINATETYPE *RotateX,ORDINATETYPE *RotateY,
            ORDINATETYPE OldX,ORDINATETYPE OldY,
            ORDINATETYPE RotateAxisX,ORDINATETYPE RotateAxisY,
            int RotateAngle)
{
  /*float Angle,SinXX,CosXX;
  ORDINATETYPE TmpX,TmpY;

  Angle=RotateAngle*PI/180.;
  SinXX=sin(Angle);
  CosXX=cos(Angle);

  TmpX=OldX-RotateAxisX;
  TmpY=OldY-RotateAxisY;
  *RotateX=CosXX*TmpX-SinXX*TmpY+RotateAxisX;
  *RotateY=SinXX*TmpX+CosXX*TmpY+RotateAxisY;*/

  FIXED SinXX,CosXX,TmpX,TmpY,TmpF1,TmpF2,TmpF3;

  Long2Fixed(SinXX,LSin(RotateAngle));
  Long2Fixed(CosXX,LCos(RotateAngle));
  Float2Fixed(TmpX,(OldX-RotateAxisX)/(float)SCALEMETER);
  Float2Fixed(TmpY,(OldY-RotateAxisY)/(float)SCALEMETER);

  FixedMul(&CosXX,&TmpX,&TmpF1);
  FixedMul(&SinXX,&TmpY,&TmpF2);
  FixedSub(&TmpF1,&TmpF2,&TmpF3);
  Long2Fixed(TmpF3,(long)SCALEMETER*Fixed2Long(TmpF3));
  *RotateX=RotateAxisX+IntofFixed(TmpF3);
  FixedMul(&SinXX,&TmpX,&TmpF1);
  FixedMul(&CosXX,&TmpY,&TmpF2);
  FixedAdd(&TmpF1,&TmpF2,&TmpF3);
  Long2Fixed(TmpF3,(long)SCALEMETER*Fixed2Long(TmpF3));
  *RotateY=RotateAxisY+IntofFixed(TmpF3);

  return;
}

int TriPointToAngle(ORDINATETYPE X1,ORDINATETYPE Y1,
                    ORDINATETYPE X2,ORDINATETYPE Y2,
                    ORDINATETYPE X3,ORDINATETYPE Y3)
{
  float CosXX,Distant1,Distant2,Distant3,Angle1,Angle2;
  int Angle;

  Distant1=sqrt((float)(X2-X1)*(float)(X2-X1)+(float)(Y2-Y1)*(float)(Y2-Y1));
  Distant2=sqrt((float)(X3-X1)*(float)(X3-X1)+(float)(Y3-Y1)*(float)(Y3-Y1));
  if (Distant1*Distant2<=0.00005)
     return(0);
  Distant3=sqrt((float)(X2-X3)*(float)(X2-X3)+(float)(Y2-Y3)*(float)(Y2-Y3));
  CosXX=(Distant1*Distant1+Distant2*Distant2-Distant3*Distant3)/
        (2.*Distant1*Distant2);
  Angle=acos(CosXX)*180/PI;
  Angle1=atan2(Y2-Y1,X2-X1);
  Angle2=atan2(Y3-Y1,X3-X1);
  if (Angle2>Angle1&&Angle2<PI+Angle1)
     return(Angle);
  else
     return(-Angle);
}

int ConvertRotateAngle(int OldAngle,ORDINATETYPE OldAxisX,
                       ORDINATETYPE OldAxisY,ORDINATETYPE NewAxisX,
                       ORDINATETYPE NewAxisY,ORDINATETYPE *Left,
                       ORDINATETYPE *Top)
{
  // float Angle;
  ORDINATETYPE MidLeft,MidTop;

  Rotate(&MidLeft,&MidTop,*Left,*Top,OldAxisX,OldAxisY,OldAngle);
  Rotate(Left,Top,MidLeft,MidTop,NewAxisX,NewAxisY,-OldAngle);

  return(OldAngle);
}

void ArctoLine(int AxisX,int AxisY,int Rx,int Ry,int StartAngle,int EndAngle,
               int *TotalPoints,int *RPoints,int AngleInc)
{
  long SinTheta,CosTheta;
  int Angle,NowX,NowY;

  for (Angle=StartAngle,*TotalPoints=0,EndAngle+=AngleInc-1;Angle<EndAngle;
       Angle+=AngleInc,(*TotalPoints)++)
  {
      CosTheta=LCos(Angle);
      SinTheta=LSin(Angle);
      NowX=AxisX+(float)Rx*((float)CosTheta/(float)0x10000l);
      NowY=AxisY+(float)Ry*((float)SinTheta/(float)0x10000l);
      //if (NowX<0||NowY<0)
      //         asm int 3;
      RPoints[2*(*TotalPoints)]=NowX;
      RPoints[2*(*TotalPoints)+1]=NowY;
  }
}
