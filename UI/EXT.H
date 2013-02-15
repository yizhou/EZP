/*-------------------------------------------------------------------
* Name: ext.h
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/

extern const char FILEHEADSTRING[];
extern PPB SubP;
extern int GlobalPageSelect;
extern Dialogs SubPageDialog[];
extern int SubPageNum;
extern int GlobalXPage;
extern int GlobalYPage;
extern int GlobalSubPage;
extern int GetUserFrame(int Pa,int *w,int *h);
extern void SaveFontPath(void);
extern Dialogs InstFontDialog[];
extern Dialogs SetFontPathDialog[];
extern int GlobalHollow;
extern int GlobalBlockHide;
extern int WriteExternBlock(FILE *fp);
extern int ReadExternBlock(FILE *fp);
extern GlobalPageRotate;
// extern int zjh_HandleMax;
extern float PaperW,PaperH;
//extern int UsePrintCut;
extern int CurrentPrinterBlock;
extern int PrinterBlockTop[],PrinterBlockBottom[];
extern PP PG;
extern int PageWI,PageHI;
//extern void InitPrintCut(void);
extern float YScale;
extern float XScale;
extern float SYScale;
extern float SXScale;
extern float PYScale;
extern float PXScale;
int myUserYToWindowY(ORDINATETYPE Y);
int myUserXToWindowX(ORDINATETYPE Y);
ORDINATETYPE myWindowYToUserY(int Y);
ORDINATETYPE myWindowXToUserX(int Y);

extern EXT_FORMAT GlobalExtFormat;
extern int get_attr(USHORT yy);
extern USHORT make_attr(int yy,int xx);

extern BOOL fNewA4;
extern int count;
extern int BoxIsModule(char *s,HBOX HBox);
extern WORD IsModule;
extern ScreenModes ScreenMode;
extern const char Symbol24FileName[];
extern const char Dotlib24FileName[];
extern const char SKHF_Name[];
extern UCHAR *ASC16LibBuffer;

extern char TrueTypeLibPath[];
extern char VectLibPath[];


extern char *cfnName;

extern BOOL fEditor,fAutoLoad;
extern BOOL fFirstBlock;
extern char DefaultFileName[];
extern UCHAR PageHeadLeftStr[],PageHeadRightStr[];
extern int  CurrentRow,PgFtStartNum;
extern BOOL tmp_footflag,tmp_headflag;
extern int tmp_foottopflag;
extern int tmp_footleftflag,tmp_headleftflag;
extern int tmp_footprevflag,tmp_headlineflag;
extern int tmp_TableHeadOption;
extern int tmp_TableStyleOption;
extern int tmp_TableLineColOption;
extern int tmp_insflag;
extern int tmp_nline;
extern int GlobalFontSize;

extern int GlobalXOffset;
extern int GlobalYOffset;
extern int GlobalXRes;
extern int GlobalYRes;
extern int GlobalReverse;
extern int GlobalYReverse;
extern int GlobalRorate90;
extern int GlobalPause;
extern int GlobalJob;
extern UDATA PageHightDot;
extern DC myDC;

extern int CellisMoving;

extern HANDLE ActiveWindow,GlobalWindowHandle;
extern HANDLE GlobalMessageHandle,GlobalHMenu;
extern HMSG MessageQueueHead,MessageQueueTail;
extern HMENU ActiveMenu;
extern HANDLE GlobalItemsHandle;
extern HITEM GlobalPageHeadHandle;
extern HITEM GlobalBoxHeadHandle;
extern HPAGE GlobalCurrentPage;
extern HWND ActiveTopMenu;
extern int  TotalPage;
extern Items *DataofItems;
extern MENUS *DataofMenus;
extern Windows *DataofWindows;
extern Messages *DataofMessages;

extern int GlobalTimer;         // in mainc.c
extern unsigned char BmpBuf[];     /* bitmap in normal and pressed status */

extern unsigned short NowEFont;
extern short ASC32AW[MAXEFONT][MAXASC32CODE];
extern HMSG IconMenuIdxArr[];
extern char *IconHintArr[];

extern ListBoxs TestList[MAXLISTBOXS];
extern int CurrentAllocList;
extern SingleLine TestLineBuffer[MAXSINGLELINEBUFFER];
extern int CurrentAllocLine;

extern OpenFileStruct *NowOpenFile;
extern UserTmpBoxs TmpBox;
extern Pages TmpPage;

extern Dialogs TableCalculateDialog[];
extern Dialogs PageFootDialog[];
extern Dialogs PageHeadDialog[];
extern Dialogs PageDialog[];
extern Dialogs TableBoxDialog[];
extern Dialogs PrintCutDialog[];
extern Dialogs TextBoxDialog[];
extern Dialogs TextBoxDialog1[];
extern Dialogs PictureBoxDialog[];
extern Dialogs PictureBoxDialog1[];
extern Dialogs BoxAlignDialog[];
extern Dialogs CharFontDialog[];
extern Dialogs CharSizeDialog[];
extern Dialogs RowGapDialog[];
extern Dialogs ColGapDialog[];
extern Dialogs UpDownDialog[];
extern Dialogs CharHSizeDialog[];
extern Dialogs CharSlantDialog[];
extern Dialogs CharColorDialog[];
extern Dialogs ParagraphAlignDialog[];

extern Dialogs TableInsLineDialog[];
extern Dialogs TableInsColDialog[];
extern Dialogs TableLineColumnDialog[];
//extern Dialogs TableDelLineDialog[];
//extern Dialogs TableDelColDialog[];
extern Dialogs TableSLantStyleDialog[];
extern Dialogs TableLineStyleDialog[];

extern Dialogs FindDialog[];
extern Dialogs ReplaceDialog[];
extern Dialogs GotoLineDialog[];
extern Dialogs GotoPageDialog[];
extern Dialogs MovePageDialog[];
extern Dialogs ExportDialog[];
extern Dialogs PrinterSetupDialog[];
extern Dialogs ExportPictDialog[];
extern Dialogs ExportFaxDialog[];
extern Dialogs PrintDialog[];
extern Dialogs KeyHelpDialog[];
extern Dialogs FaxSetupDialog[];
extern Dialogs TransmitFileDialog[];

extern LoadMenus UserMenu[],EditorMenu[];

extern char *PrinterName[];
extern char *HintPrinterName[];
extern int CurrentPrinter;
extern int PrinterFixedLeftMargin;

extern GroupBoxs GlobalGroup;
extern FindStructs GlobalFindStruct;

//extern BoradcastProcedure *LockBoradcast,*UnlockBoradcast;
//extern BoradcastNoLowMemoryProcedure *OutOfMemoryBoradcast;

extern FILE *prnstr;
extern char PrintName[],logname[];
extern int TimeCountArr[MAX_TIMER_EVENT];
extern char *rasts[];
extern int RastWidth,RastWidthByte,RastHeight,RastSize,MaxRastY;
extern int UseHP1200;
extern short *RowErrBuf;
extern int DitherBufLen,fDither;
extern unsigned char fRemapRGB, fRegist;
extern int ColorR,ColorG,ColorB;
extern int sysColor;
extern int GlobalNotDisplay;               // ByHance, 95,12.8
extern char serial[],SerialTypeLen, regist_str[];
extern unsigned short SerialSum,TypeSum;
extern unsigned short ssum,tsum;
extern int GlobalPageScale;
extern int GlobalPageHStart,GlobalPageVStart;
extern char GlobalToolBarSetting;
extern int  GlobalBoxTool;
extern char GlobalFileStateSign;
extern char DebugFileName[];
extern char ProfileName[];
extern int GlobalTextPosition;
extern int bAtLineFeed;        // ByHance, 95,12.14
extern int GlobalTextBlockStart,GlobalTextBlockEnd;
extern int Ctrl_KB_pos,Ctrl_KB_box;
extern int GlobalTableCell,GlobalTableBlockStart,GlobalTableBlockEnd;
extern unsigned char GlobalTextStatus;

extern PRINTER *printer;
extern char PrintingSign;
extern int PrinterDPI;
extern int StartPrintPage,EndPrintPage,PrintCopyN;

extern int UndoOperateSum;
extern char DoUndoSign;

extern int sysGray;
extern unsigned char DitherTable[];
extern unsigned char headdot[8];
extern unsigned char taildot[8];
extern unsigned char dot1tab[8];
extern unsigned char hdot[8];
extern unsigned char *ByteRGBPalatte;
extern unsigned char RGBPalatteSign;

extern int TextCursor;
extern int TextCursorPosX,TextCursorPosY;
extern int TextCursorRotateAngle;
extern int TextCursorRotateAxisX,TextCursorRotateAxisY;
extern char CursorBlankSign;

extern MAT2 *ImageMatrix;
extern unsigned char *ImageData;
extern unsigned char *ImageClipData;
extern unsigned char *ImageNewData,*ImageBufEnd;
extern HANDLE ImageHandle;

extern int RealWidth,ImageOriginX,ImageOriginY,ImageAxisX,ImageAxisY;
extern char NegativeSign,ContrastSign,ColorSign;
extern unsigned char PosterizedGray[MAXPOSTERIZEDCOLOR];
//extern unsigned char BitMatrixs[8];
//extern unsigned char BitFill[8];
extern int ZoomImageAxisX,ZoomImageAxisY;

extern EdgeFillLine *CurrentEdgeFillLine;
extern LineFillLine *CurrentLineFillLine;
extern HANDLE cfnHandle;
extern HANDLE efnHandle;
extern DC SysDc;

extern TextStyles DEFAULTTYPESTYLE;
extern TextStyles FormattingTextStyle;

extern FILLP fillp;
extern Boxs  TmpBuf;
extern int PolygonNumber;
extern char MoveSign,RotateSign,LinkSign;
extern int  BubleHintIdx;
extern Dialogs EditorHelpDialog[];
extern int fInZoom;
extern int screendpi;
extern char Source_num;
extern char TargetField[],Target_num;
extern int StartRecord,RecordNumber;
extern int StartCellRow,StartCellCol;
extern char DispStr[];
extern DBF_FIELDLIST   FieldList[MAXFIELDLIST+1];
extern char fGetFocusByKey;

extern char InitSection[];
extern char PrinterEntry[];
extern char VectLibPathEntry[];
extern char TrueTypeLibPathEntry[];

extern char DefaultSection[];
extern char PaperSizeSection[];
extern char PageWidthsEntry[];
extern char PageHeightsEntry[];
extern char LeftMarginEntry[];
extern char RightMarginEntry[];
extern char TopMarginEntry[];
extern char BottomMarginEntry[];

extern char ScrModeEntry[];
extern char Scr640[];
extern char Scr800[];
extern char Scr1024[];

extern char FaxSection[];
extern char DialNumEntry[];
extern char ComEntry[];
extern char AutoDialEntry[];
extern char ToneLineEntry[];
extern char LocalIdEntry[];

extern char FilePathSection[];
extern char LastFilePathEntry[];

extern int fTelTone;
extern int fTelManualDial;
extern char  LocalTelId[20];
extern char DialNumber[20];
extern int ComX;

extern int GlobalUseLIB13;
extern HFILENO GlobalMetaFile;
extern int faxStartPage,faxEndPage;
extern int fRecv;
extern BOOL fChgNextCell;
