/*-------------------------------------------------------------------
* Name: fixedc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static long sintab[360]={
        0l,
        1143l,
        2287l,
        3429l,
        4571l,
        5711l,
        6850l,
        7986l,
        9120l,
        10252l,
        11380l,
        12504l,
        13625l,
        14742l,
        15854l,
        16961l,
        18064l,
        19160l,
        20251l,
        21336l,
        22414l,
        23486l,
        24550l,
        25606l,
        26655l,
        27696l,
        28729l,
        29752l,
        30767l,
        31772l,
        32767l,
        33753l,
        34728l,
        35693l,
        36647l,
        37589l,
        38521l,
        39440l,
        40347l,
        41243l,
        42125l,
        42995l,
        43852l,
        44695l,
        45525l,
        46340l,
        47142l,
        47929l,
        48702l,
        49460l,
        50203l,
        50931l,
        51643l,
        52339l,
        53019l,
        53683l,
        54331l,
        54963l,
        55577l,
        56175l,
        56755l,
        57319l,
        57864l,
        58393l,
        58903l,
        59395l,
        59870l,
        60326l,
        60763l,
        61183l,
        61583l,
        61965l,
        62328l,
        62672l,
        62997l,
        63302l,
        63589l,
        63856l,
        64103l,
        64331l,
        64540l,
        64729l,
        64898l,
        65047l,
        65176l,
        65286l,
        65376l,
        65446l,
        65496l,
        65526l,
        65536l,
        65526l,
        65496l,
        65446l,
        65376l,
        65286l,
        65176l,
        65047l,
        64898l,
        64729l,
        64540l,
        64331l,
        64103l,
        63856l,
        63589l,
        63302l,
        62997l,
        62672l,
        62328l,
        61965l,
        61583l,
        61183l,
        60763l,
        60326l,
        59870l,
        59395l,
        58903l,
        58393l,
        57864l,
        57319l,
        56755l,
        56175l,
        55577l,
        54963l,
        54331l,
        53683l,
        53019l,
        52339l,
        51643l,
        50931l,
        50203l,
        49460l,
        48702l,
        47929l,
        47142l,
        46340l,
        45525l,
        44695l,
        43852l,
        42995l,
        42125l,
        41243l,
        40347l,
        39440l,
        38521l,
        37589l,
        36647l,
        35693l,
        34728l,
        33753l,
        32768l,
        31772l,
        30767l,
        29752l,
        28729l,
        27696l,
        26655l,
        25606l,
        24550l,
        23486l,
        22414l,
        21336l,
        20251l,
        19160l,
        18064l,
        16961l,
        15854l,
        14742l,
        13625l,
        12504l,
        11380l,
        10252l,
        9120l,
        7986l,
        6850l,
        5711l,
        4571l,
        3429l,
        2287l,
        1143l,
        0l,
        -1143l,
        -2287l,
        -3429l,
        -4571l,
        -5711l,
        -6850l,
        -7986l,
        -9120l,
        -10252l,
        -11380l,
        -12504l,
        -13625l,
        -14742l,
        -15854l,
        -16961l,
        -18064l,
        -19160l,
        -20251l,
        -21336l,
        -22414l,
        -23486l,
        -24550l,
        -25606l,
        -26655l,
        -27696l,
        -28729l,
        -29752l,
        -30767l,
        -31772l,
        -32767l,
        -33753l,
        -34728l,
        -35693l,
        -36647l,
        -37589l,
        -38521l,
        -39440l,
        -40347l,
        -41243l,
        -42125l,
        -42995l,
        -43852l,
        -44695l,
        -45525l,
        -46340l,
        -47142l,
        -47929l,
        -48702l,
        -49460l,
        -50203l,
        -50931l,
        -51643l,
        -52339l,
        -53019l,
        -53683l,
        -54331l,
        -54963l,
        -55577l,
        -56175l,
        -56755l,
        -57319l,
        -57864l,
        -58393l,
        -58903l,
        -59395l,
        -59870l,
        -60326l,
        -60763l,
        -61183l,
        -61583l,
        -61965l,
        -62328l,
        -62672l,
        -62997l,
        -63302l,
        -63589l,
        -63856l,
        -64103l,
        -64331l,
        -64540l,
        -64729l,
        -64898l,
        -65047l,
        -65176l,
        -65286l,
        -65376l,
        -65446l,
        -65496l,
        -65526l,
        -65536l,
        -65526l,
        -65496l,
        -65446l,
        -65376l,
        -65286l,
        -65176l,
        -65047l,
        -64898l,
        -64729l,
        -64540l,
        -64331l,
        -64103l,
        -63856l,
        -63589l,
        -63302l,
        -62997l,
        -62672l,
        -62328l,
        -61965l,
        -61583l,
        -61183l,
        -60763l,
        -60326l,
        -59870l,
        -59395l,
        -58903l,
        -58393l,
        -57864l,
        -57319l,
        -56755l,
        -56175l,
        -55577l,
        -54963l,
        -54331l,
        -53683l,
        -53019l,
        -52339l,
        -51643l,
        -50931l,
        -50203l,
        -49460l,
        -48702l,
        -47929l,
        -47142l,
        -46340l,
        -45525l,
        -44695l,
        -43852l,
        -42995l,
        -42125l,
        -41243l,
        -40347l,
        -39440l,
        -38521l,
        -37589l,
        -36647l,
        -35693l,
        -34728l,
        -33753l,
        -32768l,
        -31772l,
        -30767l,
        -29752l,
        -28729l,
        -27696l,
        -26655l,
        -25606l,
        -24550l,
        -23486l,
        -22414l,
        -21336l,
        -20251l,
        -19160l,
        -18064l,
        -16961l,
        -15854l,
        -14742l,
        -13625l,
        -12504l,
        -11380l,
        -10252l,
        -9120l,
        -7986l,
        -6850l,
        -5711l,
        -4571l,
        -3429l,
        -2287l,
        -1143l
};

long LSin(int a)
{
  int i;

  i = a % 360;
  if (i<0) i+=360;
  return sintab[i];
}

void FixedAdd(lpf1,lpf2,lpf)
LPFIXED lpf1,lpf2,lpf;
{
  Long2Fixed((*lpf),(Fixed2Long((*lpf1))+Fixed2Long((*lpf2))));
}

void FixedSub(lpf1,lpf2,lpf)
LPFIXED lpf1,lpf2,lpf;
{
  Long2Fixed((*lpf),(Fixed2Long((*lpf1))-Fixed2Long((*lpf2))));
}

void FixedMul(lpf1,lpf2,lpf)
LPFIXED lpf1,lpf2,lpf;
{
  Long2Fixed((*lpf),((Fixed2Long((*lpf1))>>8)*(Fixed2Long((*lpf2))>>8)));
}

void MAT2Mul(lpmat1,lpmat2,lpmat3)
LPMAT2 lpmat1,lpmat2,lpmat3;
{
  FIXED f1,f2,f3,f4,f5,f6,f7,f8;
  FixedMul(&(lpmat1->eM11),&(lpmat2->eM11),&f1);
  FixedMul(&(lpmat1->eM21),&(lpmat2->eM12),&f2);
  FixedMul(&(lpmat1->eM12),&(lpmat2->eM11),&f3);
  FixedMul(&(lpmat1->eM22),&(lpmat2->eM12),&f4);
  FixedMul(&(lpmat1->eM11),&(lpmat2->eM21),&f5);
  FixedMul(&(lpmat1->eM21),&(lpmat2->eM22),&f6);
  FixedMul(&(lpmat1->eM12),&(lpmat2->eM21),&f7);
  FixedMul(&(lpmat1->eM22),&(lpmat2->eM22),&f8);
  FixedAdd(&f1,&f2,&(lpmat3->eM11));
  FixedAdd(&f3,&f4,&(lpmat3->eM12));
  FixedAdd(&f5,&f6,&(lpmat3->eM21));
  FixedAdd(&f7,&f8,&(lpmat3->eM22));
}

#ifdef UNUSED           //ByHance, 96,1.29
FIXED AddFixed(FIXED Fixed1,FIXED Fixed2)
{
  FIXED ResultFixed;

  Long2Fixed(ResultFixed,(Fixed2Long(Fixed1)+Fixed2Long(Fixed2)));
  return(ResultFixed);
}

FIXED SubFixed(FIXED Fixed1,FIXED Fixed2)
{
  FIXED ResultFixed;

  Long2Fixed(ResultFixed,(Fixed2Long(Fixed1)-Fixed2Long(Fixed2)));
  return(ResultFixed);
}

FIXED MulFixed(FIXED Fixed1,FIXED Fixed2)
{
  FIXED ResultFixed;

  Long2Fixed(ResultFixed,((Fixed2Long(Fixed1)>>8)*(Fixed2Long(Fixed2)>>8)));
  return(ResultFixed);
}

MAT2 MulMAT2(MAT2 Mat21,MAT2 Mat22)
{
  MAT2 ResultMat2;

  MAT2Mul(&Mat21,&Mat22,&ResultMat2);
  return(ResultMat2);
}
#endif    // UNUSED           //ByHance, 96,1.29

void GetSkewMatrix2(MAT2 *Matrix,int SkewAngle)
{
  Int2Fixed(Matrix->eM11,1);
  Long2Fixed(Matrix->eM12,LSin(SkewAngle));
  Int2Fixed(Matrix->eM21,0);
  Int2Fixed(Matrix->eM22,1);
}

void GetRotateMatrix2(MAT2 *Matrix,int RotateAngle)
{
  Long2Fixed(Matrix->eM11,LCos(RotateAngle));
  Long2Fixed(Matrix->eM12,(-1)*LSin(RotateAngle));
  Long2Fixed(Matrix->eM21,LSin(RotateAngle));
  Long2Fixed(Matrix->eM22,LCos(RotateAngle));
}

void GetZoomMatrix2(MAT2 *Matrix,float ZoomX,float ZoomY)
{
  Float2Fixed(Matrix->eM11,ZoomX);
  Int2Fixed(Matrix->eM12,0);
  Int2Fixed(Matrix->eM21,0);
  Float2Fixed(Matrix->eM22,ZoomY);
}

void Matrix2ConvertPoint(MAT2 *Matrix,int *X,int *Y)
{
  FIXED TmpFixedX,TmpFixedY,TmpFixed1,TmpFixed2,TmpFixed3;

  Int2Fixed(TmpFixedX,*X);
  Int2Fixed(TmpFixedY,*Y);

  FixedMul(&(Matrix->eM11),&TmpFixedX,&TmpFixed1);
  FixedMul(&(Matrix->eM12),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  *X=IntofFixed(TmpFixed3);

  FixedMul(&(Matrix->eM21),&TmpFixedX,&TmpFixed1);
  FixedMul(&(Matrix->eM22),&TmpFixedY,&TmpFixed2);
  FixedAdd(&TmpFixed1,&TmpFixed2,&TmpFixed3);
  *Y=IntofFixed(TmpFixed3);
}
