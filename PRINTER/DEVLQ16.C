/*--------- Epson dot maxtrix B&W Printer : 180DPI ----------*/
/*-------------------------------------------------------------------
* Name: devlq16.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int lqline;
extern unsigned char *lqbuffer;
static void flush_lq_BW_line();
extern int blanklines;
//int  lq_width;
char CR_LF_cmd[]="\xd\xa";

static int lq16_init();

static void lq16_FF()
{
   if (lqline) flush_lq_BW_line();
   blanklines = 0;
   fputc(0x0c,prnstr);        //FORM FEED
}

static void lq16_over()
{
     fclose(prnstr);
     free(lqbuffer);
     free(rasts[0]);
}

static int lq16_getheight() { return RastHeight; }
static void lq16_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   BW_scanline(x1,x2,y,lpdc);
}

static void send_lq_BW_line(LPBYTE buf,int cnt);
static void lq16_block()
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

static void lq16_setcolor(int color)
{
   sysColor = color;
}
static void lq16_setGray(int gray)
{
   BW_setGray(gray);
}
static void lq16_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   lq16_setGray(gray);
}
static void lq16_setCMYKcolor(int c,int m,int y,int k)
{
}

static void send_lq_BW_line(LPBYTE buf,int cnt)
{
   int i,j,n,off;
   int line,bit;

   n = min(cnt,512);
   // n = min(cnt,lq_width/8);
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
       flush_lq_BW_line();
   }
}

void epsonLQ_skip_line(int lines)
{
    int i;

    lines *= 24;   // 24 lines per Row
    while( lines )
    {
         i=min(255,lines);
         fputs("\x1b\x33",prnstr);
         fputc(i,prnstr);
         fputs(CR_LF_cmd,prnstr);
         lines -= i;
    }
}

static void flush_lq_BW_line()
{
    // int len = lq_width*3;
    int i, len = 512*24;

    while (len>0&&lqbuffer[len-1]==0) len--;
    if(!len)
    {
       blanklines++;
       return;
    }

    if(blanklines)
    {
       epsonLQ_skip_line(blanklines);
       blanklines=0;
    }

    len=len/3+1;                 // change byte to numberof3Bytes(24 dots)
    fputs("\x1b\x2a\x27",prnstr);
    fputc(len%256,prnstr); fputc(len/256,prnstr);
    fwrite(lqbuffer,1,len*3,prnstr);

    // free a line
    epsonLQ_skip_line(1);

    memset(lqbuffer,0,512*24);
    // memset(lqbuffer,0,lq_width*3);
}

PRINTER LQBWprinter = {
  DEV_BW,
  lq16_init,
  lq16_block,
  lq16_FF,
  lq16_over,
  lq16_getheight,
  lq16_scanfill,
  lq16_setcolor,
  lq16_setRGBcolor,
  lq16_setCMYKcolor,
  lq16_setGray,
  180,
  18*170,                //  180x17
  18*220,                //  180*22
  0,0,   // 53,               //leftmargin=0,  topmargin=7.5 mm
};

static int lq16_init(UDATA pagew,UDATA pageh)
{
   int m,lines;
   //  Pages *MidPage;      By zjh 96.9.7

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);
   fputs("\033@", prnstr);               /* reset printer */

   //MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));  By zjh 96.9.7
   //lq_width=0.5+pagew*180/SCALEMETER;
   //0.5+PageGetPageWidth(MidPage)*180/SCALEMETER;   By zjh 96.9.7
   // LQBWprinter.ypixel = 0.5+pageh*180/SCALEMETER;
   //0.5+PageGetPageHeight(MidPage)*180/SCALEMETER;  By zjh 96.9.7

   //HandleUnlock(ItemGetHandle(GlobalCurrentPage));   By zjh 96.9.7

   //lq_width=((lq_width+7)/8)*8;
   //if(lq_width>512*8) lq_width=512*8;
   //LQBWprinter.xpixel = lq_width;

   // lqbuffer=malloc(lq_width/8*24);
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

   memset(lqbuffer,0,512*24);
   //memset(lqbuffer,0,lq_width*3);
   lqline = blanklines = 0;
   return 1;
}

