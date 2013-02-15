/*-------------------------------------------------------------------
* Name: ezplogc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

// #define MyDebug

/*-------------------------------------------------------------------*/
static int WaitForNoBusy()
{
   union REGS Reg;
   unsigned int i,k;
   unsigned char *p;
   volatile unsigned char ch;

   _enable();

   Reg.x.eax = 0x9000;
   int386 (0x15, &Reg, &Reg);
   if(Reg.w.cflag==1)
       return 0;

   for(k=0;k<800;k++)
   for(i=0;i<100;i++)
   {
      p=(unsigned char *)0x48e;
      ch=*p;
      if( (ch&0x80)!=0 )         // if hard disk is working
      {
          *p=0;
          return 1;
      }
   }
   return 0;
}

static int s4(unsigned short port1)     // 0x1f7
{
   int i;

   for(i=0;i<512;i++)
     if( (inp(port1)&0x8)!=0 )
         return 1;

   return 0;
}

int DetectHd(unsigned short port1,unsigned char ch)
{
    int i;

    *(unsigned char *)0x48e=0;
    _disable();
    outp( 0xa1, inp(0xa1)&0xbf );
    outp( 0x21, inp(0x21)&0xfb );
    _enable();

    outp (port1, ch+(0xEC-0x50));

    for(i=0;i<5;i++)
    {
       if(WaitForNoBusy())
           break;
       if(s4(port1))
           break;
    }
    return i;
}

static int GetIOaddr(USHORT port1)
{
    int i,retval;
    unsigned char ch;

    ch=port1-(0x1f7-0x50);         // ch=0x50;

    //!!! while (inp (0x1F7) != 0x50);
    i=0;
    // while (inp(port1)!=ch && i<2000) i++;
    while( (inp(port1)&0x80)!=0 && i<2000 ) i++;

    outp (port1-1,ch+(0xA0-0x50));
    i=DetectHd(port1,ch);

    if(i>=5)   /*--- it may be SCSI, or Second IDE ---*/
    {
       retval=-0x10;
    }
    else
       retval=0;

    return retval;
}

typedef struct {
    USHORT  u0;
    USHORT  cylinder;           /* +2 */
    USHORT  u4;
    USHORT  head;               /* +6 */
    USHORT  u8,u10;
    USHORT  sector;             /* +12 */
    USHORT  u14,u16,u18;
    unsigned char SerialNum[20];  /* +20 */
    unsigned char u40[6];
    unsigned char DiskType[46];   /* +46 */
    unsigned char u92[512-92];    /* +92 */
} HardDiskSerialStruct;

long RegistProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  HardDiskSerialStruct DriveData;
  int i,loop,IOaddr;
  unsigned short *p;
  unsigned char ch,*buf,*pch,str[44],type[22];
  USHORT  port1,port2, *ROM_p;
       #ifdef USE_REGIST_DAT
  unsigned char pass[1024],na[40];
       #endif
  // USHORT data1, data2;
  FILE *fp;

  ROM_p= ((USHORT *) 0xffe00);
  //printf("\n%p",ROM_p);
  //for (i=0;i<10;i++) printf("%x,",*ROM_p++);

  switch (Message)
  {
    case WINDOWINIT:            // unused
         i=RadioGetOrder(Window);  /* Page Size */
         MessageGo(Window,SETSTATUS,(long)i+1,0l);
         MessageInsert(Window,SELECTSELECTED,(long)i+1,0l);
         break;
    case GETLOGFILE:            // param1="ezp.log"
         pch=(unsigned char *)LONG2FP(Param1);
         buf=(unsigned char *)LONG2FP(Param2);

       #ifdef USE_REGIST_DAT
         na[0] = 'C'^0x81;   na[1] = ':'^2;
         na[2] = '\\'^0x83;  na[3] = 'e'^4;
         na[4] = 'Z'^0x85;   na[5] = 'p'^6;
         na[6] = '\\'^0x87;  na[7] = 'R'^8;
         na[8] = 'e'^0x89;   na[9] = 'G'^10;
         na[10] = 'i'^0x8b;  na[11] = 'S'^12;
         na[12] = 't'^0x8d;  na[13] = '.'^14;
         na[14] = 'd'^0x8f;  na[15] = 'A'^16;
         na[16] = 't'^0x91;  na[17] = 0;

         for (i=0;i<17;i++)
             if (i&1) na[i] = na[i] ^ (i+1);
              else    na[i] = na[i] ^ (0x81+i);
        // printf("\n%s",na);

         fp=fopen(na,"rb");
         if (fp==NULL) break;
         fread(pass,1,1024,fp);
         fclose(fp);
        #endif

         fp=fopen(pch,"rb");
         if(fp!=NULL)
         {
            fread(buf,1,1000,fp);
            fclose(fp);
       #ifdef USE_REGIST_DAT
            for (i=0;i<999;i++) buf[i] ^= pass[i];
        #endif
         }
         break;
    case SELECTSELECTED:        // unused
         i=RadioGetOrder(Window);
         MessageInsert(Window,REDRAWMESSAGE,0l,GetEditorWidth(WindowGetUserData(Window)));
         if (MessageGo(Window,GETSTATUS,0l,0l))
         {
            TmpPage.PageType=0x1f;
            TmpPage.PageType|=i;
         }
         break;

    case GETDISKSERIAL:
         port1=MAKEHI(Param1);    port2=MAKELO(Param1);

         IOaddr=GetIOaddr(port1);  //0, 0x10, -0x10
           #ifdef MyDebug
         // data1=MAKEHI(Param2);    data2=MAKELO(Param2);
         printf("port=%x,io=%d\n",port1,IOaddr);
         // data1 += IOaddr;
           #endif

         p=(unsigned short *)&DriveData;
         for (loop = 0; loop != 256; loop ++)
         {
              if (IOaddr<0)
                i = *ROM_p++;
              else
                i = inpw (port2);       //  i = inpw (0x1F0);

              if(loop>8) *p = ((i&0xff)<<8) | (i>>8);
              else *p=i;

           #ifdef MyDebug
              printf("%4x ", *p);
           #endif

              if ( (i&1)!=0 )
                 *p ^= 0x7acb;
              else
                 *p += 0x1526;

              p++;
         }

         pch=(unsigned char *)DriveData.SerialNum;
         i=loop=SerialSum=0;
         while(loop<20 && (*pch==0x20) ) { loop++; pch++; }
         if(loop==20) serial[0]=0;
         else
         {
             while(loop++<20) {
                 ch=*pch++;
                 //if(!ch) break;
                 //if( (ch>='0' && ch<='9') || (ch>='A' && ch<='Z') || ch=='.')
                 if( ch<0x40 || ch>0x70)
                    serial[i++]=ch;
             }
         }
           #ifdef MyDebug
         printf("seriallen=%d\n",i);
           #endif

         for(loop=0;loop<i;loop++)
         {
           #ifdef MyDebug
             printf("%d,%d\n",SerialSum,(unsigned short)serial[loop]);
           #endif
             SerialSum+=(unsigned short)serial[loop];
             str[loop]=serial[loop]^0x6e;
         }
         SerialTypeLen=i;

         pch=(unsigned char *)&DriveData.DiskType;
         i=loop=TypeSum=0;
         while(loop<20 && (*pch==0x20) ) { loop++; pch++; }
         if(loop==20) serial[0]=0;
         else
         {
             while(loop++<20) {
                 ch=*pch++;
                 // if(!ch) break;
                 // if( (ch>='0' && ch<='9') || (ch>='A' && ch<='Z') || ch=='.')
                 if( ch<0x40 || ch>0x70)
                    type[i++]=ch;
             }
         }
           #ifdef MyDebug
         printf("typelen=%d\n",i);
           #endif
         for(loop=0;loop<i;loop++)
         {
           #ifdef MyDebug
             printf("%d,%d\n",TypeSum,(unsigned short)type[loop]);
           #endif
             TypeSum+=(unsigned short)type[loop];
             str[loop+SerialTypeLen]=type[loop]^0x48;
         }
         SerialTypeLen+=i;

         pch=(unsigned char *)&DriveData;
         if(!SerialTypeLen)
         {  memset(pch,0xa5,0x80); memset(pch+0x80,0xc3,0x80); }
         else
         {
              int k,n;
              loop=(sizeof(DriveData)/SerialTypeLen)+3;
           #ifdef MyDebug
              printf("loop=%d\n",loop);
           #endif
              for(i=k=0;i<loop;i++)
                for(n=0;n<SerialTypeLen;n++)
                {
                   pch[k++]=str[n];
                   if(k>=sizeof(DriveData)) k=0;
                }
         }
    /*-------------------- for test , 96,1.25 --
           printf("SeiralSum=%x\n",SerialSum);
           printf("TypeSum=%x\n",TypeSum);
           getch();
        -----------------------------*/

            //    pch=(unsigned char *)&DriveData + 20;
         memcpy(PrintName,pch+20,45);
         break;
    default:    // unused
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return TRUE;
}

