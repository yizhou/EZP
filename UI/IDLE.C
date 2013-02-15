#ifdef USE_IDLE
   #define GRAPHICDRAW 1
   #define GRAPHPRINT 2
   #define GRAPHIDLECOUNT 6
   #define PRINTIDLECOUNT 6
   #define PRINTBUFFERSIZE 8192
   #ifdef __IDLE_C__
     char IdleSign=0;
     FILE *PrintFP=stdprn;
     FILE *TmpPrnFP=NULL;
     HANDLE PrintBufferHandle;
   #else
     extern char IdleSign;
     extern FILE *PrintFP;
     extern FILE *TmpPrnFP;
     extern HANDLE PrintBufferHandle;
   #endif
#define ExecNextGraphOP() 1

void DrawGraph(HWND Window)
{
  int DeviceTimer;

  DeviceTimer=TimerInsert(GRAPHIDLECOUNT,Window);
  while (!MessageQueueIsEmpty())
  {
    if (ExecNextGraphOP()<OpOK)
       break;
  }
  TimerDelete(DeviceTimer);
  return;
}

void PrintGraph(HWND Window)
{
  int DeviceTimer;
  char *PrintBuffer;

  if (TmpPrnFP==NULL)
  {
     if ((TmpPrnFP=fopen("tmpprn.000","rb"))==NULL)
        return;                        // Must message error FILEOPEN
     else
     {
        PrintBufferHandle=HandleAlloc(PRINTBUFFERSIZE,0);
        if (PrintBufferHandle==0)
           return;                     // Must message error OUTOFMEM
     }
  }
  if ((PrintBuffer=HandleLock(PrintBufferHandle))==NULL)
     return;                           // Must message error OUTOFMEM
  DeviceTimer=TimerInsert(PRINTIDLECOUNT,Window);
  while (!MessageQueueIsEmpty())
  {
    int Count;

    Count=fread(PrintBuffer,1,PRINTBUFFERSIZE,TmpPrnFP);
    if ((fwrite(PrintBuffer,1,Count,PrintFP)<Count)||Count<PRINTBUFFERSIZE)
    {
       TimerDelete(DeviceTimer);
       HandleUnlock(PrintBufferHandle);
       return;                         // Must message error FILEREADWRITE
    }
  }
  TimerDelete(DeviceTimer);
  HandleUnlock(PrintBufferHandle);
  if (feof(TmpPrnFP))
  {
     fclose(TmpPrnFP);
     HandleFree(PrintBufferHandle);
     TmpPrnFP=NULL;
     IdleSign&=~GRAPHPRINT;
  }
  return;
}
#endif    // USE_IDLE
