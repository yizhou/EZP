/*-------------------------------------------------------------------
* Name: convert.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"
#undef UP
#undef DOWN
#undef LEFT
#undef RIGHT
#undef MID

static char CONVERT_TMP_FILE[]="convtemp.$$$";

#define MAXITEM (MAXFORMLINE+20)     // 大于MAXFORMLINE即可,
typedef struct {
   int set[MAXITEM];
   unsigned int tail; //元素个数,初始化为0
} SetInt;

typedef struct {
   int  top,left,bottom,right;
   char *text;
   int  length;         // length of text
} MYCELL, *PMYCELL;

typedef struct {
   SetInt vert,hori;  // 横纵交点坐标集合
   int widthofchar;   // 表格中一个中文制表符的宽度;
   int *piHeight;     // 表格的各文本行的高度值,
   int iLineCount;    // 表格行数统计
   int iCellCount;    // 表元个数;
   PMYCELL pMyCell;   // 表元参数;
} MYFORM,*PMYFORM;

typedef struct myforms {
   PMYFORM pmyform;
   struct  myforms *next;
} MYFORMS,*PMYFORMS;
PMYFORMS pmyforms=NULL;

typedef struct {
  DWORD lineno;                 // 总行号
  DWORD y;
  SHORT x;                      // 当前行的起始坐标┌ x = 0
                                //                 y
  USHORT   cfont ;              // 当前中文字体控制码
  USHORT   efont ;              // 当前英文字体控制码
  USHORT   size ;               // 当前字号控制码,六大类
  USHORT   chardistance ;       // 字间距控制码
  USHORT   linedistance ;       // 行间距控制码
                                // 含义      取值       是否只到行尾
  USHORT   hit ;                // 加重      不小于零      N
  USHORT   hollow ;             // 空心标志   0,1          N
  USHORT   border ;             // 加框       0,1          Y
  USHORT   gild ;               // 虚体       0,1          N
  USHORT   updown;              // 上下标    无上下        Y
  USHORT   rotate;              // 旋转      上左右下      N
  USHORT   bevel ;              // 倾斜      中左右        N
  USHORT   upline;              // 上划线     0,1          Y
  USHORT   downline;            // 下划线     7种          Y
  USHORT   close;               // 上下对齐  下齐上齐      Y
  USHORT   position;            // 左右对齐  靠左居中靠右  Y
  USHORT   back;                // 背景       1-7          Y
  USHORT   shadow;              // 阴影       1-7          N
  USHORT   front;               // 前景       1-7          N
  SHORT    chupdown;            // 字符升降,用正数表示上升;

  SHORT    height , start ;     // 当前行高度 ,字符输出起始 ,向上为数字加大
  SHORT    width ;              // 当前行的宽度
  SHORT    lastch,lastx;

  SHORT    above,below,max;     // 仅供JudgeHeight访问用!!!!
} layout;

typedef struct _LINE {
   int leftch;
   UCHAR *text;
   layout lay;
   struct _LINE *next;
} LINE;

// 控制符分类,如果未定义则为修饰符
#define CFONT           0X9100
#define EFONT           0X9600
#define SIZE_0          0X9200
#define SIZE_1          0X9300
#define SIZE_2          0X9C00
#define SIZE_3          0X9D00
#define SIZE_4          0X9E00
#define SIZE_5          0X9F00
#define CHARDISTANCE    0X9900
#define LINEDISTANCE    0X9B00
#define LINEDISTANCE1   0X8800 // CCED 负行距
#define CHGOBACK        0X9700
#define CHGOUP          0X9800
#define CONTROLS        0X8900
// 各种修饰控制符定义
#define HIT             0X88FE
#define UNHIT           0X88FF
#define HOLLOW          0X9480
#define UNHOLLOW        0X9481
#define BORDER          0X9482
#define UNBORDER        0X9483
#define GILD            0X9484
#define UNGILD          0X9485
#define UPFOOT          0X9486
#define UNUPFT          0X9487
#define DOWNFOOT        0X9488
#define UNDOWNFT        0X9489
#define UNFOOT          UNDOWNFT
#define ROTATE_LEFT     0X948A
#define ROTATE_RIGHT    0X948B
#define ROTATE_DOWN     0X948C
#define UNROTATE        0X948D
#define ROTATE_UP       UNROTATE
#define BEVEL_LEFT      0X948E
#define BEVEL_RIGHT     0X948F
#define UNBEVEL         0X9490
#define BEVEL_MID       UNBEVEL
#define UPLINE          0X9491
#define UNUPLINE        0X9492

#define DOWNLINE_0      0X9493
#define DOWNLINE_1      0X9494
#define DOWNLINE_2      0X9495
#define DOWNLINE_3      0X9496
#define DOWNLINE_4      0X9497
#define DOWNLINE_5      0X9498
#define DOWNLINE_6      0X9499
#define UNDOWNLINE      0X949A

#define CLOSE_UP        0X949B
#define CLOSE_DOWN      0X949C
#define CLOSE_MID       0X949D
#define CLOSE_RIGHT     0X949E

#define BACK            0X9580
#define BACK_0          0X9580
#define BACK_1          0X9581
#define BACK_2          0X9582
#define BACK_3          0X9583
#define BACK_4          0X9584
#define BACK_5          0X9585
#define BACK_6          0X9586
#define UNBACK          0X9587

#define SHADOW          0X9588
#define SHADOW_0        0X9588
#define SHADOW_1        0X9589
#define SHADOW_2        0X958A
#define SHADOW_3        0X958B
#define SHADOW_4        0X958C
#define SHADOW_5        0X958D
#define SHADOW_6        0X958E
#define UNSHADOW        0X958F

#define FRONT           0X9590
#define FRONT_0         0X9590
#define FRONT_1         0X9591
#define FRONT_2         0X9592
#define FRONT_3         0X9593
#define FRONT_4         0X9594
#define FRONT_5         0X9595
#define FRONT_6         0X9596
#define UNFRONT         0X9597

#define HZTABLE         0XA900

#define SELFDEF         0X9000  //自定义控制字符,如果和已存在的控制字符
                                //冲突,就重新定义
#define FOUNDTABLE      (SELFDEF+0X0081)
#define TABLESPACE      (SELFDEF+0X0082)  // 将把随后的16BIT去除位15,位7后的
                                          //14 bits 作为表格空格个数

#define SIZEA5          (SIZE_0|0x80|10)  //标准5号字
#define A5DOT           24


#define NORMALSIZE      0
#define LONGSIZE        1
#define TABULARSIZE     2
#define SELFDEFSIZE     3
#define HEIGHTSIZE      NORMALSIZE

#define FILEIN          0
#define FILECOUT        1
#define FILETOUT        2
#define FILETOTAL       3

#define UP_FLAG      2
#define DOWN_FLAG    3
#define LEFT_FLAG    4
#define RIGHT_FLAG   5
#define MID_FLAG     6

#define CCED_SOFTCR     0x7f            // CCED 软回车
#define WPS_SOFTCR      0x8d            // WPS  软回车
#define CR              '\n'            // 硬回车
int SOFTCR;

#define FILEEND         0x1a            // 文本文件结束符
// #define FULLSPACE       "　"            // ZRM 的全角空格
// #define FULLSPACE1      "  "         // UCDOS 的全角空格,其实是两个半角空格
#define FULLSPACE       0xa1a1          // ZRM 的全角空格
#define SPACE           0x20            // ascii 空格
#define TABKEY          '\t'            // TAB键

#define MAXLENGTH       8192    //所处理的最大文本行长度;包括控制字符;


static USHORT efontCP=0xffff,sizeCP=0xffff,chardistanceCP=0xffff;
static USHORT csizexCP,csizeyCP,esizexCP,esizeyCP,chdistanceCP;
static char lastcharCP=0xff;

static USHORT fontsize[][16] = {
// 标准
{  90, 82, 72, 60, 48, 44, 40, 36, 32, 28, 24, 22, 20, 16, 14, 12, },
// 长型
{  72, 66, 58, 48, 38, 36, 32, 28, 26, 22, 20, 18, 16, 12, 12, 10, },
// 扁平
{ 112,102, 90, 76, 60, 56, 50, 46, 40, 36, 30, 28, 26, 20, 18, 16, },
// 自定义
{ 304,278,252,226,200,174,148,122,122,122,122,122,122,122,122,122, },
 } ;
/*  WPS 3.0F 与CCED略有出入
  96,80, 64, 56, 48, 44, 40, 40, 32, 28, 24, 22, 20, 18, 16, 14,
                             ^^^WPS3.0 无小3号字?
  72, 60, 48, 42, 36, 33, 30, 30, 24, 21, 18, 16, 15, 13, 12, 10,
 128,106, 85, 74, 64, 58, 53, 53, 42, 37, 32, 29, 26, 24, 21, 18,
 720,680,640,600,560,520,480,440
 以上取自 [[CCED 5.0 版速成实用指南]] P.192
*/
 static LINE *linehead;
 static UCHAR *lineinbuff;
 static UCHAR *lineoutbuff;
 static int outlength;// 为方便起见,将文件的输入输出操作改为先对缓冲区操作
                        // 只有OutTheChar函数对其写,OutBuffFlush将其清空,
                        // 必要时也可设计函数对其执行退格操作?
#ifdef TXT_DEBUG
 static FILE *tempfp;
#endif

 static layout lastlinelay; // Only by ReadLine access and must Initional !

static void FreeAllLine(void)
{
  LINE *t;
  while(linehead!=NULL)
  {
     t=linehead->next;
     free(linehead->text);
     free(linehead);
     linehead=t;
  }
}

// 判断 str 所指的汉字是否存在于 hz 所列出的汉字串中,
// 由于本函数使用频繁,故绝对要求 hz 串为偶数个字符!
static BOOL HZfound(UCHAR *s,UCHAR *hz)
{
   Wchar need=*(Wchar *)s;

   while(*hz)
   {
      if(need==*(Wchar *)hz)
         return TRUE;

      hz+=2;
   }

   return FALSE;
}

// 从当前位置跳到下一可显示字符
static UCHAR *NextChar(UCHAR *str)
{
   UCHAR code=*str;

   if(code>0xa0 || code==(TABLESPACE>>8))
      str+=2;
   else
   if(code<0x80 && code!=0)
      str++;

   while(1)
   {
      code=*str;
      if(code>=0xa1)
         break;
      else
      if(code>=0x80)
      {
         if(code==(TABLESPACE>>8))
             break;
         else
             str += 2;
      }
      else
         break;
   }

   return str;
}

// 过滤控制字符后的文本中从左起位置 len 的字符, 汉字算两个, 若此位置
// 是某汉字的中间或越过行尾, 返回空指针
static UCHAR *HZposi(UCHAR *s,int len)
{
  if(s==NULL||len<0)
    return NULL;

  while(1)
  {
    if(*s>=0x80&&*s<0xa1)
    {
        if(*s==(TABLESPACE>>8))
        {
           if(len>=2)
           {
              len-=2; s+=2;
           }
           else
           if(len==0)
              return s;
           else
              break;
        }
        else
           s+=2;
    }
    else
    if(len>0)
    {
        if(*s&0x80)
         { len--; s++; }

        if(*s==SOFTCR||*s==CR||*s==FILEEND||*s==0)
           break;

        len--; s++;
    }
    else
    if(len==0)
       return s;
    else
       break;
  }

  return NULL;
}

#ifdef NOT_USED
// 判断 hzch 是否在 str 串中, 对于汉字加以考虑
// 在, 返回指针, 不在, 返回 NULL
static UCHAR *hzstrchr( UCHAR * str , USHORT hzch )
{
  if ( hzch & 0x8080 ) // 汉字或控制符
  {
     while ( *str != 0 )
     {
        if ( (str[0]&0x80) && (str[1]&0x80) )
        {
           if ( hzch == *(USHORT *)str )
             return str ;
           else
             str += 2 ;
        }
        else
           str ++ ;
     }
     return NULL ;
  }
  else
     return  strchr(str,hzch);
}
#endif

static int OutTheChar(USHORT ch)
{
  if(outlength>MAXLENGTH)
    return -1;

  if(ch>0xff)
     { lineoutbuff[outlength++]=(ch>>8); ch&=0xff; }

  lineoutbuff[outlength++]=ch;
  return 0;
}

static int OutTheByte(UCHAR ch)
{
  if(outlength>MAXLENGTH)
     return -1;

  lineoutbuff[outlength++]=ch;
  return 0;
}

static int OutText(layout *lay,UCHAR *s,int length, int dist)
{
  // int flag=-1;
  // int state=0;
  USHORT ch;
  int len=length;

  if(len<0) len=MAXLENGTH;
  for(;;)
  {
      ch=*s++;
      if(ch&0x80) ch=(ch<<8)|*s++;
      if( ((ch<0x80||ch>0xa1a0)&&len<=0) || ch==0) break;

      if(ch<0x80)
         len--;
      else
      if (ch>0xa1a0)
         len-=2;

      if(ControlProcess(lay,ch)==TRUE)
         if(dist && OutTheChar(ch)!=0 )
            return -1;

      if(ch==SOFTCR||ch==CR||ch==FILEEND)
         break;
  } // end of for(;;)

  return 0;
}


// 检查表元的边界状况,并统计表元的高度和宽度;
// 如果正常,返回真,否则返回假(0);
static int CheckTableUnit(LINE *lkr,int *lineofunit,int *widthofunit)
{
   // int flag=-1;
   int width,height;
   int left;//,right; // 表元在文件中的绝对位置
   UCHAR *sl,*sr;
   LINE *lk;

   sl=HZposi(lkr->text,0);
   //check top
   if(!HZfound(sl,"┌┍┎┏├┝┞┟┠┡┢┣┬┭┮┯┰┱┲┳┼┽┾┿╀╁╂╃╄╅╆╇╈╉╊╋"))
   {
      // flag=-2; // 表格左上角字符错误;
      return 0;
   }

   width=2;
   sl=NextChar(sl);
   while(HZfound(sl,"─━┴┵┶┷┸┹┺┻"))
   {
      width+=2;
      sl=NextChar(sl);
   }

   if (!HZfound(sl,"┐┑┒┓┤┥┦┧┨┩┪┫┬┭┮┯┰┱┲┳┼┽┾┿╀╁╂╃╄╅╆╇╈╉╊╋"))
   {
      // flag=-3;//表示表格右上角字符错误
      return 0;
   }

   // 计算表元高度并且作简单检查
   *widthofunit=width+2;
   left = lkr->leftch;
   //right = left+width;
   height=1;
   lk=lkr;

   for(;;)
   {
     lk=lk->next;
     height++;
     if(lk->leftch>left)
        break; // 此时下边界已经处理过,认为合格,此处隐含着某种错误无法检测

     sl=HZposi(lk->text,left-lk->leftch);
     sr=HZposi(sl,width);
     if(HZfound(sl,"└┕┖┗├┝┞┟┠┡┢┣┴┵┶┷┸┹┺┻┼┽┾┿╀╁╂╃╄╅╆╇╈╉╊╋"))
     {
        if(HZfound(sr,"┘┙┚┛┤┥┦┧┨┩┪┫┴┵┶┷┸┹┺┻┼┽┾┿╀╁╂╃╄╅╆╇╈╉╊╋"))
        {
           int i;
           for(i=2;i<width;i+=2)
           {
             sl=NextChar(sl);
             if(!HZfound(sl,"─━┬┭┮┯┰┱┲┳"))
             {
               // flag=-4;//表示某一表格下边缘不正确;
               return 0;
             }
           }
           break;
        }
        else
        {
           // flag=-5;// 表示某表格的右边界未终止而左边界终止
           return 0;
        }
     }

     if(!HZfound(sl,"│┃┤┥┦┧┨┩┪┫"))
     {
         // flag=-6;// 表示某表格左边界不正常的结束;
         return 0;
     }

     if(!HZfound(sr,"│┃├┝┞┟┠┡┢┣"))
     {
         // flag=-7;// 表示某表格右边界不正常的结束;
         return 0;
     }
   } // end of for(;;)

   *lineofunit=height;
   return 1;
}


/*
 将缓冲区内的内容添加到 myform 结构中的CELL结构,这里CELL的存储空间要求已经给出
*/
static int OutBuffFlushToForm(PMYFORM pmyform)
{
   int v;
   PMYCELL pmycell;

   if(pmyform->iCellCount<=0)
      return -1; // 缺乏表元结构;
   if(outlength==0||lineoutbuff[0]==0)
      return 0; //不必费事了

   for(v=0;v<outlength;v++)
     if(lineoutbuff[v]==0)
        break;

   if(v<outlength)
     outlength=v;//如果能保证不含零,这里是多余的,但不会是错的

   pmycell=pmyform->pMyCell+(pmyform->iCellCount-1);
   if(pmycell->length==0)
      pmycell->text=(char*)malloc(outlength+1);
   else
      pmycell->text=(char*)realloc(pmycell->text,outlength+pmycell->length);

   if(pmycell->text==NULL)
      return -1;// Out of memery;

   if(pmycell->length==0)
   {
      memcpy(pmycell->text,lineoutbuff,outlength);
      pmycell->length=1;
   }
   else
      memcpy(pmycell->text+(pmycell->length-1),lineoutbuff,outlength);

   pmycell->length+=outlength;
   pmycell->text[pmycell->length-1]=0; // 人为的给加上零,便于使用strcpy;
   outlength=0; // clear the text fromto buff
   return 0;
}

#ifdef TXT_DEBUG
/* 仅用于测试,免得输出到文件后内存就没了; */
static int OutBuffNoFlush()
{
   int flag=0;

   if(outlength!=0)
      if(tempfp && fwrite(lineoutbuff,1,outlength,tempfp)!=outlength)
         flag=-1;

   fflush(tempfp);
   return flag;
}
#endif

static int OutBuffFlush(FILE *fp)
{
   int flag=0;

   if(outlength!=0)
   {
     if(fwrite(lineoutbuff,1,outlength,fp)!=outlength)
        flag=-1;
   // fflush(fp);
     outlength=0;
   }

   return flag;
}

// 换行时清除某些遇到行尾终止的信息
static void NewLineLayout(layout * lay)
{
   lay->border=FALSE;
   lay->updown=FALSE;
   lay->upline=FALSE;
   lay->downline=7;
   lay->close=DOWN_FLAG;
   lay->position=LEFT_FLAG;
   lay->back=7;
   lay->lastch=-1;
   lay->x=0;
   lay->height=0;
   lay->start=0;
   lay->width=0;
}

/*
  将表元内的文字处理好,输出到当前的 pmyforms链表所指向的尾部节点中;
  如果 fp 非空,则输出一部分信息到这个文件;
*/
static int TableUnitProcess(int line,LINE *lkr,int lineofunit,int widthofunit)
{
   int width;
   layout lastlay; // 假设每一个表元都从初始态开始,
   PMYCELL pmycell;

   InitLayout(&lastlay);

  #ifdef TXT_DEBUG
   if(tempfp!=NULL)
   {
   fprintf(tempfp,";表元编号(从1起) 表元坐标(纵向以行数计算,横向以半角字符个数计算)\n");
   fprintf(tempfp,"%d %d %d %d %d\n",tabcount,lkr->leftch,line-1,lkr->leftch+widthofunit-2,line-1+lineofunit-1);
   }
 #endif

   // 增加一个中间表元结构
   if(pmyforms->pmyform->iCellCount==0)
     pmycell=(PMYCELL)malloc(sizeof(MYCELL));
   else
     pmycell=(PMYCELL)realloc(pmyforms->pmyform->pMyCell,
                       (pmyforms->pmyform->iCellCount+1)*sizeof(MYCELL) );

   if(pmycell==NULL)
      return 0;
   pmyforms->pmyform->pMyCell=pmycell;
   pmyforms->pmyform->iCellCount++; // add cell count;
   //设置参数;
   pmycell=pmycell+(pmyforms->pmyform->iCellCount-1); //big pig  ^oo^
   pmycell->top=line-1;
   pmycell->left=lkr->leftch;
   pmycell->bottom=line-1+lineofunit-1;
   pmycell->right=lkr->leftch+widthofunit-2;
   pmycell->text=NULL;
   pmycell->length=0;

   width=widthofunit-2;

   OutText(&(lkr->lay),lkr->text,width,0);
   lkr->leftch+=width;
   lkr->text=HZposi(lkr->text,width); //调整表元顶行的状态
   outlength=0;   // clear buff
   while(lineofunit>2)//不处理最下面一行,同时最上面一行也处理完毕,所以是>2
   {
       int i,j;
       UCHAR *sr,*sl;

       lkr=lkr->next;
       lineofunit--;
       // 如下用于去除表格内某行文本的行尾空格,因为CCED在表格内有重定位功能
       // 变量 j 统计文本行中最后一个非空格字符所占的位置
       i=j=2;
       sl=sr=HZposi(lkr->text,2);
       while(i<width)
       {
          if((*sr&0x80)!=0)
          {
             i+=2;
             //if(HZfound(sr,FULLSPACE)==NULL)
             if( *(Wchar *)sr != FULLSPACE )
               j=i;
          }
          else
          {
             i++;
             if(sr[0]!=SPACE&&sr[0]!=TABKEY)
               j=i;
          }

          sr=NextChar(sr);
       } // end of for(;i<width;)

       OutText(&lkr->lay,lkr->text,2,0);//lkr->text指向表格线的左边,所以应跳过
       OutLayout(&lkr->lay,&lastlay);//输出状态变化;
       j-=2;
       OutText(&lkr->lay,sl,j,1);//输出本行的有效字符;

       // 表元内下一行行首的状态应该是本行末状态的换行;
       lastlay=lkr->lay;
       NewLineLayout(&lastlay);

     //if(lineofunit>2)
     //  OutTheChar(CR);//表元内部换行; 将表元内部换行清除,因为格式都不要了
   #ifdef TXT_DEBUG
       if(OutBuffNoFlush(tempfp))
          return 0;
   #endif

     // 输出到内存中,添加到myform结构里
       if(OutBuffFlushToForm(pmyforms->pmyform))
          return 0;

       OutText(&lkr->lay,lkr->text,width,0);//调整当前行的状态到右边界;
       lkr->leftch+=width;
       lkr->text=HZposi(lkr->text,width);
   }

   //写最后一个换行符到测试文件去,内存中不必
   OutTheChar(CR);

  #ifdef TXT_DEBUG
   if(OutBuffFlush(tempfp))
      return 0;
  #endif

   return 1;
}

// 根据字号得到全角字符宽度x,高度y ;
static int GetFontSize(USHORT size,USHORT *x,USHORT *y)
{
  *y = fontsize[HEIGHTSIZE][size&0xf] ;
  switch ( size & 0xfff0 )  {
   case  0x9280 : // 标准
                  * x = fontsize[NORMALSIZE][size&0xf] ;
                  break ;
   case  0x9290 : // 竖长
                  * x = fontsize[LONGSIZE][size&0xf] ;
                  break ;
   case  0x92a0 : // 扁平
                  * x = fontsize[TABULARSIZE][size&0xf] ;
                 break ;
   case  0x92b0 : // 自定义
                  * x = * y = fontsize[SELFDEFSIZE][size&0x7] ;
                 break ;
   default :
           if ( size & 0xff00 == 0x93 )     // 特大型
              *x = *y = ( ( size & 0x7f ) + 2 ) * 4 ;
           else
           if ( (size&0xfc80)==0x9c80 )
           { // 统一型
               int x1 = ( ( size & 0x0300 ) >> 5 ) | ( ( size & 0x70 ) >> 4 ) ;
               int y1 = size & 0x7 ;

               *x = x1 = x1*8+8 ;
               switch ( y1 ) {
                case 0 : *y = x1*1/3 ; break ;
                case 1 : *y = x1*1/2 ; break ;
                case 2 : *y = x1*2/3 ; break ;
                case 3 : *y = x1*3/4 ; break ;
                case 4 : *y = x1*4/3 ; break ;
                case 5 : *y = x1*3/2 ; break ;
                case 6 : *y = x1*2/1 ; break ;
                case 7 : *y = x1*3/1 ; break ;
                }
           }
           else
           {
              *x = *y = fontsize[NORMALSIZE][10] ;
              return -1 ; // 不是字号控制符
           }
  } /*- end of switch -*/

  return 0 ;
}

static int GetCHARDISTANCE(USHORT ctrl)
{
  if ((ctrl&0xff00)!=CHARDISTANCE) return 0;
  return (ctrl&0x7f)-63;
}

#ifdef NOT_USED
static int GetLINEDISTANCE(USHORT ctrl)
{
  if((ctrl&0xff00)==LINEDISTANCE)
    return ctrl&0x7f;
  if((ctrl&0xff00)==LINEDISTANCE1)
    return -((ctrl&0x7f)+1);
  return 0;
}
#endif

static int GetCHGOUPDOWN(USHORT ctrl)
{
   if(ctrl&0xff00!=CHGOUP)
     return 0;

   return (ctrl&0x7f)-63;
}

#ifdef NOT_USED
static int GetCHGOUP(USHORT ctrl)
{
  if ((ctrl&0xff00)!=CHGOUP) return 0;
  return (ctrl&0x7f)-63;
}
#endif

//处理控制字符,如果由于效率关系,比如汉字的字号大小,可以在layout内部设立变量解决
static int ControlProcess(layout *lay,USHORT ctrl)
{
  int status = TRUE;
  switch(ctrl&0xff00)
  {
     case CFONT :status=lay->cfont!=ctrl;lay->cfont=ctrl; break;
     case EFONT :status=lay->efont!=ctrl;lay->efont=ctrl; break;
     case SIZE_0:
     case SIZE_1:
     case SIZE_2:
     case SIZE_3:
     case SIZE_4:
     case SIZE_5:status=lay->size!=ctrl;
                 lay->size=ctrl;
                 break; //***在此处可计算汉字字号大小
     case CHARDISTANCE:
                 status=lay->chardistance!=ctrl;
                 lay->chardistance=ctrl;
                 break;
     case LINEDISTANCE1:
                 if(ctrl==HIT||ctrl==UNHIT)
                     goto OtherCase1;
     case LINEDISTANCE:
                 status=lay->linedistance!=ctrl;
                 lay->linedistance=ctrl;
                 break;
     //case CHGOBACK:
     //          lay->x=0;break; //****此处的X位置应该考虑一下
     case CHGOUP:lay->chupdown+=GetCHGOUPDOWN(ctrl);
                 lay->below=min(lay->below,lay->chupdown);
                 break;
                 //***如果设置字符升降后未输出字符算作无效,那么MIN应在下面
     default : goto OtherCase1;
  }
  return status;

 OtherCase1:
  status=TRUE;
  switch(ctrl&0xfff0){
        case BACK:status=lay->back!=ctrl&7;lay->back=ctrl&7; break;
        case SHADOW:lay->shadow=ctrl&7; break;
        case FRONT:lay->front=ctrl&7; break;
        default:goto OtherCase2;
  }
  return status;

 OtherCase2:
  switch(ctrl){
        case HIT:lay->hit++;break;
        case UNHIT:lay->hit=0;break;
        case HOLLOW:lay->hollow=TRUE;break;
        case UNHOLLOW:lay->hollow=FALSE;break;
        case BORDER:lay->border=TRUE;break;
        case UNBORDER:lay->border=FALSE;break;
        case GILD:lay->gild=TRUE;break;
        case UNGILD:lay->gild=FALSE;break;
        case UPFOOT:lay->updown=UP_FLAG;break;
        case DOWNFOOT:lay->updown=DOWN_FLAG;break;
        case UNUPFT:
        case UNDOWNFT:lay->updown=FALSE;break;
        case ROTATE_LEFT:lay->rotate=LEFT_FLAG;break;
        case ROTATE_RIGHT:lay->rotate=RIGHT_FLAG;break;
        case ROTATE_DOWN:lay->rotate=DOWN_FLAG;break;
        case UNROTATE:lay->rotate=UP_FLAG;break;
        case BEVEL_LEFT:lay->bevel=LEFT_FLAG;break;
        case BEVEL_RIGHT:lay->bevel=RIGHT_FLAG;break;
        case UNBEVEL:lay->bevel=MID_FLAG;break;
        case UPLINE:lay->upline=TRUE;break;
        case UNUPLINE:lay->upline=FALSE;break;

        case DOWNLINE_0:lay->downline=0;break;
        case DOWNLINE_1:lay->downline=1;break;
        case DOWNLINE_2:lay->downline=2;break;
        case DOWNLINE_3:lay->downline=3;break;
        case DOWNLINE_4:lay->downline=4;break;
        case DOWNLINE_5:lay->downline=5;break;
        case DOWNLINE_6:lay->downline=6;break;
        case UNDOWNLINE:lay->downline=7;break;

        case CLOSE_UP:lay->close=UP_FLAG;break;
        case CLOSE_DOWN:lay->close=DOWN_FLAG;break;
        case CLOSE_MID:lay->position=MID_FLAG;break;
        case CLOSE_RIGHT:lay->position=RIGHT_FLAG;break;
        default: goto OtherCase3;
  }
  return (TRUE);

 OtherCase3:
  if(ctrl>0xff&&ctrl<0xa1a1)
     return TRUE;//;其它控制字保留

  if(chardistanceCP!=lay->chardistance)
     chdistanceCP=GetCHARDISTANCE(chardistanceCP=lay->chardistance);

  if(ctrl<0x80)
  {
     if( lastcharCP!=ctrl // 也许某种字体是等宽的,可在这里判断
     || sizeCP!=lay->size
     || efontCP!=lay->efont)
        GetCharSize(lay->size,efontCP=lay->efont,ctrl,&esizexCP,&esizeyCP);

     if(lay->lastch!=-1)
        lay->lastch++;

     if(sizeCP!=lay->size)
        GetFontSize(sizeCP=lay->size,&csizexCP,&csizeyCP);

     lay->x +=(chdistanceCP+esizexCP);
  }
  else
  {
    if(lay->lastch!=-1)
       lay->lastch+=2;

    if(sizeCP!=lay->size)
       GetFontSize(sizeCP=lay->size,&csizexCP,&csizeyCP);

    if( (ctrl&0xff00) == HZTABLE )  //考虑重定位
    {
       if(lay->lastch!=-1)
          lay->x=lay->lastx+(lay->lastch*(csizexCP+chdistanceCP)/2);

       lay->lastx=lay->x;
       lay->lastch=0;
    }
    else
       lay->x +=(chdistanceCP+csizexCP);
  }

  if(lay->close==DOWN_FLAG) //  max 与 height, above 与 start 似乎可以合并!
     lay->above=max(lay->above,csizeyCP+lay->chupdown);
  else
     lay->max=max(lay->max,csizeyCP);

  return TRUE;
}


// 计算当前读入文本行的实际高度,宽度范围,
// 返回值为本行输出时实际占用的高度点阵数
// CCED中有一个奇怪的地方:[5]在[3][上齐]在[2][下齐]在
// 这一行的结果是不对的;
// 在此规定:每行的下齐线仅由字符上升控制影响,上齐线取决于文本的高度
static int JudgeHeight(layout *lay,UCHAR *str)
{
  USHORT ch;

  NewLineLayout(lay);
  lay->above=0;
  lay->max=0;
  lay->below=0;

  while((ch=*str) && ch!=SOFTCR && ch!=CR && ch!=FILEEND)
  {
     ch=*str++ ;
     if((ch&0x80)) ch=(ch<<8)|*str++;
     ControlProcess(lay,ch);
  }

  if( lay->max > lay->above - lay->below )
  {
     lay->below=lay->above-lay->max;
     lay->height=lay->max;
  }
  else
     lay->height=lay->above-lay->below;

  return 0;
}

// 取得指定的英文字母 ch 在字体 efont , 字号 size 下的宽和高
// 暂时用全角字符的一半代替, 关于不等宽英文字体的具体尺寸另行完善,但;
static int GetCharSize(USHORT size,USHORT efont,UCHAR ch,USHORT *esizex,USHORT *esizey)
{
   GetFontSize(size,esizex,esizey);
   if(efont!=EFONT+0)
     if(ch==SPACE)
       return 10;

   return (*esizex /=2);
}

// return :  0-表格未开始; 1-表格开始;
static int CheckTableBegin(UCHAR *s,int *left,int *right)
{
   int flag=0;
   int count=0;
   UCHAR ch;

   s=HZposi(s,*left);
   if(s==NULL)
      return FALSE;

   count=*left;
   while((ch=*s) && ch!=CR && ch!=SOFTCR && ch!=FILEEND)
   {
      if(ch<0x80) { flag=0; count++; s++; continue; }
      if(ch>0xa0)
      {
          if(flag==0 && HZfound(s,"┌┍┎┏"))
          {
              flag=1; // 表格左上角开始
              *left=count;
              count+=2;
              s+=2;
              continue;
          }

          if(flag==1)
          {
              if( HZfound(s,"┐┑┒┓"))
              {
                *right=count;
                count+=2;
                s+=2;
                return TRUE; //遇到表格右上角
              }

                 //当从表格左上角字符起不是连续横线时,重新判断;
              if(!HZfound(s,"─━┬┭┮┯┰┱┲┳"))
                 flag=0;
          }

          s+=2;
          count+=2;
      }
      else
      if(ch==(TABLESPACE>>8))
      {
          s+=2;
          count+=2;
      }
      else
          s+=2;
   } // end of while(  )

   return FALSE;
}



/* 申请一个MYFORM结构,并根据行数将整形数组的空间申请好,其余域初始化 numlines>0;
 不成功返回 NULL
*/
static PMYFORM AllocMyForm(int numLines)
{
    PMYFORM pMyFormRet;
    if(numLines<=0)
       return NULL;

    pMyFormRet=malloc(sizeof(MYFORM));
    if(pMyFormRet==NULL)
       return NULL;
    memset(pMyFormRet,0,sizeof(MYFORM));

    pMyFormRet->piHeight=malloc(numLines*sizeof(int));
    if(pMyFormRet->piHeight==NULL)
    {
       free(pMyFormRet);
       return NULL;
    }

    pMyFormRet->iLineCount=numLines;
    return pMyFormRet;
}

/* 撤消myform结构,包括此结构本身的存储空间的释放 */
static void FreeMyForm(PMYFORM pmyform)
{
    int i;
    if(pmyform==NULL)
       return;

    if(pmyform->piHeight!=NULL)
       free(pmyform->piHeight);

    if(pmyform->pMyCell!=NULL)
    {
      for(i=0;i<pmyform->iCellCount;i++)
         free(pmyform->pMyCell[i].text);

      free(pmyform->pMyCell);
    }

    free(pmyform);
}

#define TOHEADMODE      1001
#define TOTAILMODE      1002

// 输出表格从左到右的状态变化,在第一行注明存在一个表格,并且添加关于字符
// 空白的控制信息;
static int ClearTableChar(LINE **tablinkl,LINE **tablinkr,int width)
{
   LINE *head=linehead;
   UCHAR *sl,*sr;
   int istop=TRUE;
   int left ;
   int i,fillcount;
   LINE *lkl= *tablinkl;
   LINE *lkr= *tablinkr;

   fillcount=width; //清除表格后的填充字符个数,

           //  在第一行存在TABLEFOUND,所以从第二行起要加二
   left=lkr->next->leftch;
   for(;;)
   {
      lkl=lkl->next;
      lkr=lkr->next;
      if(lkl==NULL||lkr==NULL)
         return -2; // 错误;

      // 以下用于找到表格左侧的最后一个可见字符的右边界,从这里起的控制信息都可以
      // 被替换而不影响排版控制;
      outlength=0;       // clear the buffer
      if(istop==TRUE) //    在表格的第一行表明一个表格的开始;
      {
         OutTheChar(FOUNDTABLE);
         istop=FALSE;
      }
      else
         OutTheChar(TABLESPACE);

      for(i=0;i<fillcount;i+=2)
         OutTheChar(TABLESPACE);

      //取得表格右边界右边的第一个字符的排版状态;
      OutText(&(lkr->lay),lkr->text,left-lkr->leftch+2,0);
      //输出左右两侧状态差异到缓冲区;
      OutLayout(&lkr->lay,&lkl->lay);

      sl=lkl->text;
      sr=HZposi(lkr->text,left-lkr->leftch+2);
      //if(sr[0]==0xd && sr[1]==0xa && sr[2]==0) ;        // ByHance,97,8.5
      //else
      while(*sr)
         OutTheByte(*sr++);
      OutTheByte(0);

      //将删除表格后的文本与缓冲区的内容组成新的文本行,并更新原有的
      sl=HZposi(head->text,lkl->leftch);
      memcpy(sl,lineoutbuff,outlength);

      if(lkl==*tablinkl)
        break;
      head=head->next;
   }

   outlength=0;    // clear buff
   return 0;
}

static void FreeTabLinks(LINE **head)
{
   LINE *l;

   l=*head;
   if(l==NULL) return;

   while(*head!=(*head)->next)
   {
      l=(*head)->next;
      (*head)->next=l->next;
      free(l);
   }

   free(*head);
   *head=NULL;
}

/*
从当前的右边界中找到最左最上的一个的指针,并取得其所在本表中的行号,从1开始
  lkr :右边界链表的头指针的变量指针
  right 表格的最右边界
  linenum : none;
存在,返回真,否则返回假;
  lkr 得到所查找到的指针
  linenum 得到相对行号(从指针可以得到此行的总行号,而表格的起始总行号亦可得,)
*/
static int GetLeftTopTableUnit(LINE **lkr,int right,int *linenum)
{
   LINE *lk=(*lkr)->next;
   LINE *lklast=lk;
   int line=1;

   *linenum=1;
    //采用循环链表,并且*lkr指向的是链表的尾部,用来判断链表结束
   for(;lk->next!=*lkr;)
   {
     lk=lk->next; //从第二行起比较
     line ++;

     if(lk->leftch<right)
        if(lk->leftch<lklast->leftch)
        {
          lklast=lk;
          *linenum=line;
        }
   }

   *lkr=lklast;
   return (lklast->leftch+2<=right);
}

/*表格处理流程:
        1.判断表格的边界范围,其中左右是已经知道的了,确定上下范围,并且要建立
                表格各行的左边界状态
        1.1.下移一行,记录表格左边界的排版状态,一式两份;
        1.2.判断本行是否为表格结束,是转 1.3,否则转 1.1;
        1.3.记录顶行的表格左边界状态,返回;

        2.再从头开始,依次转换表元内容,从左到右,从上到下,记住表格边界由当前的
                竖线节点的位置决定,不由扫描到的字符决定;
        3.是否存在待处理表元?是则找到最左,最上的,转4;否则结束,转5;
        4.处理当前表元内容,调整相应状态;
        4.1.作表格边界检查,确定表元范围;
        4.2.处理表元的文本内容,其中底部仍用来留作下一个表元的查找标志,
                而其它则调整到当前表元的最右边界,作为右边表元的左边界;
        5.转换完毕,作替换工作,清楚原有的表格信息,0x87xx未使用,加以自定义?
  表格的每一行都用链表存储表格起始状态信息,用LINE结构,用到扩展的当前位置;
  然后从当前第一行开始依次判断表元的大小,并处理当前最高最左的表元,同时判
  断表格合法性;然后将表格消去,为省事,表格的初始状态链表应该保留一份;
        */
/*总体方案为转换后的每一行加上总高度,起始线高度,本行与上一行的行距,
        表格另加每行的高度列表,一个全角字宽,字间距,表元内容用所在的行偏移
        列偏移表示起始位置;文本的表示与普通文本一样;
  软回车用0X7F,例如下面表格,从左到右,从上到下,处理结果将是
                  ┏━━━━━┳━━━━━┓
                  ┃  1       ┃          ┃
                  ┣━━━━━┫    6     ┃
                  ┃  2       ┃          ┃
                  ┣━━━┳━┻━━━━━┫
                  ┃      ┃        4     ┃
                  ┃  3   ┣━━━━━━━┫
                  ┃      ┃        5     ┃
                  ┗━━━┻━━━━━━━┛
  */
static  int TableProcess(FILE *fp,int left,int right)
/*
  fp--相关的输入文件指针
  left,right--当前顶行在此两列范围内存在一个表格的开始;
  表格结果输出到 链表pmyforms中;
  成功完成,返回 0;
 */
{
   int flag=-1;
   int linecount,linenum,tableunitcount;
   int i;
   //int height,width;
   //UCHAR *sl,*sr;
   LINE *lk=linehead;
   LINE *tablinkr=NULL,*tablinkl=NULL;
   PMYFORM pmyform;
   PMYFORMS pmyformslast;
   PMYFORMS pmyform_s;
   //layout lastlay;

   linecount=TableProcessScale(fp,&tablinkl,&tablinkr,left,right);
   if(linecount<=1)
   {
      FreeTabLinks(&tablinkr);
      FreeTabLinks(&tablinkl);
      return 0;
   }

 #ifdef TXT_DEBUG
   if(tempfp!=NULL)
   {
      fprintf(tempfp,"t\n");
      fprintf(tempfp,";表格所占文本行数\n");
      fprintf(tempfp,"%d\n",linecount);
      fprintf(tempfp,";每行高度以及行间距\n");

      lk=linehead;
      for(i=0;i++<linecount;lk=lk->next)
       fprintf(tempfp,"%d %d ",lk->lay.height,GetLINEDISTANCE(lk->lay.linedistance));
      fprintf(tempfp,"\n");
   } // test
 #endif

   // 增加一个表格结构,
   pmyform=AllocMyForm(linecount);
   if(pmyform==NULL)
   {
      FreeTabLinks(&tablinkr);
      FreeTabLinks(&tablinkl);
      return -3;
   }

   //处理行间距的问题
   pmyform->widthofchar=A5DOT; //统一表格尺寸点阵
   lk=linehead;
   for(i=0;i<linecount;i++)
   {
      pmyform->piHeight[i]=A5DOT;//lk->lay.height;
      //lk=lk->next;
      //行间距不是不能考虑,只是没必要,且为了避免复杂
   }

   //加链表
   pmyformslast=pmyforms; //保存原来的尾结点,用于恢复(如有不测);
   pmyform_s=(PMYFORMS)malloc(sizeof(PMYFORMS));
   if(pmyform_s==NULL)
   {
      FreeMyForm(pmyform);
      return -1;
   }
   pmyform_s->pmyform=pmyform;

   if(pmyforms==NULL)
   {
      pmyform_s->next=pmyform_s;
      pmyforms=pmyform_s;
   }
   else
   {
      pmyform_s->next=pmyforms->next;
      pmyforms->next=pmyform_s;
      pmyforms=pmyform_s;
   }

   tableunitcount=1;
   while(1)
   {
      LINE *lkr;
      int lineofunit,widthofunit;

      lkr=tablinkr;
      if(!GetLeftTopTableUnit(&lkr,right,&linenum))
         break;// 处理完毕所有表元后结束循环
      if(!CheckTableUnit(lkr,&lineofunit,&widthofunit))
      {
         // 删除最后添加的中间表格结构
         PMYFORMS pmyform_s=pmyforms;
         PMYFORM pmyform=pmyforms->pmyform;
         if(pmyformslast!=NULL)
            pmyformslast->next=pmyforms->next;
         pmyforms=pmyformslast;
         FreeMyForm(pmyform);
         free(pmyform_s);
         flag=0;
         goto TableProcessQuit;
      }

      // 此时已经确定有一个表格是 从lkl->leftch起,
      //     高度为lineofunit, 宽度为widthofunit 的区域
      if(!TableUnitProcess(linenum,lkr,lineofunit,widthofunit))
      {
         flag=-1;
         goto TableProcessQuit;
      }
      tableunitcount++;
   }  /*- end of while -*/

   ClearTableChar(&tablinkl,&tablinkr,right-left);
   flag=0;

  TableProcessQuit: // 错误处理;
   FreeTabLinks(&tablinkr);
   FreeTabLinks(&tablinkl);
   return flag;
}

static int AppendTabLink(LINE  **head,int mode)
{
   LINE *t=(LINE *)malloc(sizeof(LINE));
   if(t==NULL) return -1;

   t->text=NULL;
   t->leftch=0;
   if(*head==NULL)
   {
     *head=t;
     t->next=t;
     return 0;
   }

   t->next=(*head)->next;
   (*head)->next=t;
   if(mode==TOTAILMODE)
      *head=t;
   return 0;
}

static int ReadLine(FILE *fp,LINE *cllk)
{
  LINE *lk;
  UCHAR ch;
  int inlength;

  if(feof(fp))
     return -2; // 文件已经结束,不必再读

  if((lk=(LINE*)malloc(sizeof(LINE)))==NULL)
     return -3;

  inlength=0; // clear input buffer
  for(;;)
  {
     ch=fgetc(fp);
     if(feof(fp))
         { ch=FILEEND; break; }
     if(ch==CR||ch==SOFTCR)
         break;
     lineinbuff[inlength++]=ch;
     if(inlength>=MAXLENGTH)
         { ch=SOFTCR; break;}
  }

  lineinbuff[inlength++]=ch;
  lineinbuff[inlength++]=0;
  if((lk->text=(UCHAR*)malloc(inlength))==NULL)
  {   // 无法申请到文本存储空间;
     free(lk);
     return -4;
  }

  memcpy(lk->text,lineinbuff,inlength);
  lk->next=NULL;
  if(cllk==NULL)
     linehead=lk;
  else
 #if 0
  if(cllk->next!=NULL) // Only test
  {
     // printf("ReadLine call error!");
     // exit(0);
  }
  else
 #endif
     cllk->next=lk;

  lastlinelay.lineno++;  // new line
  lk->lay=lastlinelay;
  JudgeHeight(&lastlinelay,lk->text);
  //CopyJudgeResult(&lk->lay,&lastlinelay ); // 在计算中有些结果本行需要预先知道!
   (lk->lay).position= lastlinelay.position;
   (lk->lay).height  = lastlinelay.height;
   (lk->lay).start   = lastlinelay.start;
   (lk->lay).width   = lastlinelay.width;

  return 0;
}

static int TableProcessScale(FILE *fpin,LINE**tablinkl,LINE**tablinkr,
                                    int left,int right)
{
   int flag=-1;
   int linecount = 1;
   UCHAR *sl,*sr;
   LINE *lk=linehead;

   // 查找表格的范围并且建立表格行的链表;
   for(;;)
   {
      if(lk->next==NULL)
        if((flag=ReadLine(fpin,lk))!=0)
           goto TableProcessScaleQuit ;

      lk=lk->next;
      sl=HZposi(lk->text,left);
      sr=HZposi(lk->text,right);

      if(AppendTabLink(tablinkr,TOTAILMODE))
         goto TableProcessScaleQuit;
      if(AppendTabLink(tablinkl,TOTAILMODE))
         goto TableProcessScaleQuit;

      linecount++;
      (*tablinkr)->lay=lk->lay;
      (*tablinkr)->text=sl;
      (*tablinkr)->leftch=left;
      OutText(&((*tablinkr)->lay),lk->text,left,0);

       // 构造完全一致的左边界,以后右边界将变化,左边界则记录原始状态;
      (*tablinkl)->lay=(*tablinkr)->lay;
      (*tablinkl)->text=sl;
      (*tablinkl)->leftch=left;

      if(HZfound(sl,"└┕┖┗"))
        if(HZfound(sr,"┘┙┚┛"))
           break;// 表格向下到头;

      if(!HZfound(sl,"│┃├┝┞┟┠┡┢┣"))
      {  flag=-2;// 表格左边界错误;
         goto TableProcessScaleQuit;
      }
      if(!HZfound(sr,"│┃┤┥┦┧┨┩┪┫"))
      {  flag=-3;// 表格右边界错误;
         goto TableProcessScaleQuit;
      }
   }// enf of for(;;);

   { // check bottom bold
    char * bottomstr;
    bottomstr=NextChar(sl);
    while(HZfound(bottomstr,"─━┴┵┶┷┸┹┺┻"))
       bottomstr=NextChar(bottomstr);

    if((void *)bottomstr!=(void *)sr)
       goto TableProcessScaleQuit;
   }

   // 把第一行加到链表中
   lk=linehead;
   sl=HZposi(lk->text,left);
    //sr=HZposi(lk->text,right);
   if(AppendTabLink(tablinkr,TOHEADMODE))
      goto TableProcessScaleQuit;
   if(AppendTabLink(tablinkl,TOHEADMODE))
      goto TableProcessScaleQuit;

   (*tablinkr)->next->lay=lk->lay;
   (*tablinkr)->next->text=sl;
   (*tablinkr)->next->leftch=left;
   OutText(&((*tablinkr)->lay),lk->text,left,0);

   lk=(*tablinkr)->next;
   (*tablinkl)->next->lay=lk->lay;
   (*tablinkl)->next->text=sl;
   (*tablinkl)->next->leftch=left;
   return linecount;

  TableProcessScaleQuit:
   FreeTabLinks(tablinkr);
   FreeTabLinks(tablinkl);
   *tablinkr=NULL;
   *tablinkl=NULL;
   return flag;
}


static int OutTheLine(FILE *fp,LINE **head)
{
  LINE *lk=*head;
  unsigned char *s=lk->text;
  // int flag;

  if(lk==NULL) return -1;
  outlength=0;  // clear buff;
  // output the text to buff;
  /* 检查这一行是否应该处理回车 */
  // 此行的硬回车应该删除,软回车当然无条件删除了
    /*  flag=1   此行的硬回车保留;
        flag=2   此行的硬回车删除;
        flag=3   此行的硬回车删除,因为这行存在表格;
    */
  if(s!=NULL)
  {
     while(*s)
     {
       #if 0
         flag=1;
         while(/**s!=CR &&*/ *s!=SOFTCR && *s!=FILEEND)
         {
            unsigned int ch=*s++;
            if(ch>=0x80)
               ch=(ch<<8)|*s++;

            switch(ch)
            {
                case FOUNDTABLE:
                case TABLESPACE: flag=3;break;
                case TABKEY:
                case SPACE:
                case 0xa9a9:// FULLSPACE,but low 8bit and high 8bit exchange
                    // 当遇到的都是空格时,需要把本行的回车去除,例如纯表格的某行
                      break;
                default:if(flag!=3)
                         if(ch<0x80||ch>0xa100)
                           flag=1;  //含其它字符,就保留这个硬回车
                      break;
            }
         } //end of while

         if(*s==CR)
         {
            if(flag==1) s++;
            else strcpy(s,s+1);
         }
         else
     #endif

         if(*s==SOFTCR && *(s+1)==CR)
            strcpy(s,s+1);//clear the CR;
         else
         // if(*s==FILEEND)
            s++;
     } //end of while
  }//end of if(s!=NULL)

  if(OutText(&lk->lay,lk->text,-1,1)!=0)
     return -1;

   //output from buff to file
  if(OutBuffFlush(fp)!=0)
     return -1;

  // free memory
  *head=lk->next;
  free(lk->text);
  free(lk);
  return 0;
}


static USHORT CreateCHGOUPDOWN(int dot)
{
   if(dot>64)
     dot=64;
   else
   if(dot<-63)
     dot=-63;

   return (CHGOUP|(dot+63));
}


static int OutLayout(layout *newlay,layout *oldlay)
{
  USHORT ch;

  if(newlay->cfont!=oldlay->cfont)
    OutTheChar(newlay->cfont);
  if(newlay->efont!=oldlay->efont)
    OutTheChar(newlay->efont);
  if(newlay->size!=oldlay->size)
    OutTheChar(newlay->size);
  if(newlay->chardistance!=oldlay->chardistance)
    OutTheChar(newlay->chardistance);
  if(newlay->linedistance!=oldlay->linedistance)
    OutTheChar(newlay->linedistance);

  if(newlay->hit!=oldlay->hit)
  {
    int count;
    if(newlay->hit<oldlay->hit)
       OutTheChar(UNHIT);
    else
       newlay->hit-=oldlay->hit;

    for(count=newlay->hit;count--;)
      OutTheChar(HIT);
  }

  if(newlay->hollow!=oldlay->hollow)
    OutTheChar(newlay->hollow==TRUE?HOLLOW:UNHOLLOW);
  if(newlay->border!=oldlay->border)
    OutTheChar(newlay->border==TRUE?BORDER:UNBORDER);
  if(newlay->gild!=oldlay->gild)
    OutTheChar(newlay->gild==TRUE?GILD:UNGILD);

  if(newlay->updown!=oldlay->updown)
  {
    switch(newlay->updown)
    {
      case FALSE:ch=UNFOOT;break;
      case DOWN_FLAG:ch=DOWNFOOT;break;
      case UP_FLAG:ch=UPFOOT;break;
    }
    OutTheChar(ch);
  }

  if(newlay->rotate!=oldlay->rotate)
  {
    switch(newlay->rotate)
    {
      case UP_FLAG:ch=UNROTATE;break;
      case DOWN_FLAG:ch=ROTATE_DOWN;break;
      case LEFT_FLAG:ch=ROTATE_LEFT;break;
      case RIGHT_FLAG:ch=ROTATE_RIGHT;break;
    }
    OutTheChar(ch);
  }

  if(newlay->bevel!=oldlay->bevel)
  {
    switch(newlay->bevel)
    {
      case MID_FLAG:ch=UNBEVEL;break;
      case LEFT_FLAG:ch=BEVEL_LEFT;break;
      case RIGHT_FLAG:ch=BEVEL_RIGHT;break;
    }
    OutTheChar(ch);
  }

  if(newlay->upline!=oldlay->upline)
    OutTheChar(newlay->upline==TRUE?UPLINE:UNUPLINE);
  if(newlay->downline!=oldlay->downline)
    OutTheChar((newlay->downline+1)%8+DOWNLINE_0);
  if(newlay->close!=oldlay->close)
    OutTheChar(newlay->close==UP_FLAG?CLOSE_UP:CLOSE_DOWN);

  if(newlay->position!=oldlay->position)
  {
    switch(newlay->position)
    {
      case MID_FLAG: OutTheChar(CLOSE_MID); break;
      case RIGHT_FLAG:OutTheChar(CLOSE_RIGHT); break;
    }
  }

  if(newlay->back!=oldlay->back)
    OutTheChar((newlay->back+1)%8+BACK_0);
  if(newlay->shadow!=oldlay->shadow)
    OutTheChar((newlay->shadow+1)%8+SHADOW_0);
  if(newlay->front!=oldlay->front)
    OutTheChar((newlay->front+1)%8+FRONT_0);

  if(newlay->chupdown!=oldlay->chupdown)
  {
    int t=oldlay->chupdown;
    int t1=newlay->chupdown;
    int updown;
    while(t1!=t)
    {
       if(t1>t)
       {
         if(t1>t+64)
          { updown=64; t+=64; }
         else
          { updown=t1-t; t=t1; }
       }
       else
      if(t1<t)
      {
         if(t1+63<t)
          { updown=-63; t-=63; }
         else
          { updown=t1-t; t=t1; }
       }
       else
         break;

       OutTheChar(CreateCHGOUPDOWN(updown));
    } // end of while
  }

  return 0;
}


 // initialize layout struct ;
static void InitLayout(layout *lay)
{
  memset(lay,0,sizeof(layout));
  lay->cfont = 0x9180 ; // 中文宋体
  lay->efont = 0x968a ; // 标准西体
  lay->size = 0x928a ;      // 标五
  lay->linedistance = 0x9b80 ; // 行距零
  lay->chardistance = 0x99bf ; // 字距零
  lay->rotate = UP_FLAG ;
  lay->bevel = MID_FLAG ;
  lay->close = DOWN_FLAG ;
  lay->position = LEFT_FLAG ;
  lay->downline =  lay->back =   lay->shadow =   lay->front = 7;
  lay->lastch=-1;
}

static void InitConvert(void)
{
   InitLayout(&lastlinelay);
   lineinbuff=lineoutbuff=NULL;
   linehead=NULL;
   outlength=0;// 为方便起见,将文件的输入输出操作改为先对缓冲区操作

   efontCP=sizeCP=chardistanceCP=0xffff;
   lastcharCP=0xff;
}

/*转换流程:
        1.判断当前第一行是否存在表格的第一行,不存在则执行 2,否则 3;
        1.1 如果当前文本不在内存,读入文本并计算出本行的高度值以及
                行尾排版状态变化情况,作为下一行的起始状态;
        1.2 如果当前文本在内存,表明状态已经定好,依次扫描各字符,
                发现┏.....┓就认为存在表格的第一行;
        2.处理这个表格,并清除表格所占的位置,转 1;
        2.1.根据起始位置扫描本行的表格位置,表格转换初始化;
        2.2.考察下一行是否为表格结束行,自行考虑是在内存还是读入;
        2.3.直到表格在内存,从头转换表元元素;
        2.4.依次消去各行的表格字符,控制号排版信息,返回;
                将表格处理完毕,并且消去表格的打印字符后,转 1;
        3.将当前顶行文本输出并且清除,转1;
另:pmyforms是表格输出的链表
*/
static int Txt2Ezp(FILE *fp, int FileType)
{
  int flag=-1;
  int left,right;
  FILE *fTmp;

  if((fTmp=fopen(CONVERT_TMP_FILE,"wb"))==NULL)
     return -1002;

  InitConvert();

 #ifdef TXT_DEBUG
  if((tempfp=fopen("formtest.out","wt"))==NULL)
   { flag=-1003; // 不能打开表格输出文件
     goto ConvertProcessQuit; }
 #endif

  if((lineinbuff=(UCHAR *)malloc(MAXLENGTH+16))==NULL)
  {  flag=-1004; // 无法申请行输入缓冲区!
     goto ConvertProcessQuit;
  }

  if(FileType==FT_WRITE)
  {
      long FileSize;

      fseek(fp,0xe,SEEK_SET);
      fread(&FileSize,1,sizeof(long),fp);
      FileSize -= 0x80;
      fseek(fp,0x80L,SEEK_SET);
      do {
         left=min(MAXLENGTH,FileSize);
         left=fread(lineinbuff,1,left,fp);
         left=fwrite(lineinbuff,1,left,fTmp);
         FileSize -= left;
      } while (left==MAXLENGTH);

      flag=0;
      goto ConvertProcessQuit;
  }
  // else
  if(FileType==FT_CCED)
      SOFTCR=CCED_SOFTCR;
  else
      SOFTCR=WPS_SOFTCR;


  if((lineoutbuff=(UCHAR *)malloc(MAXLENGTH+16))==NULL)
  {  flag=-1005; // 无法申请行输入缓冲区!
     goto ConvertProcessQuit;
  }

  for(;;)
  {
     if(linehead==NULL)
       if((flag=ReadLine(fp,NULL))!=0)
          goto ConvertProcessQuit; // 结束或出错;

     left=right=0;
     while(CheckTableBegin(linehead->text,&left,&right)==TRUE)
     {
        if(TableProcess(fp,left,right))
        {
          flag=-3;
          goto ConvertProcessQuit;
        }
        left=right;
     }

     if(OutTheLine(fTmp,&linehead)!=0)
        goto ConvertProcessQuit;
  }// enf of for(;;)

 ConvertProcessQuit:
  switch(flag)
  {
      case -2   : flag=0; break; // 让文本文件结束时返回错误信息 -2;
      case -3   :
      case -1003:
      case -1002:
      case -1001:
      case -1004:
      case -1005:
      default  : break;
  } // end of switch(flag);

  if(linehead!=NULL) FreeAllLine();
  if(lineoutbuff!=NULL) free(lineoutbuff);
  if(lineinbuff!=NULL) free(lineinbuff);

  if(fTmp!=NULL)
    fclose(fTmp);

 #ifdef TXT_DEBUG
  if(tempfp!=NULL) fclose(tempfp]);
 #endif

  return flag;
}

/* 将整数加入整数集合中; 成功返回0,否则返回-1;*/
static int AddItem(SetInt *set,int item)
{
   int i;
   int last=set->tail-1;

   if(set->tail<=0)     /*- 1st one -*/
   {
     set->tail=1;
     set->set[0]=item;
     return 0;
   }

   for(i=0;i<set->tail;i++)
   {
      if(set->set[i]==item) // already in set
         return 0;
      else
      if(set->set[i]>item) //将item插入这个位置,其后的元素顺移
         break;
   }

   if(set->tail>=MAXITEM)
      return -1;

   set->tail++;
   for(;last>=i;last--)
      set->set[last+1]=set->set[last];

   set->set[i]=item;
   return 0;
}

/* 根据元素的值返回索引号(从0起);  如果不存在返回-1; */
static int GetIndex(SetInt *set,int item)
{
   int i;

   for(i=0;i<set->tail;i++)
     if(set->set[i]==item)
        return i;

   return -1;
}

/* 根据索引返回元素值,从0起始,到元素个数减1;
 如果相应索引不存在,那么返回的值将无法判断,统一设为-1;
*/
static int GetValue(SetInt *set,int index)
{
   if(index>=set->tail)
     return -1;

   return set->set[index];
}

/* 将MYFORM中的表元结构排序,便于转换*/
//排序,要求将转化过程中取得的从左到右,从上到下的表元变为从上到下,从左到右
//采用枚举法;
static void SortMyCell(PMYFORM pmyform)
{
   int i,j,n,temp;

   n=pmyform->iCellCount ;
   for(i=0;i<n-1;i++)
   {
       temp=i ;
       for(j=i+1;j<n;j++)
       {
         if( pmyform->pMyCell[temp].top>pmyform->pMyCell[j].top)
          // 先从上到下
            temp = j;
         else
         if( pmyform->pMyCell[temp].top==pmyform->pMyCell[j].top
         &&  pmyform->pMyCell[temp].left>pmyform->pMyCell[j].left)
          // 高度相同,就从左到右;
            temp = j;
       }

       if(temp!=i)
       {
          MYCELL t = pmyform->pMyCell[temp];
          pmyform->pMyCell[temp]=pmyform->pMyCell[i];
          pmyform->pMyCell[i]=t;
       }
   }
}

/* 将表元位置由坐标形式转化为索引形式, 成功返回0;*/
static int ConvertMyCell(PMYFORM pmyform)
{
 int i;
 for(i=0;i<pmyform->iCellCount;i++)
 {
   if(AddItem(&pmyform->hori,pmyform->pMyCell[i].left)!=0)
      return -1;
   if(AddItem(&pmyform->hori,pmyform->pMyCell[i].right)!=0)
      return -1;
   if(AddItem(&pmyform->vert,pmyform->pMyCell[i].top)!=0)
      return -1;
   if(AddItem(&pmyform->vert,pmyform->pMyCell[i].bottom)!=0)
      return -1;
 }

 for(i=0;i<pmyform->iCellCount;i++)
 {
   pmyform->pMyCell[i].left=GetIndex(&pmyform->hori,pmyform->pMyCell[i].left);
   pmyform->pMyCell[i].right=GetIndex(&pmyform->hori,pmyform->pMyCell[i].right);
   pmyform->pMyCell[i].top=GetIndex(&pmyform->vert,pmyform->pMyCell[i].top);
   pmyform->pMyCell[i].bottom=GetIndex(&pmyform->vert,pmyform->pMyCell[i].bottom);
 }
 return 0;
}

 // 计算表元的长度,过滤控制字符以后的,用于字符空间的申请
static int mycelltextlen(PMYCELL pmycell)
{
   int i;
   unsigned char ch,*s=pmycell->text;

   if(s==NULL)
      return 0;

   for(i=0;i<pmycell->length;)
   {
      if((ch=*s++)==0)
         break;
      if(ch<0x80)//english char
      {
         i++;
      }
      else
      if(ch<0xa1)
      {          // control char
         if(*s++==0)
            break;
      }
      else
      {         // Chinese
         if(*s++==0)
           break;
         i+=2;
      }
   }// end of for

   return i;
}

 //拷贝表元的字符,过滤控制字符的,同时将原来未处理的TABKEY处理为一个空格,
static void copymycelltext(unsigned char *sdist, PMYCELL pmycell)
{
   int i;
   unsigned char *s=pmycell->text;

   if(s==NULL)
      return;

   for(i=0;i<pmycell->length;)
   {
      if(*s==0)
        break;

      if(*s<0x80)//english char
      {
        *sdist++=*s++;
        i++;
      }
      else
      if(*s<0xa1)//过滤
      {
        s++;
        if(*s==0)
          break;
        s++;
      }
      else
      {
        if(s[1]==0)
           break;
        *sdist++=*s++;
        *sdist++=*s++;
        i+=2;
      }
   }// end of for
}

/* 将中间格式的表格转化为EZP表格! 成功返回 0;*/
static int ConvertForm(PFormBoxs pForm, PMYFORM pmyform)
{
   int i,j,m,n,len;
   CELL *pcell;
   int *lens;
   int Totaltextlength;
   unsigned char ch,*str;
   Wchar *pTxt;

   SortMyCell(pmyform);
   ConvertMyCell(pmyform);

   // 将表元顺序转化好,填写EZP的结构;
   m=pmyform->vert.tail-1;
   n=pmyform->hori.tail-1;
   if(m>=MAXLINENUMBER||n>=MAXCLOUMNNUMBER||m<=0||n<=0)
      return -2; //**** 表格线的数目超出

   pForm->numLines=m;
   pForm->numCols=n;
   pForm->hCellTable=HandleAlloc(m*n*sizeof(CELL),0);
   pcell=(CELL*)HandleLock(pForm->hCellTable);
   if(pcell==NULL)
   {
     HandleFree(pForm->hCellTable);
     return -1;
   }

   for(i=0;i<m;i++)
    for(j=0;j<n;j++)
    {
       int t=i*n+j;
       // pcell[t].hParentBox=hFormBox;
       pcell[t].numLines=pcell[t].numCols=1;
       pcell[t].iFirst=FIRSTCELL;
       pcell[t].iSelf=t;
       pcell[t].bSlip=0;
    }

   pForm->hortlineType[0]=pForm->hortlineType[m+1]=
    pForm->vertlineType[0]=pForm->vertlineType[n+1]=LINE_BOLD;

 // 此时根据pmyform给出的每一行的高度,计算M*N个方格中每一行,列的点数
   pForm->vertline[0]=0;
   for(i=0;i<m;i++)
   {
      int top,bottom;

      top=GetValue(&pmyform->vert,i);
      bottom=GetValue(&pmyform->vert,i+1);
      pForm->hortline[i+1]=0;
      for(j=top+1;j<bottom;j++)
         pForm->hortline[i+1] += pmyform->piHeight[j];

      pForm->hortline[i+1] +=
             (pmyform->piHeight[top]+pmyform->piHeight[bottom])/2;
      pForm->hortline[i+1] = (float)pForm->hortline[i+1]*SCALEMETER/72/24*10.5;
   }
   for(i=0;i<m;i++)
      pForm->hortline[i+1]+=pForm->hortline[i];//变相对为绝对

   // 每一列,由于每列上的字符是等宽的,所以简单些
   pForm->hortline[0]=0;
   for(i=0;i<n;i++)
   {
      int left,right;
      left=GetValue(&pmyform->hori,i);
      right=GetValue(&pmyform->hori,i+1);
      pForm->vertline[i+1]=(right-left)*pmyform->widthofchar/2;
      pForm->vertline[i+1]=(float)pForm->vertline[i+1]*SCALEMETER/72/24*10.5;
   }
   for(i=0;i<n;i++)
      pForm->vertline[i+1]+=pForm->vertline[i];//变相对为绝对

   //2 CELL参数填写
   for(i=0;i<pmyform->iCellCount;i++)
   {
      PMYCELL pmycell=pmyform->pMyCell+i;
      int k;
      int ifirst=pmycell->top*n+pmycell->left;

      for(j=pmycell->top;j<pmycell->bottom;j++)
         for(k=pmycell->left;k<pmycell->right;k++)
         {
            int t=j*n+k;
            if(t==ifirst)
            {
               pcell[t].numLines=pmycell->bottom-pmycell->top;
               pcell[t].numCols=pmycell->right-pmycell->left;
            }
            else
               pcell[t].iFirst=ifirst;
         }
   }

   HandleUnlock(pForm->hCellTable);//此后pcell不用了;

   //3 文本格式转换,由于实用考虑,表元内部的控制字不必要,
   lens=(int*)malloc(sizeof(int)*pmyform->iCellCount);
   if(lens==NULL)
   {
     lbl_err:
       HandleUnlock(pForm->hCellTable);
       HandleFree(pForm->hCellTable);
       HandleUnlock(hFormBox);
       return -1;
   }

   Totaltextlength=0;
   for(i=0;i<pmyform->iCellCount;i++)
   {
      lens[i]=mycelltextlen(pmyform->pMyCell+i);
      Totaltextlength+=lens[i];
   }
   Totaltextlength+=m*n;  // m*n-1 个 TABKEY, 1 '\0'

    //将所有的pmycell的文本转换为EZP格式,用'\t'联接好,申请到空间并填写
    //pForm的TextHandle字段
   str=malloc(Totaltextlength);
   if(str==NULL)
   {
    lbl_err2:
      free(lens);
      goto lbl_err;
   }

   memset(str,TABKEY,Totaltextlength-1);
   len=0;
   for(i=0;i<pmyform->iCellCount;i++)
   {
     copymycelltext(str+len+(pmyform->pMyCell[i].left+pmyform->pMyCell[i].top*n),
          pmyform->pMyCell+i);
     len+=lens[i];
   }
   str[Totaltextlength-1]=0;

   /*-- change char to Wchar --*/
   i=0;
   Totaltextlength=1;
   while((ch=str[i]))
   {
      if(ch<0x80) i++;
      else i+=2;

      Totaltextlength++;
   }

   pForm->TextLength=Totaltextlength;
   pForm->TextHandle=HandleAlloc(sizeof(Wchar)*Totaltextlength,0);
   pTxt=(Wchar *)HandleLock(pForm->TextHandle);
   if(pTxt==NULL)
   {
      free(str);
      goto lbl_err2;
   }

   i=0;
   while((ch=str[i]))
   {
      if(ch<0x80) { *pTxt=ch; i++; }
      else { *pTxt=((Wchar)ch<<8)|str[i+1]; i+=2; }

      pTxt++;
   }
   *pTxt=0;

   HandleUnlock(pForm->TextHandle);

   free(str);
   free(lens);
   HandleUnlock(pForm->hCellTable);
   HandleUnlock(hFormBox);
   return 0;
}


/* 释放所有的中间格式表格myform,这些表格存储于全局变量 pmyforms 所构成的链表中*/
static void FreeAllMyforms()
{
   if (pmyforms==NULL)
      return ;

   for(;pmyforms->next!=pmyforms;)
   {
      PMYFORMS t=pmyforms->next;
      pmyforms->next=t->next;
      FreeMyForm(t->pmyform);
      free(t);
   }

   FreeMyForm(pmyforms->pmyform);
   free(pmyforms);
   pmyforms=NULL;
}

static int GetFileType(FILE *TextFp)
{
  int i,WpsVer;
  unsigned char MidTextBlock[0x400],ch;
  long FileSize;

  i=fread((char *)MidTextBlock,1,0x400,TextFp);
  if(*(unsigned short *)MidTextBlock>=0xff00 && i>0x300)
  {
      WpsVer=MidTextBlock[0]+2;
      if(WpsVer==2)            /* ver 2.0 */
         fseek(TextFp,0x300L,SEEK_SET);

      return FT_WPS+WpsVer;
  }
  else
  if(i>=0x80 && !memcmp(MidTextBlock,"\x31\xbe\x0\x0\x0\xab\x0\x0",8) )
  {     /*---------- it is a Window_Write file ------------*/
      fseek(TextFp,0x80L,SEEK_SET);
      // FileSize=(*(long *)((long)MidTextBlock+0xe)) - 0x80;
      return FT_WRITE;
  }
  else
  {
      fseek(TextFp,0,SEEK_END);
      FileSize=ftell(TextFp);
      if(FileSize>128)
      {
          fseek(TextFp,-129,SEEK_END);
          ch=fgetc(TextFp);
          if(ch==0x1a)
          {
             fseek(TextFp,0,SEEK_SET);
             return FT_CCED;
          }
      }
  }

  fseek(TextFp,0,SEEK_SET);
  return FT_TXT;
}

/*
 将指定的文本文件中的表格转化为轻松排版的格式
  对于表格,顺序存储于链表pformlink中; 文本存储于textfile中;
  结果正确,将返回0;
  错误结果一律为 -1,具体的错误处理需要更改源程序
*/
int TextBoxInsertTextFile(char *FileName,HBOX HBox,int *NewHBox,int Position,
                          int *NewPosition,int *BlockStart,int *BlockEnd)
{
  FILE *TextFp;
  int  FileType;
  int SaveUndoNumber,Result;
  FormBoxs *MidBox;
  HFormBoxs hForm;
  Pages *MidPage;
  int Left,x,y;
  int PageWidth,FormBoxHeight;
  PMYFORMS pmyformend;
  PMYFORMS pmyformcur;
  FILE *fTmp;
  int fHasForm=0;

  if ((TextFp=fopen(FileName,"rb"))==NULL)
     return(FILEOPEN);

  FileType=GetFileType(TextFp);
  Result=Txt2Ezp(TextFp,FileType);
  fclose(TextFp);

  if(Result)
     goto lbl_exit;

  if((fTmp=fopen(CONVERT_TMP_FILE,"rb"))==NULL)
     goto lbl_exit;

  SaveUndoNumber=UndoOperateSum;

   UndoInsertCursorGoto(Position);

   pmyformend=pmyforms;
   if(pmyforms)
   {
      fHasForm=1;
      pmyformcur=pmyforms;
      MidBox=(FormBoxs *)&TmpBuf;
      memset(MidBox,0,sizeof(FormBoxs));
      MidBox->TextDistantLeft=DEFAULTBOXTABLEDISTANT;
      MidBox->TextDistantTop =DEFAULTBOXTABLEDISTANT;
      MidBox->TextDistantRight=DEFAULTBOXTABLEDISTANT;
      MidBox->TextDistantBottom=DEFAULTBOXTABLEDISTANT;
      TableBoxSetBoxType(MidBox,TABLEBOX);

      MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
      Left=PageGetMarginLeft(MidPage);
      PageWidth=PageGetPageWidth(MidPage)-Left-PageGetMarginRight(MidPage);
      HandleUnlock(ItemGetHandle(GlobalCurrentPage));
   }

   if (*BlockStart<*BlockEnd)
      CancelBlock(HBox,BlockStart,BlockEnd);

   *NewHBox=HBox;
   *NewPosition=Position;

   while( InsertTxt2Box(fTmp,HBox,NewHBox, Position,NewPosition, &x,&y) )
   {               // 0=End of file, 1=insert form box
      HBox=*NewHBox;
      Position=*NewPosition;

      if(fHasForm)
      {
       /* 将链表 pmyforms 中的表格转换为 EZP 表格 */
         pmyformcur=pmyformcur->next;

         if( ConvertForm(MidBox,pmyformcur->pmyform) ==0 )
         {
            FormBoxHeight=MidBox->hortline[MidBox->numLines];
              //FormBoxHeight=5000;
            TextBoxSetBoxLeft(MidBox,Left);
            TextBoxSetBoxTop(MidBox,y);
            TextBoxSetBoxWidth(MidBox,PageWidth);
            TextBoxSetBoxHeight(MidBox,FormBoxHeight+DEFAULTCHARSIZE);

            // hForm=TableBoxInsert(MidBox,GlobalCurrentPage);
            // hForm=TableBoxInsert(MidBox,PageHandleToNumber(ItemGetFather(HBox)) );
            hForm=TableBoxInsert(MidBox,ItemGetFather(HBox));
            FBPlusVertLine(hForm,0,21*SCALEMETER/72/2);  // size5=10.5 P
            FBPlusHoriLine(hForm,0,DEFAULTCHARSIZE/2);
            ReFormatTableText(hForm,TRUE);
            BoxChange(GlobalBoxHeadHandle,GlobalCurrentPage);
         }

         if (pmyformcur==pmyformend)
            fHasForm=0;
      }
   } /*- end of while -*/

  fclose(fTmp);

  UndoInsertCompose(UndoOperateSum-SaveUndoNumber);
  RedrawUserField();
  Result=0;

 lbl_exit:
  FreeAllMyforms();
  return(Result);
}

static int InsertTxt2Box(FILE *TextFp, HBOX HBox,int *NewHBox,int Position,
                          int *NewPosition, int *pX, int *pY)
{
  int i,j,Result,InsertLen;
  int StartChangeLine,ChangeLines;
  // int CursorX,CursorY;
   #define  MaxReadLen     0x4000       // can't be too large, for stack
  Wchar MidTextBlock[MaxReadLen],ReadChar;
  int MidReadChar;
  int font;
  USHORT sizex,sizey;// x:水平方向,y:垂直方向
  PTextBoxs pTextBox;
  Pmark_rec eptr;


 #define FILE_END         -1
 #define INS_FORM_BOX      1
 #define CHG_RIGHT         2
 #define CHG_MIDDLE        3
 #define CHG_SIZE          4
 #define CHG_EFONT         5
 #define CHG_CFONT         6

 lbl_again:
  InsertLen=Result=0;
  while(Result==0)
  {
       for (j=0; j<MaxReadLen;)
       {
           MidReadChar=fgetc(TextFp);
           if (MidReadChar==EOF || MidReadChar==0x1a)
           {
            lbl_eof:
              Result=FILE_END;
              break;
           }

           if( (MidReadChar<0x20 && MidReadChar!=ENTER)
           || MidReadChar==0x7f
           || MidReadChar==0x8d    // Chinese (WordStar) CR
           || MidReadChar==0x8a    // Chinese (WordStar) LF
           )  continue;

           if(MidReadChar<0x80||MidReadChar>0xa0)
           {
                ReadChar=MidReadChar;
                if (ReadChar>0xa0)
                {
                   MidReadChar=fgetc(TextFp);
                   if (MidReadChar==EOF || MidReadChar==0x1a)
                      goto lbl_eof;

                   if (MidReadChar<=0xa0)
                      continue;

                   ReadChar<<=8;
                   ReadChar|=MidReadChar;
                }

                MidTextBlock[j++]=ReadChar;
           }
           else
           {
                ReadChar = MidReadChar<<8;
                ReadChar |= fgetc(TextFp);

                switch(ReadChar&0xff00)
                {
                  case CFONT :
                        font=ReadChar&0x7f;
                        Result=CHG_CFONT;
                        break;
                  case EFONT :
                        font=(ReadChar&0x7f)+1024;
                        Result=CHG_EFONT;
                        break;
                  case SIZE_0:
                  case SIZE_1:
                  case SIZE_2:
                  case SIZE_3:
                  case SIZE_4:
                  case SIZE_5:
                        GetFontSize(ReadChar,&sizex,&sizey);
                        sizex=(float)sizex*SCALEMETER/72/24*10.5;
                        sizey=(float)sizey*SCALEMETER/72/24*10.5;
                        Result=CHG_SIZE;
                        break;

                default:
                  switch(ReadChar)
                  {
                     case CLOSE_MID:
                           Result=CHG_MIDDLE;
                           break;
                     case CLOSE_RIGHT:
                           Result=CHG_RIGHT;
                           break;
                     case FOUNDTABLE:
                           Result=INS_FORM_BOX;
                           break;
                     case TABLESPACE:
                           break;
                     default: break;
                  }
                }  /*- end of switch -*/

                if(Result)
                   break;
           }
       }  /*- for -*/

       if(j)
       {
           i=TextBoxInsertString(HBox,Position+InsertLen,MidTextBlock,j);
           InsertLen+=i;
       }
  } /*- end of while -*/

// lbl_end:
  FormatInsertText(HBox,Position,InsertLen,&StartChangeLine,&ChangeLines,FALSE);
  *NewPosition=Position+InsertLen;
  CursorLocate(HBox,NewHBox,*NewPosition,pX,pY);

  switch(Result)
  {
   case FILE_END:
     Result=0;
     break;
   case INS_FORM_BOX:
     pTextBox=HandleLock(ItemGetHandle(*NewHBox));
     eptr=LocateMarkbyPos(pTextBox,*NewPosition);
     if(eptr==NULL)
        *pY += pTextBox->BoxTop;
     else
     {
        // *pY = eptr->line_height + eptr->y;
        if(eptr->type==E_END)
            eptr=eptr->prev;
        *pY = eptr->y + pTextBox->BoxTop + 20;
     }
     HandleUnlock(ItemGetHandle(HBox));
     break;
   case CHG_RIGHT :
     Result=ALIGNRIGHT;
  lbl_align:
     GlobalNotDisplay=1;
     i=TextSearchAttribute(GlobalBoxHeadHandle,
                       *NewPosition,PARAGRAPHALIGN,&j)&0xf0;
     TextChangeAttribute(*NewHBox,*NewPosition,
                       0,
                       PARAGRAPHALIGN,i|Result, &GlobalTextPosition,
                       &GlobalTextBlockStart,&GlobalTextBlockEnd);
     GlobalNotDisplay=0;
     break;
   case CHG_MIDDLE:
     Result=ALIGNCENTRE;
     goto lbl_align;
     // break;
   case CHG_SIZE :
     GlobalNotDisplay=1;
     TextChangeAttribute(*NewHBox,*NewPosition,
                       0,
                       CHARSIZE,sizey, &GlobalTextPosition,
                       &GlobalTextBlockStart,&GlobalTextBlockEnd);
     *NewPosition=GlobalTextPosition;
     TextChangeAttribute(*NewHBox,*NewPosition,
                       0,
                       CHARHSIZE,sizex, &GlobalTextPosition,
                       &GlobalTextBlockStart,&GlobalTextBlockEnd);
     GlobalNotDisplay=0;
     break;
   case CHG_EFONT :
   case CHG_CFONT :
     GlobalNotDisplay=1;
     TextChangeAttribute(*NewHBox,*NewPosition,
                       0,
                       CHARFONT, font, &GlobalTextPosition,
                       &GlobalTextBlockStart,&GlobalTextBlockEnd);
     GlobalNotDisplay=0;
     break;
  } /*- end of switch -*/

  if(Result>1)
  {
     Position=*NewPosition=GlobalTextPosition;
     HBox=*NewHBox;
     goto lbl_again;
  }
  else
  return Result;
}

