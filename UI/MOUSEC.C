/*-------------------------------------------------------------------
* Name: mousec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

static BOOL fMouseInstall=FALSE;

typedef struct tagHardQueues
{
  unsigned short HardParam0;
  unsigned short HardParam1;
  unsigned short HardParam2;
  unsigned short HardParam3;
  unsigned char HardType;
} HardQueues;

int lock_region (void *address, unsigned length)
{
    union REGS regs;
    unsigned linear;

    /* for DOS/4GW: zero-based flat memory model, converting
       a pointer of any type to a linear address is trivial.
    */
    linear = (unsigned) address;

    regs.w.ax = 0x600;                  /* DPMI Lock Linear Region */
    regs.w.bx = (unsigned short) (linear >> 16); /* Linear address in BX:CX */
    regs.w.cx = (unsigned short) (linear & 0xFFFF);
    regs.w.si = (unsigned short) (length >> 16); /* Length in SI:DI */
    regs.w.di = (unsigned short) (length & 0xFFFF);
    int386 (0x31, &regs, &regs);
    return (! regs.w.cflag);            /* Return 0 if can't lock */
}

int unlock_region (void *address, unsigned length)
{
    union REGS regs;
    unsigned linear;

    linear = (unsigned) address;

    regs.w.ax = 0x601;                  /* DPMI Unlock Linear Region */
    regs.w.bx = (unsigned short) (linear >> 16); /* Linear address in BX:CX */
    regs.w.cx = (unsigned short) (linear & 0xFFFF);
    regs.w.si = (unsigned short) (length >> 16); /* Length in SI:DI */
    regs.w.di = (unsigned short) (length & 0xFFFF);
    int386 (0x31, &regs, &regs);
    return (! regs.w.cflag);            /* Return 0 if can't lock */
}


/* Following is Mouse Code */
#pragma off (check_stack)

#define MOUSEINT 1
#define TIMERINT 2
#define DOWNONINT 3

void _loadds far MouseDeal (int max, int mbx, int mcx, int mdx)
                                // int msi, int mdi)
{
#pragma aux MouseDeal parm [EAX] [EBX] [ECX] [EDX] // [ESI] [EDI]
//  if (!GetIntSign())
    static void InsertQueue(unsigned short Param0,unsigned short Param1,
                unsigned short Param2,unsigned short Param3,short HardType);
    InsertQueue(max,mbx,mcx,mdx,MOUSEINT);
}  /* MouseDeal */

/*------------- hard queue --------------*/
#define MAXQUEUELEN 64
static volatile HardQueues HardwareQueue[MAXQUEUELEN];
static volatile int HardQueueHead=0,HardQueueTail=0;

static int Qempty()
{
  return HardQueueHead==HardQueueTail;
}
static int Qfull()
{
   return HardQueueHead==((HardQueueTail+1 ) % MAXQUEUELEN);
}

// get a Handle from queue head for delete from queue
//return -1 when null
static int Qget()
{
   if (Qempty())
    return -1;
   else
   { int    p;
     p=HardQueueHead;
     HardQueueHead=(HardQueueHead+1) % MAXQUEUELEN;
     return p;
   }
}

// get a handle from queue tail for insert to queue
// return -1 when full
static int Qput()
{
   if (Qfull())
    return -1;
   else
   { int  p;
     p=HardQueueTail;
     HardQueueTail=(HardQueueTail+1) % MAXQUEUELEN;
     return p;
   }
}

static int MergeQueue(int Param0,int Param1,int Param2,int Param3,char HardType)
{
  if (HardType==MOUSEINT)
  {
     int p;

     p=HardQueueHead;
     while (p!=HardQueueTail)
     {
         if (HardType==HardwareQueue[p].HardType
             &&Param0==HardwareQueue[p].HardParam0
             &&Param1==HardwareQueue[p].HardParam1)
         {
            HardwareQueue[p].HardParam2=Param2;
            HardwareQueue[p].HardParam3=Param3;
            return(OpOK);
         }
         else
            p=(p+1) % MAXQUEUELEN;
     }
  }
  return(1);
}

static void InsertQueue(unsigned short Param0,unsigned short Param1,
                unsigned short Param2,unsigned short Param3,short HardType)
{
  if (HardType==MOUSEINT)
     if (MergeQueue(Param0,Param1,Param2,Param3,HardType)!=OpOK)
     {
         int cell=Qput();
         if (cell!=-1)
         {
           HardwareQueue[cell].HardParam0=Param0;
           HardwareQueue[cell].HardParam1=Param1;
           HardwareQueue[cell].HardParam2=Param2;
           HardwareQueue[cell].HardParam3=Param3;
           HardwareQueue[cell].HardType=HardType;
         }
     }
}

void HardQueuetoSoftQueue(void)
{
  int cell;

//  SetIntSign();
  while ((cell=Qget())!=-1)
  {
    switch (HardwareQueue[cell].HardType)
    {
      case MOUSEINT:
           MessageCreatbyMouse(HardwareQueue[cell].HardParam0,
                               HardwareQueue[cell].HardParam1,
                               HardwareQueue[cell].HardParam2,
                               HardwareQueue[cell].HardParam3);
           break;
      case TIMERINT:
           MessageCreatbyTimer(HardwareQueue[cell].HardParam0,
                               HardwareQueue[cell].HardParam1);
           break;
      case DOWNONINT:
           MouseDownOnTrigger(HardwareQueue[cell].HardParam0,
                              HardwareQueue[cell].HardParam1,
                              HardwareQueue[cell].HardParam2);
           break;
    }
  }
  ClearIntSign();
}

int fGuideLine=0;

void MouseShow(void)
{
  union REGS Reg;

  //if (ActiveWindow==1&&GlobalBoxHeadHandle>0) fGuideLine=1;
  //         else fGuideLine=0;


  if (fGuideLine)
  {
   struct viewporttype ViewInformation;
   int oldcolor=getcolor();
   int oldmode=getwritemode();
   getviewsettings(&ViewInformation);

   setviewport(0,0,getmaxx(),getmaxy(),0);
   setwritemode(XOR_PUT);
   setcolor(14);

   line(0,300,639,300);

   setviewport(ViewInformation.left,ViewInformation.top,
               ViewInformation.right,ViewInformation.bottom,
               ViewInformation.clip);
   setwritemode(oldmode);
   setcolor(oldcolor);
  }

  if (fMouseInstall)
  {
     //Reg.w.ax=0x20;                    /* Enable Mouse */
     //int86(0x33,&Reg,&Reg);
     Reg.w.ax=0x1;                     /* Show Mouse */
     int386(0x33,&Reg,&Reg);
  }
}

void MouseHidden(void)
{
  union REGS Reg;

  //if (ActiveWindow==1&&GlobalBoxHeadHandle>0) fGuideLine=1;
  //         else fGuideLine=0;

  if (fMouseInstall)
  {
     Reg.w.ax=0x2;                     /* Hide Mouse */
     int386(0x33,&Reg,&Reg);
     //Reg.w.ax=0x1f;                    /* Disable Mouse */
     //int86(0x33,&Reg,&Reg);
  }

  if (fGuideLine)
  {
   struct viewporttype ViewInformation;
   int oldcolor=getcolor();
   int oldmode=getwritemode();
   getviewsettings(&ViewInformation);

   setviewport(0,0,getmaxx(),getmaxy(),0);
   setwritemode(XOR_PUT);
   setcolor(14);

   line(0,300,639,300);

   setviewport(ViewInformation.left,ViewInformation.top,
               ViewInformation.right,ViewInformation.bottom,
               ViewInformation.clip);
   setwritemode(oldmode);
   setcolor(oldcolor);
  }
}

static unsigned short MouseMask[MAXMOUSESHAPE*32] = {
        0x3FFF,0x1FFF,0x0FFF,0x07FF,0x03FF,0x01FF,   // ARROW   1,1
        0x00FF,0x007F,0x003F,0x001F,0x01FF,0x10FF,
        0x30FF,0xF87F,0xF87F,0xF87F,
        0x0000,0x4000,0x6000,0x7000,0x7800,0x7C00,
        0x7E00,0x7F00,0x7F80,0x7C00,0x6C00,0x4600,
        0x0600,0x0300,0x0300,0x0000,

        0xe1ff, 0xe1ff, 0xe1ff, 0xe1ff,              // Finger 4,1
        0xe000, 0xe000, 0xe000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0c00, 0x0c00, 0x0c00,
        0x0c00, 0x0db6, 0x0db6, 0x0db6,
        0x6db6, 0x6ffe, 0x6ffe, 0x6ffe,
        0x7ffe, 0x7ffe, 0x0000, 0x0000,

        0xffff, 0xffff, 0xffff, 0xffff,             // caret cursor 6,7
        0xffff, 0xffff, 0xffff, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0880, 0x0500, 0x0200,
        0x0200, 0x0200, 0x0200, 0x0200,
        0x0200, 0x0200, 0x0200, 0x0200,
        0x0200, 0x0500, 0x0880, 0x0000,

        0xfe77, 0xe423, 0xc003, 0xc003,            // browse hand 8,8
        0xe004, 0xe000, 0x8000, 0x0001,
        0x8007, 0x8007, 0xe00f, 0xf00f,
        0xf81f, 0xffff, 0xffff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0188, 0x1998, 0x1998,
        0x0db0, 0x0db2, 0x07f6, 0x67fc,
        0x7ff8, 0x1ff0, 0x07e0, 0x07e0,
        0x0000, 0x0000, 0x0000, 0x0000,

        0xfeff, 0xfc7f, 0xf83f, 0xf01f,                 // move arrow
        0xe00f, 0xc6c7, 0x86c3, 0x0001,
        0xc6c7, 0xc6c7, 0xe00f, 0xf01f,
        0xf83f, 0xfc7f, 0xfeff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0100, 0x0380, 0x07c0,
        0x0100, 0x1110, 0x3118, 0x7ffc,
        0x3118, 0x1110, 0x0100, 0x07c0,
        0x0380, 0x0100, 0x0000, 0x0000,

        0xffff, 0xf8ff, 0xe03f, 0xc01f,        // zoom
        0x800f, 0x800f, 0x800f, 0x800f,
        0xc01f, 0xc01f, 0xf00f, 0xffc7,
        0xffe1, 0xffe0, 0xfff0, 0xfff8,
        /* Cursor Mask */
        0x0000, 0x0000, 0x0700, 0x1fc0,
        0x38e0, 0x3f60, 0x3f60, 0x3fe0,
        0x1fc0, 0x0f80, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000,

        0xffbf, 0xffcf, 0xe003, 0xffcf,       // moving car
        0xffbf, 0xe03f, 0xc01f, 0x800f,
        0x0001, 0x0001, 0x0000, 0x0000,
        0x8001, 0xc3c3, 0xe7e7, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x1fc0, 0x3660,
        0x6430, 0x4c3c, 0x7ffe, 0x7ffe,
        0x1c38, 0x0000, 0x0000, 0x0000,

        0xffff, 0xffff, 0xffff, 0xffff,        // link
        0xffff, 0x8421, 0x0000, 0x318c,
        0x0000, 0x0000, 0x8421, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x3bdc, 0x4422,
        0x4422, 0x3bdc, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000,

        0xffff, 0xffff, 0xf7df, 0xfbbf,        // unlink
        0xffff, 0xcff3, 0x03c0, 0x318c,
        0x03c0, 0x03c0, 0xcff3, 0xffff,
        0xf7df, 0xefef, 0xffff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x366c, 0x4c32,
        0x4c32, 0x366c, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000,

        0xffff, 0xffff, 0xffff, 0xffff,        //hresize
        0xe3c7, 0xc3c3, 0x8001, 0x0000,
        0x8001, 0x8001, 0xc3c3, 0xe3c7,
        0xffff, 0xffff, 0xffff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0000, 0x0000, 0x0000,
        0x1c38, 0x2424, 0x47e2, 0x8001,
        0x8001, 0x47e2, 0x2424, 0x1c38,
        0x0000, 0x0000, 0x0000, 0x0000,

//      0xfeff, 0xfc7f, 0xf83f, 0xf01f,         // vresize
//      0xe00f, 0xc007, 0xe00f, 0xf87f,
//      0xe00f, 0xe00f, 0xc007, 0xe00f,
//      0xf01f, 0xf83f, 0xfc7f, 0xfeff,
        /* Cursor Mask */
//      0x0000, 0x0100, 0x0380, 0x07c0,
//      0x0fe0, 0x1ff0, 0x0380, 0x0380,
//      0x0380, 0x0380, 0x1ff0, 0x0fe0,
//      0x07c0, 0x0380, 0x0100, 0x0000,

        0xfe7f, 0xfc3f, 0xf81f, 0xf00f,        // vresize
        0xf00f, 0xf00f, 0xfc3f, 0xfc3f,
        0xfc3f, 0xfc3f, 0xf00f, 0xf00f,
        0xf00f, 0xf81f, 0xfc3f, 0xfe7f,
        /* Cursor Mask */
        0x0180, 0x0240, 0x0420, 0x0810,
        0x0810, 0x0e70, 0x0240, 0x0240,
        0x0240, 0x0240, 0x0e70, 0x0810,
        0x0810, 0x0420, 0x0240, 0x0180,

//      0xf00f, 0xe7e7, 0xcff3, 0xcff3,       // lock
//      0xcff3, 0xcff3, 0xcff3, 0x0000,
//      0x0000, 0x0000, 0x0000, 0x0000,
//      0x0000, 0x0000, 0x0000, 0x0000,
        /* Cursor Mask */
//      0x0000, 0x0000, 0x0000, 0x0000,
//      0x0000, 0x0000, 0x0000, 0x0000,
//      0x0000, 0x0000, 0x0180, 0x0180,
//      0x03c0, 0x0180, 0x0000, 0x0000,


       0xffff, 0xffff, 0xfc3f, 0xf3cf,          //Lock cursor
       0xeff7, 0xeff7, 0xeff7, 0xc003,
       0xc3c3, 0xc3c3, 0xc3c3, 0xc183,
       0xc003, 0xffff, 0xffff, 0xffff,
       /* Cursor Mask */
       0x0000, 0x0000, 0x0000, 0x0000,
       0x0000, 0x0000, 0x0000, 0x0000,
       0x0000, 0x0000, 0x0000, 0x0000,
       0x0000, 0x0000, 0x0000, 0x0000,


//      0xffff, 0x807f, 0x803f, 0x807f,         // resize 1
//      0x80ff, 0x807f, 0x803b, 0x8011,
//      0xdc01, 0xdc01, 0xfe01, 0xff01,
//      0xfe01, 0xfc01, 0xfe01, 0xffff,
        /* Cursor Mask */
//      0x0000, 0x0000, 0x3f80, 0x3f00,
//      0x3e00, 0x3f00, 0x3f80, 0x37c4,
//      0x23ec, 0x01fc, 0x00fc, 0x007c,
//      0x00fc, 0x01fc, 0x0000, 0x0000,

        0xffff, 0xffff, 0xc03f, 0xc03f,         // resize 1
        0xc07f, 0xc0ff, 0xc073, 0xc023,
        0xce03, 0xce03, 0xff03, 0xfe03,
        0xfc03, 0xfc03, 0xffff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0000, 0x3fc0, 0x2040,
        0x2080, 0x2100, 0x208c, 0x2454,
        0x2a24, 0x3104, 0x0084, 0x0104,
        0x0204, 0x03fc, 0x0000, 0x0000,


//      0xffff, 0xfe01, 0xfc01, 0xfe01,         // resize 2
//      0xff01, 0xfe01, 0xdc01, 0x8801,
//      0x803b, 0x803b, 0x807f, 0x80ff,
//      0x807f, 0x803f, 0x807f, 0xffff,
        /* Cursor Mask */
//      0x0000, 0x0000, 0x01fc, 0x00fc,
//      0x007c, 0x00fc, 0x01fc, 0x23ec,
//      0x37c4, 0x3f80, 0x3f00, 0x3e00,
//      0x3f00, 0x3f80, 0x0000, 0x0000,

        0xffff, 0xffff, 0xfc03, 0xfc03,         // resize 2
        0xfe03, 0xff03, 0xce03, 0xc403,
        0xc073, 0xc073, 0xc0ff, 0xc07f,
        0xc03f, 0xc03f, 0xffff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0000, 0x03fc, 0x0204,
        0x0104, 0x0084, 0x3104, 0x2a24,
        0x2454, 0x208c, 0x2100, 0x2080,
        0x2040, 0x3fc0, 0x0000, 0x0000 ,

        0xffbf | 0x5555, 0xffcf | 0xaaaa, 0xe003 | 0x5555, 0xffcf | 0xaaaa,       // moving car 50%
        0xffbf | 0x5555, 0xe03f | 0xaaaa, 0xc01f | 0x5555, 0x800f | 0xaaaa,
        0x0001 | 0x5555, 0x0001 | 0xaaaa, 0x0000 | 0x5555, 0x0000 | 0xaaaa,
        0x8001 | 0x5555, 0xc3c3 | 0xaaaa, 0xe7e7 | 0x5555, 0xffff | 0xaaaa,
        /* Cursor Mask */
        0x0000 | 0x5555, 0x0000 | 0xaaaa, 0x0000 | 0x5555, 0x0000 | 0xaaaa,
        0x0000 | 0x5555, 0x0000 | 0xaaaa, 0x1fc0 | 0x5555, 0x3660 | 0xaaaa,
        0x6430 | 0x5555, 0x4c3c | 0xaaaa, 0x7ffe | 0x5555, 0x7ffe | 0xaaaa,
        0x1c38 | 0x5555, 0x0000 | 0xaaaa, 0x0000 | 0x5555, 0x0000 | 0xaaaa,

        0xffff, 0xffff, 0xffff, 0xffff,         // cup with word
        0xffff, 0xffff, 0xffff, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff,
        /* Cursor Mask */
        0x3f80, 0xe0e0, 0x8060, 0xe0fc,
        0xbfe2, 0xbffa, 0xbfea, 0xbfca,
        0xbfca, 0xbfd2, 0xbfcc, 0xbfd0,
        0xdfa0, 0x4020, 0x3fc0, 0x0000,

        0xffff, 0xffff, 0xffff, 0xffff,         // pouring cup
        0xffff, 0xffff, 0xffff, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0188, 0x02d4, 0x0462,
        0x082d, 0x1015, 0x7fff, 0x5ffc,
        0x07fe, 0x43fe, 0x41fc, 0x00f8,
        0x4000, 0x4000, 0x0000, 0x0000,


       0xffff, 0xffff, 0xffff, 0xffff,          // finger 2
       0x801f, 0x0007, 0x8003, 0xf800,
       0xfc00, 0xfc00, 0xfc00, 0xfe00,
       0xfe00, 0xfff8, 0xffff, 0xffff,
       /* Cursor Mask */
       0x0000, 0x0000, 0x0000, 0x0000,
       0x0000, 0x7fe0, 0x0038, 0x03b8,
       0x001a, 0x01da, 0x001a, 0x00da,
       0x0002, 0x0000, 0x0000, 0x0000,




        0xffff, 0xffff, 0xffff, 0xffff,         // cross
        0xffff, 0xffff, 0xffff, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff,
        /* Cursor Mask */
        0x0000, 0x0100, 0x0100, 0x07c0,
        0x0920, 0x1110, 0x1110, 0x7ffc,
        0x1110, 0x1110, 0x0920, 0x07c0,
        0x0100, 0x0100, 0x0000, 0x0000,

        0xffff, 0xffff, 0xfff7, 0xffe3,         // rotate
        0xffc1, 0xff80, 0xffe2, 0xffe3,
        0xfdc3, 0xfdc3, 0xf887, 0xf007,
        0xe00f, 0xf07f, 0xf8ff, 0xfcff,
        /* Cursor Mask */
        0x0000, 0x0000, 0x0000, 0x0008,
        0x001c, 0x002a, 0x0008, 0x0008,
        0x0008, 0x0010, 0x0210, 0x0460,
        0x0f80, 0x0400, 0x0200, 0x0000,

        0xffff, 0xe007, 0xe007, 0xf00f,         // busy
        0xf81f, 0xfc3f, 0xfe7f, 0xfe7f,
        0xfc3f, 0xfc3f, 0xf81f, 0xf00f,
        0xe007, 0xc003, 0x8001, 0x8001,
        /* Cursor Mask */
        0x7ffe, 0x7ffe, 0x3ffc, 0x1ff8,
        0x0810, 0x0420, 0x0240, 0x0000,
        0x0240, 0x0660, 0x0ff0, 0x1e78,
        0x3e7c, 0x4002, 0x8001, 0x8001,
  };

static int HotSpot[MAXMOUSESHAPE][2] = {
       {1,1}, {4,1}, {6,7}, {8,8}, {7,7},
       {7,7}, {7,7}, {7,7}, {7,7}, {7,7},
       {7,8}, {7,8}, {7,8}, {7,8}, {7,8},
       {5,7}, {5,7}, {0,5}, {7,7}, {7,8},
       {7,6},
};

static int MouseCursor=0;

void MouseSetGraph(int MouseType)
{
  union REGS Reg;
  struct SREGS SReg;

  if (!fMouseInstall)
     return;

  if (MouseType == MouseCursor) return;
  if (MouseType>=MAXMOUSESHAPE) return;

  Reg.w.bx = HotSpot[MouseType][0];
  Reg.w.cx = HotSpot[MouseType][1];
  MouseCursor = MouseType;

  segread(&SReg);
  Reg.x.edx=FP_OFF(&MouseMask[MouseType*32]);
  SReg.es=FP_SEG(&MouseMask[MouseType*32]);
  Reg.w.ax=0x9;                 /* Set Graph Mouse Pointer */
  int386x(0x33,&Reg,&Reg,&SReg);
}

void MouseMoveTo(int X,int Y)
{
  union REGS Reg;

  if (fMouseInstall)
  {
     Reg.w.cx=X;
     Reg.w.dx=Y;
     Reg.w.ax=0x4;                     /* Set Mouse Position */
     int386(0x33,&Reg,&Reg);
  }
}

void MouseGetPosition(int *X,int *Y)
{
  union REGS Reg;

  if (fMouseInstall)
  {
     Reg.w.ax=0x3;                     /* Get Mouse Position */
     int386(0x33,&Reg,&Reg);
     *X=Reg.w.cx;
     *Y=Reg.w.dx;
  }
  else
     *X=*Y=-1;
}

static void MouseEnd (void) /* Dummy function so we can */
{                   /* calculate size of code to lock */
}                   /* (MouseEnd - MouseDeal) */


/*-------
extern volatile   long MouseDownTime;
extern volatile   unsigned short MouseButton;
extern volatile   int UserIntSign;
----------*/

int LockMouseMemory()
{
  if(!lock_region((void near *) MouseDeal,
       (char *) MouseEnd - (char near *) MouseDeal)
     ||!lock_region((void *) HardwareQueue, sizeof(HardwareQueue) )
     ||!lock_region((void *) &HardQueueHead, sizeof(HardQueueHead) )
     ||!lock_region((void *) &HardQueueTail, sizeof(HardQueueTail) )
     ||!lock_region((void *) MouseMask, sizeof(MouseMask) )
/*-------
     ||!lock_region((void near *) TimerDeal, (char *)TimerEndProc-(char *)TimerDeal)
     ||!lock_region((void *) &MouseDownTime, sizeof(MouseDownTime) )
     ||!lock_region((void *) &MouseButton, sizeof(MouseButton) )
     ||!lock_region((void *) &UserIntSign, sizeof(UserIntSign) )
     ||!lock_region((void *)MessageBeginProc, (char *)MessageEndProc-(char *)MessageBeginProc)
-------*/
    )
     Error(MOUSEINIT);

  ReturnOK();
}

int UnlockMouseMemory()
{
  if(!unlock_region((void near *) MouseDeal,
       (char *) MouseEnd - (char near *) MouseDeal)
     ||!unlock_region((void *) HardwareQueue, sizeof(HardwareQueue) )
     ||!unlock_region((void *) &HardQueueHead, sizeof(HardQueueHead) )
     ||!unlock_region((void *) &HardQueueTail, sizeof(HardQueueTail) )
     ||!unlock_region((void *) MouseMask, sizeof(MouseMask) )
/*-------
     ||!unlock_region((void near *) TimerDeal, (char *)TimerEndProc-(char *)TimerDeal)
     ||!unlock_region((void *) &MouseDownTime, sizeof(MouseDownTime) )
     ||!unlock_region((void *) &MouseButton, sizeof(MouseButton) )
     ||!unlock_region((void *) &UserIntSign, sizeof(UserIntSign) )
     ||!unlock_region((void *)MessageBeginProc, (char *)MessageEndProc-(char *)MessageBeginProc)
--------*/
    )
     Error(MOUSEINIT);

  ReturnOK();
}

int MouseConstruct(int maxx,int maxy)
{
  union REGS Reg;
  struct SREGS SReg;
  void (far *function_ptr)();

  segread(&SReg);
  Reg.w.ax=0x3533;                     /* Get Mouse Vector */
  intdosx(&Reg,&Reg,&SReg);

  if (SReg.es==0&&Reg.x.ebx==0)        /* Install? */
     Error(MOUSEINIT);

  Reg.w.ax=0;                          /* Reset Mouse */
  int386(0x33,&Reg,&Reg);

  if (Reg.w.ax!=0xffff)                /* Mouse is OK? */
     Error(MOUSEINIT);

  fMouseInstall=TRUE;

        /* set mouse x range */
  Reg.w.ax = 0x7;
  Reg.w.cx = 0x0;
  Reg.w.dx = maxx;
  int386(0x33,&Reg,&Reg);

        /* set mouse y range */
  Reg.w.ax = 0x8;
  Reg.w.cx = 0x0;
  Reg.w.dx = maxy;
  int386(0x33,&Reg,&Reg);

            /* install click watcher */
  Reg.w.ax = 0xC;
  Reg.w.cx = 0x1f;
  function_ptr = ( void(*)(void) ) MouseDeal;
  Reg.x.edx= FP_OFF (function_ptr);
  SReg.es = FP_SEG (function_ptr);
  int386x(0x33,&Reg,&Reg,&SReg);

  MouseShow();

  ReturnOK();
}

void MouseDestruct(void)
{
  union REGS Reg;

  if (fMouseInstall)
  {
     MouseHidden();

    /* check installation again (to clear watcher) */
     Reg.w.ax = 0;
     int386(0x33,&Reg,&Reg);
     fMouseInstall=FALSE;
  }
}
