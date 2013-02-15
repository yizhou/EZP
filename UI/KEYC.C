/*-------------------------------------------------------------------
* Name: keyc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

void GetKey(Wchar *KeyCode,Wchar *KeyShiftStatus)
{
  unsigned short LowCode,HighCode;
  Wchar ext_key;

  (*KeyCode)=_bios_keybrd(0x10);
  //(*KeyShiftStatus)=_bios_keybrd(0x12);
  (*KeyShiftStatus)=_bios_keybrd(0x2);         //By zjh 9.2.96
  ext_key=_bios_keybrd(0x12);         //By zjh 9.2.96

#ifdef NOT_USED
  switch (*KeyCode)
  {
    case 2:
    case 4:
    case 6:
    case 8:
            *KeyCode+=0x500;
            return ;
  }
#endif

  LowCode=(*KeyCode)&0x00ff;
  if(LowCode==0x7f)
  {
     *KeyCode=0;
     return;
  }

  HighCode=((*KeyCode)&0xff00)>>8;
  if ((LowCode==0xe0)&&HighCode)
     LowCode=0;
  (*KeyCode)=(LowCode==0)?HighCode+0x100:LowCode;

  switch ((*KeyCode))                     /* Deal with Shift_... */
  {
#ifndef REGIST_VERSION
    case F11:
         TestHandle();
         break;
#endif
    case '.':                          /* Shift_Del */
    case DEL:
         if ((*KeyShiftStatus)&0x03)
            (*KeyCode)=SHIFT_DEL;
         break;

    case '0':
    case INS:
         if ((*KeyShiftStatus)&0x03)   /* Shift_Ins */
            (*KeyCode)=SHIFT_INS;
         break;

    case '1':
    case END:
         if ((*KeyShiftStatus)&0x03)   /* Shift_End */
            (*KeyCode)=SHIFT_END;
         break;

    case '2':
    case DOWN:
         if ((*KeyShiftStatus)&0x03)
            (*KeyCode)=SHIFT_DOWN;
         break;

    case '3':
    case PGDN:
         if ((*KeyShiftStatus)&0x03)
            (*KeyCode)=SHIFT_PGDN;
         break;

    case '4':
    case LEFT:
         if ((*KeyShiftStatus)&0x03)
            (*KeyCode)=SHIFT_LEFT;
         break;

    case '6':
    case RIGHT:
         if ((*KeyShiftStatus)&0x03)
            (*KeyCode)=SHIFT_RIGHT;
         break;

   case '7':
    case HOME:
         if ((*KeyShiftStatus)&0x03)
            (*KeyCode)=SHIFT_HOME;
         break;

    case '8':
    case UP:
         if ((*KeyShiftStatus)&0x03)
            (*KeyCode)=SHIFT_UP;
         break;

    case '9':
    case PGUP:
         if ((*KeyShiftStatus)&0x03)
            (*KeyCode)=SHIFT_PGUP;
         break;

    case CTRL_HOME:                    /* Shift_Ctrl_... */
    case CTRL_PGUP:
    case CTRL_LEFT:
    case CTRL_RIGHT:
    case CTRL_END:
    case CTRL_PGDN:
         if ((*KeyShiftStatus)&0x03)
                 (*KeyCode)+=0x0200;
         break;

    case TAB:                          /* CTRL_TAB */
         if ((*KeyShiftStatus)&0x04)
            (*KeyCode)=CTRL_TAB;
         break;
    //case BACKSPACE:                    /* ALT_BACKSPACE */
    //   if ((*KeyShiftStatus)&0x08)
    //      (*KeyCode)+=0x0800;
    //   break;
    case 32:                           /* ALT_BLANK */
         if ((*KeyShiftStatus)&0x08)
            (*KeyCode)=ALT_BLANK;
         else
         if (ext_key&0x04)
            (*KeyCode)=RIGHT_CTRL_BLANK;
         break;
  }
  //return((*KeyCode));
}

int KeyIsMenuHotKey(HMENU CompareMenu,Wchar KeyCode)
{
  HMENU MidMenu;

  MidMenu=CompareMenu;
  while (MidMenu)
  {
    if (KeyCode==MenuGetShortKey(MidMenu))
    {
       if (ActiveMenu>0)
             MenuAllClose(ActiveMenu);

       if (MenuGetFather(MenuGetFather(MidMenu))==0
       &&MenuGetPrev(MenuGetFather(MidMenu))==0)
       {
          WaitMessageEmpty();           // ByHance
          MessageInsert(MenuGetReturnValue(MenuGetTopest(MidMenu)),
                        MenuGetReturnValue(MidMenu),0,0);
       } else {
          WaitMessageEmpty();           // ByHance
          MessageInsert(MenuGetReturnValue(MenuGetTopest(MidMenu)),
                        MENUCOMMAND,MenuGetReturnValue(MidMenu),0);
       }
       return(1);
    }
    if (MenuGetChild(MidMenu))
    {
       if (KeyIsMenuHotKey(MenuGetChild(MidMenu),KeyCode))
          return(1);
    }
    MidMenu=MenuGetNext(MidMenu);
  }
  return(0);
}

int MenuHotKeyToMessage(HWND Window,Wchar KeyCode)
{
  HMENU MidMenu,MidTopMenu;

 // while (Window && (!WindowHasSystemMenu(Window)) && Window!=1)
  while (Window&&(!WindowHasSystemMenu(Window)))
     Window=WindowGetFather(Window);
  if (!Window)
     return(0);

  MidMenu=WindowGetUserData(Window);
  if (!MidMenu)
     return(0);

  if(Window==1 && ActiveMenu<=0 && ActiveTopMenu<=0)
  {
      switch(KeyCode) {
         case ESC:
             MouseShow();
             KeyCode=UserMenu[1].MenuShortKey; //ALT_F:Use Esc to open main_menu
             break;
         case CTRL_C:
             KeyCode=CTRL_INS;          // copy block
             break;
         case CTRL_Z:
             KeyCode=SHIFT_INS;         // paste block
             break;
         case F2:
             KeyCode=CTRL_S;         // paste block
             break;
         case F6:                       // font size
             if(!fEditor)
             {
                WaitMessageEmpty();           // ByHance
                MessageInsert(Window,MENUCOMMAND,MENU_FONTSIZE,0L);
                return(0);
             }
             break;
         case F8:                       // color
             if(!fEditor)
             {
                WaitMessageEmpty();           // ByHance
                MenuHotKeyToMessage(Window,ALT_Y);        // ByHance
                MenuDefaultProc(Window,KEYDOWN,'C',0);   // in UserMenu.C
                return(0);
             }
             break;
     #ifdef UNUSED
         case F9: {
               GraphInitial();
             }
             break;
     #endif
      }  // switch
  }

  MidTopMenu=MidMenu;
  while (MidTopMenu)
  {
    if (KeyCode==MenuGetShortKey(MidTopMenu))
    {
       if (ActiveMenu>0)
       {
          if (ActiveMenu!=MidTopMenu)
             MenuAllClose(ActiveMenu);
          else
             return(1);
       }
       WaitMessageEmpty();           // ByHance
       MenuOpen(MidTopMenu);
       return(1);
    }
    if (KeyIsMenuHotKey(MenuGetChild(MidTopMenu),KeyCode))
       return(1);
    MidTopMenu=MenuGetNext(MidTopMenu);
  }
  return(0);
}

void KeyToMessage(void)
{
  static unsigned char KeyString[32];
  char KeyStringLength;
  Wchar KeyCode,KeyShiftStatus;

  HardQueuetoSoftQueue();
  KeyStringLength=0;
  KeyCode=0;
  while (_bios_keybrd(0x11))
  {
     GetKey(&KeyCode,&KeyShiftStatus);
     if (KeyCode>=BLANK && KeyCode<255 && KeyStringLength<31)
        KeyString[KeyStringLength++]=KeyCode;
     else
        break;
  }

  if(KeyCode) {
     ClearMainTimer();
     MessageGo(1,DELBUBLE,0L,0L);  // delete old hint window
   #ifdef REGIST_VERSION
     if(!TimeCountArr[2])     // if have not got log file
        UserMouseMove(1,DELBUBLE,0L,0L);        // get logic file
     else
     if(fRegist)
     {
        int n=KeyStringLength*3+TimeCountArr[1];
        n=n%SerialTypeLen;
        n=0x7&(serial[n]-regist_str[n]);
        if(n!=0 && n<=KeyStringLength+3
        && ActiveMenu<0
        && KeyCode!=BLANK && KeyCode!=ENTER && KeyCode!=ESC && KeyCode!=ALT_X
        && KeyCode!=TAB )
        {
          KeyStringLength-=8;
          MessageInsert(ActiveWindow,REGISTERROR,KeyStringLength,KeyCode);
          if( KeyCode>255 || (KeyCode>0&&KeyCode<BLANK) )
             KeyCode=BLANK;
        }
     }
   #endif
  }

  if (KeyStringLength>0)
  {
     if (KeyStringLength==1)
        MessageCreatbyKeyboard(KeyCode,KeyShiftStatus);
     else
     {
        KeyString[KeyStringLength]=0;
        MessageCreatbyKeyString(KeyString,KeyStringLength);
     }
  }

  if( KeyCode>255 || (KeyCode>0&&KeyCode<BLANK) )
  {
     if (!MenuHotKeyToMessage(ActiveWindow,KeyCode))
        MessageCreatbyKeyboard(KeyCode,KeyShiftStatus);
  }
}
