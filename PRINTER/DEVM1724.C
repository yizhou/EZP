/*--------- Brother M1724,M2024 dot maxtrix B&W Printer : 180DPI ----------*/
/*-------------------------------------------------------------------
* Name: devM1724.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int lqline;
extern unsigned char *lqbuffer;
extern int blanklines;
extern char CR_LF_cmd[];

static void flush_m1724_BW_line();

static int m1724_init(UDATA pagew,UDATA pageh);
static void m1724_FF()
{
   if (lqline) flush_m1724_BW_line();
   blanklines = 0;
   fputc(0x0c,prnstr);        //FORM FEED
}

static void m1724_over()
{
     fclose(prnstr);
     free(lqbuffer);
     free(rasts[0]);
}

static int m1724_getheight() { return RastHeight; }
static void m1724_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   BW_scanline(x1,x2,y,lpdc);
}

static void send_m1724_BW_line(LPBYTE buf,int cnt);
static void m1724_block()
{
   LPDC lpdc=&SysDc;
   int  i,byteoff;

   byteoff = 0;
   for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
   {
       send_m1724_BW_line(rasts[0]+byteoff, RastWidthByte);
       byteoff += RastWidthByte;
   }
   // if(fDither)
    memset(rasts[0],0,RastSize);        // clear buffer
}

static void m1724_setcolor(int color)
{
   sysColor = color;
}
static void m1724_setGray(int gray)
{
   BW_setGray(gray);
}
static void m1724_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   m1724_setGray(gray);
}
static void m1724_setCMYKcolor(int c,int m,int y,int k)
{
}

static void send_m1724_BW_line(LPBYTE buf,int cnt)
{
   int i,j,n,off;
   int line,bit;

   // n = min(cnt,256);
   n = min(cnt,512);
   line = lqline/8;
   bit = lqline%8;

   if( buf[0]==0 && !memicmp(buf,buf+1,n-1) )  n=0;  // zero line

   for (i=0;i<n;i++)
         for (j=0;j<8;j++) {
             off = (i*8+j)*3+line;
             if (buf[i]&dot1tab[j])
                 lqbuffer[off] |= dot1tab[bit];
         }

   lqline++;
   if (lqline==24)
   {
       lqline=0;
       flush_m1724_BW_line();
   }
}

static void m1724_skip_line(int lines)
{
    int i;

    lines *= 16;   // 24 lines per Row, 16/120==24/180
    while( lines )
    {
         i=min(255,lines);
         fputs("\x1b\x4a",prnstr);
         fputc(i,prnstr);
         fputs(CR_LF_cmd,prnstr);
         lines -= i;
    }
}

static void flush_m1724_BW_line()
{
    //int len = 256*24;
    int len = 512*24;

    while (len>0&&lqbuffer[len-1]==0) len--;
    if(!len)
    {
       blanklines++;
       return;
    }

    if(blanklines)
    {
       m1724_skip_line(blanklines);
       blanklines=0;
    }

    len=len/3+1;                 // change byte to numberof3Bytes(24 dots)
    fputs("\x1b\x47",prnstr);    // \x34==bidirectional, \x47==unidirectional
    fputc(len/256,prnstr); fputc(len%256,prnstr);
    fwrite(lqbuffer,1,len*3,prnstr);

    // free a line
    m1724_skip_line(1);

   //  for (i=0;i<512*24;i++) lqbuffer[i]=0;
    // memset(lqbuffer,0,256*24);
    memset(lqbuffer,0,512*24);
}

PRINTER M1724printer = {
  DEV_BW,
  m1724_init,
  m1724_block,
  m1724_FF,
  m1724_over,
  m1724_getheight,
  m1724_scanfill,
  m1724_setcolor,
  m1724_setRGBcolor,
  m1724_setCMYKcolor,
  m1724_setGray,
  180,
  18*170,                // 180x17
  18*220 ,               // 180*22
  0,0,               //leftmargin=0,  topmargin=0 mm
};

static int m1724_init(UDATA pagew,UDATA pageh)
{
   int m,lines;

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);
   fputs("\033@", prnstr);               /* reset printer */

   // lqbuffer=malloc(256*24);
   lqbuffer=malloc(512*24);
   if(lqbuffer==NULL)
       goto err_exit;

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;
   //lines= printer->ypixel+32;
   lines=240+24;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
            // for other malloc's space   -----------^
   rasts[0]=NULL;

   while (rasts[0]==NULL && lines>24)
   {
       lines -= 24;
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

   // memset(lqbuffer,0,256*24);
   memset(lqbuffer,0,512*24);
   lqline = blanklines = 0;
   return 1;
}

