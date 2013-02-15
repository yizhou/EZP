/*-------------------------------------------------------------------
* Name: informc.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"

#ifdef __ENGLISHMODE__
  static char TitleInformation[][10]={"Error!","Warning!","Hello!"};
  static char InformInformation[][30]=
  {
     "Invaild rotate angle!",
     "Invaild axis x ordinate!",
     "Invaild axis y ordinate!",
     "Invaild box left!",
     "Invaild box top!",
     "Invaild box width!",
     "Invaild box height!",
     "Invaild text left distant!",
     "Invaild text top distant!",
     "Invaild text width distant!",
     "Invaild text height distant!",
     "Invaild round left distant!",
     "Invaild round top distant!",
     "Invaild round width distant!",
     "Invaild round height distant!",
     "Invaild column!",
     "Invaild column distant!",
     "Invaild page width!",
     "Invaild page height!",
     "Invaild page margin left!",
     "Invaild page margin top!",
     "Invaild page margin right!",
     "Invaild page margin bottom!",
     "File has been modifed, save it?",
     "File load error!",
     "File save error!",
     "Invaild char font!",
     "Invaild char size!",
     "Invaild char hor size!",
     "Invaild char slant angle!",
     "Invaild char color!",
     "Invalid file name!",
     "Too many regions!"
  };
#else
  static char TitleInformation[][8]={"错误！","警告！"};
  static char InformInformation[][40]=
  {
     "旋转角不合理,请修正",
     "旋转轴X坐标不合理,请修正",
     "旋转轴Y坐标不合理,请修正",
     "版框左坐标不合理,请修正",
     "版框上坐标不合理,请修正",
     "版框宽度不合理,请修正",
     "版框高度不合理,请修正",
     "文本左边空不合理,请修正",
     "文本上边空不合理,请修正",
     "文本右边空不合理,请修正",
     "文本下边空不合理,请修正",
     "绕排左边空不合理,请修正",
     "绕排上边空不合理,请修正",
     "绕排右边空不合理,请修正",
     "绕排下边空不合理,请修正",
     "分栏数在1-10之间",
     "栏间距不合理,请修正",
     "页宽度不合理,请修正",
     "页高度不合理,请修正",
     "左边空不合理,请修正",
     "上边空不合理,请修正",
     "右边空不合理,请修正",
     "下边空不合理,请修正",
     "文件已被改动,保存吗?",
     "取文件不成功!",
     "存文件不成功",
     "字体序号不合理,请修正",
     "字符大小不合理,请修正",
     "字符水平大小不合理,请修正",
     "字符倾斜角度不合理,请修正",
     "字符颜色不合理,请修正",
     "找不到此文件,请重新输入!",
     "绕排区域太多,排版困难",
     "版面尺寸与边空不一致,请修正",
  };
#endif

char *GetTitleString(int Number)
{
  if (Number<2)
     return(TitleInformation[Number]);
  return(NULL);
}

char *GetInformString(int Number)
{
  if (Number<MAXINFORM)
     return(InformInformation[Number]);
  return(NULL);
}

unsigned long KeyHelpProcedure(HWND Window,HMSG Message,long Param1,long Param2)
{
  if(Message>10000)
  {
     MessageInsert(Window,DIALOGBOXEND,Message,0l);
     return(TRUE);
  }

  return(DialogDefaultProcedure(Window, Message, Param1, Param2));
}
