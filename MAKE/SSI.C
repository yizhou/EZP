#include <bios.h>
#include <stdio.h>
unsigned char buff[512*20];
unsigned char my_buff[512];
main(int n,char *p[])
{
FILE *fp;
int i,j,k,r;
int tr,sr,dr;
if (n<2)
{
printf("Usage:SSI [disk] [file]\n");
exit(0);
}
if (p[1][0]=='a'||p[1][0]=='A') dr=0; else dr=1;
fp=fopen(p[2],"wb");
if (fp==NULL)
{
printf("Can't open file !");
exit(1);
}

for (r=0;r<3;r++)
{
k=biosdisk(2,dr,0,0,1,1,buff);
if (k==0) break;
}

if (k!=0)
{
printf("\nCan't read boot sector !");
exit(0);
}
clrscr();
sr=buff[0x18];
tr=(buff[0x14]*256+buff[0x13])/sr/2;
for (i=0;i<tr;i++) for (j=0;j<2;j++)
{
if (bioskey(1)==0x011b) goto bye;
for (r=0;r<3;r++)
{
k=biosdisk(2,dr,j,i,1,sr,buff);
if (k==0) break;
}
if (k!=0)
{
printf("\nErr:Head:%d Track:%d",j,i);
}
else
{
gotoxy(1,1); printf("Read Head:%d Track:%d",j,i);
}
fwrite(buff,1,sr*512,fp);
}
bye:
fclose(fp);
}