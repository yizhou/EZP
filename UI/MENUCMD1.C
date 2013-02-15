/*-------------------------------------------------------------------
* Name: menucmd1.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

extern int tmp_range;
extern int tmp_fileflag;

#define DPMI_INT        0x31
static void MemInfoDisp()
{
    union REGS regs;
    struct SREGS sregs;
    char dat[600],s[100];
   struct meminfo {
       unsigned LargestBlockAvail;
       unsigned MaxUnlockedPage;
       unsigned LargestLockablePage;
       unsigned LinAddrSpace;
       unsigned NumFreePagesAvail;
       unsigned NumPhysicalPagesFree;
       unsigned TotalPhysicalPages;
       unsigned FreeLinAddrSpace;
       unsigned SizeOfPageFile;
       unsigned Reserved[3];
   } MemInfo;

    regs.x.eax = 0x00000500;
    memset( &sregs, 0, sizeof(sregs) );
    sregs.es = FP_SEG( &MemInfo );
    regs.x.edi = FP_OFF( &MemInfo );

    int386x( DPMI_INT, &regs, &regs, &sregs );

    sprintf(s, "  ×î´ó¿ÉÓÃÄÚ´æ×Ö½Ú: %lu\n", MemInfo.LargestBlockAvail );
    strcpy(dat,s);

    sprintf(s, "      ×î´ó·ÖÅäÒ³Ãæ: %lu\n", MemInfo.MaxUnlockedPage );
    strcat(dat,s);

    sprintf(s, "¿É¹©·ÖÅäºÍ¼ÓËøÒ³Ãæ: %lu\n",MemInfo.LargestLockablePage );
    strcat(dat,s);

    sprintf(s, "      ÏßÐÔ¿Õ¼ä×ÜÒ³: %lu\n", MemInfo.LinAddrSpace );
    strcat(dat,s);

    sprintf(s, "¿É¹©Ê¹ÓÃµÄ×ÔÓÉÒ³Ãæ: %lu\n",MemInfo.NumFreePagesAvail );
    strcat(dat,s);

    sprintf(s, "  ¿ÉÊ¹ÓÃµÄÎïÀíÒ³Ãæ: %lu\n", MemInfo.NumPhysicalPagesFree );
    strcat(dat,s);

    sprintf(s, "      ×î´óÎïÀíÒ³Ãæ: %lu\n", MemInfo.TotalPhysicalPages );
    strcat(dat,s);

    sprintf(s, "  ×ÔÓÉÏßÐÔµØÖ·Ò³Ãæ: %lu\n", MemInfo.FreeLinAddrSpace );
    strcat(dat,s);

    sprintf(s, "      ÎÄ¼þÒ³±í´óÐ¡: %lu\n", MemInfo.SizeOfPageFile );
    strcat(dat,s);
    MessageBox("ÄÚ´æÊ¹ÓÃ×´¿ö",dat,1,0);
}

#ifdef NOT_USED
static int mycopy(char *nas,char *nad)
{
  FILE *fp,*fp1;
  unsigned char buff[1030];
  int len=1024;
  fp=fopen(nas,"rb");
  if (fp==NULL) return -1;
  fp1=fopen(nad,"wb");
  if (fp1==NULL)
  {
   fclose(fp);
   return -1;
  }
  while (len==1024&&(!feof(fp)))
  {
    len=fread(buff,1,1024,fp);
    if (len) fwrite(buff,1,len,fp1);
  }
  fclose(fp);
  fclose(fp1);
  return 0;
}
#endif

long UserMenuCommand(HWND Window,HMSG Message,long Param1)
{
  int Result;
  int tmpColor,tmpCharSize;
  float pointn;
  int AttributePosition;
  int pos,Tmp;
  int Align,iCell;
  char TmpString[500];
  int Left,Top,Right,Bottom;
  int PageNumber;
  HPAGE MidHPage;
  Pages *MidPage;
  HBOX NewHBox;
  int SaveUndoNum;
  TextBoxs *BoxPointer;
  static char OpHelpStr[]="Ê¹ÓÃËµÃ÷";
  static char SelectToolStr[]="Ê¹ÓÃ <±à¼©Í¼ÎÄ> ¹¤¾ß,\n²¢ÇÒ";
  static char SelectInputToolStr[]="Ê¹ÓÃ\n  <Éè¼Æ°æÊ½>»ò<±à¼©Í¼ÎÄ>\n¹¤¾ß";
  int CreateFile;
  int reg1,reg2;
  int col1,col2,row1,row2;
  FILE *fp;

  switch(Message)
  {
    case MENUCOMMAND:
         if( Param1>=MENU_ALIGNUPDOWN
         && Param1<=MENU_TABLEDISMERGE)
         {
           if( GlobalTableCell<0 || !BoxIsTableBox(GlobalBoxHeadHandle) )
           {
             strcpy(TmpString,"ÇëÏÈ");

             if(GlobalBoxTool!=IDX_SELECTBOX
             && GlobalBoxTool!=IDX_INPUTBOX)
                strcat(TmpString,SelectInputToolStr);
             strcat(TmpString,
                  "Ñ¡Ôñ±í¸ñ¿ò(¿ÉÔÚ°æ¿òÖÐ\n"
                  "°´Ò»´ÎÊó±ê×ó¼ü,Ê¹Æä±ß¿ò±ä\n"
                  "ÎªÊµÏßºÚ¿ò),ÔÙÊ¹ÓÃ±¾¹¦ÄÜ!");

             MessageBox(OpHelpStr, TmpString, 1,0);
             break;
           }

           if( (Param1==MENU_TABLEMERGE || Param1==MENU_TABLECALCULATE)
           && BoxIsTableBox(GlobalBoxHeadHandle)
           && GlobalTableBlockStart==GlobalTableBlockEnd )
           {       // must define a block
             strcpy(TmpString,"ÇëÏÈ");
             if(GlobalBoxTool!=IDX_INPUTBOX)
                strcat(TmpString,SelectToolStr);
             strcat(TmpString,"ÍÏ¶¯Êó±ê,Ñ¡ÖÐÓû");
             if(Param1==MENU_TABLEMERGE)
                strcat(TmpString,"ºÏ²¢");
             else       // Param1==MENU_TABLECALCULATE
                strcat(TmpString,"¼ÆËã");
             strcat(TmpString,"µÄÐÐ\nºÍÁÐ,ÔÙÊ¹ÓÃ±¾¹¦ÄÜ!");
             MessageBox(OpHelpStr, TmpString, 1,0);
             break;
           }

           if( (Param1==MENU_TABLEDISMERGE || Param1==MENU_TABLESLANTTYPE)
           && BoxIsTableBox(GlobalBoxHeadHandle)
           && GlobalTableBlockStart!=GlobalTableBlockEnd )
           {       // must cancel the block, select a cell
             strcpy(TmpString,"ÇëÏÈ");
             if(GlobalBoxTool!=IDX_INPUTBOX)
                strcat(TmpString,SelectToolStr);
             strcat(TmpString,
                    "È¡Ïû±ê¼Ç¿é,Ñ¡ÖÐÒ»¸ö±íÔª,\nÔÙÊ¹ÓÃ±¾¹¦ÄÜ!");
             MessageBox(OpHelpStr, TmpString, 1,0);
             break;
           }
         }

         if( GlobalBoxHeadHandle<=0
          &&(Param1==MENU_BOX
           ||Param1==MENU_COPY
           ||Param1==MENU_CUT
           ||Param1==MENU_BOXLOCK
           ||Param1==MENU_BOXUNLOCK
           ||Param1==MENU_FORWARD
           ||Param1==MENU_BACKWARD
           ||Param1==MENU_FRONT
           ||Param1==MENU_BACK
           ||Param1==MENU_TOOLROTATE
         )) {           // must select a Box before this opertion
         lbl_err:
             strcpy(TmpString,"ÇëÏÈ");

             if(GlobalBoxTool!=IDX_SELECTBOX
             && GlobalBoxTool!=IDX_INPUTBOX)
                strcat(TmpString,SelectInputToolStr);
             strcat(TmpString,
                  "Ñ¡ÔñÒ»¸ö°æ¿ò(¿ÉÔÚ°æ¿òÖÐ\n"
                  "°´Ò»´ÎÊó±ê×ó¼ü,Ê¹Æä±ß¿ò±äÎª\n"
                  "ÊµÏßºÚ¿ò),ÔÙÊ¹ÓÃ±¾¹¦ÄÜ!");

             MessageBox(OpHelpStr, TmpString, 1,0);
             break;
         }

         if(Param1==MENU_TOOLROTATE && BoxIsTableBox(GlobalBoxHeadHandle))
         {              // ByHance, 96,3.6
             strcpy(TmpString,"±¾°æ±¾±í¸ñ²»ÄÜÐý×ª!");
             MessageBox(OpHelpStr, TmpString, 1,0);
             break;
         }

         if(Param1==MENU_IMPORTTEXT && !BoxIsTextBox(GlobalBoxHeadHandle)
         && !BoxIsPictureBox(GlobalBoxHeadHandle) )
         {
             strcpy(TmpString,"ÇëÏÈ");

             if(GlobalBoxTool!=IDX_SELECTBOX
             && GlobalBoxTool!=IDX_INPUTBOX)
                strcat(TmpString,SelectInputToolStr);
             strcat(TmpString,
                  "Ñ¡ÔñÎÄ±¾»òÍ¼Ïñ°æ¿ò(¿ÉÔÚ\n"
                  "°æ¿òÖÐ°´Ò»´ÎÊó±ê×ó¼ü,Ê¹Æä±ß\n"
                  "¿ò±äÎªÊµÏßºÚ¿ò),ÔÙÊ¹ÓÃ±¾¹¦ÄÜ!");

             MessageBox(OpHelpStr, TmpString, 1,0);
             break;
         }

         if(Param1==MENU_CLEAR)
         if (GlobalBoxHeadHandle<=0 && GlobalGroupGetSign()==0)
             goto lbl_err;

         if(Param1==MENU_COPY||Param1==MENU_CUT)
         if(BoxCanEditable(GlobalBoxHeadHandle) && GlobalBoxTool==IDX_INPUTBOX
           && GlobalTextBlockStart>=GlobalTextBlockEnd)
         {       // must define a block
             MessageBox(OpHelpStr,
                    "ÇëÏÈÓÃ<¿é²Ù×÷>¶¨ÒåÒ»¸öÎÄ±¾¿é,\nÔÙÊ¹ÓÃ±¾¹¦ÄÜ!",
                    1,0);
             break;
         }

         if(Param1==MENU_COPY||Param1==MENU_PASTE||Param1==MENU_CUT)
         if(GlobalBoxTool!=IDX_SELECTBOX && GlobalBoxTool!=IDX_INPUTBOX)
         {       // can only use 2 tools
             MessageBox(OpHelpStr,
                    "ÇëÏÈÊ¹ÓÃ\n <Éè¼Æ°æÊ½>»ò<±à¼©Í¼ÎÄ>¹¤¾ß,\nÔÙÊ¹ÓÃ±¾¹¦ÄÜ!",
                    1,0);
             break;
         }

         if(Param1>=MENU_CHARBLACK && Param1<=MENU_CHARDKGRAY
           && BoxIsPictureBox(GlobalBoxHeadHandle) )
         {
            if(GlobalBoxTool!=IDX_INPUTBOX)
            {
               strcpy(TmpString,"ÇëÏÈ");
               // strcat(TmpString,"Ê¹ÓÃ\n  <±à¼©Í¼ÎÄ>\n¹¤¾ß");
               strcat(TmpString,SelectToolStr);
               strcat(TmpString,
                    "Ñ¡ÔñÍ¼Ïñ°æ¿ò(¿ÉÔÚ°æ¿òÖÐ\n"
                    "°´Ò»´ÎÊó±ê×ó¼ü,Ê¹Æä±ß¿ò±äÎª\n"
                    "ÊµÏßºÚ¿ò),ÔÙÊ¹ÓÃ±¾¹¦ÄÜ!");

               MessageBox(OpHelpStr, TmpString, 1,0);
               break;
            }

            Result=PictureBoxGetPictureColor(GlobalBoxHeadHandle,&Tmp);
            if(Result==-1)
            {           // only BW picture can be colored
               strcpy(TmpString,"Ö»ÄÜ¶ÔºÚ°×Í¼ÏñÉÏÉ«!");
               MessageBox(OpHelpStr, TmpString, 1,0);
               break;
            }
            else
            if(Result==-2)
            {           // load picture first
               strcpy(TmpString,"ÇëÏÈÊ¹ÓÃ\n");
               strcat(TmpString,"  <ÎÄ¼þ>²Ëµ¥ÖÐµÄ'²åÈëÍ¼ÎÄ'\n");
               strcat(TmpString,"¶ÁÈëÒ»¸öÍ¼ÏñÎÄ¼þ(¿ì½Ý¼ü\nÊÇF3),ÔÙÊ¹ÓÃ±¾¹¦ÄÜ!");
               MessageBox(OpHelpStr, TmpString, 1,0);
               break;
            }
            goto  lbl_test_end;
         }

         if(Param1==MENU_FIND||Param1==MENU_REPLACE||Param1==MENU_NEXT
          ||(Param1>=MENU_ALIGNMENTLEFT && Param1<=MENU_ALIGNMENTAVERAGE
              && !BoxIsTableBox(GlobalBoxHeadHandle))
          ||(Param1==MENU_FONT)
          ||(Param1==MENU_GOTOLINE)
          ||(Param1==MENU_CALCULATELINE)
          ||(Param1==MENU_CALCULATEPERSON)
          ||(Param1==MENU_CALCULATEMORE)
          ||(Param1>=MENU_ROWGAP1 && Param1<=MENU_ROWGAPUSER)
          ||(Param1>=MENU_CHARBLACK && Param1<=MENU_CHARDKGRAY)
          ||(Param1>=MENU_CHARNORMAL && Param1<=MENU_UPDOWN)
          ||(Param1>=MENU_FONTSIZE0 && Param1<=MENU_FONTSIZEFREE) )
         if(!BoxCanEditable(GlobalBoxHeadHandle) || GlobalBoxTool!=IDX_INPUTBOX
        // || (BoxIsTableBox(GlobalBoxHeadHandle) && GlobalTableBlockStart!=GlobalTableBlockEnd)
         )
         {              // must be text box
             strcpy(TmpString,"ÇëÏÈ");

             if(GlobalBoxTool!=IDX_INPUTBOX)
                strcat(TmpString,SelectToolStr);
             strcat(TmpString,
                  "Ñ¡ÔñÎÄ±¾»ò±í¸ñ°æ¿ò(¿ÉÔÚ\n"
                  "°æ¿òÖÐ°´Ò»´ÎÊó±ê×ó¼ü,Ê¹Æä±ß\n"
                  "¿ò±äÎªÊµÏßºÚ¿ò),ÔÙÊ¹ÓÃ±¾¹¦ÄÜ!");

             MessageBox(OpHelpStr, TmpString, 1,0);
             break;
         }

    lbl_test_end:
         switch (Param1)
         {
           case MENU_MEMSTAT:
                MemInfoDisp();
                break;
           case MENU_NEW:
                if(FileHasBeenLoaded() && FileHasBeenModified()
                && !FileHasBeenSaved())
                {
                   Result=MessageBox(GetTitleString(WARNINGINFORM),
                                     GetInformString(FILENOTSAVE),
                                     3,Window);
                   if (Result==1)
                      break;
                   if (Result==0)
                   {
                      if (!DebugFileName[0])
                      {
                         Result=GetFileName(Window,DefaultFileName,DebugFileName,1,SAVEFILE_TITLE);
                         if (Result)
                            break;
                      }
                      MouseSetGraph(BUSYMOUSE);
                      if(!fEditor)
                         filename_cat(DebugFileName,FILE_EXT_NAME);

                      Result=FileSave(DebugFileName);
                      MouseSetGraph(ARRAWMOUSE);
                      if (Result<OpOK)
                      {
                         MessageBox(GetTitleString(ERRORINFORM),
                                    GetInformString(FILESAVEERROR),1,
                                    Window);
                         break;
                      }
                      FileSetNotModified();
                      FileSetSaved();
                   }
                }  // FileHasBeenLoaded

                faxStartPage=-1;
                if(fEditor)
                {
                   memset((char *)&TmpPage,0,sizeof(TmpPage));
                  /*--------
                   TmpPage.MarginLeft=TmpPage.MarginRight=
                      TmpPage.MarginLeft=ConvertToUserMeter(32)*SCALEMETER;
                   TmpPage.MarginTop=TmpPage.MarginBottom=
                      TmpPage.MarginLeft=ConvertToUserMeter(25)*SCALEMETER;
                   TmpPage.PageWidth=ConvertToUserMeter(200)*SCALEMETER;
                   TmpPage.PageHeight=ConvertToUserMeter(280)*SCALEMETER;
                   ---------*/
                   TmpPage.PageWidth=ConvertToUserMeter(164*2)*SCALEMETER;
                   TmpPage.PageHeight=ConvertToUserMeter(304)*SCALEMETER;
                   TmpPage.PageColumn=1;
                   goto lbl_creat_new;
                }

                if (!Page_Setup(Window))
                {
                   if(TmpPage.MarginLeft+TmpPage.MarginRight>=TmpPage.PageWidth
                   || TmpPage.MarginTop+TmpPage.MarginBottom>=TmpPage.PageHeight)
                   {
                      MessageBox(GetTitleString(ERRORINFORM),
                            GetInformString(INVALIDMARGIN),1,
                            WindowGetFather(Window));
                      break;
                   }

              lbl_creat_new:
                   GlobalFontSize=10.5*SCALEMETER/72;
                   IsModule=0;
                   PageDeleteAll();
                   SetPageFootOption(0);
                   SetPageHeadOption(0);
                   SetDefaultExternBlock();        //By zjh 12.9/96
                   DebugFileName[0]=0;

                   MidHPage=PageNew(&TmpPage,0);
                   if (MidHPage)
                   {
                      UndoClear();  // Clear undo stack
                      GlobalTextPosition=0;
                      GlobalTextBlockStart=GlobalTextBlockEnd=-1;
                      GlobalTableBlockStart=GlobalTableBlockEnd=-1;
                      GlobalCurrentPage=MidHPage;
                      if (BoxCanEditable(GlobalBoxHeadHandle))
                      {
                         BoxEnter(GlobalBoxHeadHandle);
                         SetNewCursor();
                      }
                      else
                         GlobalBoxHeadHandle=0;

                      MenuScaleChangeMethod(MENU_VIEWPORTACTUAL);
                      GlobalPageScale=((float)SCALEMETER/(float)screendpi);
                      MovePageToCenter(Window,GlobalCurrentPage);  // ByHance

                      TextCursorSetRotate(0,0,0);
                      TextCursorMoveTo(0,0);
                      if(fEditor)
                        //TextCursorSetHeight(160/GlobalPageScale);
                        TextCursorSetHeight(myUserYToWindowY(160));
                      else
                        //TextCursorSetHeight(DEFAULTCHARSIZE/GlobalPageScale);
                        TextCursorSetHeight(myUserYToWindowY(DEFAULTCHARSIZE));

                      SetNewCursor();
                      RedrawUserField();
                      TellStatus();               // ByHance
                      TellFileName();               // ByHance
                      // MenuScaleChangeMethod(MENU_VIEWPORTACTUAL);
                      FileSetLoaded();
                      FileSetNotModified();  // FileSetModified(); //ByHance
                      FileSetSaved();
                   }
                }
                break;

           case MENU_PAGESETUP:
                if (FileHasBeenLoaded())
                {
                   MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
                   memcpy(&TmpPage,MidPage,sizeof(Pages));

                   if (!Page_Setup(Window))
                   {            // ByHance
                      if(TmpPage.MarginLeft+TmpPage.MarginRight>=TmpPage.PageWidth
                      || TmpPage.MarginTop+TmpPage.MarginBottom>=TmpPage.PageHeight)
                      {
                         MessageBox(GetTitleString(ERRORINFORM),
                               GetInformString(INVALIDMARGIN),1,
                               WindowGetFather(Window));
                         break;
                      }
                      if(TotalPage>0 && PageHaveInitialBox(*MidPage)
                      &&(MidPage->MarginLeft!=TmpPage.MarginLeft
                        ||MidPage->MarginTop!=TmpPage.MarginTop
                        ||MidPage->MarginRight!=TmpPage.MarginRight
                        ||MidPage->MarginBottom!=TmpPage.MarginBottom
                        ||MidPage->PageWidth !=TmpPage.PageWidth
                        ||MidPage->PageHeight!=TmpPage.PageHeight) )
                      {
                        ChangePageSetup(MidPage);      // in ..\kernl\pagec.c
                        memcpy(MidPage,&TmpPage,sizeof(Pages));
                        FormatAll(PageGetBoxHead(GlobalCurrentPage));
                        MovePageToCenter(Window,GlobalCurrentPage);  // ByHance
                        RedrawUserField();
                        if (BoxCanEditable(GlobalBoxHeadHandle))
                        {
                         // added ByHance, for display cursor
                         CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                             GlobalTextPosition,&Left,&Top);
                        }
                        TellStatus();
                        FileSetModified();
                      }
                   }
                   HandleUnlock(ItemGetHandle(GlobalCurrentPage));
                }
                break;
           case MENU_BOX:
                if (GlobalBoxHeadHandle<=0)
                   break;
                BoxPointer=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                if (BoxPointer==NULL)
                   return(OUTOFMEMORY);

                if (BoxCanNotEdit(BoxPointer))
                {
                  lbl_menu_box_err:
                   HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                   break;
                }

                switch (TextBoxGetBoxType(BoxPointer))
                {
                  case TABLEBOX:
                       memcpy(&TmpFormBox,&(BoxPointer->BoxStatus),sizeof(TmpFormBoxs));
                       Result=MakeDialogBox(Window,TableBoxDialog);
                       if(Result)
                          goto lbl_menu_box_err;

                       BoxPointer->TextDistantTop=TmpFormBox.TextDistantTop;
                       BoxPointer->TextDistantLeft=TmpFormBox.TextDistantLeft;
                       BoxPointer->TextDistantRight=TmpFormBox.TextDistantRight;
                       BoxPointer->TextDistantBottom=TmpFormBox.TextDistantBottom;
                       //BoxChange(GlobalBoxHeadHandle,GlobalCurrentPage);
                       FormatAll(GlobalBoxHeadHandle);
                       RedrawUserField();
                       FileSetModified();
                       HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                       break;
                  case TEXTBOX:
                       memcpy(&TmpTextBox,&(BoxPointer->BoxStatus),sizeof(TmpTextBoxs));
                       if (IsModule) Result=MakeDialogBox(Window,TextBoxDialog1);
                       else Result=MakeDialogBox(Window,TextBoxDialog);

                       if (Result==10091)   //Set Text Left space
                       {
                        TmpFormBox.TextDistantTop=BoxPointer->TextDistantTop;
                        TmpFormBox.TextDistantLeft=BoxPointer->TextDistantLeft;
                        TmpFormBox.TextDistantRight=BoxPointer->TextDistantRight;
                        TmpFormBox.TextDistantBottom=BoxPointer->TextDistantBottom;

                        Result=MakeDialogBox(Window,TableBoxDialog);
                        if(Result)
                           goto lbl_menu_box_err;

                        BoxPointer->TextDistantTop=TmpFormBox.TextDistantTop;
                        BoxPointer->TextDistantLeft=TmpFormBox.TextDistantLeft;
                        BoxPointer->TextDistantRight=TmpFormBox.TextDistantRight;
                        BoxPointer->TextDistantBottom=TmpFormBox.TextDistantBottom;
                        //BoxChange(GlobalBoxHeadHandle,GlobalCurrentPage);
                        InitRL(BoxPointer);
                        FormatAll(GlobalBoxHeadHandle);
                        RedrawUserField();
                        FileSetModified();
                        HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                        break;
                       }

                       if (!Result)
                       {
                          if( memcmp(&(BoxPointer->BoxStatus),&TmpTextBox,
                                sizeof(TmpTextBoxs)) == 0 )
                          {             // if not changed, do nothing
                              HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                              break;
                          }

                          BoxPointer->BoxStatus=TmpTextBox.BoxStatus;
                   //-- if result=0, only BoxStatus changed, needn't format --
                          Result=memcmp(&(BoxPointer->BoxStatus),&TmpTextBox,
                                       sizeof(TmpTextBoxs));

                          Tmp=(BoxPointer->RotateAngle==TmpTextBox.RotateAngle)
                           && (BoxPointer->RotateAxisX==TmpTextBox.RotateAxisX)
                           && (BoxPointer->RotateAxisY==TmpTextBox.RotateAxisY);

                          memcpy(&(BoxPointer->BoxStatus),&TmpTextBox,sizeof(TmpTextBoxs));
                          if(Result)
                          {             // if box param changed
                              BoxChange(GlobalBoxHeadHandle,GlobalCurrentPage);
                              RedrawUserField();
                              if(Tmp)   // rotate param not change
                                 CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                     GlobalTextPosition,&Left,&Top);
                              else
                                 SetNewCursor();
                           }
                           FileSetModified();
                       }
                       HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                       break;
                  case RECTANGLEPICTUREBOX:
                  case CORNERPICTUREBOX:
                  case ELIPSEPICTUREBOX:
                  case POLYGONPICTUREBOX:
                       memcpy(&TmpImageBox,&(BoxPointer->BoxStatus),sizeof(TmpPictureBoxs));
                       if (IsModule) Result=MakeDialogBox(Window,PictureBoxDialog1);
                       else Result=MakeDialogBox(Window,PictureBoxDialog);

                       if (!Result)
                       {
                          if( memcmp(&(BoxPointer->BoxStatus),&TmpImageBox,
                                sizeof(TmpPictureBoxs)) == 0 )
                          {             // if not changed, do nothing
                              HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                              break;
                          }

                          BoxPointer->BoxStatus=TmpImageBox.BoxStatus;
                   //-- if result=0, only BoxStatus changed, needn't format --
                          Result=memcmp(&(BoxPointer->BoxStatus),&TmpImageBox,
                                       sizeof(TmpPictureBoxs));
                          memcpy(&(BoxPointer->BoxStatus),&TmpImageBox,sizeof(TmpPictureBoxs));
                          if(Result)
                          {             // if box param changed
                              ImageChangeNewParameter(GlobalBoxHeadHandle);
                              BoxChange(GlobalBoxHeadHandle,GlobalCurrentPage);
                              RedrawUserField();
                          }
                          FileSetModified();
                       }
                       HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                       break;
                   default:
                       HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                       break;
                }
                break;
           case MENU_ABOUT:
                sprintf(TmpString,
                        "        ÇáËÉÈí¼þÏµÁÐ\n"
                        "             Ö®\n"
                        "        ÇáËÉÅÅ°æ 3.0\n \n"
                        "±±¾©ÀíµÂ¼¯ÈºÉÌÓÃ¼¼ÊõÓÐÏÞ¹«Ë¾°æÈ¨\n"
                        "ËùÓÐ     Copyright(c) 1995,1997\n"
                        "REDTEK Business Technology LTD.\n"
                        "ÑÏ½ûÈÎºÎÇÖÈ¨ÐÐÎª   1997Äê8ÔÂ\n");

                //TmpString[65] = 'í';
                //TmpString[64] = 'À';
                //TmpString[67] = 'Â';
                //TmpString[66] = 'µ';
                MessageBox("¹ØÓÚÇáËÉÅÅ°æ",TmpString,1,Window);
                break;
           case MENU_KEYIDX:
               if(fEditor)
                  Result=MakeDialogBox(Window,EditorHelpDialog);
               else
                  Result=MakeDialogBox(Window,KeyHelpDialog);

               switch(Result) {
                   case 10091: HelpBox(0,1);
                    break;
                   case 10092: HelpBox(100,1);
                    break;
                   case 10093: HelpBox(150,1);
                    break;
                   case 10094: HelpBox(160,1);
                    break;
                   case 10095: HelpBox(170,1);
                    break;
                /*----- for editor -----*/
                   case 10096: HelpBox(200,1);
                    break;
                /*----- end for editor -----*/
               } // switch
               break;
           case MENU_SERVICE:
                {
                  sprintf(TmpString,
                          "    ±±¾©ÀíµÂ¼¯ÈºÉÌÓÃ¼¼ÊõÓÐÏÞ¹«\n"
                          "Ë¾µÄ·Ü¶·Ä¿±ê:¸øÄú×îÏÈ½øµÄ²úÆ·,\n"
                          "¸øÄú×îÓÅÖÊµÄ·þÎñ¡£\n"
                          "    ÇëÓëÀíµÂ¹«Ë¾±£³ÖÁªÏµ, ÒÔ±ã\n"
                          "ÁË½â×îÐÂµÄÉý¼¶ÐÅÏ¢¡£ÎÒÃÇÒ²»á¸ü\n"
                          "¼ÓÅ¬Á¦, ÈÃÄú¸üÂúÒâ!\n"
                          "µç»°:(010) 62532582,62532583\n"
                          "´«Õæ:(010) 62532584\n"
                          "email:redtek@public3.bta.net.cn");
                  MessageBox("¹ØÓÚ²úÆ··þÎñ",TmpString,1,0);
                }
                break;
           case MENU_EXIT:
                MessageInsert(1,WINDOWCLOSE,0l,0l);
                break;
           case MENU_TOOLSELECT:
           case MENU_TOOLMOVE:
           case MENU_TOOLROTATE:
           case MENU_TOOLZOOM:
           case MENU_TOOLTEXT:
           case MENU_TOOLTABLE:
           case MENU_TOOLRECTANGLE:
           case MENU_TOOLCORNER:
           case MENU_TOOLELLIPSE:
           case MENU_TOOLPOLYGON:
           case MENU_TOOLSTRAIGHTLINE:
           case MENU_TOOLLINK:
           case MENU_TOOLUNLINK:

                if(fEditor) break;
              #ifdef VERSION10
                if (MenuToolChangeMethod(Param1))
              #endif
                {
                   int i;
                   HWND MidWindow;

                   for(i=0;i<TotalIconNumber;i++)          // ByHance
                     if(IconMenuIdxArr[i]==Param1) break;

                   ToolExchange(GlobalBoxTool,i);      // New!!

                   MidWindow=SearchBackToolBarWindow(Window);
                   if(MidWindow>0)                       // ByHance
                      MessageGo(MidWindow,TOOLCHANGE,i,0);
                   else GlobalBoxTool=i;

                   if(Param1==MENU_TOOLSELECT)
                             SetNewCursor();
                   else if(Param1==MENU_TOOLROTATE)
                             RotateSign=0;
                }
                break;

           case MENU_VIEWPORTFITWINDOW:
                if (MenuScaleChangeMethod(Param1))
                {
                   int WindowWidth,WindowHeight;
                   ORDINATETYPE PageWidth,PageHeight;

                   WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
                   WindowWidth=Right-Left;
                   WindowHeight=Bottom-Top;

                   MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
                   if (MidPage==NULL)
                      break;
                   PageWidth=PageGetPageWidth(MidPage)+150;
                         // +2*PAGETOPDISTANT;    //ByHance, 95,12.7
                   PageHeight=PageGetPageHeight(MidPage)+150; //+2*PAGELEFTDISTANT;
                   HandleUnlock(ItemGetHandle(GlobalCurrentPage));

                   if ((float)(PageWidth)/(float)(WindowWidth)>
                       (float)(PageHeight)/(float)(WindowHeight))
                      GlobalPageScale=(float)PageWidth/(float)WindowWidth+1;
                   else
                      GlobalPageScale=(float)PageHeight/(float)WindowHeight+1;

                   //GlobalPageHStart=GlobalPageVStart=0;
                   //GlobalPageHStart=-6000;   GlobalPageVStart=1300;
                   MovePageToCenter(Window,GlobalCurrentPage);  // ByHance

                   SetNewCursor();
                   RedrawUserField();
                }
                break;
           case MENU_VIEWPORTHALF:
                if (MenuScaleChangeMethod(Param1))
                {
                   GlobalPageScale=((float)SCALEMETER/(float)screendpi)*2.;
                   //GlobalPageHStart=220;   GlobalPageVStart=920;
                   MovePageToCenter(Window,GlobalCurrentPage);  // ByHance

                   SetNewCursor();
                   RedrawUserField();
                }
                break;
           case MENU_VIEWPORTQUART:
                if (MenuScaleChangeMethod(Param1))
                {
                   GlobalPageScale=4*SCALEMETER/screendpi/3;
                   // GlobalPageHStart=1754;   GlobalPageVStart=1505;
                   MovePageToCenter(Window,GlobalCurrentPage);  // ByHance

                   SetNewCursor();
                   RedrawUserField();
                }
                break;
           case MENU_VIEWPORTACTUAL:
                if (MenuScaleChangeMethod(Param1))
                {
                   GlobalPageScale=((float)SCALEMETER/(float)screendpi);
                   //GlobalPageHStart=2084;  GlobalPageVStart=1835;
                   MovePageToCenter(Window,GlobalCurrentPage);  // ByHance

                   SetNewCursor();
                   RedrawUserField();
                }
                break;
           case MENU_VIEWPORTDOUBLE:
                if (MenuScaleChangeMethod(Param1))
                {
                   GlobalPageScale=((float)SCALEMETER/(float)screendpi)/2.;
                   //GlobalPageHStart=2164;  GlobalPageVStart=1920;
                   MovePageToCenter(Window,GlobalCurrentPage);  // ByHance

                   SetNewCursor();
                   RedrawUserField();
                }
                break;
           case MENU_BOXREDRAW:
                RedrawUserField();
                break;
           case MENU_GOTOLINE:
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxIsTextBox(GlobalBoxHeadHandle))
                if (!MakeDialogBox(Window,GotoLineDialog))
                   CursorGotoLine(GlobalBoxHeadHandle,&NewHBox,
                       GlobalTextPosition,&GlobalTextPosition,GetLineNumber(),
                       &GlobalTextBlockStart,&GlobalTextBlockEnd);
                break;
           case MENU_CALCULATELINE:
                Tmp=0;
                goto lbl_calc;
           case MENU_CALCULATEPERSON:
                Tmp=1;
                goto lbl_calc;
           case MENU_CALCULATEMORE:
                Tmp=2;
              lbl_calc:
                if( BoxIsTableBox(GlobalBoxHeadHandle)
                && GlobalTableBlockStart!=GlobalTableBlockEnd)
                    break;

                Result=calculate(GlobalBoxHeadHandle,
                                GlobalTextPosition,&GlobalTextPosition,Tmp);
                if(Result)
                {
                   MessageBox(GetTitleString(WARNINGINFORM),
                         "¶Ô²»Æð,´ËÐÐÓ¦¸ÃÊÇÒ»¸öÊýÑ§¼ÆËãÊ½,\n"
                         "ÀýÈç: (1+23)/55.3+26*(55-9)\n"
                         "±¾ÏµÍ³Ö§³ÖµÄËã·ûÓÐ:\n"
                         "  +  -  *  /  ^  %  (  )    \n"
                         "ÆäÖÐ, * ^ % ·Ö±ðÎª³Ë·¨,³Ë·½ºÍÇó\n"
                         "ÓàÊýËã·û; Ò²¿ÉÒÔÊ¹ÓÃÈý½Çº¯ÊýµÈ¡£",
                         1,Window);
                }
                break;

           case MENU_NEW1:
                Tmp=1;
                goto open_act;
           case MENU_OPEN:
                Tmp=0;
           open_act:
                if (FileHasBeenLoaded()&&FileHasBeenModified()
                && !FileHasBeenSaved())
                {
                   Result=MessageBox(GetTitleString(WARNINGINFORM),
                                     GetInformString(FILENOTSAVE),
                                     3,Window);
                   if (Result==1)
                      break;
                   if (Result==0)               // save it
                   {
                      if(!DebugFileName[0]) {
                          Result=GetFileName(Window,DefaultFileName,DebugFileName,1,SAVEFILE_TITLE);
                          if (Result)    // no input, cancel
                              return FALSE;      // break;
                      }
                      MouseSetGraph(BUSYMOUSE);
                      //if(!fEditor)  filename_cat(DebugFileName,FILE_EXT_NAME);
                      if(!fEditor && !Tmp)
                           filename_cat(DebugFileName,FILE_EXT_NAME);

                      Result=FileSave(DebugFileName);
                      MouseSetGraph(ARRAWMOUSE);
                      if (Result<OpOK)
                      {
                         MessageBox(GetTitleString(ERRORINFORM),
                                    GetInformString(FILESAVEERROR),1,
                                    Window);
                         return FALSE;      // break;
                      }
                      FileSetNotModified();
                      FileSetSaved();
                   }
                }

                if(!fAutoLoad)
                {
                   if (Tmp)     // creat from FRA
                     Result=GetFileName(Window,"*.FRA",DebugFileName,0,OPENFILE_TITLE);
                   else
                     Result=GetFileName(Window,DefaultFileName,DebugFileName,0,OPENFILE_TITLE);
                   if (Result)
                       return FALSE;      // break;

                   //if(!fEditor) filename_cat(DebugFileName,FILE_EXT_NAME);
                   if(!fEditor && !Tmp)
                        filename_cat(DebugFileName,FILE_EXT_NAME);

                  #ifdef OLD_VERSION_zjh
                   if (Align)
                   {
                       mycopy(DebugFileName,"NONAME.EZP");
                       strcpy(DebugFileName,"NONAME.EZP");
                       DebugFileName[0]=0;
                       FileSetModified();
                       goto lbl_open_10;
                   }
                  #endif
                }
                fAutoLoad=FALSE;

                PageDeleteAll();
                faxStartPage=-1;
                MouseSetGraph(BUSYMOUSE);

                if(fEditor)
                {
                   FileSetNotModified();  // FileSetModified(); //ByHance
                   FileSetSaved();
                   strcpy(TmpString,DebugFileName);
                   UserMenuCommand(Window,Message,MENU_NEW);
                   WaitMessageEmpty();
                   strcpy(DebugFileName,TmpString);
                   Result=TextBoxInsertTextFile(DebugFileName,
                            GlobalBoxHeadHandle,&GlobalBoxHeadHandle,
                            GlobalTextPosition,&GlobalTextPosition,
                            &GlobalTextBlockStart,&GlobalTextBlockEnd);
                   TellFileName();
                }
                else
                {
                   Result=FileLoad(DebugFileName,
                            &GlobalCurrentPage,&GlobalBoxHeadHandle);
                   if(Tmp)      // creat from FRA
                      IsModule=0;
                }
                MouseSetGraph(ARRAWMOUSE);

             #ifndef OLD_VERSION_zjh
                if (Tmp)      // creat from FRA
                {
                    DebugFileName[0]=0;
                    FileSetModified();
                }
              #endif

                GlobalFontSize=10.5*SCALEMETER/72;
                if (Result<OpOK)
                {
                   /* By zjh 1997.4.1
                   MessageBox(GetTitleString(ERRORINFORM),
                              GetInformString(FILELOADERROR),1,
                              Window);
                   return FALSE;      // break;
                   */
                   if(Result==INVALIDVERSION)
                   {
                      MessageBox(GetTitleString(ERRORINFORM),
                                 "ÇëÊ¹ÓÃ¸ü¸ß°æ±¾µÄÇáËÉÅÅ°æÏµÍ³\n¶ÁÈ¡´ËÎÄ¼þ!",
                                 1,Window);
                      goto lbl_creat_new;    // return FALSE;
                   }

                   if (Result==FILEFORMAT||(Result==FILEOPEN&&CreateFile))
                   {
                      if (Result==FILEFORMAT)
                      {
                        Result=MessageBox("¸ñÊ½×ª»»",
                               "¸ÃÎÄ¼þ²»ÊÇEZP¸ñÊ½ÎÄ¼þ,×ª»»ÎªEZP¸ñÊ½Âð?",
                               2,Window);
                        if(Result) goto lbl_creat_new;    // return FALSE;
                        CreateFile=0;
                      }

                      //Create new page
                      //PageDeleteAll();
                      SetPageFootOption(0);
                      SetPageHeadOption(0);

                      TmpPage.PageType=2;
                      TmpPage.PageColumn=1;
                      TmpPage.VirtualPage=0;
                      TmpPage.PageWidth=8267;
                      TmpPage.PageHeight=11692;
                      TmpPage.ColumnDistant=270;
                      TmpPage.MarginLeft=1259;
                      TmpPage.MarginRight=1259;
                      TmpPage.MarginTop=984;
                      TmpPage.MarginBottom=984;

                      MidHPage=PageNew(&TmpPage,0);
                      if (MidHPage)
                      {
                         UndoClear();  // Clear undo stack
                         GlobalTextPosition=0;
                         GlobalTextBlockStart=GlobalTextBlockEnd=-1;
                         GlobalTableBlockStart=GlobalTableBlockEnd=-1;
                         GlobalCurrentPage=MidHPage;
                         if (BoxCanEditable(GlobalBoxHeadHandle))
                         {
                            BoxEnter(GlobalBoxHeadHandle);
                            SetNewCursor();
                         }
                         //DebugFileName[0]=0;

                         MenuScaleChangeMethod(MENU_VIEWPORTACTUAL);
                         GlobalPageScale=((float)SCALEMETER/(float)screendpi);
                         MovePageToCenter(Window,GlobalCurrentPage);  // ByHance

                         TextCursorSetRotate(0,0,0);
                         TextCursorMoveTo(0,0);
                         TextCursorSetHeight(DEFAULTCHARSIZE/GlobalPageScale);

                         SetNewCursor();
                         RedrawUserField();
                         TellStatus();               // ByHance
                         TellFileName();               // ByHance
                         // MenuScaleChangeMethod(MENU_VIEWPORTACTUAL);
                         FileSetLoaded();
                         FileSetNotModified();  // FileSetModified(); //ByHance
                         FileSetSaved();

                         if (CreateFile) break;
                         //Insert Text
                         strcpy(TmpString,DebugFileName);

                         MouseSetGraph(BUSYMOUSE);
                         //GlobalNotDisplay=1;                // ByHance, 95,12.8
                         Result=TextBoxInsertTextFile(TmpString,
                             GlobalBoxHeadHandle,&GlobalBoxHeadHandle,
                             GlobalTextPosition,&GlobalTextPosition,
                             &GlobalTextBlockStart,&GlobalTextBlockEnd);
                         //GlobalNotDisplay=0;
                         MouseSetGraph(ARRAWMOUSE);
                         if (Result<OpOK)
                         {
                           MessageBox(GetTitleString(ERRORINFORM),
                                    GetInformString(FILELOADERROR),1,
                                    Window);
                         }
                         else
                         {
                           FileSetModified();
                           TellStatus();               // ByHance
                         }
                         break;
                      }  //MifPage

                      MessageBox(GetTitleString(ERRORINFORM),
                              "´´½¨ÐÂÒ³Ê§°Ü!",1,
                              Window);

                      return FALSE;  //Error;
                   }

                   if (Result==FILEOPEN)
                   {
                      MessageBox(GetTitleString(ERRORINFORM),
                              GetInformString(FILELOADERROR),1,
                              Window);
                      return FALSE;
                   }

                   MessageBox(GetTitleString(ERRORINFORM),
                              "ÎÄ¼þ¸ñÊ½²»¶Ô!",1,
                              Window);
                   return FALSE;
                }

                if (!Tmp) // open file
                   FileSetNotModified();

                FileSetLoaded();
                FileSetNotSaved();
                if(!fEditor)
                {
                   UndoClear();      // Clear undo stack
                   ItemSetChild(GlobalPageHeadHandle,GlobalCurrentPage);
                   if (BoxCanEditable(GlobalBoxHeadHandle))
                   {
                      GlobalTextPosition=0;
                      GlobalTextBlockStart=GlobalTextBlockEnd=-1;
                      GlobalTableBlockStart=GlobalTableBlockEnd=-1;
                      BoxEnter(GlobalBoxHeadHandle);
                      TextBoxSeekTextPosition(GlobalBoxHeadHandle,0,
                                              1,&GlobalTextPosition);
                      TextBoxSeekTextPosition(GlobalBoxHeadHandle,
                                              GlobalTextPosition,
                                              -1,&GlobalTextPosition);
                   }
                   else
                      GlobalBoxHeadHandle=0;

                   MouseHidden();
                   WindowGetRect(Window,&Left,&Top,&Right,&Bottom);
                   setviewport(Left,Top,Right,Bottom,1);
                   setfillstyle(1,EGA_WHITE);
                   bar(1,1,Right-Left,Bottom-Top);
                   MouseShow();

                   GlobalPageScale=((float)SCALEMETER/(float)screendpi);
                   // GlobalPageHStart=2084;  GlobalPageVStart=1835;
                   MovePageToCenter(Window,GlobalCurrentPage);  // ByHance

                   SetNewCursor();
                   MenuScaleChangeMethod(MENU_VIEWPORTACTUAL);
                   RedrawUserField();
                }

                TellStatus();
                TellFileName();
                break;

           case MENU_EXPORTTEXT:
                if( MakeDialogBox(Window,ExportDialog) )
                   break;
                if(GetExportOption()==0         // only this story
                 && !BoxIsTextBox(GlobalBoxHeadHandle))
                {
                   MessageBox(OpHelpStr,"ÇëÏÈÑ¡ÔñÒ»¸öÎÄ±¾°æ¿ò,\nÔÙÊ¹ÓÃ±¾¹¦ÄÜ!",
                         1,0);
                   break;
                }

                Result=GetFileName(Window,"*.TXT;*.*",TmpString,1,EXPTEXT_TITLE);
                if (Result)
                   break;
                MouseSetGraph(BUSYMOUSE);
                if( GetExportOption()==0 )        // only this story
                    Result=FileExportStory(TmpString,GlobalBoxHeadHandle);
                else
                    Result=FileExportText(TmpString);

                MouseSetGraph(ARRAWMOUSE);
                if (Result<OpOK)
                {
                   MessageBox(GetTitleString(ERRORINFORM),
                              GetInformString(FILESAVEERROR),1,
                              Window);
                   return(FALSE);
                }
                return(TRUE);
                break;

           case MENU_SAVE:
                if (DebugFileName[0])
                {
                  if (!FileHasBeenLoaded())
                     break;
              lbl_file_save:
                  MouseSetGraph(BUSYMOUSE);
                  if(fEditor)
                     Result=FileExportText(DebugFileName);
                  else
                     Result=FileSave(DebugFileName);

                  MouseSetGraph(ARRAWMOUSE);
                  if (Result<OpOK)
                  {
                     MessageBox(GetTitleString(ERRORINFORM),
                                GetInformString(FILESAVEERROR),1,
                                Window);
                     return(FALSE);
                  }
                  FileSetNotModified();
                  FileSetSaved();
                  return(TRUE);
                }
           case MENU_SAVEAS:
                if (!FileHasBeenLoaded())
                   break;
                if(GetFileName(Window,DefaultFileName,DebugFileName,1,SAVEAS_TITLE ))
                   return(FALSE);        // ByHance, 96,1.22

                // if(!fEditor)  filename_cat(DebugFileName,FILE_EXT_NAME);
                if(!fEditor)
                {
                   int pos=0;
                   char ch;

                   Tmp=FALSE;
                   while((ch=DebugFileName[pos++])!=0)
                   {
                      if(ch=='.') Tmp=TRUE;
                      else
                      if(ch==':' || ch=='\\' || ch=='/')
                           Tmp=FALSE;
                   }

                   if(!Tmp)
                       filename_cat(DebugFileName,FILE_EXT_NAME);
                }

                TellFileName();
                goto lbl_file_save;

/*
           case MENU_INSTALLFONT:
                MakeDialogBox(Window,InstFontDialog);
                break;
           case MENU_SETFONTPATH:
                if (!MakeDialogBox(Window,SetFontPathDialog))
                   SaveFontPath();
                break;
*/
           case MENU_FONT:
                if( BoxIsTableBox(GlobalBoxHeadHandle)
                && GlobalTableBlockStart!=GlobalTableBlockEnd)
                {
                    reg1=GlobalTableBlockStart;
                    reg2=GlobalTableBlockEnd;
                    if (reg1>reg2)
                    {
                      iCell=reg1; reg1=reg2; reg2=iCell;
                    }

                    FBGetCellRect(GlobalBoxHeadHandle,reg1,&col1,&row1,&Right,&Bottom);
                    FBGetCellRect(GlobalBoxHeadHandle,reg2,&Left,&Top,&col2,&row2);

                    SaveUndoNum=UndoOperateSum;

                    CancelCellBlock(GlobalBoxHeadHandle);
                    fChgNextCell=FALSE;
                    GlobalNotDisplay=1;

                    for (iCell=reg1;iCell<=reg2;iCell++)
                    {
                       FBGetCellRect(GlobalBoxHeadHandle,iCell,
                                        &Left,&Top,&Right,&Bottom);
                       if (Left<col1 || Top<row1 || Right>col2 || Bottom>row2)
                            continue;

                       GlobalTextBlockStart=GlobalTextPosition
                            =TableCellGetTextHead(GlobalBoxHeadHandle,iCell);
                       GlobalTextBlockEnd=GlobalTextBlockStart
                            +TableCellGetTextLength(GlobalBoxHeadHandle,iCell);

                       if(SetCharAttr(Window))
                          break;

                       CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                       if(!fChgNextCell)
                          fChgNextCell=TRUE;
                    }

                    fChgNextCell=FALSE;
                    GlobalNotDisplay=0;
                    GlobalTextBlockStart=GlobalTextBlockEnd=-1;

                    TableBoxDraw(GlobalBoxHeadHandle);
                    CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&pos,&Tmp);
                    UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                else
                    SetCharAttr(Window);
                break;
              #ifdef NOT_USED
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                   if(/* GlobalTextBlockStart<GlobalTextBlockEnd && */
                     !MakeDialogBox(Window,CharFontDialog) )
                   {
                      if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                           pos = GlobalTextBlockStart;
                      else pos = GlobalTextPosition;

                      GlobalNotDisplay=1;     //1.26
                      SaveUndoNum=UndoOperateSum;
                      /*---- change Chinese_Font -------*/
                      Result=TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARFONT,TmpAttribute,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);

                      if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                           pos = GlobalTextBlockStart;
                      else pos = GlobalTextPosition;

                      //if(Result)
                      GlobalNotDisplay=0;
                      /*---- change English_Font -------*/
                      TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARFONT,TmpAttribute2,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);
                    //  GlobalNotDisplay=0;
                      UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                   }
                }
                break;
              #endif  // NOT_USED

           case MENU_FONTSIZE8:   pointn = 5; goto lbl_Size;
           case MENU_FONTSIZE7:   pointn = 5.5; goto lbl_Size;
           case MENU_FONTSIZE6:   pointn = 8; goto lbl_Size;
           case MENU_FONTSIZE5X:  pointn = 9; goto lbl_Size;
           case MENU_FONTSIZE5:   pointn = 10.5; goto lbl_Size;
           case MENU_FONTSIZE4X:  pointn = 12; goto lbl_Size;
           case MENU_FONTSIZE4:   pointn = 13.5; goto lbl_Size;
           case MENU_FONTSIZE3:   pointn = 16; goto lbl_Size;
           case MENU_FONTSIZE2:   pointn = 22; goto lbl_Size;
           case MENU_FONTSIZE1:   pointn = 26; goto lbl_Size;
           case MENU_FONTSIZE0:   pointn = 42; // goto lbl_Size;

           lbl_Size:     //-- must be mm --
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                      TmpAttribute2=TmpAttribute=pointn*SCALEMETER/72;
                      goto lbl_chg_FontSize;
                }
                break;
          case MENU_FONTSIZEFREE:
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                   if(MakeDialogBox(Window,CharSizeDialog))
                        break;

                lbl_chg_FontSize:
                   if( BoxIsTableBox(GlobalBoxHeadHandle)   // ByHance,97,8.12
                   && GlobalTableBlockStart!=GlobalTableBlockEnd)
                   {
                      Tmp=GlobalTableCell;

                      reg1=GlobalTableBlockStart;
                      reg2=GlobalTableBlockEnd;
                      if (reg1>reg2)
                      {
                        iCell=reg1; reg1=reg2; reg2=iCell;
                      }

                      FBGetCellRect(GlobalBoxHeadHandle,reg1,&col1,&row1,&Right,&Bottom);
                      FBGetCellRect(GlobalBoxHeadHandle,reg2,&Left,&Top,&col2,&row2);

                      SaveUndoNum=UndoOperateSum;

                      CancelCellBlock(GlobalBoxHeadHandle);
                      GlobalNotDisplay=1;

                      for (iCell=reg1;iCell<=reg2;iCell++)
                      {
                         FBGetCellRect(GlobalBoxHeadHandle,iCell,
                                          &Left,&Top,&Right,&Bottom);
                         if (Left<col1 || Top<row1 || Right>col2 || Bottom>row2)
                              continue;

                         GlobalTableCell=iCell;
                         if( !IsCellWidthValid(GlobalBoxHeadHandle,TmpAttribute) )
                             goto lbl_not_chg_cell_fontsize;

                         GlobalTextBlockStart=pos
                              =TableCellGetTextHead(GlobalBoxHeadHandle,iCell);
                         GlobalTextBlockEnd=GlobalTextBlockStart
                              +TableCellGetTextLength(GlobalBoxHeadHandle,iCell);

                         Result=TextChangeAttribute(GlobalBoxHeadHandle,pos,
                               GlobalTextBlockEnd-GlobalTextBlockStart,
                               CHARSIZE,TmpAttribute,&GlobalTextPosition,
                               &GlobalTextBlockStart,&GlobalTextBlockEnd);

                         pos=GlobalTextBlockStart;
                         TextChangeAttribute(GlobalBoxHeadHandle,pos,
                               GlobalTextBlockEnd-GlobalTextBlockStart,
                               CHARHSIZE,TmpAttribute2,&GlobalTextPosition,
                               &GlobalTextBlockStart,&GlobalTextBlockEnd);
                         CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                      }

                      GlobalFontSize=TmpAttribute;
                   lbl_not_chg_cell_fontsize:
                      UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                      GlobalNotDisplay=0;
                      GlobalTextBlockStart=GlobalTextBlockEnd=-1;
                      AdjustTableCells(GlobalBoxHeadHandle);
                      CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&pos,&Tmp);
                      TableBoxDraw(GlobalBoxHeadHandle);
                      break;
                   }

                   if( !IsCellWidthValid(GlobalBoxHeadHandle,TmpAttribute) )
                      break;         // ByHance, 96,4.13

                   if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                        pos = GlobalTextBlockStart;
                   else
                        pos = GlobalTextPosition;

                   GlobalNotDisplay=1;
                   GlobalFontSize=TmpAttribute;
                   SaveUndoNum=UndoOperateSum;
                   Result=TextChangeAttribute(GlobalBoxHeadHandle,pos,
                               GlobalTextBlockEnd-GlobalTextBlockStart,
                               CHARSIZE,TmpAttribute,&GlobalTextPosition,
                               &GlobalTextBlockStart,&GlobalTextBlockEnd);

                   if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                        pos = GlobalTextBlockStart;
                   else
                        pos = GlobalTextPosition;

                   //if(Result)
                      GlobalNotDisplay=0;
                   TextChangeAttribute(GlobalBoxHeadHandle,pos,
                               GlobalTextBlockEnd-GlobalTextBlockStart,
                               CHARHSIZE,TmpAttribute2,&GlobalTextPosition,
                               &GlobalTextBlockStart,&GlobalTextBlockEnd);
                   //GlobalNotDisplay=0;

                   if( BoxIsTableBox(GlobalBoxHeadHandle) )   // ByHance,97,8.12
                       AdjustTableCells(GlobalBoxHeadHandle);

                   UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;


           case MENU_FONTSIZE:
                MenuHotKeyToMessage(Window,ALT_Y);        // ByHance
                MenuDefaultProc(Window,KEYDOWN,'H',0);   // in UserMenu.C
                break;

           case MENU_SUPERSCRIPT:
                Result=SUPERSCRIPT;
                goto lbl_chg_script;
           case MENU_SUBSCRIPT:
                Result=SUBSCRIPT;
             lbl_chg_script:
                if( BoxIsTableBox(GlobalBoxHeadHandle)   // ByHance,97,8.12
                && GlobalTableBlockStart!=GlobalTableBlockEnd)
                   break;

                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                      if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                           pos = GlobalTextBlockStart;
                      else
                           pos = GlobalTextPosition;

                      Tmp=TextSearchAttribute(GlobalBoxHeadHandle,pos,
                                            Result,&AttributePosition);
                      if (Tmp)
                          break;

                      tmpCharSize=TextSearchAttribute(GlobalBoxHeadHandle,
                                         pos,CHARSIZE,&AttributePosition);
                      //tmpCharSize = (float)tmpCharSize*58/100/FONTSIZEFACT*1000/72+0.5;
                      tmpCharSize=(float)tmpCharSize*58/72/FONTSIZEFACT*FONTSIZEFACT+0.5;

                      GlobalNotDisplay=1;
                      SaveUndoNum=UndoOperateSum;
                      if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                           pos = GlobalTextBlockStart;
                      else
                           pos = GlobalTextPosition;
                      TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARSIZE,tmpCharSize,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);

                      if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                           pos = GlobalTextBlockStart;
                      else
                           pos = GlobalTextPosition;
                      TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARHSIZE,tmpCharSize,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);

                      GlobalNotDisplay=0;
                      if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                           pos = GlobalTextBlockStart;
                      else
                           pos = GlobalTextPosition;
                      TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  Result,1,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);

                      UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;

           /* case MENU_TEXTSLANT: not used now */
           case MENU_CHARITALIC:     // 15 degree
                if( BoxIsTableBox(GlobalBoxHeadHandle)   // ByHance,97,8.12
                && GlobalTableBlockStart!=GlobalTableBlockEnd)
                {
                    reg1=GlobalTableBlockStart;
                    reg2=GlobalTableBlockEnd;
                    if (reg1>reg2)
                    {
                      iCell=reg1; reg1=reg2; reg2=iCell;
                    }

                    FBGetCellRect(GlobalBoxHeadHandle,reg1,&col1,&row1,&Right,&Bottom);
                    FBGetCellRect(GlobalBoxHeadHandle,reg2,&Left,&Top,&col2,&row2);

                    SaveUndoNum=UndoOperateSum;

                    CancelCellBlock(GlobalBoxHeadHandle);
                    GlobalNotDisplay=1;

                    for (iCell=reg1;iCell<=reg2;iCell++)
                    {
                       FBGetCellRect(GlobalBoxHeadHandle,iCell,
                                        &Left,&Top,&Right,&Bottom);
                       if (Left<col1 || Top<row1 || Right>col2 || Bottom>row2)
                            continue;

                       GlobalTextBlockStart=pos
                            =TableCellGetTextHead(GlobalBoxHeadHandle,iCell);
                       GlobalTextBlockEnd=GlobalTextBlockStart
                            +TableCellGetTextLength(GlobalBoxHeadHandle,iCell);

                       TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARSLANT,15,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);

                       CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                    }

                    GlobalNotDisplay=0;
                    GlobalTextBlockStart=GlobalTextBlockEnd=-1;

                    TableBoxDraw(GlobalBoxHeadHandle);
                    CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&pos,&Tmp);
                    UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                    break;
                }

                //Result=15;
                if( GlobalBoxTool==IDX_INPUTBOX
                && BoxCanEditable(GlobalBoxHeadHandle))
                {
                    if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                         pos = GlobalTextBlockStart;
                    else
                         pos = GlobalTextPosition;

                    SaveUndoNum=UndoOperateSum;
                         // Here, we use only 15 degree
                    TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARSLANT,15,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);
                    UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;
           case MENU_CHARNORMAL:     // 0 degree slant
                if( BoxIsTableBox(GlobalBoxHeadHandle)   // ByHance,97,8.12
                && GlobalTableBlockStart!=GlobalTableBlockEnd)
                {
                    reg1=GlobalTableBlockStart;
                    reg2=GlobalTableBlockEnd;
                    if (reg1>reg2)
                    {
                      iCell=reg1; reg1=reg2; reg2=iCell;
                    }

                    FBGetCellRect(GlobalBoxHeadHandle,reg1,&col1,&row1,&Right,&Bottom);
                    FBGetCellRect(GlobalBoxHeadHandle,reg2,&Left,&Top,&col2,&row2);

                    SaveUndoNum=UndoOperateSum;

                    CancelCellBlock(GlobalBoxHeadHandle);
                    GlobalNotDisplay=1;

                    for (iCell=reg1;iCell<=reg2;iCell++)
                    {
                       FBGetCellRect(GlobalBoxHeadHandle,iCell,
                                        &Left,&Top,&Right,&Bottom);
                       if (Left<col1 || Top<row1 || Right>col2 || Bottom>row2)
                            continue;

                       GlobalTextBlockStart=pos
                            =TableCellGetTextHead(GlobalBoxHeadHandle,iCell);
                       GlobalTextBlockEnd=GlobalTextBlockStart
                            +TableCellGetTextLength(GlobalBoxHeadHandle,iCell);

                       Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                                             pos,CHARSLANT,&AttributePosition);
                       if (Tmp)
                       {
                           TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                           GlobalTextBlockEnd-GlobalTextBlockStart,
                                           CHARSLANT,0,&GlobalTextPosition,
                                           &GlobalTextBlockStart,&GlobalTextBlockEnd);
                           pos = GlobalTextBlockStart;
                       }

                       //SUPERSCRIPT->Normal
                       Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                                            pos,SUPERSCRIPT,&AttributePosition);
                       if (Tmp)
                       {
                           TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                           GlobalTextBlockEnd-GlobalTextBlockStart,
                                           SUPERSCRIPT,0,&GlobalTextPosition,
                                           &GlobalTextBlockStart,&GlobalTextBlockEnd);

                           pos = GlobalTextBlockStart;

                           tmpCharSize=TextSearchAttribute(GlobalBoxHeadHandle,
                                                pos,CHARSIZE,&AttributePosition);
                           //tmpCharSize = (float)tmpCharSize*100/58*FONTSIZEFACT/1000*72+0.5;
                           tmpCharSize=(float)tmpCharSize*72/58/FONTSIZEFACT*FONTSIZEFACT+0.5;

                           TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                     GlobalTextBlockEnd-GlobalTextBlockStart,
                                     CHARSIZE,tmpCharSize,&GlobalTextPosition,
                                     &GlobalTextBlockStart,&GlobalTextBlockEnd);

                           pos = GlobalTextBlockStart;
                           TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                     GlobalTextBlockEnd-GlobalTextBlockStart,
                                     CHARHSIZE,tmpCharSize,&GlobalTextPosition,
                                     &GlobalTextBlockStart,&GlobalTextBlockEnd);

                           pos = GlobalTextBlockStart;
                       }

                       //SUBSCRIPT->Normal
                       Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                                               pos,SUBSCRIPT,&AttributePosition);
                       if (Tmp)
                       {
                           TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                           GlobalTextBlockEnd-GlobalTextBlockStart,
                                           SUBSCRIPT,0,&GlobalTextPosition,
                                           &GlobalTextBlockStart,&GlobalTextBlockEnd);

                           pos = GlobalTextBlockStart;

                           tmpCharSize=TextSearchAttribute(GlobalBoxHeadHandle,
                                                pos,CHARSIZE,&AttributePosition);
                           //tmpCharSize = (float)tmpCharSize*100/58*FONTSIZEFACT/1000*72+0.5;
                           tmpCharSize=(float)tmpCharSize*72/58/FONTSIZEFACT*FONTSIZEFACT+0.5;

                           TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                     GlobalTextBlockEnd-GlobalTextBlockStart,
                                     CHARSIZE,tmpCharSize,&GlobalTextPosition,
                                     &GlobalTextBlockStart,&GlobalTextBlockEnd);

                           pos = GlobalTextBlockStart;

                           TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                     GlobalTextBlockEnd-GlobalTextBlockStart,
                                     CHARHSIZE,tmpCharSize,&GlobalTextPosition,
                                     &GlobalTextBlockStart,&GlobalTextBlockEnd);
                       }

                       CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                    }

                    GlobalNotDisplay=0;
                    GlobalTextBlockStart=GlobalTextBlockEnd=-1;

                    TableBoxDraw(GlobalBoxHeadHandle);
                    CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&pos,&Tmp);
                    UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                    break;
                }

                if(GlobalBoxTool==IDX_INPUTBOX
                && BoxCanEditable(GlobalBoxHeadHandle))
                {
                    if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                           pos = GlobalTextBlockStart;
                    else
                           pos = GlobalTextPosition;

                    SaveUndoNum=UndoOperateSum;
                    //Slant->Normal
                    Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                                          pos,CHARSLANT,&AttributePosition);
                    if (Tmp)
                    {
                        TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                        GlobalTextBlockEnd-GlobalTextBlockStart,
                                        CHARSLANT,0,&GlobalTextPosition,
                                        &GlobalTextBlockStart,&GlobalTextBlockEnd);
                        if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                               pos = GlobalTextBlockStart;
                        else
                               pos = GlobalTextPosition;
                    }

                    //SUPERSCRIPT->Normal
                    Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                                         pos,SUPERSCRIPT,&AttributePosition);
                    if (Tmp)
                    {
                        GlobalNotDisplay=1;
                        TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                        GlobalTextBlockEnd-GlobalTextBlockStart,
                                        SUPERSCRIPT,0,&GlobalTextPosition,
                                        &GlobalTextBlockStart,&GlobalTextBlockEnd);

                        if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                               pos = GlobalTextBlockStart;
                        else   pos = GlobalTextPosition;

                        tmpCharSize=TextSearchAttribute(GlobalBoxHeadHandle,
                                             pos,CHARSIZE,&AttributePosition);
                        //tmpCharSize = (float)tmpCharSize*100/58*FONTSIZEFACT/1000*72+0.5;
                        tmpCharSize=(float)tmpCharSize*72/58/FONTSIZEFACT*FONTSIZEFACT+0.5;

                        TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARSIZE,tmpCharSize,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);

                        if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                               pos = GlobalTextBlockStart;
                        else   pos = GlobalTextPosition;

                        GlobalNotDisplay=0;
                        TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARHSIZE,tmpCharSize,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);

                        if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                               pos = GlobalTextBlockStart;
                        else   pos = GlobalTextPosition;
                    }

                    //SUBSCRIPT->Normal
                    Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                                            pos,SUBSCRIPT,&AttributePosition);
                    if (Tmp)
                    {
                        GlobalNotDisplay=1;
                        TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                        GlobalTextBlockEnd-GlobalTextBlockStart,
                                        SUBSCRIPT,0,&GlobalTextPosition,
                                        &GlobalTextBlockStart,&GlobalTextBlockEnd);

                        if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                               pos = GlobalTextBlockStart;
                        else   pos = GlobalTextPosition;

                        tmpCharSize=TextSearchAttribute(GlobalBoxHeadHandle,
                                             pos,CHARSIZE,&AttributePosition);
                        //tmpCharSize = (float)tmpCharSize*100/58*FONTSIZEFACT/1000*72+0.5;
                        tmpCharSize=(float)tmpCharSize*72/58/FONTSIZEFACT*FONTSIZEFACT+0.5;

                        TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARSIZE,tmpCharSize,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);

                        if ( GlobalTextBlockEnd>GlobalTextBlockStart)
                               pos = GlobalTextBlockStart;
                        else   pos = GlobalTextPosition;

                        GlobalNotDisplay=0;
                        TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARHSIZE,tmpCharSize,&GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);
                    }

                    UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;

        /*------------- Not used now, --------
           case MENU_COLOR:
                if (GlobalBoxHeadHandle>0&& GlobalBoxTool==IDX_INPUTBOX &&
                    GlobalTextBlockStart<GlobalTextBlockEnd)
                {
                   CharColorDialog[4].ItemProcedure=CharColorProcedure;
                   if (!MakeDialogBox(Window,CharColorDialog))
                      TextChangeAttribute(GlobalBoxHeadHandle,GlobalTextBlockStart,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARCOLOR,TmpAttribute3|(TmpAttribute4<<4),
                                  &GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);
                }
                break;
         ------------------------*/

#define   CLR_BLACK     0
#define   CLR_BLUE      1
#define   CLR_GREEN     2
#define   CLR_CYAN      3
#define   CLR_RED       4
#define   CLR_MAGENTA   5
#define   CLR_DYELLOW   6
#define   CLR_LGRAY     7
#define   CLR_DGRAY     8
#define   CLR_LBLUE     9
#define   CLR_LGREEN    10
#define   CLR_LCYAN     11
#define   CLR_LRED      12
#define   CLR_LMAGENTA  13
#define   CLR_YELLOW    14
#define   CLR_WHITE     15


           case MENU_CHARBLACK:  tmpColor =   CLR_BLACK; goto Do_color;
           case MENU_CHARWHITE:  tmpColor =   CLR_WHITE; goto Do_color;
           case MENU_CHARRED:    tmpColor =   CLR_LRED;   goto Do_color;
           case MENU_CHARGREEN:  tmpColor =   CLR_LGREEN; goto Do_color;
           case MENU_CHARBLUE :  tmpColor =   CLR_LBLUE ; goto Do_color;
           case MENU_CHARCYAN :  tmpColor =   CLR_LCYAN ; goto Do_color;
           case MENU_CHARMAGENTA:tmpColor =   CLR_LMAGENTA; goto Do_color;
           case MENU_CHARYELLOW: tmpColor =   CLR_YELLOW;  goto Do_color;
           case MENU_CHARDKGRAY: tmpColor =   CLR_DGRAY;  goto Do_color;
           case MENU_CHARLTGRAY: tmpColor =   CLR_LGRAY;  goto Do_color;
           case MENU_CHARDKRED: tmpColor =   CLR_RED;    goto Do_color;
           case MENU_CHARDKGREEN: tmpColor =   CLR_GREEN; goto Do_color;
           case MENU_CHARDKBLUE: tmpColor =   CLR_BLUE; goto Do_color;
           case MENU_CHARDKCYAN: tmpColor =   CLR_CYAN;    goto Do_color;
           case MENU_CHARDKMAGENTA: tmpColor =   CLR_MAGENTA; goto Do_color;
           case MENU_CHARDKYELLOW: tmpColor =   CLR_DYELLOW; //goto Do_color;


           Do_color:
                TmpAttribute3=tmpColor;

                if( BoxIsTableBox(GlobalBoxHeadHandle)   // ByHance,97,8.12
                && GlobalTableBlockStart!=GlobalTableBlockEnd)
                {
                    reg1=GlobalTableBlockStart;
                    reg2=GlobalTableBlockEnd;
                    if (reg1>reg2)
                    {
                      iCell=reg1; reg1=reg2; reg2=iCell;
                    }

                    FBGetCellRect(GlobalBoxHeadHandle,reg1,&col1,&row1,&Right,&Bottom);
                    FBGetCellRect(GlobalBoxHeadHandle,reg2,&Left,&Top,&col2,&row2);

                    SaveUndoNum=UndoOperateSum;

                    CancelCellBlock(GlobalBoxHeadHandle);
                    GlobalNotDisplay=1;

                    for (iCell=reg1;iCell<=reg2;iCell++)
                    {
                       FBGetCellRect(GlobalBoxHeadHandle,iCell,
                                        &Left,&Top,&Right,&Bottom);
                       if (Left<col1 || Top<row1 || Right>col2 || Bottom>row2)
                            continue;

                       GlobalTextBlockStart=pos
                            =TableCellGetTextHead(GlobalBoxHeadHandle,iCell);
                       GlobalTextBlockEnd=GlobalTextBlockStart
                            +TableCellGetTextLength(GlobalBoxHeadHandle,iCell);

                       TmpAttribute4=TextSearchAttribute(GlobalBoxHeadHandle,
                                pos,CHARCOLOR,&AttributePosition)&0xf0;
                       TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARCOLOR,TmpAttribute3|TmpAttribute4,
                                  &GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);

                       CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                    }

                    GlobalNotDisplay=0;
                    GlobalTextBlockStart=GlobalTextBlockEnd=-1;

                    TableBoxDraw(GlobalBoxHeadHandle);
                    CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&pos,&Tmp);
                    UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                    break;
                }

                if(GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  if (GlobalTextBlockStart<GlobalTextBlockEnd)
                     pos = GlobalTextBlockStart;
                  else
                     pos = GlobalTextPosition;

                  SaveUndoNum=UndoOperateSum;
                  TmpAttribute4=TextSearchAttribute(GlobalBoxHeadHandle,
                           pos,CHARCOLOR,&AttributePosition)&0xf0;
                  TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                  GlobalTextBlockEnd-GlobalTextBlockStart,
                                  CHARCOLOR,TmpAttribute3|TmpAttribute4,
                                  &GlobalTextPosition,
                                  &GlobalTextBlockStart,&GlobalTextBlockEnd);
                  UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                } else

                if ( GlobalBoxTool==IDX_INPUTBOX
                 && BoxIsPictureBox(GlobalBoxHeadHandle))
                {
                   int OldColor;
                   if (!PictureBoxGetPictureColor(GlobalBoxHeadHandle,&OldColor))
                   {
                        TmpAttribute4=OldColor&0xf0;
                        PictureBoxSetPictureColor(GlobalBoxHeadHandle,
                               TmpAttribute3|TmpAttribute4);
                   }
                }
                break;

           case MENU_ALIGNMENTLEFT:
                Result=ALIGNLEFT;
            lbl_align:
                if (BoxIsTableBox(GlobalBoxHeadHandle))
                {
                     Align=PARAGRAPHALIGN;
                     goto lbl_change_table_align;
                }

                if( GlobalBoxTool==IDX_INPUTBOX
                  && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  if (GlobalTextBlockStart<GlobalTextBlockEnd)
                     pos = GlobalTextBlockStart;
                  else
                     pos = GlobalTextPosition;

                  SaveUndoNum=UndoOperateSum;
                  Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                           pos,PARAGRAPHALIGN,&AttributePosition)&0xf0;
                  TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                    GlobalTextBlockEnd-GlobalTextBlockStart,
                                    PARAGRAPHALIGN,Tmp|Result,&GlobalTextPosition,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);
                  UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;
           case MENU_ALIGNMENTRIGHT:
                Result=ALIGNRIGHT;
                goto lbl_align;
           case MENU_ALIGNMENTMIDDLE:
                Result=ALIGNCENTRE;
                goto lbl_align;
           case MENU_ALIGNMENTAVERAGE:
                Result=ALIGNLEFTRIGHT;
                goto lbl_align;

          case MENU_ROWGAP175:  // zjh 96.6
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP175;
                  goto lbl_rowgap;
                }
                break;

          case MENU_ROWGAP125:  // zjh 96.6
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP125;
                  goto lbl_rowgap;
                }
                break;


          case MENU_ROWGAPUSER:  // zjh 96.6
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  if(MakeDialogBox(Window,RowGapDialog))
                        break;

                  Tmp=TmpAttribute;
                  Result=LINEGAPUSER+(Tmp%10);
                  Tmp=(Tmp/10)%8;
                  Result=Result|(Tmp<<8);

                  goto lbl_rowgap;
                }
                break;

          case MENU_ROWGAP1:  // jerry
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP1;
                  goto lbl_rowgap;
                }
                break;

          case MENU_ROWGAP15:  // jerry
                if (GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP15;
                  goto lbl_rowgap;
                }
                break;

          case MENU_ROWGAP2:  // jerry
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP2;

               lbl_rowgap:
                  if( BoxIsTableBox(GlobalBoxHeadHandle)   // ByHance,97,8.12
                  && GlobalTableBlockStart!=GlobalTableBlockEnd)
                  {
                    reg1=GlobalTableBlockStart;
                    reg2=GlobalTableBlockEnd;
                    if (reg1>reg2)
                    {
                      iCell=reg1; reg1=reg2; reg2=iCell;
                    }

                    FBGetCellRect(GlobalBoxHeadHandle,reg1,&col1,&row1,&Right,&Bottom);
                    FBGetCellRect(GlobalBoxHeadHandle,reg2,&Left,&Top,&col2,&row2);

                    SaveUndoNum=UndoOperateSum;

                    CancelCellBlock(GlobalBoxHeadHandle);
                    GlobalNotDisplay=1;

                    for (iCell=reg1;iCell<=reg2;iCell++)
                    {
                       FBGetCellRect(GlobalBoxHeadHandle,iCell,
                                        &Left,&Top,&Right,&Bottom);
                       if (Left<col1 || Top<row1 || Right>col2 || Bottom>row2)
                            continue;

                       GlobalTextBlockStart=pos
                            =TableCellGetTextHead(GlobalBoxHeadHandle,iCell);
                       GlobalTextBlockEnd=GlobalTextBlockStart
                            +TableCellGetTextLength(GlobalBoxHeadHandle,iCell);

                       TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                    GlobalTextBlockEnd-GlobalTextBlockStart,
                                    ROWGAP,Result,&GlobalTextPosition,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);

                       CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                    }

                    GlobalNotDisplay=0;
                    GlobalTextBlockStart=GlobalTextBlockEnd=-1;

                    TableBoxDraw(GlobalBoxHeadHandle);
                    CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&pos,&Tmp);
                    UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                    break;
                  }

                  if (GlobalTextBlockStart<GlobalTextBlockEnd)
                     pos = GlobalTextBlockStart;
                  else
                     pos = GlobalTextPosition;

                  SaveUndoNum=UndoOperateSum;
                  //Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                    //       pos,PARAGRAPHALIGN,&AttributePosition)&0xf;
                  TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                    GlobalTextBlockEnd-GlobalTextBlockStart,
                 // ByHance, 96,1.18 PARAGRAPHALIGN,Tmp|Result,&GlobalTextPosition,
                                    ROWGAP,Result,&GlobalTextPosition,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);
                  UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;

           /*  By zjh for ColGap   */
           //--------------------------start --------------
           case MENU_COLGAP175:  // zjh 96.6
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP175+0x10;
                  goto lbl_colgap;
                }
                break;

          case MENU_COLGAP125:  // zjh 96.6
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP125+0x10;
                  goto lbl_colgap;
                }
                break;


          case MENU_COLGAPUSER:  // zjh 96.6
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  if(MakeDialogBox(Window,ColGapDialog))
                        break;

                  Tmp=TmpAttribute;
                  Result=LINEGAPUSER+0x10+(Tmp%10);
                  Tmp=(Tmp/10)%8;
                  Result=Result|(Tmp<<8);

                  goto lbl_colgap;
                }
                break;

          case MENU_COLGAP1:  // zjh
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP1+0x10;
                  goto lbl_colgap;
                }
                break;

          case MENU_COLGAP15:  // zjh
                if (GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP15+0x10;
                  goto lbl_colgap;
                }
                break;

          case MENU_COLGAP2:  // jerry
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Result=LINEGAP2+0x10;


               lbl_colgap:
                  if (GlobalTextBlockStart<GlobalTextBlockEnd)
                     pos = GlobalTextBlockStart;
                  else
                     pos = GlobalTextPosition;

                  SaveUndoNum=UndoOperateSum;
                  //Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                    //       pos,PARAGRAPHALIGN,&AttributePosition)&0xf;
                  TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                    GlobalTextBlockEnd-GlobalTextBlockStart,
                 // ByHance, 96,1.18 PARAGRAPHALIGN,Tmp|Result,&GlobalTextPosition,
                                    COLGAP,Result,&GlobalTextPosition,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);
                  UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;
           //-----------------------ColGap---end-------------

           case MENU_UPDOWN:  // By zjh
                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                if(MakeDialogBox(Window,UpDownDialog))
                        break;

                  Tmp=TmpAttribute;
                  if (Tmp>=0) Result=Tmp&0x7ff;
                  else
                  Result=(((-Tmp)&0x7ff)|0x400);

                  if (GlobalTextBlockStart<GlobalTextBlockEnd)
                     pos = GlobalTextBlockStart;
                  else
                     pos = GlobalTextPosition;

                  SaveUndoNum=UndoOperateSum;
                  TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                    GlobalTextBlockEnd-GlobalTextBlockStart,
                                    UPDOWN,Result,&GlobalTextPosition,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);
                  UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;
           //-----------------------UpDown---end-------------

           case MENU_SUBLINE0:  // By zjh  12.10/96
           case MENU_SUBLINE1:  // By zjh  12.10
           case MENU_SUBLINE2:  // By zjh  12.10
           case MENU_SUBLINE3:  // By zjh  12.10
           case MENU_SUBLINE4:  // By zjh  12.10

                if( GlobalBoxTool==IDX_INPUTBOX
                 && BoxCanEditable(GlobalBoxHeadHandle))
                {
                  Tmp=Param1-MENU_SUBLINE0;
                  if (Tmp>=0&&Tmp<5)
                     Result=Tmp;
                  else
                     break;

                  if (GlobalTextBlockStart<GlobalTextBlockEnd)
                     pos = GlobalTextBlockStart;
                  else
                     pos = GlobalTextPosition;

                  SaveUndoNum=UndoOperateSum;
                  TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                    GlobalTextBlockEnd-GlobalTextBlockStart,
                                    SUBLINE,Result,&GlobalTextPosition,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);
                  UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;
           //=========================================================

           case MENU_IMPORTTEXT:
                if (GlobalBoxHeadHandle<=0)
                   break;

                //if (BoxCanEditable(GlobalBoxHeadHandle))
                if (BoxIsTextBox(GlobalBoxHeadHandle))
                {
                   if( GetFileName(Window,"*.TXT;*.WPS;*.WRI;*.*",TmpString,0,IMPTEXT_TITLE) )
                      break;

                   MouseSetGraph(BUSYMOUSE);
                   //GlobalNotDisplay=1;                // ByHance, 95,12.8
                   Result=TextBoxInsertTextFile(TmpString,
                          GlobalBoxHeadHandle,&GlobalBoxHeadHandle,
                          GlobalTextPosition,&GlobalTextPosition,
                          &GlobalTextBlockStart,&GlobalTextBlockEnd);
                   //GlobalNotDisplay=0;
                   MouseSetGraph(ARRAWMOUSE);
                   if (Result<OpOK)
                   {
                      MessageBox(GetTitleString(ERRORINFORM),
                                 GetInformString(FILELOADERROR),1,
                                 Window);
                   }
                   else {
                      FileSetModified();
                      TellStatus();               // ByHance
                   }
                   break;
                }
                else   // case MENU_IMPORTPICTURE:
                {
                  PictureBoxs *BoxPointer;

                  if (GlobalBoxHeadHandle<=0)
                     break;
                  BoxPointer=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                  if (BoxPointer==NULL)
                     return(OUTOFMEMORY);

                  Result=(PictureBoxGetBoxType(BoxPointer)>=RECTANGLEPICTUREBOX
                      && PictureBoxGetBoxType(BoxPointer)<=POLYGONPICTUREBOX );
                  HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));

                  if (Result)
                  {
                     Result=GetFileName(Window,"*.TIF;*.PCX;*.BMP",TmpString,0,IMPIMAGE_TITLE);
                     if (Result)
                        break;

                     MouseSetGraph(BUSYMOUSE);
                     Result=PictureBoxImportTiff(TmpString,GlobalBoxHeadHandle);
                     MouseSetGraph(ARRAWMOUSE);
                     if (Result<OpOK)
                     {
                        MessageBox(GetTitleString(ERRORINFORM),
                                   GetInformString(FILELOADERROR),1,
                                   Window);
                     }
                     else
                     {
                        FileSetModified();
                        // PictureBoxDisplayPicture(GlobalBoxHeadHandle);
                        BoxDrawBorder(GlobalBoxHeadHandle,0);
                     }
                  }
                }
                break;
           case MENU_INSERTPAGE:
                if (GlobalCurrentPage>0&&FileHasBeenLoaded()&&CanOperatePage())
                {
                   MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
                   MidHPage=PageNew(MidPage,PageHandleToNumber(GlobalCurrentPage));
                   HandleUnlock(ItemGetHandle(GlobalCurrentPage));
                   FileSetModified();
                   if (MidHPage)
                      PageChangeCurrent(MidHPage);
                }
                break;
           case MENU_APPENDPAGE:
                if (GlobalCurrentPage>0&&FileHasBeenLoaded()&&CanOperatePage())
                {
                   MidPage=HandleLock(ItemGetHandle(GlobalCurrentPage));
                   MidHPage=PageNew(MidPage,PageHandleToNumber(GlobalCurrentPage)+1);
                   HandleUnlock(ItemGetHandle(GlobalCurrentPage));
                   FileSetModified();
                   if (MidHPage)
                      PageChangeCurrent(MidHPage);
                }
                break;
           case MENU_DELETEPAGE:
                if (GlobalCurrentPage>0&&CanOperatePage()&&TotalPage>1)
                {
                   Result=MessageBox(GetTitleString(WARNINGINFORM),
                                   "×¢Òâ: Ò³É¾³ý²Ù×÷½«ÎÞ·¨¸´Ô­!\n"
                                   "ÕæµÄÒªÉ¾³ýµ±Ç°Ò³?",
                                    2,Window);
                   if (Result==1)       // cancel
                      break;

                   PageNumber=PageHandleToNumber(GlobalCurrentPage);
                   PageDelete(PageNumber);
                   FileSetModified();
                   if (TotalPage<=0)
                      PageChangeCurrent(-1);
                   else
                      if (PageNumber>=TotalPage)
                         PageChangeCurrent(PageNumberToHandle(TotalPage-1));
                      else
                         PageChangeCurrent(PageNumberToHandle(PageNumber));
                }
                break;
           case MENU_PAGEFEED:
                if (GlobalCurrentPage>0&&BoxIsTextBox(GlobalBoxHeadHandle))
                {
                   *(Wchar *)&TmpString[0]=0xc;
                   TextBoxEnterKey(GlobalBoxHeadHandle,&NewHBox,
                                 GlobalTextPosition,&GlobalTextPosition,
                                 1,&GlobalTextBlockStart,
                                 &GlobalTextBlockEnd,(Wchar *)TmpString);
                }
                break;
           case MENU_PREVPAGE:
                if (GlobalCurrentPage>0&&CanOperatePage())
                {
                   PageNumber=PageHandleToNumber(GlobalCurrentPage);
                   if (PageNumber>0)
                      PageChangeCurrent(PageNumberToHandle(PageNumber-1));
                }
                break;
           case MENU_NEXTPAGE:
                if (GlobalCurrentPage>0&&CanOperatePage())
                {
                   PageNumber=PageHandleToNumber(GlobalCurrentPage);
                   if (PageNumber<TotalPage-1)
                      PageChangeCurrent(PageNumberToHandle(PageNumber+1));
                }
                break;
           case MENU_FIRSTPAGE:
                if (GlobalCurrentPage>0&&CanOperatePage())
                {
                   PageNumber=PageHandleToNumber(GlobalCurrentPage);
                   if (PageNumber>0)
                      PageChangeCurrent(PageNumberToHandle(0));
                }
                break;
           case MENU_LASTPAGE:
                if (GlobalCurrentPage>0&&CanOperatePage())
                {
                   PageNumber=PageHandleToNumber(GlobalCurrentPage);
                   if (PageNumber<TotalPage-1)
                      PageChangeCurrent(PageNumberToHandle(TotalPage-1));
                }
                break;
           case MENU_GOTOPAGE:         /////////By Jerry
                if (GlobalCurrentPage>0&&FileHasBeenLoaded()&&CanOperatePage())
                  if (!MakeDialogBox(Window,GotoPageDialog))
                    PageChangeCurrent(PageNumberToHandle(GetPageNumber()));
                break;
           case MENU_MOVEPAGE:
                if (GlobalCurrentPage>0&&FileHasBeenLoaded()&&CanOperatePage())
                  if (!MakeDialogBox(Window,MovePageDialog))
                  {
                      MovePage(PageHandleToNumber(GlobalCurrentPage),
                               GetPageNumber(),GetPageMoveOption());
                      FileSetModified();
                  }
                break;
           case MENU_COPY:
                if (GlobalBoxHeadHandle<=0)
                   break;
                if (GlobalBoxTool==IDX_INPUTBOX)
                {
                   if (BoxCanEditable(GlobalBoxHeadHandle)
                       &&GlobalTextBlockEnd>GlobalTextBlockStart)
                      ClipBoardInsertText(GlobalBoxHeadHandle,
                                          GlobalTextBlockStart,
                                          GlobalTextBlockEnd,1);
                   else
                      if (BoxIsPictureBox(GlobalBoxHeadHandle))
                         ClipBoardInsertImage(GlobalBoxHeadHandle,1);
                }
                else
                   if (GlobalBoxTool==IDX_SELECTBOX)
                      ClipBoardInsertBox(GlobalBoxHeadHandle,1);
                break;
           case MENU_PASTE:
                if (GlobalCurrentPage<=0)
                   break;
                MouseSetGraph(BUSYMOUSE);
                ClipBoardRead();
                switch (GlobalBoxTool)
                {
                 case IDX_INPUTBOX:
                   if (GlobalBoxHeadHandle<=0)
                      break;
                   if (BoxCanEditable(GlobalBoxHeadHandle))
                   {
                      ClipBoards ClipBoardDataInformation;
                      HANDLE TextHBlock;
                      Wchar *TextBlock;
                      int ReadLength,StartChangeLine,ChangeLines;
                      int CursorX,CursorY;

                      ClipBoardGetDataInfomation(&ClipBoardDataInformation);
                      if (ClipBoardDataInformation.ClipBoardDataLength==0)
                         break;

                      if (ClipBoardDataInformation.ClipBoardDataType==BOXDATA)
                      {
                         HBOX HBox;

                         if(BoxIsTableBox(GlobalBoxHeadHandle))
                            break;

                         HBox=ClipBoardGetBox(GETTOTEXTBOX);
                         if (HBox)
                            TextBoxInsertBox(GlobalBoxHeadHandle,&NewHBox,
                                             GlobalTextPosition,
                                             &GlobalTextPosition,
                                             &GlobalTextBlockStart,
                                             &GlobalTextBlockEnd,HBox);
                         break;
                      }

                      if (ClipBoardDataInformation.ClipBoardDataType!=TEXTDATA)
                         break;

                      if(BoxIsTableBox(GlobalBoxHeadHandle)     // 96,4.8
                      && ClipBoardDataInformation.ClipBoardDataLength>=40)
                         break;

                      TextHBlock=HandleAlloc(         // Must Changed! ByHance
                        ClipBoardDataInformation.ClipBoardDataLength,0);
                      if (!TextHBlock)
                         break;        // !!! Out of memory
                      TextBlock=HandleLock(TextHBlock);
                      if (TextBlock==NULL)
                         break;        // !!! Out of memory
                      ReadLength=ClipBoardGetText(TextBlock);
                      if (ReadLength>0)
                      {
                         SaveUndoNum=UndoOperateSum;

                         if (GlobalTextBlockEnd>GlobalTextBlockStart)
                         {
                            HandleUnlock(TextHBlock);
                            MessageGo(Window,KEYDOWN,DEL,1);
                            TextBlock=HandleLock(TextHBlock);
                            if (TextBlock==NULL)
                               break;
                         }
                         ReadLength=TextBoxInsertString(GlobalBoxHeadHandle,
                                             GlobalTextPosition,
                                             TextBlock,ReadLength);
                         HandleUnlock(TextHBlock);
                         HandleFree(TextHBlock);
                         if (ReadLength)
                         {
                            UndoInsertCursorGoto(GlobalTextPosition);
//                          UndoInsertCursorDefineBlock(GlobalTextBlockStart,
//                                                      GlobalTextBlockEnd);
//                          GlobalTextBlockStart=GlobalTextPosition;
//                          GlobalTextBlockEnd=GlobalTextPosition+ReadLength;
                            GlobalTextBlockStart=-1;
                            GlobalTextBlockEnd=-1;
                            FormatInsertText(GlobalBoxHeadHandle,GlobalTextPosition,
                                 ReadLength,&StartChangeLine,&ChangeLines, FALSE);

                            GlobalTextPosition+=ReadLength; //ByHance,96,1.19

                            if(BoxIsTableBox(GlobalBoxHeadHandle)) // ByHance, 96,4.8
                               AdjustTableCells(GlobalBoxHeadHandle);

                            if (!CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                      GlobalTextPosition,&CursorX,&CursorY))
                               TextBoxRedraw(NewHBox,StartChangeLine,ChangeLines,FALSE);

                            TellStatus();
                         }
                         UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                      }
                      else
                      {
                         HandleUnlock(TextHBlock);
                         HandleFree(TextHBlock);
                      }
                   }    //end of text box
                   else
                   if (BoxIsPictureBox(GlobalBoxHeadHandle))
                   {
                      InsertImages InsertImage;

                      if (ClipBoardGetImage(&InsertImage)>0)
                      {
                         PictureBoxs *PictureBox;
                         ImageDescribes *TiffPresent;

                         PictureBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                         if (PictureBox==NULL)
                            break;  // !!! Out of memory
                         if (strcmp(InsertImage.InsertFileName,
                             PictureBoxGetPictureFileName(PictureBox)))
                         {
                            TiffPresent=&(PictureBoxGetPicturePresent(PictureBox));
                            PictureBoxSetPictureFileName(PictureBox,
                                          InsertImage.InsertFileName);
                            memcpy(TiffPresent,&InsertImage.InsertPresent,
                                   sizeof(*TiffPresent));
                            MouseSetGraph(BUSYMOUSE);
                            PictureBoxImportTiff(PictureBoxGetPictureFileName(PictureBox),
                                                 GlobalBoxHeadHandle);
                            MouseSetGraph(ARRAWMOUSE);
                         }
                         HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                      }
                   }
                   break;
                 case IDX_SELECTBOX:
                      if (ClipBoardGetBox(GETTOPAGE)>0)
                      {
                         BoxChange(GlobalBoxHeadHandle,GlobalCurrentPage);
                         RedrawUserField();
                         FileSetModified();
                      }
                      break;
                }       // end of switch GlobalBoxTool
                MouseSetGraph(ARRAWMOUSE);
                break;
           case MENU_CUT:
                if (GlobalBoxHeadHandle<=0)
                   break;
                switch (GlobalBoxTool)
                {
                   case IDX_INPUTBOX:
                       if (BoxCanEditable(GlobalBoxHeadHandle)
                           &&GlobalTextBlockEnd>GlobalTextBlockStart)
                       {
                          ClipBoardInsertText(GlobalBoxHeadHandle,
                                              GlobalTextBlockStart,
                                              GlobalTextBlockEnd,1);
                       }
                       else
                       {
                          if (BoxIsPictureBox(GlobalBoxHeadHandle))
                             ClipBoardInsertImage(GlobalBoxHeadHandle,1);
                       }
                       MessageGo(Window,KEYDOWN,DEL,1);
                       break;
                   case IDX_SELECTBOX:
                       ClipBoardInsertBox(GlobalBoxHeadHandle,1);
                       MessageGo(Window,KEYDOWN,DEL,1);
                       break;
                }
                break;
           case MENU_CLEAR:
                MessageGo(Window,KEYDOWN,DEL,1);
                break;
           case MENU_UNDO:
                Undo();
                break;
           case MENU_VIEWTOOLS:
                ToolBarWindowModify(Window);
                break;
           case MENU_CLIBRATION:
                ClibrationWindowModify(Window);
                break;
           case MENU_FIND:
                if( BoxCanEditable(GlobalBoxHeadHandle)
                && GlobalBoxTool==IDX_INPUTBOX)
                {
                   FindStructs TmpFindStruct;

                   memcpy(&TmpFindStruct,&GlobalFindStruct,sizeof(FindStructs));
                   if (!MakeDialogBox(Window,FindDialog))
                   {
                      if( BoxIsTableBox(GlobalBoxHeadHandle)   // ByHance,97,8.12
                      && GlobalTableBlockStart!=GlobalTableBlockEnd)
                         CancelCellBlock(GlobalBoxHeadHandle);

                      FindClearReplaceSign(GlobalFindStruct);
                      FindText(&GlobalBoxHeadHandle,
                               &GlobalFindStruct,
                               &GlobalTextPosition,
                               &GlobalTextBlockStart,
                               &GlobalTextBlockEnd);
                   }
                   else
                      memcpy(&GlobalFindStruct,&TmpFindStruct,
                             sizeof(FindStructs));
                }
                break;
           case MENU_REPLACE:
                if( BoxCanEditable(GlobalBoxHeadHandle)
                  && GlobalBoxTool==IDX_INPUTBOX)
                {
                   FindStructs TmpFindStruct;

                   memcpy(&TmpFindStruct,&GlobalFindStruct,sizeof(FindStructs));
                   if (!MakeDialogBox(Window,ReplaceDialog))
                   {
                      if( BoxIsTableBox(GlobalBoxHeadHandle)   // ByHance,97,8.12
                      && GlobalTableBlockStart!=GlobalTableBlockEnd)
                         CancelCellBlock(GlobalBoxHeadHandle);

                      FindSetReplaceSign(GlobalFindStruct);
                      FindText(&GlobalBoxHeadHandle,
                               &GlobalFindStruct,
                               &GlobalTextPosition,
                               &GlobalTextBlockStart,
                               &GlobalTextBlockEnd);
                   }
                   else
                      memcpy(&GlobalFindStruct,&TmpFindStruct,
                             sizeof(FindStructs));
                }
                break;
           case MENU_NEXT:                      // find next
                if( BoxCanEditable(GlobalBoxHeadHandle)
                  && GlobalBoxTool==IDX_INPUTBOX)
                {
                    if( BoxIsTableBox(GlobalBoxHeadHandle)   // ByHance,97,8.12
                    && GlobalTableBlockStart!=GlobalTableBlockEnd)
                         CancelCellBlock(GlobalBoxHeadHandle);

                    FindorReplaceNext(&GlobalBoxHeadHandle,
                                      &GlobalFindStruct,
                                      &GlobalTextPosition,
                                      &GlobalTextBlockStart,
                                      &GlobalTextBlockEnd);
                }
                break;

           case MENU_BOXLOCK:      ///////Jerry
                //if( (GlobalBoxTool==IDX_SELECTBOX||GlobalBoxTool==IDX_INPUTBOX)
                //    &&GlobalCurrentPage>0)
                if(GlobalCurrentPage>0)
                {
                   if (GlobalGroupGetSign())
                   {
                      GroupLock();
                      FileSetModified();
                   }
                   else
                   if (GlobalBoxHeadHandle>0)
                   {
                      BoxPointer=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                      if (BoxPointer != NULL)
                      {
                         BoxSetLocked(BoxPointer);
                         HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                         FileSetModified();
                      }
                   }
                }
                break;

           case MENU_BOXUNLOCK:      ///////Jerry
               // if( (GlobalBoxTool==IDX_SELECTBOX||GlobalBoxTool==IDX_INPUTBOX)
               //     &&GlobalCurrentPage>0)
                if(GlobalCurrentPage>0)
                {
                   if (GlobalGroupGetSign())
                   {
                      GroupUnlock();
                      FileSetModified();
                   }
                   else
                      if (GlobalBoxHeadHandle)
                      {
                        if (GlobalBoxHeadHandle<=0)
                           break;
                        BoxPointer=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                        if (BoxPointer != NULL) BoxSetUnlocked(BoxPointer);
                        FileSetModified();
                      }
                }
                break;

           case MENU_FORWARD:
                if ( // GlobalBoxTool==IDX_SELECTBOX &&         //ByHance
                    GlobalCurrentPage>0)
                {
                   if (GlobalBoxHeadHandle)
                   {
                      BoxSetForward(GlobalBoxHeadHandle);
                      BoxChangeAll(GlobalCurrentPage);
                      RedrawUserField();
                      FileSetModified();
                      TellStatus();
                   }
                }
                break;
           case MENU_BACKWARD:
                if ( // GlobalBoxTool==IDX_SELECTBOX &&
                    GlobalCurrentPage>0)
                {
                   if (GlobalBoxHeadHandle)
                   {
                      BoxSetBackward(GlobalBoxHeadHandle);
                      BoxChangeAll(GlobalCurrentPage);
                      FileSetModified();
                      RedrawUserField();
                      TellStatus();
                   }
                }
                break;
           case MENU_FRONT:
                if ( // GlobalBoxTool==IDX_SELECTBOX &&
                    GlobalCurrentPage>0)
                {
                   if (GlobalGroupGetSign())
                   {
                      GroupSetFront();
                      BoxChangeAll(GlobalCurrentPage);
                      RedrawUserField();
                      FileSetModified();
                      TellStatus();
                   }
                   else
                   if (GlobalBoxHeadHandle)
                   {
                      BoxSetFront(GlobalBoxHeadHandle);
                      BoxChangeAll(GlobalCurrentPage);
                      FileSetModified();
                      RedrawUserField();
                      TellStatus();
                   }
                }
                break;
           case MENU_BACK:
                if ( // GlobalBoxTool==IDX_SELECTBOX &&
                    GlobalCurrentPage>0)
                {
                   if (GlobalGroupGetSign())
                   {
                      GroupSetBack();
                      BoxChangeAll(GlobalCurrentPage);
                      RedrawUserField();
                      FileSetModified();
                      TellStatus();
                   }
                   else
                   if (GlobalBoxHeadHandle)
                   {
                      BoxSetBackground(GlobalBoxHeadHandle);
                      BoxChangeAll(GlobalCurrentPage);
                      RedrawUserField();
                      FileSetModified();
                      TellStatus();
                   }
                }
                break;

       /*-------------------- not used now ------
           case MENU_TABLESLIP:
                if (BoxIsTableBox(GlobalBoxHeadHandle))
                   if (FBDisembleCell(GlobalBoxHeadHandle,GlobalTableCell)>=0)
                   {
                      GlobalTableBlockStart=GlobalTableBlockEnd=-1;
                      TableBoxClear(GlobalBoxHeadHandle);
                      BoxDrawBorder(GlobalBoxHeadHandle,0);
                      TableBoxDraw(GlobalBoxHeadHandle);
                      FileSetModified();
                   }
                break;
           case MENU_LINESTYLE:
           case MENU_LINEHEAD:
           case MENU_LINETAIL:
                break;
           case MENU_LINEWIDTH0:
                Result=0;
                goto SetLineWidth;
           case MENU_LINEWIDTH1:
                Result=1;
                goto SetLineWidth;
           case MENU_LINEWIDTH2:
                Result=2;
                goto SetLineWidth;
           case MENU_LINEWIDTH4:
                Result=4;
                goto SetLineWidth;
           case MENU_LINEWIDTH6:
                Result=6;
                goto SetLineWidth;
           case MENU_LINEWIDTH8:
                Result=8;
                goto SetLineWidth;
           case MENU_LINEWIDTH12:
                Result=12;
                goto SetLineWidth;
           case MENU_USERLINEWIDTH:
                Result=10;
             SetLineWidth:
                Result=Result*SCALEMETER/72;
                if (GlobalBoxHeadHandle>0&&GlobalCurrentPage>0
                    &&BoxIsLineBox(GlobalBoxHeadHandle))
                {
                   LineBoxs *LineBox;

                   LineBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                   if (LineBox==NULL)
                      return(OUTOFMEMORY);
                   if (Result==LineBoxGetBoxHeight(LineBox))
                   {
                      HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                      break;
                   }
                   LineBoxSetBoxHeight(LineBox,Result);
                   HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                   BoxChange(GlobalBoxHeadHandle,GlobalCurrentPage);
                   FileSetModified();
                   RedrawUserField();
                }
                break;

           case MENU_LINECOLOR:
                if (GlobalBoxHeadHandle>0&&GlobalCurrentPage>0
                    &&BoxIsLineBox(GlobalBoxHeadHandle))
                {
                   LineBoxs *LineBox;
                   int OldColor;

                   LineBox=HandleLock(ItemGetHandle(GlobalBoxHeadHandle));
                   if (LineBox==NULL)
                      return(OUTOFMEMORY);
                   OldColor=LineBoxGetBoxBackColor(LineBox);
                   TmpAttribute3=OldColor&0xf;
                   TmpAttribute4=(OldColor&0xf0)>>4;
                   CharColorDialog[4].ItemProcedure=ImageColorProcedure;
                   CharColorDialog[6].ItemProcedure=ImageShadowProcedure;
                   if (!MakeDialogBox(Window,CharColorDialog))
                   {
                      OldColor=TmpAttribute3|(TmpAttribute4<<4);
                      LineBoxSetBoxBackColor(LineBox,OldColor);
                   }
                   CharColorDialog[4].ItemProcedure=CharColorProcedure;
                   CharColorDialog[6].ItemProcedure=CharShadowProcedure;
                   HandleUnlock(ItemGetHandle(GlobalBoxHeadHandle));
                   FileSetModified();
                }
                break;

           case MENU_FONTUSE:
                break;
           case MENU_PICTUREUSE:
                break;

           case MENU_LOADLIB:
                if (LibraryLoad("BLOCKLIB.001",0,&GlobalCurrentPage,
                            &GlobalBoxHeadHandle)>=0)
                {
                   BoxChangeFrom(GlobalBoxHeadHandle);
                   RedrawUserField();
                   FileSetModified();
                   FileSetModified();
                }
                break;
           case MENU_SAVELIB:
                LibrarySave("BLOCKLIB.001",1);
                break;
           case MENU_DELETELIB:
                LibraryDelete("BLOCKLIB.001",1);
                break;
           case MENU_IMAGENORMALCONTRAST:
           case MENU_IMAGEHIGHCONTRAST:
           case MENU_IMAGEPOSTERIZE:
                {
                  int OldContrast;

                  if (GlobalBoxHeadHandle>0&& GlobalBoxTool==IDX_INPUTBOX &&
                      BoxIsPictureBox(GlobalBoxHeadHandle)>0)
                  {
                     Result=PictureBoxGetPictureContrast(GlobalBoxHeadHandle,
                                                         &OldContrast);
                     if (Result>0)
                     {
                        if (Param1==MENU_IMAGENORMALCONTRAST)
                           OldContrast=NORMALCONTRAST;
                        else
                           if (Param1==MENU_IMAGEHIGHCONTRAST)
                              OldContrast=HIGHCONTRAST;
                           else
                              OldContrast=POSTERIZED;

                        PictureBoxSetPictureNewContrast(GlobalBoxHeadHandle,
                                                        OldContrast);
                        FileSetModified();
                     }
                  }
                }
                break;
           case MENU_IMAGENEGATIVE:
                if (GlobalBoxHeadHandle>0&& GlobalBoxTool==IDX_INPUTBOX &&
                    BoxIsPictureBox(GlobalBoxHeadHandle)>0)
                   PictureBoxSetPictureNegative(GlobalBoxHeadHandle);
                break;

           case MENU_IMAGECOLOR:
                {
                  int OldColor;

                  if (GlobalBoxHeadHandle>0&& GlobalBoxTool==IDX_INPUTBOX &&
                      BoxIsPictureBox(GlobalBoxHeadHandle)>0)
                  {
                     Result=PictureBoxGetPictureColor(GlobalBoxHeadHandle,
                                                      &OldColor);
                     if (Result>0)
                     {
                        TmpAttribute3=OldColor&0xf;
                        TmpAttribute4=(OldColor&0xf0)>>4;
                        CharColorDialog[4].ItemProcedure=ImageColorProcedure;
                        CharColorDialog[6].ItemProcedure=ImageShadowProcedure;
                        if (!MakeDialogBox(Window,CharColorDialog))
                        {
                           PictureBoxSetPictureColor(GlobalBoxHeadHandle,
                                                     TmpAttribute);
                        }
                        CharColorDialog[4].ItemProcedure=CharColorProcedure;
                        CharColorDialog[6].ItemProcedure=CharShadowProcedure;
                     }
                  }
                }
                break;

           case MENU_INCH:
                if (FileMETERIsCM())
                {
                   FileSetMeterINCH();
                   if (ToolBarHasRulerBar())
                      goto draw_ruler;
                }
                break;
           case MENU_CM:
                if (FileMETERIsINCH())
                {
                   FileSetMeterCM();
                   if (ToolBarHasRulerBar())
                   {
                   draw_ruler:
                      HWND MidWindow;
                      MidWindow=SearchHClibrationWindow(Window);
                      if (MidWindow)
                         MessageInsert(MidWindow,REDRAWMESSAGE,0l,
                                       MAKELONG(WindowGetWidth(MidWindow),
                                                WindowGetHeight(MidWindow)));
                      MidWindow=SearchVClibrationWindow(Window);
                      if (MidWindow)
                         MessageInsert(MidWindow,REDRAWMESSAGE,0l,
                                       MAKELONG(WindowGetWidth(MidWindow),
                                                WindowGetHeight(MidWindow)));
                   }
                }
                break;
           case MENU_SELECTALL:
                if (GlobalBoxTool==IDX_SELECTBOX
                    &&GlobalCurrentPage>0)
                {
                   GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                   GroupAll(GlobalCurrentPage);
                   GroupDrawAllBorder(DRAWVIRTUALBORDOR|DRAWXORBORDER);
                }
                break;
        --------------------------------*/


           case MENU_PRINTERSETUP:
                Result=CurrentPrinter;
                if (!MakeDialogBox(Window,PrinterSetupDialog)) {
                  if(Result!=CurrentPrinter)
                    SetProfileString( ProfileName,InitSection,
                         PrinterEntry,PrinterName[CurrentPrinter] );
                } else CurrentPrinter=Result;   //restore old value
                break;

/*
           case MENU_OPENPRINTCUT:
                Result=GetFileName(Window,"*.PNT",TmpString,0,OPENSET_TITLE);
                if (Result) break;
                fp=fopen(TmpString,"rb");
                if (fp==NULL)
                {
                    MessageBox(GetTitleString(ERRORINFORM),
                               GetInformString(FILELOADERROR),1,
                               Window);
                    break;
                }
                SetDefaultExternBlock();
                Result=ReadExternBlock(fp);
                fclose(fp);
                if (Result<0)
                {
                    MessageBox(GetTitleString(ERRORINFORM),
                                "ÎÄ¼þ¸ñÊ½²»¶Ô!",1,0);
                    SetDefaultExternBlock();
                }
                break;
           case MENU_SAVEPRINTCUT:
                Result=GetFileName(Window,"*.PNT",TmpString,1,SAVESET_TITLE);
                if (Result) break;
                fp=fopen(TmpString,"wb");
                if (fp==NULL)
                {
                  lbl_save_cut_err:
                    MessageBox(GetTitleString(ERRORINFORM),
                               GetInformString(FILESAVEERROR),1,
                               Window);
                    break;
                }
                Result=WriteExternBlock(fp);
                fclose(fp);
                if (Result<0)
                    goto lbl_save_cut_err;
                break;
           case MENU_PRINTCUT:
                if (!MakeDialogBox(Window,PrintCutDialog)&&UsePrintCut())
                {
                      MessageBox(GetTitleString(WARNINGINFORM),
                                            "   µ±ÄúÒÔºó´òÓ¡Ê±¾ù½«\n"
                                            "Ê¹ÓÃÄú¸Õ²ÅµÄÉèÖÃ,ÈôÒª\n"
                                            "È¡Ïû,ÇëÔÙ´Î½øÈë,Ñ¡Ôñ\n"
                                            "<È¡Ïû>°´Å¥"
                      ,1,0);
                }
                break;
           case MENU_PAGEEXPORTPICT:
                if(TotalPage<1)
                   break;          // no page
                Result=tmp_range;   // keep old value

                tmp_range=0;
                if (!MakeDialogBox(Window,ExportPictDialog))
                {
                   char *p;
                   switch(tmp_range)
                   {
                     case 0: strcpy(TmpString,".TIF"); break;
                     case 1: strcpy(TmpString,".PCX"); break;
                     case 2: strcpy(TmpString,".DCL"); break;
                   }
                   if(DebugFileName[0])
                      strcpy(PrintName,DebugFileName);
                   else
                      strcpy(PrintName,DefaultPrintName);
                   p=strchr(PrintName,'.');
                   if(p) *p=0;
                   strcat(PrintName,TmpString);

                   MouseSetGraph(BUSYMOUSE);
                   {
                      int currentPage=PageHandleToNumber(GlobalCurrentPage);

                      memset(&PG,0,sizeof(PG));
                      tmp_fileflag=1;   // print to file
                      PXScale=PYScale=1.0;
                      GlobalSubPage=0;
                      PrintCopyN=1;

                      if( SetPrinter(PIC_TIFF+tmp_range) >= OpOK )
                         PrintToDevice(currentPage,currentPage);

                      GlobalSubPage=1;
                   }
                   MouseSetGraph(ARRAWMOUSE);
                   PrintName[0]=0;     // for regist_serial_number
                }
                tmp_range=Result;   // restore old value
                break;
*/
           case MENU_PRINT:
                if(TotalPage<1)
                   break;          // no page

                if (!MakeDialogBox(Window,PrintDialog)) {
                   //-- return: EndPrintPage=total print page
                   //-- now, just it to real end print page
                    EndPrintPage +=StartPrintPage-1;
                    if(EndPrintPage>TotalPage)
                        EndPrintPage = TotalPage;

                    if(GetPrint2FileOption())
                    {
                       if(GetFileName(Window,"*.PRN",PrintName,1,PRNFILE_TITLE))
                           return(FALSE);
                    }
                    else strcpy(PrintName,DefaultPrintName);

                    MouseSetGraph(BUSYMOUSE);
                    if( SetPrinter(CurrentPrinter) >= OpOK )
                         PrintToDevice(StartPrintPage-1,EndPrintPage-1);
                    MouseSetGraph(ARRAWMOUSE);
                    PrintName[0]=0;     // for regist_serial_number
                }
                break;

           case MENU_PAGENUM:
                if (GlobalCurrentPage>0&&FileHasBeenLoaded())
                {
                     Result=GetPageFootOption();
                     Left=GetPageFootLeftOption();
                     Top=GetPageFootTopOption();
                     PageNumber=PgFtStartNum;
                     pos=GetPageFootPrevOption();

                     if (!MakeDialogBox(Window,PageFootDialog))
                     {
                         if( Result!=GetPageFootOption()
                         || ( GetPageFootOption() &&
                               (Left!=GetPageFootLeftOption()
                               || Top!=GetPageFootTopOption()
                               || Right!=PgFtStartNum
                               || pos!=GetPageFootPrevOption() )
                         ))
                         {
                               DrawScreenPageFootHead();
                               //RedrawUserField();
                               FileSetModified();
                         }
                     }
                }
                break;
           case MENU_PAGEHEAD:
                if (GlobalCurrentPage>0&&FileHasBeenLoaded())
                {
                     Result=GetPageHeadOption();
                     Left=GetPageHeadLeftOption();
                     pos=GetPageHeadLineOption();
                     strcpy(TmpString,PageHeadLeftStr);
                     strcpy(TmpString+100,PageHeadRightStr);

                     if (!MakeDialogBox(Window,PageHeadDialog))
                     {
                        // if one page_head_str is NULL, assign they are EQU
                         if(PageHeadRightStr[0]==0)
                             strcpy(PageHeadRightStr,PageHeadLeftStr);
                         else
                         if(PageHeadLeftStr[0]==0)
                             strcpy(PageHeadLeftStr,PageHeadRightStr);

                         if( Result!=GetPageHeadOption()
                         ||(GetPageHeadOption()&&(Left!=GetPageHeadLeftOption()
                                              ||pos!=GetPageHeadLineOption()) )
                         ||(GetPageHeadOption() && GetPageHeadLineOption()
                              && (strcmp(TmpString,PageHeadLeftStr)
                                 ||strcmp(TmpString+100,PageHeadRightStr) )
                         ))
                         {
                               DrawScreenPageFootHead();
                               FileSetModified();
                         }
                     }
                }
                break;

           case MENU_TABLEINSERTLINE:
                if (GlobalTableCell>=0&&BoxIsTableBox(GlobalBoxHeadHandle))
                {
                   if (MakeDialogBox(Window,TableInsLineDialog))
                      break;

                   Tmp=GetTableLineNumber()+GetTableInsOption();
                   //if (Tmp<0) Tmp=0;
                   Result=FBInsALine(GlobalBoxHeadHandle,Tmp,0,TRUE);
                   goto lbl_disp_all;
                }
                break;
           case MENU_TABLEINSERTCOLUMN:
                if (GlobalTableCell>=0&&BoxIsTableBox(GlobalBoxHeadHandle))
                {
                   if (MakeDialogBox(Window,TableInsColDialog))
                       break;

                   Tmp=GetTableLineNumber()+GetTableInsOption();
                   //if (Tmp<0) Tmp=0;
                   Result=FBInsACol(GlobalBoxHeadHandle,Tmp,0,TRUE);
                   goto lbl_disp_all;
                }
                break;
           case MENU_TABLEDELETELINE:
                if (GlobalTableCell>=0&&BoxIsTableBox(GlobalBoxHeadHandle))
                {
                   //if (MakeDialogBox(Window,TableDelLineDialog))
                     //  break;
                   //Result=FBDelALine(GlobalBoxHeadHandle,TmpFormBox.numLines-1,TRUE);
                   Tmp=TableBoxGetLineFromCell(GlobalBoxHeadHandle,GlobalTableCell);
                   Result=FBDelALine(GlobalBoxHeadHandle,Tmp,TRUE);
                   goto lbl_disp_all;
                }
                break;

            //By zjh 9.12
           case MENU_ALIGNUPDOWN:
                Align=VPARAGRAPHALIGN;
                Result=ALIGNUPDOWN;
                goto lbl_change_table_align;
           case MENU_ALIGNVCENTRE:
                Align=VPARAGRAPHALIGN;
                Result=ALIGNVCENTRE;
           lbl_change_table_align:
                if( BoxIsTableBox(GlobalBoxHeadHandle)
                && GlobalTableBlockStart!=GlobalTableBlockEnd)
                {
                  reg1=GlobalTableBlockStart;
                  reg2=GlobalTableBlockEnd;
                  if (reg1>reg2)
                  {
                    iCell=reg1; reg1=reg2; reg2=iCell;
                  }

                  FBGetCellRect(GlobalBoxHeadHandle,reg1,&col1,&row1,&Right,&Bottom);
                  FBGetCellRect(GlobalBoxHeadHandle,reg2,&Left,&Top,&col2,&row2);

                  SaveUndoNum=UndoOperateSum;

                  CancelCellBlock(GlobalBoxHeadHandle);
                  GlobalNotDisplay=1;
                  for (iCell=reg1;iCell<=reg2;iCell++)
                  {
                     FBGetCellRect(GlobalBoxHeadHandle,iCell,
                                      &Left,&Top,&Right,&Bottom);
                     if (Left<col1 || Top<row1 || Right>col2 || Bottom>row2)
                          continue;

                     pos = TableCellGetTextHead(GlobalBoxHeadHandle,iCell);
                     Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                              pos,Align,&AttributePosition)&0xf0;
                     TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                    GlobalTextBlockEnd-GlobalTextBlockStart,
                                    Align,Result|Tmp,&GlobalTextPosition,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);
                  }

                  GlobalNotDisplay=0;
                  UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                  TableBoxDraw(GlobalBoxHeadHandle);
                  CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&pos,&Tmp);
                }
                else
                if (GlobalTableCell>=0 && BoxIsTableBox(GlobalBoxHeadHandle))
                {
                  pos = GlobalTextPosition;
                  SaveUndoNum=UndoOperateSum;
                  Tmp=TextSearchAttribute(GlobalBoxHeadHandle,
                           pos,Align,&AttributePosition)&0xf0;
                  TextChangeAttribute(GlobalBoxHeadHandle,pos,
                                    GlobalTextBlockEnd-GlobalTextBlockStart,
                                    Align,Result|Tmp,&GlobalTextPosition,
                                    &GlobalTextBlockStart,&GlobalTextBlockEnd);
                  UndoInsertCompose(UndoOperateSum-SaveUndoNum);
                }
                break;

           case MENU_TABLEADDCOL:
                Result=1;
              lbl_add_col:
                if (GlobalTableCell>=0&&BoxIsTableBox(GlobalBoxHeadHandle))
                {
                   AddColTableCells(GlobalBoxHeadHandle,Result);
                }
                break;
           case MENU_TABLEADDROW:
                Result=1;
              lbl_add_row:
                if (GlobalTableCell>=0&&BoxIsTableBox(GlobalBoxHeadHandle))
                {
                   AddRowTableCells(GlobalBoxHeadHandle,Result);
                }
                break;
           case MENU_TABLEDELCOL:
                Result=-1;
                goto lbl_add_col;
           case MENU_TABLEDELROW:
                Result=-1;
                goto lbl_add_row;

           case MENU_TABLEINSERTDBF:
                goto lbl_disp_all;
           case MENU_TABLEROWCALC:
                Result=ExpressCalc(Window,0);
                goto lbl_disp_all;
           case MENU_TABLECOLCALC:
                Result=ExpressCalc(Window,1);
                goto lbl_disp_all;

           case MENU_TABLEDELETECOLUMN:
                if (GlobalTableCell>=0&&BoxIsTableBox(GlobalBoxHeadHandle))
                {
                   //if (MakeDialogBox(Window,TableDelColDialog))
                     //  break;
                   //Result=FBDelACol(GlobalBoxHeadHandle,TmpFormBox.numLines-1,TRUE);
                   Tmp=TableBoxGetColFromCell(GlobalBoxHeadHandle,GlobalTableCell);
                   Result=FBDelACol(GlobalBoxHeadHandle,Tmp,TRUE);

                lbl_disp_all:
                   if(Result>=0)
                   {
                      GlobalTableBlockStart=GlobalTableBlockEnd=-1;
                      GlobalTextBlockStart=GlobalTextBlockEnd=-1; // New in 1996.3
                      CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                      //TableBoxClear(GlobalBoxHeadHandle);
                      //BoxDrawBorder(GlobalBoxHeadHandle,0);
                      //TableBoxDraw(GlobalBoxHeadHandle);
                      FileSetModified();
                      RedrawUserField();
                   }
                }
                break;
           case MENU_TABLEMERGE:
                if( BoxIsTableBox(GlobalBoxHeadHandle)
                && GlobalTableBlockStart!=GlobalTableBlockEnd)
                {
                   Result=FBMergeBlock(GlobalBoxHeadHandle,GlobalTableBlockStart,GlobalTableBlockEnd);
                   if( Result>=0)
                   {
                      GlobalTextPosition=TableCellGetTextHead(GlobalBoxHeadHandle,GlobalTableCell);
                      CursorLocate(GlobalBoxHeadHandle,&NewHBox,
                                        GlobalTextPosition,&pos,&Tmp);

                   lbl_disp_table:
                      GlobalTextBlockStart=GlobalTextBlockEnd=-1; // New in 1996.3
                      GlobalTableBlockStart=GlobalTableBlockEnd=-1;
                      //CancelBlock(GlobalBoxHeadHandle,&GlobalTextBlockStart,&GlobalTextBlockEnd);
                      TableBoxClear(GlobalBoxHeadHandle);
                      TableBoxDraw(GlobalBoxHeadHandle);
                      //BoxDrawBorder(GlobalBoxHeadHandle,0);
                      FileSetModified();
                   }
                }
                break;
           case MENU_TABLEDISMERGE:
                if (BoxIsTableBox(GlobalBoxHeadHandle)
                && GlobalTableCell>=0
                && GlobalTableBlockStart==GlobalTableBlockEnd)
                {
                   if (FBDdisMergeBlock(GlobalBoxHeadHandle,GlobalTableCell)>=0)
                      goto lbl_disp_table;
                }
                break;
           case MENU_TABLELINETYPE:     // Line or Col's Line_Style
                if (GlobalTableCell>=0&&BoxIsTableBox(GlobalBoxHeadHandle))
                {
                   if (MakeDialogBox(Window,TableLineStyleDialog))
                      break;
                   /*-------------
                    CursorPgUp(GlobalBoxHeadHandle,&NewHBox,
                                GlobalTextPosition,&GlobalTextPosition,
                               &GlobalTextBlockStart,&GlobalTextBlockEnd);
                   --------------------*/
                   if (GetTableLineColOption()==0) // Line
                     Result=FBChangeHLineType(GlobalBoxHeadHandle,TmpFormBox.numLines,GetTableStyleOption());
                   else             // Cols
                     Result=FBChangeVLineType(GlobalBoxHeadHandle,TmpFormBox.numLines,GetTableStyleOption());

                   if(Result>=0)
                      goto lbl_disp_table;
                }
                break;
           case MENU_TABLESLANTTYPE:    // Cell's Slant_line_style
                if (GlobalTableCell>=0&&BoxIsTableBox(GlobalBoxHeadHandle)
                && GlobalTableBlockStart==GlobalTableBlockEnd)
                {
                   if(GetTableStyleOption()>2)
                      SetTableStyleOption(0);

                   if (MakeDialogBox(Window,TableSLantStyleDialog))
                      break;

                   if (FBSlipCell(GlobalBoxHeadHandle,GlobalTableCell,GetTableStyleOption())>=0)
                      goto lbl_disp_table;
                }
                break;
           case MENU_TABLECALCULATE:
                if( BoxIsTableBox(GlobalBoxHeadHandle)
                && GlobalTableBlockStart!=GlobalTableBlockEnd)
                {
                   TableBoxCalculate(GlobalBoxHeadHandle,
                          GlobalTableBlockStart,GlobalTableBlockEnd);
                }
                break;

           case MENU_MODEMSETUP:
                if(!MakeDialogBox(Window,FaxSetupDialog))
                   SaveFaxConfig();
                break;
           case MENU_EXPORTFAX:
                if(TotalPage<1)
                   break;          // no page

                if(MakeDialogBox(Window,ExportFaxDialog))
                    break;

                SetProfileString( ProfileName,FaxSection, DialNumEntry, DialNumber );
                   //-- return: EndPrintPage=total print page
                   //-- now, just it to real end print page
                EndPrintPage +=StartPrintPage-1;
                if(EndPrintPage>TotalPage)
                    EndPrintPage = TotalPage;

                Result=GlobalReverse;    // save old

                memset(&PG,0,sizeof(PG));
                tmp_fileflag=1;   // print to file
                GlobalReverse=0;
                PXScale=1.0; PYScale=(float)98.0/204;
                GlobalSubPage=0;
                PrintCopyN=1;

                MouseSetGraph(BUSYMOUSE);

                if( (!FileHasBeenModified())
                && faxStartPage==StartPrintPage
                && faxEndPage==EndPrintPage)
                   SendFax();
                else
                {
                   faxStartPage=StartPrintPage;
                   faxEndPage=EndPrintPage;
                   if( SetPrinter(DEV_FAX) >= OpOK )
                   {
                      PrintToDevice(StartPrintPage-1,EndPrintPage-1);
                      SendFax();
                   }
                }

                GlobalSubPage=1;
                PXScale=PYScale=1.0;
                MouseSetGraph(ARRAWMOUSE);
                GlobalReverse=Result;   // restore
                break;
           case MENU_SENDFILE:
                if(GetFileName(Window,"*.ARJ;*.EZP;*.*",TmpString,0,SENDFILE_TITLE))
                   break;

                if(!fTelManualDial)
                {
                   if(MakeDialogBox(Window,TransmitFileDialog))
                       break;

                    SetProfileString( ProfileName,FaxSection, DialNumEntry, DialNumber );
                }

                goto lbl_recv_send_file;
           case MENU_RECVFILE:
                fRecv=1;
              lbl_recv_send_file:
                SendRecvFile(TmpString);
                fRecv=0;
                break;
           default:
                break;
         }
         break;
  }
  return(TRUE);
}
