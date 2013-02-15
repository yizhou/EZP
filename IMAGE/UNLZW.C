#include <stdio.h>
#include <string.h>
#include <mem.h>

#define FALSE    0
#define TRUE     	!FALSE
#define TABSIZE  	4096
#define STACKSIZE 	TABSIZE
#define NO_PRED  	0xFFFF
#define EMPTY    	0xF000
#define NOT_FND  	0xFFFF
#define NO_UPDATE 	FALSE
#define MEOF        ((unsigned short)257)
#define UEOF        ((unsigned short)MEOF)
static unsigned short inbuf = 0;
static unsigned short inchar;
struct entry {
  char used;
  unsigned short predecessor; /* 12 bit code                  */
  unsigned char follower;
} static string_tab[TABSIZE];

static char stack[STACKSIZE];    /* stack for pushing and popping characters     */
static short sp = 0;             /* current stack pointer                        */
static int code_len;
static int outlimit;
static int outcurrent = 0;
static int limit;
static int current = 0;
static char *insector,*outsector;

/*
static short push(short c)
{
  stack[sp] = ((char) c);  // coerce passed integer into a character
  ++sp;
  if (sp >= STACKSIZE) return -1;
  //  fprintf(stderr,"Stack overflow, aborting\n");
  return 0;
}
*/
#define push(val)  ((sp<STACKSIZE) ? 0&(stack[sp++]=(char)val) : -1)
#define pop()   ((sp>0) ? (short)stack[--sp] : (short)EMPTY)
/*
static short pop()
{
  if (sp > 0)
  {
    --sp;                         // push leaves sp pointing to next empty slot
    return ( (short) stack[sp] ); // make sure pop returns char
  }
  else
    return (short)EMPTY;
}
*/
static upd_tab(unsigned short pred,unsigned short foll,unsigned short order)
{
  register struct entry *ep;

  ep = &string_tab[ order ];
  ep->used = TRUE;
  ep->predecessor = pred;
  ep->follower = foll;

  switch(order)
  {
   case 510:
	      code_len=10;
	      break;
   case 1022:
	      code_len=11;
	      break;
   case 2046:
	      code_len=12;
	      break;
  }

}

static init_tab()
{
  register unsigned short i;
  memset((char *)string_tab,0,sizeof(string_tab));
  for (i = 0; i <= 255; i++)
  {
    upd_tab(NO_PRED,i,i);
  }
}
/*
static writec(short c)
{
  if (outcurrent<outlimit)
    outsector[outcurrent++] = ( (char) c);
}
*/

#define writec(val)   if (outcurrent<outlimit) \
			  outsector[outcurrent++] = ( (char) (val));
#define readc()       ((current<limit) ? (insector[current++]&0xff) : (MEOF))

/*
static unsigned short readc()
{
  register short returnval;

  if (current == limit)
      return (MEOF);

  returnval = (insector[current++]);
  return (returnval & 0xFF);
}
*/

static unsigned char BitSet[]={0,1,3,7,15,31,63,127};
static unsigned short getcode()
{
  register unsigned short d1,len;

  len=code_len;
  d1=0;
  while (len>0)
  {
   if (inbuf==0)
   {
    if (UEOF==(inchar=readc())) return MEOF;
    inbuf=8;
   }

   if (len>=inbuf) { len=len-inbuf; d1=((d1<<inbuf)|inchar); inbuf=0; }
   else { inbuf=inbuf-len; d1=((d1<<len)|(inchar>>inbuf)); len=0;
	  /* inchar=(inchar<<(16-inbuf)); inchar=(inchar>>(16-inbuf)); */
	  inchar &=BitSet[inbuf];
	  }
  }
  if (d1==257)
  {
  return UEOF;
  }
  return d1;
}


int unlzw(char *sbuff,int slen ,char *tbuff,int tlen )
{
  register unsigned short tempc, code, oldcode, incode, finchar;
  short code_count ;
  register struct entry *ep;

  #define ClearCode 256
  #define EndCode 257

  insector=sbuff; limit=slen; current=0;
  outsector=tbuff; outlimit=tlen; outcurrent=0;
  inbuf=0;
  code_len=9;
  init_tab();
  code_count=258;

while ((code=getcode())!=UEOF)
{
  if (code==ClearCode)
  {
  sp=0;
  code_len=9;
  init_tab();
  code_count = 258;

  code = getcode();
  if (code==UEOF) break;
  writec(code);
  oldcode=code;

  }
  else
  {
    if (string_tab[code].used)
     {
	ep=&string_tab[code];
        while (NO_PRED != ep->predecessor)      /* means table */
	 {
	  if (push( ep->follower)==-1)
	     return outcurrent;                 /* decode string backwards into */
						/* stack                        */
	  incode = ep->predecessor;
	  ep = &string_tab[incode];
	 }

	finchar=ep->follower;
	writec(finchar);

        while ( EMPTY != (tempc = pop()) )
	{
	 writec(tempc);
	}
	upd_tab(oldcode,finchar,code_count++);
	oldcode=code;
     }
      else
     {
	ep=&string_tab[oldcode];
        while (NO_PRED != ep->predecessor)      /* means table */
	 {
	  if (push( ep->follower)==-1)
	     return outcurrent;                 /* decode string backwards into */
						/* stack                        */
	  incode = ep->predecessor;
	  ep = &string_tab[incode];
	 }

	finchar=ep->follower;
	writec(finchar);
        while ( EMPTY != (tempc = pop()) )
	{
	 writec(tempc);
	}
        writec(finchar);
	upd_tab(oldcode,finchar,code_count++);
	oldcode=code;
     }
   }  /* end of not clear */
 }   /* end while */
  return outcurrent;
}


/*
char in_buff[10000];
char out_buff[10000];
main(int nn,char *argv[])
{
FILE *fp;
int i,j;

if (nn!=3)
  {
  printf("Usage : Unlzw lzwfile,outfile\n");
  exit(1);
  }
fp=fopen(argv[1],"rb");
if (fp==NULL)
 {
  printf("Open file 1 error !");
  exit(1);
  }
 i=fread(in_buff,1,10000-1,fp);


//current=0;
//limit=i;
//inbuf=0;
//insector=in_buff;
//code_len=9;

//while (1)
//{
//
//  i=getcode();
//  printf("%03x ",i);
//  if (i==EOF) break;
//}
//exit(1);
//

 j=10000;
 j=unlzw(in_buff,i,out_buff,10000-1);


fp=fopen(argv[2],"wb");
if (fp==NULL)
 {
  printf("Open file 2 error !");
  exit(1);
  }
  fwrite(out_buff,1,j,fp);
}
*/
