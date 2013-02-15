/*-------------------------------------------------------------------
* Name: cache.c            Cache method: FIFO
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define NEG_TWO         -2

#define CACHE_NOT_FOUND -1
#define CACHE_DEPTH     1240

#define FIRST_HANZI             0xa1a1
#define MAX_CACHE_CODE          0xd7fe
#define MAX_ASCII_CODE          127
#define MAX_CHINESE_NUM         5170
#define MAX_HIFREQ_CODE_NUMBER  MAX_CHINESE_NUM+MAX_ASCII_CODE

/************Cache Key structure************
   size: 7bit   only to 128*128 ( char's width must be EQU to height )
   font: 7bit   only allow font 0 - 127
   code number: 16 bit
*********************************************/
#define MAX_CACHE_CHAR_SIZE     127
#define MAX_CACHE_FONT_NUM      127
#define SIZE_BIT        23
#define FONT_BIT        16
#define Key2Num(key)    (key & 0xffff)
#define MakeKey(num,font,size)  \
     ( (ULONG)( num | (font << FONT_BIT) | (size <<SIZE_BIT) ) )

//#define     RingAdd(a, b, m)    (a+b > m-1) ? a+b-m   : a+b
//#define     RingSub(a, b, m)    (a-b < 0)   ? m+(a-b) : a-b
//#define     RingDec(a, m)       (--a < 0)   ? m-1     : a
//#define     RingInc(a, m)       (++a >= m) ? 0 : a

static SHORT CodeInTableIdx[MAX_HIFREQ_CODE_NUMBER];
static ULONG KeyTable[CACHE_DEPTH];
static char *CacheBuffer[CACHE_DEPTH];
static int cache_top,cache_bottom,cache_empty;

void InitCache()
{
    int i;

    cache_top = cache_bottom = 0;
    cache_empty = CACHE_DEPTH;

    for (i=0;i<MAX_HIFREQ_CODE_NUMBER;i++)
         CodeInTableIdx[i] = NEG_ONE;

    for (i=0;i<CACHE_DEPTH;i++)
    {
         KeyTable[i] = NEG_ONE;
         CacheBuffer[i] = NULL;
    }
} /* InitCache */

void DestoryCache()
{
    CloseCache();
    InitCache();
} /* DestoryCache */

void CloseCache()
{
    int i;
    for (i=0;i<CACHE_DEPTH;i++)
      if(CacheBuffer[i] != NULL)
         free(CacheBuffer[i]);
}

static int Code2Num(Wchar code)
{
   int hh;
   hh = ((code>>8) - 0xa1)*94;
   return hh+((code&0xff) - 0xa1);
}

static int CacheHit(ULONG key)
{
    int i;

    i = cache_top;
    while  (i!= cache_bottom)  {
        if (KeyTable[i] == key)       // found it
            return i;

        //i = RingInc(i,CACHE_DEPTH);
        ++i;
        if(i==CACHE_DEPTH) i=0;
    }
    return CACHE_NOT_FOUND;
}

int GetCacheData(Wchar code,int font,int size,char *pBuffer,int Length)
{
  int i,num;
  ULONG key;

  if (code<MAX_ASCII_CODE)  {                  // AscII font !
      if (font<0 || font >MAX_CACHE_FONT_NUM
         || size<1 || size >MAX_CACHE_CHAR_SIZE )
      return CACHE_NOT_FOUND;
      num = MAX_CHINESE_NUM + code;
      goto start_get_cache;
  }


  if( font<0 || font>MAX_CACHE_FONT_NUM     // font not in cache
   || size<1 || size>MAX_CACHE_CHAR_SIZE    // size not in cache
   || code<FIRST_HANZI || code>MAX_CACHE_CODE) // code is not hi freq code
      return CACHE_NOT_FOUND;

  num=Code2Num(code);                // GB code to glyph number

start_get_cache:

  if ((i=CodeInTableIdx[num]) < 0 )     // code not in cache
     return CACHE_NOT_FOUND;

  key = MakeKey(num,font,size);
  if (KeyTable[i]==key)             // Cache hit at once !!
  {
    lbl_found_it:
      if (CacheBuffer[i] == NULL)
         return CACHE_NOT_FOUND;
      memcpy(pBuffer, CacheBuffer[i], Length);
      ReturnOK();
  }

  if ((i=CacheHit(key)) >=0 )     // search the whole cache
  {
      CodeInTableIdx[num] = i;
      goto lbl_found_it;
  }
  return CACHE_NOT_FOUND;
}

void PutCacheData(Wchar code,int font,int size,char *pBuffer,int Length)
{
  int i,num,num1;
  ULONG key;


  if (code<MAX_ASCII_CODE)  {                  // AscII font !
      if (font<0 || font >MAX_CACHE_FONT_NUM
         || size<1 || size >MAX_CACHE_CHAR_SIZE )
      return;
      num = MAX_CHINESE_NUM + code;
      goto start_put_cache;
  }


  if( font<0 || font>MAX_CACHE_FONT_NUM     // font not in cache
   || size<1 || size>MAX_CACHE_CHAR_SIZE    // size not in cache
   || code<FIRST_HANZI || code>MAX_CACHE_CODE) // code is not hi freq code
      return;

  num=Code2Num(code);                // GB code to glyph number

start_put_cache:

  i=CodeInTableIdx[num];

  key = MakeKey(num,font,size);
  if (i>=0 && KeyTable[i] == key)     // already in Cache
      return;

  if (CacheHit(key)>=0)         // already in Cache
      return;

  if (cache_empty == 0)       // no enough room, so destroy the bottom cache
  {
      cache_empty++;
      ++cache_top;
      if(cache_top==CACHE_DEPTH) cache_top=0;
  }

  i = cache_bottom;
  num1 = Key2Num(KeyTable[i]);            // destroy old char cache
  if (num1<MAX_HIFREQ_CODE_NUMBER)
      CodeInTableIdx[num1] = NEG_ONE;   //NEG_TWO;

  if (CacheBuffer[i] != NULL)
      free(CacheBuffer[i]);

  //if ((CacheBuffer[i] = malloc(Length)) == NULL)
  if ((CacheBuffer[i] = malloc(Length)) < 0x1000)
  {
      ReportMemoryError("cache");
      return;
  }
  memcpy(CacheBuffer[i],pBuffer,Length);

  KeyTable[i] = key;
  CodeInTableIdx[num] = i;

  ++cache_bottom;
  if(cache_bottom==CACHE_DEPTH) cache_bottom=0;
  --cache_empty;
}

#ifdef TestCache
void main(int argc,char *argv[])
{
    FILE *fp;
    int c;
    char buf[400];
    USHORT code;
    long hzcnt = 0;
    long cachehit = 0;

    if (argc<2)
    {
        printf("Usage: cache TextFile\n");
        exit(0);
    }

    CacheInit();
    fp = fopen(argv[1],"r");
    if (fp==NULL) {
        printf("file not found !\n");
        exit (0);
    }

    while ((c = getc(fp)) != EOF) {
        if (isspace(c)) continue;
        if (c>0x80) {
             code = c*256+getc(fp);
             hzcnt ++;
             if (GetCacheData(code,1,16,buf,300)==OpOK)
                 cachehit++;
             else  PutCacheData(code,1,16,buf,300);
        }
    }
    fclose(fp);
    printf("Done!\n");
    printf("Total = %ld   Hit = %ld\n",hzcnt,cachehit);
    printf("Cache Hit Rate = %3.3f\n",(float)cachehit*100/(float)hzcnt);
}
#endif
