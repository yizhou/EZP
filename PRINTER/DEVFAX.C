/*-------------------------------------------------------------------
* Name: devfax.c       Fax 204x98 DPI
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern const unsigned char RevTable[256];

static int faxn;
static int fax_init(UDATA pagew,UDATA pageh)
{
   int m,lines;
   char filename[80];

   faxn=0;
   GetFaxFilename(faxn,filename);
   if ((prnstr=fopen(filename,"wb"))==NULL)
       return(-1);

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;
   printer->ypixel = 0.5+pageh*204/SCALEMETER;

   //lines= printer->ypixel+32;
   lines=512+48;
   //lines=(16*1024*1024)/(4*RastWidthByte+3*RastWidth)/4+32;
   rasts[0]=NULL;
   while (rasts[0]==NULL && lines>48)
   {
       lines -= 48;
       m = lines*RastWidthByte;
       rasts[0] = malloc(m);
   }

   if(rasts[0]==NULL)
   {
       fclose(prnstr);
       return(-1);
   }

   RastHeight=lines;
   RastSize=m;
   memset(rasts[0],0,RastSize);         // clear it
   return 1;
}

static void fax_FF()
{
   char filename[80];
   int i;

   for(i=0;i<3;i++)
   {
       putc(0,prnstr);
       putc(0x8,prnstr);
       putc(0x80,prnstr);
   }
   putc(0,prnstr);

   if(StartPrintPage+faxn<EndPrintPage)
   {
       fclose(prnstr);
       faxn++;
       GetFaxFilename(faxn,filename);
       prnstr=fopen(filename,"wb");
   }
}

static void fax_over()
{
   fclose(prnstr);
   free(rasts[0]);
}

static int fax_getheight() { return RastHeight; }
static void fax_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   BW_scanline(x1,x2,y,lpdc);
}


static void fax_setcolor(int color)
{
   sysColor = color;
}
static void fax_setGray(int gray)
{
   BW_setGray(gray);
}
static void fax_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   fax_setGray(gray);
}
static void fax_setCMYKcolor(int c,int m,int y,int k)
{
}

/*--------------------- CCITT G3 compress --------------------*/
typedef struct {
  unsigned char code;
  int bitn;
} BITPAIR;

/* White run termination codes. */
static BITPAIR wterm[64] =
{
        {0x35, 8}, {0x7, 6},  {0x7, 4},  {0x8, 4},
        {0xb, 4},  {0xc, 4},  {0xe, 4},  {0xf, 4},
        {0x13, 5}, {0x14, 5}, {0x7, 5},  {0x8, 5},
        {0x8, 6},  {0x3, 6},  {0x34, 6}, {0x35, 6},
        {0x2a, 6}, {0x2b, 6}, {0x27, 7}, {0xc, 7},
        {0x8, 7},  {0x17, 7}, {0x3, 7},  {0x4, 7},
        {0x28, 7}, {0x2b, 7}, {0x13, 7}, {0x24, 7},
        {0x18, 7}, {0x2, 8},  {0x3, 8},  {0x1a, 8},
        {0x1b, 8}, {0x12, 8}, {0x13, 8}, {0x14, 8},
        {0x15, 8}, {0x16, 8}, {0x17, 8}, {0x28, 8},
        {0x29, 8}, {0x2a, 8}, {0x2b, 8}, {0x2c, 8},
        {0x2d, 8}, {0x4, 8},  {0x5, 8},  {0xa, 8},
        {0xb, 8},  {0x52, 8}, {0x53, 8}, {0x54, 8},
        {0x55, 8}, {0x24, 8}, {0x25, 8}, {0x58, 8},
        {0x59, 8}, {0x5a, 8}, {0x5b, 8}, {0x4a, 8},
        {0x4b, 8}, {0x32, 8}, {0x33, 8}, {0x34, 8}
};

/* White run make-up codes. */
static BITPAIR wmakeup[41] =
{
        {0, 0}, {0x1b, 5}, {0x12, 5}, {0x17, 6},
        {0x37, 7}, {0x36, 8}, {0x37, 8}, {0x64, 8},
        {0x65, 8}, {0x68, 8}, {0x67, 8}, {0xcc, 9},
        {0xcd, 9}, {0xd2, 9}, {0xd3, 9}, {0xd4, 9},
        {0xd5, 9}, {0xd6, 9}, {0xd7, 9}, {0xd8, 9},
        {0xd9, 9}, {0xda, 9}, {0xdb, 9}, {0x98, 9},
        {0x99, 9}, {0x9a, 9}, {0x18, 6}, {0x9b, 9},
        {0x8, 11}, {0xc, 11}, {0xd, 11}, {0x12, 12},
        {0x13, 12}, {0x14, 12}, {0x15, 12}, {0x16, 12},
        {0x17, 12}, {0x1c, 12}, {0x1d, 12}, {0x1e, 12},
        {0x1f, 12}
};

/* Black run termination codes. */
static BITPAIR bterm[64] =
{
        {0x37, 10}, {0x2, 3},   {0x3, 2},   {0x2, 2},
        {0x3, 3},   {0x3, 4},   {0x2, 4},   {0x3, 5},
        {0x5, 6},   {0x4, 6},   {0x4, 7},   {0x5, 7},
        {0x7, 7},   {0x4, 8},   {0x7, 8},   {0x18, 9},
        {0x17, 10}, {0x18, 10}, {0x8, 10},  {0x67, 11},
        {0x68, 11}, {0x6c, 11}, {0x37, 11}, {0x28, 11},
        {0x17, 11}, {0x18, 11}, {0xca, 12}, {0xcb, 12},
        {0xcc, 12}, {0xcd, 12}, {0x68, 12}, {0x69, 12},
        {0x6a, 12}, {0x6b, 12}, {0xd2, 12}, {0xd3, 12},
        {0xd4, 12}, {0xd5, 12}, {0xd6, 12}, {0xd7, 12},
        {0x6c, 12}, {0x6d, 12}, {0xda, 12}, {0xdb, 12},
        {0x54, 12}, {0x55, 12}, {0x56, 12}, {0x57, 12},
        {0x64, 12}, {0x65, 12}, {0x52, 12}, {0x53, 12},
        {0x24, 12}, {0x37, 12}, {0x38, 12}, {0x27, 12},
        {0x28, 12}, {0x58, 12}, {0x59, 12}, {0x2b, 12},
        {0x2c, 12}, {0x5a, 12}, {0x66, 12}, {0x67, 12}
};

/* Black run make-up codes. */
static BITPAIR bmakeup[41] =
{
        {0, 0}, {0xf, 10}, {0xc8, 12}, {0xc9, 12},
        {0x5b, 12}, {0x33, 12}, {0x34, 12}, {0x35, 12},
        {0x6c, 13}, {0x6d, 13}, {0x4a, 13}, {0x4b, 13},
        {0x4c, 13}, {0x4d, 13}, {0x72, 13}, {0x73, 13},
        {0x74, 13}, {0x75, 13}, {0x76, 13}, {0x77, 13},
        {0x52, 13}, {0x53, 13}, {0x54, 13}, {0x55, 13},
        {0x5a, 13}, {0x5b, 13}, {0x64, 13}, {0x65, 13},
        {0x8, 11}, {0xc, 11}, {0xd, 11}, {0x12, 12},
        {0x13, 12}, {0x14, 12}, {0x15, 12}, {0x16, 12},
        {0x17, 12}, {0x1c, 12}, {0x1d, 12}, {0x1e, 12},
        {0x1f, 12}
};

static int findblack(unsigned char ch)
{
   int i=0;
   unsigned int j=0x80;

   while (i<8 && !(j&ch))
   {
     i++;
     j >>=1;
   }
   return i;
}

static unsigned char mask1[]= {0xff,0x7f,0x3f,0x1f,0x0f,7,3,1};
static int CheckWhiteRun(char *ibuf,int x,int bytesperline)
{
   int xbyte,xbit,xbit1,x1;
   int run = 0;
   int byteoff = x/8;

   xbyte = ibuf[byteoff];
   xbit = x % 8;
   xbit1 = 8-xbit;

   xbyte &= mask1[xbit];
   if (xbyte !=0)
   {
      x1 = findblack(xbyte);
      return x1-xbit;
   }

   run = xbit1;
   byteoff++;
   while(byteoff<bytesperline && ibuf[byteoff] ==0)
   {
      run+=8;
      byteoff ++;
   }

   if (byteoff<bytesperline) x1 = findblack(ibuf[byteoff]);
   else x1 = 0;
   return run+x1;
}

static int findwhite(unsigned char ch)
{
   int i=0;
   unsigned char j=0x80;

   while (i<8 && ((~j)|ch)&j)
   {
     i++;
     j >>=1;
   }
   return i;
}

static int CheckBlackRun(unsigned char *ibuf,int x,int bytesperline)
{
   int xbyte,xbit,xbit1,x1;
   int run = 0;
   int byteoff = x/8;

   xbyte = ibuf[byteoff];
   xbit = x % 8;
   xbit1 = 8-xbit;

   xbyte |= (~mask1[xbit]);
   if (xbyte !=0xff)
   {
      x1 = findwhite(xbyte);
      return x1-xbit;
   }

   run = xbit1;
   byteoff++;
   while(byteoff<bytesperline && ibuf[byteoff] ==0xff) {
      run+=8;
      byteoff ++;
   }

   if (byteoff<bytesperline) x1 = findwhite(ibuf[byteoff]);
   else x1 = 0;
   return run+x1;
}

static unsigned char linebuf[1024];
static int byteoff = 0;
static int bitoff  = 0;
static void InitRun(void)
{
   byteoff = 0;
   bitoff = 0;
   memset(linebuf,0,1024);
}

static PutRun(unsigned char value, int bitn)
{
   unsigned char ch;
   int off;

   if (bitn>=8)
   {
        ch = RevTable[value];
        bitoff += (bitn-8);

        if (bitoff>7)
        {
            bitoff -=8;
            byteoff++;
        }

        if (bitoff==0)
        {
            linebuf[byteoff] = ch;
            byteoff++;
        }
        else
        {
            linebuf[byteoff] |= (ch << bitoff);
            byteoff++;
            linebuf[byteoff] |= (ch>>(8-bitoff));
        }
   }
   else
   {
        ch = RevTable[value]>>(8-bitn);
        if (bitoff == 0 )
        {
            linebuf[byteoff] = ch;
            bitoff = bitn;
        }
        else
        {
            linebuf[byteoff] |= (ch<< bitoff);
            off = 8-bitoff;
            bitoff += bitn;
            if (bitoff>7)
            {
               bitoff -= 8;
               byteoff++;
               linebuf[byteoff] |= (ch >> (bitn-bitoff));
            }
        }
   }
}

static void PutEOL(void)
{
   if (bitoff>0) byteoff++;
   linebuf[byteoff++] = 0;
   linebuf[byteoff++] = 0x80;
}

static PutWhiteRun(int run)
{
   int makeup,term;

   makeup = run/64;
   term = run % 64;
   if (makeup)
     PutRun(wmakeup[makeup].code, wmakeup[makeup].bitn);

   PutRun(wterm[term].code, wterm[term].bitn);
}

static PutBlackRun(int run)
{
   int makeup,term;

   makeup = run/64;
   term = run % 64;
   if (makeup)
     PutRun(bmakeup[makeup].code, bmakeup[makeup].bitn);

   PutRun(bterm[term].code, bterm[term].bitn);
}

static int Compress1D(char *ibuf,int len)
{
   int x = 0;
   int run;
   int bytesperline = len/8;

   InitRun();
   while (x<len)
   {
       run = CheckWhiteRun(ibuf,x,bytesperline);
       PutWhiteRun(run);
       x+= run;
       if (x>=len) break;
       run = CheckBlackRun(ibuf,x,bytesperline);
       PutBlackRun(run);
       x+=run;
       if (x>=len) break;
   }
   PutEOL();
   return byteoff;
}
/*--------------------- CCITT G3 compress end --------------------*/

static void fax_block()
{
   LPDC lpdc=&SysDc;
   int  i,off,n;

   off = 0;
   for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
   {
      n=Compress1D(&rasts[0][off],1728);
      fwrite(linebuf,1,n,prnstr);
      off += RastWidthByte;
   }
  // if(fDither)
   memset(rasts[0],0,RastSize);        // clear buffer
}

PRINTER FAXprinter = {
  DEV_BW,
  fax_init,
  fax_block,
  fax_FF,
  fax_over,
  fax_getheight,
  fax_scanfill,
  fax_setcolor,
  fax_setRGBcolor,
  fax_setCMYKcolor,
  fax_setGray,
  204,
  1728,         // A4: 215mm/25.4*204=1728
  2385,
  0,0,
};
