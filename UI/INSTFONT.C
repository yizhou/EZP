#include "ezpHead.h"

extern char CFontsName[][8];
extern char EFontName[][8];

// static short SelectLib[1100];
static short *SelectLib;
static int TotalSelect;
static int SelectCurr=0;
static unsigned long DiskFreeSize;
static unsigned long TotalDiskSpace;

static char SourcePath[][MAXSINGLETEXTLENGTH]={
    "d:\\bttf;e:\\bttf;f:\\bttf;g:\\bttf",
    "d:\\cttf;e:\\cttf;f:\\cttf;g:\\cttf",
    "d:\\ttf;e:\\ttf;f:\\ttf;g:\\ttf"
    };
static char TargetPath[][128]={
    "c:\\bttf",
    "c:\\cttf",
    "c:\\ttf"
    };
static char StatStr[80]="Font";
static int font_type=0;
static int InitOver=0;
static int BaseWindow=-1;
static int LISTWINDOW;
static int SELECTWINDOW;
static int SPATHWINDOW;
static int TPATHWINDOW;
static int STATWINDOW;
static int FREEDISKSPACEWINDOW;
static int TOTALDISKSPACEWINDOW;
static int DISPWINDOW;

#define InitOk()  (InitOver&&BaseWindow!=-1)
//#define LISTWINDOW (BaseWindow+2)

static int mycopy(char *nas,char *nad)
{
  FILE *fp,*fp1;
 #define COPY_LEN   4096
  unsigned char buff[COPY_LEN];
  int len;

  fp=fopen(nas,"rb");
  if (fp==NULL) return -1;
  fp1=fopen(nad,"wb");
  if (fp1==NULL)
  {
    fclose(fp);
    return -1;
  }

  len=COPY_LEN;
  while (len==COPY_LEN)
  {
    if (_bios_keybrd(1) && _bios_keybrd(0)==0x011b)
    {
        if (!MessageBox(GetTitleString(WARNINGINFORM),
                  "中止当前操作吗?",2,1) )
        {
              fclose(fp);
              fclose(fp1);
              unlink(nad);
              return -2;
        }
    }

    len=fread(buff,1,COPY_LEN,fp);
    if (len)
    {
        if (fwrite(buff,1,len,fp1)<len)
        {
            fclose(fp);
            fclose(fp1);
            return -3;
        }
    }
  } /*- while -*/

  fclose(fp);
  fclose(fp1);
  return 0;
 #undef COPY_LEN
}

static int GetPathName(int Cfont,char *pp,int mode)
{
  char *cp1,*cp2;
  char name[80];
  char fontname[13];
  char ext[4];
  int  fontn;

  switch (font_type)
  {
    case 2:
         fontn=Cfont+1;
         strcpy(fontname,"TTFALIB.");
         break;
    case 0:
         fontn=Cfont+1;
         strcpy(fontname,"THVECT.");
         break;
    case 1:
         fontn=Cfont+101;
         if (fontn>113) fontn+=200-114;
         strcpy(fontname,"RED0000.TTF");
         fontname[4]  += fontn/100;
         fontname[5]  += (fontn%100)/10;
         fontname[6]  += fontn%10;
         break;
  }

  ext[0]  = '0'+(fontn/100)%10;
  ext[1]  = '0'+ (fontn%100)/10;
  ext[2]  = '0'+ fontn%10;
  ext[3] = '\0';
  if (font_type==0||font_type==2)
     strcat(fontname,ext);

  if (mode==1)
  {
    strcpy(pp,fontname);
    return 0;
  }

  cp1=SourcePath[font_type];

  while (*cp1) {                        /* SEARCH FOR ALL PATH */
     cp2 = name;
     while(*cp1 != ';' && *cp1 != '\0')
                 *cp2++ = *cp1++;
     if (*(cp2-1) !='/' && *(cp2-1) != '\\') *cp2++ = '\\';
     if (*cp1 == ';') cp1++;
     strcpy(cp2,fontname);
     if(access(name,0)==0)      // this lib is exist
     {
        strcpy(pp,name);
        return 0;
     }
  }

  sprintf(pp,"在指定的路径中未发现%s,\n请插入系统光盘或重新指定路径。",fontname);
  return -1;
}

static int InstallAct(void)
{
    char pp[100],pp1[100],pp2[100];
    int i,curr,err;

    if (TotalSelect<1) return 0;
    strcpy(pp,TargetPath[font_type]);
    i=strlen(pp);
    if (i<1) return -1;
    if (pp[i-1]==';') { pp[i-1]=0; i--; }
    if (i<1) return -1;
    if (pp[i-1]=='\\') { pp[i-1]=0; i--; }

    strcpy(pp1,pp);
    strcat(pp1,"\\.");
    if (access(pp1,0)!=0)
    {
        //no exist dir
        if (mkdir(pp)!=0) return -1;  //can't create
    }

    if (pp[i-1]!='\\') { pp[i]='\\'; pp[i+1]=0; }

    for (i=0;i<TotalSelect;i++)
    {
        curr=SelectLib[i];
        GetPathName(curr,pp2,1);
        MessageGo(STATWINDOW,11909,FP2LONG(pp2),0l);
        WaitMessageEmpty();
        strcpy(pp1,pp);
        strcat(pp1,pp2);
        GetPathName(curr,pp2,0);
        if ((err=mycopy(pp2,pp1))!=0) return err;
    }
   return 0;
}

static unsigned long GetFileLeng(char *name)
{
    FILE *fp;
    unsigned long leng;
    fp=fopen(name,"rb");
    if (fp==NULL) return 0l;
    fseek(fp,0l,SEEK_END);
    leng=ftell(fp);
    fclose(fp);
    return leng;
}

static unsigned char sInvalidPath[]="字库路径错误";
static unsigned char sInstStatus[]="安装状态";

static void AddFontLib(int curr)
{
    int i;
    char pp[100];

    if (GetPathName(curr,pp,0)==-1)
    {
        MessageBox(sInvalidPath, pp,1,1);
        return;
    }

    if (TotalSelect<1000)
      {
        for (i=0;i<TotalSelect;i++)
         if (curr==SelectLib[i])
           {
            MessageBox(GetTitleString(WARNINGINFORM),
                       "字库已选择,无须再次安装!",
                       1,1);
            return ;
           }

        SelectCurr=TotalSelect;
        SelectLib[TotalSelect++]=curr;
        TotalDiskSpace+=GetFileLeng(pp);
        if (InitOk())
          {
            MessageInsert(SELECTWINDOW,WINDOWINIT,0l,0l);
            MessageInsert(TOTALDISKSPACEWINDOW,WINDOWINIT,0l,0l);
          }
      }
}

static void AddFontLibAll()
{
  int j,i,err1=0,err2=0;
  char pp[100];
  int coun;

  switch (font_type)
  {
     case 0: coun=55; break;
     case 1: coun=81; break;
     case 2: coun=999; break;
  }

  for (j=0;j<coun;j++)
  {
     if (GetPathName(j,pp,0)==-1)
        err1++;
     else
     {
        if (TotalSelect<1000)
        {
           for (i=0;i<TotalSelect;i++)
            if (j==SelectLib[i])
            {
               err2++;
               break;
            }

            if (i==TotalSelect)
            {
              SelectCurr=TotalSelect;
              SelectLib[TotalSelect++]=j;
              TotalDiskSpace+=GetFileLeng(pp);
            }
        }
     }  // GetPathName
  }   // for j

  if (err1)
  {
        sprintf(pp,"一共有%d个文件未找到!",err1);
        MessageBox(sInvalidPath, pp,1,1);
  }

  if (err2)
  {
        sprintf(pp,"一共有%d个文件重复!",err2);
        MessageBox(GetTitleString(WARNINGINFORM),
                   pp,1,1);
  }

  if (InitOk())
  {
        MessageInsert(SELECTWINDOW,WINDOWINIT,0l,0l);
        MessageInsert(TOTALDISKSPACEWINDOW,WINDOWINIT,0l,0l);
   }
}

static void DelFontLib(int curr)
{
    int i;
    char pp[100];

    if (GetPathName(SelectLib[curr],pp,0)==-1)
    {
        MessageBox(sInvalidPath,pp,1,1);
        return;
    }

    if (curr>=0&&curr<TotalSelect)
    {
        for (i=curr;i<TotalSelect;i++) SelectLib[i]=SelectLib[i+1];
        TotalSelect--;
        SelectCurr=curr-1;
        TotalDiskSpace-=GetFileLeng(pp);
    }
    if (InitOk())
     {
         MessageInsert(SELECTWINDOW,WINDOWINIT,0l,0l);
         MessageInsert(TOTALDISKSPACEWINDOW,WINDOWINIT,0l,0l);
     }
}

unsigned long StatProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           STATWINDOW=Window;
           MessageGo(Window,SETLINEBUFFER,FP2LONG(StatStr),0l);
           MessageInsert(Window,REDRAWMESSAGE,0L,
                  MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
           break;
    case GETFOCUS:              // ByHance, 97,5.8
    case MOUSEMOVE:
          return FALSE;
    case 11909:
           sprintf(StatStr,"  %s",LONG2FP(Param1));
           MessageGo(Window,SETLINEBUFFER,FP2LONG(StatStr),0l);
           MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
           break;
    case 11908:
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long DispProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           DISPWINDOW=Window;
           break;
    case GETFOCUS:              // ByHance, 97,5.13
    case MOUSEMOVE:
          return FALSE;
    default:
           return (ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long GetFreeSpace(char *pa)
{
  int dr,result;
  struct _diskfree_t diskdata;
  if (pa[1]==':')
    dr=((pa[0]|0x20)-'a'+1);
  else
    dr=getdisk();

  if (dr<3) return 0l;
  result=_dos_getdiskfree(dr,&diskdata);
  if(result==0)
  {
     DiskFreeSize=diskdata.avail_clusters;
     DiskFreeSize *= diskdata.sectors_per_cluster;
     DiskFreeSize *= diskdata.bytes_per_sector;
     return DiskFreeSize;
  }
  else
   return 0l;
}

static void FormatNumText(char *s,int len1)
{
     int i,j,k,len,st;
     if (*s=='C')
     {
       len=min(strlen(s),len1);
       for (i=0;i<len;i++) s[i]=s[i+1];  //Get off the C ID
       for (i=0;i<len;i++)
        if (s[i]==0) break;
        else
          if (s[i]>='0'&&s[i]<='9')
            {
               for (j=i;j<len;j++)
                  if (s[j]<'0'||s[j]>'9')
                  {
                   st=strlen(s);
                   while (j-i>3&&st<len1-1)
                     {
                      st++;
                      for (k=st;k>j-3;k--) s[k]=s[k-1];
                      s[j-3]=',';
                      j=j-3;
                     }
                   break;
                  }

               break;
            }
     }
     s[len1-1]=0;
}

unsigned long FreeDiskSpaceProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           FREEDISKSPACEWINDOW=Window;
           sprintf(StatStr,"C%lu",GetFreeSpace(TargetPath[font_type]));
           FormatNumText(StatStr,75);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(StatStr),0l);
           MessageInsert(Window,REDRAWMESSAGE,0L,
                MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
           break;
    case GETFOCUS:              // ByHance, 97,5.8
    case MOUSEMOVE:
          return FALSE;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TotalDiskSpaceProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           TOTALDISKSPACEWINDOW=Window;
           sprintf(StatStr,"C%lu",TotalDiskSpace);
           FormatNumText(StatStr,75);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(StatStr),0l);
           MessageInsert(Window,REDRAWMESSAGE,0L,
               MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
           break;
    case GETFOCUS:              // ByHance, 97,5.8
    case MOUSEMOVE:
          return FALSE;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long SourcePathProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           SPATHWINDOW=Window;
           MessageGo(Window,SETLINEBUFFER,FP2LONG(SourcePath[font_type]),0l);
           MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
    case KEYDOWN:
    case KEYSTRING:
           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
           strcpy(SourcePath[font_type],
                  LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)));
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TargetPathProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           TPATHWINDOW=Window;
           MessageGo(Window,SETLINEBUFFER,FP2LONG(TargetPath[font_type]),0l);
           MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
    case KEYDOWN:
    case KEYSTRING:
           SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2);
           strcpy(TargetPath[font_type],
                  LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)));
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

ULONG FontLibType(HWND Window,HMSG Message,long Param1,long Param2)
{
  int Order;
  switch (Message)
  {
    case WINDOWINIT:
         Order=RadioGetOrder(Window);
         if (font_type!=Order) break;
         MessageGo(Window,SETSTATUS,1l,0l);
         MessageInsert(Window,SELECTSELECTED,(long)Order+1,0l);
         break;
    case SELECTSELECTED:
         Order=RadioGetOrder(Window);
         if (Order==font_type) break;

         font_type=Order;
         if (InitOk())
         {
            TotalSelect=0;
            TotalDiskSpace=0l;
            sprintf(StatStr,"选择字库种类");
            MessageInsert(STATWINDOW,WINDOWINIT,0l,0l);
            MessageInsert(LISTWINDOW,WINDOWINIT,0l,0l);
            MessageInsert(SELECTWINDOW,WINDOWINIT,0l,0l);
            MessageInsert(SPATHWINDOW,WINDOWINIT,0l,0l);
            MessageInsert(TPATHWINDOW,WINDOWINIT,0l,0l);
            MessageInsert(FREEDISKSPACEWINDOW,WINDOWINIT,0l,0l);
         }
         break;
    default:
         return(RadioDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long InstallFont(HWND Window,HMSG Message,long Param1,long Param2)
{
  int i,hlist;
  switch (Message)
   {
    case REDRAWMESSAGE:
            i=DialogDefaultProcedure(Window, Message, Param1, Param2);
            WaitMessageEmpty();
            InitOver=1;
            return i;
    case WINDOWINIT:
            InitOver=0;
            TotalSelect=0;
            TotalDiskSpace=0l;
            BaseWindow=Window;
            SelectLib=(short *)&TmpBox;
            i=DialogDefaultProcedure(Window, Message, Param1, Param2);
            return i;
    case 11900:         //add
            hlist = WindowList(LISTWINDOW);
            AddFontLib(ListGetCurrent(hlist));
            break;
    case 11901:         //add all
            AddFontLibAll();
            break;
    case 11902:         //del
            hlist = WindowList(SELECTWINDOW);
            DelFontLib(ListGetCurrent(hlist));
            break;
    case 11903:         //del all
            TotalSelect=0;
            TotalDiskSpace=0;
            SelectCurr=0;
            MessageInsert(SELECTWINDOW,WINDOWINIT,0l,0l);
            MessageInsert(TOTALDISKSPACEWINDOW,WINDOWINIT,0l,0l);
            break;
    case 11904:         //to install
            if (TotalSelect<=0)
            {
                MessageBox(sInstStatus,"没有选择任何字库!",1,1);
                break;
            }

            if (DiskFreeSize<TotalDiskSpace)
            {
                if (
                MessageBox(GetTitleString(WARNINGINFORM),
                     "磁盘空间不够,继续吗?",2,1)
                )
                break;
            }

            switch (InstallAct())
            {
              case 0:
                MessageBox(sInstStatus,"字库文件拷贝成功!",1,1);
                strcpy(StatStr,"安装正常完成");
                switch (font_type)
                {
                    case 0:
                            //strcpy(TrueTypeLibPath,savestr1);
                            strcpy(VectLibPath,"c:\\ezp\\fonts;");
                            strcat(VectLibPath,TargetPath[0]);
                            strcat(VectLibPath,";d:\\bttf;e:\\bttf;f:\\bttf;g:\\bttf");
                            break;
                    case 1:
                    case 2:
                            strcpy(TrueTypeLibPath,"c:\\ezp\\fonts;");
                            strcat(TrueTypeLibPath,TargetPath[2]);
                            strcat(TrueTypeLibPath,";");
                            strcat(TrueTypeLibPath,TargetPath[1]);
                            strcat(TrueTypeLibPath,";d:\\ttf;e:\\ttf;f:\\ttf;g:\\ttf;d:\\cttf;e:\\cttf;f:\\cttf;g:\\cttf");
                            break;
                }
                SaveFontPath();
                break;
              case -1:
                MessageBox(sInstStatus,"路径格式错误!",1,1);
                goto lbl_err;
              case -2:
                MessageBox(sInstStatus,"用户中止安装!",1,1);
                goto lbl_err;
              case -3:
                MessageBox(sInstStatus,"磁盘空间不够!",1,1);
                goto lbl_err;
              default:
                MessageBox(sInstStatus,"字库文件拷贝失败!",1,1);
               lbl_err:
                strcpy(StatStr,"安装异常退出");
                break;
            }
            MessageInsert(STATWINDOW,WINDOWINIT,0l,0l);
            break;
    case 11905:         //end
            Message=DIALOGBOXOK;
            break;
  }
  return(DialogDefaultProcedure(Window, Message, Param1, Param2));
}

static void DispLD(int curr)
{
    int ty=font_type+2;
    int CFont=curr;
    int Width=40;
    int Height=41;
    int Color=0;
    static char Tmp_str[]={"REDTEK"};
    static char Test_str[]={"理德"};
    char *p,savestr1[MAXSINGLETEXTLENGTH],savestr2[MAXSINGLETEXTLENGTH];
    struct viewporttype ViewInformation;
    UINT Code;
    int PosX,PosY;
    int sx,sy,ex,ey;
    int oldcolor=getcolor();

    strcpy(savestr1,TrueTypeLibPath);
    strcpy(savestr2,VectLibPath);

    sx=WindowGetLeft(DISPWINDOW)+WindowGetLeft(BaseWindow);
    sy=WindowGetTop(DISPWINDOW)+WindowGetTop(BaseWindow);
    ex=WindowGetRight(DISPWINDOW)+WindowGetLeft(BaseWindow);
    ey=WindowGetBottom(DISPWINDOW)+WindowGetTop(BaseWindow);

    p=Test_str;

    getviewsettings(&ViewInformation);
    MouseHidden();

    setviewport(sx,sy,ex,ey,1);
    setcolor(15);
    bar(0,0,ex-sx,ey-sy);
    PosX=2;
    PosY=ey-sy-2;

    switch (ty)
    {
       case 2:
                strcpy(VectLibPath,SourcePath[0]);
                if(!DetectVecFont(CFont)||CFont==23||CFont==29||CFont==32||CFont==33||CFont==45)
                   ty=5;
                break;
       case 3:
                strcpy(TrueTypeLibPath,SourcePath[1]);
                if(!DetectTrueTypeFont(CFont))
                   ty=5;
                break;
       case 4:
                strcpy(TrueTypeLibPath,SourcePath[2]);
                CFont-=0x400;
                p=Tmp_str;
                Width/=2;
                if (!detectEFont(CFont)) ty=5;
                break;
    }

    while (*p)
    {
       if(kbhit())
         break;

       switch(ty)
       {
          case 2:
                 Code=((*p++)<<8)+(*p++);
                 ViewportDisplayCharVec(PosX,PosY,Code,CFont,Width,Height,0,0,Color);
                 PosX=PosX+Width;
                 break;
          case 3:
                 Code=((*p++)<<8)+(*p++);
                 ViewportDisplayCharTTF(PosX,PosY,Code,CFont,Width,Height,0,0,Color);
                 PosX=PosX+Width;
                 break;
          case 4:
                 Code=*p++;
                 //if (Code==' ') break;
                 //ViewportDisplayEChar(PosX,PosY,Code,CFont,Width,Height,0,0,Color);
                 BuildChar(PosX,PosY,Code,CFont+1,Width,Height,0,0,Color,0,RITALICBIT|ROTATEBIT,0);

                 PosX=PosX+Width;
                 break;
          default:  p++;
                 break;
       }
    }

    strcpy(TrueTypeLibPath,savestr1);
    strcpy(VectLibPath,savestr2);

    setviewport(ViewInformation.left,ViewInformation.top,
                   ViewInformation.right,ViewInformation.bottom,
                   ViewInformation.clip);
    MouseShow();
    setcolor(oldcolor);
}

static unsigned long FontList(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist,i,curr;

  switch (Message)
  {
    case WINDOWINIT:
         LISTWINDOW=Window;
         MessageGo(Window,LISTSETITEMHEIGHT,16,0);

         #ifdef OLD_VERSION
         switch (font_type)
         {
          case 0:
          case 1:
              MessageGo(Window,LISTSETITEMLENGTH,8,0);
              break;
          case 2:
              MessageGo(Window,LISTSETITEMLENGTH,8,0);
              break;
         }
         #else
              MessageGo(Window,LISTSETITEMLENGTH,8,0);
         #endif

         MessageGo(Window,LISTDELETEALL,0L,0L);
         switch(font_type)
         {
          case 0:
              for (i=0;i<MAXVECFONT;i++)
                  MessageGo(Window, LISTINSERTITEM, FP2LONG(CFontsName[MAXVECFONT-i-1]), 0L);
              break;
          case 1:
              for (i=MAXVECFONT;i<TOTALCFONTS;i++)
                MessageGo(Window, LISTINSERTITEM, FP2LONG(CFontsName[TOTALCFONTS-i+MAXVECFONT-1]), 0L);
              break;
          case 2:
              for (i=994;i>=0;i--)
              {
                 sprintf(EFontName[4]+4,"%03d",i+5);
                 MessageGo(Window, LISTINSERTITEM, FP2LONG(EFontName[4]), 0L);
              }
              for (i=0;i<4;i++)
                 MessageGo(Window, LISTINSERTITEM, FP2LONG(EFontName[3-i]), 0L);
              break;
         }

         curr=0;
         hlist = WindowList(Window);
         ListSetTop(hlist,0);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,ITEMSELECT,-1l,-1l);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
         break;
    case ITEMSELECT:
         hlist = WindowList(Window);
         switch (font_type)
         {
            case 0:
                curr = ListGetCurrent(hlist);
                break;
            case 1:
                curr = ListGetCurrent(hlist);
                if (curr<MAXTTFONTJ-100) curr = curr+100;
                else
                    curr = curr-(MAXTTFONTJ-100)+200;
                break;
            case 2:
                curr = ListGetCurrent(hlist)+0x400;
                break;
         }
         DispLD(curr);

         sprintf(StatStr,"选择安装的字库");
         if (InitOk()&&Param1!=-1l) MessageInsert(STATWINDOW,WINDOWINIT,0l,0l);
         break;
    case LISTBOXCONFIRM:
         hlist = WindowList(Window);
         i = ListGetTop(hlist);
         curr = ListGetCurrent(hlist);
         AddFontLib(curr);
         MessageInsert(Window,ITEMSELECT,0,0);

         MessageGo(Window,WINDOWINIT,0,0);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         ListSetTop(hlist,i);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long SelectList(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist,i,curr;
  char name[10];

  switch (Message)
  {
    case WINDOWINIT:
         SELECTWINDOW=Window;
         MessageGo(Window,LISTSETITEMHEIGHT,16,0);

         switch (font_type)
         {
          case 0:
          case 1:
              MessageGo(Window,LISTSETITEMLENGTH,10,0);
              break;
          case 2:
              MessageGo(Window,LISTSETITEMLENGTH,10,0);
              break;
         }
         MessageGo(Window,LISTDELETEALL,0L,0L);

         switch(font_type)
         {
          case 0:
              for (i=TotalSelect-1;i>=0;i--) {
                memset(name,0,10);
                strncpy(name,CFontsName[SelectLib[i]],8);
                MessageGo(Window, LISTINSERTITEM, FP2LONG(name), 0L);
              }
              break;
          case 1:
              for (i=TotalSelect-1;i>=0;i--) {
                memset(name,0,10);
                strncpy(name,CFontsName[SelectLib[i]+MAXVECFONT],8);
                MessageGo(Window, LISTINSERTITEM, FP2LONG(name), 0L);
              }
              break;
          case 2:
              for (i=TotalSelect-1;i>=0;i--)
              {
                 if (SelectLib[i]<4)
                    MessageGo(Window, LISTINSERTITEM, FP2LONG(EFontName[SelectLib[i]]), 0L);
                 else
                 {
                    sprintf(EFontName[4]+4,"%03d",SelectLib[i]+1);
                    MessageGo(Window, LISTINSERTITEM, FP2LONG(EFontName[4]), 0L);
                 }
              }
              break;
         }

         curr=SelectCurr;
         hlist = WindowList(Window);
         if (curr<4)  ListSetTop(hlist,0); else ListSetTop(hlist,curr-4);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,ITEMSELECT,-1l,-1l);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
         break;
    case ITEMSELECT:
         if(TotalSelect<=0)         // ListGetTotal(hlist)
            break;

         hlist = WindowList(Window);
         switch (font_type)
         {
            case 0:
                curr = SelectLib[ListGetCurrent(hlist)];
                break;
            case 1:
                curr = SelectLib[ListGetCurrent(hlist)];
                if (curr<MAXTTFONTJ-100) curr= curr+100;
                else
                   curr = curr-(MAXTTFONTJ-100)+200;
                break;
            case 2:
                curr = SelectLib[ListGetCurrent(hlist)]+0x400;
                break;
         }
         DispLD(curr);
         sprintf(StatStr,"选中安装的字库");
         if (InitOk()&&Param1!=-1l) MessageInsert(STATWINDOW,WINDOWINIT,0l,0l);
         break;
    case LISTBOXCONFIRM:
         if(TotalSelect<=0)         // ListGetTotal(hlist)
            break;
         hlist = WindowList(Window);
         i = ListGetTop(hlist);
         curr = ListGetCurrent(hlist);
         DelFontLib(curr);
         MessageInsert(Window,ITEMSELECT,0,0);

         MessageGo(Window,WINDOWINIT,0,0);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         ListSetTop(hlist,i);
         ListSetCurrent(hlist,curr);
         MessageInsert(Window,REDRAWMESSAGE,0L,
            MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
         break;
    case GETFOCUS:
         if(TotalSelect<=0)         // ListGetTotal(hlist)
            return FALSE;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

Dialogs InstFontDialog[]=
{
  #define X     88
  #define Y     62
  #define nn    26
  { GLOBALITEM, 23 , X, Y+18, 20+551, 385+18, 0, InstallFont,"安装字库" },

  { STATICTEXTITEM, 0, 275-X-nn, 102-Y, 10+345-X-nn/2, 122-Y, 0, NULL, "字库列表" },  //1
  { LISTBOXITEM,    0, 275-X-nn, 125-Y, 10+345-X-nn/2, 280-Y, 0, FontList, "" },      //2

  { USERBUTTONITEM, 0, 274-X-nn, 299-Y, 346-X-nn/2, 326-Y, 11900, NULL, " 增  加 " }, //3
  { USERBUTTONITEM, 0, 274-X-nn, 339-Y, 346-X-nn/2, 366-Y, 11901, NULL, "全部增加" }, //4

  { STATICTEXTITEM, 0, 10+355-X-nn/2, 102-Y, 20+425-X, 122-Y, 0, NULL, "已选字库" },     //5
  { LISTBOXITEM,    0, 10+355-X-nn/2, 125-Y, 20+425-X, 280-Y, 0, SelectList, "" },             //6

  { USERBUTTONITEM, 0, 274+80-X-nn/2, 299-Y, 346+80-X, 326-Y, 11902, NULL, " 删  除 " }, //7
  { USERBUTTONITEM, 0, 274+80-X-nn/2, 339-Y, 346+80-X, 366-Y, 11903, NULL, "全部删除" }, //8

  { STATICTEXTITEM, 0, 20+435-X, 102-Y, 20+475-X, 122-Y, 0, NULL, "字样" }, //9
  { LISTBOXITEM,    0, 20+435-X, 125-Y, 20+542-X, 185-Y, 0, DispProcedure, "" },     //10

  { STATICTEXTITEM, 0, 20+435-X, 195-Y, 20+542-X, 215-Y, 0, NULL, "需要磁盘空间" }, //11
  { SINGLELINEEDITORITEM, 0, 20+437-X, 217-Y, 20+540-X, 233-Y, 0,TotalDiskSpaceProcedure, "" }, //12

  { STATICTEXTITEM, 0, 20+435-X, 195-Y+45, 20+542-X, 215-Y+45, 0, NULL, "剩余磁盘空间" }, //13
  { SINGLELINEEDITORITEM, 0, 20+437-X, 217-Y+45, 20+540-X, 233-Y+45, 0,FreeDiskSpaceProcedure, "" },

  { USERBUTTONITEM, 0, 20+445-X, 328+5-Y-38, 20+532-X, 358+5-Y-38, 11904, NULL, "安 装" },
  { USERBUTTONITEM, 0, 20+445-X, 328+10-Y, 20+532-X, 358+10-Y, 11905, NULL, "退 出" },


  { STATICTEXTITEM, 0, 111-X, 220-Y, 230-X, 240-Y, 0, NULL, "字库位置" },
  { STATICTEXTITEM, 0, 111-X, 270-Y, 230-X, 290-Y, 0, NULL, "安装到" },
  { SINGLELINEEDITORITEM, 0, 112-X, 242-Y, 256-X-nn, 262-Y, 0,SourcePathProcedure, "" },
  { SINGLELINEEDITORITEM, 0, 112-X, 292-Y, 256-X-nn, 312-Y, 0,TargetPathProcedure, "" },

  { STATICTEXTITEM, 0, 111-X, 320-Y, 230-X, 340-Y, 0, NULL, "当前状态:" },
  { SINGLELINEEDITORITEM, 0, 112-X, 342-Y, 256-X-nn, 362-Y, 0,StatProcedure, "" },

  { FRAMEITEM, 3, 111-X, 102-Y, 310-X-48-nn, 306-Y-95, 0, NULL, "字库种类" },
      #define FX        111
      #define FY        102
      { SINGLESELECT, 0, 133-FX, 129-FY, 203+47-FX-nn, 149-FY, 0, FontLibType, "中文矢量" },
      { SINGLESELECT, 0, 133-FX, 154-FY, 203+47-FX-nn, 174-FY, 1, FontLibType, "中文曲线" },
      { SINGLESELECT, 0, 133-FX, 179-FY, 203+47-FX-nn, 199-FY, 2, FontLibType, "西文曲线" },

      #undef FX
      #undef FY

#undef X
#undef Y
#undef nn
};

unsigned long VectPathProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(VectLibPath),0l);
           break;
    case DIALOGBOXOK:
           strcpy(VectLibPath,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)));
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

unsigned long TruePathProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
           MessageGo(Window,SETLINEBUFFER,FP2LONG(TrueTypeLibPath),0l);
           break;
    case DIALOGBOXOK:
           strcpy(TrueTypeLibPath,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0l,0l)));
           break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

Dialogs SetFontPathDialog[]=
{
  #define ggg 40
  #define gggg 30
  { GLOBALITEM, 6 , 0, 0, 433, 240, 0, NULL, "设置字库路径" },

  { STATICTEXTITEM, 0, 49+40,  42+32-gggg, 234+120,  61+32-gggg, 0, NULL, "中文矢量字库:" },
  { SINGLELINEEDITORITEM, 0, 49+40,  70+27-gggg, 234+120,  89+27-gggg, 0, VectPathProcedure, "" },
  { STATICTEXTITEM, 0, 49+40,  42+32+ggg, 234+120,  61+32+ggg, 0, NULL, "中、英文曲线字库:" },
  { SINGLELINEEDITORITEM, 0, 49+40,  70+27+ggg, 234+120,  89+27+ggg, 0, TruePathProcedure, "" },

  { OKBUTTON, 0, 295-200+10, 184, 396-200-10, 213, 0, NULL, "确定" },
  { CANCELBUTTON, 0, 295-50+10, 184, 396-50-10, 213, 0, NULL, "放弃" },
  #undef ggg
  #undef gggg
};
