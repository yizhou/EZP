/*-------------------------------------------------------------------
* Name: mainc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#ifdef NOT_USED
static char HelpMessage[]="Usage:\n"
                          "  -? -H: display this page;\n"
                          "  -FXX: change default library name, and default is YFONT.LIB;\n"
                          "  -PXX: change default REDTEK library path,\n"
                          "        default is root directory of the disk REDTEK font maker in;\n"
                          "  -LXX: change default REDTEK loading font, and default is teh first font;\n"
                          "  -QXX: change default QQ to import, and default is 16;\n"
                          "  -WXX: change default WW to import, and default is 1;\n"
                          "  -NXXXX: change default to load, edit and save, and default is 0;\n"
                          "  -EXXX: change default polygon edges to be created, default is 3;\n"
                          "  -K: use PC/XT/AT 83 key keyboard, and default is 101/103 key.\n"
                          "  -V: turn buttons display off, and for cga, ega, herc video must turn it off.\n"
                          "*** When any parameter error is detected, REDTEK font maker will halt. ***\n";

void HelpCommand(void)
{
//  fprintf(stdout,WelcomeMessage);
  fprintf(stdout,HelpMessage);
  exit(0);
}
#endif  // not_used

void SaveFontPath(void)
{
  LPVECFNT Lpvfnt;

  if ((Lpvfnt=HandleLock(cfnHandle))==NULL)
      return;

  if ((SysDc.lpttf=HandleLock(efnHandle))==NULL)
  {
     HandleUnlock(cfnHandle);
     return;
  }

  strcpy (Lpvfnt->cfnPath,VectLibPath);
  strcpy (SysDc.lpttf->ttPath,TrueTypeLibPath);
  SetProfileString( ProfileName,InitSection, VectLibPathEntry,VectLibPath);
  SetProfileString( ProfileName,InitSection, TrueTypeLibPathEntry,TrueTypeLibPath);

  HandleUnlock(cfnHandle);
  HandleUnlock(efnHandle);
}

static void ReadDefaultScreenMode(void)
{
      char ScrM[200];

      if (ScreenMode!=-1) return ;

      GetProfileString( ProfileName,DefaultSection,ScrModeEntry,ScrM, Scr640);
      if (!strncmp(ScrM,Scr800,3))
           ScreenMode=MODE800X600X16;
      else
      if (!strncmp(ScrM,Scr1024,4))
           ScreenMode=MODE1024X768X16;
      else
      //if (!strncmp(ScrM,"640",3))
      //     ScreenMode=MODE640X480X16;
      //else
           ScreenMode=MODE640X480X16;
}

static void WriteDefaultScreenMode(void)
{
      char ScrM[64];

      switch (ScreenMode)
       {
        case MODE800X600X16:
             strcpy(ScrM,Scr800);
             break;
        case MODE1024X768X16:
             strcpy(ScrM,Scr1024);
             break;
       // case MODE640X480X16:
        default:
             strcpy(ScrM,Scr640);
             break;
       }

       SetProfileString( ProfileName,DefaultSection,ScrModeEntry,ScrM);
}

#ifdef NOT_USED
static int EMMTest(void)
{
    FILE *fp;
    fp=fopen("EMMXXXX0","rb");
    if (fp==NULL)  return 0;
    fclose(fp);
    return 1;
}

static int EMMMessage(void)
{
     static char MouseErrMsg1[]="·¢ÏÖEMM386ÒÑ¾­ÔËÐÐ,ÈôÄúÔÚÔËÐÐ±¾";
     static char MouseErrMsg2[]="Èí¼þ¹ý³ÌÖÐ·¢ÏÖÓÐÒì³££¬Çë¹Ø±ÕEMM386¡£";
     static char MouseErrMsg3[]="[½«Config.sysÖÐµÄ device=EMM386 É¾³ý";
     static char MouseErrMsg4[]="»òÓÃ REM ×¢ÊÍµô»òÐÂ½¨Ò»²Ëµ¥Ñ¡Ïî]";
     static char MouseErrMsg5[]=" ÏÖÔÚÄú¿É°´<Esc>ÍË³ö, ±ðµÄ¼ü½«¼ÌÐø";
     int x=getmaxx(),y=getmaxy();
     int disp_x=x/2-(strlen(MouseErrMsg2)+4)*ASC16WIDTH/2;
     int disp_y=y/2-10;
     int Result;

     setfillstyle(1,EGA_BLACK);
     bar(0,0,x,y-40);

     DisplayString(MouseErrMsg1,disp_x+4*ASC16WIDTH,disp_y-3*ASC16HIGHT-4,
               EGA_WHITE,EGA_BLACK);
     DisplayString(MouseErrMsg2,disp_x,disp_y-2*ASC16HIGHT,
               EGA_WHITE,EGA_BLACK);
     DisplayString(MouseErrMsg3,disp_x,disp_y-1*ASC16HIGHT+4,
               EGA_WHITE,EGA_BLACK);
     DisplayString(MouseErrMsg4,disp_x,disp_y-0*ASC16HIGHT+8,
               EGA_WHITE,EGA_BLACK);
     DisplayString(MouseErrMsg5,disp_x,disp_y+1*ASC16HIGHT+20,
               EGA_WHITE,EGA_BLACK);
     fflush(stdout);
     Result=getch();
     if(Result==0) getch();
     else if(Result==ESC) return(-1);

     while (kbhit()) getch();
     return 0;
}
#endif

static void set_path(char *p)
{
  unsigned old;
  if (!p[0]) return;
  if (p[1]==':') _dos_setdrive((p[0]|0x20)-'a'+1,&old);
  chdir(p);
}

static void MakeName(char *str)
{
   char *p;
   char s1[100],s2[100];
   int i,len;

   if(fAutoLoad)        // only get ONE file
      return;

   fAutoLoad=TRUE;
   if(fEditor)
   {
      strcpy(DebugFileName,str);
      return;
   }
   /*
   //----- else, it is xxxx.EZP ------
   p = str;
   i = 0;
   while (*p!='.'&&*p!='\0') DebugFileName[i++] = *p++;
   DebugFileName[i++] = '.';
   DebugFileName[i++] = 'E';
   DebugFileName[i++] = 'Z';
   DebugFileName[i++] = 'P';
   DebugFileName[i] = '\0';
   */
   getcwd(s1,80);
   //printf("\nOld Path:%s",s1);
   p=str;
   strupr(p);

   strcpy(s2,p);
   len=strlen(s2);
   for (i=len-1;i>=0;i--) if (s2[i]=='\\'||s2[i]==':')
     {
        s2[i+1]=0;
        break;
     }
   //if (s2[1]==':'&&i>2&&s2[i]=='\\') s2[i]=0;
   if (i>2&&s2[i]=='\\') s2[i]=0;
   i++;
   set_path(s2);
   //printf("\nS2:%s",s2);
   getcwd(DebugFileName,60);
   //printf("\nNew Path:%s",DebugFileName);
   if (strlen(DebugFileName)>3) strcat(DebugFileName,"\\");
   strcat(DebugFileName,p+i);
   set_path(s1);
   filename_cat(DebugFileName,FILE_EXT_NAME);
   //printf("\nName:%s",DebugFileName);
   //exit(0);
}

static int SystemConstruct(void)
{
  int Result;
  int x,y;

  _harderr(handler);

  ReadDefaultScreenMode();

  if ((Result=LockMouseMemory())<0)
     return(Result);

  TimerInit();
  HandleInitial();
  if ((Result=ChineseLibInitial())<0)
     return(Result);
  if ((Result=ItemInitial())<0)
     return(Result);

  x=getmaxx();  y=getmaxy();
    if ((Result=WindowInitial(x,y-30))<0)
     return(Result);

  if ((Result=GraphInitial())<0)
     return(Result);

  if ((Result=MouseConstruct(x,y))<0 && !fEditor) {
   #ifdef OLD_VERSION
     //printf("I can't find mouse or mouse driver. Press a key ...\n");
     static char MouseErrMsg[]="ÇëÏÈ°²×°Êó±êÇý¶¯³ÌÐò¡£°´<Esc>ÍË³ö¡­¡­";
     DisplayString(MouseErrMsg,(x-strlen(MouseErrMsg)*ASC16WIDTH)/2,y/2,
               EGA_LIGHTRED,getbkcolor());
     fflush(stdout);
     getch();
     return(Result);
   #else
     static char MouseErrMsg1[]="Î´°²×°Êó±êÇý¶¯³ÌÐò(ÓÐÐ©¹¦ÄÜ±ØÐëÓÉ";
     static char MouseErrMsg2[]="Êó±ê²ÅÄÜÍê³É)! Äú¿ÉÍË³öÏµÍ³, ÏÈÔËÐÐÊó";
     static char MouseErrMsg3[]="±êÇý¶¯³ÌÐò, Èç:   AMOUSE     [»Ø³µ]";
     static char MouseErrMsg4[]="ÈôÊó±ê×°ÔÚ´®¿Ú¶þÉÏ, Ôò¼üÈë:  AMOUSE/2";
     static char MouseErrMsg5[]="ÏÖÔÚÄú¿É°´<Esc>ÍË³ö, ±ðµÄ¼ü½«¼ÌÐø";
     int disp_x=x/2-(strlen(MouseErrMsg2)+4)*ASC16WIDTH/2;
     int disp_y=y/2-10;

     DisplayString(MouseErrMsg1,disp_x+4*ASC16WIDTH,disp_y-3*ASC16HIGHT-4,
               EGA_WHITE,EGA_BLACK);
     DisplayString(MouseErrMsg2,disp_x,disp_y-2*ASC16HIGHT,
               EGA_WHITE,EGA_BLACK);
     DisplayString(MouseErrMsg3,disp_x,disp_y-1*ASC16HIGHT+4,
               EGA_WHITE,EGA_BLACK);
     DisplayString(MouseErrMsg4,disp_x,disp_y-0*ASC16HIGHT+8,
               EGA_WHITE,EGA_BLACK);
     DisplayString(MouseErrMsg5,disp_x,disp_y+1*ASC16HIGHT+20,
               EGA_WHITE,EGA_BLACK);
     fflush(stdout);
     Result=getch();
     if(Result==0) getch();
     else if(Result==ESC) return(-1);

     while (kbhit()) getch();
   #endif
  }

 #ifdef NOT_USED
  if (EMMTest())
    {
        if (EMMMessage()<0) return -1;
    }
 #endif

/*--------
  if ((Result=KeySpeed())<0)
     return(Result);
----*/
  PageInitial();

  if ((Result=FontInitial())<0)
     return(Result);

  InitCache();
  init_paper();             //By zjh

  GetFaxConfig();
  ReturnOK();
}

int SystemDestruct(void)
{
  save_paper();             //By zjh

  SetIntSign();         /* cli:: disable creat message */
  TimerEnd();
  FontEnd();
  CloseCache();
  PageFinish();
  MouseDestruct();
  UnlockMouseMemory();

  WindowEnd();
  ItemFinish();
  ChineseLibDone();
  GraphFinish();
  HandleFinish();

  WriteDefaultScreenMode();
  ReturnOK();
}

int main(int argc,char *argv[])
{
  int i,Result;      //MessageNumber;
  HWND Window;
  HMSG Message;
  ULONG Param1,Param2;
  char *opt;

  GetProfileString( ProfileName,InitSection, VectLibPathEntry,VectLibPath,
       "c:\\ezp\\fonts\\;c:\\;d:\\;e:\\;d:\\bttf;e:\\bttf;f:\\bttf;g:\\bttf");
  GetProfileString( ProfileName,InitSection, TrueTypeLibPathEntry,TrueTypeLibPath,
       "c:\\ezp\\fonts\\;d:\\cttf;e:\\cttf;f:\\cttf;g:\\cttf;d:\ttf;e:\\ttf;f:\\ttf;g:\\ttf");

  for (i=1;i<argc;i++)
  {
      opt = argv[i];
      if (*opt == '-'|| *opt == '/')
      {
          opt ++;
          switch (toupper(*opt)) {
              case '6':
                   if(!strcmp(opt,"640"))
                      ScreenMode=MODE640X480X16;
                   break;
              case '8':
                   if(!strcmp(opt,"800"))
                      ScreenMode=MODE800X600X16;
                   break;
              case '1':
                   if(!strcmp(opt,"1024"))
                      ScreenMode=MODE1024X768X16;
                   break;
              case 'E':
                   fEditor=TRUE;
                   break;
              case 'D':
              default:break;
          }     // switch
      } else
      if (*opt == '%')
      {
          opt ++;
          if(*opt=='%')
             Result=60000;
      }
      else
          MakeName(opt);
  } /*-- argc > 1 ---*/

  InitDot13LIB();

  i=Result;
  if ((Result=SystemConstruct())<OpOK)
  {
     SystemDestruct();
     Error(Result);
  }

  Result=i;
  FileSetMeterCM();
  WaitMessageEmpty();           // run all init, ByHance

/*-------------
//  test_speed();
  printf("size of Box = %d\n",sizeof(Boxs));
  printf("size of Pages = %d\n",sizeof(Pages));
  printf("size of window= %d\n",sizeof(Windows));
  printf("size of fillp = %d\n",sizeof(FILLP));
  printf("size of fillp = %d\n",sizeof(FILLP));
  getch();
  goto game_over;
-----------------*/

  if(!fEditor)
  {     // get printer type from file <EZP.INI>
     char str[80];
     GetProfileString( ProfileName,InitSection, PrinterEntry,str,
                        "HPÏµÁÐ¼¤¹â´òÓ¡»ú(A4 300DPI)");
     i=0;
     while(PrinterName[i])
     {
       if( strcmp(str,PrinterName[i])==0 )
         break;
       i++;
     }
     if(PrinterName[i]==NULL) i=0;
     CurrentPrinter=i;

     MessageInsert(1,MENUCOMMAND,MENU_VIEWTOOLS,0L);
     MessageInsert(1,MENUCOMMAND,MENU_CLIBRATION,0L);
     strcpy(DefaultFileName,"*.EZP;*.FRA;*.TXT;*.*");
     WaitMessageEmpty();           // run all init, ByHance
  }
  else
  {
     strcpy(DefaultFileName,"*.TXT");
     GlobalBoxTool=IDX_INPUTBOX;
     DEFAULTTYPESTYLE.CharSize=DEFAULTTYPESTYLE.CharHSize=
       FormattingTextStyle.CharSize=FormattingTextStyle.CharHSize=160;  // 16*16
     DEFAULTTYPESTYLE.ParagraphAlign=
       FormattingTextStyle.ParagraphAlign=ALIGNLEFT;
     DEFAULTTYPESTYLE.VParagraphAlign=
       FormattingTextStyle.VParagraphAlign=ALIGNUPDOWN;
  }



  if(fAutoLoad)
     MessageInsert(1,MENUCOMMAND,MENU_OPEN,0L);
  else
     //if(fEditor) MessageInsert(1,MENUCOMMAND,MENU_NEW,0L);
  {
  MessageInsert(1,MENUCOMMAND,MENU_NEW,0L);    //By zjh 96.10.17 for auto load
  fNewA4=TRUE;
  }

// #define OLD_VERSION
#ifdef OLD_VERSION
  DrawIconImage();    // for debug
#endif

/*----------------- for test -----
  WaitMessageEmpty();
  for(i=0;i<200;i++)
  {
    //RedrawUserField();
    //WaitMessageEmpty();
    TextBoxRedrawPart(GlobalBoxHeadHandle,0,30000,0,FALSE);
  }
  goto game_over;
  -----------------*/

  GlobalTimer=TimerInsert(1,1);   // in main window, 18.2/second

  while(1)
  {
    if(Result==60000) {
        if(0==strcmp(&argv[1][2],"Han&Zhou"))
        {
           static unsigned char REDTEK_Company[32]={
              '±'+3,'±'+2,
              '¾'+1,'©'+0,
              'À'-1, 'í'-2,
              'µ'-3, 'Â'-4,
              'É'-5, 'Ì'-6,
              'Ó'-7, 'Ã'-8,
              '¼'-9, '¼'-10,
              'Ê'-11,'õ'-12,
              'Ó'-13,'Ð'-14,
              'Ï'-15,'Þ'-16,
              '¹'-17,'«'-18,
              'Ë'-19,'¾'-20,
              '°'-21,'æ'-22,
              'È'-23,'¨'-24,
              'Ë'-25,'ù'-26,
              'Ó'-27,'Ð'-28
           };

           Result=0;
           while(PageDialog[Result].ItemProcedure!=PageInitialBoxProcedure)
                Result++;
           PageDialog[Result].ItemUserData=1;       // pageInitBox=TRUE;

           //UserMenuCommand(1,MENUCOMMAND,MENU_NEW,0L);
           MenuHotKeyToMessage(1,ALT_F);        // ByHance
           MenuDefaultProc(1,KEYDOWN,'N',0);   // in UserMenu.C
           //MessageCreatbyKeyboard(ENTER,0);
           *(short *)0x41a=0x1e;
           *(short *)0x41c=0x20;
           *(short *)0x41e=0x1c0d;

           WaitMessageEmpty();
           for(Result=0;Result<32;Result++)
                REDTEK_Company[Result]+=(Result-3)&0xff;
           MessageCreatbyKeyString(REDTEK_Company,32);
        }
    }
    else

    KeyToMessage();
    if ((Result=MessageGet(&Window,&Message,&Param1,&Param2))<OpOK)
       break;
    if (Result<MAXMESSAGES)
    {
       if ((Result=MessageGo(Window,Message,Param1,Param2))==SYSTEMQUIT)
          break;
    }
    else
       MessageGo(1,SYSTEMIDLE,Param1,Param2);           // main window idle
  }

  TimerDelete(GlobalTimer);

//game_over:
#ifdef WELCOME_DISPLAY
  MessageNumber=Result;

  if ((Result=SystemDestruct())<OpOK)
  {
     SystemDealError(Result);
  }
  SystemDealError(MessageNumber);
#else
  SystemDestruct();
#endif

 //printf("OK.\n");
 return(0);
}


