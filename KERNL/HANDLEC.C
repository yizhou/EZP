/*-------------------------------------------------------------------
* Name: handlec.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define MAXHANDLES 1024
void *GlobalHandles[MAXHANDLES];

static void OutOfMemorySave(void)
{
  int Result=1;

  SetIntSign();
  setviewport(1,1,getmaxx(),getmaxy(),1);
  setfillstyle(1,EGA_BLUE);
  bar(1,1,getmaxx(),100);
  Alarm();
  DisplayString("Out of memory! system will enter a danger status",
                10,16,EGA_RED,EGA_BLUE);
  if (FileHasBeenLoaded()&&(FileHasBeenModified()&&!FileHasBeenSaved()))
  {
     DisplayString("Save the editing file in OUTOFMEM.EZP!",
                   10,32,EGA_RED,EGA_BLUE);
     //FreeAllLowSegment();
     //DataofItems=HandleLock(GlobalItemsHandle);
     Result=FileSave("OUTOFMEM.EZP");
  }
  //else FreeAllLowSegment();

  SystemDestruct();
  if (Result<=0)
     fprintf(stderr,"Abort: Out of Memory! Editing file save as .\\OUTOFMEM.EZP\n");
  if (Result<0)
     fprintf(stderr,"But failed!\n");
  exit(-1);
}

HANDLE HandleAlloc(long Size,char AllocType)
{
  void *MidPointer;
  //static int zjh_HandelMax=0;  See Varc.c
  int i;

  for (i=1;i<MAXHANDLES;i++)
      if (GlobalHandles[i]==NULL)
         break;
  //if ((i>=MAXHANDLES)||((MidPointer=(void *)malloc(Size))==NULL))
  if ((i>=MAXHANDLES)||((MidPointer=(void *)malloc(Size))<0x1000))
  {
     ReportMemoryError("handlealloc");
     OutOfMemorySave();         // it will exit
    // return(0);
  }
  GlobalHandles[i]=(void *)MidPointer;
  // if (i>zjh_HandleMax)    zjh_HandleMax=i;
  return(i);
}

void HandleFree(HANDLE Handle)
{
  if (Handle<MAXHANDLES&&GlobalHandles[Handle]!=NULL)
  {
     //MemFree(GlobalHandles[Handle]);
     free(GlobalHandles[Handle]);
     GlobalHandles[Handle]=NULL;
  }
}

void *HandleLock(HANDLE Handle)
{
  if (Handle<MAXHANDLES)
     return(GlobalHandles[Handle]);
  else
     return(NULL);
}

/*-------------
void HandleUnlock(HANDLE Handle)
{
  return;
}
------------*/

HANDLE HandleRealloc(HANDLE Handle,long Size)
{
  void *MidPointer;

  MidPointer=GlobalHandles[Handle];
  MidPointer=(void *)realloc(MidPointer,Size);
  //MidPointer=farrealloc(MidPointer,Size);
  if (MidPointer!=NULL)
  {
     GlobalHandles[Handle]=MidPointer;
     return(Handle);
  }
  else
     return(0);
}

void HandleInitial(void)
{
  memset(GlobalHandles,0,sizeof(GlobalHandles));
}

void HandleFinish(void)
{
  HANDLE i;

  for (i=1;i<MAXHANDLES;i++)    // ByHance, 96,1.19
      HandleFree(i);

  if(GlobalHandles[0]!=0)
    printf("Overflow!\n");

  return;
}

#ifndef REGIST_VERSION
void TestHandle()
{
  HANDLE i;
  for (i=1;i<MAXHANDLES;i++)
      if (GlobalHandles[i]!=NULL && GlobalHandles[i]<0x1000)
      {
         printf("[%d]=%x",i,GlobalHandles[i]);
      }
}
#endif
