/*-------------------------------------------------------------------
* Name: timer.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define TIMER_VECTOR    8
#define MOUSEHOLDTIME   2
#define MAXTIMER        10

typedef struct {
   short ClickN;
   short IntCount;
   short Window;
} TimerStruct;

void (__interrupt __far  *OldTimerProc)()=( void(*)(void) )0;

volatile   long MouseDownTime=0;
volatile   unsigned short MouseButton=0;
volatile   int UserIntSign=0;
static volatile   TimerStruct TimerArr[MAXTIMER];
volatile unsigned short TimerTicks_8,TimerTicks;

#pragma off (check_stack)

int GetIntSign()
{
   return UserIntSign;
}
void SetIntSign()
{
   UserIntSign=1;
}
void ClearIntSign()
{
   UserIntSign=0;
}

void __interrupt __far TimerDeal()
{
    // short change_day;
    long time_tick;
    int   i;

    if( (TimerTicks_8 & 7) == 0 )
    {    /*- enter 8 times, call old int8 -*/
       (*OldTimerProc)();

       if(UserIntSign==0)
       {
          UserIntSign=1;
          if(MouseButton)
          {
             //change_day=*(volatile char *)0x470;
             time_tick=*(long *)0x46c;
             //change_day=_bios_timeofday(_TIME_GETCLOCK, (long *)&time_tick);
             if( /* !change_day && */ time_tick>MouseDownTime+MOUSEHOLDTIME )
             {
                 MouseDownTime=time_tick;
                 MouseDownOnTrigger(MouseButton,0,0);
             }
          }

          for(i=0;i<MAXTIMER;i++)
             if(TimerArr[i].IntCount) {
                if( (--TimerArr[i].ClickN)==0 ) {
                   TimerArr[i].ClickN=TimerArr[i].IntCount;   //restore old value
                   MessageCreatbyTimer(TimerArr[i].Window,i);
                }
             }
          UserIntSign=0;
       }
    }
    else        /*- 1/8 of normal timer -*/
       outp(0x20,0x20);  /*- EOI -*/

    TimerTicks_8++;
    TimerTicks++;
}  /* TimerDeal */

void SetMouseDownOnTime(long time,unsigned short Status)
{
    MouseDownTime=time;
    MouseButton=Status&3;
}

// void TimerEndProc (void) {}

int TimerInsert(int TimerCount,HWND WindowNumber)
{
    int   i;

    for(i=0;i<MAXTIMER;i++)
       if(!TimerArr[i].IntCount) {
          TimerArr[i].IntCount=TimerArr[i].ClickN=TimerCount;
          TimerArr[i].Window=WindowNumber;
          return i;
       }

    return -1;          // can not insert this timer
}

int TimerDelete(int TimerIdx)
{
    if( TimerIdx<0 || TimerIdx>=MAXTIMER || (!TimerArr[TimerIdx].IntCount) )
         return -1;
    memset( (char *)&TimerArr[TimerIdx], 0, sizeof(TimerStruct) );
    return 0;
}

/*---- delay 10*DelayN ms -----*/
void myDelay(int delayN)
{
   /*---- 55/8*(16/11)=10 ms ---*/
   short n=(long)delayN*16/11 + TimerTicks_8;
   while(TimerTicks_8<n) {}
}

int TimerInit(void)
{
  memset((char *)TimerArr,0,sizeof(TimerArr));
  OldTimerProc=_dos_getvect( TIMER_VECTOR );
  _dos_setvect( TIMER_VECTOR, TimerDeal );

  outp(0x43, 0x34);  /*- set timer_count, first low, then high byte -*/
  outp(0x40, 0x0);
  outp(0x40, 0x20);  /*- 0x2000, that is 1/8 of normal timer_count -*/

  TimerTicks_8=TimerTicks=0;
  ReturnOK();
}

int TimerEnd(void)
{
  if(OldTimerProc)             // restore old timer vector
  {
     outp(0x43, 0x34);  /*- set timer_count, first low, then high byte -*/
     outp(0x40, 0x0);
     outp(0x40, 0x0);  /*- 0x10000, that is normal timer_count -*/

     _dos_setvect( TIMER_VECTOR, OldTimerProc );
  }
  ReturnOK();
}

