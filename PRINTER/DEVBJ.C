/*--------- BJ: A4,A3, 300,400, 600DPI ----------*/
/*-------------------------------------------------------------------
* Name: devbj.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int blanklines;

typedef struct BJtag
{
    USHORT TopBlank;      //Lines
    USHORT LeftBlank;     //Points
    USHORT PageWidth;     //Word 16 bits
    USHORT PageHeight;    //Lines
    char  *Addr;
}  BJ_PARA;

/*
static BJ_PARA print[]={
    {100,10,160,3450,NULL},
    {100,10,213,4600,NULL},
    {100,10,320,6900,NULL},

    {100,10,320,6900,NULL},
    {100,10,426,9200,NULL},
    {100,10,640,13800,NULL},

    {100,10,188,4140,NULL},
    {100,10,225,4968,NULL},
    {100,10,250,5520,NULL},
    {100,10,375,8280,NULL}
};
*/
/*
static unsigned short PaperPara[][3] = {
                  {142,3200,300},  // A4300 //
                  {192,4096,400},  // A4400 //
                  {280,6200,600},  // A4600 //

                  {210,4800,300},  // A3300 //
                  {280,6400,400},  // A3400 //
                  {420,9600,600},  // A3600 //

                  {180,4125,300},  // B4300 //
                  {216,4950,360},  // B4360 //
                  {240,5500,400},  // B4400 //
                  {360,8250,600}   // B4600 //
                };
*/
static unsigned short *myPrn,PrnParaT[6] = { 40,40,256,3000, 0,0 };

static unsigned short *GetBuff(void)
{
   return (unsigned short *)0x0220;
}
static void SwapBuff(void)
{
  int i;
  unsigned short tt;

  for (i=0;i<6;i++)
  {
    tt=myPrn[i];
    myPrn[i]=PrnParaT[i];
    PrnParaT[i]=tt;
  }
}

static int bj_init(UDATA pagew,UDATA pageh,int ty)
{
  union REGS Reg;
  struct SREGS SReg;
  int lines;
  ULONG m;

  PrnParaT[0]=printer->TopMargin;
  PrnParaT[1]=printer->LeftMargin;

  PrnParaT[2]=(printer->xpixel+15)/16;
  PrnParaT[3]=(printer->ypixel);

  prnstr=NULL;

  segread(&SReg);

  Reg.w.ax=0;                   //Init printer
  int386(0xf0,&Reg,&Reg);
  if (Reg.w.ax)
  {
    MessageBox("北佳打印机","  初始化错误 ! ",1,0);
    return -1;
  }

  //if ((pagew+15)/16<PrnParaT[2])
  //    PrnParaT[2]=(pagew+15)/16;
  //if (pageh<PrnParaT[3])
  //    PrnParaT[3]=pageh;

                                    //Allow space
  RastWidth=PrnParaT[2]*16;
  RastWidthByte = RastWidth>>3;
  lines= PrnParaT[3];

  rasts[0]=NULL;

  m = lines*RastWidthByte;
  rasts[1] = malloc(m+0x10000);

  if(rasts[1]==NULL)
   {
       MessageBox("北佳打印机","    内存不够 ! ",1,0);
       return(-1);
   }

  if (!lock_region(rasts[1],m+0x10000))  //Lock it
  {
     free(rasts[1]);
     return -1;
  };

  rasts[0]=(char *)(((unsigned long)rasts[1]/0x10000)*0x10000+0x10000);

  RastHeight=lines;
  RastSize=m;
  memset(rasts[0],0,RastSize);         // clear it

  PrnParaT[4]=(unsigned long)rasts[0]%0x10000;
  PrnParaT[5]=(unsigned long)rasts[0]/0x10000;

  myPrn=GetBuff();
  Reg.w.si= (unsigned long)myPrn%0x10000;    //Set Page
  SReg.es = ((unsigned long)myPrn/0x10000)*0x1000;  //Set Page
  Reg.w.cx= 6;
  Reg.w.ax=0x0100;

  SwapBuff();
  int386x(0xf0,&Reg,&Reg,&SReg);
  SwapBuff();

  if (Reg.w.ax)
  {
    MessageBox("北佳打印机","  页面设置错误 ! ",1,0);
    return -1;
  }

  blanklines = 0;
  fFirstBlock=TRUE;
  return 1;
}

static int bj_a4_300_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,0);}
static int bj_a4_400_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,1);}
static int bj_a4_600_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,2);}
static int bj_a3_300_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,3);}
static int bj_a3_400_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,4);}
static int bj_a3_600_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,5);}
static int bj_b4_300_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,6);}
static int bj_b4_360_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,7);}
static int bj_b4_400_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,8);}
static int bj_b4_600_init(UDATA pagew,UDATA pageh) { return bj_init(pagew,pageh,9);}

static void bj_FF()
{
  union REGS Reg;
  int i;

  for (i=0;i<PrintCopyN;i++)
  {
  Reg.w.ax=0x0300;                   //feed printer
  Reg.w.bx=1;
  int386(0xf0,&Reg,&Reg);
  }
  memset(rasts[0],0,RastSize);
  fFirstBlock=TRUE;
  blanklines = 0;
}

static void bj_over()
{
    unlock_region(rasts[1],RastSize+0x10000);
    free(rasts[1]);
}

static int bj_getheight() { return RastHeight; }
static void bj_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   BW_scanline(x1,x2,y,lpdc);
}

static void bj_block()
{
}

static void bj_setcolor(int color)
{
   sysColor = color;
}

static void bj_setGray(int gray)
{
   BW_setGray(gray);
}

static void bj_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   bj_setGray(gray);
}

static void bj_setCMYKcolor(int c,int m,int y,int k)
{
}

PRINTER BJA3_600printer = {
  DEV_BW,
  bj_a3_600_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  600,
  60*115,                // 600x11.5
  60*170,                // 600*17.0
  118,0,               // leftmargin=5mm
};

PRINTER BJA3_400printer = {
  DEV_BW,
  bj_a3_400_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  400,
  40*115,                // 300x11.5
  40*170,                // 300*17.0
  78,0,                // leftmargin=5mm
};

PRINTER BJA3_300printer = {
  DEV_BW,
  bj_a3_300_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  300,
  30*115,                // 300x11.5
  30*170,                // 300*17.0
  59,0,                // leftmargin=5mm
};

PRINTER BJA4_600printer = {
  DEV_BW,
  bj_a4_600_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  600,
  60*85,                 // 600x8.5
  60*115,                // 600*11.5
  118,0,               // leftmargin=5mm,
};

PRINTER BJA4_300printer = {
  DEV_BW,
  bj_a4_300_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  300,
  30*85,                 // 300x8.5
  30*115,                // 300*11.5
  59,0,                // leftmargin=5mm,
};

PRINTER BJA4_400printer = {
  DEV_BW,
  bj_a4_400_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  400,
  40*85,                 // 300x8.5
  40*115,                // 300*11.5
  78,0,                // leftmargin=5mm,
};

PRINTER BJB4_300printer = {
  DEV_BW,
  bj_b4_300_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  300,
  30*100,                 // 300x8.5
  30*138,                // 300*11.5
  78,0,                // leftmargin=5mm,
};

PRINTER BJB4_360printer = {
  DEV_BW,
  bj_b4_360_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  360,
  36*100,                 // 300x8.5
  36*138,                // 300*11.5
  78,0,                // leftmargin=5mm,
};

PRINTER BJB4_400printer = {
  DEV_BW,
  bj_b4_400_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  400,
  40*100,                 // 300x8.5
  40*138,                // 300*11.5
  78,0,                // leftmargin=5mm,
};

PRINTER BJB4_600printer = {
  DEV_BW,
  bj_b4_600_init,
  bj_block,
  bj_FF,
  bj_over,
  bj_getheight,
  bj_scanfill,
  bj_setcolor,
  bj_setRGBcolor,
  bj_setCMYKcolor,
  bj_setGray,
  600,
  60*100,                 // 600x8.5
  60*138,                // 600*11.5
  118,0,               // leftmargin=5mm,
};

