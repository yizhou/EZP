/*--------- Epson dot maxtrix B&W Printer : 180DPI ----------*/
/*-------------------------------------------------------------------
* Name: devbj10.c       360 DPI LQ compatible
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int lqline;
extern unsigned char *lqbuffer;
static void flush_lq_BW_line();

static int bj10_init(UDATA pagew,UDATA pageh)
{
   int m,lines;

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);

   fwrite("\033@\033U0",1,5,prnstr);

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;
   //lines= printer->ypixel+32;
   lines=512+48;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
            // for other malloc's space   -----------^
   rasts[0]=NULL;

   lqbuffer=malloc(512*48);
   if(lqbuffer==NULL)
       goto err_exit;

   while (rasts[0]==NULL && lines>48)
   {
       lines -= 48;
       m = lines*RastWidthByte;
       rasts[0] = malloc(m);
   }

   if(rasts[0]==NULL)
   {
    err_exit:
       fclose(prnstr);
       return(-1);
   }

   RastHeight=lines;
   RastSize=m;
   memset(rasts[0],0,RastSize);         // clear it

   memset(lqbuffer,0,512*48);
   lqline = 0;
   return 1;
}

static void bj10_FF()
{
   if (lqline) flush_lq_BW_line();
   fputs("\f", prnstr);
}

static void bj10_over()
{
     fputs("\033@", prnstr);
     fclose(prnstr);
     free(lqbuffer);
     free(rasts[0]);
}

static int bj10_getheight() { return RastHeight; }
static void bj10_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   BW_scanline(x1,x2,y,lpdc);
}

static void send_lq_BW_line(LPBYTE buf,int cnt);
static void bj10_block()
{
   LPDC lpdc=&SysDc;
   int  i,byteoff;

   byteoff = 0;
   for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
   {
      send_lq_BW_line(rasts[0]+byteoff, RastWidthByte);
      byteoff += RastWidthByte;
   }
  // if(fDither)
   memset(rasts[0],0,RastSize);        // clear buffer
}

static void bj10_setcolor(int color)
{
   sysColor = color;
}
static void bj10_setGray(int gray)
{
   BW_setGray(gray);
}
static void bj10_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   bj10_setGray(gray);
}
static void bj10_setCMYKcolor(int c,int m,int y,int k)
{
}


static void send_lq_BW_line(LPBYTE buf,int cnt)
{
   int i,j,n,off;
   int line,bit;

   n = min(cnt,512);   // n = min(cnt,512);
   line = lqline/8;
   bit = lqline%8;

   if( buf[0]==0 && !memicmp(buf,buf+1,n-1) )  n=0;  // zero line

   for (i=0;i<n;i++)
         for (j=0;j<8;j++) {
             off = (i*8+j)*6+line;
             if (buf[i]&dot1tab[j])
                 lqbuffer[off] |= dot1tab[bit];
         }

   lqline++;
   if (lqline==48)
   {
       lqline=0;
       flush_lq_BW_line();
   }
}

static void flush_lq_BW_line()
{
    int len = 512*48;    // len = 512*24;

    while (len>0&&lqbuffer[len-1]==0) len--;
    if(len)
    {
       len=len/6+1;                 // change byte to numberof3Bytes(48 dots)
       fputs("\x1b*H",prnstr);
       fputc(len%256,prnstr); fputc(len/256,prnstr);
       fwrite(lqbuffer,1,len*6,prnstr);
       fputs("\x0d",prnstr);
    }
    // free a line
    fputs("\x1b+0\x0a",prnstr);
    memset(lqbuffer,0,512*48);
}

PRINTER BJ10eprinter = {
  DEV_BW,
  bj10_init,
  bj10_block,
  bj10_FF,
  bj10_over,
  bj10_getheight,
  bj10_scanfill,
  bj10_setcolor,
  bj10_setRGBcolor,
  bj10_setCMYKcolor,
  bj10_setGray,
  360,
  36*85,                // 360x8.5
  36*108,               // 360x10.8
  0,128,
};
