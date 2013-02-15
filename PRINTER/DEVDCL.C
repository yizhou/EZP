/*--------- DCL ----------*/
/*-------------------------------------------------------------------
* Name: devdcl.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int blanklines;
static char *prev_row;
extern int pic_dpi;
/*
 * Mode 3 compression routine for the HP LaserJet III family.
 * Compresses bytecount bytes starting at current, storing the result
 * in compressed, comparing against and updating previous.
 * Returns the number of bytes stored.  In the worst case,
 * the number of bytes is bytecount+(bytecount/8)+1.
 */
int
gdev_pcl_mode3compress(int bytecount, const unsigned char *current, unsigned char *previous, unsigned char *compressed)
{       register const unsigned char *cur = current;
        register unsigned char *prev = previous;
        register unsigned char *out = compressed;
        const unsigned char *end = current + bytecount;
        while ( cur < end )
           {    /* Detect a maximum run of unchanged bytes. */
                const unsigned char *run = cur;
                register const unsigned char *diff;
                const unsigned char *stop;
                int offset, cbyte;
                while ( cur < end && *cur == *prev )
                   {    cur++, prev++;
                   }
                if ( cur == end ) break;        /* rest of row is unchanged */
                /* Detect a run of up to 8 changed bytes. */
                /* We know that *cur != *prev. */
                diff = cur;
                stop = (end - cur > 8 ? cur + 8 : end);
                do
                   {    *prev++ = *cur++;
                   }
                while ( cur < stop && *cur != *prev );
                /* Now [run..diff) are unchanged, and */
                /* [diff..cur) are changed. */
                /* Generate the command byte(s). */
                offset = diff - run;
                cbyte = (cur - diff - 1) << 5;
                if ( offset < 31 )
                        *out++ = cbyte + offset;
                else
                   {    *out++ = cbyte + 31;
                        offset -= 31;
                        while ( offset >= 255 )
                                *out++ = 255, offset -= 255;
                        *out++ = offset;
                   }
                /* Copy the changed data. */
                while ( diff < cur )
                        *out++ = *diff++;
           }
        return out - compressed;
}

static int dcl_init(UDATA pagew,UDATA pageh)
{
   int m,lines;

   if ((prnstr=fopen(PrintName,"wb"))==NULL)
       return(-1);

   printer->resolution=pic_dpi;
   printer->xpixel=((pagew*pic_dpi/SCALEMETER+7)/8)*8;
   printer->ypixel=pageh*pic_dpi/SCALEMETER;

   RastWidth=(printer->xpixel+7) & 0xfffffff8;
   RastWidthByte = RastWidth>>3;
   prev_row=malloc(RastWidthByte);
   if(prev_row==NULL)       return -1;

   fprintf(prnstr,"Redtek  PageFile.dcl,%d,%d,%d b3M",
             printer->xpixel,printer->ypixel,RastWidthByte);

   //lines= printer->ypixel+32;
   lines=160+32;
   rasts[0]=NULL;
   while (rasts[0]==NULL && lines>32)
   {
       lines -= 32;
       m = lines*RastWidthByte;
       rasts[0] = malloc(m);
   }

   if(rasts[0]==NULL)
   {
       free(prev_row);
       fclose(prnstr);
       return(-1);
   }

   RastHeight=lines;
   RastSize=m;
   memset(rasts[0],0,RastSize);         // clear it
   memset(prev_row,0,RastWidthByte);         // clear it
   blanklines = 0;
   return 1;
}

static void dcl_FF()
{
   blanklines = 0;
   fputs("l0H",prnstr);
}

static void dcl_over()
{
   fclose(prnstr);
   free(rasts[0]);
   free(prev_row);
}

static int dcl_getheight() { return RastHeight; }
static void dcl_scanfill(int x1,int x2, int y, LPDC lpdc)
{
   BW_scanline(x1,x2,y,lpdc);
}

static void dcl_block()
{
    int  i;
    char *linebuf, *end_word;
    int  out_count;
    unsigned char outbuf[0x1280];
    LPDC lpdc=&SysDc;

    for (i=lpdc->top;i<lpdc->bottom && i<printer->ypixel;i++)
    {
         linebuf=&rasts[0][(i-lpdc->top)*RastWidthByte];
         end_word=linebuf+RastWidthByte;

          // check if this line is zero line, from right side to left
         while ( end_word > linebuf && end_word[-1] == 0 ) end_word-- ;

         if ( end_word == linebuf) {    /* Blank line */
              blanklines++;
              continue;
         }

  ////////if there are blank lines , skip the blank lines///////////////
         if( blanklines>0 )  {    /* move down from current position */
              fprintf(prnstr,"b%dY",blanklines);
              blanklines=0;
              memset(prev_row, 0, RastWidthByte);
         }

         out_count = gdev_pcl_mode3compress(RastWidthByte, linebuf,
                                               prev_row, (char *)outbuf);

         fprintf(prnstr, "b%dW",out_count);
         fwrite(outbuf,sizeof(char),out_count,prnstr);
    } /*--- i ---*/

   // if(fDither)
    memset(rasts[0],0,RastSize);        // clear buffer
} /* dcl_block */

static void dcl_setcolor(int color)
{
   sysColor = color;
}
static void dcl_setGray(int gray)
{
   BW_setGray(gray);
}
static void dcl_setRGBcolor(int r,int g,int b)
{
   int gray=(30*r+59*g+11*b)/100;
   dcl_setGray(gray);
}
static void dcl_setCMYKcolor(int c,int m,int y,int k)
{
}

PRINTER DCLprinter = {
  DEV_BW,
  dcl_init,
  dcl_block,
  dcl_FF,
  dcl_over,
  dcl_getheight,
  dcl_scanfill,
  dcl_setcolor,
  dcl_setRGBcolor,
  dcl_setCMYKcolor,
  dcl_setGray,
  600,
  60*115,                // 600x11.5
  60*170,                // 600*17.0
  0,0
};

