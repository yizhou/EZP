#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>

#define PORT_CONST      97

#ifndef USHORT
  typedef unsigned short int  USHORT;
#endif

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
char serial[22],SerialTypeLen,type[22];
HardDiskSerialStruct DriveData;

USHORT SerialSum=0,TypeSum=0;


unsigned char pass[1024],buf[1024];

static int GetIOaddr(unsigned short port1)
{
    int i,retval;
    unsigned char ch;

    port1 += PORT_CONST;
    ch=port1-(0x1f7-0x50);         // ch=0x50;

    i=0;
    _enable();
    //!!! while (inp (0x1F7) != 0x50);
    while (inp(port1)!=ch && i<2000) i++;

    _disable();

    outp (port1-1,ch+(0xA0-0x50));
    outp (port1, ch+(0xEC-0x50));

    //!!! while (inp (0x1F7) != 0x58);
    while (inp(port1)!=ch+8 && i<20000) i++;

    if(i>19097)   /*--- it may be SCSI, or Second IDE ---*/
    {
        _enable();
        i=0;
        while (inp(port1)!=ch && i<2000) i++;
        _disable();
        outp (port1-1, ch+(0xB0-0x50));
        outp (port1, ch+(0xEC-0x50));
        while (inp(port1)!=ch+8 && i<20000) i++;
        if(i>19697)   /*--- it may be SCSI ---*/
          retval=-0x10;
        else
          retval=0x10;
    }  /*-- second IDE --*/

    else
      retval=0;

    //_enable();
    return retval;
}

void hdregist()
{
    int IOaddr;
    unsigned int i,loop;
    unsigned short *p,port2=0x1f0-79,port1=0x1f7-PORT_CONST,*ROM_p;
    unsigned char ch,*pch,str[48];

    ROM_p=((unsigned short *)0xf000fe00);

 /*-------
    while (inp (0x1F7) != 0x50);
    _disable();
    outp (0x1F6,0xA0);
    outp (0x1F7,0xEC);
    while (inp (0x1F7) != 0x58);
  -----------*/
    IOaddr=GetIOaddr(port1);
    p=(unsigned short *)&DriveData;

    for (loop = 0; loop < 256; loop ++)
    {
         if (IOaddr<0)
            i = *ROM_p++;
         else
             i = inpw (port2+79);       //  i = inpw (0x1F0);

         if(loop>8) *p = ((i&0xff)<<8) | (i>>8);
         else *p=i;

         if (i&1)      //By zjh 1997.01.12  for Different from vision 1.2 1.3
            *p ^= 0x7acb;
         else
            *p += 0x1526;
         p++;
    }
    _enable();

    pch=(unsigned char *)DriveData.SerialNum;
    i=loop=0;
    while(loop<20 && (*pch==0x20) ) { loop++; pch++; }
    if(loop==20) serial[0]=0;
    else {
        while(loop++<20) {
            ch=*pch++;
            //if(!ch) break;
            //if( (ch>='0' && ch<='9') || (ch>='A' && ch<='Z') || ch=='.')
            if( ch<0x40 || ch>0x70)
               serial[i++]=ch;
        }
    }
    for(loop=0;loop<i;loop++) {
        SerialSum+=(unsigned char)serial[loop];
        str[loop]=serial[loop]^0x6e;
    }
    SerialTypeLen=i;

    pch=(unsigned char *)&DriveData.DiskType;
    i=loop=0;
    while(loop<20 && (*pch==0x20) ) { loop++; pch++; }
    if(loop==20) serial[0]=0;
    else {
        while(loop++<20) {
            ch=*pch++;
            //if(!ch) break;
            //if( (ch>='0' && ch<='9') || (ch>='A' && ch<='Z') || ch=='.')
            if( ch<0x40 || ch>0x70)
               type[i++]=ch;
        }
    }
    for(loop=0;loop<i;loop++) {
        TypeSum+=(unsigned char)type[loop];
        str[loop+SerialTypeLen]=type[loop]^0x48;
    }
    SerialTypeLen+=i;

    pch=(unsigned char *)&DriveData;
    if(!SerialTypeLen)
    {  memset(pch,0xa5,0x80); memset(pch+0x80,0xc3,0x80); }
    else {
         int k,n;
         loop=(sizeof(DriveData)/SerialTypeLen)+3;
         for(i=k=0;i<loop;i++)
           for(n=0;n<SerialTypeLen;n++)
           {
              pch[k++]=str[n];
              if(k>=sizeof(DriveData)) k=0;
           }
    }
  /*---------------
    printf("serialSum=%x, typeSum=%x\n", SerialSum, TypeSum);
    printf("serial=%s, type=%s\n",serial,type);
    ---------------*/
} /* hdregist */

const char ezpname[]="c:/ezp/ezp.log";
main()
{
   FILE *fp;
   unsigned char *buf;
   unsigned short *pp=(unsigned short *)&buf[0];
   unsigned short code[2];
   int i,k,n;

    fp=fopen("c:/ezp/regist.dat","rb");
    if(fp==NULL)
    {
       printf("can not open regist.dat\n");
       exit(2);
    }
    fread(pass,1,1024,fp);
    fclose(fp);

    hdregist();

    buf[257]=2;
    code[0]=0;
    code[1]=2;
    memcpy(&buf[258],code,4);

    i=298+20-4;
    memcpy(&buf[298],(char *)&DriveData,sizeof(DriveData));
    pp[i/2]=SerialSum;
    pp[i/2+1]=TypeSum;

    for(n=i/2;n<(i+44)/2;n++)
        for(k=0;k<2;k++)
          pp[n] -= code[k];

    for (i=0;i<1024;i++) buf[i] ^= pass[i];

    _dos_setfileattr(ezpname,_A_NORMAL);
    unlink(ezpname);

    fp=fopen(ezpname,"wb");
    if(fp==NULL)
    {
       printf("can not creat ezp.log\n");
       exit(2);
    }
    fwrite(buf,1,1024,fp);
    fclose(fp);
}
