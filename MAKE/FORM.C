#include <stdio.h>
#include <dos.h>
#include <bios.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <xms1.c>
#include <mem.h>
#include <string.h>

#define  STARTTRACT  30
unsigned char pwsd[6];
int DRV=0;



void mymemswp(unsigned char *s,unsigned char *s1,unsigned int len)
{
   unsigned int i;
   unsigned char ch;
   for (i=0;i<len;i++)
     {
       ch=s[i];
       s[i]=s1[i];
       s1[i]=ch;
     }
}

static int Key1(int track)
{
   return (59*track+track/3)%256;
}

static int Key2(int track)
{
   return ((17+track)*track)%127;
}

int GetKeySect(int track)
{
  int j,i;
  if (track<STARTTRACT) return 18;

  i=track;
  j=(Key1(i)^i)*(Key2(i)-i)%19;
  j=(j+19)%19;
  return j;
}

long int GetSector(long int sector)
{
  long int s;

  if (sector<18*STARTTRACT) return sector;
  sector -= STARTTRACT*18;
  s=1997L*31+231L*229/13;
  s=s-sector+18*(160-STARTTRACT);
  return (s%(18*(160-STARTTRACT)))+18*STARTTRACT;
}

static unsigned char far *DpPtr;
static unsigned char DiskPara[20]=
{0xDF,02,0x25,02,0x13,0x1b,0xFF,0x1e,0xF6,0xF,8,0x4F,00,0,0,0,0,0,0,0 };

static void SaveDp(void)
{
  int i;
  unsigned char ch;
  DpPtr=(unsigned char far *)getvect(0x1e);
  for (i=0;i<11;i++)
  ch=DiskPara[i];
  DiskPara[i]=*(DpPtr+i);
  *(DpPtr+i)=ch;
}

#define RestDp() SaveDp()

static unsigned char SelfBuff[20*512];
static unsigned char SelfBuff1[20*512];
/* static unsigned char BackBuff[20*512]; */

unsigned long GetKey(unsigned char *buff,int len)
{
  int j;
  unsigned long key,ll,*pp;
  unsigned char ch;
  unsigned short cc,*p;
  unsigned char str[]={"ReDtEk CoPyRiGhT 1996"};
  /*
  for (j=0;j<len;j++)
    buff[j] ^= str[j%20];
  */
  p=(unsigned short *)buff;
  pp=(unsigned long *)buff;

     key=0;
     ch=0;
     cc=0;
     ll=0;
     for (j=0;j<len;j++)
        if (buff[j]!=ch)
          {
          ch=buff[j];
          key=key+ch;
          }

     for (j=0;j<len/2;j++)
        if (p[j]!=cc)
          {
          cc =p[j];
          key +=cc;
          }

     for (j=0;j<len/4;j++)
        if (pp[j]!=ll)
          {
          ll =pp[j];
          key ^=ll;
          }
    printf("\nKey:%lu",key);
    return key;
}

void LockSect(unsigned char *buff)
{
 unsigned char *p= &buff[512*18];
 int i,j;
 return ;
 for (i=0;i<512*18;i++)
   {
     j=((i%37)*1997+(i^33)+197*13);
     j= j%128;
     buff[i] ^=p[j];
   }
}

static unsigned char pass[512];
void SetBuff(unsigned char *buff,int sect)
{
  unsigned long Key,*pk;
  int i;

  for (i=0;i<512;i++) buff[18*512+i]=(random(256)&0xff);
  LockSect(buff);
  Key=GetKey(buff,18*512+128);

  *(unsigned long *)(&buff[18*512+128+sect])=key;

  /* memcpy(BackBuff,buff,18*512+512); */

  mymemswp(buff+18*512,buff+sect*512,512);
}

int UnSetBuff(unsigned char *buff,int sect)
{
  unsigned long Key,*pk,Key1;
  int i;

  mymemswp(buff+18*512,buff+sect*512,512);

  pk=(unsigned long *)(&buff[18*512+128+sect]);
  Key=*pk;

  /*
  for (i=0;i<18*512+512;i++)
    if (buff[i]!=BackBuff[i])
     {
      printf("\7eee");
      break;
     }
  */

  Key1=GetKey(buff,18*512+128);

  if (Key==Key1)
  {
    LockSect(buff);
    return 0;
  }
  else
   return -1;
}

void GetSect(unsigned char *buff,long sect)
{
   MoveXMSBlock(XMSHandle,sect*512L,0,(long)buff,512L);
}

int LockFormat(int track,int head)
{
unsigned char TrackPara[20*4];
int i,nsect,j,k;
long sects;
 SaveDp();
 for (i=0;i<18;i++)
  {
     sects=(track*2+head)*18+i;
     sects=GetSector(sects);
     GetSect(SelfBuff+i*512,sects);
  }
 if (track==0&&head==0)
  {
    for (i=0;i<6;i++) SelfBuff[5+i]=pwsd[i];
    SelfBuff[3]=0;
    SelfBuff[4]=0;
  }
 for (i=0;i<19;i++)
 {
  TrackPara[i*4]=track;
  TrackPara[i*4+1]=head;
  TrackPara[i*4+2]=i+1;
  TrackPara[i*4+3]=2;
 }

  i=(GetKeySect(head+track*2));
  nsect=18+(i&1);

  /* nsect=19; */

  if (nsect==19)
  {
     /* printf("\nTrack:%d,Head:%d",track,head); */
     SetBuff(SelfBuff,i);
  }
  else
  {

    if (track>=STARTTRACT)
       for (j=0;j<512*18;j++) SelfBuff[j] ^= pwsd[j%6];

  }

  /*
  if (nsect==18&&track==9&&head==0)
    nsect++;

  if (nsect==18&&track==13&&head==1)
    nsect++;
  */

  /* printf("\n%d",nsect); */

  *(DpPtr+4)=nsect;

  for (i=0;i<3;i++)
  {
    j=biosdisk(5,DRV,head,track,1,nsect,TrackPara);
    if (!j)
      {
        j=biosdisk(3,DRV,head,track,1,nsect,SelfBuff);
        memset(SelfBuff1,97,nsect*512);
        biosdisk(2,DRV,head,track,1,nsect,SelfBuff1);
        for (k=0;k<nsect*512;k++) if (SelfBuff[k]!=SelfBuff1[k]) break;
        if (k==nsect*512) break;
      }
    else j=-1;
  }
 RestDp();
 memset(SelfBuff,0,nsect*512);
 return j;
}

int LockFormat80(int track,int head)
{
unsigned char TrackPara[20*4];
int i,nsect,j,k;
long sects;
 SaveDp();

 for (i=0;i<18;i++)
  {
     sects=((track-1)*2+head)*18+i;
     sects=GetSector(sects);
     GetSect(SelfBuff+i*512,sects);
  }

 for (i=0;i<18;i++)
 {
  TrackPara[i*4]=track;
  TrackPara[i*4+1]=head;
  TrackPara[i*4+2]=i+1;
  TrackPara[i*4+3]=random(5)+3;
 }

 TrackPara[5*4+3]=2;

 nsect=18;

 *(DpPtr+4)=nsect;

 for (i=0;i<3;i++)
 {
    j=biosdisk(5,DRV,head,track,1,nsect,TrackPara);
    if (!j)
      {
        SelfBuff[6]='a';
        SelfBuff[7]=0;
        j=biosdisk(3,DRV,head,track,6,1,SelfBuff);
        memset(SelfBuff1,97,nsect*512);
        biosdisk(2,DRV,head,track,6,1,SelfBuff1);
        for (k=0;k<512;k++) if (SelfBuff[k]!=SelfBuff1[k]) break;
        if (k==512) break;
      }
    else j=-1;
  }
 RestDp();
 memset(SelfBuff,0,nsect*512);
 return j;
}

int LockRead(int track,int head)
{
int i,nsect,j,k;
long sects;
 SaveDp();

  i=(GetKeySect(head+track*2));
  nsect=18+(i&1);



  *(DpPtr+4)=nsect;

  memset(SelfBuff,0,nsect*512);
  for (k=0;k<3;k++)
  {
   j=biosdisk(2,DRV,head,track,1,nsect,SelfBuff);
   if (!j) break;
  }

 RestDp();

 if (nsect==19)
  {
     j=UnSetBuff(SelfBuff,i);
  }

 return j;
}

void getkey(unsigned char far *p1,unsigned char far *p2)
{
unsigned short i,j,len;
unsigned char ch;
j=0;
for (i=0;i<4094;i=i+2)
 if (p1[i]!=p1[i+2]||p1[i+1]!=p1[i+3])
 {
 p2[j++]=p1[i]; p2[j++]=p1[i+1];
 }
 len=j;
 for (i=len;i<1024;i++)
   {
      j=((i%17)*1997+(i^33)+197*13);
      ch=(j&0xff);
      p2[i]=ch;
   }
 for (i=0;i<1024;i++)
   {
      ch=p2[i];
      j=((i+3+ch)%256);
      j=(j*j-j+5);
      j=((j%17)*1997+(j^33)+197*13);
      ch=(j&0xff);
      p2[i] ^= ch;
   }
   p2[5]=('Z'^0x5a);
   p2[29]=('J'^0x5a);
   p2[70]=('H'^0x5a);
}

main(int nn,char *np[])
{
int i,j,err=0;
int mx,my;
int dr=1;
FILE *fp;
clock_t st=clock();
char cmmd[]={"mycopy regist.dat b:regist.dat"};
/*
i=GetSector(1005);
printf("\n%ld,%ld,%ld",GetSector(1080),GetSector(1018),GetSector(1000));

getch();
exit(0);
*/
if (nn>1)
 {
   if ((np[1][0]|0x20)=='b') DRV=1;
 }

 cmmd[18]=DRV+'a';
/*
for (i=0;i<160*18;i++)
  if (GetSector(GetSector(i))!=i)
    {
    printf("\nError");
    exit(0);
    }
printf("\nOk");
exit(0);
*/
clrscr();
if (XMSInit())
{
    printf("\nMemory out !");
    exit(0);
}

if (read_file(0L,"InstDisk.dat")==-1)
{
    XMSOver();
    printf("\nCan't open file InstDisk.dat");
    exit(0);
}

_AH=0x8;
_DL=(dr&1);
geninterrupt(0x13);

mx=1;
my=wherey();
randomize();
loop_1:

fp=fopen("pswd.dat","rb");
if (fp==NULL)
 {
    printf("\nPassword file no exist !");
    err=0;
    goto bye;
 }
i=fread(SelfBuff,1,4096,fp);
if (i!=4096)
 {
    printf("\nPassword file error !");
    err=0;
    goto bye;
 }

i=fread(pwsd,1,6,fp);
if (i!=6)
 {
    printf("\nPassword file error !");
    err=0;
    goto bye;
 }
fclose(fp);

getkey(SelfBuff,SelfBuff+4096);
fp=fopen("regist.dat","wb");
if (fp==NULL)
 {
    printf("\nCan't open regist.dat !");
    err=0;
    goto bye;
 }
i=fwrite(SelfBuff+4096,1,1024,fp);
if (i!=1024)
 {
    printf("\nWrite regist.dat error !");
    err=0;
    goto bye;
 }

fclose(fp);
/*
_DL=DRV;
_AX=0x1703;
geninterrupt(0x13);
*/
if ((err=LockFormat80(80,1))!=0) goto bye;

for (i=0;i<80;i++)
 for (j=0;j<2;j++)
  if ((err=LockFormat(i,j))!=0)
  {
    goto bye;
  }
  else
  {
  /*
  err=LockRead(i,j);
  if (err) goto bye;
  */
  gotoxy(mx,my);
  printf("Track:%2d    Head:%2d     Time:%ld Secs ",i,j,(long)((clock())/18.2f));

  if (bioskey(1))
  {
    bioskey(0);
    printf("\nAbort ?(Y/N)");
    if ((getch()|0x20)=='y')
      {
        err=1;
        goto bye;
      }
  }
  }

  /*
  printf("\nInsert another disk and Press any key continue...");
  if (getch()==27) goto bye;

  clrscr();
  goto loop_1;
  */

  system(cmmd);
bye:
  XMSOver();
  if (err)
   printf("\nDisk Copy Error:%d ",err);
  else
   printf("\nDisk Copy Ok !");
  /*
  _DL=DRV;
  _AX=0x1700;
  geninterrupt(0x13);
  */
}



