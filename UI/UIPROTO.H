/* buttonc.c 06/12/96 07.22.20 */
int CreatButton (int Left,
         int Top,
         int Right,
         int Bottom,
         long WindowStyle,
         int ButtonReturn,
         char *ButtonTitle,
         Function *ButtonProcedure,
         HWND FatherWindow);
long ButtonDefaultProc (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
void ToolBarMouseMove (HWND Window,
         HMSG Message,
         ULONG Param1,
         ULONG Param2);
void DialogMouseMove (HWND Window,
         HMSG Message,
         ULONG Param1,
         ULONG Param2);
/* chinesec.c 11/05/97 20.45.30 */
unsigned char *GetChineseDot (Wchar Code);
void DisplayString (unsigned char *String,
         int XPos,
         int YPos,
         int Color,
         int BkColor);
void ViewportDisplayString (unsigned char *String,
         int XPos,
         int YPos,
         int Color,
         int BkColor);
int ChineseLibInitial (void);
int ChineseLibDone (void);
void ViewportDisplaySmallChar (int XPos,
         int YPos,
         int Width,
         int Height,
         int Color);
void ZoomCharBuffer (unsigned char *LibBuffer,
         int dotlib_w,
         int dotlib_h,
         float ZoomW,
         float ZoomH,
         unsigned char *pImage,
         int BytesPerLine);
void ViewportDisplayCharVec (int XPos,
         int YPos,
         Wchar Code,
         int CFont,
         int Width,
         int Height,
         int SlantAngle,
         int RotateAngle,
         int Color);
void ViewportDisplayCharTTF (int XPos,
         int YPos,
         Wchar Code,
         int CFont,
         int Width,
         int Height,
         int SlantAngle,
         int RotateAngle,
         int Color);
void ViewportDisplayChar24 (int XPos,
         int YPos,
         Wchar Code,
         int CFont,
         int Width,
         int Height,
         int SlantAngle,
         int RotateAngle,
         int Color);
void ViewportDisplaySymbol24 (int XPos,
         int YPos,
         Wchar Code,
         int Width,
         int Height,
         int SlantAngle,
         int RotateAngle,
         int Color);
void ViewportDisplayChar16 (int XPos,
         int YPos,
         Wchar Code,
         int CFont,
         int Width,
         int Height,
         int SlantAngle,
         int RotateAngle,
         int Color);
void ViewportDisplayEChar (int XPos,
         int YPos,
         Wchar Code,
         int EFont,
         int Width,
         int Height,
         int SlantAngle,
         int RotateAngle,
         int Color);
/* comboc.c 26/10/96 00.42.38 */
void DrawComboButton (HWND Window);
HWND ComboFindListBox (HWND Window);
int ComboDefaultProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int ComboButtonDefaultProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int CreatCombo (int Left,
         int Top,
         int Right,
         int Bottom,
         Function *ComboProcedure,
         HWND FatherWindow);
/* dbf.c 10/05/97 19.01.56 */
unsigned long format_0_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long prev_0_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long succ_0_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long End_0_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long End_1_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long Start_0_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long Start_1_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long Resu_0_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long Exp_1_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long Exp_2_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long Exp_3_Procedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long StartRecordProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long RecordNumberProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long StartCellRowProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long StartCellColProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int InsertAllField (int num);
int ReadDbf (long rec_num, char *buff);
char *GetField (int num_field, char *record);
int CloseDbf (void);
int BoxInsertDbf (HFormBoxs hFormBox);
int DbfInsert (HWND Window, char *filename);
int ExpressCalc (HWND Window, int mode);
/* dialogc.c 14/05/97 13.30.58 */
int WindowTableOrderNext (HWND Window);
int WindowTableOrderPrev (HWND Window);
long DialogDefaultProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int MakeDialogBox (HWND FatherWindow, Dialogs *Dialog);
HWND MessageBox (char *Title,
         char *Information,
         int TotalButton,
         HWND FatherWindow);
HWND HelpBox (int nTopic, HWND FatherWindow);
/* drawc.c 03/06/97 13.52.34 */
int GraphInitial (void);
int GraphFinish (void);
void Area3DDown (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int Depth);
void Area3DDownColor (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int Depth,
         int Color);
void Hline3DDown (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int x1,
         int x2,
         int y);
void Vline3DDown (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int x,
         int y1,
         int y2);
void DrawButtonPolygon (int PointNumber, short *ButtonPoints);
void DrawMenuButton (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int ButtonStyle);
void DrawLeftScrollButton (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int ButtonStyle);
void DrawRightScrollButton (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int ButtonStyle);
void DrawUpScrollButton (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int ButtonStyle);
void DrawDownScrollButton (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int ButtonStyle);
void DrawScroll (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom);
void DrawScrollButton (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int ButtonState);
void DrawTitleBar (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         char *Title);
void DrawWindow (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int LineSpace);
void DrawIcon (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int ButtonState,
         int Handle);
void DrawUserButton (int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         int Left,
         int Top,
         int Right,
         int Bottom,
         int ButtonState,
         char *ButtonText);
void scan_line (int x1, int x2, int y, int color);
void copymono (char *buf, int x, int y, int w, int h, int color);
char *GetDateString (void);
/* express.c 05/05/97 21.08.14 */
double get_num (char *str);
int get_exp (char *str, double *result);
void set_val (int n, double num);
/* ezplogc.c 06/05/97 14.37.52 */
long RegistProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
/* faxc.c 28/05/97 15.32.02 */
void SetUserCmdStr (char *str);
void GetFaxConfig (void);
void SaveFaxConfig (void);
void GetFaxFilename (int n, char *fn);
void ReadPageToBuf (int n);
void __interrupt __far NewComIntDeal (void);
void FaxHint(char *str);
void TellFaxProcStatus(int i,int TotalChars);
void FaxStatus(char *str, int mode);
int SendFax (void);
int SendRecvFile(char *fn);
/* framec.c 26/10/96 00.42.38 */
void DrawFrame (HWND Window,
         int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom);
int CreatFrameWindow (int Left,
         int Top,
         int Right,
         int Bottom,
         char *Title,
         Function *FrameProcedure,
         HWND FatherWindow);
/* func2.c 03/06/97 13.31.06 */
int CursorAdjusttoWindow (int CursorLeft, int CursorTop);
int ToolExchange (int OldTool, int NewTool);
int MenuScaleChangeMethod (int ToolMethod);
void MenuScaleZoomChange (int ZoomFlag);
void DrawRotateAxis (HWND Window);
void DrawCreatingPolygon (int PolygonNumber, ORDINATETYPE *PolygonEdges);
int PointInLine (int X, int Y, int X1, int Y1, int X2, int Y2);
int BoxChangeBorder (HBOX SelectBox,
         unsigned long LastParam1,
         unsigned long Param2);
void UserFunctionInitial (HWND Window);
void UserFunctionFinish (void);
void RedrawUserField (void);
void DrawToolBar (HWND Window,
         int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom,
         char SelectSign);
long ToolBarProcedure1 (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
long ToolBarProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
HWND SearchBackToolBarWindow (HWND Window);
void ToolBarWindowModify (HWND Window);
void DrawCurrentClibration (HWND Window, int x, int y);
HWND SearchHClibrationWindow (HWND Window);
HWND SearchVClibrationWindow (HWND Window);
void ClibrationWindowModify (HWND Window);
void SetHScrollLeft (HWND Window, int PageWidth);
void SetVScrollTop (HWND Window, int PageHeight);
void MovePageToCenter (HWND Window, HPAGE PageItem);
void DrawLinkedBox (void);
void DrawScreenPageFootHead (void);
void filename_cat (char *filename, char *ext_name);
void ZoomPageAtPoint (HWND Window, ULONG MouseXY, int fZoomOut);
int ZoomPageByRect (HWND Window, int x1, int y1, int x2, int y2);
void FileLoadMeta (char *pname);
int LoadMetaFile (void);
int StopMetaFile (void);
/* idle.c 26/10/96 00.42.38 */
/* informc.c 26/10/96 00.42.38 */
char *GetTitleString (int Number);
char *GetInformString (int Number);
unsigned long KeyHelpProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
/* instfont.c 30/05/97 07.35.26 */
unsigned long StatProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long DispProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long GetFreeSpace (char *pa);
unsigned long FreeDiskSpaceProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TotalDiskSpaceProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long SourcePathProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TargetPathProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG FontLibType (HWND Window, HMSG Message, long Param1, long Param2);
unsigned long VectPathProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TruePathProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
/* keyc.c 12/05/97 14.52.40 */
void GetKey (Wchar *KeyCode, Wchar *KeyShiftStatus);
int KeyIsMenuHotKey (HMENU CompareMenu, Wchar KeyCode);
int MenuHotKeyToMessage (HWND Window, Wchar KeyCode);
void KeyToMessage (void);
/* listboxc.c 11/05/97 11.41.42 */
int ListInsertItem (HWND Window, int Pos, char *String);
int ListDeleteItem (HWND Window, int Pos);
long ListBoxDefaultProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int CreatListBox (int Left,
         int Top,
         int Right,
         int Bottom,
         Function *ListProcedure,
         HWND FatherWindow);
/* mainc.c 03/06/97 13.45.10 */
void SaveFontPath (void);
int SystemDestruct (void);
/* memory.c 17/12/96 18.17.38 */
/* menuc.c 26/10/96 00.42.38 */
int MenuInitial (void);
int MenuEnd (void);
HMENU GetMenuFromReturnValue (int Value);
HMENU MenuGetTopest (HMENU Menu);
int MenuDelete (HMENU DeleteMenu);
int MenuGetTopHeight (HMENU Menu, int *Left, int *Top);
int MenuGetRealLeftTop (HMENU Menu, int *Left, int *Top);
int MenuOpen (HMENU Menu);
int MenuClose (HMENU Menu);
int MenuAllClose (HMENU Menu);
int MenuGetFocus (HMENU hMenu, int X, int Y);
long MenuDefaultProc (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
HMENU CreatSystemMenu (HWND Window);
int CreatMenuWindow (HWND FatherWindow, LoadMenus *MenuData);
/* menucmd1.c 03/06/97 13.38.50 */
long UserMenuCommand (HWND Window, HMSG Message, long Param1);
/* messagec.c 27/05/97 09.43.28 */
int MessageInitial (void);
int MessageFinish (void);
unsigned long MessageGo (HWND Window,
         HMSG Message,
         ULONG Data,
         ULONG ExtraData);
int MessageGet (HWND *Window,
         HMSG *Message,
         ULONG *Data,
         ULONG *ExtraData);
void WaitMessageEmpty (void);
int MessageInsert (HWND Window,
         HMSG Message,
         ULONG Data,
         ULONG ExtraData);
int MessageCreatbyKeyboard (unsigned Key, unsigned Shift);
int MessageCreatbyKeyString (unsigned char *KeyString, unsigned Length);
int MessageCreatbyTimer (HWND WindowNumber, int TimerNumber);
void MouseDownOnTrigger (short MouseLRState, short MouseX, short MouseY);
int MessageCreatbyMouse (unsigned MouseStatus,
         unsigned MouseButton,
         unsigned MouseX,
         unsigned MouseY);
/* mousec.c 12/05/97 10.28.22 */
int lock_region (void *address, unsigned length);
int unlock_region (void *address, unsigned length);
void _loadds far MouseDeal (int max, int mbx, int mcx, int mdx);
void HardQueuetoSoftQueue (void);
void MouseShow (void);
void MouseHidden (void);
void MouseSetGraph (int MouseType);
void MouseMoveTo (int X, int Y);
void MouseGetPosition (int *X, int *Y);
int LockMouseMemory (void);
int UnlockMouseMemory (void);
int MouseConstruct (int maxx, int maxy);
void MouseDestruct (void);
/* openfilc.c 26/05/97 09.09.54 */
void name_ext (char *name, char *new_ext);
int __far handler (unsigned deverr,
         unsigned errval,
         unsigned far *devhdr);
int GetFileName (HWND Window,
         char *WildCardTable,
         char *ReturnFileName,
         char IsNewFileSign,
         int titletype);
/* printcut.c 13/05/97 09.51.00 */
unsigned long DispSubPage (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long SubPageMain (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
/* profilec.c 27/05/97 10.27.16 */
int GetProfileString (char *fname,
         char *section,
         char *entry,
         char *stbuf,
         char *stdefault);
int GetProfileInt (char *fname,
         char *section,
         char *entry,
         int *val,
         int valdefault);
int SetProfileString (char *fname,
         char *section,
         char *entry,
         char *stbuf);
int SetProfileInt (char *fname, char *section, char *entry, int val);
/* radiobnc.c 06/12/96 06.44.42 */
long RadioDefaultProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
HWND CreatRadioButton (int Left,
         int Top,
         int Right,
         int Order,
         int SelectSign,
         char *Title,
         int Group,
         Function *RadioButtonProcedure,
         HWND FatherWindow);
HWND CreatFrameButton (int Left,
         int Top,
         int Right,
         int Order,
         int SelectSign,
         char *Title,
         int Group,
         Function *FrameButtonProcedure,
         HWND FatherWindow);
/* scrollc.c 08/05/97 09.59.36 */
HWND CreatHScroll (int Left,
         int Top,
         int Right,
         Function *HScrollProcedure,
         HWND FatherWindow);
HWND CreatVScroll (int Left,
         int Top,
         int Bottom,
         Function *VScrollProcedure,
         HWND FatherWindow);
void TellStatus (void);
HWND CreatStatusWin (int Left, int Top, int Right, HWND FatherWindow);
void TellFileName (void);
/* selfontc.c 12/05/97 13.17.30 */
int SetCharAttr (HWND Window);
/* singlelc.c 13/05/97 08.57.02 */
void Alarm (void);
int IsInChineseChar (unsigned char *String, int Pos);
void CursorMoveTo (int MovetoX, int MovetoY);
void CursorSetHeight (int Height);
unsigned long SingleLineEditorDefaultProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int CreatSingleLineEditor (int Left,
         int Top,
         int Right,
         int Bottom,
         Function *SingleLineEditorProcedure,
         HWND FatherWindow);
void SingleLineEditorInitial (void);
void SingleLineEditorEnd (void);
/* statictc.c 26/10/96 00.42.38 */
HWND CreatStaticText (int Left,
         int Top,
         int Right,
         char *Title,
         Function *StaticTextProcedure,
         HWND FatherWindow);
/* timer.c 27/05/97 09.43.24 */
int GetIntSign (void);
void SetIntSign (void);
void ClearIntSign (void);
void __interrupt __far TimerDeal (void);
void SetMouseDownOnTime (long time, unsigned short Status);
int TimerInsert (int TimerCount, HWND WindowNumber);
int TimerDelete (int TimerIdx);
void myDelay (int delayN);
int TimerInit (void);
int TimerEnd (void);
/* userattr.c 05/05/97 22.00.36 */
unsigned long PrintLeftRightProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrintTopBottomProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TextDistantTopProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TextDistantBottomProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TextDistantLeftProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TextDistantRightProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int CompressCHARSIZE (int yy);
int UncompressCHARSIZE (int yy);
USHORT make_attr (int II, int AA);
int get_attr (USHORT GG);
unsigned long CFontOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long EFontOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long CharCFontProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long CharEFontProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long CharSizeOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long CharSizeProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long RowGapProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long UpDownProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long CharHSizeProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TableLineProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TableLineStyleProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TableColumnProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long FindOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long FindStringProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long ReplaceStringProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int GetExportOption (void);
unsigned long ExportOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TableInsOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TableInsProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TableStyleOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TableLineColOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TableHeadOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
/* userboxc.c 26/10/96 00.42.38 */
int IsInvalidDigit (char *String);
float ConvertToSystemMeter (float X);
float ConvertToUserMeter (float X);
unsigned long BoxRotateAngleProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxRotateAxisXProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxRotateAxisYProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxBoxLeftProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxBoxTopProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxWidthProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxHeightProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxBoxColumnProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxColumnDistantProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PictureBoxPictureOringleXProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PictureBoxPictureOringleYProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PictureBoxPictureImageScaleXProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PictureBoxPictureImageScaleYProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PictureBoxPictureRotateAngleProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PictureBoxPictureSkewAngleProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxPrintableProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long BoxEditablePro (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
/* userfaxc.c 27/05/97 10.33.00 */
unsigned long TelPlusProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long TelAutoDialProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG TelComXProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG TelLocalIDProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG TelRemotePhoneNumProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
/* userfunc.c 03/06/97 13.40.22 */
int RectangleIsInRectangle (ORDINATETYPE Left1,
         ORDINATETYPE Top1,
         ORDINATETYPE Right1,
         ORDINATETYPE Bottom1,
         ORDINATETYPE Left2,
         ORDINATETYPE Top2,
         ORDINATETYPE Right2,
         ORDINATETYPE Bottom2);
unsigned long WindowToUserWindow (HWND Window, unsigned long Param);
int WindowGetRect (HWND Window,
         int *Left,
         int *Top,
         int *Right,
         int *Bottom);
long UserProcedure (HWND Window, HMSG Message, long Param1, long Param2);
void UserMouseMove (HWND Window,
         HMSG Message,
         ULONG Param1,
         ULONG Param2);
/* userpage.c 27/05/97 10.34.00 */
int Page_Setup (HWND Window);
void init_paper (void);
void save_paper (void);
unsigned long PageSizeProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageWidthProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageHeightProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageMarginLeftProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageMarginTopProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageMarginRightProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageMarginBottomProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageInitialBoxProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageColumnProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageColumnDistantProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int GetPageNumber (void);
unsigned long GotoPageProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int GetPageMoveOption (void);
unsigned long PageMoveOptionProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageMoveProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int GetLineNumber (void);
unsigned long GotoLineProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
/* userprnc.c 26/05/97 10.01.18 */
unsigned long PrintRangeProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrintStartPageProc (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrintTotalPageProc (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrintCopyNumProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrinterList (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long DefaultPrinterProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
int GetPrint2FileOption (void);
unsigned long Print2FileOptionProc (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrintReverse (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PageRotate (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrintRorate90 (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrintPause (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrintJob (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PrintHollow (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PageFootEnableProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PageHeadEnableProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PgFtTDProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PgFtLRProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PgFtPrevProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PgFtStartNumProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PageHeadLeftProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PageHeadRightProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PgHdLRProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
ULONG PgHdLineProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
unsigned long PicDPIProcedure (HWND Window,
         HMSG Message,
         long Param1,
         long Param2);
/* varc.c 03/06/97 13.36.28 */
/* windeftc.c 27/05/97 08.18.00 */
void ClearMainTimer (void);
long WindowDefaultProcedure (HWND Window,
         HMSG Message,
         ULONG Param1,
         ULONG Param2);
/* windowc.c 26/05/97 10.19.18 */
int WindowGetRealRect (HWND WindowNumber,
         int *Left,
         int *Top,
         int *Right,
         int *Bottom);
int WindowGetFocus (int WindowNumber);
int WindowResize (HWND Window,
         int Left,
         int Top,
         int Right,
         int Bottom);
int WindowDraw (HWND WindowNumber,
         int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom);
int WindowGetLastChild (HWND WindowNumber);
int WindowGetNumber (int X,
         int Y,
         HWND WindowNumber,
         int SearchDirection);
int WindowAppend (Windows *AppendWindow, HWND FatherWindow);
int WindowDelete (HWND WindowNumber);
int CreatWindow (int Left,
         int Top,
         int Right,
         int Bottom,
         HWND FatherWindow,
         Function *WindowProcedure,
         unsigned long CreatStyle,
         char *UserTitle,
         LoadMenus UserMenu[]);
int WindowInitial (int MaxX, int MaxY);
int WindowEnd (void);

/* tranfile.c 23/07/97 21.11.00 */
int RecvFile(unsigned char *buffer);
int SendFile(char *FileName,unsigned char *buffer);

/* convert.c 03/08/97 21.11.00 */
int TextBoxInsertTextFile (char *FileName,
         HBOX HBox,
         int *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
