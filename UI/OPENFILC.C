/*-------------------------------------------------------------------
* Name: openfilc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"
// #include "dbf.h"
// #include "express.h"

#define DISKCHANGE    1201

static int first,bExtMustChange;

void name_ext( char *name, char *new_ext)
{
   int i,len;
   len=strlen(name);
   for (i=len-1;i>0;i--) if (name[i]=='.') break;
   if (name[i]=='.') name[i]=0;
   strcat(name,".");
   strcat(name,new_ext);
   return;
}

int __far handler(unsigned deverr, unsigned errval, unsigned far *devhdr)
{
     return(_HARDERR_IGNORE);
//   _hardretn(1);
}

static unsigned long FileNameList(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist;

  switch (Message)
  {
    case DIRCHANGE:
         MessageGo(Window,WINDOWINIT,0,0);
         hlist = WindowList(Window);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         break;

    case ITEMSELECT:
         MessageGo(WindowGetFather(Window),FILECHANGE,Param1,Param2);
         break;

    case LISTBOXCONFIRM:
         hlist=WindowGetUserData(Window);
         strcpy(NowOpenFile->filename,ListGetItem(hlist,ListGetCurrent(hlist)));
         MessageGo(WindowGetFather(Window),DIALOGBOXOK,0l,0l);
         break;

    case WINDOWINIT:
         {
           char  *filename=NowOpenFile->filename;
           char  *dirs=NowOpenFile->dirs;
           char  *drive=NowOpenFile->drive;
           char  fn[128];
           int   r;

           #ifdef _TURBOC_
              struct ffblk opffblk;
           #else
              struct find_t opffblk;
           #endif

           MessageGo(Window,LISTSETITEMHEIGHT,16,0);
           MessageGo(Window,LISTSETITEMLENGTH,13,0);

           /*------ ByHance, 96,3.25 ----
           hlist = WindowList(Window);
           ListSetTotal(hlist,0);
           ListSetCurrent(hlist,0);
           ListSetTop(hlist,0);
           --------------------*/
           MessageGo(Window,LISTDELETEALL,0L,0L);

     //      _harderr(handler);

           if( !dirs[0] ) {
              #ifdef __TURBOC__
                 getcurdir((int)(*drive-'A'+1), dirs);
              #else
                 unsigned total,old;
                 _dos_getdrive(&old);
                 if(old!=*drive-'A'+1)
                    _dos_setdrive( *drive-'A'+1, &total );
                 getcwd(dirs, 64);
                 memmove(dirs,dirs+3,61);       // cancel "C:\"
                 if(!dirs[0])
                     strcpy(dirs,"\\");
                 if(old!=*drive-'A'+1)
                    _dos_setdrive( old, &total );
              #endif
           }

           memset(&opffblk, 0, sizeof(opffblk));

           strcpy(fn,drive);
           //strcat(fn,"\\");
           //if( strcmp(dirs,"\\") && strcmp(dirs,"/") )
           //{
           //    strcat(fn,dirs);
           //    strcat(fn,"\\");
           //}
           if(dirs[0])
           {
                  char ch;
                  int len;

                  ch=dirs[0];
                  if(ch!='\\' && ch!='/')
                       strcat(fn,"\\");

                  strcat(fn,dirs);
                  len=strlen(fn);
                  ch=fn[len-1];
                  if(ch!='\\' && ch!='/')
                       strcat(fn,"\\");
           }

           strcat(fn,filename);

           r = findfirst(fn, &opffblk, _A_NORMAL);

           if (!r)              // found at least 1 file
              strcpy(NowOpenFile->filename, opffblk.ff_name);

           while (!r)
           {
              MessageGo(Window, LISTINSERTITEMSORTED, FP2LONG(opffblk.ff_name), 0L);
              r = findnext(&opffblk);
           }

           //MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
           //   ByHance, 95,12.11
             MessageInsert(Window,REDRAWMESSAGE,0L,
               MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );
         }
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long DirectoryList(HWND Window,HMSG Message,long Param1,long Param2)
{
  int hlist;

  switch (Message)
  {
    case WINDOWINIT:
         {
           //int   i,j;
           char  *dirs=NowOpenFile->dirs;
           char  *drive=NowOpenFile->drive;
           #ifdef _TURBOC_
              struct ffblk opffblk;
           #else
              struct find_t opffblk;
           #endif

           char  fn[128];
           int   r;

           MessageGo(Window,LISTSETITEMHEIGHT,16,0);
           MessageGo(Window,LISTSETITEMLENGTH,13,0);

           /*------ ByHance, 96,3.25 ----
           hlist = WindowList(Window);
           ListSetTotal(hlist,0);
           ListSetCurrent(hlist,0);
           ListSetTop(hlist,0);
           --------------------*/
           MessageGo(Window,LISTDELETEALL,0L,0L);

//           _harderr(handler);
           if( ! drive[0] )
            {
 //             *drive     = (char)getdisk()+'A';
             *drive     = (char)getdisk()+'A'-1;
             *(drive+1) = ':';
             *(drive+2) = 0;
            }

           if( !dirs[0] ) {
              #ifdef __TURBOC__
                 getcurdir((int)(*drive-'A'+1), dirs);
              #else
                 unsigned total,old;
                 _dos_getdrive(&old);
                 if(old!=*drive-'A'+1)
                    _dos_setdrive( *drive-'A'+1 , &total );
                 getcwd(dirs, 64);
                 memmove(dirs,dirs+3,61);       // cancel "C:\"
                 if(!dirs[0])
                     strcpy(dirs,"\\");
                 if(old!=*drive-'A'+1)
                    _dos_setdrive( old, &total );
              #endif
           }

           memset(&opffblk, 0, sizeof(opffblk));
           strcpy(fn,drive);

           //if( strcmp(dirs,"\\") && strcmp(dirs,"/") )
           //{
           //    strcat(fn,dirs);
           //    strcat(fn,"\\");
           //}
           if(dirs[0])
           {
                  char ch;
                  int len;

                  ch=dirs[0];
                  if(ch!='\\' && ch!='/')
                       strcat(fn,"\\");

                  strcat(fn,dirs);
                  len=strlen(fn);
                  ch=fn[len-1];
                  if(ch!='\\' && ch!='/')
                       strcat(fn,"\\");
           }

           strcat(fn,"*.*");

           r = findfirst(fn, &opffblk, FA_DIREC);
           while (!r)
           {
            if((opffblk.ff_attrib & FA_DIREC) && strcmp(opffblk.ff_name,"."))
              MessageGo(Window, LISTINSERTITEMSORTED, FP2LONG(opffblk.ff_name), 0L);
            r = findnext(&opffblk);
           }

           //MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
           //   ByHance, 95,12.11
           MessageInsert(Window,REDRAWMESSAGE,0L,
               MAKELONG(WindowGetWidth(Window),WindowGetHeight(Window)) );

           {    /*--- display directory's name ----*/
              #define max_n  (304/ASC16WIDTH)
                 int x,y,w,h;
                 unsigned total,old;
                 int len;
                 int SaveColor;
                 struct viewporttype TmpViewPort;
                 char disk[20],dirs[64],file[14],ext[5];

                 strupr(fn);
                 _splitpath(fn,disk,dirs,file,ext);

                 _dos_getdrive(&old);
                 if(old!=disk[0]-'A'+1)
                    _dos_setdrive( disk[0]-'A'+1 , &total );
                 len=strlen(dirs);
                 if(len>1)
                 {
                   char ch=dirs[len-1];
                   if(ch=='\\' || ch=='/') dirs[len-1]=0;
                 }

                 chdir(dirs);
                 getcwd(dirs, 64);
                 _dos_setdrive( old, &total );

                 MouseHidden();
                 SaveColor=getcolor();
                 getviewsettings(&TmpViewPort);
                 setviewport(0,0,getmaxx(),getmaxy(),1);

                 WindowGetRealRect(Window,&x,&y,&w,&h);
                 y-=26;

                 len=strlen(dirs);
                 if(len>max_n)
                 {
                    int i;
                    i=len-1;
                    while(dirs[i]!='\\' && dirs[i]!='/' && i>max_n-12) i--;
                    strcpy(dirs,"...");
                    strcat(dirs,&dirs[i]);
                 }

                 // WaitMessageEmpty();
                 setfillstyle(1,EGA_LIGHTGRAY);
                 bar(x,y,x+304,y+25);            /*--- clear old area --*/
                 DisplayString(dirs,x,y,EGA_BLACK,EGA_LIGHTGRAY);

                 setviewport(TmpViewPort.left,TmpViewPort.top,TmpViewPort.right,
                             TmpViewPort.bottom,TmpViewPort.clip);
                 setcolor(SaveColor);
                 MouseShow();
              #undef max_n
            }
         }
         break;

    case LISTBOXCONFIRM:{
         char dir[20];
         char path[40];

         hlist = WindowList(Window);
         strcpy(dir,ListGetItem(hlist,ListGetCurrent(hlist)));
         strcpy(path,NowOpenFile->drive);
         strcat(path,dir);
         chdir(path);
         #ifdef __TURBOC__
            getcurdir(path[0]-'A'+1,NowOpenFile->dirs);
         #else
         {
            unsigned total,old;
            _dos_getdrive(&old);
            if(old!=path[0]-'A'+1)
               _dos_setdrive( path[0]-'A'+1 , &total );
            getcwd(NowOpenFile->dirs, 64);
            memmove(NowOpenFile->dirs,NowOpenFile->dirs+3,61); // cancel "C:\"
            if(old!=path[0]-'A'+1)
               _dos_setdrive( old, &total );
         }
         #endif

         MessageGo(Window,WINDOWINIT,0,0);
         MessageGo(Window,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
         ListSetTop(hlist,0);
         ListSetCurrent(hlist,0);
         MessageGo(WindowGetFather(Window),DIRCHANGE,0L,0L);
         }
         break;
    default:
         return(ListBoxDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long WildCardList(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           char  *wd=NowOpenFile->wildcard;
           char  wid[10];

           MessageGo(Window,LISTSETITEMHEIGHT,16,0);
           MessageGo(Window,LISTSETITEMLENGTH,12,0);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(wd),0L);
           while(*wd)
           {
             strcpy(wid,wd);
             MessageGo(Window,LISTAPPENDITEM,FP2LONG(wid),0L);
             while(*wd) wd++;
             wd++;              // point to next wildcast name:
                                // exp.  *.tif;*.bmp;*.pcx
           }
         }
         break;
    case ITEMSELECT:
         {
           HWND MidWin,hlist;

          // ComboDefaultProcedure(Window,COMBOPULL,0l,0l);   //ByHance,96,1.23
           ComboDefaultProcedure(Window,Message,Param1,Param2);
           MidWin = ComboFindListBox(Window);
           hlist = WindowList(MidWin);
           strcpy(NowOpenFile->wildcard,ListGetItem(hlist,ListGetCurrent(hlist)));

           MidWin = WindowGetFather(Window); ///Dialog
           MessageGo(MidWin,DIRCHANGE,0l,0l);
         }
         break;
    default:
         return(ComboDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long DriverList(HWND Window,HMSG Message,long Param1,long Param2)
{
  switch (Message)
  {
    case WINDOWINIT:
         {
           unsigned save,disk,total,UnusedDisk;
           char diskn[3];
           char *dri=diskn;

           diskn[1] = ':';
           diskn[2] = '\0';

           MessageGo(Window,LISTSETITEMHEIGHT,16,0);
           MessageGo(Window,LISTSETITEMLENGTH,3,0);

               /* save original drive */
           save = getdisk();
//           diskn[0] = save+'A';
           diskn[0] = save+'A'-1;
//           _harderr(handler);
           MessageGo(Window,SETLINEBUFFER,FP2LONG(dri),0L);
           MessageGo(Window,LISTDELETEALL,0L,0L);

       /*------------
           for (disk = 1;disk < 26;++disk) {
                 setdisk(disk);
                 if (disk == getdisk()) {
                      setdisk(save);
       ------------*/

           disk = 1;            // from diskette A:
           UnusedDisk=0;
           {                         // ByHance, for diskette error
                 char *p=(char *)0x410;           // equipment list
                 total=*p;
                 if(total&1) {          // diskette present, count how many
                    total=(total>>6)&0x3;
                    if(!total) UnusedDisk=2;    // if only 1, can not use B:
                 } else disk=3;         // no diskette present, so, from C:
           }
           for (; disk<26; ++disk) {
                 if(disk==UnusedDisk) continue;
                 _dos_setdrive(disk,&total);
                 if (disk == getdisk()) {
                      _dos_setdrive(save,&total);
                      if(first && disk==save) {           // ByHance
                          HWND MidWindow=ComboFindListBox(Window);
                          if (MidWindow)
                               ListSetCurrent(WindowList(MidWindow),
                                        ListGetTotal(WindowList(MidWindow)) );
                          first=0;
                      }
                      diskn[0] = disk+'A'-1;
                      MessageGo(Window,LISTAPPENDITEM,FP2LONG(dri),0L);
                 }
           }

       /*---------
           setdisk(save);
           diskn[0] = save+'A';
        ----------*/
           _dos_setdrive(save,&total);
           diskn[0] = save+'A'-1;

           strcpy(NowOpenFile->drive,diskn);
         }
         break;
  /*-----------
    case TAB:
         ComboDefaultProcedure(Window,LOSTFOCUS,0l,0l);
         WindowTableOrderNext(Window);
         break;
    case SHIFT_TAB:
         ComboDefaultProcedure(Window,LOSTFOCUS,0l,0l);
         WindowTableOrderPrev(Window);
         break;
  ---------*/
    case ITEMSELECT:
         {
           HWND MidWin,hlist;

          // ComboDefaultProcedure(Window,COMBOPULL,0l,0l);   //ByHance,96,1.23
           ComboDefaultProcedure(Window,Message,Param1,Param2);
           MidWin = ComboFindListBox(Window);
           hlist = WindowList(MidWin);
           strcpy(NowOpenFile->drive,ListGetItem(hlist,ListGetCurrent(hlist)));

           MidWin = WindowGetFather(Window); ///Dialog
           strcpy(NowOpenFile->dirs,"");
           MessageGo(MidWin,DISKCHANGE,0l,0l);
         }
         break;
    default:
         return(ComboDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long FileNameInput(HWND Window,HMSG Message,
                      long Param1,long Param2)
{
  HWND MidWin;
  char  *p;
  char disk[20],dirs[64],file[14],ext[5];
  char filename[128];  // =NowOpenFile->filename;
  int  len;
  int Result;

  switch (Message)
  {
    case WINDOWINIT:
         MessageGo(Window,SETLINEBUFFER,FP2LONG(NowOpenFile->wildcard),0L);
         break;
    case DIALOGBOXOK:
         MidWin = WindowGetFather(Window);

         strcpy(filename,(char *)LONG2FP(MessageGo(Window,GETLINEBUFFER,0L,0L)));
         _splitpath(filename,disk,dirs,file,ext);
         strupr(disk);   strupr(dirs);   strupr(file);   strupr(ext);

         strcpy(NowOpenFile->filename,file);
         if(ext)
         {
            strcat(NowOpenFile->filename,ext);
            p=strchr(NowOpenFile->wildcard,'.');
            if(p) *p=0;
            strcat(NowOpenFile->wildcard,ext);
         }

         p=NowOpenFile->filename;
         while (*p) {
           if(*p=='*' || *p=='?') break;
           p++;
         }

         len=strlen(dirs);
         if(len>1 && (dirs[len-1]=='\\' || dirs[len-1]=='/') )
             dirs[len-1]=0;

         if(disk[0]!=0 && disk[0]!=NowOpenFile->drive[0])
         {
               NowOpenFile->drive[0]=disk[0];
               strcpy(NowOpenFile->dirs,dirs);
               if(*p)
               {
                   MessageGo(MidWin,DISKCHANGE,0l,0l);
                   return FALSE;
               }
         }
         if(dirs[0]!=0 && strcmp(dirs,NowOpenFile->dirs) )
         {
               strcpy(NowOpenFile->dirs,dirs);
               if(*p)
               {
                   // MessageGo(MidWin,DIRCHANGE,0l,0l);
                   MessageGo(MidWin,DISKCHANGE,0l,0l);
                   return FALSE;
               }
         }

         if(*p)         // wildcase
         {
           MidWin = WindowGetChild(MidWin);
           while (MidWin) {
                if (WindowGetProcedure(MidWin)==FileNameList)
                {
                        int hlist = WindowList(MidWin);
                        MessageInsert(MidWin,WINDOWINIT,0,0);
                        MessageInsert(MidWin,WMPAINT,0,
                             ListGetHeight(hlist)*CHARHEIGHT);
                        break;
                }
                MidWin=WindowGetNext(MidWin);
           }
           return FALSE;
         }
         else           // no wildcase in file name
         {
              strcpy(filename,NowOpenFile->drive);
              if (NowOpenFile->dirs[0])
              {
                  char ch;
                  ch=NowOpenFile->dirs[0];
                  if(ch!='\\' && ch!='/')
                       strcat(filename,"\\");

                  strcat(filename,NowOpenFile->dirs);
                  len=strlen(filename);
                  ch=filename[len-1];
                  if(ch!='\\' && ch!='/')
                       strcat(filename,"\\");
              }
              strcat(filename,NowOpenFile->filename);

              if(bExtMustChange)
                   filename_cat(filename,FILE_EXT_NAME);

       //          _harderr(handler);
              Result=access(filename,F_OK);   // if exist, return 0, else, -1

              if(NowOpenFile->IsNewFileName)    // save file
              {
                 if(Result==0)
                 {      // overwrite it ?
                   Result=MessageBox(GetTitleString(ERRORINFORM),
                            "文件已存在,覆盖此文件?",
                            2,Window);
                   if(Result==1)        // cancel
                      return FALSE;
                 }
              }
              else            // open file, so, the file must be exist
              if(Result!=0)     // no file exist
              {
                 MessageBox(GetTitleString(ERRORINFORM),
                            GetInformString(FILENAMEERROR),1,
                            Window);
                 // Alarm();
                 return(FALSE);
              }
         }
         break;
    default:
         return(SingleLineEditorDefaultProcedure(Window,Message,Param1,Param2));
  }
  return(TRUE);
}

static unsigned long OpenFile(HWND Window,HMSG Message,long Param1,long Param2)
{
  HWND MidWin;
  int  hlist;

  switch(Message)
  {
    case DIRCHANGE:
         MidWin=WindowGetChild(Window);
         while (MidWin) {
            if (WindowGetProcedure(MidWin)==FileNameList){
                    MessageInsert(MidWin,DIRCHANGE,0,0);
                    break;
            }
            MidWin=WindowGetNext(MidWin);
         }

         MidWin=WindowGetChild(Window);
         while (MidWin) {
            if (WindowGetProcedure(MidWin)==FileNameInput)
            {   int hlist;
                    MessageInsert(MidWin,WINDOWINIT,0,0);
                    hlist = WindowList(MidWin);
                    MessageInsert(MidWin,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
                    MessageInsert(MidWin,DIALOGBOXOK,0,0);
                    break;
            }
            MidWin=WindowGetNext(MidWin);
         }
         break;
    case FILECHANGE:
         MidWin=WindowGetChild(Window);
         while (MidWin) {
            if (WindowGetProcedure(MidWin)==FileNameInput)
            {
                   hlist = WindowList(Param2);
                   if(ListGetTotal(hlist))              // ByHance
                   {
                      MessageInsert(MidWin, SETLINEBUFFER,
                          FP2LONG(ListGetItem(hlist,Param1)), 0L);
                      MessageInsert(MidWin,WMPAINT,0l,GetEditorWidth(WindowGetUserData(MidWin)));
                   }
                   break;
            }
            MidWin=WindowGetNext(MidWin);
         }
         break;
    case DRAWWINDOW:
         DialogDefaultProcedure(Window, Message, Param1, Param2);
    case DISKCHANGE:
         MidWin=WindowGetChild(Window);
         while (MidWin) {
            if (WindowGetProcedure(MidWin)==DirectoryList)
            {
                    MessageInsert(MidWin,WINDOWINIT,0,0);
                    hlist = WindowList(MidWin);
                    MessageInsert(MidWin,WMPAINT,0,ListGetHeight(hlist)*CHARHEIGHT);
                    break;
            }
            MidWin=WindowGetNext(MidWin);
         }
         MessageInsert(Window,DIRCHANGE,0,0);
         break;
    default:
       return(DialogDefaultProcedure(Window, Message, Param1, Param2));
  }
  return(TRUE);
}

static Dialogs OpenFileDialog[]=
{
  { GLOBALITEM, 11 , 0, 0, 538, 296, 0, OpenFile,"打开文件" },

   //  { STATICTEXTITEM, 0, 1120, 93, 240, 113, 0, NULL, "文件:" },

  { STATICTEXTITEM, 0, 16, 36, 196, 55, 0, NULL, "文件名:" },
  { SINGLELINEEDITORITEM, 0, 16, 56, 196, 75, 0, FileNameInput, "" },
  { LISTBOXITEM, 0, 16, 87, 196, 231, 0, FileNameList, "" },

  { STATICTEXTITEM, 0,225, 36, 408, 55, 0, NULL, "目录:" },
  { LISTBOXITEM, 0, 225, 87, 408, 231, 0, DirectoryList, "" },

    //  { FRAMEITEM, 1, 20, 300, 240, 350, 0, NULL, "文件类别" },
  { STATICTEXTITEM, 0, 16, 236,196,255, 0, NULL, "列出下列类型文件" },
  { COMBOXITEM, 0, 16, 256, 196, 394, 0, WildCardList, "" },

    //  { FRAMEITEM, 1, 245, 300, 380, 350, 0, NULL, "驱动器" },
  { STATICTEXTITEM, 0, 225, 236, 408, 255, 0, NULL, "驱动器" },
  { COMBOXITEM, 0, 225, 256, 408, 394, 0, DriverList, "" },

  { OKBUTTON,    0, 421, 200, 520, 227, 0, NULL, "确定" },
  { CANCELBUTTON,0, 421, 236, 520, 263, 0, NULL, "取消" },
};

static char *filetitle[]={
   "打开轻松排版文件",
   "保存轻松排版文件",
   "换名保存轻松排版文件",
   "存成文本文件",
   "插入文本文件",
   "插入图像文件",
   "打印到磁盘文件",
   "注入数据库文件",
   "打开打印版式文件",
   "保存打印版式文件",
   "选择要传送的文件",
};

static char *edit_filetitle[]={
   "打开文本文件",
   "保存文本文件",
   "换名保存文本文件",
};

int GetFileName(HWND Window,char *WildCardTable,char *ReturnFileName,
                char IsNewFileSign,int titletype)
{
  int Result;
  char *Tmp;
  OpenFileStruct TmpFileOpenStruct;
  char path[280],mypath[280];
  int drive;

//titletype==OPENFILE_TITLE||titletype==SAVEFILE_TITLE||titletype==SAVEAS_TITLE
  if(fEditor && titletype<3)
     strcpy(OpenFileDialog[0].ItemTitle,edit_filetitle[titletype]);
  else
     strcpy(OpenFileDialog[0].ItemTitle,filetitle[titletype]);

//titletype==OPENFILE_TITLE||titletype==SAVEFILE_TITLE||titletype==SAVEAS_TITLE
  bExtMustChange=(!fEditor)&&(titletype<3);

  _harderr(handler);
/*
  drive = _getdrive();
  #ifdef __TURBOC__
     getcurdir(drive,path);
  #else
     getcwd(path, 80);
     memmove(path,path+3,77);       // cancel "C:\"
  #endif
*/

  getcwd(mypath, 80);
  GetProfileString(ProfileName,FilePathSection, LastFilePathEntry,path, mypath);

  drive=path[0]-'A'+1;
  memmove(path,path+3,77);       // cancel "C:\"

  { unsigned total;
    _dos_setdrive(drive,&total);
  }

  chdir("\\");
  chdir(path);

  first=1;
  NowOpenFile=&TmpFileOpenStruct;
  memset(NowOpenFile,0,sizeof(OpenFileStruct));
  strcpy(NowOpenFile->wildcard,WildCardTable);
  NowOpenFile->drive[0] = drive+'A'-1;
  NowOpenFile->drive[1] = ':';

  for (Tmp=NowOpenFile->wildcard;*Tmp;Tmp++)
      if ((*Tmp)==';')
         (*Tmp)=0;

  strcpy(NowOpenFile->filename,NowOpenFile->wildcard);
  NowOpenFile->IsNewFileName=IsNewFileSign;

  Result=MakeDialogBox(Window,OpenFileDialog);

  strcpy(path,mypath);

  if (!Result) {
     strcpy(ReturnFileName,NowOpenFile->drive);
     if (NowOpenFile->dirs[0]) {
       char ch;
       ch=NowOpenFile->dirs[0];
       if(ch!='\\' && ch!='/')
          strcat(ReturnFileName,"\\");

       strcat(ReturnFileName,NowOpenFile->dirs);
       ch=ReturnFileName[strlen(ReturnFileName)-1];
       if(ch!='\\' && ch!='/')
          strcat(ReturnFileName,"\\");

       strcpy(mypath,ReturnFileName);
       ch=strlen(mypath);
       if (mypath[ch-1]=='\\') mypath[ch-1]=0;
       SetProfileString(ProfileName,FilePathSection, LastFilePathEntry,mypath);
     }

     strcat(ReturnFileName,NowOpenFile->filename);
     if(bExtMustChange)
          filename_cat(ReturnFileName,FILE_EXT_NAME);
  }

//  _chdrive(drive);

  drive=path[0]-'A'+1;            //By zjh
  memmove(path,path+3,77);      //By zjh

  { unsigned total;
    _dos_setdrive(drive,&total);
  }

  chdir("\\");
  chdir(path);

  return(Result);
}


