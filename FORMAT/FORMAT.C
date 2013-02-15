
/*-------------------------------------------------------------------
* Name: format.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/

#include <assert.h>
#include "ezpHead.h"

//int UncompressCHARSIZE(int yy);
#ifdef  DEBUG_VERSION
void check_link(PTextBoxs hw)
{
 Pmark_rec eptr;
 int i=0;
 int str[100];

 i=hw->TextLength*2;
 memcpy(code1,hw->text,i);

 for (i=0;i<=39;i++) str[i]=0;

 eptr=hw->formatted_elements;
 while (eptr)
   {
      str[eptr->regno]=1;
      if (eptr->type==E_END) break;
      eptr=eptr->next;
   }
   for (i=0;i<=39;i++) if (str[i]==0)
   {
   i=i+1;
   break;
   }
   //if (i<39)
   //    MessageBox("Link information",str,1,0);
}
#else
#define check_link(hw)
#endif

static Pmark_rec GetCurrentLineHead(Pmark_rec pmarkCurrent);
static int mark_invlad=0;

#define ISeFC(t) (t&(1<<10))
#define TOePART(t) (t<<8)
#define TOcPART(t) (t)
#define GETePART(t) (t&0xFF00)
#define GETcPART(t) (t&0xFF)
#define GETeFONT(t) ((t&0xFF00)>>8)

/*------
#undef assert
#define assert(p) 0
#define ASSERT(p) 0
-------*/

//#define Warning(s)      printf(s)

void mprintf(char *s)
{
    printf(s);
}

#define Warning(s)      mprintf(s)

void AssertFailed(char * FILE, int LINE,char *p)
{
  printf("Assertion failed: %s file %s, line %d \n",p, FILE, LINE );
}
#define ASSERT(p) ((p) ? (void)0 : AssertFailed(__FILE__, __LINE__,#p))

#define MARKISSEGMENTEND(pmark1) ((pmark1->type==E_HARDLINEFEED)\
                ||(pmark1->type==E_END)||(pmark1->type==E_PAGEFEED)||(pmark1->type==E_FORMTAB))

#define Cisalpha(val)   ((val<256)&&isalpha(val))
#define Cisdigit(val)   ('0'<=val && val<='9')       // ByHance

//to avoid some char appear at the head or end of a line
//     and can not break in englist word
#define ISNOTBREAKABLE(end) (CNoEnd(*(end))||CNoBegin(*((end)+1))\
                      ||(Cisalpha(*(end))&&Cisalpha(*((end)+1))) \
                ||('.'==(*(end))&&Cisdigit(*((end)+1))) \
                ||(Cisdigit(*(end))&&Cisdigit(*((end)+1)))) /* exp(0.12) */
                // added ByHance
/*
void my_disp(HTEXTBOX pBox)
{
      PTextBoxs hw;
      char tmpc[100];

      hw=HandleLock(ItemGetHandle(pBox));
      if (hw==NULL) return -1;
      sprintf(tmpc,"Top:%d,Left:%d,Width:%d,high:%d,Space:%d",
      hw->BoxTop,hw->BoxLeft,hw->BoxWidth,hw->BoxHeight,hw->TextDistantBottom);
      MessageBox("Box Paramter",tmpc,1,NULL);
      HandleUnlock(ItemGetHandle(CurrentBox));
    }
*/

Wchar Wtoupper(Wchar s)
{
   if(s<0x80)
     return toupper(s);
   else
     return s;
}

size_t Wstrlen(Wchar *s)
{
   int len=0;

   while (*s++)
     len++;
   return len;
}

void MakeWchar(unsigned char *s, Wchar *p)
{
   while (*s)
   {
        if(*s>0x80)
        {
          *p++=*(Wchar*)s;
          s+=2;
        }
        else
          *p++=*s++;
   }
   *p=0;
}

Wchar *Wstrcat(Wchar *dest, Wchar *src)
{
   return (Wchar*)memmove(dest+Wstrlen(dest), src,(Wstrlen(src)+1)*sizeof(Wchar) );
}

Wchar *Wstrcpy(Wchar *dest, Wchar *src)
{
   return (Wchar*)memmove(dest, src, (Wstrlen(src)+1)*sizeof(Wchar));
}

static int Cstyle(Wchar val)
{
   return (Cisctext(val)?0:((val&0x7800)>>11));
}

//when format text a word must less then the length
#define MAXWORDLEN 200

static Wchar NoEndSet[]={
//    '¡²',     '¡®',    '¡°',    '¡´',    '¡¶',
      0xA1B2,  0xA1AE,   0xA1B0,  0xA1B4,  0xA1B6,

//    '¡¼',     '¡¾',     '£¨',    '£Û',    '£û',
      0xA1BC,  0xA1BE,   0xA3A8,  0xA3DB,  0xA3FB,

    '[' ,     '(' ,    '{' ,
     0
};
int CNoEnd(Wchar val)
{
    Wchar *p=NoEndSet,ch;
    while( (ch=*p) )
    {
        if (val==ch)
           return 1;
        p++;
    }
    return 0;
}

static Wchar NoBeginSet[]={
//    '¡¢',   '¡£',   '¡³',   '¡¯',    '¡±',
     0xA1A2, 0xA1A3, 0xA1B3, 0xA1AF,  0xA1B1,

//    '¡µ',   '¡·',   '¡½',   '¡¿',    '£¬',
     0xA1B5, 0xA1B7, 0xA1BD, 0xA1BF,  0xA3AC,

//    '£©',   '£º',   '£»',   '£Ý',    '£ý',
     0xA3A9, 0xA3BA, 0xA3BB, 0xA3DD,  0xA3FD,

//    '£¡',   '£¿',
     0xA3A1,  0xA3C1,

    ',' ,   '.' ,   ']' ,   ')' ,  ':' ,  ';' ,   '}' ,  '!' ,  '?' ,
     0
};
int CNoBegin(Wchar val)
{
    Wchar *p=NoBeginSet,ch;
    while( (ch=*p) )
    {
        if (val==ch)
           return 1;
        p++;
    }
    return 0;
}


//Format Env: globe variable for format

static int LineNumber;
static int MaxLineHeight;
static int BaseLine;
static int TextIndent=0;
static int FormattingLineRight;
static int FormattingLineWidth;
#define MarginW             0
static int FormattingReg=0;
static Pmark_rec pmarkCurrentAppendPos;
static HTEXTBOX CurrentBox;
static int MarkId;
//static int CurrentLine;
static Wchar *FormattingLineStart;

static int FormFeed(HBOX HBox,int *x, int *y);
static int LineFeed(int *x, int *y);
static Wchar *FormatTextInTextBox(PTextBoxs hw,Wchar *Iter,int *x,int *y,Wchar *end);
static Wchar *FormatTextInFormBox(PFormBoxs hw,Wchar *Iter,int *x,int *y,Wchar *end);
static void FormBoxSetRg(PFormBoxs pBox,int iCell);
static void FormBoxSetRgNoBottom(HBOX HBox,PFormBoxs pBox,int iCell);

static int GetCharDistant(int maxh,int ColGap)     //By zjh
{
   short distant,hi,low;
   distant = (ColGap & 0xf0);
   hi=(ColGap>>8);
   low=(ColGap&0xf);

   switch (distant) {
      case 0:    return 0;
      case LINEGAP1+0x10:return 0;
      case LINEGAP2+0x10:return maxh;
      case LINEGAP175+0x10:return maxh*0.75;
      case LINEGAP125+0x10:return maxh*0.25;
      case LINEGAPUSER+0x10: return maxh*(hi-1+(float)low/10.0);
      case LINEGAP15+0x10:
      default:
           return maxh*0.5;
   }
}

void GetTtfWidth(Wchar code,int font,USHORT *aw, SHORT *lsb)
{
  if(code<0x21 || fEditor)
  {
      *aw=CHAR_WIDTH_IN_DOT/2;
      *lsb=0;
  }
  else
//  if(font<=1 && code>=0x21 && code<=0x7e)
  if(font<MAXEFONT /* && code>=0x21 && code<=0x7e */)
  {
      assert(code<=0x7e);
      *aw=ASC32AW[font][code-0x21];
      *lsb=0;
  }
  else
  {
      //if(!font) font=1;
      TTWidth(&SysDc, code,font+1, aw, lsb);
  }
}

static void GetTextExtents(TextStyles *pTextStyle, Wchar *ptr, int len,int *dir,
                    int *ascent,int *descent, unsigned int *width)
{
   int i,font;
   int textlen=0;
   USHORT aw;
   Wchar code;
   short  lsb;

   // *ascent=pTextStyle->CharSize;
   // *dir=*descent=0;

   font=GETeFONT(pTextStyle->CharFont);

   for (i=0; i<len; i++)
   {
       if( (code=*ptr++)<=0x7e )
       {
           GetTtfWidth(code, font, &aw, &lsb);
           //textlen+=aw;    By zjh
           textlen+=(aw+GetCharDistant(aw,pTextStyle->ColGap));    //By zjh for test colgap

           if (*ptr >0xff) {
               textlen+=CHAR_WIDTH_IN_DOT/4;
           }

           //textlen+=CHAR_WIDTH_IN_DOT/2;
       }
       else
           //textlen+=(CHAR_WIDTH_IN_DOT);     //By zjh
           textlen+=(CHAR_WIDTH_IN_DOT+
                    GetCharDistant(CHAR_WIDTH_IN_DOT,pTextStyle->ColGap));   //By zjh for test
   }

      // Char_width_in_dot == 256
   //*width=(pTextStyle->CharHSize*textlen+CHAR_WIDTH_IN_DOT/2)/CHAR_WIDTH_IN_DOT;
   *width=(pTextStyle->CharHSize*textlen+128)>>8;
}

/*------------------------
static TextStyles *GetElementStyle(Pmark_rec eptr)
{
   return (TextStyles*)&(eptr->CharSize);           // get first address
}
static Wchar *GetElementEdata(PTextBoxs hw,Pmark_rec eptr)
{
   return hw->text+eptr->start_pos;        // before using it, lock hw
}
static int GetMarkParagraphAlignMode(Pmark_rec eptr)
{
   return (eptr->ParagraphAlign)&0xf;
}
-------------------*/
#define GetElementStyle(eptr)    ( (TextStyles*)&(eptr->CharSize) )
#define GetElementEdata(hw,eptr) ( (Wchar *)(hw->text+eptr->start_pos) )
#define GetMarkParagraphAlignMode(eptr)   ((eptr->ParagraphAlign)&0xf)

//static int GetLineDistant(int maxh,int ParagraphAlign)



static int GetLineDistant(int maxh,int RowGap)
{
   short distant,hi,low;
   distant = (RowGap & 0xf0);
   hi=(RowGap>>8);
   low=(RowGap&0xf);

   switch (distant) {
      case LINEGAP1:return maxh;
      case LINEGAP2:return maxh*2;
      case LINEGAP175:return maxh*1.75;
      case LINEGAP125:return maxh*1.25;
      case LINEGAPUSER: return maxh*(hi+(float)low/10.0);
      case LINEGAP15:
      default:
           return maxh*1.5;
   }
}

int GetLineBottom(Pmark_rec eptr)
{
   short distant,hi,low;
   int maxh=eptr->line_height;

   distant = eptr->RowGap & 0xf0;
   hi=(eptr->RowGap>>8);
   low=(eptr->RowGap&0xf);
   switch (distant) {
      case LINEGAP1:return 0;
      case LINEGAP2:return maxh/2;
      case LINEGAP175:return maxh/5;
      case LINEGAP125:return maxh*3/7;
      case LINEGAPUSER:return maxh*(hi-1+(float)low/10.0)/(hi+(float)low/10.0);
      case LINEGAP15:
      default:
           return maxh/3;
   }
}


static Wchar *GetMarkText(Pmark_rec eptr)        // get start pos
{
  PTextBoxs hw;
  Wchar *wcpPos;

  /*----- !!!! GetFirstLinkBox(eptr->hBox) ??==?? eptr->hBox --*/
  hw=HandleLock(ItemGetHandle(GetFirstLinkBox(eptr->hBox)));
  wcpPos=hw->text+eptr->start_pos;
  HandleUnlock(ItemGetHandle(GetFirstLinkBox(eptr->hBox)));
  return wcpPos;
}

static void MoveMarksPosDown(Pmark_rec eptr,int down)  //when insert text,do it
{
   while (eptr != NULL)
   {
           eptr->start_pos+=down;
           eptr = eptr->next;
   }
}

static int GetBoxRgListNo(HBOX HBox)          // get total num of region
{
   int numofRg;

   if (BoxIsTextBox(HBox))
   {
        PTextBoxs pBox;
        pBox=HandleLock(ItemGetHandle(HBox));
        if (pBox==NULL)
        {
          HandleUnlock(ItemGetHandle(HBox));
          ASSERT(0);
        }
        numofRg=pBox->numRg;
   }
   else  if (BoxIsTableBox(HBox))
   {
        PFormBoxs pBox;
        pBox=HandleLock(ItemGetHandle(HBox));
        if (pBox==NULL)
        {
          HandleUnlock(ItemGetHandle(HBox));
          ASSERT(0);
        }
        numofRg=pBox->numLines*pBox->numCols;
   }
   else
   {
        Warning("HBox is neither a TABLE nor a TextBOX");
   }
   HandleUnlock(ItemGetHandle(HBox));
   return numofRg;
}

/***********************************************************************
This Function : build virtual region List by ceil information
  if the box is a TABLEBOX,  use FormattingReg as index
***********************************************************************/
static LPREGIONITEM GetBoxRgListItem(HBOX HBox,int iRg) //get the ith region
{
   PTextBoxs pBox;
   LPREGIONITEM pRg;

   if(HBox<=0)
      return NULL;

   pBox=HandleLock(ItemGetHandle(HBox));
   if (pBox==NULL)
   {
     HandleUnlock(ItemGetHandle(HBox));
     ASSERT(0);
     return NULL;
   }
   if (BoxIsTextBox(HBox))
   {
        pRg=(pBox->rgList)+iRg;
   }
   else  if (BoxIsTableBox(HBox))
   {
       //set the region as if region in the cell
       FormBoxSetRg((PFormBoxs)pBox,iRg);
       //fool the other
       pRg=pBox->rgList;
   }
   else
   {
        Warning("HBox is neither a TABLE nor a TextBOX");
   }
   HandleUnlock(ItemGetHandle(HBox));
   return pRg;
}

/***********************************************************************
_LocateMarkbyLine  : give line no., found first Mark(which.line==iLine)
     pmarkHead   startpoint
     iLine       newlineno
  return new mark
if the pmarkHead near the iLine
   the function will fast then other procedure
************************************************************************/
static Pmark_rec _LocateMarkbyLine(Pmark_rec pmarkHead,int iLine)
{
  Pmark_rec pmark1=pmarkHead;

  if(pmark1!=NULL)
  {
      if(pmark1->line_number>iLine)
          while(pmark1!=NULL)
          {
                 if(pmark1->line_number<=iLine) break;
                 pmark1=pmark1->prev;
          }
      else
          while(pmark1!=NULL)
          {
                 if(pmark1->line_number>=iLine) break;
                 pmark1=pmark1->next;
          }
  }
  return(pmark1);
}

/* give line no., found first Next_Mark(which.line==iLine) */
static Pmark_rec LineToMark(Pmark_rec pmarkHead,int iLine)
{
  Pmark_rec pmark1=pmarkHead;
  while(pmark1!=NULL)
  {
         if(pmark1->line_number>=iLine) break;
         pmark1=pmark1->next;
  }
  return(pmark1);
}

/*
 * Locate the element (if any) that is at the passed location
 * in the widget.  If there is no corresponding element, return
 * NULL.  If an element is found return the position of the character
 * you are at in the pos pointer passed.
 */
/*-- get mark_list_head, then LineToMark ---*/
static Pmark_rec LocateMarkbyLine(PTextBoxs hw,int line)
{
  Pmark_rec pmark1;
  HANDLE HBox;

  HBox=GetFirstLinkBox(TextBoxGetPrevLinkBox(hw));
  if (HBox!=0)
  {
    hw=HandleLock(ItemGetHandle(HBox));
    if (hw==NULL)
    {
       HandleUnlock(ItemGetHandle(HBox));
       ASSERT(0);
       return NULL;
    }
  }

  pmark1=LineToMark(hw->formatted_elements,line);
  if (HBox!=0)
        HandleUnlock(ItemGetHandle(HBox));

  if(pmark1==NULL)
  {
        //Warning("LocateMarkbyLine get a invaild iLine");
        //printf("%d\n",line);
        return NULL;
  }
  return pmark1;
}

//Attention!!!
//When start_pos is between two mark, return the second
//   start_pos must less than mark_end->start_pos+edata_len,
//     so that the second mark is certainly exist!!
// When start_pos is <0 or >TextLength, return NULL
// Call it when Mark List is consistant with text string
//    or you will get error Mark
//
Pmark_rec LocateMarkbyPos(PTextBoxs hw,int start_pos)
{
        Pmark_rec eptr;
        Pmark_rec oldeptr;
        Pmark_rec rptr;
        HANDLE HBox;

        //ASSERT(start_pos<=hw->TextLength);
        //ASSERT(start_pos>=0);
        if(start_pos<0 || start_pos>hw->TextLength)
           return NULL;

   // start:       // find the first Box
        HBox=GetFirstLinkBox(TextBoxGetPrevLinkBox(hw));
        if (HBox!=0)
        {
           hw=HandleLock(ItemGetHandle(HBox));
           if (hw==NULL)
           {
              HandleUnlock(ItemGetHandle(HBox));
              //ASSERT(0);
              return NULL;
           }
        }

        eptr=hw->formatted_elements;
        if(!eptr)
          return NULL;

        rptr = NULL;
        while( eptr && eptr->type!=E_END )
        {
                ASSERT((eptr->type<E_END)&&(eptr->type>E_ERROR));
                if( eptr->start_pos<=start_pos
                && start_pos<eptr->start_pos+eptr->edata_len )
                {
                    rptr = eptr;
                    break;
                } else
                if(eptr->start_pos>start_pos)
                {             //It Must be in a Style blank pos
                    rptr = eptr;
                    break;
                }
                oldeptr=eptr;
                eptr = eptr->next;
        } /*end of while eptr !=NULL*/

        //mark list have no end mark
        /*     delete By zjh
        if(eptr==NULL)
        {
            ASSERT(eptr!=NULL);
            goto start;
        }
       */
        if(rptr==NULL)
            rptr=eptr;

        //when paste in
        //assert fail for Textlength>start_pos>mark_end->start_pos+edata_len
        //ASSERT(rptr->start_pos+rptr->edata_len>start_pos);
        //assert can proved by loop
        //if (rptr->prev)
         // ASSERT((rptr->prev->start_pos+rptr->prev->edata_len-1)<start_pos);

        if (HBox!=0)
           HandleUnlock(ItemGetHandle(HBox));
        return rptr;
}

//call by FormatChange
//Call it when Mark List is NOT consistant with text string
static Pmark_rec _LocateMarkbyPos(Pmark_rec start,int start_pos)
{
        Pmark_rec eptr;
        Pmark_rec rptr;

        //ASSERT(start_pos<=hw->TextLength);
        ASSERT(start_pos>=0);
        // find the first Box
        eptr=start;
        if(!eptr)
          return NULL;
        rptr = NULL;
        while ((eptr!= NULL)&&(eptr->type != E_END))
        {
                if ((eptr->start_pos<=start_pos)&&(
                        (eptr->start_pos+eptr->edata_len)>start_pos))
                {
                        rptr = eptr;
                        break;
                }
                else if(eptr->start_pos>start_pos)
                {//It Must be in a Style blank pos
                        rptr = eptr;
                        break;
                }
                eptr = eptr->next;
        }/*end of while eptr !=NULL*/

        //mark list have no end mark
        ASSERT(eptr!=NULL);
        if(rptr==NULL)
            rptr=eptr;

        return rptr;
}

// Pos: start from BOX, return: X in BOX, not in mark(must add left margin)
static int XOfMarkPos(Pmark_rec eptr,int Pos)
{
  int X;

  if (!eptr)
  {
       X=TextIndent;            // BOX's global LeftMargin
  }
  else if (Pos>=eptr->start_pos)
  {
     if(Pos>=eptr->start_pos+eptr->edata_len)
     {
         X=eptr->x+eptr->width;
     }
     else
     {
        int dir, ascent, descent;
        unsigned int word_width;

        GetTextExtents(GetElementStyle(eptr), GetMarkText(eptr),
           (Pos-eptr->start_pos), &dir, &ascent, &descent, &word_width);
        // return:word_width:: x_offset in mark

        if( GetMarkParagraphAlignMode(eptr)==ALIGNLEFTRIGHT
         && eptr->type==E_TEXT )
        {
          unsigned int eptr_width;

          GetTextExtents(GetElementStyle(eptr), GetMarkText(eptr),
             (eptr->edata_len), &dir, &ascent, &descent, &eptr_width);
          if(!eptr_width) X=eptr->x;
          else X=eptr->x+word_width*eptr->width/eptr_width;
        }
        else
          X=eptr->x+word_width;
     }
  }
  else
  {                             // in mark left area
        X=eptr->x;
  }

  return X;
}

// give x in BOX and mark_idx, return positon relative to Mark.StartPos
static int x2Pos(Pmark_rec eptr,int x)
{
    int epos;
    int dir, ascent, descent;
    unsigned int word_width;

    //x in the mark
    ASSERT(eptr);
    if(eptr==NULL)              // ByHance, 96,2.2
       return 0;

    if(x<=eptr->x)              // ByHance, 96,4.6
       return 0;

    x-=eptr->x;
    if( GetMarkParagraphAlignMode(eptr)==ALIGNLEFTRIGHT
     && eptr->type==E_TEXT )
    {
          unsigned int eptr_width;

          GetTextExtents(GetElementStyle(eptr), GetMarkText(eptr),
                (eptr->edata_len), &dir, &ascent, &descent, &eptr_width);
          x=x*eptr_width/eptr->width;
    }

    epos = eptr->edata_len;
    while (epos>0)
    {
          GetTextExtents(GetElementStyle(eptr), GetMarkText(eptr),
                epos, &dir, &ascent, &descent, &word_width);
          if( word_width <= x+ eptr->CharHSize*2/5 )
             break;
          epos--;
    }

    if(epos<0) epos=0;      // add ByHance, 96,1.28
    return epos;
}

static int GetCurrentMaxLineHeight(Pmark_rec pmarkCurrent)
{
/*  Pmark_rec pmark1=pmarkCurrent;
  int h=0;
  while((pmark1->line_number==pmarkCurrent->line_number)&&pmark1)
  {
         if(pmark1->CharSize>h)
                h=pmark1->CharSize;
         pmark1=pmark1->prev;
  }
  {
    pmark1=pmarkCurrent;
    while((pmark1->line_number==pmarkCurrent->line_number)&&pmark1)
    {
           if(pmark1->CharSize>h)
                  h=pmark1->CharSize;
           pmark1=pmark1->next;
    }
  }*/

  ASSERT(pmarkCurrent);
  if(pmarkCurrent==NULL)        // ByHance,96,2.2
     return 0;
  return(pmarkCurrent->line_height);
}

// give position in BOX, return (x,y) in BOX
static int PosToBoxXY(HBOX HBox,HBOX *phBoxnew,int Pos,int *X,int *Y)
{
  TextBoxs *TextBox;
  Pmark_rec eptr;

  *phBoxnew=HBox;
  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  eptr=LocateMarkbyPos(TextBox,Pos);

  if (eptr) {                   // changed ByHance
     if(eptr->type==E_HARDLINEFEED || eptr->type==E_FORMTAB
     || eptr->type==E_END||eptr->type==E_PAGEFEED)
        *X=eptr->x;
     else
        *X=XOfMarkPos(eptr,Pos);

     *Y=eptr->y;
     *phBoxnew=eptr->hBox;
  } else {
     *X=TextBoxGetTextDistantLeft(TextBox);
     *Y=TextBoxGetTextDistantTop(TextBox);
  }
  HandleUnlock(ItemGetHandle(HBox));

  // ASSERT(*X>=0);
  if (*X<0)
     *X=0;

  TextBox=HandleLock(ItemGetHandle(*phBoxnew));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  if (*X>TextBoxGetBoxWidth(TextBox))
     *X=TextBoxGetBoxWidth(TextBox);
  HandleUnlock(ItemGetHandle(*phBoxnew));

  ReturnOK();
}


/*--------------------------------------------
 Return value
     0    if move to an exist Pos
     -1   if Pos is begin or end of buffer;
-------------------------------------------------*/
int BoxCursorPosDown(HBOX HBox,HBOX *NewHBox,int Position,
                    int *NewPosition,int linedown)
{
  TextBoxs *TextBox;
  Pmark_rec eptr,pMarknew;
  int x,y;

  *NewHBox=HBox;
  *NewPosition=Position;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  eptr=LocateMarkbyPos(TextBox,Position);
  pMarknew=_LocateMarkbyLine(eptr,eptr->line_number+linedown);
  if (!eptr||!pMarknew)
  {
        HandleUnlock(ItemGetHandle(HBox));
        return -1;
  }

  //get orig X
  PosToBoxXY(HBox,NewHBox,Position,&x,&y);
  y=pMarknew->y;

  // find next line  or prev line
  eptr=GetCurrentLineHead(pMarknew);
  while (eptr->line_number==pMarknew->line_number)
  {
       int tx2;
       tx2 = eptr->x + eptr->width;
       if (x < tx2)  // (x,y) in it or is the first mark at the right of (x,y).
          break;

       if( eptr->type==E_LINEFEED || eptr->type==E_HARDLINEFEED
       || eptr->type==E_END||eptr->type==E_PAGEFEED )
       // no mark includes (x,y) because (x,y) is at the rightest end of the
       // line. So me locate it at the linefeed mark.
       {
           break;
       }
       eptr = eptr->next;
  }/*end of while eptr !=NULL*/

  /*
   * If we found an element, locate the exact character position within
   * that element.
   */
  if(eptr->type==E_LINEFEED)
  {
      if( eptr->prev!=NULL && eptr->prev->line_number==pMarknew->line_number)
          eptr=eptr->prev;
      *NewPosition = eptr->start_pos+eptr->edata_len-1;
  }
  else
  if(eptr->type==E_HARDLINEFEED || eptr->type==E_END||eptr->type==E_PAGEFEED)
      *NewPosition = eptr->start_pos;
  else
      *NewPosition = x2Pos(eptr,x)+eptr->start_pos;

  //to get the correct NewHBox
  //PosToBoxXY(pMarknew->hBox,NewHBox,*NewPosition,&x,&y);
  *NewHBox=pMarknew->hBox;
  HandleUnlock(ItemGetHandle(HBox));
  ReturnOK();
}


int PosToBoxCursorXY(HBOX HBox,HBOX *phBoxnew,int Pos,int *X,int *Y)
{
  TextBoxs *TextBox;
  Pmark_rec eptr;
  int height;

  *phBoxnew=HBox;
  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  eptr=LocateMarkbyPos(TextBox,Pos);

  if (eptr) {                   // changed ByHance
     if(eptr->type==E_HARDLINEFEED||eptr->type==E_END||eptr->type==E_FORMTAB||eptr->type==E_PAGEFEED)
        *X=eptr->x;
     else
        *X=XOfMarkPos(eptr,Pos);

     *Y=eptr->y - GetLineBottom(eptr);

     height=eptr->CharSize;       //  GlobalPageScale;
     if (eptr->superscript)
               *Y-=height/2;
     else if (eptr->subscript)
               *Y+=height/6;
     else if (eptr->UpDown)           //By zjh
           if (eptr->UpDown&0x400)
                *Y-=height*(eptr->UpDown&0x3ff)/10;
           else *Y+=height*(eptr->UpDown&0x3ff)/10;

     *phBoxnew=eptr->hBox;
  } else {
     *X=TextBoxGetTextDistantLeft(TextBox);
    // *Y=TextBoxGetTextDistantTop(TextBox);       // By DG
     *Y=TextBoxGetTextDistantTop(TextBox)+105*100/72;
  }
  HandleUnlock(ItemGetHandle(HBox));

//  ASSERT(*X>=0);
  if (*X<0)
     *X=0;

  if(*phBoxnew!=HBox)
  {
      TextBox=HandleLock(ItemGetHandle(*phBoxnew));
      if (TextBox==NULL)
         return(OUTOFMEMORY);

      if (*X>TextBoxGetBoxWidth(TextBox))
         *X=TextBoxGetBoxWidth(TextBox);
      HandleUnlock(ItemGetHandle(*phBoxnew));
  }

  ReturnOK();
}

#ifdef OLD_VERSION
int Old_PosToBoxCursorXY(HBOX HBox,HBOX *phBoxnew,int Pos,int *X,int *Y)
{
  TextBoxs *TextBox;
  Pmark_rec eptr;

  *phBoxnew=HBox;
  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  eptr=LocateMarkbyPos(TextBox,Pos);

  if (eptr) {                   // changed ByHance
     if(eptr->type==E_HARDLINEFEED||eptr->type==E_END||eptr->type==E_FORMTAB||eptr->type==E_PAGEFEED)
        *X=eptr->x;
     else
        *X=XOfMarkPos(eptr,Pos);
     *Y=eptr->y - GetLineBottom(eptr);
     *phBoxnew=eptr->hBox;
  } else {
     *X=TextBoxGetTextDistantLeft(TextBox);
    // *Y=TextBoxGetTextDistantTop(TextBox);       // By DG
     *Y=TextBoxGetTextDistantTop(TextBox)+105*100/72;
  }
  HandleUnlock(ItemGetHandle(HBox));

//  ASSERT(*X>=0);
  if (*X<0)
     *X=0;

  if(*phBoxnew!=HBox)
  {
      TextBox=HandleLock(ItemGetHandle(*phBoxnew));
      if (TextBox==NULL)
         return(OUTOFMEMORY);

      if (*X>TextBoxGetBoxWidth(TextBox))
         *X=TextBoxGetBoxWidth(TextBox);
      HandleUnlock(ItemGetHandle(*phBoxnew));
  }

  ReturnOK();
}
#endif

//Return x,y  's region no
// if x, y is not in those region
//       return the nearest   region which have correct y
//              in the worst condition return 0;

static int myBoxGetRegionNo(HBOX HBox,int x,int y)
{
  //PTextBoxs *pBox;
  int xyDots[8];
  int iRg,lastiRg,TotalRg;

 /*-------------
  pBox=HandleLock(ItemGetHandle(HBox));
  if (pBox==NULL)
  {
     HandleUnlock(ItemGetHandle(HBox));
     return(OUTOFMEMORY);
  }
 ------------------*/
  TotalRg=GetBoxRgListNo(HBox);
  for (lastiRg=iRg=0;iRg<TotalRg;iRg++)
  {
     GetRgXY(GetBoxRgListItem(HBox,iRg),xyDots);
     if (y>xyDots[1]&&y<=xyDots[5])
     {
        int x0,x1;

        GetTXX0X1(xyDots,y,&x0,&x1);
        //if (x<x1)     Why ?     modify by zjh 9.12
        if (x0<=x&&x<x1)
        {
          //HandleUnlock(ItemGetHandle(HBox));
          return iRg;
        }

        lastiRg=iRg;
     }
  }

  //HandleUnlock(ItemGetHandle(HBox));
  return lastiRg;
}

/****************************************************************
 * Locate the Mark  in the Box.  If there is no corresponding
 * Mark , return  NULL.
 * If an element is found return the position of the character
*****************************************************************/
// give (x,y) in BOX, return position in BOX and  mark
static Pmark_rec LocateMark(HBOX HBox, int x, int y, int *pos)
{
    TextBoxs *hw;
    Pmark_rec eptr;
    int tx2, ty1, ty2;
    int Cregno;
    BOOL prebox=FALSE;

    // hw=HandleLock(ItemGetHandle(HBox));      // ByHance, 96,4.4
    hw=HandleLock(ItemGetHandle(GetFirstLinkBox(HBox)));
    if (hw==NULL)
    {
  locate_mark_error_ret:
       HandleUnlock(ItemGetHandle(HBox));
       return NULL;
    }

    if (x<0) x=0;
    if (y<0) { prebox=TRUE; y=0; }

    *pos = 0;

   /*---- 1: get region from (x,y) ----*/
    Cregno=myBoxGetRegionNo(HBox,x,y);
   /*---- 2: get first mark of region from Cregno ----*/
  #ifdef OLD_VERSION
    while (Cregno>=0)
    {
        if( (eptr=hw->region2mark[Cregno]) !=NULL )
           break;
        Cregno--;
    }
    if (Cregno<0)           // not found
        goto locate_mark_error_ret;
  #else                        // ByHance, 96,4.4

    eptr=hw->formatted_elements;
    while(eptr)
    {
       if(eptr->hBox==HBox && eptr->regno==Cregno)
           break;
       eptr = eptr->next;
    }
  #endif

    //while (eptr && eptr->type!=E_END && eptr->type!=E_FORMATAB)
    while (eptr && eptr->type<E_FORMTAB)       // ByHance, 96,4.6
    {
       tx2 = eptr->x + eptr->width;
       ty2 = eptr->y;
       ty1 = ty2 - GetCurrentMaxLineHeight(eptr);

       if(y<=ty1)   // There is no line feed mark at the end of the line
          break;

       if(y<=ty2)
       {
            if (x<tx2) // in this mark
               break;

            if( eptr->type==E_LINEFEED||eptr->type==E_HARDLINEFEED||eptr->type==E_PAGEFEED)
            // || eptr->type==E_END )      // it can not be E_END,see while...
            // no mark includes (x,y) because (x,y) is at the
            // rightest end of the line. So me locate it at the linefeed mark.
            {
                break;
            }
       }
       eptr = eptr->next;
    } /*end of while eptr !=NULL*/

    if(!eptr)   // ByHance, 95,12.5
       goto locate_mark_error_ret;

     // If we found an element, locate the exact character position within
     // that element.
     /*--------------------- ByHance --------
    if ((eptr->type == E_LINEFEED))
    {
        if (eptr->prev!=NULL)
            eptr=eptr->prev;
        *pos = eptr->start_pos+eptr->edata_len-1;
        goto locate_mark_exit;
    }
    if  (eptr->type == E_END)
     --------------------- ByHance --------*/

    //if (eptr->type == E_END || eptr->type == E_LINEFEED)  // ByHance,96,4.6
    if( eptr->type==E_END || eptr->type==E_LINEFEED || eptr->type==E_FORMTAB)
    {
        *pos = eptr->start_pos;
        goto locate_mark_exit;
    }

    if (prebox && eptr->prev)
    {
        eptr=eptr->prev;
        *pos=eptr->start_pos;
    }
    else     /*---- in this mark, try next_linefeed_mark -----*/
    {
        *pos = x2Pos(eptr,x)+eptr->start_pos;
        if(eptr->next && eptr->next->type==E_LINEFEED   // 95,12.14
        && *pos==eptr->next->start_pos)   // it is in End of Line
        {
           eptr=eptr->next;
        }
    }

  locate_mark_exit:
    if(*pos>=hw->TextLength)                // ByHance, 95,12.5
       *pos=hw->TextLength-1;
    HandleUnlock(ItemGetHandle(HBox));
    return(eptr);
}

// why give the new
Pmark_rec BoxXYToPos(HBOX HBox,HBOX *NewHBox,int X,int Y,int *Pos)
{
  Pmark_rec eptr;

  eptr=LocateMark(HBox,X,Y,Pos);
  *NewHBox=HBox;
  if (*Pos<0)
     *Pos=0;
  return(eptr);
}

int PosToBoxLineColumn(HBOX HBox,HBOX *NewHBox,int Pos,int *Line,int *Column)
{
  TextBoxs *TextBox;
  Pmark_rec eptr,linehead;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  eptr=LocateMarkbyPos(TextBox,Pos);
  linehead= LocateMarkbyLine(TextBox,eptr->line_number);
  *Line=eptr->line_number;
  *Column=eptr->start_pos-linehead->start_pos;

  HandleUnlock(ItemGetHandle(HBox));
  *NewHBox=eptr->hBox;
  ReturnOK();
}

int BoxLineColumnToPos(HBOX HBox,HBOX *NewHBox,int Line,int Column,int *Pos)
{
  TextBoxs *TextBox;
  Pmark_rec eptr;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  eptr=LocateMarkbyLine(TextBox,Line);
  if (eptr!=NULL)
  {
     *Pos=eptr->start_pos+Column;
     if(*Pos>=TextBoxGetTextLength(TextBox))            //ByHance,95,12.13
        *Pos=TextBoxGetTextLength(TextBox)-1;
     *NewHBox=eptr->hBox;
     HandleUnlock(ItemGetHandle(HBox));
     ReturnOK();
  }
         // else                  // by Yang
  *Pos=TextBoxGetTextLength(TextBox)-1;
  *NewHBox=HBox;
  while (TextBoxGetNextLinkBox(TextBox)!=0)  // find the last linked_Box
  {
     *NewHBox=TextBoxGetNextLinkBox(TextBox);
     HandleUnlock(ItemGetHandle(HBox));
     TextBox=HandleLock(ItemGetHandle(*NewHBox));
     if (TextBox==NULL)
        return(-1);
     HBox=*NewHBox;
  }
  HandleUnlock(ItemGetHandle(HBox));
  return(-1);
}

// next paragraph
static Pmark_rec LocateNextSegmentMark(Pmark_rec pmarkCurrent)
{
  Pmark_rec pmark1=pmarkCurrent;
  while(pmark1!=NULL)
  {
         if(MARKISSEGMENTEND(pmark1)) break;
         pmark1=pmark1->next;
  }
  return(pmark1);
}

#ifdef UNUSED   // ByHance, 96,1.30
// prev paragraph
static Pmark_rec LocatePrevSegmentMark(Pmark_rec pmarkCurrent)
{
  Pmark_rec pmark1=pmarkCurrent;
  while(pmark1!=NULL)
  {
         if(MARKISSEGMENTEND(pmark1)) break;
         pmark1=pmark1->prev;
  }
  return(pmark1);
}
#endif

/*
 * Create a formatted element
 * Warning so many data are from Format Env !
 */
// for init mark
static Pmark_rec CreateMark(int type,
        int x, int y, long _edata_pos,int _edata_len)
{
        Pmark_rec eptr;

        eptr = (Pmark_rec)malloc(sizeof(MarkStruct));
        //if(eptr==NULL)
        if(eptr<0x1000)
        {
           ReportMemoryError("createmark");
           return NULL;
        }

        eptr->hBox =CurrentBox ;
        eptr->regno = FormattingReg;
        eptr->type = type;
        eptr->selected = FALSE;
        eptr->x = x;
        eptr->y = y;
        eptr->line_number = LineNumber;
        eptr->CharSize = FormattingTextStyle.CharSize;
        eptr->CharHSize  = FormattingTextStyle.CharHSize;
        eptr->font= FormattingTextStyle.CharFont;
        eptr->slant= FormattingTextStyle.CharSlant;
        eptr->CharColor= FormattingTextStyle.CharColor;
        eptr->ParagraphAlign= FormattingTextStyle.ParagraphAlign;
        eptr->VParagraphAlign= FormattingTextStyle.VParagraphAlign;  //By zjh
        eptr->RowGap= FormattingTextStyle.RowGap;
        eptr->ColGap= FormattingTextStyle.ColGap;       //By zjh
        eptr->UpDown= FormattingTextStyle.UpDown;       //By zjh
        eptr->SubLine= FormattingTextStyle.SubLine;       //By zjh

        eptr->SubLine= FormattingTextStyle.SubLine;       //By zjh 12.1
        eptr->Hollow= FormattingTextStyle.Hollow;       //By zjh 12.1
        eptr->Dim3= FormattingTextStyle.Dim3;       //By zjh 12.1
        eptr->BackGround= FormattingTextStyle.BackGround;       //By zjh 12.1
        eptr->BackColor= FormattingTextStyle.BackColor;       //By zjh 12.1

        eptr->superscript= FormattingTextStyle.Superscript;
        eptr->subscript= FormattingTextStyle.Subscript;

        /* get a unique element id */
        MarkId++;
        eptr->ele_id = MarkId;

         //for a linefeed after a style char change max lineheight
         // do not effort
        if((MaxLineHeight<FormattingTextStyle.CharSize)&&
           (type!=E_LINEFEED))
           MaxLineHeight=FormattingTextStyle.CharSize ;

        // fill it at adjust base Line
        eptr->line_height = -1;

        switch(type)
        {

                case E_INSERTBOX:
                case E_TEXT:
                        eptr->edata_len = _edata_len;
                        eptr->start_pos= _edata_pos;
                        break;
                case E_OMIT:                //By zjh 9.12
                        eptr->type=E_TEXT;
                        eptr->CharSize = 0;
                        eptr->edata_len = _edata_len;
                        eptr->start_pos = _edata_pos;
                        break;
                case E_LINEFEED:
                case E_HARDLINEFEED:        //By zjh
                case E_PAGEFEED:
                case E_FORMTAB:
                case E_END:
                        eptr->CharSize = FormattingTextStyle.CharSize;
                        eptr->width    = FormattingLineWidth-x;
                        eptr->edata_len = _edata_len;
                        eptr->start_pos = _edata_pos;
                        break;
                default:
                        fprintf(stderr, "CreateMark:  Unknown type %d\n", type);
                        eptr->ele_id = MarkId;
                        eptr->edata_len = 0;
                        break;
        }
        return(eptr);
}

/*
 * Set the formatted element into the format list.  Use a pre-allocated
 * list position if possible, otherwise allocate a new list position.
 * hw must be linked box's head
 */
// for init mark list
static void SetMark(PTextBoxs hw,int type,int x,int y,long edata_pos,int edata_len)
{
     Pmark_rec eptr;
     //int x0,x1,y0,y1;

   //if (hw->BoxType==TABLEBOX&&mark_invlad) type=E_OMIT ;     //By zjh 9.12
/*
   if (hw->BoxType==TABLEBOX)     //By zjh 9.12
    {
        FBPGetCellRect((PFormBoxs)hw,FormattingReg,&x0,&y0,&x1,&y1);
        if (y+FormattingTextStyle.CharSize>y1) type=E_OMIT;
    }
*/
   #ifdef OLD_VERSION
     /*
      * There is not pre-allocated format list, or we have reached
      * the end of the pre-allocated list.  Create a new element, and
      * add it.
      */
     if ((hw->formatted_elements == NULL)||
             ((pmarkCurrentAppendPos != NULL)&&(pmarkCurrentAppendPos->next == NULL)))
     {
        eptr = CreateMark( type,  x, y, edata_pos,edata_len);
        pmarkCurrentAppendPos = AddEle(&(hw->formatted_elements), pmarkCurrentAppendPos, eptr);
        return;
     }
     else
     {/*hwp modify AddElement so that AddEle can put into list*/
             eptr = CreateMark(type,  x, y, edata_pos,edata_len);
             pmarkCurrentAppendPos = AddEle(&(hw->formatted_elements), pmarkCurrentAppendPos, eptr);
             return;
     }

  #else       // ByHance, 96,3.26

   eptr = CreateMark(type, x, y, edata_pos,edata_len);
   pmarkCurrentAppendPos=AddEle(&(hw->formatted_elements),
                                 pmarkCurrentAppendPos, eptr);

  #endif   // OLD_VERSION
}


  /*-----!!!! below is really FORMAT_Program ------!!!!*/

//Adjust Base Line & Aligment of Current Line
// use TextIndent FormattingTextStyle
//      MaxLineHeight and Align Mode adjust the mark in current Line
// when CR or EOF, call it
static void AdjustBaseLine(void)
{
  Pmark_rec eptr=pmarkCurrentAppendPos;

  ASSERT(eptr);
  while (eptr&&(eptr->line_number==pmarkCurrentAppendPos->line_number))
  {
        eptr->line_height=GetLineDistant(MaxLineHeight,pmarkCurrentAppendPos->RowGap);
        eptr->ParagraphAlign= FormattingTextStyle.ParagraphAlign;
        eptr->RowGap= FormattingTextStyle.RowGap;
        eptr=eptr->prev;
  }

  eptr=pmarkCurrentAppendPos;
  switch (GetMarkParagraphAlignMode(eptr))
  {
  case ALIGNLEFTRIGHT:
  /* mark                                            |
        1   2        3            4  return
            x'
   x+=      1*       2*               3*         4    return
  */
    if(eptr->type==E_LINEFEED)
    {
       float scale; //=1.0;

       eptr=pmarkCurrentAppendPos->prev;
        // a blank line has a Align mode have no mean Loop will skipped
        // to avoid Divide by Zero is impossible
       if ((pmarkCurrentAppendPos->x!=0)
       && eptr&&(eptr->line_number==pmarkCurrentAppendPos->line_number))
       {
        /*-------- for test, ByHance, 96,1.19 ---
          if (pmarkCurrentAppendPos->x>FormattingTextStyle.CharHSize)
             scale=(float)(FormattingLineWidth-FormattingTextStyle.CharHSize)
                 / (pmarkCurrentAppendPos->x - FormattingTextStyle.CharHSize);
          else
         ---------*/
             scale=(float)FormattingLineWidth/pmarkCurrentAppendPos->x;

          //for each mark in Current Line add width for them
          while (eptr&&(eptr->line_number==pmarkCurrentAppendPos->line_number))
          {
             //when have one element width + ;
              eptr->y+=eptr->line_height;
              //move mark to the pos
              eptr->x=TextIndent+eptr->x*scale;
              eptr->width=eptr->width*scale;
              eptr=eptr->prev;
          }
       }

        //return should have no space, so move it to end of line
       pmarkCurrentAppendPos->y+=pmarkCurrentAppendPos->line_height;
       pmarkCurrentAppendPos->x+=TextIndent+pmarkCurrentAppendPos->width;
       break;
    }
  case ALIGNLEFT :
    while (eptr&&(eptr->line_number==pmarkCurrentAppendPos->line_number))
    {
        eptr->y+=eptr->line_height;
        eptr->x+=TextIndent;
        eptr=eptr->prev;
    }
    break;
  case ALIGNRIGHT  :
    while (eptr&&(eptr->line_number==pmarkCurrentAppendPos->line_number))
    {
        eptr->y+=eptr->line_height;
        eptr->x+=pmarkCurrentAppendPos->width+TextIndent;
        eptr=eptr->prev;
    }
    break;
  case ALIGNCENTRE :
    while (eptr&&(eptr->line_number==pmarkCurrentAppendPos->line_number))
    {
        eptr->y+=eptr->line_height;
        eptr->x+=TextIndent+pmarkCurrentAppendPos->width/2;
        eptr=eptr->prev;
    }
    break;
  default:
          fprintf(stderr,"switch to Align Mode not support!\n");
  } //end of switch Align Mode
}

// When CR or change style, call it
// give region Idx, now y in BOX, required Height,
//  return X(LeftMargin), line_width, new Y

//static OldReg;
static int HBoxGetLine(HTEXTBOX HBox,int iRg,int y,int RequiredHeight,
               int *x0Line,int *wLine,int *yNew)
{
  int Rg;
  PTextBoxs hw;
  HTEXTBOX IterHBox,tmpHBox;
  static int x1;
  //static int save1,save2;



  IterHBox=HBox;
  hw=HandleLock(ItemGetHandle(IterHBox));
  if (hw==NULL)
      return 0;

  if (BoxIsTextBox(HBox))
  {
     Rg=BoxGetLine(hw,iRg,y,RequiredHeight, x0Line,wLine,yNew);
  }
  else  if (BoxIsTableBox(HBox))
  {
     //iRg is the cell number, set y1 to page end
     mark_invlad=0;
     if (BoxIsLocked(hw))
     FormBoxSetRg((PFormBoxs)hw,iRg);                   //By zjh 9.12
      else
     FormBoxSetRgNoBottom(HBox,(PFormBoxs)hw,iRg);

     //x1=*x0Line;          //By zjh 9.12
     //x2=*wLine;
     //x3=*yNew;
     //region tab is omit for space

     Rg=BoxGetLine(hw,0,y,RequiredHeight,x0Line,wLine,yNew);

     //if (Rg==-1)     {     }
     if (Rg==-1)                                    //By zjh 9.12
          {
          mark_invlad=1;
          FBPGetCellRect((PFormBoxs)hw,iRg,x0Line,&x1,&x1,yNew);
          //*x0Line+=DEFAULTBOXTEXTDISTANT;
          //*yNew-=DEFAULTBOXTEXTDISTANT;
          *x0Line += ((PFormBoxs)hw)->TextDistantLeft;
          *yNew -= ((PFormBoxs)hw)->TextDistantBottom;
          //FormattingTextStyle.CharSize=1;
          //FormattingTextStyle.CharHSize=1;
          //FormBoxSetRgNoBottom(HBox,(PFormBoxs)hw,iRg);
          //*x0Line=x1;
          //*wLine=x2;
          //*yNew=x3;
          //Rg=BoxGetLine(hw,0,y,RequiredHeight,x0Line,wLine,yNew);
          }
       else
         {
          //x1=*x0Line;          //By zjh 9.12
          //x2=*wLine;
          //x3=*yNew;
          mark_invlad=0;
          //save1=FormattingTextStyle.CharSize;
          //save2=FormattingTextStyle.CharHSize;
         }
     Rg=iRg;                                      //By zjh 9.12
     //if (Rg!=-1) Rg=iRg; else OldReg=iRg;
  }
  else
  {
     Warning("HBox is neither a TABLE nor a TextBOX");
  }

  if (Rg!=-1)
  {
    //success allocate space
    HandleUnlock(ItemGetHandle(IterHBox));
  }
  else   // find space in all following Link Box
  while (1)
  {
    tmpHBox=TextBoxGetNextLinkBox(hw);
    HandleUnlock(ItemGetHandle(IterHBox));
    IterHBox=tmpHBox;
    if (IterHBox==NULL)    // fail to find next region
       break ;
    hw=HandleLock(ItemGetHandle(IterHBox));
    if (hw==NULL) return(0);

    //find space from start of region
    y = Y0ofRg(&(hw->rgList[0]));
    Rg=BoxGetLine(hw,0,y,RequiredHeight,x0Line,wLine,yNew);

    if (Rg!=-1)
    {
      CurrentBox=IterHBox;
      HandleUnlock(ItemGetHandle(IterHBox));
      break;
    }
  }

  return Rg;
}

/*
 * We have encountered a line break.  Increase the line counter,
 * and move down some space.
 * FormattingTextStyle.CharSize>MaxLineHeight only when next Char is too Height
 * FormattingTextStyle.CharSize=the LineHeight after the LineFeed
 */
static int LineFeed(int *x, int *y )
{
     int newy;
     int RequiredRowGap;

        if (BaseLine <= 0)
            BaseLine = MaxLineHeight;

        RequiredRowGap=GetLineDistant(BaseLine,FormattingTextStyle.RowGap);
        FormattingReg=HBoxGetLine(CurrentBox,FormattingReg,
               *y+RequiredRowGap,
               FormattingTextStyle.CharSize,
               &TextIndent,&FormattingLineWidth,&newy);
        if (FormattingReg>=0)
        {
           *x=0;
           *y=newy;
           FormattingLineRight=TextIndent+FormattingLineWidth;
           BaseLine = -100;
           MaxLineHeight=FormattingTextStyle.CharSize;
           LineNumber++;
        }
        else
        {    //nospace do nothing set full end to
           *x=FormattingLineRight;
           (*y)+=MaxLineHeight;
           LineNumber++;
        }
        return  FormattingReg;
}

static int PageFeed(int *x, int *y )
{
     int newy,myy;
     int RequiredRowGap;
     PTextBoxs hw;

      hw=HandleLock(ItemGetHandle(CurrentBox));
      if (hw==NULL) return -1;
      myy=TextBoxGetBoxBottom(hw)-TextBoxGetTextDistantBottom(hw);
      HandleUnlock(ItemGetHandle(CurrentBox));

        if (BaseLine <= 0)
            BaseLine = MaxLineHeight;

        RequiredRowGap=GetLineDistant(BaseLine,FormattingTextStyle.RowGap);
        FormattingReg=HBoxGetLine(CurrentBox,FormattingReg,
               // By zjh 96.6
               //*y+RequiredRowGap,
               myy,
               FormattingTextStyle.CharSize,
               &TextIndent,&FormattingLineWidth,&newy);
        if (FormattingReg>=0)
        {
           *x=0;
           *y=newy;
           FormattingLineRight=TextIndent+FormattingLineWidth;
           BaseLine = -100;
           MaxLineHeight=FormattingTextStyle.CharSize;
           LineNumber++;
        }
        else
        {    //nospace do nothing set full end to
           *x=FormattingLineRight;
           (*y)+=MaxLineHeight;
           LineNumber++;
        }
        return  FormattingReg;
}

/*************************************************************************
Call when meet FormFeed \09
     Locate x, y to Form's head
     set FormatEnv to new;
note:
     return -1 if no such Cell
     Call only  when Formatting!
     FormattingReg
*************************************************************************/
static int FormFeed(HBOX HBox,int *x, int *y )
{
  PFormBoxs hw;
  int newy;
  hw=HandleLock(ItemGetHandle(HBox));
  if (hw==NULL)
  {
     HandleUnlock(ItemGetHandle(HBox));
     return 0;
  }
  //goto new y
  //goto next Cell
  FormattingReg++;

  //restart from region's top
  //FormBoxSetRg(hw,FormattingReg);                   //By zjh 9.12
  //*y = Y0ofRg(&(hw->rgList[0]));
  *y = Y0ofRg(GetBoxRgListItem(HBox,FormattingReg));
  FormattingReg=HBoxGetLine(HBox,FormattingReg,*y,
          FormattingTextStyle.CharSize, &TextIndent,&FormattingLineWidth,
          &newy);
     {
        *x=0;
        *y=newy;
        FormattingLineRight=TextIndent+FormattingLineWidth;
        BaseLine = -100;
        MaxLineHeight=FormattingTextStyle.CharSize;
        LineNumber++;
    }
    /*
  else                                                      //By zjh 9.12
        {    //nospace do nothing set full end to
           *x=FormattingLineRight;
           (*y)+=MaxLineHeight;
           LineNumber++;
        }
  */
  HandleUnlock(ItemGetHandle(HBox));
  return 0;
  //return FormattingReg;                   //By zjh 9.12
}


/*************************************************************************
!!  Copy from InTextBox
modify
  1. To process tab \09 as if region end
     add following code
        SetMark(hw, E_FORMTAB, ...
        do Adjust
           else if(Cisformtab(*Iter))
           {
               // GOTONEXTCELL
               SetMark(hw, E_FORMTAB,
                      *x, *y, Iter-hw->text,1);
               AdjustBaseLine();
               FormFeed(CurrentBox,x,y);
               ITER ++;
           }  else
  2. change the style of BoxGet next Line
                   GETNEWLINESPACE
*************************************************************************/
static Wchar *FormatTextInFormBox(PFormBoxs hw,Wchar *Iter,int *x,int *y,Wchar *end)
{
   return FormatTextInTextBox((PTextBoxs)hw,Iter,x,y,end);
    /*
   Wchar *ret_p;
   while (1)
   {
   ret_p=FormatTextInTextBox((PTextBoxs)hw,Iter,x,y,end);
   if (ret_p||FormattingReg!=-1) break;
   if (Cisend(*(Iter-1))) break ;
   while (1)
          {
            if (Cisend(*Iter)) return ret_p;
            if (end && Iter>=end+1) return ret_p;
            if (Cisformtab(*Iter)) break;
            Iter++;
          }
   FormattingReg=OldReg;
   }
   return ret_p;
   */
}

/*
 * hw must be linked box's head
 * Format all text in this Box,  return new x,y
 * return NextPos which can not be put in Box
 *    if    NULL , this line is an unfinished line, wait inputing(no formating)
 *    if   !=NULL, format will stop at the segment end;
 */
#define GETNEWLINESPACE        { if (LineFeed(x, y)==-1) goto nospace; }

static Wchar *FormatTextInTextBox1(PTextBoxs hw,Wchar *Iter,
                       int *x,int *y,Wchar *pend);

static Wchar *FormatTextInTextBox(PTextBoxs hw,Wchar *Iter,
                       int *x,int *y,Wchar *pend)
{
   if (hw->BoxType!=TABLEBOX)
       return FormatTextInTextBox1(hw,Iter,x,y,pend);
   else
   {
    Wchar *ret_p,*start,*end;
    Pmark_rec eptr_st,eptr_ed,eptr,eptr_l;
    int iCell,iCellStart,StartPos;
    HBOX HBox;
    int x0,x1,y0,y1,newy0,newy1,linecount,st,ed,bett;
    int reg1,reg2;
    // HANDLE pHBox;

    //eptr_st pointer to the last mark , if its NULL ,means hw->plist
    //eptr_ed pointer to the mark added last

    reg1=FormattingReg;
    start=Iter; end=pend;

    check_link(hw);
    ret_p=FormatTextInTextBox1(hw,Iter,x,y,pend);
    check_link(hw);

    eptr_l=pmarkCurrentAppendPos->next;
    pmarkCurrentAppendPos->next=NULL;

    eptr=pmarkCurrentAppendPos;
    if (!eptr) goto ret_format;
    HBox=eptr->hBox;
    reg2=eptr->regno+1;
    //reg2=reg1+1;
    x0=TableCellGetTextHead(HBox,reg1);
    start=hw->text+x0;
    x1=TableCellGetTextHead(HBox,reg2);
    if (x1<=x0) x1=hw->TextLength;
    end=hw->text+x1;

    while (start<end)
    {
       if (Cisend(*start)) goto ret_format;
       //if (end&&start>=end) goto ret_format;
       //if (start-hw->text>eptr_l->prev->start_pos+eptr_l->prev->edata_len) goto ret_format;
       if (Cstyle(*start)==VPARAGRAPHALIGN&&(*start&0xf)==ALIGNVCENTRE)
       //if (Cstyle(*start)==PARAGRAPHALIGN&&(*start&0xf)==ALIGNCENTRE)
         {
           StartPos=start-hw->text;
           start++;
           eptr=LocateMarkbyPos(hw,StartPos);
           if (!eptr) continue;
           HBox=eptr->hBox;
           iCell=TableTextGetCellNumber(HBox,StartPos);
           iCellStart=TableCellGetTextHead(HBox,iCell);
           FBPGetCellRect((PFormBoxs)hw,iCell,&x0,&y0,&x1,&y1);
           //y1+=DEFAULTBOXTEXTDISTANT;
           //y0-=DEFAULTBOXTEXTDISTANT;
           y1 += ((PFormBoxs)hw)->TextDistantBottom;
           y0 -= ((PFormBoxs)hw)->TextDistantTop;
           eptr=LocateMarkbyPos(hw,iCellStart);
           eptr_st=eptr;
           bett=GetLineBottom(eptr);
           newy1= eptr->y - bett;
           newy0= newy1-eptr->CharHSize;
           st=newy0;
           ed=newy1;
           linecount=1;
           if (newy0>=y0&&newy1<=y1)
             {
                bett= ((PFormBoxs)hw)->TextDistantTop;        //DEFAULTBOXTEXTDISTANT;
                if (eptr->type!=E_END&&eptr->type!=E_FORMTAB)
                while (1)
                {

                    if (!eptr->next) break;
                    eptr=eptr->next;

                    //if (eptr->type==E_TEXT)
                    {
                        newy1= eptr->y - GetLineBottom(eptr);
                        newy0= newy1-eptr->CharHSize;
                        if (newy1>y1)
                        {
                            //if (eptr->prev->type==E_LINEFEED||
                            //    eptr->prev->type==E_HARDLINEFEED)
                            //    {
                            //      linecount--;
                            //      bett-=GetLineBottom(eptr->prev->prev);
                            //    }
                            break;
                        }
                        if (ed<newy1) ed=newy1;
                    }

                       if (eptr->type==E_LINEFEED||
                        eptr->type==E_HARDLINEFEED)
                        {
                             linecount++;
                             bett+=GetLineBottom(eptr->prev);
                        }
                      if (eptr->type==E_END||eptr->type==E_FORMTAB) break;
                }
                bett=(y1-ed+bett)/(linecount+1);
                if (bett<0) bett=0;
                eptr_ed=eptr;
                eptr=eptr_st;
                newy1=bett;
                newy0=((PFormBoxs)hw)->TextDistantTop;             //DEFAULTBOXTEXTDISTANT;
                while (1)
                {

                    //eptr->y=eptr->y-GetLineBottom(eptr)+newy1-newy0;
                    eptr->y=eptr->y+newy1-newy0; //-GetLineBottom(eptr)/2;
                    if (eptr->type==E_LINEFEED||
                        eptr->type==E_HARDLINEFEED)
                         {
                            newy1+=bett;
                            newy0+=GetLineBottom(eptr);
                         }
                    if (eptr->type==E_END||eptr->type==E_FORMTAB) break;
                    if (!eptr->next||eptr==eptr_ed) break;
                    eptr=eptr->next;
                }  //End while

             }  //End if in
         }  //end if Align
      else
        start++;
     }
ret_format:
    //CursorLocate(GlobalBoxHeadHandle,&pHBox,GlobalTextPosition,&x0,&x1);
    pmarkCurrentAppendPos->next=eptr_l;
    check_link(hw);
    return ret_p;
   }
}

static Wchar *FormatTextInTextBox1(PTextBoxs hw,Wchar *Iter,
                       int *x,int *y,Wchar *pend)
{
   HBOX hBoxInsert;
   PTextBoxs pBoxInsert;
   int ncell;
   int newsize,oldsize,boxwidth;

   ASSERT(TextBoxGetPrevLinkBox(hw)==0);
   ncell=((PFormBoxs)hw)->numLines*((PFormBoxs)hw)->numCols;
   while (FormattingReg!=-1)
   {
       if(Cisend(*Iter))
       {
           ASSERT((Iter-hw->text+1)==hw->TextLength);
           SetMark(hw, E_END, *x, *y, Iter-hw->text,1);
           AdjustBaseLine();
           Iter++;
           return NULL;
       }   else

       if(Cispageend(*Iter))
       {
           SetMark(hw, E_PAGEFEED, *x, *y, Iter-hw->text,1);
           Iter++;
           AdjustBaseLine();
           //By zjh 96.6
           //GETNEWLINESPACE;
           if (PageFeed(x,y)==-1) goto nospace;   //By zjh
           if (pend && Iter==pend+1) return Iter;
           continue;
       }    else

       if(Cisformtab(*Iter))
       {           // GOTONEXTCELL
           SetMark(hw, E_FORMTAB, *x, *y, Iter-hw->text,1);
           AdjustBaseLine();
           FormFeed(CurrentBox,x,y);
           Iter ++;
           if (pend && Iter==pend+1) return Iter;
           //if (pend && Iter>=pend) return Iter;         //By zjh 9.12
           continue;
       }  else
       if(Cisparaend(*Iter))
       {
           SetMark(hw, E_HARDLINEFEED, *x, *y, Iter-hw->text,1);
           Iter++;
           AdjustBaseLine();
           //By zjh 96.6
           GETNEWLINESPACE;
           //if (PageFeed(x,y)==-1) goto nospace;   //By zjh
           if (pend && Iter==pend+1) return Iter;
           continue;
       }
       else if(Cisstyle(*Iter))
       {
         Wchar TmpChar=*Iter;
         int  newReg,newx,newy,newwidth;

         switch(Cstyle(TmpChar))
         {
            case CHARFONT:
                 if (!ISeFC(GetAttribute(TmpChar)))
                 {
                    FormattingTextStyle.CharFont =
                     TOcPART(GetAttribute(TmpChar))|
                     GETePART(FormattingTextStyle.CharFont) ;
                 }
                 else
                 {
                    FormattingTextStyle.CharFont =
                     TOePART(GetAttribute(TmpChar))|
                     GETcPART(FormattingTextStyle.CharFont) ;
                 }
                 break;
            case INSERTBOX:
                 hBoxInsert=GetAttribute(TmpChar);
                 pBoxInsert=HandleLock(ItemGetHandle(hBoxInsert));
                 if (pBoxInsert==NULL)
                 {
                    HandleUnlock(ItemGetHandle(hBoxInsert));
                    return NULL;
                 }
                 boxwidth =TextBoxGetBoxWidth(pBoxInsert);
                 oldsize=FormattingTextStyle.CharSize;
                 newsize=TextBoxGetBoxHeight(pBoxInsert);
                 FormattingTextStyle.CharSize=newsize;

                 if( MaxLineHeight<newsize     //add 8.30 by huweiping
                                    //coward to avoid add it exceed line width
                 || (boxwidth+*x>=FormattingLineWidth && *x!=0) )
                 {                //try to allocate new line space
                    newReg=HBoxGetLine(CurrentBox,FormattingReg,*y,newsize,
                             &newx,&newwidth,&newy);
                    if ( newReg==FormattingReg && *x<=newwidth && newy==*y
                      && (boxwidth+*x<FormattingLineWidth || *x==0) )
                    {     // we can have a higher line at origal position
                         //may TextIndent Incorrect by *x is independent on it
                         TextIndent=newx;
                         FormattingLineWidth=newwidth;
                         FormattingLineRight=newx+FormattingLineWidth;
                    }
                    else
                    {   //allocate new space fail
                         // it is break in region or no space
                         /*
                         * For formatted documents there are Height increase
                         * if Line is too heigh find new region and do a Line Feed
                         * else contine as if  nothing happens
                         */
                         FormattingTextStyle.CharSize=oldsize;
                         SetMark(hw,E_LINEFEED,*x,*y,Iter-hw->text,0);
                         FormattingTextStyle.CharSize=newsize;
                         AdjustBaseLine();
                         if( LineFeed(x, y)==-1 )
                         {
                              HandleUnlock(ItemGetHandle(hBoxInsert));
                              goto nospace;
                         }
                    }
                 }

                 SetMark(hw, E_INSERTBOX, *x, *y, Iter-hw->text,1);
                 FormattingTextStyle.CharSize=oldsize;
                 pmarkCurrentAppendPos->CharHSize = \
                 pmarkCurrentAppendPos->width =TextBoxGetBoxWidth(pBoxInsert);
                 pmarkCurrentAppendPos->hInsertBox=hBoxInsert;
                 *x+=boxwidth;
                 HandleUnlock(ItemGetHandle(hBoxInsert));
                 break ;
            case CHARSIZE:
                 //newsize=UncompressCHARSIZE(GetAttribute(TmpChar)*FONTSIZEFACT);
                 newsize=(GetAttribute(TmpChar)*FONTSIZEFACT);
                 if (MaxLineHeight>=newsize)
                     FormattingTextStyle.CharSize=newsize;
                 else
                 {                //try to allocate new line space
                     newReg=HBoxGetLine(CurrentBox,FormattingReg,*y,newsize,
                                         &newx,&newwidth,&newy);
                     if( newReg==FormattingReg && *x<=newwidth && newy==*y )
                     {        // we can have a higher line at origal position
                           FormattingTextStyle.CharSize=newsize;
                             //may TextIndent Incorrect by *x is independent on it
                           TextIndent=newx;
                           FormattingLineWidth=newwidth;
                           FormattingLineRight=newx+FormattingLineWidth;
                     }
                     else
                     {    //allocate new space fail break
                           /*
                           * For formatted documents there are Height increase
                           * if Line is too heigh find new region and do a Line Feed
                           * else contine as if  nothing happens
                           */
                           SetMark(hw, E_LINEFEED, *x, *y, Iter-hw->text,0);
                           FormattingTextStyle.CharSize=newsize;
                           AdjustBaseLine();
                           GETNEWLINESPACE;
                     }
                 }
                 break;
            case CHARSLANT:
                 FormattingTextStyle.CharSlant=GetAttribute(TmpChar);
                 break;
            case CHARHSIZE:
                 //FormattingTextStyle.CharHSize=UncompressCHARSIZE(GetAttribute(TmpChar)*FONTSIZEFACT);
                 FormattingTextStyle.CharHSize=(GetAttribute(TmpChar)*FONTSIZEFACT);
                 break;
            case CHARCOLOR:
                 FormattingTextStyle.CharColor=GetAttribute(TmpChar);
                 break;
            case PARAGRAPHALIGN:
                 FormattingTextStyle.ParagraphAlign=GetAttribute(TmpChar);
                 break;
            case VPARAGRAPHALIGN:
                 FormattingTextStyle.VParagraphAlign=GetAttribute(TmpChar);
                 break;

            case ROWGAP:
                 FormattingTextStyle.RowGap=GetAttribute(TmpChar);
                 break;
            case COLGAP:        //By zjh
                 FormattingTextStyle.ColGap=GetAttribute(TmpChar);
                 break;
            case UPDOWN:        //By zjh
                 FormattingTextStyle.UpDown=GetAttribute(TmpChar);
                 break;
            case SUBLINE:        //By zjh
                 FormattingTextStyle.SubLine=GetAttribute(TmpChar);
                 break;
            case SUPERSCRIPT:
                 FormattingTextStyle.Superscript=GetAttribute(TmpChar);
                 break;
            case SUBSCRIPT:
                 FormattingTextStyle.Subscript=GetAttribute(TmpChar);
                 break;
            default:
                ASSERT(*Iter==TmpChar);
                fprintf(stderr,"unknown char in input stream%d\n",*Iter);
         }/*end of switch char type*/

         Iter++;
         continue;
       }/*end of if style char*/
       else
       {        // it is Code
         Wchar *begin,*start,*end;
         int dir, ascent, descent;
         unsigned int word_width, wasteWidth;
         int line_x;
         int countofwastespace=0;

         line_x = *x;
         FormattingLineStart =end =Iter;

         while(!Cisparaend(*end)&&!Cisstyle(*end)&&!Cisformtab(*end))
         //while(!Cispageend(*end)&&!Cisparaend(*end)&&!Cisstyle(*end)&&!Cisformtab(*end))   //By zjh
         {
           /* make start and end point to one word */
           begin=start = end;
           while(!Cisparaend(*end)&&!Cisstyle(*end)&&!Cisformtab(*end) )
           //while(!Cispageend(*end)&&!Cisparaend(*end)&&!Cisstyle(*end)&&!Cisformtab(*end))   //By zjh
           {
               //assert end+1 will have mean or 0, the text should no be break
               if ( ISNOTBREAKABLE(end) )
               {
                  end++;
                  if( end-begin>MAXWORDLEN ) break;
                  continue;
               }
               else
               {
                  end++; break;
               }
           }

           switch(Cstyle(*start))
           {
             case 0:    // normal Code
               /*****************************************************
               * Add the word to the end of this line, or insert a linefeed
               *  and put the word at the start of the next line.
               *  if the word can not be put in the next line repeat
               *********************************************************/
               GetTextExtents(&FormattingTextStyle, begin, end-begin, &dir,
                        &ascent, &descent, &word_width);

               if( *x+word_width+MarginW<=FormattingLineWidth )
                    goto lbl_put_it;

               if(*x==0)
               {
                    while(end>=begin)
                    {              //force break;
                        end--;
                        GetTextExtents(&FormattingTextStyle, begin, end-begin,
                                  &dir,&ascent, &descent, &word_width);
                        if( *x+word_width+MarginW <= FormattingLineWidth )
                           break;
                    }

                    if (end==begin) // when line is too short that can not put in
                    {          // even one char, we do line feed on a empty line
                        if( LineFeed(x, y)==-1 )
                        {       //backtrace one word
                           Iter = begin;
                           goto nospace;
                        }
                        line_x = *x;
                        word_width=0;
                    }
               }    /*--- *x==0 ----*/
               else
               if( *begin==BLANK && end==begin+1 )
               {    //simple ignore space at end of line
                    countofwastespace++;
               }
               else
               {
                  if (FormattingLineStart!=begin)
                  {
                     while( *(begin-countofwastespace-1)==BLANK )
                     {    //Re-ignore space at end of line
                        countofwastespace++;
                     }

                     GetTextExtents(&FormattingTextStyle,begin-countofwastespace,
                           countofwastespace,&dir,&ascent,&descent,&wasteWidth);

                     SetMark(hw,E_TEXT,line_x,*y,FormattingLineStart-hw->text,
                             begin-FormattingLineStart-countofwastespace);

                     pmarkCurrentAppendPos->width = *x-line_x-wasteWidth+1;
                     FormattingLineStart=begin;
                  }

                  SetMark(hw,E_LINEFEED,*x-wasteWidth,*y,begin-hw->text,0);
                  countofwastespace=0;
                  wasteWidth=0;
                  AdjustBaseLine();
                  if (LineFeed( x, y)==-1)
                  {        //backtrace one word
                     Iter = begin;
                     goto nospace;
                  }

                  line_x = *x;
                  if( *x+word_width+MarginW>FormattingLineWidth )
                  {                 //force break;
                     end=begin;
                     word_width=0;
                  }
               }

             lbl_put_it:
               *x += word_width;
               break;

            default:   //a style char at end of a stream of space
               ASSERT(end==start);
               ASSERT(begin!=start);
                //those space can be compacted but not implement
                //let the outer collect it, so break;
               break;
           } /*end of switch */
         }/*end of while parargraph end*/

         if (FormattingLineStart!=end)
         {  //begin is the begin pos of last word, use end-1 to count last line
             wasteWidth=0;
             if(countofwastespace)
             {
                while( *(end-countofwastespace-1)==BLANK )
                {      //Re-ignore space at end of line
                   countofwastespace++;
                }

                GetTextExtents(&FormattingTextStyle,end-countofwastespace,
                        countofwastespace,&dir,&ascent,&descent,&wasteWidth);
             }

             SetMark(hw,E_TEXT,line_x,*y,FormattingLineStart-hw->text ,
                     end-FormattingLineStart-countofwastespace);
             pmarkCurrentAppendPos->width = *x -line_x -wasteWidth+ 1;
         }
         Iter=end;
         continue;
       } /*- end of if(CStyle) -*/
   } /*while 1*/

  nospace:
     // *Iter is the first char in the Box, text do not include \0, so add 1
/*
    if (hw->BoxType==TABLEBOX)
      {
        while (1)
          {
            if (Cisend(*Iter)) break ;    //return NULL;
            if (pend && Iter>=pend+1) break;   //return Iter;
            if (Cisformtab(*Iter))
            {
             SetMark(hw, E_FORMTAB, *x, *y, Iter-hw->text,1);
             AdjustBaseLine();
             FormFeed(CurrentBox,x,y);
             Iter++;
             return Iter;
            }
            Iter++;
          }
        }
        */
    SetMark(hw,E_END,*x,*y,Iter-hw->text,hw->TextLength-(Iter-hw->text)+1);
    pmarkCurrentAppendPos->width=0;    //the prev linefeed have the space;
    return NULL;
} /* FormatTextInTextBox */

static void FormBoxSetRg(PFormBoxs pBox,int iCell)
{
  int x0,x1,y1,y0,slip;
  slip=FBPGetCellRect(pBox,iCell,&x0,&y0,&x1,&y1);
  FreeRL((PTextBoxs)pBox);
  /*
  x0+=
  x1-=
  y0+=
  y1-=
  */
  InsertTXRg(pBox->rgList,&pBox->numRg,0,x0,x1,y0, x0,x1,y1);
}

//as if FormBox Region has no buttom
//for format Text in Form
static void FormBoxSetRgNoBottom(HBOX HBox, PFormBoxs pBox,int iCell)
{
  int x0,x1,y1,y0,Bottom,slip;
  HPAGE Page;
  Pages *MidPage;

  slip=FBPGetCellRect(pBox,iCell,&x0,&y0,&x1,&y1);
  FreeRL((PTextBoxs)pBox);
 /*----------
  x0+=DEFAULTBOXTABLEDISTANT;
  y0+=DEFAULTBOXTABLEDISTANT;
  x1-=DEFAULTBOXTABLEDISTANT;
  x1-=DEFAULTBOXTABLEDISTANT;
 ---------*/

  Page=ItemGetFather(HBox);             // ByDG, 96,4.12
  MidPage=HandleLock(ItemGetHandle(Page));
  Bottom=PageGetPageHeight(MidPage)-40;
  HandleUnlock(ItemGetHandle(Page));

  InsertTXRg(pBox->rgList,&pBox->numRg,0,x0,x1,y0, x0,x1, Bottom);

  //y1=Bottom;
  /*
  switch (slip)                      //By zjh 96.9.12
  {
  case 2:
  case 0: InsertTXRg(pBox->rgList,&pBox->numRg,0,x0,x1,y0, x0,x1,y1);
          break;
  case 1: InsertTXRg(pBox->rgList,&pBox->numRg,0,x0,x0+1,y0, x0,x1,y1);
          InsertTXRg(pBox->rgList,&pBox->numRg,1,x0,x1,y0, x1-1,x1,y1);
          break;
  }
  */
}

static Pmark_rec GetCurrentLineHead(Pmark_rec pmarkCurrent)
{
  Pmark_rec pmark1,pmarkpre;

  if(!pmarkCurrent) return NULL;

  pmark1=pmarkpre=pmarkCurrent;
  while(pmark1 && pmark1->line_number==pmarkCurrent->line_number)
  {
         pmarkpre=pmark1;
         pmark1=pmark1->prev;
  }
  ASSERT(pmarkpre);
  return(pmarkpre);
}

static void GetCurrentLineInfo(Pmark_rec pmarkCurrent, int bGetRect,
                        int *Left,int *Right,int *Height)
{
  Pmark_rec pmark1;
  Pmark_rec pmarkpre,pmarkend;

  ASSERT(pmarkCurrent);
  if(pmarkCurrent==NULL)        // ByHance,96,2.2
     return;

  *Height=GetCurrentMaxLineHeight(pmarkCurrent);

  pmark1=pmarkpre=pmarkend=pmarkCurrent;
  while(pmark1 && pmark1->line_number==pmarkCurrent->line_number)
  {
         pmarkpre=pmark1;
         pmark1=pmark1->prev;
  }
  ASSERT(pmarkpre);

  pmark1=pmarkCurrent;
  while(pmark1 && pmark1->line_number==pmarkCurrent->line_number)
  {
         pmarkend=pmark1;
         pmark1=pmark1->next;
  }
  ASSERT(pmarkend);

  switch (GetMarkParagraphAlignMode(pmarkend))
  {
   case ALIGNLEFT :
        *Left=pmarkpre->x;
        if(bGetRect)
            *Right=pmarkend->x+pmarkend->width;
        else
        {
            //if (pmarkend->type==E_HARDLINEFEED)
            if (pmarkend->type==E_HARDLINEFEED||pmarkend->type==E_PAGEFEED)     //By zjh
               //*Right=pmarkend->x+pmarkend->CharSize;   // ByHance, 96,2.6
               *Right=pmarkend->x+pmarkend->CharSize/2;
            else
               *Right=pmarkend->x;
        }
        break;
   case ALIGNRIGHT  :
        *Right=pmarkend->x;
        *Left=pmarkpre->x-pmarkend->width;
        break;
   case ALIGNCENTRE :
        *Right=pmarkend->x+pmarkend->width/2;
        *Left=pmarkpre->x-pmarkend->width/2;
        break;
   case ALIGNLEFTRIGHT:
        *Left=pmarkpre->x;
        //---- ByHance, 96,2.3    *Right=pmarkend->x;
        //if (pmarkend->type==E_HARDLINEFEED)
        if (pmarkend->type==E_HARDLINEFEED||pmarkend->type==E_PAGEFEED)
           *Right=pmarkend->x+pmarkend->CharSize/2;
        else
           *Right=pmarkend->x;
        break;
   default:
        //fprintf(stderr,"switch to Align Mode not support!\n");
        break;
  } //end of switch Align Mode
}

//return:  StartChangeLine
//pmarkCurrentAppendPos will be set to last unchanged mark!!!
static int SetFormatEnvForTextBox(HBOX HBox,Pmark_rec p,int*x ,int *y)
{
  Pmark_rec linehead,plist;
  int StartChangeLine;
  PTextBoxs hw;

// Current is the LineHead
  hw=HandleLock(ItemGetHandle(HBox));
  if (hw==NULL)
  {
     HandleUnlock(ItemGetHandle(HBox));
     return(0);
  }

  BaseLine = -100;
  CurrentBox = HBox;

  linehead=GetCurrentLineHead(p);
  if (!linehead||!linehead->prev)
  {
        StartChangeLine=LineNumber=1;
        *y = Y0ofRg(&(hw->rgList[0]));
        FormattingTextStyle=DEFAULTTYPESTYLE;
        FormattingReg=HBoxGetLine(CurrentBox,0,*y,FormattingTextStyle.CharSize,
                                  &TextIndent,&FormattingLineWidth,y);
        pmarkCurrentAppendPos=NULL;
  }
  else
  {
        plist=linehead->prev;         // ByHance, 96,4.12
       /*-----------
        plist=linehead;
        if(plist->prev->type!=E_LINEFEED)
             plist=plist->prev;
        ------------------------------*/

        StartChangeLine=linehead->line_number;
        LineNumber=p->line_number;

             //we have to get y and TextStyle From the forward element
        *y = plist->y;     //start y

        //By zjh   For PageFeed
        if (plist->type==E_PAGEFEED)
        {
         *y=TextBoxGetBoxBottom(hw)-TextBoxGetTextDistantBottom(hw);
        }
        FormattingTextStyle.CharSize=plist->CharSize ;
        FormattingTextStyle.CharHSize=plist->CharHSize;
        FormattingTextStyle.CharFont=plist->font ;
        FormattingTextStyle.CharSlant=plist->slant ;
        FormattingTextStyle.CharColor=plist->CharColor ;
        FormattingTextStyle.ParagraphAlign=plist->ParagraphAlign ;
        FormattingTextStyle.VParagraphAlign=plist->VParagraphAlign ;  //By zjh
        FormattingTextStyle.RowGap=plist->RowGap;
        FormattingTextStyle.ColGap=plist->ColGap;       //By zjh
        FormattingTextStyle.UpDown=plist->UpDown;       // By zjh
        FormattingTextStyle.SubLine=plist->SubLine;       // By zjh

        FormattingTextStyle.Superscript=plist->superscript;
        FormattingTextStyle.Subscript=plist->subscript;
        CurrentBox = plist->hBox;
        FormattingReg=HBoxGetLine(CurrentBox,plist->regno,*y,
              FormattingTextStyle.CharSize,&TextIndent,&FormattingLineWidth,y);
        pmarkCurrentAppendPos=plist;
  }

  *x=0;
  FormattingLineRight=FormattingLineWidth;

  MaxLineHeight=FormattingTextStyle.CharSize;
  HandleUnlock(ItemGetHandle(HBox));

  return StartChangeLine;
}

static int SetFormatEnvForFormBox(HBOX HBox,Pmark_rec p,int*x ,int *y)
{
  Pmark_rec linehead,plist;
  int StartChangeLine;
  PTextBoxs hw;

// Current is the LineHead
  hw=HandleLock(ItemGetHandle(HBox));
  if (hw==NULL)
  {
     HandleUnlock(ItemGetHandle(HBox));
     return(0);
  }

  BaseLine = -100;
  CurrentBox = HBox;

  linehead=GetCurrentLineHead(p);
  if (!linehead||!linehead->prev)
  {
        StartChangeLine=1;
        LineNumber=1;
                //we change region
        *y = Y0ofRg(GetBoxRgListItem(HBox,0));
        FormattingTextStyle=DEFAULTTYPESTYLE;
        FormattingReg=HBoxGetLine(CurrentBox,0,*y,FormattingTextStyle.CharSize,
                                     &TextIndent,&FormattingLineWidth,y);
        pmarkCurrentAppendPos=NULL;
  }
  else
  {
        plist=linehead->prev;
             //Format From p
        LineNumber=StartChangeLine=linehead->line_number;
              //we have to get y and TextStyle From the forward element
        FormattingTextStyle.CharSize=plist->CharSize ;
        FormattingTextStyle.CharHSize=plist->CharHSize;
        FormattingTextStyle.CharFont=plist->font ;
        FormattingTextStyle.CharSlant=plist->slant ;
        FormattingTextStyle.CharColor=plist->CharColor ;
        FormattingTextStyle.ParagraphAlign=plist->ParagraphAlign ;
        FormattingTextStyle.VParagraphAlign=plist->VParagraphAlign ;
        FormattingTextStyle.RowGap=plist->RowGap;

        FormattingTextStyle.ColGap=plist->ColGap;       //By zjh
        FormattingTextStyle.UpDown=plist->UpDown;       // By zjh
        FormattingTextStyle.SubLine=plist->SubLine;       // By zjh

        FormattingTextStyle.Superscript=plist->superscript;
        FormattingTextStyle.Subscript=plist->subscript;

        CurrentBox = plist->hBox;
        if (linehead->prev->type==E_FORMTAB)
        {          //we must do something like formfeed
          FormattingReg=plist->regno+1;

                 //restart from region's top
          *y = Y0ofRg(GetBoxRgListItem(HBox,FormattingReg));
          FormattingReg=HBoxGetLine(HBox,FormattingReg,*y,
              FormattingTextStyle.CharSize,&TextIndent,&FormattingLineWidth,y);
        }
        else
        //By zjh   For PageFeed
        if (plist->type==E_PAGEFEED)
        {
         *y=TextBoxGetBoxBottom(hw)-TextBoxGetTextDistantBottom(hw);
         FormattingReg=HBoxGetLine(CurrentBox,plist->regno,*y,
               FormattingTextStyle.CharSize, &TextIndent,&FormattingLineWidth,y);
        }
        else
        {
           *y = plist->y;      //start y
                 //we must change region to regno + 1
           FormattingReg=HBoxGetLine(CurrentBox,plist->regno,*y,
               FormattingTextStyle.CharSize, &TextIndent,&FormattingLineWidth,y);
        }
        pmarkCurrentAppendPos=plist;
  }

  *x=0;
  FormattingLineRight=FormattingLineWidth;

  MaxLineHeight=FormattingTextStyle.CharSize;
  HandleUnlock(ItemGetHandle(HBox));

  return StartChangeLine;
}

static int SetFormatEnv(HBOX HBox,Pmark_rec p,int*x ,int *y)
{

   if (BoxIsTextBox(HBox))
        return SetFormatEnvForTextBox(HBox,p,x,y);
   else  if (BoxIsTableBox(HBox))
        return SetFormatEnvForFormBox(HBox,p,x,y);
   else
   {
        Warning("HBox is neither a TABLE nor a TextBOX");
   }
   return -1;
}


/*
 * Called by the Editor to format all the objects in the
 * parsed object list to fit its current window size.
 * Returns the max_height of the entire document.
 */
int  FormatAll(HBOX HBox)
{
     PTextBoxs pBox;
     int x, y;
     Wchar *Iter,*TextBlock;
     int handle;
     //int ncell;

     BoxIsModule("FormatAll()",HBox);
   //format all must start from head box
     HBox=GetFirstLinkBox(HBox);
     pBox=HandleLock(ItemGetHandle(HBox));
     if (pBox==NULL)
        return(OUTOFMEMORY);

     handle=TextBoxGetTextHandle(pBox);
     if ( !handle || NULL==(TextBlock=HandleLock(handle)) )
     {
        HandleUnlock(ItemGetHandle(HBox));
        return(OUTOFMEMORY);
     }

     Iter=pBox->text=TextBlock;
                  //    pBox->text[pBox->TextLength]=0;
     SetFormatEnv(HBox,NULL,&x,&y);

   /* Free up previously formatted elements */
     FreeMarkList(pBox->formatted_elements);
     pBox->formatted_elements = NULL;

   /* Start a null element list, to be filled in as we go. */
     if (BoxIsTextBox(HBox))
          FormatTextInTextBox(pBox,Iter,&x,&y,NULL);
     else
     if (BoxIsTableBox(HBox))
          FormatTextInFormBox((PFormBoxs)pBox,Iter,&x,&y,NULL);
     else
          Warning("HBox is neither a TABLE nor a TextBOX");

     // MakeRegionList(pBox->formatted_elements);       // ByHance,96,4.4

     HandleUnlock(TextBoxGetTextHandle(pBox));
     HandleUnlock(ItemGetHandle(HBox));

     if (pmarkCurrentAppendPos->type==E_END)
     {
          HBOX EndHBox=pmarkCurrentAppendPos->hBox;
          //if we are at the last incompleted Line, ChangeLines must add 1
          pBox=HandleLock(ItemGetHandle(EndHBox));
          if (pBox!=NULL)
          {
             if (pmarkCurrentAppendPos->edata_len!=1)
             {
                 if( !TextBoxGetNextLinkBox(pBox) && TextBoxDependPage(pBox) )
                 {
                     TextBoxs *TextBox;
                     HBOX TmpHBox;

                     BoxAutoAppdenPage(EndHBox);
                     TellStatus();      // ByHance, 96,4.9
                     TmpHBox=GetFirstLinkBox(EndHBox);
                     TextBox=HandleLock(ItemGetHandle(TmpHBox));
                     if (TextBox==NULL)
                        return(OUTOFMEMORY);
                     InitRL(TextBox);
                     FormatAll(TmpHBox);
                     HandleUnlock(ItemGetHandle(TmpHBox));
                 }
                 else
                 {
                    TextBoxSetFormatFull(pBox);
                    TextBoxDrawTail(EndHBox,0); // draw full tag
                 }
             }
             else
             {
                 if(TextBoxIsFormatFull(pBox))
                 {
                    TextBoxDrawTail(EndHBox,1);      // clear old tail
                 }
                 TextBoxSetFormatNotFull(pBox);
             }
          }
          HandleUnlock(ItemGetHandle(EndHBox));
     }

   #ifdef TIMING
     {
       struct  time t;
       gettime(&t);
       printf("FormatAll Exit at: %2d:%02d:%02d.%02d\n",
                     t.ti_hour, t.ti_min, t.ti_sec, t.ti_hund);
     }
   #endif

     return(y);
}  /* FormatAll */


void TextBoxInitialLineTable(PTextBoxs TextBox)
{
  // int i;

  ASSERT(TextBox);
  TextBox->formatted_elements=NULL;
  TextBox->elements_end=NULL;
  TextBox->text=NULL;

/*      ByHance
  for (i=0;i<MAXVIRTUALREGIONNUM;i++)
      TextBox->region2mark[i]=NULL;
*/
/*--------- ByHance, 96,4.4 ---
  memset((void *)TextBox->region2mark,0,
        sizeof(TextBox->region2mark[0])*MAXVIRTUALREGIONNUM);
 --------------------*/

  TextBox->numRg=0;
  InitRL(TextBox);
}

void TextBoxDeleteLineTable(PTextBoxs TextBox)
{
  // int i;
  TextBox->text=NULL;
  FreeMarkList(TextBox->formatted_elements);
/*      ByHance
  for (i=0;i<MAXVIRTUALREGIONNUM;i++)
      TextBox->region2mark[i]=NULL;
*/
/*--------- ByHance, 96,4.4 ---
  memset((void *)TextBox->region2mark,0,
        sizeof(TextBox->region2mark[0])*MAXVIRTUALREGIONNUM);
 --------------------*/
  TextBox->formatted_elements=NULL;
  TextBox->elements_end=NULL;
  FreeRL(TextBox);
}

//ReFormatTexts until end is meet
//return  1 if Reformat is stable in this segment
//        0 if the segment has a different position
// FormatEnv must be Correct set
// plist ==NULL when format from head
// end == NULL when format to end of text
static int ReFormatTexts(PTextBoxs hw,Pmark_rec plist,
              Pmark_rec end, int *x, int *y, BOOL bEraseBk)
{
  Wchar *Iter,*newPos,*tail;
  int stable=0;
  int iCell=FormattingReg;

  //if (plist&&hw->BoxType==TABLEBOX&&plist->type==E_FORMTAB) stable=1;
  //if (end&&hw->BoxType==TABLEBOX&&end->type==E_FORMTAB) stable=1;
  //if (end&&hw->BoxType==TABLEBOX&&end->prev&&end->prev->type==E_FORMTAB) stable=1;

  //WaitMessageEmpty();
  //printf("\7");
  //getch();
  //delay(1000);

  //char tmpc[100];
  //if (plist)
  //{
  // my_disp(plist->hBox);
  // sprintf(tmpc,"box:%X,Line:%X,Width:%x,Height:%x",
  //plist->hBox,plist->line_number,

  //);
  //MessageBox("Debug Box",tmpc,1,NULL);
  //}

  if (plist)
        Iter=hw->text+plist->start_pos+plist->edata_len;      // 96,4.12
        //Iter=hw->text+plist->start_pos;
  else
        Iter=hw->text;

  if (end)
     tail=GetElementEdata(hw,end);
  else
     tail=NULL;

  if (hw->BoxType==TABLEBOX)                            //By zjh 9.12
     newPos=FormatTextInFormBox((PFormBoxs)hw,Iter,x,y,tail);
  else
     newPos=FormatTextInTextBox(hw,Iter,x,y,tail);

  //Current pointted to the new mark of end
  if (newPos==NULL)
  {
     ASSERT(pmarkCurrentAppendPos->type==E_END);
     if (pmarkCurrentAppendPos->type!=E_END)
     {
           newPos=NULL;
     }
     FreeMarks(&hw->formatted_elements,
                pmarkCurrentAppendPos->next, NULL, bEraseBk);
     check_link(hw);
     return 1;
  }
  else
  {
  // format in the text
     ASSERT(pmarkCurrentAppendPos->type!=E_END);

     if (newPos==GetElementEdata(hw,end)+1)
     {
        if (pmarkCurrentAppendPos->y==end->y
              // && pmarkCurrentAppendPos->x==end->x
        && pmarkCurrentAppendPos->line_number==end->line_number)
             stable=1;
     }

     // Think of Merge cell
     if (hw->BoxType==TABLEBOX&&iCell!=FormattingReg&&0)
     {
         Pmark_rec htpr;
         htpr=pmarkCurrentAppendPos;
         end=htpr->next;

         while (htpr)   //look for the last mark of format cell
         {
                if (htpr->regno==iCell) break;
                htpr=htpr->prev;
         }

         iCell++;

         while (end)  //look for the first mark of next cell
         {
                if (end->regno==iCell) break;
                end=end->next;
         }
         if (end) end=end->prev;
         stable=1;

         FreeMarks(&hw->formatted_elements,
                   htpr->next, end, bEraseBk);

         LineNumber=htpr->line_number+1;  //modify the change lines

          {   // modify line number for disp
              int myLineNumber;
              Pmark_rec myeptr;
              myeptr=htpr;
              if (myeptr)
              {
               myLineNumber=myeptr->line_number;
               while (1)
                    {
                        if (myeptr->type==E_LINEFEED||
                        myeptr->type==E_HARDLINEFEED||
                        myeptr->type==E_END||
                        myeptr->type==E_FORMTAB) myLineNumber++;
                        myeptr=myeptr->next;
                        if (!myeptr) break;
                        myeptr->line_number=myLineNumber;

                     }
              }    // !myeptr
          }

     }
     else
     FreeMarks(&hw->formatted_elements,
                   pmarkCurrentAppendPos->next, end, bEraseBk);
     check_link(hw);
     return stable;
  }
}

// changedLength:: insert length,   origlength:: delete length
void FormatChangeText(HBOX HBox,int StartPos,int ChangedLength,int OrigLength,
        int *StartChangeLine,int *ChangeLines, BOOL bEraseBk)
{
  TextBoxs *hw;
  Wchar *TextBlock;
  Pmark_rec eptr;
  Pmark_rec ChangeEnd;
  int x, y,FormatStart;
  int iCell,iCellStart,iCellEnd,i,Align=0;

  TextCursorOff();

  HBox=GetFirstLinkBox(HBox);
  hw=HandleLock(ItemGetHandle(HBox));
  if (hw==NULL) return;

  if (hw->BoxType==TABLEBOX)
  {
      iCell=TableTextGetCellNumber(HBox,StartPos);
      iCellStart=TableCellGetTextHead(HBox,iCell);
      iCellEnd=iCellStart+TableCellGetTextLength(HBox,iCell);
  }

  if (!TextBoxGetTextHandle(hw))
  {
      HandleUnlock(ItemGetHandle(HBox));
      return;
  }
  TextBlock=HandleLock(TextBoxGetTextHandle(hw));
  if (TextBlock==NULL)
  {
      HandleUnlock(TextBoxGetTextHandle(hw));
      HandleUnlock(ItemGetHandle(HBox));
      return;
  }

  Align=0;
  if (hw->BoxType==TABLEBOX)
    for (i=iCellStart;i<iCellEnd;i++)
    {
        if (Cstyle(TextBlock[i])==VPARAGRAPHALIGN)
        //if (Cstyle(TextBlock[i])==PARAGRAPHALIGN)
         {
            Align=(TextBlock[i]&0xf);
            if (Align==ALIGNCENTRE) break;
            Align=0;
         }
    }

 //  ASSERT(hw->TextLength>=StartPos);
  if(hw->TextLength<=StartPos) StartPos=hw->TextLength;  // ByHance!!!
  hw->text=TextBlock;
  if((FormatStart=StartPos)>0) FormatStart--;       // 96,4.12
  // FormatStart=StartPos;

/*---- ByHance, 96,3.26 ---
  if (hw->TextLength>0 && TextBlock[hw->TextLength-1]!=0)
  {
        TextBlock[hw->TextLength]=0;
        Warning("Text Do not end by 0 ");
  }
 ------------*/

  /* ReFormat All
  *     1. StartPos  ->line start mark
  *     2. Correct all start_pos and edata_len
  *     3. for all text in current segment
  *          ReFormatText
  *     4. ReFormatText All element followed
  */
  //for avoid the NoBegin char
  while( FormatStart>0 && ISNOTBREAKABLE(TextBlock+FormatStart-1))  //By zjh
     FormatStart--;

  if (hw->BoxType==TABLEBOX&&(Align||FormatStart<iCellStart))
     FormatStart=iCellStart;
  eptr=LocateMarkbyPos(hw,FormatStart);

  if (eptr==NULL)
  {   //it must be at the first call
          FormatAll(HBox);
          *StartChangeLine=1;
          *ChangeLines=LineNumber;
  //   lbl_format_error:
          HandleUnlock(ItemGetHandle(HBox));
          return;
  }

  if (eptr->prev&&(eptr->prev->type==E_LINEFEED)) //By zjh
     eptr=eptr->prev;
  //else
  //if (eptr->type==E_PAGEFEED) // By zjh
 // eptr=eptr->next;
/*-------
  else
  if (eptr->type==E_HARDLINEFEED) // ByHance
     eptr=eptr->next;
----------*/
  //if (eptr->type==E_FORMTAB) eptr=eptr->next;   //By zjh 9.12

// this section correct all StartPos and edata_len
// this time we can not sure StartPos+OrigLength is correct
// the Pos  may have been deleted .

  ChangeEnd=_LocateMarkbyPos(eptr,StartPos+OrigLength);

  *StartChangeLine=SetFormatEnv(HBox,eptr,&x,&y);
  //Current is prev changelist !!
  *ChangeLines=1;

  if(ChangeEnd)
  {
     if(ChangeEnd->start_pos<StartPos+OrigLength)
        ChangeEnd=ChangeEnd->next;

     if (ChangeEnd)
     {
        ASSERT(ChangeEnd->start_pos>=StartPos+OrigLength);
        //form ChangeEnd  Mark's fields are correct   but  start_pos
        // corrent the field start_pos of all the marks which is influenced.
        MoveMarksPosDown(ChangeEnd, ChangedLength-OrigLength );

        //find the segment's mark which was not be influenced by change
        eptr=LocateNextSegmentMark(ChangeEnd);
     }
     else
     {
        // this branch we  are insert or delete in the end !
        // Box is full but the fool guy want to Insert in it
        eptr=NULL;
     }
  }
  else
  {
     //changeEnd is not exist  when ?
     //I am sure ChangeEnd is NULL
     eptr=NULL;
  }

//  if (hw->BoxType==TABLEBOX)  //&&(Align||eptr->start_pos>iCellEnd)) //By zjh 9.12
//  eptr=LocateMarkbyPos(hw,iCellEnd);

  //repeat until stable
  while(1)
  {
    if (ReFormatTexts(hw,pmarkCurrentAppendPos,eptr, &x,&y, bEraseBk))
         break;

    //Current is the last carrige return or end of file
    eptr=LocateNextSegmentMark(pmarkCurrentAppendPos->next);
  }

    // Reestablish the Region mark List.
  //MakeRegionList(hw->formatted_elements);       // ByHance,96,4.4
  *ChangeLines=LineNumber-*StartChangeLine;

  HandleUnlock(TextBoxGetTextHandle(hw));
  HandleUnlock(ItemGetHandle(HBox));

  if (pmarkCurrentAppendPos->type==E_END)
  {
    HBOX EndHBox=pmarkCurrentAppendPos->hBox;
    //if we are at the last incompleted  Line, ChangeLines must add 1
    (*ChangeLines)++;
    hw=HandleLock(ItemGetHandle(EndHBox));
    if (hw!=NULL)
    {
       if (pmarkCurrentAppendPos->edata_len!=1)
       {
           if(!TextBoxGetNextLinkBox(hw) && TextBoxDependPage(hw) )
           {
                TextBoxs *TextBox;
                HBOX TmpHBox;

                BoxAutoAppdenPage(EndHBox);
                TmpHBox=GetFirstLinkBox(EndHBox);
                TextBox=HandleLock(ItemGetHandle(TmpHBox));
                if (TextBox==NULL)
                   return;
                InitRL(TextBox);
                FormatAll(TmpHBox);
                HandleUnlock(ItemGetHandle(TmpHBox));
           }
           else
           {
                TextBoxSetFormatFull(hw);
                TextBoxDrawTail(EndHBox,0);
           }
       }
       else
       {
           if(TextBoxIsFormatFull(hw))
           {
              setwritemode(XOR_PUT);
              TextBoxDrawTail(EndHBox,1);      // clear old tail
              setwritemode(COPY_PUT);
           }
           TextBoxSetFormatNotFull(hw);
       }
    }
    HandleUnlock(ItemGetHandle(EndHBox));
  }
}

void FormatInsertText(HBOX HBox,int StartPos,int Length,
          int *StartChangeLine,int *ChangeLines, BOOL bEraseBk)
{
  FormatChangeText(HBox, StartPos,Length,0,
                 StartChangeLine,ChangeLines, bEraseBk);
}

void FormatDeleteText(HBOX HBox,int StartPos,int Length,
                     int *StartChangeLine,int *ChangeLines, BOOL bErase)
{
  FormatChangeText(HBox, StartPos,0,Length,
                 StartChangeLine,ChangeLines, bErase);   // ByDg
}

Pmark_rec LineToMark(Pmark_rec pmarkHead,int iLine);

/***********************************************************************
Pmark_rec NextBoxFirstMark(Pmark_rec pmarkFrom)
   Find the first mark which has the different hBox field to pmarkFrom
from pmarkFrom.
************************************************************************/
static Pmark_rec NextBoxFirstMark(Pmark_rec pmarkFrom)
{
   HANDLE hOldBox;
   if (pmarkFrom==NULL)
      return NULL;
   else
     hOldBox=pmarkFrom->hBox;

   while(1)
   {
      if (pmarkFrom==NULL) return NULL;
      if (pmarkFrom->hBox!=hOldBox) return pmarkFrom;
      pmarkFrom=pmarkFrom->next;
   }
}

/*********************************************************************
  Return value:
    0: Current Line don't need to be redrawed
    1: Current Line need to be redrawed
**********************************************************************/
static int CurLineInWindow(Pmark_rec pmarkCurrent)
{
     int x0Line,x1Line,hLine;

     GetCurrentLineInfo(pmarkCurrent,1,&x0Line,&x1Line,&hLine);

     if (TextBoxRectInWindow(pmarkCurrent->hBox,
                     x0Line, pmarkCurrent->y-hLine,
                     x1Line, pmarkCurrent->y))
       return 1;
     else
       return 0;
}

/*********************************************************************
  Return value:
    0: mark don't need to be erased
    1: mark has been erased
**********************************************************************/
void EraseMark(Pmark_rec pmarkErase, BOOL bEraseBk)      // ByDg, 96.1.8
{
     PTextBoxs TextBox;
     int wMark,y,height,newy;
     Wchar *TextBlock;
     int start,end,i;

     assert(pmarkErase);
     //     By zjh 9.12
     if (bEraseBk || pmarkErase->type==E_LINEFEED
      || pmarkErase->type==E_HARDLINEFEED || pmarkErase->type==E_END||pmarkErase->type==E_PAGEFEED )
        return;

     //if (bEraseBk || pmarkErase->type!=E_TEXT) return ;

     if(TextBoxRectInWindow(pmarkErase->hBox,
          pmarkErase->x, pmarkErase->y-pmarkErase->line_height,
          pmarkErase->x+pmarkErase->width, pmarkErase->y) )
     {
         TextBox=HandleLock(ItemGetHandle(pmarkErase->hBox));
         if (TextBox==NULL)
            return;


   if (pmarkErase->type==E_TEXT)      //By zjh 9.12    avoid blank
   {
    TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
    if (TextBlock!=NULL)
    {
      start=pmarkErase->start_pos;
      end=start+pmarkErase->edata_len;
      for (i=start;i<end;i++)                  //By zjh 9.12
       {
        if (TextBlock[i]!=BLANK&&TextBlock[i]!=0xa3a0) break;
       }
      if (i>=end)
       {
        HandleUnlock(TextBoxGetTextHandle(TextBox));
        HandleUnlock(ItemGetHandle(pmarkErase->hBox));
        return ;
       }
      HandleUnlock(TextBoxGetTextHandle(TextBox));
    }
   }

         if (TextBox->BoxType==TABLEBOX)                  //By zjh 9.12
            {
            newy= pmarkErase->y - GetLineBottom(pmarkErase);
            FBPGetCellRect((PFormBoxs)TextBox,pmarkErase->regno,&height,&height,&height,&y);
            y += ((PFormBoxs)TextBox)->TextDistantBottom;       //DEFAULTBOXTEXTDISTANT;
            if (newy>=y)
                {
                    //line(start-DEFAULTBOXTEXTDISTANT,y,end+DEFAULTBOXTEXTDISTANT,y);
                    HandleUnlock(ItemGetHandle(pmarkErase->hBox));
                    return ;
                }
            }

         wMark=pmarkErase->width;
         if (pmarkErase->type==E_FORMTAB) wMark=1;       //By zjh 9.12

         // Add By zjh--------------------
         height=pmarkErase->CharSize;       //  GlobalPageScale;
         y=pmarkErase->y;        //-GetLineBottom(pmarkErase);
         newy=pmarkErase->y-GetLineBottom(pmarkErase);
         if (pmarkErase->superscript)
               y-=height/2;
          else if (pmarkErase->subscript)
               y+=height/6;
          else if (pmarkErase->UpDown)           //By zjh
                if (pmarkErase->UpDown&0x400) y-=height*(pmarkErase->UpDown&0x3ff)/10;  //By zjh
                                      else   y+=height*(pmarkErase->UpDown&0x3ff)/10;  //By zjh
         //--------------end------------
         if(wMark)
         {
            MouseHidden();
            if (TextBox->BoxType==TABLEBOX)
            BoxBar(TextBox,pmarkErase->x,newy-pmarkErase->CharHSize-1,
                   pmarkErase->x+wMark,newy+1);
             else
            BoxBar(TextBox,pmarkErase->x,y-pmarkErase->line_height-1,
                   pmarkErase->x+wMark,y+2);
            MouseShow();
         }

         HandleUnlock(ItemGetHandle(pmarkErase->hBox));
     }
}
/*
void TestEraseMark(Pmark_rec pmarkErase, BOOL bEraseBk)
{
   PTextBoxs TextBox;
   Wchar *TextBlock;
   int start,end;
   int x,y,newy,width,height,angleRot;
   int xAxis,yAxis,font;
   Wchar code;
   int StartX,  ImageW_Bit, nw;  //ImageW,
   int FontNum;
   USHORT aw;
   short  lsb;

     assert(pmarkErase);
     if (bEraseBk || pmarkErase->type==E_LINEFEED
      || pmarkErase->type==E_HARDLINEFEED || pmarkErase->type==E_END||pmarkErase->type==E_PAGEFEED )
        return;

   TextBox=HandleLock(ItemGetHandle(pmarkErase->hBox));
   TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
   TextBox->text=TextBlock;

       start=pmarkErase->start_pos;
       end=start+pmarkErase->edata_len;
       if (start>end) goto er_end;

          font=pmarkErase->font;
          y=newy=BoxYToWindowY(pmarkErase->y - GetLineBottom(pmarkErase),TextBox);

          //width=pmarkErase->CharHSize/GlobalPageScale;
          //height=pmarkErase->CharSize/GlobalPageScale;
          width=myUserXToWindowX(pmarkErase->CharHSize);
          height=myUserYToWindowY(pmarkErase->CharSize);

          if (pmarkErase->superscript)
               y-=height/2;
          else if (pmarkErase->subscript)
               y+=height/6;
          else if (pmarkErase->UpDown)           //By zjh
                if (pmarkErase->UpDown&0x400) y-=height*(pmarkErase->UpDown&0x3ff)/10;  //By zjh
                                      else   y+=height*(pmarkErase->UpDown&0x3ff)/10;  //By zjh



          FontNum=GETeFONT(font);

          StartX=BoxXToWindowX(XOfMarkPos(pmarkErase,start),TextBox);
          nw=ImageW_Bit=BoxXToWindowX(XOfMarkPos(pmarkErase,end-1),TextBox)
                   +width-StartX+1;

            //-- clear this area --
          if(TextBlock[end-1]<0xa0) // it is English Char, get aw
          {
              GetTtfWidth(TextBlock[end-1],FontNum,&aw,&lsb);
              nw-=width-(long)width*aw/CHAR_WIDTH_IN_DOT;
          }

          setfillstyle(1,EGA_WHITE);
          if(angleRot)
          {
             ORDINATETYPE xx,yy;
             short XY[8];

             xAxis=BoxXToWindowX(TextBoxGetRotateAxisX(TextBox),TextBox);
             yAxis=BoxYToWindowY(TextBoxGetRotateAxisY(TextBox),TextBox);

             RotatePoint(&xx,&yy,StartX,y-height,xAxis,yAxis,angleRot);
             XY[0]=xx;  XY[1]=yy;
             RotatePoint(&xx,&yy,StartX+nw-1,y-height,xAxis,yAxis,angleRot);
             XY[2]=xx;  XY[3]=yy;
             RotatePoint(&xx,&yy,StartX+nw-1,y,xAxis,yAxis,angleRot);
             XY[4]=xx;  XY[5]=yy;
             RotatePoint(&xx,&yy,StartX,y,xAxis,yAxis,angleRot);
             XY[6]=xx;  XY[7]=yy;
             fillpoly(4,XY);
          }
          else
          {
             bar(StartX,y-height,StartX+nw-1,y);
          }
er_end:
   HandleUnlock(TextBoxGetTextHandle(TextBox));
   HandleUnlock(ItemGetHandle(pmarkDraw->hBox));
} */


#ifdef OLDUNUSE
static void DrawSubLine(int x0,int y0,int x1,int y1,int x2,int y2,int color,int ty)
{
    int width,i;
    POINT midp1,midp2;

    if(PrintingSign)
     width=UserXToWindowX(10);
    else
     width=myUserXToWindowX(10);      // /GlobalPageScale;      /* use 5' size */

    if (width==0) width=1;

    SetDeviceColor(color,0);
    if (PrintingSign)
    switch (ty)
        {
          case 1:         //Dot Line
            width *= 2;
            for (i=0;i<width;i++)
               printer->printScanLine(x2-width/2,x2+width-width/2,y2+i,&SysDc);
            break;
          case 2:        //Normal

            for (i=0;i<width;i++)
               printer->printScanLine(x0,x1,y0+i,&SysDc);
            //printf("%d,%d,%d\n",x0,x1,y0);
            break;
          case 3:         // Bold
           for (i=0;i<2*width;i++)
               printer->printScanLine(x0,x1,y0+i,&SysDc);
            break;
          case 4:
           for (i=0;i<width;i++)
               printer->printScanLine(x0,x1,y0+i,&SysDc);

           for (i=0;i<2*width;i++)
               printer->printScanLine(x0,x1,y0+i+2*width,&SysDc);

           break;
        }
      else
      switch (ty)
        {
          case 1:         //Dot Line
            width *= 2;
            for (i=0;i<width;i++)
               line(x2-width/2,y2+i,x2+width-width/2,y2+i);
            break;
          case 2:        //Normal
            for (i=0;i<width;i++)
               line(x0,y0+i,x1,y1+i);
            break;
          case 3:         // Bold
           for (i=0;i<2*width;i++)
               line(x0,y0+i,x1,y1+i);
            break;
          case 4:
           for (i=0;i<width;i++)
               line(x0,y0+i,x1,y1+i);

           for (i=0;i<2*width;i++)
               line(x0,y0+i+2*width,x1,y1+i+2*width);
           break;
        }
}
#endif

void LineToPath(LPDC lpdc,POINT point1,POINT point2,UINT LineWidth);
static void DrawSubLine(int x0,int y0,int x1,int y1,int x2,int y2,int color,int ty,int rot)
{
    int width,i;
    POINT p1,p2;
    double ang;

    i=6;
    if(PrintingSign)
     width=UserXToWindowX(i);
    else
     width=myUserXToWindowX(i);      // /GlobalPageScale;      /* use 5' size */

    if (width==0) width=1;

    SetDeviceColor(color,0);

    switch (ty)
        {
          case 1:         //Dot Line
            width *= 2;
            p1.x=x2-width/2;             p1.y=y2;
            p2.x=x2+width-width/2;       p2.y=y2;
            LineToPath(&SysDc,p1,p2,width);
            break;
          case 2:        //Normal
            p1.x=x0;             p1.y=y0;
            p2.x=x1;             p2.y=y1;
            LineToPath(&SysDc,p1,p2,width);
            break;
          case 3:         // Bold
            p1.x=x0;             p1.y=y0;
            p2.x=x1;             p2.y=y1;
            LineToPath(&SysDc,p1,p2,width*2);
            break;
          case 4:
            p1.x=x0;             p1.y=y0;
            p2.x=x1;             p2.y=y1;
            LineToPath(&SysDc,p1,p2,width);

            ang=-(rot*3.14159265/180);
            p1.x=(sin(ang)*3.0*width+.2)+x0;
            p1.y=(cos(ang)*3.0*width+.2)+y0;
            p2.x=(sin(ang)*3.0*width+.2)+x1;
            p2.y=(cos(ang)*3.0*width+.2)+y1;
            LineToPath(&SysDc,p1,p2,width*2);
            break;
        }
}

static void DrawMark(Pmark_rec pmarkDraw, int StartPos, BOOL bEraseBk)
{
   PTextBoxs TextBox;
   Wchar *TextBlock;
   int i,start,end;
   int x,y,newy,width,height,angleRot,newx,newx1,newx0,newyw;
   int xAxis,yAxis,font;
   Wchar code;

   TextBox=HandleLock(ItemGetHandle(pmarkDraw->hBox));
   TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));
   TextBox->text=TextBlock;

   if (TextBox->BoxType==TABLEBOX)                  //By zjh 9.12
   {
     newy= pmarkDraw->y - GetLineBottom(pmarkDraw);
     FBPGetCellRect((PFormBoxs)TextBox,pmarkDraw->regno,&start,&i,&end,&y);
     y+= ((PFormBoxs)TextBox)->TextDistantBottom;     //DEFAULTBOXTEXTDISTANT;
     if (newy>=y)
        goto lbl_drawmark_end;
   }

   // Draw each char in mark.
   switch(pmarkDraw->type)
   {
    case E_INSERTBOX:
       DrawBoxInBox(pmarkDraw->hInsertBox,pmarkDraw->hBox,
                   pmarkDraw->x,pmarkDraw->y-pmarkDraw->CharSize);
       break;
    case E_LINEFEED:
    case E_HARDLINEFEED:
    //case E_OMIT:
    case E_PAGEFEED:
    case E_END:
       break;
    default:
       start=pmarkDraw->start_pos;
       end=start+pmarkDraw->edata_len;

       //if(StartPos>start && StartPos<=end)    // ByHance, 96,3.22
       if(StartPos>start) start = StartPos;
       if(start>end) break;

       font=pmarkDraw->font;
       angleRot=TextBoxGetRotateAngle(TextBox);
       y=newy=BoxYToWindowY(pmarkDraw->y - GetLineBottom(pmarkDraw),TextBox);
       newyw=BoxYToWindowY(pmarkDraw->y - GetLineBottom(pmarkDraw)+10,TextBox);

       //dy=BoxYToWindowY(20,TextBox)/4;

       //compute width and height, do not need to rotate.
       if (!PrintingSign)
       {
          int StartX, /*ImageW,*/ ImageW_Bit, nw;
          int FontNum;
          USHORT aw;
          short  lsb;

          //width=pmarkDraw->CharHSize/GlobalPageScale;
          //height=pmarkDraw->CharSize/GlobalPageScale;
          width=myUserXToWindowX(pmarkDraw->CharHSize);
          height=myUserYToWindowY(pmarkDraw->CharSize);

          if (pmarkDraw->superscript)
               y-=height/2;
          else if (pmarkDraw->subscript)
               y+=height/6;
          else if (pmarkDraw->UpDown)           //By zjh
                if (pmarkDraw->UpDown&0x400)
                       y-=height*(pmarkDraw->UpDown&0x3ff)/10;
                else   y+=height*(pmarkDraw->UpDown&0x3ff)/10;

          FontNum=GETeFONT(font);

          StartX=BoxXToWindowX(XOfMarkPos(pmarkDraw,start),TextBox);
          nw=ImageW_Bit=BoxXToWindowX(XOfMarkPos(pmarkDraw,end-1),TextBox)
                   +width-StartX+1;

            //-- clear this area --
          if(TextBlock[end-1]<0xa0) // it is English Char, get aw
          {
              GetTtfWidth(TextBlock[end-1],FontNum,&aw,&lsb);
              nw-=width-(long)width*aw/CHAR_WIDTH_IN_DOT;
          }

          setfillstyle(1,EGA_WHITE);
          if(angleRot)
          {
             ORDINATETYPE xx,yy;
             short XY[8];

             xAxis=BoxXToWindowX(TextBoxGetRotateAxisX(TextBox),TextBox);
             yAxis=BoxYToWindowY(TextBoxGetRotateAxisY(TextBox),TextBox);

             RotatePoint(&xx,&yy,StartX,y-height,xAxis,yAxis,angleRot);
             XY[0]=xx;  XY[1]=yy;
             RotatePoint(&xx,&yy,StartX+nw-1,y-height,xAxis,yAxis,angleRot);
             XY[2]=xx;  XY[3]=yy;
             RotatePoint(&xx,&yy,StartX+nw-1,y,xAxis,yAxis,angleRot);
             XY[4]=xx;  XY[5]=yy;
             RotatePoint(&xx,&yy,StartX,y,xAxis,yAxis,angleRot);
             XY[6]=xx;  XY[7]=yy;
             fillpoly(4,XY);
          }
          else
          {
             bar(StartX,y-height,StartX+nw-1,y);
          }

          if (bEraseBk)
             goto lbl_drawmark_end;
       }
       else
       {                // now is printing
          width=UserXToWindowX(pmarkDraw->CharHSize);
          height=UserYToWindowY(pmarkDraw->CharSize);
          if (pmarkDraw->superscript)
               y-=height/2;
          else if (pmarkDraw->subscript)
               y+=height/6;
          else if (pmarkDraw->UpDown)           //By zjh
                if (pmarkDraw->UpDown&0x400)
                       y-=height*(pmarkDraw->UpDown&0x3ff)/10;
                else   y+=height*(pmarkDraw->UpDown&0x3ff)/10;
       }

 // lbl_drawmark_char:
       if (angleRot)
          yAxis=BoxYToWindowY(TextBoxGetRotateAxisY(TextBox),TextBox);
       for (i=start;i<end;i++)
       {
           code=TextBlock[i];
           //if(code<=BLANK||code==0xa1a1)
           if(code<BLANK)         //By zjh 12.16
               continue;

           // compute (x,y), need to rotate.
           newx=x=BoxXToWindowX(XOfMarkPos(pmarkDraw,i),TextBox);
           newx0=newx+width/2;
           if (!PrintingSign&&!angleRot)
             {
                if (x>SysDc.right||x+width<SysDc.left||
                    y>SysDc.bottom||y+height<SysDc.top)
                continue;
             }
           if (angleRot)
           {
               xAxis=BoxXToWindowX(TextBoxGetRotateAxisX(TextBox),TextBox);
               RotatePoint(&x,&y,x,newy,xAxis,yAxis,angleRot);
           }

           if( code >= 0xa0 )           // It is a Chinese char.
            {
              if (code!=0xa1a1)     //12.16 By zjh
               BuildChineseChar( x, y, code, GETcPART(font),
                                 width, height, pmarkDraw->slant,
                                 angleRot, pmarkDraw->CharColor );
           }
           else           // It is an Englist char.
           {
             if (code!=BLANK)     //12.16 By zjh
               BuildEnglishChar( x, y, code, GETeFONT(font),
                                 width, height, pmarkDraw->slant,
                                 angleRot, pmarkDraw->CharColor );
               newx0-=width/4;
           }

          if (pmarkDraw->SubLine)
          {                                //Add By zjh for subline
            int x0,y0,x1,y1,x2,y2;
            newx1=BoxXToWindowX(XOfMarkPos(pmarkDraw,i+1),TextBox);

            if (angleRot)
            {
              RotatePoint(&x0,&y0,newx, newyw,xAxis,yAxis,angleRot);
              RotatePoint(&x1,&y1,newx1,newyw,xAxis,yAxis,angleRot);
              RotatePoint(&x2,&y2,newx0,newyw,xAxis,yAxis,angleRot);
            }
            else
            {
              x0=newx; y0=newyw;
              x1=newx1; y1=y0;
              x2=newx0; y2=y0;
            }
            DrawSubLine(x0,y0,x1,y1,x2,y2,pmarkDraw->CharColor,pmarkDraw->SubLine,angleRot);
          }
       }//end of for
   }//end of switch

lbl_drawmark_end:
   HandleUnlock(TextBoxGetTextHandle(TextBox));
   HandleUnlock(ItemGetHandle(pmarkDraw->hBox));
} /* DrawMark */


#define IFENDTHENQUIT \
   if (pmarkCurrent==NULL) goto quit;\
   if (pmarkCurrent->line_number>=StartLine+LineSum) goto quit;\
   if ((pmarkCurrent->type==E_END)) \
      { TextBoxDrawTail(HBox,0); goto quit; }

void TextBoxRedrawPart(HBOX HBox,int StartLine,int LineSum, int StartPos, BOOL bEraseBk)
{
  HBOX hCurBox;
  int noCurLine;
  TextBoxs *TextBox;
  Wchar *TextBlock;
  Pmark_rec pmarkHead,pmarkCurrent;
  struct viewporttype TmpViewPort;
  int SaveColor;       //,SaveLineStyle;

  HBox=GetFirstLinkBox(HBox);
// 1. Test the validness of the parameter
  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL) return;
  if (!TextBoxGetTextHandle(TextBox))
  {
          HandleUnlock(ItemGetHandle(HBox));
          return;
  }
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox)); //commented for test.
  if (TextBlock==NULL)
  {
          HandleUnlock(ItemGetHandle(HBox));
          return;
  }

  TextCursorOff();
// 2. Set the Viewport according to window
  if (!PrintingSign)
  {
     int Left,Top,Right,Bottom;

     MouseHidden();
     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
     SaveColor=getcolor();
     //SaveLineStyle=getlinestyle();
  }

  TextBox->text=TextBlock;

// 3. Initialize Loop parameter.
  pmarkHead = TextBox->formatted_elements;
  pmarkCurrent= LineToMark(pmarkHead,StartLine);

// 4. Redraw all the text between pmarkStart to pmarkStop inclusively.
  hCurBox=-1;
  noCurLine=-1;
  while (1)
  {
      IFENDTHENQUIT;

      if (pmarkCurrent->hBox!=hCurBox)      // Box changes.
      {
          if (hCurBox!=-1)
              HandleUnlock(ItemGetHandle(hCurBox));

          while(1)          // find next box that need to be redrawed.
          {
             IFENDTHENQUIT;
             if (TextBoxInWindow(pmarkCurrent->hBox)) break;
             pmarkCurrent=NextBoxFirstMark(pmarkCurrent);
          }

          IFENDTHENQUIT;

          hCurBox=pmarkCurrent->hBox;
          TextBox=HandleLock(ItemGetHandle(hCurBox));
      }

      if ((pmarkCurrent->line_number!=noCurLine))      // Line changes
      {
          while (1)          // Find next line that need to be redrawed.
          {
              IFENDTHENQUIT;

              if (pmarkCurrent->hBox!=hCurBox) break;
              if (CurLineInWindow(pmarkCurrent)) break;
              pmarkCurrent=LineToMark(pmarkHead,pmarkCurrent->line_number+1);
          }

          IFENDTHENQUIT;

          if (pmarkCurrent->hBox!=hCurBox) continue;
          noCurLine=pmarkCurrent->line_number;
      }

      // Redraw current mark. // Add By DG
      //if (pmarkCurrent->type==E_PAGEFEED)           //By zjh
      //  TextBoxDrawTail(pmarkCurrent->hBox,0);      //By zjh
      //  else                                        //By zjh
      //if (pmarkCurrent->type!=E_OMIT)                 //By zjh 9.12
      DrawMark(pmarkCurrent, StartPos, bEraseBk);

      // Goto next mark.
      pmarkCurrent=pmarkCurrent->next;
  }

// 5. Normal return
quit:
  if (!PrintingSign) {
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
     setcolor(SaveColor);
    // setlinestyle(SaveLineStyle);
     MouseShow();
  }

  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HBox));
  if (hCurBox!=-1)
    HandleUnlock(ItemGetHandle(hCurBox));
}

void TextBoxRedraw(HBOX HBox,int StartLine,int LineSum, BOOL bEraseBk)
{
   TextBoxRedrawPart(HBox,StartLine,LineSum,0,bEraseBk);
}

static void InverseBoxRect(TextBoxs *TextBox,int Left,int Top,int Right,int Bottom)
{
  int test1;
  int Edges[10],numEdge;
  int RotateAxisX,RotateAxisY,RotateAngle;
  int i;
  DC dc;
  struct viewporttype TmpViewPort;
  int SaveColor;
  int test2;

  test1=test2=0;
  if (TextBox==NULL)
     return;
  if (TextBoxGetBoxType(TextBox)==TEXTBOX
      ||TextBoxGetBoxType(TextBox)==TABLEBOX)
  {
     Left+=TextBoxGetBoxLeft(TextBox);
     Right+=TextBoxGetBoxLeft(TextBox);
     Top+=TextBoxGetBoxTop(TextBox);
     Bottom+=TextBoxGetBoxTop(TextBox);

     Edges[0]=Left;
     Edges[1]=Top;
     Edges[2]=Right;
     Edges[3]=Top;
     Edges[4]=Right;
     Edges[5]=Bottom;
     Edges[6]=Left;
     Edges[7]=Bottom;

     RotateAngle=TextBoxGetRotateAngle(TextBox);

     if (RotateAngle)
     {
        RotateAxisX=TextBoxGetRotateAxisX(TextBox)+TextBoxGetBoxLeft(TextBox);
        RotateAxisY=TextBoxGetRotateAxisY(TextBox)+TextBoxGetBoxTop(TextBox);

        for (i=0;i<4;i++)
            Rotate((ORDINATETYPE *)&Edges[2*i],(ORDINATETYPE *)&Edges[2*i+1],
                   Edges[2*i],Edges[2*i+1],
                   RotateAxisX,RotateAxisY,RotateAngle);
     }

     for (i=0;i<4;i++)
     {
         Edges[2*i]=UserXToWindowX(Edges[2*i]);
         Edges[2*i+1]=UserYToWindowY(Edges[2*i+1]);
     }

     SaveColor=getcolor();
     setcolor(EGA_WHITE);
     setwritemode(XOR_PUT);
   #ifdef _TURBOC_
     setlinestyle(SOLID_LINE,1,1);
   #else
     setlinestyle(0xffff);
   #endif

     getviewsettings(&TmpViewPort);
     WindowGetRect(1,&dc.left,&dc.top,&dc.right,&dc.bottom);
     setviewport(dc.left,dc.top,dc.right,dc.bottom,1);
     dc.left=dc.top=0;
     dc.right=dc.bottom=2048;

     // Using default PolyPolygon fill function to draw text.
     numEdge=4;
     PolyFillPolygon((LPDC)&dc,(LPPOINT)&Edges,(LPINT)&numEdge,1);

     setwritemode(COPY_PUT);
     setcolor(SaveColor);
     setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
  }
  assert(test1==0&&test2==0);
}

#define SETINLIMIT(value,limit1,limit2) \
        ((value>limit2)\
         ?limit2\
         :( (value<limit1)\
            ?limit1:value))
static void RectFitinWindow(int * x0,int * y0,int * x1, int * y1, int window)
{
     int Left,Top,Right,Bottom;
     WindowGetRect(window,&Left,&Top,&Right,&Bottom);
     Right-=Left;
     Bottom-=Top;
     Left=Top=0;
     *x0=SETINLIMIT(*x0,Left,Right);
     *x1=SETINLIMIT(*x1,Left,Right);
     *y0=SETINLIMIT(*y0,Top,Bottom);
     *y1=SETINLIMIT(*y1,Top,Bottom);
}

static void DisplayBlock2(HBOX HBox,int posBStart,int posBStop)
{
   PTextBoxs pBox;
   Pmark_rec pmarkStart,pmarkCurrent,pmarkStop;
   int x0Start,x1Stop,yStartBot,yStopBot;
   HBOX hFirstBox,hTmp,hBoxBStart,hBoxBEnd;
   struct viewporttype TmpViewPort;
   int SaveColor;

// 1. Set the Viewport according to window
  {
     int Left,Top,Right,Bottom;
     getviewsettings(&TmpViewPort);
     SaveColor=getcolor();
     WindowGetRect(1,&Left,&Top,&Right,&Bottom);
     setviewport(Left,Top,Right,Bottom,1);
  }

// 2. Initialize Loop parameter.
   hFirstBox=GetFirstLinkBox(HBox);
   pBox=HandleLock(ItemGetHandle(hFirstBox));
   if (pBox==NULL)
          return;

   TextCursorOff();
   MouseHidden();
   pmarkStart= LocateMarkbyPos(pBox,posBStart);
   hBoxBStart=pmarkStart->hBox;
   pmarkCurrent=pmarkStart;
   pmarkStop = LocateMarkbyPos(pBox,posBStop);
   HandleUnlock(ItemGetHandle(hFirstBox));

   PosToBoxXY(HBox,&hTmp,posBStart,&x0Start,&yStartBot);
   PosToBoxXY(HBox,&hBoxBEnd,posBStop,&x1Stop,&yStopBot);

   while (1)
   {
        int x0,x1,y0,y1;
        int x0Win,x1Win,y0Win,y1Win;
        int hMax,yCut;
        int line_number1;

        if (pmarkCurrent==NULL) break;
        if (pmarkCurrent->type==E_END) break;

     // Get the Draw area of Current Line.
        GetCurrentLineInfo(pmarkCurrent,0,&x0,&x1,&hMax);
        line_number1=pmarkCurrent->line_number;
        y1=pmarkCurrent->y;
        y0=y1-hMax;

        if (pmarkCurrent==pmarkStart)
            x0=x0Start;      // this is the first line of the block

        if( (pmarkStop==pmarkCurrent||pmarkStop==pmarkCurrent->next
            ||pmarkStop->line_number==pmarkCurrent->line_number)
         && y1==yStopBot&&x0<=x1Stop&&x1Stop<=x1)
            x1=x1Stop;       // this is the last line of the block

        if (TextBoxRectInWindow(pmarkCurrent->hBox,x0,y0,x1,y1))
        {   // Block need to be redrawed
           pBox=HandleLock(ItemGetHandle(pmarkCurrent->hBox));
           if (TextBoxGetRotateAngle(pBox)==0)
           {          // Do not need rotate.
               y0Win=BoxYToWindowY(y0,pBox);
               y1Win=BoxYToWindowY(y1,pBox)-1;
               x0Win=BoxXToWindowX(x0,pBox)+1;
               x1Win=BoxXToWindowX(x1,pBox);

               RectFitinWindow(&x0Win,&y0Win,&x1Win,&y1Win,1);

               if (x0Win<=x1Win)
               {        // Inverse Current Line.
                  int SaveColor;

                  SaveColor=getcolor();
                  setcolor(EGA_WHITE);
                  setwritemode(XOR_PUT);

                  #ifdef _TURBOC_
                    setlinestyle(SOLID_LINE,1,1);
                  #else
                    setlinestyle(0xffff);
                  #endif

                  for (yCut=y0Win;yCut<=y1Win;yCut++)
                     line(x0Win,yCut,x1Win,yCut);

                  setwritemode(COPY_PUT);
                  setcolor(SaveColor);
               }
           }
           else
           {
              InverseBoxRect(pBox,x0,y0,x1,y1);
           }
           HandleUnlock(ItemGetHandle(pmarkCurrent->hBox));
        }

     // if this is the last line of block,we can quit now.
        if ((pmarkStop==pmarkCurrent)&&
           (y1==yStopBot)&&(x0<=x1Stop)&&(x1Stop<=x1))
               break;

     // Goto Next Line's first mark.
        while(pmarkCurrent->line_number==line_number1)
        {
          if (pmarkCurrent==NULL) goto out;
          if (pmarkCurrent->type==E_END) goto out;
          if (pmarkStop==pmarkCurrent) goto out;
          pmarkCurrent=pmarkCurrent->next;
        }
  }

// 5. Normal return
out:
   setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
              TmpViewPort.bottom,TmpViewPort.clip);
   setcolor(SaveColor);
   MouseShow();
}

/*
  This function is to deal with the Rotate Box, let it no rest lines in screen.

  when text box has been rotated and will display posBStart to posBEnd,
  like follow two case:
    1. line1: posBStart
       line2:             posBEnd
    2. line1: posBStart   posBEnd
  we deal case 1 to:
      1) deal line1:         from posBStart to LineEnd
      2) deal line1-line2-1: from LineStart to LineEnd
      3) deal line2:         from LineStart to posBEnd
  the above three step all can deal with case2.
  we deal case 2 to:
      1) deal line1: if (LineStart!=posBStart)
                        Deal LineStart to posBStart
      2) deal line1: LineStart to posBEnd
  because the operation is display in XOR mode, display twice equal not do.
 */

void DisplayBlock(HBOX HBox,int posBStart,int posBStop)
{
  TextBoxs *TextBox;
  HBOX NewHBox;
  int LineStart1,LineStart2,CursorY2,CursorX2,CursorY1,CursorX1,RotateAngle;

  if(IsInGlobalBrowser())
     return;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return;
  RotateAngle=TextBoxGetRotateAngle(TextBox);
  HandleUnlock(ItemGetHandle(HBox));
  if (!RotateAngle)
  {
     DisplayBlock2(HBox,posBStart,posBStop);
     return;
  }

  PosToBoxXY(HBox,&NewHBox,posBStart,&CursorX1,&CursorY1);
  PosToBoxXY(HBox,&NewHBox,posBStop,&CursorX2,&CursorY2);
  if (CursorY1==CursorY2)
  {
     if (CursorX1!=0)
     {
        PosToBoxLineColumn(HBox,&NewHBox,posBStart,&CursorY1,&CursorX1);
        BoxLineColumnToPos(HBox,&NewHBox,CursorY1,0,&LineStart1);
        DisplayBlock2(HBox,LineStart1,posBStart);
     }
     DisplayBlock2(HBox,LineStart1,posBStop);
  }
  else
  {
     int LineIndex;

     PosToBoxLineColumn(HBox,&NewHBox,posBStart,&CursorY1,&CursorX1);
     PosToBoxLineColumn(HBox,&NewHBox,posBStop,&CursorY2,&CursorX2);
     BoxLineColumnToPos(HBox,&NewHBox,CursorY1+1,0,&LineStart2);
     BoxLineColumnToPos(HBox,&NewHBox,CursorY1,0,&LineStart1);
     DisplayBlock2(HBox,LineStart1,posBStart);
     DisplayBlock2(HBox,LineStart1,LineStart2);

     for (LineIndex=CursorY1+1;LineIndex<CursorY2;LineIndex++)
     {
         BoxLineColumnToPos(HBox,&NewHBox,LineIndex,0,&LineStart1);
         BoxLineColumnToPos(HBox,&NewHBox,LineIndex+1,0,&LineStart2);
         DisplayBlock2(HBox,LineStart1,LineStart2);
     }

     BoxLineColumnToPos(HBox,&NewHBox,CursorY2,0,&LineStart1);
     DisplayBlock2(HBox,LineStart1,posBStop);
  }
}

/*--ByHance: if it's LINEFEED, return 1, otherwise, return 0 ---*/
int PosToBoxRowEnd(HBOX HBox,int Pos,int *NewPosition,int *Y,int *X)
{
  TextBoxs *TextBox;
  int row;
  Pmark_rec eptr;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);

  *NewPosition=Pos;
  eptr=LocateMarkbyPos(TextBox,Pos);
  if(!eptr)
    goto lbl_err;

  row=eptr->line_number;
  while (eptr && eptr->line_number==row)
  {
      if( eptr->type==E_LINEFEED||eptr->type==E_HARDLINEFEED
      || eptr->type==E_FORMTAB || eptr->type==E_END ||eptr->type==E_PAGEFEED)
           break;
      eptr=eptr->next;
  }

  if(!eptr)
    goto lbl_err;

  *NewPosition = eptr->start_pos;
  if (eptr->type == E_LINEFEED)
  {
      *X=eptr->x;
      *Y=eptr->y - GetLineBottom(eptr);
      HandleUnlock(ItemGetHandle(HBox));
      return 1;
  }

lbl_err:
  HandleUnlock(ItemGetHandle(HBox));
  return 0;
}

int GetLineText(HBOX HBox,int Pos,char *str)
{
  TextBoxs *TextBox;
  int row,i,k,start,end;
  Wchar code;
  Pmark_rec eptr,pptr,nptr;
  Wchar *TextBlock;

  TextBox=HandleLock(ItemGetHandle(HBox));
  if (TextBox==NULL)
     return(OUTOFMEMORY);
  TextBlock=HandleLock(TextBoxGetTextHandle(TextBox));

  eptr=LocateMarkbyPos(TextBox,Pos);
  if(!eptr)
  {
  lbl_err:
     HandleUnlock(TextBoxGetTextHandle(TextBox));
     HandleUnlock(ItemGetHandle(HBox));
     return -1;
  }

  row=eptr->line_number;

  //------- seek first mark at this row -------
  pptr=eptr;
  while (pptr->prev && pptr->prev->line_number==row)
      pptr=pptr->prev;

  //------- seek last mark at this row -------
  nptr=eptr;
  while (nptr->next && nptr->next->line_number==row)
      nptr=nptr->next;

  i=0; str[0]=0;
  eptr=pptr;
  while(1)
  {
     if(eptr->type==E_LINEFEED||eptr->type==E_HARDLINEFEED
     || eptr->type==E_FORMTAB || eptr->type==E_END ||eptr->type==E_PAGEFEED)
        break;

     start=eptr->start_pos;
     end=start+eptr->edata_len;
     k=start;
     while(TextBlock[k]<=BLANK||TextBlock[k]==0xa1a1) k++;

     for(;k<end;k++)
     {
         code=TextBlock[k];
         if(code<BLANK)
             continue;

         if(code==0xa1a1)
             code=BLANK;
         else
         if(code>=0xa000 || i>510)
             goto lbl_err;
         else
         if( (code&ATTRIBUTEPRECODE)!=HIGHENGLISHCHAR )
             continue;

         str[i++]=code;
     }

     if(eptr==nptr)
        break;
     eptr=pptr->next;
  } /*- while 1 -*/

  HandleUnlock(TextBoxGetTextHandle(TextBox));
  HandleUnlock(ItemGetHandle(HBox));
  str[i]=0;

  if(str[0]==0)
     return -1;

  ReturnOK();
}

void AdjustTableCells(HBOX HBox)
{
    int leftCell, topCell, rightCell, bottomCell,NewBottomCell;
    Pmark_rec pMark;
    int yMark,Position;
    TextBoxs *TextBox;

    TextBox=HandleLock(ItemGetHandle(HBox));
    if (TextBox==NULL)
       return;

    if (BoxIsLocked(TextBox))
      {
        HandleUnlock(ItemGetHandle(HBox));
        return ;
      }
    if (GlobalTableCell<0) GlobalTableCell=0;
    FBGetCellRect(HBox, GlobalTableCell, &leftCell, &topCell, &rightCell, &bottomCell);
    Position=TableCellGetTextHead(HBox, GlobalTableCell)+
                   TableCellGetTextLength(HBox, GlobalTableCell)-1;
    pMark=LocateMarkbyPos(TextBox,Position);
    yMark=pMark->y;
    //if(pMark->type==E_HARDLINEFEED)
    if(pMark->type==E_HARDLINEFEED||pMark->type==E_PAGEFEED)  //By zjh
        yMark += GetLineDistant(pMark->CharSize,pMark->ParagraphAlign);

    if (bottomCell<yMark)         // Adjust TableBox
    {
        NewBottomCell=yMark+((PFormBoxs)TextBox)->TextDistantBottom;       //DEFAULTBOXTEXTDISTANT;
        if (FBChangeCellBottomLine(HBox,GlobalTableCell,NewBottomCell)>=0)
        {
            TextBoxSetBoxHeight(TextBox,TextBoxGetBoxHeight(TextBox)+NewBottomCell-bottomCell);
            ReFormatTableText(HBox,FALSE);
            BoxChange(HBox,GlobalCurrentPage);
            RedrawUserField();
            FileSetModified();
        }
    }
    HandleUnlock(ItemGetHandle(HBox));
}

#define DEFAULTCELLBIT DEFAULTBOXTEXTDISTANT
void AddRowTableCells(HBOX HBox,int dir)
{
    int leftCell, topCell, rightCell, bottomCell,NewBottomCell;
    TextBoxs *TextBox;
    HANDLE pHBox;

    TextBox=HandleLock(ItemGetHandle(HBox));
    if (TextBox==NULL)
       return;


    if (GlobalTableCell<0) GlobalTableCell=0;
    //GlobalTextPosition=TableCellGetTextHead(HBox, GlobalTableCell)+1;
    //CursorLocate(HBox,&HBox,GlobalTextPosition,&leftCell,&leftCell);


    FBGetCellRect(HBox, GlobalTableCell, &leftCell, &topCell, &rightCell, &bottomCell);

    {
        NewBottomCell=bottomCell+DEFAULTCELLBIT*dir;
        if (FBChangeCellBottomLine(HBox,GlobalTableCell,NewBottomCell)>=0)
        {
            TextBoxSetBoxHeight(TextBox,TextBoxGetBoxHeight(TextBox)+NewBottomCell-bottomCell);
            ReFormatTableText(HBox,FALSE);
            BoxChange(HBox,GlobalCurrentPage);
            RedrawUserField();
            FileSetModified();
            GlobalTextPosition=TableCellGetTextHead(HBox, GlobalTableCell);
            CursorLocate(HBox,&pHBox,GlobalTextPosition,&leftCell,&rightCell);
        }
    }
    HandleUnlock(ItemGetHandle(HBox));
}

void AddColTableCells(HBOX HBox,int dir)
{
    int leftCell, topCell, rightCell, bottomCell,NewBottomCell;
    TextBoxs *TextBox;
    HANDLE pHBox;

    TextBox=HandleLock(ItemGetHandle(HBox));
    if (TextBox==NULL)
       return;

    if (GlobalTableCell<0) GlobalTableCell=0;


    FBGetCellRect(HBox, GlobalTableCell, &leftCell, &topCell, &rightCell, &bottomCell);

    {
        NewBottomCell=rightCell+DEFAULTCELLBIT*dir;
        if (FBChangeCellRightLine(HBox,GlobalTableCell,NewBottomCell)>=0)
        {
            TextBoxSetBoxWidth(TextBox,TextBoxGetBoxWidth(TextBox)+NewBottomCell-rightCell);
            ReFormatTableText(HBox,FALSE);
            BoxChange(HBox,GlobalCurrentPage);
            RedrawUserField();
            FileSetModified();
            GlobalTextPosition=TableCellGetTextHead(HBox, GlobalTableCell);
            CursorLocate(HBox,&pHBox,GlobalTextPosition,&leftCell,&rightCell);
        }
    }
    HandleUnlock(ItemGetHandle(HBox));
}
