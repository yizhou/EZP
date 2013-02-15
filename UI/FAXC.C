/*-------------------------------------------------------------------
* Name: faxc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern volatile unsigned short TimerTicks;
//void myDelay(int n){}
int fFaxFine=0;                 // if fine=1, use 196 dpi

#pragma off (check_stack)

#define TIME_OUT_N      500
#define WaitTime        55

#define DLE             0x10
#define ETX             0x03
#define CAN             0x18            // cancel

#define XOFF            0x13
#define XON             0x11

static char InitStr[64]; // ="AT&F&C1&D2";

char BpsCmdStr[64];
char BpsCmdStr2[64];

unsigned short Class1CapBits;

int TotalChars;
int MinChars1Row,CharInRow_send;
unsigned char *FaxData=NULL,LastChar;
int firstRecv;

int DIS_retryN;
static int SendFTHflag;

int TrainN;

 #define ID_CMDBAD           0
 #define ID_CMDOK            1
 #define ID_CMDCONNECT       2
 #define ID_CMDERROR         3
 #define ID_CMDDIALTONE      5
 #define ID_CMDBUSY          6
 #define ID_CMDRING          7
 #define ID_CMDUSER          9
static char *RetCmdStrArr[]={
     "OK", "CONNECT", "ERROR", "NO CARRIER", "DIALTONE", "BUSY", "RING",
     "ANSWER",
     "user defined",            // it can be changed
     "O",  "K",  "O",  "K", "O",    /*- not used now -*/
     "+FDIS:", "+FCSI:", "+FNSF:", "+FHNG:"
};
static short RetCmdLenArr[]={
     2, 7, 5, 10, 8, 4, 4, 6, 12,  /*- 9 -*/
     1, 1, 1, 1,  1, /*- 14 -*/
     6, 6, 6, 6  /*- 18 -*/
};

void SetUserCmdStr(char *str)
{
   strcpy(RetCmdStrArr[ID_CMDUSER-1], str);
   RetCmdLenArr[ID_CMDUSER-1] = strlen(str);
}

const unsigned char RevTable[256] =
{
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};


 // disable fast training mask: 0011 0110 1101 1111 = 0x36df
    /*------- see page 307 -----*/
static unsigned char Class1ReceiverCapArr[4*0xf]=
{
//bps:  300 v.21        2400 v.27       4800 v.27         7200 v.29(chooseable)
   03,00,0x00,00,    24,00,0x00,00,    48,00,0x10,00,    72,00,0x30,00,
//bps: 7200 v.17        7200 v.17 f_n    9600 v.29         9600 v.17
   73,00,0x34,00,    74,00,0x34,00,    96,00,0x20,00,    97,00,0x24,00,
//bps: 9600 v.17 f_n   12000 v.17      12000 v.17        12000 v.17 f_n
   98,00,0x24,00,   121,00,0x18,00,   121,00,0x14,00,   122,00,0x14,00,
//bps:14400 v.17       14400 v.17      14400 v.17 fast training_not used
  145,00,0x08,00,   145,00,0x04,00,   146,00,0x04,00
};
static unsigned short BpsArr[0xf]=
{
   300,              2400,             4800,              4800,
  4800,              4800,             9600,              9600,
  9600,             12200,            12200,             12200,
 14400,             14400,            14400
};

                                         //  0x43=RecvTable[FCF_SNDR|FCF_TSI]
unsigned char TsiFrame[0x17+2]={ 0x17, 0x00, 0xff, 0x03, 0x43 };
                                         //  0x83=RecvTable[FCF_SNDR|FCF_DCS]
unsigned char DcsFrame[]={ 7, 0, 0xff, 0x13, 0x83, 0, 0x02, 0xb0, 0};
                                         //  0xfb=RecvTable[FCF_SNDR|FCF_DCN]
unsigned char DcnFrame[]={ 3, 0, 0xff, 0x13, 0xfb };
                                         //  0x4f=RecvTable[FCF_SNDR|FCF_MPS]
unsigned char MpsFrame[]={ 3, 0, 0xff, 0x13, 0x4f };
                                         //  0x2f=RecvTable[FCF_SNDR|FCF_EOP]
unsigned char EopFrame[]={ 3, 0, 0xff, 0x13, 0x2f };

char TmpRecvBuf[0x40];
int  TmpRecvBuf_i;
unsigned short BpsOfCap, MinDelay1Row_ms;
char flowCtrl=XON,fUseFlow;
int  fCanBreak;

 #define RECV_MOD_MASK  0xff
char RecvBuf[RECV_MOD_MASK+1];
int  RecvBufTail,RecvBufHead;
//char SendBuf[0x40];
//int  SendBufTail,SendBufHead;

int  ClassX;

char DialCmdStr[80];
unsigned short PortBase,PortBase_1,PortBase_2;
unsigned short PortBase_3,PortBase_4,PortBase_5,PortBase_6;
unsigned short COMM_INT;
unsigned char IntDisableMask,IntEnableMask;


void  GetFaxConfig()
{
   char str[80];

   GetProfileInt( ProfileName,FaxSection, ComEntry, &ComX, 2);
   GetProfileInt( ProfileName,FaxSection, AutoDialEntry, &fTelManualDial, 0);
   GetProfileInt( ProfileName,FaxSection, ToneLineEntry, &fTelTone, 1);
   GetProfileString( ProfileName,FaxSection, "InitString", InitStr, "AT&F&C1&D2");
   GetProfileString( ProfileName,FaxSection, LocalIdEntry, str, "");
   str[20]=0;
   strcpy(LocalTelId,str);
   GetProfileString( ProfileName,FaxSection, DialNumEntry, str, "");
   if(access("c:\\DOS\\韩兆强的",F_OK)==0)
       strcpy(str,"62282931");

   str[20]=0;
   strcpy(DialNumber,str);
}

void  SaveFaxConfig()
{
   SetProfileInt( ProfileName,FaxSection, ComEntry, ComX );
   SetProfileInt( ProfileName,FaxSection, AutoDialEntry, fTelManualDial );
   SetProfileInt( ProfileName,FaxSection, ToneLineEntry, fTelTone );
   SetProfileString( ProfileName,FaxSection, LocalIdEntry, LocalTelId );
}
/*---------------------------------------------------*/
static void FreeFaxData()
{
   if(FaxData!=NULL)
   {
      free(FaxData);
      FaxData=NULL;
   }
}

void GetFaxFilename(int n,char *fn)
{
   FILE *fp;
   fp=fopen("c:\\ezp\\fax\\ok","wb");
   if(fp==NULL)
      mkdir("c:\\ezp\\fax");
   else
      fclose(fp);

   sprintf(fn,"c:\\ezp\\fax\\fax.%.3d",n);
}

void ReadPageToBuf(int n)
{
   FILE *fp;
   long fileLen;
   char filename[80];

   if(FaxData!=NULL)
       FreeFaxData();

   GetFaxFilename(n-StartPrintPage,filename);
   TotalChars=0;
   fp=fopen(filename,"rb");
   if(fp==NULL)
   {
      puts("open fax file error");
      return;
   }

   fseek(fp,0,SEEK_END);
   fileLen=ftell(fp);

   FaxData=malloc(fileLen);
   if(FaxData==NULL)
   {
      fclose(fp);
      puts("memory alloc error");
      return;
   }

   fseek(fp,0,SEEK_SET);
   TotalChars=fread(FaxData,1,fileLen,fp);
   fclose(fp);
}
/*---------------------------------------------------*/

void (__interrupt __far  *OldComIntProc)() = ( void(*)(void) )0;

void __interrupt __far NewComIntDeal()
{
   unsigned char st;

   outp(0x20,0x20);  /*- EOI -*/
   _enable();
   inp(PortBase_5);

   for(;;)
   {
      st=inp(PortBase_2);
      if( (st & 1) != 0 )
          break;

      if( (st&6) == 4 )
      {
         st=inp(PortBase);
         RecvBuf[RecvBufTail++]=st;
         RecvBufTail &= RECV_MOD_MASK;
         if(st==XON) flowCtrl=st;
         else
         if(st==XOFF && fUseFlow) flowCtrl=st;
      }
   } /*- for -*/
} /*- end new comm int -*/

static void EnableSoftFlow()
{
    fUseFlow=1;
}
static void DisableSoftFlow()
{
    fUseFlow=0;
    flowCtrl=XON;
}

static void SetNewCommInt()
{
   OldComIntProc=_dos_getvect( COMM_INT );
   _dos_setvect( COMM_INT, NewComIntDeal );
}
static void RestoreCommInt()
{
   if(OldComIntProc)
   {
       outp( 0x21, inp(0x21) | IntEnableMask );
       outp(PortBase_4,0);
       outp(PortBase_1,0);
       outp(PortBase_2,0);

       _dos_setvect( COMM_INT, OldComIntProc );
   }
}

/*--------------------------------------------------------------*/
static void InitCommBuf()
{
   RecvBufTail=RecvBufHead=0;
   TmpRecvBuf_i=0;
}

static void InitComPort()
{
   unsigned char st,i;
 #define NEW_RESET

   inp(PortBase);
   inp(PortBase_5);

   _disable();
   st=inp(PortBase_3);
   outp(PortBase_3,st|0x80);    /* LCR : next is baud rate */
   //outpw(PortBase_1,0);
   outpw(PortBase,6);     // 19200=115200/6  /* Baud rate low byte */
   outp(PortBase_3,st);

   outp(PortBase_3,3);    // Dlab=0, no parity, 1 stop, 8 data
   _enable();

   st=inp(PortBase_2) & 0xc0;
   if(st==0xc0)
      outp(PortBase_2,0xc7);

 #ifdef NEW_RESET
   outp(PortBase_2,0);
   outp(PortBase_1,0);
   st=inp(PortBase_4) & 0xef;
   outp(PortBase_4,st|0xb);
 #endif

   inp(PortBase);
 #ifdef NEW_RESET
   inp(PortBase_2);
 #endif
   inp(PortBase_5);
   inp(PortBase_6);

   _disable();
   st=inp(0x21) & IntDisableMask;
 #ifdef NEW_RESET
   outp(PortBase_1,0x7);
 #else
   outp(PortBase_1,0x3);
 #endif
   outp(PortBase_4,0xb);    // MCR: DTR=1, RTS=1
   outp(0x21,st);
   _enable();

   for (i=0; i<8; i++)                  /* Edge-triggering, read the ports */
        inp(PortBase + i);          /* Port to clear                   */
}

static void SetCommPara()
{
   InitCommBuf();
   SetNewCommInt();
   InitComPort();
   InitCommBuf();
}

/* ComX=[1..4], that is COM1 to COM4 */
static void SetPortPara(int ComX)
{
   static struct {
      unsigned short PortBase;
      unsigned char IntDisableMask,IntEnableMask;
      int CommInt;
   }
   PortArr[]=
   {
       // { 0x2e8, 0xdf, 0x20, 5 },
       { 0x3f8, 0xef, 0x10, 0xc },
       { 0x2f8, 0xf7, 0x08, 0xb },
       { 0x3e8, 0xef, 0x10, 0xc },
       { 0x2e8, 0xf7, 0x08, 0xb },
   };
   int i=ComX;

   if(i<1 || i>4)
     i=2;
   i--;

   PortBase       = PortArr[i].PortBase;
   IntDisableMask = PortArr[i].IntDisableMask;
   IntEnableMask  = PortArr[i].IntEnableMask;
   COMM_INT       = PortArr[i].CommInt;
   PortBase_1     = PortBase+1;
   PortBase_2     = PortBase+2;
   PortBase_3     = PortBase+3;
   PortBase_4     = PortBase+4;
   PortBase_5     = PortBase+5;
   PortBase_6     = PortBase+6;
}
/*---------------------------------------------------*/


/*-- return:
  high 8 bit=1: received none,  0: ch=char received
--------------*/
static int GetRecvChar()
{
   unsigned char ch;

   if(RecvBufHead==RecvBufTail) return 0x100;
   ch=RecvBuf[RecvBufHead++];
   RecvBufHead &= RECV_MOD_MASK;
   return ch;
}

static int GetRecvStr(char *buf,int len)
{
   int i,ret;

   TimerTicks=i=0;
   len--;
   while(TimerTicks<=400)
   {
      ret=GetRecvChar();
      if( (ret&0xff00)==0 && ret>0xf)
      {
          *buf=ret; i=1;
          break;
      }
   }

   for(;;)
   {
      ret=GetRecvChar();
      if(ret==0xd || ret==0xa || TimerTicks>400)
         break;
      if( (ret&0xff00)==0 && i<len)
         buf[i++]=ret;
   }

   buf[i]=0;
   return i;
}

/*-- return:
 0=OK, other=failed
---------*/
static int WaitRetChar(int idx)
{
   int i,len,k;
   char *p;

   for(i=0;i<9;i++)
   {
      k=idx;
      p=RetCmdStrArr[i];
      len=RetCmdLenArr[i];
      while(len>0)
      {
          if(p[len-1]==TmpRecvBuf[k])
          {  /*- try next char -*/
             len--;
             k=(k-1)&0x3f;
          }
          else
             break;
      }

      if(len==0)
         return i+1;
   } /*- for -*/

   if(ClassX==2)        /*- class 2 -*/
     for(i=14;i<18;i++)
     {
         k=idx;
         p=RetCmdStrArr[i];
         len=RetCmdLenArr[i];
         while(len>0)
         {
             if(p[len-1]==TmpRecvBuf[k])
             {
                len--;
                k=(k-1)&0x3f;
             }
             else
                break;
         }

         if(len==0)
             return i-4;
     }

   return 0;
}

/*-- return:
  0=time out, other: id of return cmd string(see RetCmdStrArr)
--------------*/
static int WaitRetString(int DelayTime)
{
   int ret;
   int i;

   TimerTicks=0;
   while(TimerTicks<DelayTime)
   {
     lbl_next:
        ret=GetRecvChar();
        if((ret&0xff00)!=0)
        {
            if(TimerTicks<=DelayTime)
            {
                if(kbhit())
                   if(getch()==0x1b && fCanBreak)
                      return 0x1b;
                goto lbl_next;
            }
        }

        if((ret&0x00e0)!=0)
        {
            i=TmpRecvBuf_i;
            TmpRecvBuf[i]=(unsigned char)ret;
            TmpRecvBuf_i=(i+1)&0x3f;
            if( (ret=WaitRetChar(i)) != 0 )
                return ret;      /*- found correct receive_cmd_string -*/
        }
   } /*- end while -*/

   return 0;
}
/*---------------------------------------------------*/

/*-- return:
  1=time out, 0=ok
--------------*/
static int SendCmdChar(unsigned char ch)
{
    int n;

    if(fUseFlow==0)
    {
     lbl_send:
       n=0x1ff;
       do {
          if( (inp(PortBase_5)&0x20) !=0 )
          {
             _disable();
             outp(PortBase,ch);
             _enable();
             return 0;
          }
          n--;
       } while(n);
       return 2;        /*- time out -*/
    }

    if(flowCtrl==XOFF)
       return 1;

    if( (inp(PortBase_6)&0x10) !=0 )
       goto lbl_send;

    return 1;
}

/*-- return:
  0=time out, otherwise return 1
--------------*/
static int SendCmdStr(unsigned char *CmdStr)
{
   int i;
   char ch;

   TimerTicks=i=0;
   while( (ch=CmdStr[i]) !=0 )
   {
      if(TimerTicks>=TIME_OUT_N)
         return 0;

      while(SendCmdChar(ch))
      {
         if(TimerTicks>=TIME_OUT_N)
            break;
      }
      i++;
   }

   return 1;
}
/*---------------------------------------------------*/


/*-- return:
  0=error, otherwise return 1
--------------*/
static int RecvFrame(char *buf, int DelayN)
{
    int ret;

    if(!firstRecv)
    {
       SendCmdStr("AT+FRH=3\r");
       ret=WaitRetString(DelayN);
       if(ret!=ID_CMDOK && ret!=ID_CMDCONNECT)
       {
        lbl_err:
           while(SendCmdChar(CAN)) {}
           return 0;
       }
    }

    firstRecv=ret=TimerTicks=*buf=*(buf+1)=0;
    while(TimerTicks<=DelayN)
    {
       ret=GetRecvChar();
       if( ret==0xff )
          break;
    }

    if(ret!=0xff)
        goto lbl_err;

    do {
         if(ret==DLE)           // ignore first DLE
         {
            for(;;)
            {
                ret=GetRecvChar();
                if( (ret&0xff00)==0 || TimerTicks>=DelayN)
                   break;
            }
            if(ret==ETX)        // EOP
               break;
         }

         buf[buf[0]+2]=ret;
         buf[0]++;
         for(;;)
         {
             ret=GetRecvChar();
             if( (ret&0xff00)==0 || TimerTicks>=DelayN)
                break;
         }
    } while(buf[0]<0x1c);

    if(TimerTicks>=DelayN)
        goto lbl_err;

    if(WaitRetString(300)!=ID_CMDOK)
        return 0;
     return 1;
}

/*-- return:
  0=time out, otherwise return 1
--------------*/
static int SendComFrame(char *buf,int flag, int DelayN)
{
   int ret,i,total;

   if(SendFTHflag)
   {
      SendCmdStr("AT+FTH=3\r");
      ret=WaitRetString(DelayN);
      if(ret!=ID_CMDOK && ret!=ID_CMDCONNECT)
         return 0;
      myDelay(2);
   }

   SendFTHflag=flag;
   if(flag) buf[3] |= 0x10;
   else buf[3] &= 0xef;

   total=*(short *)&buf[0];
   i=2;
   total += 2;
   TimerTicks=0;
   for(;i<total;i++)
   {
       while( SendCmdChar(buf[i]) )
          if(TimerTicks>=DelayN)
             break;
       if(buf[i]==DLE)
          while( SendCmdChar(DLE) )
             if(TimerTicks>=DelayN)
                break;
   }

   while( SendCmdChar(DLE) )
      if(TimerTicks>=DelayN)
         break;
   while( SendCmdChar(ETX) )
      if(TimerTicks>=DelayN)
         break;

   if(TimerTicks>=DelayN)
      return 0;

   ret=WaitRetString(600);
   if( ret!=ID_CMDOK && ret!=ID_CMDCONNECT )
      return 0;

   return 1;
}
/*---------------------------------------------------*/

static int GetClass1Cap()
{
   char str[0x40],*p;
   int  i,len,n;

   SendCmdStr("AT+FTM=?\r");    /*- bps when send data in C_segment -*/
   GetRecvStr(str,0x38);
   str[0x3c]=0;
   if( WaitRetString(600)!=ID_CMDOK )
      return 0;
   Class1CapBits=0;
   len=strlen(str);   /*- str may be : 3,24,48,72,96 -*/

   p=str;
   while(p-str<len)
   {
      sscanf(p,"%d",&n);
      if(n)
      {
        for(i=0;i<0xf;i++)
          if(Class1ReceiverCapArr[i*4]==n)
              Class1CapBits |= (1<<i);
      }

      do {
          if(*p++==',') break;
      } while (p-str<len);
   } /*-- for --*/

   myDelay(0xf);
   return 1;
}

static int bps_idx,bps;
static int CalculateBps(int fUseMaxBps)
{
   int i,n;

   if(fUseMaxBps) bps_idx=0xe;
   else  bps_idx--;     /*- dec bps when resend -*/

   for(;bps_idx>0;bps_idx--)
      if(Class1ReceiverCapArr[bps_idx*4+3]!=0)
         break;
   if(bps_idx==0)
      return 0;

    /*------- see page 276 & 307 -----*/
   DcsFrame[6] &= 0xc3;         /*- clear CCITT's bit14..11 -*/
   i=Class1ReceiverCapArr[bps_idx*4+2];
   DcsFrame[6] |= RevTable[i];

   bps=Class1ReceiverCapArr[bps_idx*4];
   if(bps_idx==3 || bps_idx==7 || bps_idx==10 || bps==13)
       n=bps+1;
   else
       n=bps;

   sprintf(BpsCmdStr,"AT+FTM=%d\r",bps);
   sprintf(BpsCmdStr2,"AT+FTM=%d\r",n);
        /*---- (BPS/8)(bytes/s) * (ms/1000)(s) + 1 -----*/
   MinChars1Row=BpsArr[bps_idx]/64*MinDelay1Row_ms/125 + 1;
   TrainN=BpsArr[bps_idx]/16*3;         /*-- BPS/8(bytes/s) * 1.5(s) ---*/
   return 1;
}
/*---------------------------------------------------*/

/*---entry:  see page 276,277 -----------------
  in CCITT
 1. DIS_byte1:
     bit
     16         0=1d encoding         1=2d encoding
     15         0=98 DPI              1=196 DPI
   11..14       signal rate in C_segment
     10         T4 receive power
      9         T4 send power
 3. DIS_byte0:
    8..6        reserved for T3
      5         T3 receiver
      4         T3 sender
    3..1        for T2

 2. DIS_byte2:
     24         1=followed with another 8 bits
    21..23      min time when scaning one row
    19..20      paper length:  0=A4(297), 1=unlimited, 2=A4,B4(364), 3=reserved
    17..18      paper width: 0 = 1728(A4:215),
                             1,3=1728(A4), 2048(B4:255), 2432(A3:303)
                             2 = 1728(A4), 2048(B4)
-------------------------------------------------------------------------*/
static int SetBpsArrFlag(int DIS_byte1,int DIS_byte2)
{
    int i,Mask;
    unsigned char ch=RevTable[DIS_byte1 & 0x3c]>>2; /*- bit13..10: DIS_rate -*/
    unsigned short n;

    switch(ch)          /*- in CCITT, it is bit11..14 -*/
    {
       case 0:                  /*- v.27 fallback: 2400 bps -*/
            Mask=3;
            break;
       case 4:                  /*- v.27: 4800 bps -*/
            Mask=7;
            break;
       case 8:                  /*- v.29 -*/
            Mask=0x49;
            break;
       case 12:                 /*- v.27 & v.29 -*/
            Mask=0x4f;
            break;
       case 13:                 /*- v.27 & v.29 & v.33 & v.17 -*/
            Mask=0x7fff;
            break;
       case 14:                 /*- v.27 & v.29 & v.33 -*/
            Mask=0x124f;
            break;
       default:
            return 0;   /*- reserved -*/
    }

    n = Class1CapBits & Mask & 0x36df;   // diable fast training

    for(i=0;i<0xf;i++)
    {
       if( ((n>>i)&1) )
           Class1ReceiverCapArr[i*4+3]=1;
       else
           Class1ReceiverCapArr[i*4+3]=0;
    }

    ch=(DIS_byte2&0xf0)<<1;     /*- bit22..20: min scan time -*/
    ch=RevTable[ch];
    switch(ch)          /*- in CCITT, it is bit21..23 -*/
    {
       case 6:  /*- in 98dpi: 20, but in 196dpi: 10 -*/
            if(fFaxFine)
                goto lbl_other;
       case 0:
       lbl_0:
            MinDelay1Row_ms=20;
            ch=0|0x80;
            break;
       case 5:  /*- in 98dpi: 40, but in 196dpi: 20 -*/
            if(fFaxFine)
                goto lbl_0;
       case 1:
            MinDelay1Row_ms=40;
            ch=0x40|0x80;               /*- x100 ==> 001 -*/
            break;

       case 7:
            MinDelay1Row_ms=0;
            ch=0x70|0x80;              /*- x111 ==> 111 -*/
            break;
       case 4:
       lbl_4:
            MinDelay1Row_ms=5;
            ch=0x10|0x80;             /*- x001 ==> 100 -*/
            break;
       case 3:
            if(fFaxFine)
                goto lbl_4;
       // case 2:
       default:
       lbl_other:
            MinDelay1Row_ms=10;
            ch=0x20|0x80;             /*- x010 ==> 010 -*/
            break;
    } /*- end switch -*/

    //6816
    DcsFrame[7] = ch;           // bit 21-23: min_delay_time
    if(DIS_retryN--)
       return 1;
    return 0;
}
/*------------------------------------------------------------------------*/

/*------ T.30 session subparameter codes, for class2  ------------------
 Entry: str="(0-1),(0-5),(0-2),(0-2),(0),(0),(0),(0-7)"
            VertRes(0=98,1=196)
                 BitRate(0=2400,1=4800,2=7200,3=9600,4=12000,5=14400)
                        PageWidth(0=1728,1=2048,2=2432,3=1216,4=864)
                               PageLen(0=A4,1=B4,2=Unlimited)
                                    CompressFormat(0=1d,1,2,3=2d)
                                        ErrorCorrection(0=disable ECM,1=enable,64 bytes/frame,2=256 bytes)
                                             BinaryFileTransfer(0=disable,1=enable)
                                                 ScanTimePerLine(see below 0..7)
-------------------------------------------------------------------------*/
   /*--- ScanDelayMinTimePerLine      VR=98             VR=196  -----
                0                       0                0
                1                       5                5
                2                      10                5
                3                      10               10
                4                      20               10
                5                      20               20
                6                      40               20
                7                      40               40
   -----------------------------------------------------------------------*/
static void CalculateClass2Delay(char *str)
{
   int  n,n1,n2,len,flag;
   char ch,*p=str;
   int  i;
   static short Delay_msArr[8]={ 0, 5, 10, 10, 20, 20, 40, 40 };
   short cap[8];

   memset(cap,0,sizeof(cap));
   len=strlen(str);
   flag=n=n1=0;

   while(p-str<len)
   {
      ch=*p++;
      switch(ch)
      {
         case '(':  flag++; break;
         case ')':  flag--; break;
         case '-':
            do {
              ch=*p++;
              if(ch>='0' && ch<='9')
                 break;
            } while(p-str<len);

            n2=ch-'0';

            do {
               if(n2>n1) n1++;
               else if(n2<n1) n1--;
               cap[n] |= (1<<n1);
            } while(n1!=n2);

            break;
         case ',':
            if(flag==0)
            {
               n++;
               if(n>=8) goto lbl_exit;
            }
            break;
         default:
            if(ch>='0' && ch<='9')
            {
               n1=ch-'0';
               cap[n] |= (1<<n1);
            }
            break;
      } /*- end switch -*/
   } /*-- while --*/

 lbl_exit:
   BpsOfCap=0;   n=cap[1]>>1;           // bit rate
   while(n) { BpsOfCap++;  n >>= 1; }

   i=0;   n=cap[7]>>1;          // ScanTimePerLine
   while(n) { i++;  n >>= 1; }

   MinDelay1Row_ms=Delay_msArr[i];
   MinChars1Row = 3 * (BpsOfCap+1) * MinDelay1Row_ms / 10 + 1;
}

static int GetClass2Cap()
{
   char str[0x40];

   SendCmdStr("AT+FDCC=?\r");
   GetRecvStr(str,0x38);
   if( WaitRetString(600)!=ID_CMDOK )
      return 0;

   str[0x40-4]=0;
   CalculateClass2Delay(str);

   myDelay(0xf);
   return 1;
}
/*------------------------------------------------------------------------*/


///////////////////FAXINFO/////////////////////////
#define FAX_W  250
#define FAX_H  105
#include "faxicon.h"

static int fax_x,fax_y;

void FaxStatus(char *str, int mode)
{
   setviewport(0,0,getmaxx(),getmaxy(),1);
   MouseHidden();

   setfillstyle(1,EGA_LIGHTGRAY);
   bar(fax_x+86,fax_y+56,fax_x+FAX_W-5,fax_y+72);
   ViewportDisplayString(str,fax_x+86,fax_y+56,
                          EGA_BLACK,EGA_LIGHTGRAY);

   switch(mode) {
     case NULL_MODE:
          setcolor(EGA_BLACK);
          line(fax_x+43,fax_y+25,fax_x+206,fax_y+25);
          line(fax_x+43,fax_y+28,fax_x+206,fax_y+28);
          break;
     case SEND_MODE:
          setcolor(EGA_LIGHTGREEN);
          line(fax_x+43,fax_y+25,fax_x+206,fax_y+25);
          setcolor(EGA_BLACK);
          line(fax_x+43,fax_y+28,fax_x+206,fax_y+28);
          break;
     case DIAL_MODE:
          setcolor(EGA_YELLOW);
          line(fax_x+43,fax_y+25,fax_x+206,fax_y+25);
          setcolor(EGA_BLACK);
          line(fax_x+43,fax_y+28,fax_x+206,fax_y+28);
          break;
     case CONNECT_MODE:
          setcolor(EGA_LIGHTGREEN);
          line(fax_x+43,fax_y+25,fax_x+206,fax_y+25);
          line(fax_x+43,fax_y+28,fax_x+206,fax_y+28);
          break;
     case RECEIVE_MODE:
          setcolor(EGA_BLACK);
          line(fax_x+43,fax_y+25,fax_x+206,fax_y+25);
          setcolor(EGA_LIGHTGREEN);
          line(fax_x+43,fax_y+28,fax_x+206,fax_y+28);
          break;
    /*---------------
     case ERROR_MODE:
          setcolor(EGA_LIGHTRED);
          line(fax_x+43,fax_y+25,fax_x+206,fax_y+25);
          line(fax_x+43,fax_y+28,fax_x+206,fax_y+28);
          break;
      ---------------*/
   }
   MouseShow();
}

void TellFaxProcStatus(int i,int TotalChars)
{
   static int oldx;

   setviewport(0,0,getmaxx(),getmaxy(),1);
   if(i==0)
   {
      oldx=0;
      setcolor(EGA_BLACK);
      line(fax_x+43,fax_y+25,fax_x+206,fax_y+25);
      line(fax_x+43,fax_y+28,fax_x+206,fax_y+28);
   }
   else
   {
      int x=fax_x+43+i*163/TotalChars;
      if(oldx==x)
         return;
      oldx=x;
      setcolor(EGA_LIGHTGREEN);
      line(fax_x+43,fax_y+25,x,fax_y+25);
      line(fax_x+43,fax_y+28,x,fax_y+28);
   }
}

void FaxHint(char *str)
{
   setviewport(0,0,getmaxx(),getmaxy(),1);
   MouseHidden();
   setfillstyle(1,EGA_LIGHTGRAY);
   bar(fax_x+62,fax_y+82,fax_x+FAX_W-5,fax_y+98);

   ViewportDisplayString(str,fax_x+62,fax_y+82,
                          EGA_BLACK,EGA_LIGHTGRAY);
   MouseShow();
}

static void FaxUiOpen()
{
   MouseHidden();
   setfillstyle(1,EGA_LIGHTGRAY);
   bar(fax_x,fax_y,fax_x+FAX_W,fax_y+FAX_H);
   setcolor(EGA_WHITE);
   line(fax_x+1,fax_y+1,fax_x+1,fax_y+FAX_H-1);
   line(fax_x+1,fax_y+1,fax_x+FAX_W-1,fax_y+1);
   setcolor(EGA_DARKGRAY);
   line(fax_x+FAX_W-1,fax_y+1,fax_x+FAX_W-1,fax_y+FAX_H-1);
   line(fax_x+1,fax_y+FAX_H-1,fax_x+FAX_W-1,fax_y+FAX_H-1);
   _putimage(fax_x+8,fax_y+4,faxicon,_GPSET);
   _putimage(fax_x+FAX_W-47,fax_y+4,faxicon,_GPSET);
   setcolor(EGA_BLACK);
   line(fax_x+43,fax_y+25,fax_x+206,fax_y+25);
   line(fax_x+43,fax_y+28,fax_x+206,fax_y+28);

   ViewportDisplayString("工作状态:",fax_x+14,fax_y+56,
                          EGA_BLACK,EGA_LIGHTGRAY);

   setcolor(EGA_WHITE);
   line(fax_x+14,fax_y+76,fax_x+236,fax_y+76);
   setcolor(EGA_DARKGRAY);
   line(fax_x+14,fax_y+75,fax_x+236,fax_y+75);

   ViewportDisplayString("提示:",fax_x+14,fax_y+82,
                          EGA_BLACK,EGA_LIGHTGRAY);
   MouseShow();
}

static struct viewporttype ViewInformation;
static char *pImage;

static void FaxUiClose()
{
   setviewport(0,0,getmaxx(),getmaxy(),1);
   MouseHidden();
   putimage(fax_x,fax_y,pImage,COPY_PUT);
   MouseShow();
   setviewport(ViewInformation.left,ViewInformation.top,
               ViewInformation.right,ViewInformation.bottom,
               ViewInformation.clip);
}

static int InitModem()
{
   char str[80];
   int  len;

   getviewsettings(&ViewInformation);
   setviewport(0,0,getmaxx(),getmaxy(),1);

   fax_x = (getmaxx()-FAX_W)/2;
   fax_y = (getmaxy()-FAX_H)/2;

   len=imagesize(fax_x,fax_y,fax_x+FAX_W,fax_y+FAX_H);
   pImage=(char *)malloc(len);
   //if(pImage==NULL)
   if(pImage<0x1000)
   {
      ReportMemoryError("creatfaxui");
      return -1;
   }
   MouseHidden();
   getimage(fax_x,fax_y,fax_x+FAX_W,fax_y+FAX_H,pImage);
   MouseShow();

   FaxUiOpen();
/*----------------------------------------------------------------*/
   DIS_retryN=3;
   firstRecv=1;
   SendFTHflag=1;
   BpsOfCap=0;
   MinDelay1Row_ms=10;
   flowCtrl=XON; fUseFlow=0;
   fCanBreak=0;

   FaxStatus("初始化调制解调器",NULL_MODE);
   FaxHint("请稍候...");

   SetPortPara(ComX);
   outp(PortBase_4,0);

   SetCommPara();
   SendCmdStr("ATZ0\r");
   if(WaitRetString(250)!=ID_CMDOK)
   {
       myDelay(50);
       SendCmdStr("ATZ\r");
       WaitRetString(250);
   }

   myDelay(5);
   strcpy(str,InitStr);
   strcat(str,"\r");
   SendCmdStr(str);

   if(WaitRetString(300)!=ID_CMDOK)
   {
      MessageBox(GetTitleString(ERRORINFORM),
            "调制解调器设置不正确\n请在<传真通讯设置>中修改串口号",1,1);
      MouseSetGraph(BUSYMOUSE);
      return -1;
   }

   myDelay(5);
   SendCmdStr("ATE0V1\r");      // don't echo, send result as words
   WaitRetString(300);

   myDelay(5);
   sprintf(str,"ATX3S7=%d\r", WaitTime);
   SendCmdStr(str);
   WaitRetString(300);
   return 0;
}

static int DialModem(int fFaxDialing)
{
   int ret;
   char hintstr[80];

   strcpy(DialCmdStr, "ATDT ");         // tone

   myDelay(5);
   if(fTelManualDial==1)
   {
      if(fFaxDialing)
      {
         ret=MessageBox("等待拨号",
                "先用电话拨通对方, 待对方给您\n"
                "传真信号时, 选择<确认>可继续",
                2,0);
      }
      else
      {
         if(fRecv)
             ret=MessageBox("等待接收",
                    "先等对方拨通电话, 与对方通话\n"
                    "结束后, 选择<确认>, 然后挂上\n"
                    "话机可继续接收",
                    2,0);
         else
             ret=MessageBox("等待拨号",
                    "先用电话拨通对方, 与对方通话\n"
                    "结束后, 选择<确认>, 然后挂上\n"
                    "话机就可发送文件了",
                    2,0);
      }

      if(ret)  // cancel
          return ret;

      MouseSetGraph(BUSYMOUSE);
      strcpy(hintstr, "正在连接...");
      strcat(DialCmdStr, ",");
   }
   else
   {
      if(fRecv)
         strcpy(hintstr, "等待对方拨号");
      else
      {
         sprintf(hintstr, "拨号%s",DialNumber);
         strcat(DialCmdStr, DialNumber);
      }
   }

   strcat(DialCmdStr, "\r");
   if(fTelTone==0)
       DialCmdStr[3]='P';       // plus

   // if(fFaxDialing)
   {
       FaxStatus(hintstr,DIAL_MODE);
       FaxHint("按ESC键退出");
   }

   fCanBreak=1;
   SetUserCmdStr("+FCON");

   if(fRecv)
   {
       do {
          ret=WaitRetString(0x1c70);
       } while (ret!=ID_CMDRING && ret!=0x1b);

       if(ret!=0x1b)
       {
          SendCmdStr("ATA\r");
          ret=WaitRetString(0x1c70);
       }
   }
   else
   {
       SendCmdStr(DialCmdStr);
       ret=WaitRetString(146*WaitTime);
   }

   if(ret==0x1b)
   {
      MessageBox(GetTitleString(ERRORINFORM),
            "用户中断拨号!",1,1);
    lbl_dial_err:
      MouseSetGraph(BUSYMOUSE);
      if(fFaxDialing)
      {
         FaxStatus("挂机...",DIAL_MODE);
         FaxHint("通讯结束");
      }

      SendCmdStr("\r");
      myDelay(150);
      WaitRetString(30);
      WaitRetString(30);
      return -3;
   }

   if(ret!=ID_CMDUSER && ret!=ID_CMDCONNECT)
   {
      //puts("phone line busy");
      MessageBox(GetTitleString(ERRORINFORM),
            "无法拨通对方电话!",1,1);
      goto lbl_dial_err;
   }

   fCanBreak = 0;
   FaxStatus("建立连接",CONNECT_MODE);
   FaxHint("等待应答...");

   return 0;
}

static void ModemHangup(int fFax)
{
   MouseSetGraph(BUSYMOUSE);
   FaxStatus("挂机...",DIAL_MODE);
   FaxHint("通讯结束");
   myDelay(20);
   if(fFax)
   {
      SendCmdStr("ATH0\r");
      if(WaitRetString(100)!=ID_CMDOK)
      {
      lbl_force_hangup:
          outp(PortBase_4,0);
          myDelay(200);
          outp(PortBase_4,0xb);
          WaitRetString(200);
          myDelay(0x8);
          SendCmdStr("ATH0\r");
          myDelay(5);
      }
   }
   else
      goto lbl_force_hangup;

   if(fFax)
   {
      FreeFaxData();

      myDelay(5);
      SendCmdStr("AT+FCLASS=0\r");
      myDelay(40);
   }

   FaxUiClose();
   RestoreCommInt();
   MouseSetGraph(ARRAWMOUSE);
}

/*----------------- main ----------------------------------*/
int SendFax()
{
   int ret,len,i,retryN=3,d6234,d623a,d623b;
   char str[80],ch;
   // char RecvrPhoneStr[20];
   int  NowPage=StartPrintPage;
  /*----------------------------------------------------------------*/

   for(i=0;i<20;i++)
   {
      ch=LocalTelId[i];
      if(ch && ch!=0x20) str[i]=LocalTelId[i];
      if(ch==0)
      {
          for(;i<20;i++) str[i]=0x20;
      }
   }
   // memcpy(LocalTelId,str,20);
   for(i=0; i<20; i++)
      TsiFrame[5+i]=str[19-i];

   ReadPageToBuf(NowPage);

 /*--------------------- reset modem ------------------*/
   ret=InitModem();
   if(ret==-1)
      goto lbl_hangup;

 /*--------------------- set modem class ------------------*/
   myDelay(5);
   SendCmdStr("AT+FCLASS=?\r");
   GetRecvStr(str,30);
   str[31]=0;
   WaitRetString(300);

   ret=0;
   len=strlen(str);
   for(i=0;i<len;i++)
   {
      if(str[i]=='1')  { ret = 1; break; }
      else
      if(str[i]=='2')  ret=2;
   }
   if(ret==0)
   {
    lbl_cap_err:
      MessageBox(GetTitleString(ERRORINFORM),
            "此调制解调器不支持传真功能!",1,1);
      ret=-2;
      goto lbl_hangup;
   }
   ClassX=ret;

   myDelay(5);
   if(ClassX==1)
      SendCmdStr("AT+FCLASS=1\r");
   else
      SendCmdStr("AT+FCLASS=2\r");
   WaitRetString(400);

   myDelay(5);
   if(ClassX==1)
   {
      if( GetClass1Cap()==0 )
         goto lbl_cap_err;
   }
   else
   {    /*- class 2 -*/
      if( GetClass2Cap()==0 )
          goto lbl_cap_err;

       /*-------- see subparameter codes, total 8 bytes -------------*/
      // sprintf(str,"AT+FDCC=1,%d,0,0,0,0,0,0\r",BpsOfCap);    // 196dpi
      sprintf(str,"AT+FDCC=%d,%d,0,0,0,0,0,0\r",fFaxFine,BpsOfCap);
      SendCmdStr(str);
      WaitRetString(300);
      myDelay(5);
      sprintf(str,"AT+FLID=\"%-20s\"\r",LocalTelId);
      SendCmdStr(str);
      WaitRetString(600);
   }

 /*--------------------- modem dialing ------------------*/
   ret=DialModem(1);
   if(ret!=0)
      goto lbl_hangup;

 /*--------------------- modem train ------------------*/
   if(ClassX==1)
   {
       d6234=0;
       do {
           myDelay(2);
           if(RecvFrame(str,1350)==ID_CMDBAD)
           {
              if(RecvFrame(str,1000)==ID_CMDBAD)       // retry again
              {
                  // puts("frame received error,abort");
                  ret=1;        // error
                  goto lbl_hangup;
              }
           }

           i=str[4];
           ret=RevTable[i];
           if(ret==1)                   // DIS frame
           {
               d6234=*(short *)&str[0];
               d623a=str[6];
               d623b=str[7];
           }
       } while((str[3]&0x10)!=0x10); /*-bit5:if it isn't Receiver, repeat -*/

       if(d6234==0)
       {
          //puts("No DIS frame,abort");
          ret=1;        // error
          goto lbl_hangup;
       }

     /*-----------------
       for(i=0;i<20;i++)
          str[i]=RecvrPhoneStr[19-i];
       str[20]=0;
       printf("link to fax %s\n",str);
      -----------------*/

       SetBpsArrFlag(d623a,d623b);
       CalculateBps(1);

       retryN=2;

     lbl_train:
       myDelay(10);
       SendComFrame(&TsiFrame,0,600);

       myDelay(8);
       SendComFrame(DcsFrame,1,600);

       myDelay(8);
       SendCmdStr(BpsCmdStr);
       if(WaitRetString(600)!=ID_CMDCONNECT)
       {
          // puts("time out for bps");
          ret=1;        // error
          goto lbl_hangup;
       }

       /*--- modem train ----*/
       EnableSoftFlow();
       myDelay(8);
       // TimerTicks=0;
       for(i=0;i<TrainN;i++)    /*- TCF: send 0 for 1.5 seconds -*/
           while(SendCmdChar(0)) {}

       while(SendCmdChar(DLE)) {}
       while(SendCmdChar(ETX)) {}
       DisableSoftFlow();

       if(WaitRetString(600)!=ID_CMDOK)
       {
          //puts("time out for wait");
          ret=1;        // error
          goto lbl_hangup;
       }

       myDelay(2);
       if(RecvFrame(str,725)==ID_CMDBAD)
       {
          while(SendCmdChar(CAN));
          WaitRetString(30);
          myDelay(2);
          if(retryN--) goto lbl_train;
          //puts("training 3 times, remote fax not response");
          ret=1;        // error
          goto lbl_exit;
       }

    lbl_8349:
       i=str[4];
       ret=RevTable[i];
       switch(ret)
       {
           case 0x21:
                break;
           case 0x22:
                // puts("failed,need retrain");
                if(CalculateBps(0)==0)
                {
                   // puts("unable to train more");
                   ret=1;        // error
                   goto lbl_exit;
                }
                myDelay(120);
           case 0x58:
                goto lbl_train;
           case 0x2:
           case 0x4:
                if((str[3]&0x10)!=0x10 || RecvFrame(str,600)==ID_CMDBAD)
                { ret=1; goto lbl_exit; }
                goto lbl_8349;
           case 0x1:             // DIS
                d6234=*(short *)&str[0];
                d623a=str[6];
                d623b=str[7];
                // puts("resend DCS");
                if(SetBpsArrFlag(d623a,d623b)==0)
                {
                   //puts("DIS received 3 times,DCS not recogonized");
                   ret=1;
                   goto lbl_exit;
                }
                if(CalculateBps(1)!=0)
                   goto lbl_train;
           default:
                ret=1;        // error
                goto lbl_exit;
       } /*- end switch -*/

     lbl_838e:
       TimerTicks=0;
       while(TimerTicks<15);

       SendCmdStr(BpsCmdStr2);
       WaitRetString(TIME_OUT_N);
       EnableSoftFlow();
   }
   else         /*-- class 2 --*/
   {
       do {
           ret=WaitRetString(1350);
           if( ret!=1 && (ret<0xa || ret>0xc) )
              goto lbl_exit;
       } while (ret!=ID_CMDOK);

       myDelay(2);
       SetUserCmdStr("+FDCS:");
       SendCmdStr("AT+FDT\r");
       /*-- negotiation ... --*/
       if (WaitRetString(3000)!=ID_CMDUSER)
       {
           ret=1;
           goto lbl_exit;
       }
       GetRecvStr(str,30);
       str[31]=0;
       CalculateClass2Delay(str);
       EnableSoftFlow();

       if(WaitRetString(1800)!=ID_CMDCONNECT)
       {
           ret=1;
           goto lbl_exit;
       }
       // printf("Connect at %d bps\n",(BpsOfCap+1)*2400);
   }

 /*--------------------- send page ------------------*/
   FaxStatus("开始传真...",SEND_MODE);
 lbl_send_page:
   sprintf(str,"正在传送第%d页",NowPage);
   FaxHint(str);

   MouseHidden();

   myDelay(2);
   for(i=0;i<200;i++)
       while( SendCmdChar(0xff) );

   CharInRow_send=LastChar=0;

   for(i=0;i<TotalChars;i++)
   {
       TellFaxProcStatus(i,TotalChars);
       ch=FaxData[i];
       if(ch==0x80 && LastChar==0)      /*- EOL -*/
       {
           for(;CharInRow_send<=MinChars1Row;CharInRow_send++)
              while( SendCmdChar(0) );       /*- fill line with 0 -*/

           CharInRow_send=0;
       }

       while( SendCmdChar(ch) );
       if(ch==DLE)
           while( SendCmdChar(DLE) );

       LastChar=ch;
       CharInRow_send++;
   }
   while( SendCmdChar(DLE) );
   while( SendCmdChar(ETX) );
   DisableSoftFlow();

   MouseShow();

   if(ClassX==2)
   {
      WaitRetString(0x3e8);
      myDelay(1);
      SetUserCmdStr("+FPTS:");
      if(NowPage<EndPrintPage)          /*-- more pages left --*/
      {
          ret=1;
          SendCmdStr("AT+FET=0\r");
          if(WaitRetString(2000)!=ID_CMDUSER)
          {
              //puts("err1");
              goto lbl_exit;
          }

          GetRecvStr(str,30);
          for(i=0;i<30;i++)
          {
              ch=str[i];
              if(ch>='0' && ch<='9')
                  break;
          }
          ch-='0';

          if(WaitRetString(1000)!=ID_CMDOK)
          {
              // puts("err2");
              goto lbl_exit;
          }

          if(ch==1 || ch==3)
          {
              NowPage++;
              ReadPageToBuf(NowPage);
              SendCmdStr("AT+FDT\r");
              EnableSoftFlow();
              if(ch==1)
              {
                  if(WaitRetString(900)==ID_CMDCONNECT)
                      goto lbl_send_page;
              }
              else
              if(WaitRetString(2000)==ID_CMDCONNECT)
                  goto lbl_send_page;
          }
      }
      else    /*- the lastest page -*/
      {
          ret=1;        // error
          SendCmdStr("AT+FET=2\r");
          if(WaitRetString(2000)!=ID_CMDUSER)
          {
              //puts("err1");
              goto lbl_exit;
          }

          TimerTicks=0;
          GetRecvStr(str,30);
          for(i=0;i<30;i++)
          {
             ch=str[i];
             if(ch>='0' && ch<='9')
                 break;
          }

          if(ch!='0' && ch!='2')
             ret=100;     // ok

           // SetUserCmdStr("+FHNG:");
          WaitRetString(0x3e8);
          WaitRetString(0x3e8);
      }
   }
   else         // class 1
   {
      if(WaitRetString(400)!=ID_CMDOK)
      {
          while(SendCmdChar(CAN));
          WaitRetString(30);
      }

      TimerTicks=0;
      retryN=3;

    lbl_send_ppm:
      ret=1;
      if(NowPage<EndPrintPage)
      {
          while(TimerTicks<18);
          if(SendComFrame(MpsFrame,1,700)==ID_CMDBAD)
          {
              //puts("mps");
              goto lbl_exit;
          }
      }
      else   /*- the lastest page -*/
      {
          while(TimerTicks<19);
          if(SendComFrame(EopFrame,1,700)==ID_CMDBAD)
          {
              //puts("eop");
              goto lbl_exit;
          }
      }

      myDelay(2);
      if(RecvFrame(str,0x384)==ID_CMDBAD)
      {
          // puts("ppm response not received");
          while(SendCmdChar(CAN));
          WaitRetString(30);
          myDelay(2);
          if(retryN--) { /*puts("resend ppm");*/ goto lbl_send_ppm; }
      }
      else
      {
          i=str[4];
          ret=RevTable[i];
          switch(ret)
          {
             case 0x33:
                   // puts("RTP:page received acceptable");
                   if(NowPage>=EndPrintPage)
                       break;
                   NowPage++;
                   ReadPageToBuf(NowPage);
                   goto lbl_train;
             case 0x31:
                   if(NowPage>=EndPrintPage)
                   {
                       //puts("Transmission ok");
                       ret=100;
                       goto lbl_exit;
                   }
                   NowPage++;
                   ReadPageToBuf(NowPage);
                   goto lbl_838e;
             case 0x58:
                   if(retryN--)
                   {
                      //puts("CRP: resend PPM");
                      goto lbl_send_ppm;
                   }
                   //puts("CRP received 3 times,abort");
                   //goto lbl_exit;
             default:
                   //puts("Unknown PPM response, abort");
                   //goto lbl_exit;
                   ret=1;
                   break;
         } /*- end switch -*/
      }
   } /*-------- end of if(class) -------*/

 lbl_exit:

   myDelay(2);
   if(ClassX==1)
      SendComFrame(DcnFrame,1,TIME_OUT_N);

 lbl_hangup:
   ModemHangup(1);

   if(ret==100)
       MessageBox("理德传真",
             "传真发送成功!",1,1);
   else
       MessageBox(GetTitleString(ERRORINFORM),
             "传真发送失败!",1,1);

   return(ret);
}

int SendRecvFile(char *fn)
{
   int ret;
   unsigned char *buffer;
   extern unsigned char *int_buffer;         /* Pointer to interrupt buffer */

   buffer = (unsigned char *) malloc(DAT_LEN*2);
   if (buffer==NULL)
   {
       //puts("no memory");
       MessageBox(GetTitleString(ERRORINFORM),
               "无内存可用",1,1);
       return -1;
   }
   int_buffer = buffer + DAT_LEN;

   ret=InitModem();
   if(ret==-1)
      goto lbl_hangup;

 /*--------------------- modem dialing ------------------*/
   ret=DialModem(0);
   if(ret!=0)
      goto lbl_hangup;

   FaxHint("按ESC键退出");

   if(fRecv)
      RecvFile(buffer);
   else
      SendFile(fn,buffer);

 lbl_hangup:
   ModemHangup(0);

   free (buffer);
   return(ret);
}
