/*-------------------------------------------------------------------
* Name: express.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#define ERR_EXP   1
#define CALC_ERR  1000
#define MATH_ERR  2000
#define PARA_ERR  3000
#define NULL_EXP  2
#define END_EXP   6000
#define LEFT_ERR  4000
#define RIGHT_ERR 5000
#define MATH_NAME_ERR 3
#define ERR_STR   4
#define ERR_VAR   5

#define DELIMITER 1
#define VARIABLE  2
#define NUMBER    3
#define MATH      4
#define QUOTE     5
#define FINISHED  6

#define SQRT      0
#define SQR       1
#define SIN       2
#define GSIN      3
#define COS       4
#define LOG       5
#define POW       6
#define GCOS      7
#define TG        8
#define GTG       9
#define ASIN      10
#define AGSIN     11
#define ACOS      12
#define AGCOS     13
#define CTG       14
#define GCTG      15
#define ACTG      16
#define AGCTG     17
#define MIN       18
#define MAX       19
#define ATG       20
#define AGTG      21
#define LN        22
#define LG        23
#define EXP       24

static char math_str[][7]={"sqrt","sqr","sin","gsin","cos","log","pow",
                     "gcos","tg","gtg","asin","agsin","acos","agcos",
                     "ctg","gctg","actg","agctg","min","max","atg",
                     "agtg","ln","lg","exp"
};
#define MATHNUM  (sizeof(math_str)/sizeof(math_str[0]))
static char math_pri[]={      1   ,  1  ,  1  ,  1   ,  1  ,  2  ,  2 ,
                       1   ,  1 ,  1  ,   1  ,  1   ,   1  ,   1   ,
                       1   ,  1  ,   1  ,   1   ,  2  ,  2,    1 ,
                       1   ,  1  , 1 ,  1
 };

static double var_low[26];
// static double var_upr[26];
#define EXPBUFFLEN 400
static char express_buff[EXPBUFFLEN];
static char *prog,token[40];
static short token_type,tok;

static int get_token(void);
static int level2(double *val);
static int level3(double *val);
static int level4(double *val);
static int level5(double *val);
static int level6(double *val);
static short get_math(char *val);
static double get_var(char *val);
double get_num(char *val);

#ifdef NOT_USED
static void errorlist(int errorcode)
{
   int hi,lo;
   char errorstr[100];
   if (errorcode==ERR_STR&&*prog==0) errorcode=END_EXP;
   hi=errorcode/1000;
   lo=errorcode%1000;
   switch (hi)
    {
      case 0:
                switch (lo)
                {
                 case ERR_EXP:
                         sprintf(errorstr,"表达式错误: %s",prog);
                         break;
                 case NULL_EXP:
                         sprintf(errorstr,"表达式为空白 !");
                         break;
                 case MATH_NAME_ERR:
                         sprintf(errorstr,"非法的数学函数:%s",token);
                         break;
         case ERR_STR:
                         sprintf(errorstr,"表达式中出现非法字符:'%s'",token);
                         break;
         case ERR_VAR:
             sprintf(errorstr,"表达式中变量超界:'%s'",token);
             break;
                 default:
                         sprintf(errorstr,"表达式错 !");
                         break;
                }
                break;
      case 2:
                sprintf(errorstr,"函数'%s'的值超界",math_str[lo]);
                break;
      case 1:
                sprintf(errorstr,"'%c'计算错误",lo);
                break;
      case 3:
                sprintf(errorstr,"函数'%s'参数错误",math_str[lo]);
                break;
      case 4:
                sprintf(errorstr,"表达式中缺少 '%c' ",lo);
                break;
      case 5:
                sprintf(errorstr,"表达式中缺少 '%c' in express",lo);
                break;
      case 6:
                sprintf(errorstr,"表达式非正常结束");
                break;
      default:
                sprintf(errorstr,"表达式错误");
                break;
    }
    printf("\n");
    printf(errorstr);
}
#endif

static short exp_err;
static int arith(char op,double *r,double *h)
{
   exp_err=0;
   switch (op)
    {
      case '+': *r=*r+(*h); break;
      case '-': *r=*r-(*h); break;
      case '*': *r=*r*(*h); break;
      case '/':
                if (*h==0.0f)
                {
                exp_err=1;
                break;
                }
                *r=*r/(*h); break;
      case '%':
                if (*h==0.0f)
                {
                exp_err=1;
                break;
                }
                *r=fmod(*r,*h); break;
      case '^': *r=pow(*r,*h); break;
    }
   if (exp_err) return CALC_ERR+op;
   return 0;
}

#ifdef NOT_USED
int matherr(struct exception *e)
{
   e->retval=0.0;
   exp_err=1;
   return 1;
}
#endif

static int math_math(short op,double *r,double *h)
{
   exp_err=0;
   switch (op)
    {
       case SQRT:       *r=sqrt(*h);
                        break;
       case SQR:        *r=(*h*(*h));
                        break;
       case GSIN:       *r=sin(*h);
                        break;
       case SIN:        *r=sin((*h)*3.1415926/180.0);
                        break;
       case GCOS:       *r=cos(*h);
                        break;
       case COS:        *r=cos((*h)*3.1415926/180.0);
                        break;
       case GTG:        *r=tan(*h);
                        break;
       case TG:         *r=tan((*h)*3.1415926/180.0);
                        break;
       case GCTG:       *r=1/tan(*h);
                        break;
       case CTG:        *r=1/tan((*h)*3.14159265/180.0);
                        break;
       case ASIN:
                        if((*h)>1) *r=0;
                        else
                           *r=asin(*h)*180.0/3.14159265;
                        break;
       case AGSIN:
                        if((*h)>1) *r=0;
                        else
                           *r=asin(*h);
                        break;
       case ACOS:
                        if((*h)>1) *r=0;
                        else
                           *r=acos(*h)*180.0/3.14159265;
                        break;
       case AGCOS:
                        if((*h)>1) *r=0;
                        else
                            *r=acos(*h);
                        break;
       case ATG:        *r=atan(*h)*180.0/3.14159265;
                        break;
       case AGTG:       *r=atan(*h);
                        break;
       case ACTG:       *r=atan(1/(*h))*180.0/3.14159265;
                        break;
       case AGCTG:      *r=atan(1/(*h));
                        break;
       case POW:
                        *r=pow(h[0],h[1]);
                        break;
       case MIN:        if (h[0]>h[1]) *r=h[1];
                        else *r=h[0];
                        break;
       case MAX:        if (h[0]>h[1]) *r=h[0];
                        else *r=h[1];
                        break;
       case LOG:        *r=log(h[1])/log(h[0]);
                        break;
       case LG:         *r=log(h[0])/log(10);
                        break;
       case LN:         *r=log(h[0]);
                        break;
       case EXP:        *r=exp(*h);
    }
    if (exp_err) return op+MATH_ERR;
    return 0;
}

static int primitive(double *result)
{
   short math_op,i;
   char op;
   double hold[10];
   int ret;
   switch (token_type)
    {
      case NUMBER:      *result=get_num(token);
                        return get_token();
      case VARIABLE:    *result=get_var(token);
                        return get_token();
      case MATH:
                     math_op=get_math(token);
                     if (math_op<0) return MATH_NAME_ERR;

                     if (math_pri[math_op]>1)
                      {
                        ret=get_token();    /* skip '('  */
                        if (ret) return ret;
                        op=*token;
                        if (op!='('&&op!='['&&op!='{') return LEFT_ERR+'(';
                        for (i=0;i<math_pri[math_op];i++)
                        {
                         ret=get_token();    /* pointer to new express */
                         if (ret) return ret;
                         ret=level2(&hold[i]);
                         if (ret) return ret;
                         if (*token!=',') break;
                        }
                        if (i!=math_pri[math_op]-1) return PARA_ERR+math_op;
                        switch (op)
                         {
                         case '(': if (*token!=')') return RIGHT_ERR+')';
                                   break;
                         case '[': if (*token!=']') return RIGHT_ERR+']';
                                   break;
                         case '{': if (*token!='}') return RIGHT_ERR+'}';
                                   break;
                         }

                        ret=math_math(math_op,result,hold);
                        if (ret) return ret;
                        return get_token();   /* skip ')' or ',' */
                      }
                      else
                      {
                        ret=get_token();    /* skip '('  */
                        if (ret) return ret;

                        ret=level6(&hold[0]);
                        if (ret) return ret;

                        ret=math_math(math_op,result,hold);
                        return ret;

                      }
    }
    return ERR_STR;
}

static short get_math(char *str)
{
   int i;
   for (i=0;i<MATHNUM;i++)
    if (!strcmp(str,math_str[i]))
      return i;
   return -1;
}

double get_num(char *str)
{
   short val;
   char str1[40];
   int i,j,len;

   //str1=str;
   strlwr(str);

   strncpy(str1,str,30);
   str1[30]=0;

   i=0;
   len=strlen(str1);
   str1[len+1]=0;
   str1[len]=0;

   while (str1[i])
   {
     if (str1[i]==',')
        for (j=i+1;j<=len;j++) str1[j-1]=str1[j];
     else
        i++;
   }

   while (*str&&(*str=='0'||*str==' ')) str++;

   switch(*str)
   {
      case  0 :  return 0.0;
      case 'x':  sscanf(str1,"%x",&val);
                 return (double)val;
      case 'o':  sscanf(str+1,"%o",&val);
                 return (double)val;
      default :
                 return atof(str1);
   }
}

static double get_var(char *str)
{
   if (*str<'a')
     return var_low[*str-'A'];
   return var_low[*str-'a'];
}

static short isdelim(char c)
{
   if (strchr(",+-/*%^=()[]{} ",c)||c==9||c=='\r'||c==0)
      return 1;
   return 0;
}
static short iswhite(char c)
{
   if (c==' '||c=='\t') return 1;
   return 0;
}

static int get_token(void)
{
   register char *temp,*old;
   int i;

   express_buff[EXPBUFFLEN-5]=0;
   express_buff[EXPBUFFLEN-4]=0;
   express_buff[EXPBUFFLEN-3]=0;
   express_buff[EXPBUFFLEN-2]=0;
   express_buff[EXPBUFFLEN-1]=0;

   i=strlen(express_buff);
   temp=express_buff+i;
   if (temp<prog) return END_EXP;

   token_type=0;
   tok=0;
   temp=token;

   while (iswhite(*prog)) prog++;

   if (*prog==0)
    {
       *token=0;
       tok=FINISHED;
       token_type=DELIMITER;
       return 0;
    }

   if (strchr("+-*/%^=()[]{},",*prog))
    {
       *temp++=*prog++;
       *temp=0;
       token_type=DELIMITER;
       return 0;
    }

   if (isdigit(*prog))
    {
      while (!isdelim(*prog)) *temp++=*prog++;
      *temp=0;
      token_type=NUMBER;
      return 0;
    }

   if ((*prog=='C'||*prog=='c'||
        *prog=='L'||*prog=='l'||
        *prog=='R'||*prog=='r'||
        *prog=='H'||*prog=='h')&&
       isdigit(*(prog+1)))
    {
       prog++;
       while (isdigit(*prog)) *temp++=*prog++;
       i=atoi(token);
       token[0]=0;
       if (i<1||i>26) return ERR_VAR;
       token[0]='A'+i-1;
       token[1]=0;
       token_type=VARIABLE;
       return 0;
    }

   if (isalpha(*prog))
    {
      old=prog;
      while (!isdelim(*prog)) *temp++=*prog++;

      *temp=0;
      if (strlen(token)==1)
          {
          token_type=VARIABLE;
          return 0;
          }
      strlwr(token);
      for (i=0;i<MATHNUM;i++)
        if (!strcmp(token,math_str[i]))
          {
          token_type=MATH;
          return 0;
          }

      for (i=0;i<MATHNUM;i++)
        if (!strncmp(token,math_str[i],strlen(math_str[i])))
          {
          i=strlen(math_str[i]);
          prog=old+i;
          token[i]=0;
          token_type=MATH;
          return 0;
          }
    }

   return ERR_STR;
}

static int level2(double *result)
{
   register char op;
   double hold;
   int ret;

   ret=level3(result);
   if (ret) return ret;
   while ((op=*token)=='+'||op=='-')
     {
       ret=get_token();
       if (ret) return ret;
       ret=level3(&hold);
       if (ret) return ret;
       ret=arith(op,result,&hold);
       if (ret) return ret;
     }
   return 0;
}

static int level3(double *result)
{
   register char op;
   double hold;
   int ret;

   ret=level4(result);
   if (ret) return ret;

   while ((op=*token)=='*'||op=='/'||op=='%')
     {
       ret=get_token();
       if (ret) return ret;
       ret=level4(&hold);
       if (ret) return ret;
       ret=arith(op,result,&hold);
       if (ret) return ret;
     }
   return 0;
}

static int level4(double *result)
{
   register char op;
   double hold;
   int ret;

   ret=level5(result);
   if (ret) return ret;

   if ((op=*token)=='^')
     {
       ret=get_token();
       if (ret) return ret;

       ret=level4(&hold);
       if (ret) return ret;

       return arith(op,result,&hold);
     }

   return 0;
}

static int level5(double *result)
{
   register char op;
   int ret;

   op=0;
   if ((token_type==DELIMITER)&&*token=='+'||*token=='-')
     {
       op=*token;
       ret=get_token();
       if (ret) return ret;
     }
   ret=level6(result);
   if (ret) return ret;

   if (op=='-')
     *result=-(*result);

   return 0;
}

static int level6(double *result)
{
   register char op;
   int ret;

   op=*token;
   if ((token_type==DELIMITER)&&(op=='('||op=='['||op=='{'))
     {
       ret=get_token();
       if (ret) return ret;
       ret=level2(result);
       if (ret) return ret;
       switch (op)
         {
         case '(': if (*token!=')') return RIGHT_ERR+')';
                   break;
         case '[': if (*token!=']') return RIGHT_ERR+']';
                   break;
         case '{': if (*token!='}') return RIGHT_ERR+'}';
                   break;
         }
       return get_token();
     }
   else
     return primitive(result);
}

int get_exp(char *str,double *result)
{
   int ret;
   strncpy(express_buff,str,EXPBUFFLEN-2);
   express_buff[EXPBUFFLEN-2]=0;
   prog=express_buff;
   ret=get_token();
   if (ret) return ret;
   if (*token)
   {
   ret=level2(result);
   if (ret) return ret;
   if (*token==0) return 0;
   return ERR_EXP;
   }
   else
   return NULL_EXP;
}

void set_val(int n,double num)
{
 if (n>=26||n<0) return ;
 var_low[n]=num;
}

/*
main()
{
  double ex=0.0;
  int ret;
  SetFillData();
  exit(0);
  var_low[0]=100;
  ret=get_exp("8*lgA+tgA+100+1e2",&ex);
  if (ret) errorlist(ret);
  printf("\nResult:%lf",ex);
  getch();
}
*/
