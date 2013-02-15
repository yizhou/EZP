#define __MEMORYDEBUG__

#ifdef __MEMORYDEBUG__
  #include "../kernl/handlec.c"
#else

#define __MEMORY_C__

#define DEBUG
 #define ReleaseVersion

//#include <dos.h>
//#include "syshare.h"
//#include "cmodel.h"
//#include "../kernl/handle.h"

#define OK              0
#define ERROR           -1

#define GETEMSSTATUS    0x40
#define GETSEGMENT      0x41
#define GETPAGENUM      0x42
#define MALLOCPAGE      0x43
#define ACTIVEPAGE      0x44
#define CLOSEPAGE       0x44
#define FREEPAGE        0x45
#define LIM             0x67

#define MINALLOCSIZE    512            // 2**9
#define MAXALLOCHANDLE  8192           // 2**13

#define EMSPAGESIZE     16384l
#define INTERNLPAGE     3l
#define USERPAGE        0
#define MINNEEDMEMORY   64

/*
   Note:

     First, we initial memory ( Alloc Internl Block (64K) and all other EMS
   as user memory ) , construct a free link, link head is 1 ( 0 is always
   assume NULL and can not alloc ),
     InternlHandle[0] =
          { 0, 0, 0x8000 (Used), 0 };
     InternlHandle[1] = ( NowFreeLinkHead )
          { 0 (AbsoluteAddress) , MAXMEMORY (AllocSize),
            2 (LockCount), 0 (LowSegment) };
     InternlHandle[2] =
          { MAXMEMORY (AbsoluteAddress), 0 (AllocSize),
            3 (LockCount), 0 (LowSegment) };
     others set to null ( LockCount set to index+1 )

     Second, when do alloc, we search the free link to get a suitable block,
   and reconstruct free link; when do free, we search free link to merge the
   memory to free link

     Last, when program is ending, we free all EMS memory.
 */

/*
   Data struct:
     AbsoluteAddress: Block address of EMS / MINALLOCSIZE ( 512 - 32M )
     AllocSize: Block alloc size / MINALLOCSIZE( 512 - 32M )
     LockCount:
                Bit    15 -- Handle use sign
                When Bit 15 == 1:
                  Bit  0-12 -- Lock count
                      13-14 -- privilege of lock
                When Bit 15 == 0:
                  Bit  0-12 -- next free link

     LowSegment: When block is lock in conventional memory, it record the
                 segment of this memory block, it's offset address assume
                 to 4
*/

typedef struct tagInternlHandles
{
  unsigned short AbsoluteAddress;
  unsigned short AllocSize;
  unsigned short LockCount;
  unsigned short LowSegment;
} InternlHandles;

#define InternlHandleGetAddress(II) ((II).AbsoluteAddress)
#define InternlHandleGetSize(II) ((II).AllocSize)
#define InternlHandleGetCount(II) ((II).LockCount)
#define InternlHandleGetSegment(II) ((II).LowSegment)

#define InternlHandleSetAddress(II,AA) ((II).AbsoluteAddress=AA)
#define InternlHandleSetSize(II,SS) ((II).AllocSize=SS)
#define InternlHandleSetCount(II,CC) ((II).LockCount=CC)
#define InternlHandleSetSegment(II,SS) ((II).LowSegment=SS)

#define InternlHandleGetNext(II) ((II).LockCount&(MAXALLOCHANDLE-1))
#define InternlHandleGetLockCount(II) ((II).LockCount&0x1fff)
#define InternlHandleGetUsed(II) ((II).LockCount&0x8000)
#define InternlHandleGetClass(II) ((((II).LockCount&0x6000)>>13)&3)

#define InternlHandleSetNext(II,NN) ((II).LockCount=((NN)&(MAXALLOCHANDLE-1)))
#define InternlHandleSetLockCount(II,CC) { (II).LockCount&=0xe000; \
                                          (II).LockCount|=((CC)&0x1fff); }
#define InternlHandleSetUsed(II)  ((II).LockCount|=0x8000)
#define InternlHandleSetCanUse(II) ((II).LockCount&=0x7fff)
#define InternlHandleSetClass(II,CC) { (II).LockCount&=~0x6000; \
                                       (II).LockCount|=(((CC)&3)<<13); }

#define MAKEINTERNLBLOCK(MM) (MM&0x7ff)

#define EMSPAGETOHANDLESIZE(PP) ((PP)*(EMSPAGESIZE/MINALLOCSIZE))
#define HANDLESIZETOEMSPAGE(HH) ((HH)/(EMSPAGESIZE/MINALLOCSIZE))

short InternlEMSHandle=0;              /*
                                          Point to EMS which saves
                                          InternlHandles
                                        */
char NowEMSHandleIndex=-1;             /*
                                          Point to the block (0-3) now keep in
                                          conventional's InternlHandles
                                        */
short NowFreeLinkHead;                 /* Save a free link's head */
short UserEMSHandle=0;                 /* User's EMS Handle */

unsigned short UserEMSFrame;           /* UserEMSFrame EMS Frame segment */

void OutOfMemorySave();

#ifdef DEBUG

static void PromptOutOfMemory(void)
{
  printf("Error: Out of memory!\n");
  asm int 3;
}

static void PromptOutofOrder(void)
{
  //printf("Warning: Lock and unlock out of order!\n");
  //asm int 3;
}

#endif

short GetEmsStatus(void)
{
  union REGS regs;                     // ah == 0 is sucess

  regs.h.ah=GETEMSSTATUS;
  int86(LIM,&regs,&regs);
  if (regs.h.ah==0)
     return (OK);
  else
     return (ERROR);
}

short GetEmsSegment(void)
{
  union REGS regs;                     // bx=segment

  regs.h.ah=GETSEGMENT;
  int86(LIM,&regs,&regs);
  if (regs.h.ah==0)
     return (regs.x.bx);
  else
     return(ERROR);
}

short GetEmsPageNum(void)
{
  union REGS regs;                     // bx=avail page num, each is 16KB

  regs.h.ah=GETPAGENUM;
  int86(LIM,&regs,&regs);
  if (regs.h.ah==0)
     return (regs.x.bx);
  else
     return(ERROR);
}

short MallocEmsPage(int num)
{
  union REGS regs;                     // bx=how      dx=handle

  regs.h.ah=MALLOCPAGE;
  regs.x.bx=num;
  int86(LIM,&regs,&regs);
  if (regs.h.ah==0)
     return (regs.x.dx);
  else
     return(ERROR);
}

short ActiveEmsPage(int physicspagenum, int logicpagenum, int handle)
{
  union REGS regs;                     // al=physics bx=logic

  regs.h.ah=ACTIVEPAGE;
  regs.h.al=physicspagenum;
  regs.x.bx=logicpagenum;
  regs.x.dx=handle;
  int86(LIM,&regs,&regs);
  if (regs.h.ah==0)
     return (regs.x.dx);
  else
  {
     // asm int 3;                  // ????
     return(ERROR);
  }
}

short CloseEmsPage(int physicspagenum, int handle)
{
  union REGS regs;                     // bx=pagenum

  regs.h.ah=CLOSEPAGE;
  regs.h.al=physicspagenum;
  regs.x.bx=0xffff;
  regs.x.dx=handle;
  int86(LIM,&regs,&regs);
  if (regs.h.ah==0)
     return (OK);
  else
  {
     asm int 3;
     return (ERROR);
  }
}

short FreeEmsPage(int handle)
{
  union REGS regs;                     // bx=handle

  regs.h.ah=FREEPAGE;
  regs.x.dx=handle;
  int86(LIM,&regs,&regs);
  if (regs.h.ah==0)
     return (OK);
  else
     return (ERROR);
}

int DefaultLockBoradcast(unsigned short Handle,short Class,void huge *LockAddress)
{
  return(Handle);
}

int DefaultUnlockBoradcast(unsigned short Handle,short Class,void huge *LockAddress)
{
  return(1);
}

void DefaultNoLowMemoryBoradcast(void)
{
  return;
}

int EMSAlloc(long *AllocSize)          // By Handle size
{
  int EmsAllocSize;
  int EmsAllocHandle;

  EmsAllocSize=HANDLESIZETOEMSPAGE(*AllocSize);
  EmsAllocHandle=MallocEmsPage(EmsAllocSize);
  if (EmsAllocHandle==ERROR) {
     int pagen=GetEmsPageNum();  // if not enough memory, used the max left
     if(pagen==ERROR) return ERROR;
     *AllocSize=EMSPAGETOHANDLESIZE(pagen);
     EmsAllocSize=HANDLESIZETOEMSPAGE(*AllocSize);
     EmsAllocHandle=MallocEmsPage(EmsAllocSize);
  }
  return(EmsAllocHandle);
}

static InternlHandles huge *InternlEMSTmpLock(int EmsHandle)
{                                      // Only use for initial clear
  int i,TotalPage;

  TotalPage=HANDLESIZETOEMSPAGE(MAXALLOCHANDLE/MINALLOCSIZE
            *sizeof(InternlHandles));
  for (i=0;i<TotalPage;i++)
      if( ERROR== ActiveEmsPage(i,i,EmsHandle) )
         return(NULL);

  return(MK_FP(UserEMSFrame,0));
}

static InternlHandles huge *LockInternlBlock(int Handle)
{                                 // Use for map InternlHandles in page 3
  if ((Handle>>11)!=NowEMSHandleIndex)
  {
     if (NowEMSHandleIndex<4&&NowEMSHandleIndex>0)
        CloseEmsPage(INTERNLPAGE,InternlEMSHandle);
     NowEMSHandleIndex=(Handle>>11)&3;
     ActiveEmsPage(INTERNLPAGE,NowEMSHandleIndex,InternlEMSHandle);
  }
  return((InternlHandles huge *)MK_FP(UserEMSFrame,INTERNLPAGE*EMSPAGESIZE));
}

long EMSLock(InternlHandles huge *TobeLock,char huge *LowAddress)
{
  long LockSize,CopySize,i;
  unsigned short LockPage,FirstPage,StartAddress;
  char huge *MidLowAddress;
  char far *EmsFrameAddress;
  InternlHandles SourceHandleStruct;

  MemCpy(&SourceHandleStruct,TobeLock,sizeof(InternlHandles));
  LockSize=InternlHandleGetSize(SourceHandleStruct);
  LockSize=LockSize*(long)MINALLOCSIZE;

  /*---- LockPage:total ems pages to be locked,
        FirstPage::StartAddress : align to 16K edge
     ----------------------------------------------*/
  LockPage=InternlHandleGetAddress(SourceHandleStruct);
  FirstPage=HANDLESIZETOEMSPAGE(LockPage);
  StartAddress=(LockPage%(EMSPAGESIZE/MINALLOCSIZE))*MINALLOCSIZE;
  LockPage=(LockPage+(EMSPAGESIZE/MINALLOCSIZE)-1)/(EMSPAGESIZE/MINALLOCSIZE);
  EmsFrameAddress=MK_FP(UserEMSFrame,USERPAGE*EMSPAGESIZE);
  MidLowAddress=LowAddress;
  if (NowEMSHandleIndex<4&&NowEMSHandleIndex>=0&&INTERNLPAGE==USERPAGE)
  {
     CloseEmsPage(INTERNLPAGE,InternlEMSHandle);
     NowEMSHandleIndex=-1;
  }

  // Deal first page
  if (FirstPage!=LockPage)
  {
     ActiveEmsPage(USERPAGE,FirstPage,UserEMSHandle);
     if (LockSize>EMSPAGESIZE-StartAddress)
        CopySize=EMSPAGESIZE-StartAddress;
     else
        CopySize=LockSize;
     MemCpy(MidLowAddress,EmsFrameAddress+StartAddress,CopySize);
     i=CopySize;
     MidLowAddress+=CopySize;
  }
  else
     i=0;

  // Deal from next page to end page
  for (;i<LockSize;i+=EMSPAGESIZE,MidLowAddress+=EMSPAGESIZE,LockPage++)
  {
      ActiveEmsPage(USERPAGE,LockPage,UserEMSHandle);      // ??? ERROR
      if (LockSize-i>EMSPAGESIZE)
         CopySize=EMSPAGESIZE;
      else
         CopySize=LockSize-i;
      MemCpy(MidLowAddress,EmsFrameAddress,CopySize);
  }
  CloseEmsPage(USERPAGE,UserEMSHandle);
  return(LockSize);
}

int EMSUnlock(InternlHandles huge *TobeUnlock,char huge *LowAddress)
{
  long LockSize,i,CopySize;
  unsigned short LockPage,FirstPage,StartAddress;
  char huge *MidLowAddress;
  char far *EmsFrameAddress;
  InternlHandles SourceHandleStruct;

  if (FP_SEG(LowAddress)==0)
     asm int 3;

  MemCpy(&SourceHandleStruct,TobeUnlock,sizeof(InternlHandles));
  LockSize=InternlHandleGetSize(SourceHandleStruct);
  LockSize=LockSize*(long)MINALLOCSIZE;
  LockPage=InternlHandleGetAddress(SourceHandleStruct);
  FirstPage=HANDLESIZETOEMSPAGE(LockPage);
  StartAddress=(LockPage%(EMSPAGESIZE/MINALLOCSIZE))*MINALLOCSIZE;
  LockPage=(LockPage+(EMSPAGESIZE/MINALLOCSIZE)-1)/(EMSPAGESIZE/MINALLOCSIZE);
  EmsFrameAddress=MK_FP(UserEMSFrame,USERPAGE*EMSPAGESIZE);
  MidLowAddress=LowAddress;
  if (NowEMSHandleIndex<4&&NowEMSHandleIndex>0&&INTERNLPAGE==USERPAGE)
  {
     CloseEmsPage(INTERNLPAGE,InternlEMSHandle);
     NowEMSHandleIndex=-1;
  }

  // Deal first page
  if (FirstPage!=LockPage)
  {
     ActiveEmsPage(USERPAGE,FirstPage,UserEMSHandle);
     if (LockSize>EMSPAGESIZE-StartAddress)
        CopySize=EMSPAGESIZE-StartAddress;
     else
        CopySize=LockSize;
     MemCpy(EmsFrameAddress+StartAddress,MidLowAddress,CopySize);
     CloseEmsPage(USERPAGE,UserEMSHandle);
     i=CopySize;
     MidLowAddress+=CopySize;
  }
  else
     i=0;

  // Deal from next page to end page
  for (;i<LockSize;i+=EMSPAGESIZE,MidLowAddress+=EMSPAGESIZE,LockPage++)
  {
      ActiveEmsPage(USERPAGE,LockPage,UserEMSHandle);
      if (LockSize-i>EMSPAGESIZE)
         CopySize=EMSPAGESIZE;
      else
         CopySize=LockSize-i;
      MemCpy(EmsFrameAddress,MidLowAddress,CopySize);
      CloseEmsPage(USERPAGE,UserEMSHandle);
  }
  return(LockSize);
}

static int EMSMovetoEMS(int DestHandle,int SourceHandle)
{
  InternlHandles SourceHandleStruct,DestHandleStruct;
  InternlHandles huge *InternlHandle;//,*TmpHandle;
  int BlockHandle;
  int NowSourcePage,NowDestPage;
  int SourceStartAddress,DestStartAddress;
  long RealMoveSize,CopySize1,CopySize2,SourcePoint,DestPoint;
  unsigned char huge *EmsFrameAddress;

  InternlHandle=LockInternlBlock(DestHandle);
  BlockHandle=MAKEINTERNLBLOCK(DestHandle);
  MemCpy(&DestHandleStruct,&(InternlHandle[BlockHandle]),sizeof(InternlHandles));

  InternlHandle=LockInternlBlock(SourceHandle);
  BlockHandle=MAKEINTERNLBLOCK(SourceHandle);
  MemCpy(&SourceHandleStruct,&(InternlHandle[BlockHandle]),sizeof(InternlHandles));

  RealMoveSize=InternlHandleGetSize(SourceHandleStruct);
  if (RealMoveSize>InternlHandleGetSize(DestHandleStruct))
     RealMoveSize=InternlHandleGetSize(DestHandleStruct);

  if (InternlHandleGetSegment(SourceHandleStruct))
  {
     char huge *p;

     InternlHandleSetSize(DestHandleStruct,RealMoveSize);
     p=MK_FP(InternlHandleGetSegment(SourceHandleStruct),4);
     EMSUnlock(&DestHandleStruct,p);

     //InternlHandle=LockInternlBlock(SourceHandle);
     //BlockHandle=MAKEINTERNLBLOCK(SourceHandle);
     //InternlHandleSetSegment(InternlHandle[BlockHandle],0);
  }
  else
  {
     unsigned char TransportBuffer[EMSPAGESIZE];

     if (NowEMSHandleIndex<4&&NowEMSHandleIndex>0&&INTERNLPAGE==USERPAGE)
     {
        CloseEmsPage(INTERNLPAGE,InternlEMSHandle);
        NowEMSHandleIndex=-1;
     }

     RealMoveSize=RealMoveSize*(long)MINALLOCSIZE;

     NowSourcePage=InternlHandleGetAddress(SourceHandleStruct);
     SourceStartAddress=(NowSourcePage%(EMSPAGESIZE/MINALLOCSIZE))
                        *MINALLOCSIZE;
     NowSourcePage=HANDLESIZETOEMSPAGE(NowSourcePage);

     NowDestPage=InternlHandleGetAddress(DestHandleStruct);
     DestStartAddress=(NowDestPage%(EMSPAGESIZE/MINALLOCSIZE))
                        *MINALLOCSIZE;
     NowDestPage=HANDLESIZETOEMSPAGE(NowDestPage);

     EmsFrameAddress=MK_FP(UserEMSFrame,USERPAGE*EMSPAGESIZE);

     SourcePoint=0;
     DestPoint=0;

     while (DestPoint<RealMoveSize)
     {
       CopySize1=CopySize2=0;

       ActiveEmsPage(USERPAGE,NowSourcePage,UserEMSHandle);
       if (RealMoveSize-SourcePoint>EMSPAGESIZE-SourceStartAddress)
          CopySize1=EMSPAGESIZE-SourceStartAddress;
       else
          CopySize1=RealMoveSize-SourcePoint;
       MemCpy(TransportBuffer,EmsFrameAddress+SourceStartAddress,CopySize1);
       NowSourcePage++;
       SourcePoint+=CopySize1;

       if (SourceStartAddress&&SourcePoint<RealMoveSize)
       {
          ActiveEmsPage(USERPAGE,NowSourcePage,UserEMSHandle);
          if (RealMoveSize-SourcePoint>EMSPAGESIZE-CopySize1)
             CopySize2=EMSPAGESIZE-CopySize1;
          else
             CopySize2=RealMoveSize-SourcePoint;
          MemCpy(&TransportBuffer[CopySize1],EmsFrameAddress,CopySize2);
          SourcePoint+=CopySize2;
       }

       ActiveEmsPage(USERPAGE,NowDestPage,UserEMSHandle);
       if (RealMoveSize-DestPoint>EMSPAGESIZE-DestStartAddress)
          CopySize1=EMSPAGESIZE-DestStartAddress;
       else
          CopySize1=RealMoveSize-DestPoint;
       MemCpy(EmsFrameAddress+DestStartAddress,TransportBuffer,CopySize1);
       CloseEmsPage(USERPAGE,UserEMSHandle);
       NowDestPage++;
       DestPoint+=CopySize1;

       if (DestStartAddress&&DestPoint<RealMoveSize)
       {
          ActiveEmsPage(USERPAGE,NowDestPage,UserEMSHandle);
       //   if (RealMoveSize-DestPoint<EMSPAGESIZE)
          if (RealMoveSize-DestPoint>EMSPAGESIZE-CopySize1)
             CopySize2=EMSPAGESIZE-CopySize1;
          else
             CopySize2=RealMoveSize-DestPoint;
          MemCpy(EmsFrameAddress,&TransportBuffer[CopySize1],CopySize2);
          CloseEmsPage(USERPAGE,UserEMSHandle);
          DestPoint+=CopySize2;
        }
     }
  }
  ReturnOK();
}

static int HandleLinkSearchFree(unsigned short BlockSize,int *OldPrevFreeHandle)
{
  InternlHandles huge *InternlHandle;
  int BlockHandle,FreeNext;

  FreeNext=NowFreeLinkHead;
  *OldPrevFreeHandle=0;

  while (FreeNext)
  {
    InternlHandle=LockInternlBlock(FreeNext);
    BlockHandle=MAKEINTERNLBLOCK(FreeNext);

    if (InternlHandleGetSize(InternlHandle[BlockHandle])>=BlockSize)
       return(FreeNext);
    else
    {
       *OldPrevFreeHandle=FreeNext;
       FreeNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
    }
  }
  return(0);
}

static int HandleLinkSlipFree(int ToAlloc,unsigned short BlockSize,
                              char AllocType,int OldPrevFreeHandle)
{
  InternlHandles huge *InternlHandle;
  int BlockHandle,NewPrevFree,NewNextFree,NewNext;

  InternlHandle=LockInternlBlock(ToAlloc);
  BlockHandle=MAKEINTERNLBLOCK(ToAlloc);

  if (InternlHandleGetSize(InternlHandle[BlockHandle])>BlockSize)
  {                                    // Must slip free link item
     int FreeNext;
     unsigned short OldAddress,OldSize,OldNext;

     OldAddress=InternlHandleGetAddress(InternlHandle[BlockHandle]);
     OldSize=InternlHandleGetSize(InternlHandle[BlockHandle]);
     OldNext=InternlHandleGetNext(InternlHandle[BlockHandle]);

     FreeNext=NowFreeLinkHead;
     NewPrevFree=0;
     while (FreeNext)
     {                                 // Get a not using free link item
       InternlHandle=LockInternlBlock(FreeNext);
       BlockHandle=MAKEINTERNLBLOCK(FreeNext);

       if (InternlHandleGetAddress(InternlHandle[BlockHandle])==0
           &&InternlHandleGetSize(InternlHandle[BlockHandle])==0)
       {
          NewNextFree=InternlHandleGetNext(InternlHandle[BlockHandle]);
          InternlHandleSetAddress(InternlHandle[BlockHandle],
                                  OldAddress+BlockSize);
          InternlHandleSetSize(InternlHandle[BlockHandle],OldSize-BlockSize);
          if (FreeNext!=OldNext)
          {
             InternlHandleSetNext(InternlHandle[BlockHandle],OldNext);
             InternlHandle=LockInternlBlock(NewPrevFree);
             BlockHandle=MAKEINTERNLBLOCK(NewPrevFree);
             InternlHandleSetNext(InternlHandle[BlockHandle],NewNextFree);
          }
          if (OldPrevFreeHandle)
          {
             InternlHandle=LockInternlBlock(OldPrevFreeHandle);
             BlockHandle=MAKEINTERNLBLOCK(OldPrevFreeHandle);
             InternlHandleSetNext(InternlHandle[BlockHandle],FreeNext);
          }
          else                         // Slip free link head
             NowFreeLinkHead=FreeNext;
          break;
       }
       else
       {
          NewPrevFree=FreeNext;
          FreeNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
       }
     }
     if (!FreeNext)
        return(OUTOFMEMORY);
     else
     {
        InternlHandle=LockInternlBlock(ToAlloc);
        BlockHandle=MAKEINTERNLBLOCK(ToAlloc);
        InternlHandleSetUsed(InternlHandle[BlockHandle]);
        InternlHandleSetSize(InternlHandle[BlockHandle],BlockSize);
        InternlHandleSetLockCount(InternlHandle[BlockHandle],0);
        InternlHandleSetClass(InternlHandle[BlockHandle],AllocType);
     }
  }
  else
  {
     NewNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
     InternlHandleSetUsed(InternlHandle[BlockHandle]);
     InternlHandleSetSize(InternlHandle[BlockHandle],BlockSize);
     InternlHandleSetLockCount(InternlHandle[BlockHandle],0);
     InternlHandleSetClass(InternlHandle[BlockHandle],AllocType);

     if (OldPrevFreeHandle==0)
        NowFreeLinkHead=NewNext;
     else
     {
        InternlHandle=LockInternlBlock(OldPrevFreeHandle);
        BlockHandle=MAKEINTERNLBLOCK(OldPrevFreeHandle);
        InternlHandleSetNext(InternlHandle[BlockHandle],NewNext);
     }
  }
  ReturnOK();
}

static void FreeLinkAppend(int ToFree)
{
  InternlHandles huge *InternlHandle;
  int BlockHandle,FreeNext,OldNext;

  InternlHandle=LockInternlBlock(ToFree);
  BlockHandle=MAKEINTERNLBLOCK(ToFree);
  MemSet(&(InternlHandle[BlockHandle]),0,sizeof(InternlHandles));

  FreeNext=NowFreeLinkHead;
  while (FreeNext)
  {
    InternlHandle=LockInternlBlock(FreeNext);
    BlockHandle=MAKEINTERNLBLOCK(FreeNext);
    if (!InternlHandleGetAddress(InternlHandle[BlockHandle])
        &&!InternlHandleGetSize(InternlHandle[BlockHandle]))
       break;
    else
       FreeNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
  }                                    // search to the end of free link
  if (FreeNext)
  {
     OldNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
     InternlHandleSetNext(InternlHandle[BlockHandle],ToFree);
     InternlHandle=LockInternlBlock(ToFree);
     BlockHandle=MAKEINTERNLBLOCK(ToFree);
     InternlHandleSetNext(InternlHandle[BlockHandle],OldNext);
  }
  else
     NowFreeLinkHead=ToFree;
}

static void ReconstructFreeLink(int ToFree)
{
  InternlHandles huge *InternlHandle;
  int BlockHandle,FreeNext;
  unsigned short OldAddress,OldSize,OldPrevFree;
  char MergeSign=0;                    /*
                                          0 -- Can't merge
                                          1 -- Prev-merge
                                          2 -- Post-merge
                                          3 -- Middle-merge
                                        */

  InternlHandle=LockInternlBlock(ToFree);
  BlockHandle=MAKEINTERNLBLOCK(ToFree);

  OldAddress=InternlHandleGetAddress(InternlHandle[BlockHandle]);
  OldSize=InternlHandleGetSize(InternlHandle[BlockHandle]);

  OldPrevFree=0;
  FreeNext=NowFreeLinkHead;
  while (FreeNext&&!MergeSign)
  {
    InternlHandle=LockInternlBlock(FreeNext);
    BlockHandle=MAKEINTERNLBLOCK(FreeNext);

    if (InternlHandleGetAddress(InternlHandle[BlockHandle])>OldAddress
        ||!InternlHandleGetAddress(InternlHandle[BlockHandle]))
    {
       if (InternlHandleGetAddress(InternlHandle[BlockHandle])==
           OldAddress+OldSize)         // Search find can post-merge
       {
          InternlHandleSetAddress(InternlHandle[BlockHandle],
            InternlHandleGetAddress(InternlHandle[BlockHandle])-OldSize);
          InternlHandleSetSize(InternlHandle[BlockHandle],
            InternlHandleGetSize(InternlHandle[BlockHandle])+OldSize);
          MergeSign=2;
       }
       break;          // Search end, can't merge, then insert
    }
    if (InternlHandleGetAddress(InternlHandle[BlockHandle])
       +InternlHandleGetSize(InternlHandle[BlockHandle])==OldAddress)
    {                                  // can prev-merge
       InternlHandleSetSize(InternlHandle[BlockHandle],
         InternlHandleGetSize(InternlHandle[BlockHandle])+OldSize);
       MergeSign=1;
                                       // Search for middle-merge
       OldAddress=InternlHandleGetAddress(InternlHandle[BlockHandle]);
       OldSize=InternlHandleGetSize(InternlHandle[BlockHandle]);
       OldPrevFree=FreeNext;
       if (!InternlHandleGetNext(InternlHandle[BlockHandle]))
         break;
       FreeNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
       InternlHandle=LockInternlBlock(FreeNext);
       BlockHandle=MAKEINTERNLBLOCK(FreeNext);
       if (InternlHandleGetAddress(InternlHandle[BlockHandle])==
           OldAddress+OldSize)         // Search can middle-merge
       {
          int OldNextFree;

          OldSize=InternlHandleGetSize(InternlHandle[BlockHandle]);
          OldNextFree=InternlHandleGetNext(InternlHandle[BlockHandle]);
          InternlHandle=LockInternlBlock(OldPrevFree);
          BlockHandle=MAKEINTERNLBLOCK(OldPrevFree);
          InternlHandleSetSize(InternlHandle[BlockHandle],
            InternlHandleGetSize(InternlHandle[BlockHandle])+OldSize);
          InternlHandleSetNext(InternlHandle[BlockHandle],OldNextFree);
          MergeSign=3;
          FreeLinkAppend(FreeNext);
       }
       break;
    }
    else
    {
       OldPrevFree=FreeNext;
       FreeNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
    }
  }   /* while */

  if (MergeSign)
     FreeLinkAppend(ToFree);
  else                                 // Search end, can't merge, then insert
  {
     int OldNextFree;

     if (OldPrevFree==0)
     {                                 // It Is prev free link head
        InternlHandle=LockInternlBlock(ToFree);
        BlockHandle=MAKEINTERNLBLOCK(ToFree);
        InternlHandleSetNext(InternlHandle[BlockHandle],NowFreeLinkHead);
        NowFreeLinkHead=ToFree;
     }
     else
     {
        InternlHandle=LockInternlBlock(OldPrevFree);
        BlockHandle=MAKEINTERNLBLOCK(OldPrevFree);
        OldNextFree=InternlHandleGetNext(InternlHandle[BlockHandle]);
        InternlHandleSetNext(InternlHandle[BlockHandle],ToFree);
        InternlHandle=LockInternlBlock(ToFree);
        BlockHandle=MAKEINTERNLBLOCK(ToFree);
        InternlHandleSetNext(InternlHandle[BlockHandle],OldNextFree);
     }
  }
} /* ReconstructFreeLink */

void *HandleInternlLock(int Handle,int BoradcastSign)
{
  InternlHandles huge *InternlHandle;
  int BlockHandle,Count,Class;
  char huge *LockMemory;
  long LockSize;
  unsigned short LockSeg;

  if (!Handle)
  {
     asm int 3;
     return(NULL);
  }

  InternlHandle=LockInternlBlock(Handle);
  BlockHandle=MAKEINTERNLBLOCK(Handle);

  Count=InternlHandleGetLockCount(InternlHandle[BlockHandle]);
  if (Count||InternlHandleGetSegment(InternlHandle[BlockHandle]))
  {
     #ifdef DEBUG
     if (Count==8191)
        PromptOutofOrder();
     #endif
     if (BoradcastSign)
     {
        Count++;
        InternlHandleSetLockCount(InternlHandle[BlockHandle],Count);
     }
     LockSeg=InternlHandleGetSegment(InternlHandle[BlockHandle]);
  }
  else
  {
     LockSize=InternlHandleGetSize(InternlHandle[BlockHandle]);
     LockSize=LockSize*(long)MINALLOCSIZE;
     LockMemory=MemAlloc(LockSize);
     if (LockMemory==NULL)
     {
        int i,j;

        for (i=0;i<=3;i++)
        {
            for (j=0;j<MAXALLOCHANDLE;j++,BlockHandle++)
            {
                if (!(j&((MAXALLOCHANDLE>>2)-1)))
                {
                   InternlHandle=LockInternlBlock(j);
                   BlockHandle=0;
                }
                if (InternlHandleGetLockCount(InternlHandle[BlockHandle])==0
                    &&InternlHandleGetSegment(InternlHandle[BlockHandle])
                    &&InternlHandleGetClass(InternlHandle[BlockHandle])==i)
                {
                   LockMemory=MK_FP(InternlHandleGetSegment(
                                    InternlHandle[BlockHandle]),4);
                   Class=InternlHandleGetClass(InternlHandle[BlockHandle]);
                   Class=UnlockBoradcast(j,Class,LockMemory);
                   if (Class)
                      EMSUnlock(&(InternlHandle[BlockHandle]),LockMemory);
                   MemFree(LockMemory);
                   InternlHandle=LockInternlBlock(j);
                   InternlHandleSetSegment(InternlHandle[BlockHandle],0);
                   if ((LockMemory=MemAlloc(LockSize))!=NULL)
                      break;
                }
            }
            if (LockMemory!=NULL)
               break;
        }
     }
     if (LockMemory!=NULL)
     {
        LockSeg=FP_SEG(LockMemory);
        InternlHandle=LockInternlBlock(Handle);
        BlockHandle=MAKEINTERNLBLOCK(Handle);
        Class=InternlHandleGetClass(InternlHandle[BlockHandle]);
        InternlHandleSetSegment(InternlHandle[BlockHandle],FP_SEG(LockMemory));
        if (BoradcastSign)
        {
           InternlHandleSetLockCount(InternlHandle[BlockHandle],1);
        }
        else
        {
           InternlHandleSetLockCount(InternlHandle[BlockHandle],0);
        }
        EMSLock(&(InternlHandle[BlockHandle]),LockMemory);
        if (BoradcastSign)
           LockBoradcast(Handle,Class,LockMemory);
     }
     else
     {
        #ifdef DEBUG
        PromptOutOfMemory();
        #endif
        NoLowMemoryBoradcast();
        return(NULL);
     }
  }
  return(MK_FP(LockSeg,4));
}

void HandleFirstLock(int Handle)
{
  HandleInternlLock(Handle,0);
}

void *HandleLock(HANDLE Handle)
{
  return(HandleInternlLock(Handle,1));
}

void HandleUnlock(HANDLE Handle)
{
  InternlHandles huge *InternlHandle;
  int BlockHandle,Count;

  if (!Handle)
  {
     asm int 3;
  }

  InternlHandle=LockInternlBlock(Handle);
  BlockHandle=MAKEINTERNLBLOCK(Handle);

  Count=InternlHandleGetLockCount(InternlHandle[BlockHandle]);
  if (Count>0)
  {
     Count--;
     InternlHandleSetLockCount(InternlHandle[BlockHandle],Count);
  }
  #ifdef DEBUG
  else
     PromptOutofOrder();
  #endif

  return;
}

HANDLE HandleAlloc(long Size,char AllocType)
{
  int InternlSize,HandleToAlloc,OldPrevFreeHandle;

  if (Size<=0)
  {
     asm int 3;
     return(0);
  }

  InternlSize=(Size+MINALLOCSIZE-1)/MINALLOCSIZE;
  HandleToAlloc=HandleLinkSearchFree(InternlSize,&OldPrevFreeHandle);
  if (HandleToAlloc)
  {
     if (HandleLinkSlipFree(HandleToAlloc,InternlSize,AllocType,
                            OldPrevFreeHandle)<0)
     {
        #ifdef DEBUG
        PromptOutOfMemory();
        #endif
        return(0);
     }
     else
        HandleFirstLock(HandleToAlloc);
  }
  #ifdef DEBUG
  else
     PromptOutOfMemory();
  #endif

  return(HandleToAlloc);
}

HANDLE HandleRealloc(HANDLE Handle,long Size)
{
  InternlHandles huge *InternlHandle;
  int BlockHandle,OldClass;

  if (!Handle)
  {
     asm int 3;
     return(0);
  }

  InternlHandle=LockInternlBlock(Handle);
  BlockHandle=MAKEINTERNLBLOCK(Handle);
  if (InternlHandleGetLockCount(InternlHandle[BlockHandle]))
  {
     InternlHandleSetLockCount(InternlHandle[BlockHandle],0);
     //return(0);
  }

  OldClass=InternlHandleGetClass(InternlHandle[BlockHandle]);
  if ((Size+MINALLOCSIZE)/MINALLOCSIZE
      <InternlHandleGetSize(InternlHandle[BlockHandle]))
//  if (Size<InternlHandleGetSize(InternlHandle[BlockHandle]))
  {                                    // Slip the node and free some space
     int FreeNext;
     unsigned short OldAddress,OldSize,OldPrevFree;
     char MergeSign=0;
                                       // See if can merge by another node
     Size=(Size+MINALLOCSIZE)/MINALLOCSIZE;
     OldSize=InternlHandleGetSize(InternlHandle[BlockHandle])-Size;
     OldAddress=InternlHandleGetAddress(InternlHandle[BlockHandle])+Size;
     OldPrevFree=0;
     FreeNext=NowFreeLinkHead;
     while (FreeNext&&!MergeSign)
     {
       InternlHandle=LockInternlBlock(FreeNext);
       BlockHandle=MAKEINTERNLBLOCK(FreeNext);

       if (InternlHandleGetAddress(InternlHandle[BlockHandle])>OldAddress
           ||!InternlHandleGetAddress(InternlHandle[BlockHandle]))
       {
          if (InternlHandleGetAddress(InternlHandle[BlockHandle])==
              OldAddress+OldSize)         // Search find can post-merge
          {
             InternlHandleSetAddress(InternlHandle[BlockHandle],
               InternlHandleGetAddress(InternlHandle[BlockHandle])-OldSize);
             InternlHandleSetSize(InternlHandle[BlockHandle],
               InternlHandleGetSize(InternlHandle[BlockHandle])+OldSize);
          }
          else                         // Search end, can't merge, then insert
          {
                                       // First must get a empty node
             int OldPrevofFree,OldofFree;

             OldPrevofFree=OldPrevFree;
             OldofFree=FreeNext;
             if (InternlHandleGetAddress(InternlHandle[BlockHandle]))
             {
                OldPrevFree=FreeNext;
                FreeNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
             }
             while (FreeNext)
             {
               InternlHandle=LockInternlBlock(FreeNext);
               BlockHandle=MAKEINTERNLBLOCK(FreeNext);

               if (!InternlHandleGetAddress(InternlHandle[BlockHandle])
                   &&!InternlHandleGetSize(InternlHandle[BlockHandle]))
               {                       // Find it
                  int NewNext;

                  NewNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
                  InternlHandleSetAddress(InternlHandle[BlockHandle],OldAddress);
                  InternlHandleSetSize(InternlHandle[BlockHandle],OldSize);
                  if (!OldPrevFree)    // At free link head, possible can't happen
                     NowFreeLinkHead=FreeNext;
                  else                 // Link prev's next to current's next
                  {
                     InternlHandleSetNext(InternlHandle[BlockHandle],OldofFree);
                     InternlHandle=LockInternlBlock(OldPrevFree);
                     BlockHandle=MAKEINTERNLBLOCK(OldPrevFree);
                     InternlHandleSetNext(InternlHandle[BlockHandle],NewNext);
                                       // Link old link
                     if (!OldPrevofFree)
                        NowFreeLinkHead=FreeNext;
                     else
                     {
                        InternlHandle=LockInternlBlock(OldPrevofFree);
                        BlockHandle=MAKEINTERNLBLOCK(OldPrevofFree);
                        InternlHandleSetNext(InternlHandle[BlockHandle],FreeNext);
                     }
                  }
                  break;
               }
               else
               {
                  OldPrevFree=FreeNext;
                  FreeNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
               }
             }
             if (!FreeNext)            // Not find a empty node, can't insert
                                       // free space to free link, so keep it
                                       // this is a awfully case
                return(Handle);
          }
          break;
       }
       else
       {
          OldPrevFree=FreeNext;
          FreeNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
       }
     }
     InternlHandle=LockInternlBlock(Handle);
     BlockHandle=MAKEINTERNLBLOCK(Handle);
     InternlHandleSetSize(InternlHandle[BlockHandle],Size);
     if (InternlHandleGetSegment(InternlHandle[BlockHandle]))
     {                                 // Realloc low address memory
        unsigned char huge *p;
        long NewSize;

        p=MK_FP(InternlHandleGetSegment(InternlHandle[BlockHandle]),4);
        NewSize=Size;
        NewSize=NewSize*MINALLOCSIZE;
        p=MemRealloc(p,NewSize);
        InternlHandleSetSegment(InternlHandle[BlockHandle],FP_SEG(p));
     }
     return(Handle);
  }
  else
  {
     int NewHandle;

     NewHandle=HandleAlloc(Size,OldClass);
     if (NewHandle)
     {
        InternlHandle=LockInternlBlock(NewHandle);
        BlockHandle=MAKEINTERNLBLOCK(NewHandle);
        if (InternlHandleGetSegment(InternlHandle[BlockHandle]))
        {
           char huge *p;

           p=MK_FP(InternlHandleGetSegment(InternlHandle[BlockHandle]),4);
           MemFree(p);
           InternlHandleSetSegment(InternlHandle[BlockHandle],0);
        }
        EMSMovetoEMS(NewHandle,Handle);
     }
     HandleFree(Handle);
     HandleFirstLock(NewHandle);
     return(NewHandle);
  }
}

void HandleFree(HANDLE Handle)
{
  InternlHandles huge *InternlHandle;
  int BlockHandle;

  if (!Handle)
  {
     asm int 3;
  }

  InternlHandle=LockInternlBlock(Handle);
  BlockHandle=MAKEINTERNLBLOCK(Handle);

  #ifdef DEBUG
  if (InternlHandleGetLockCount(InternlHandle[BlockHandle])
      ||!InternlHandleGetUsed(InternlHandle[BlockHandle]))
     PromptOutofOrder();
  #endif

  if (InternlHandleGetSegment(InternlHandle[BlockHandle]))
  {
     char far *p;

     p=MK_FP(InternlHandleGetSegment(InternlHandle[BlockHandle]),4);
     MemFree(p);
  }
  if (InternlHandleGetUsed(InternlHandle[BlockHandle]))
  {
     InternlHandleSetCount(InternlHandle[BlockHandle],0);
     InternlHandleSetSegment(InternlHandle[BlockHandle],0);
  }
  ReconstructFreeLink(Handle);
}

/*long HandleGetMaxFree(void)
{
  InternlHandles huge *InternlHandle;
  int BlockHandle;
  unsigned short MaxFree=0;

  InternlHandle=LockInternlBlock(Handle);
  BlockHandle=MAKEINTERNLBLOCK(Handle);

  FreeNext=NowFreeLinkHead;
  while (FreeNext)
  {
    InternlHandle=LockInternlBlock(FreeNext);
    BlockHandle=MAKEINTERNLBLOCK(FreeNext);
    if (!InternlHandleGetSize(InternlHandle[BlockHandle]))
       break;
    if (MaxFree<InternlHandleGetSize(InternlHandle[BlockHandle]))
       MaxFree=InternlHandleGetSize(InternlHandle[BlockHandle]);
    FreeNext=InternlHandleGetNext(InternlHandle[BlockHandle]);
  }
  return((long)MaxFree*(long)MINALLOCSIZE);
}*/

/* When out of memory, it can free all low memory and do some save */
void FreeAllLowSegment(void)
{
  int j,Class;
  InternlHandles huge *InternlHandle;
  int BlockHandle;
  void huge *LockMemory;

  for (j=0;j<MAXALLOCHANDLE;j++,BlockHandle++)
  {
      if (!(j&((MAXALLOCHANDLE>>2)-1)))
      {
         InternlHandle=LockInternlBlock(j);
         BlockHandle=0;
      }
      if (InternlHandleGetSegment(InternlHandle[BlockHandle]))
      {
         LockMemory=MK_FP(InternlHandleGetSegment(
                          InternlHandle[BlockHandle]),4);
         Class=InternlHandleGetClass(InternlHandle[BlockHandle]);
         UnlockBoradcast(j,Class,LockMemory);
         EMSUnlock(&(InternlHandle[BlockHandle]),LockMemory);
         MemFree(LockMemory);
         InternlHandle=LockInternlBlock(j);
         InternlHandleSetSegment(InternlHandle[BlockHandle],0);
         InternlHandleSetCount(InternlHandle[BlockHandle],0);
      }
  }
}

int HandleInitial(void)
{
  int Result,i;
  long MaxAllocableMemory;
  InternlHandles huge *InternlHandle;
  int TotalEmsPage;

  if (((*((long *)(MK_FP(0,0x67*4))))==0)||(GetEmsStatus()==ERROR))
  {
     fprintf(stderr,"No EMS or EMS initial error! System halt.\n");
     return(ERROR);
  }

  UserEMSFrame=GetEmsSegment();
  #ifdef ReleaseVersion
/*------------*/
  MaxAllocableMemory=MAXALLOCHANDLE/MINALLOCSIZE*sizeof(InternlHandles);
  InternlEMSHandle=EMSAlloc(&MaxAllocableMemory);
  if (InternlEMSHandle==ERROR)    // Alloc MAXALLOCHANDLE*MINALLOCSIZE for system
     return(OUTOFMEMORY);

  TotalEmsPage=GetEmsPageNum();
  MaxAllocableMemory=EMSPAGETOHANDLESIZE(TotalEmsPage)-MaxAllocableMemory;
  if(MaxAllocableMemory<=0)
     return(OUTOFMEMORY);

  UserEMSHandle=EMSAlloc(&MaxAllocableMemory);
  if (UserEMSHandle==ERROR)            // Alloc all for user
     return(OUTOFMEMORY);
/* --------------*/
 #else
 /*------------- !!! Becareful !!! run GETEMS.EXE first -------*/
  InternlEMSHandle=1;
  UserEMSHandle=2;
  MaxAllocableMemory=EMSPAGETOHANDLESIZE(211);
 #endif

  InternlHandle=InternlEMSTmpLock(InternlEMSHandle);
  if (InternlHandle==NULL)
     return(OUTOFMEMORY);

  // MemSet(InternlHandle,0,(MAXALLOCHANDLE/2)*sizeof(InternlHandles));
  /* MAXALLOCHANDLE*sizeof(InternlHandles==0x10000L, so, do it as bellow */
  MemSet(InternlHandle,0,(MAXALLOCHANDLE/2)*sizeof(InternlHandles));
  MemSet((char huge *)(&InternlHandle[MAXALLOCHANDLE/2]),
         0,(MAXALLOCHANDLE/2)*sizeof(InternlHandles));

  /*----- don't use handle[0] -------*/
  InternlHandleSetAddress(InternlHandle[0],0);
  InternlHandleSetSize(InternlHandle[0],0);
  InternlHandleSetUsed(InternlHandle[0]);
  InternlHandleSetNext(InternlHandle[0],0);

  /*----- handle[1] point to free link ----*/
  NowFreeLinkHead=1;
  InternlHandleSetAddress(InternlHandle[1],0);
  InternlHandleSetSize(InternlHandle[1],MaxAllocableMemory);
  InternlHandleSetCanUse(InternlHandle[1]);

  for (i=1;i<MAXALLOCHANDLE-1;i++)
  {
      InternlHandleSetNext(InternlHandle[i],i+1);
  }

  /*------- for link_tail, set next_pointer==NULL ----*/
  InternlHandleSetNext(InternlHandle[MAXALLOCHANDLE-1],0);



  TotalEmsPage=HANDLESIZETOEMSPAGE(MAXALLOCHANDLE/MINALLOCSIZE
               *sizeof(InternlHandles));
  for (i=0;i<TotalEmsPage;i++)
      CloseEmsPage(i,InternlEMSHandle);
  LockBoradcast=DefaultLockBoradcast;
  UnlockBoradcast=DefaultUnlockBoradcast;
  NoLowMemoryBoradcast=OutOfMemorySave;//DefaultNoLowMemoryBoradcast;
  ReturnOK();
}

int HandleFinish(void)
{
  int Result;

  #ifdef ReleaseVersion
  if (UserEMSHandle)
  {
     Result=ERROR;
     while (Result==ERROR)
       Result=FreeEmsPage(UserEMSHandle);
  }
  if (InternlEMSHandle)
  {
     Result=ERROR;
     while (Result==ERROR)
       Result=FreeEmsPage(InternlEMSHandle);
  }
  #endif

  ReturnOK();
}

#endif  // __MEMORYDEBUG__
