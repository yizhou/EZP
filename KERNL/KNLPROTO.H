/* bldchar.c 03/06/97 14.23.46 */
void BuildImage (char *pImage,
         int XPos,
         int YPos,
         int Width,
         int Height,
         int Slant,
         int RotateAngle,
         int Color);
void BuildChar16 (int XPos,
         int YPos,
         Wchar Code,
         int CFont,
         int Width,
         int Height,
         int Slant,
         int RotateAngle,
         int Color);
void InitDot13LIB (void);
void BuildChar13 (int XPos, int YPos, Wchar Code, int Color);
void BuildCharVec (int XPos,
         int YPos,
         Wchar Code,
         int CFont,
         int Width,
         int Height,
         int Slant,
         int RotateAngle,
         int Color);
void BuildCharTTF (int XPos,
         int YPos,
         Wchar Code,
         int CFont,
         int Width,
         int Height,
         int Slant,
         int RotateAngle,
         int Color);
void BuildChar24 (int XPos,
         int YPos,
         Wchar Code,
         int CFont,
         int Width,
         int Height,
         int Slant,
         int RotateAngle,
         int Color);
void BuildSymbol24 (int XPos,
         int YPos,
         Wchar Code,
         int Width,
         int Height,
         int Slant,
         int RotateAngle,
         int Color);
/* boxc.c 01/06/97 18.32.06 */
int PolygonGetMinRectangle (int PolygonNumber,
         ORDINATETYPE *PolygonEdges,
         ORDINATETYPE *MinX,
         ORDINATETYPE *MinY,
         ORDINATETYPE *MaxX,
         ORDINATETYPE *MaxY);
void BoxPolygonRotate (int BoxDots,
         ORDINATETYPE *BoxXY,
         PictureBoxs *Box);
void BoxPolygonToWindowXY (int BoxDots, ORDINATETYPE *BoxXY);
void BoxGetPolygonBorder (Boxs *Box,
         int *BoxDots,
         ORDINATETYPE *BoxXY);
void BoxGetPolygonDrawBorder (Boxs *Box,
         int *BoxDots,
         ORDINATETYPE *BoxXY);
HBOX TextBoxInsert (TextBoxs *InsertBox, HITEM PageItem);
HBOX TableBoxInsert (FormBoxs *InsertBox, HITEM PageItem);
HBOX LineBoxInsert (LineBoxs *InsertBox, HITEM PageItem);
HBOX PictureBoxInsert (PictureBoxs *InsertBox, HITEM PageItem);
HBOX BoxInsert (Boxs *InsertBox, HITEM PageItem);
void BoxDelete (HITEM BoxItem);
void BoxMove (HITEM BoxItem, ORDINATETYPE MoveX, ORDINATETYPE MoveY);
void BoxSetBackward (HBOX HBox);
void BoxSetForward (HBOX HBox);
void BoxSetBackground (HBOX HBox);
void BoxSetFront (HBOX HBox);
void BoxGetRect (HBOX HBox,
         ORDINATETYPE *BoxLeft,
         ORDINATETYPE *BoxTop,
         ORDINATETYPE *BoxRight,
         ORDINATETYPE *BoxBottom);
void XorPutCell (HBOX BoxItem, int DrawCell);
void TableBoxClear (HBOX BoxItem);
void TableBoxDraw (HBOX BoxItem);
void TextBoxDrawTail (HBOX BoxItem, int Hidden);
void BoxDrawBorder (HITEM BoxItem, char AttributeSign);
void BoxDraw (HITEM BoxItem, char AttributeSign);
void DefaultNewBox (Pages *NewPage, TextBoxs *NewBox);
HBOX FindXYInBox (HPAGE Page, int MouseX, int MouseY);
void SetTextBoxTextCursor (HBOX HBox);
void SetNewCursor (void);
void BoxEnter (HBOX HBox);
void BoxLeave (HBOX HBox);
int BoxSelect (HPAGE Page, int MouseX, int MouseY);
void BoxChange (HBOX HBox, HPAGE HPage);
void BoxChangeFrom (HBOX HBox);
void BoxChangeAll (HPAGE HPage);
void BoxBar (TextBoxs *TextBox,
         int Left,
         int Top,
         int Right,
         int Bottom);
void FindXYInCell (HBOX HTableBox,
         ORDINATETYPE BoxX,
         ORDINATETYPE BoxY,
         int *FindCell);
void BoxSelectOrMove (HBOX HBox,
         unsigned long Mouse1,
         unsigned long Mouse2,
         unsigned long MidMouse,
         int *BlockStart,
         int *BlockEnd);
int BoxIsPictureBox (HBOX HBox);
int BoxIsTextBox (HBOX HBox);
int BoxIsTableBox (HBOX HBox);
int BoxCanEditable (HBOX HBox);
int BoxIsLineBox (HBOX HBox);
void SetAllLinkBoxTextHandle (HTEXTBOX HTextBox);
void SetAllUnLinkBoxTextHandle (HTEXTBOX HTextBox);
int GetFirstLinkBox (HTEXTBOX HTextBox);
int TextBoxInWindow (HTEXTBOX hBox);
int TextBoxRectInWindow (HTEXTBOX hBox,
         int Left,
         int Top,
         int Right,
         int Bottom);
int TextBoxLinkCycle (HTEXTBOX HPrevLinkBox, HTEXTBOX HNextLinkBox);
void DrawBoxInBox (HBOX insertBox, HBOX HBox, int x, int y);
/* clpboard.c 26/10/96 00.43.08 */
void ClipBoardInsert (int DataLength);
int ClipBoardAppend (void *AppendData, int DataLength, char DataType);
void ClipBoardRead (void);
int ClipBoardInsertText (HTEXTBOX HBox,
         int BlockStart,
         int BlockEnd,
         char InsertSign);
int ClipBoardInsertImage (HPICTUREBOX HBox, char InsertSign);
int ClipBoardInsertBox (HBOX HBox, char InsertSign);
int ClipBoardInsertGroup (HANDLE *Group, int GroupNumber);
void ClipBoardGetDataInfomation (ClipBoards *ClipBoardDataInformation);
int ClipBoardGetText (Wchar *Text);
int ClipBoardGetImage (InsertImages *InsertImage);
int ClipBoardGetBox (int PasteSign);
/* devbw.c 26/05/97 10.04.06 */
void BW_setGray (int gray);
void BW_scanline (int x1, int x2, int y1, LPDC lpdc);
/* devcolor.c 08/12/96 05.45.20 */
void ChangeColorForRBYKDotMatrixPrinter();
void RestoreColorForRBYKDotMatrixPrinter();
void setSYScolor (int sColor);
void setGray (int gray);
void setRGBcolor (int r, int g, int b);
void setCMYKcolor (int c, int m, int y, int k);
int InitDitherBuf (void);
void CloseDitherBuf (void);
void DitherRGB (LPDC lpdc);
int RLEcompress (DWORD *row, DWORD *end_row, char *compressed);
void RGB_scanline (int x1, int x2, int y1, LPDC lpdc);
/* ezl.c 03/06/97 14.21.02 */
int mm2mil (double n);
int getPointN (char *str, POINT point[], int *pN);
int getrealn (char *str, double val[], int n);
int ParseLINE (char *str);
int ParseTRIANGLE (char *str);
int ParseFILLTRIANGLE (char *str);
int ParseRECTANGLE (char *str);
int ParseFILLRECTANGLE (char *str);
int ParseSQUARE (char *str);
int ParseFILLSQUARE (char *str);
int ParseROUNDRECTANGLE (char *str);
int ParseFILLROUNDRECTANGLE (char *str);
int ParseCIRCLERECTANGLE (char *str);
int ParseFILLCIRCLERECTANGLE (char *str);
int ParseCIRCLE (char *str);
int ParseFILLCIRCLE (char *str);
int ParseELLIPSE (char *str);
int ParseFILLELLIPSE (char *str);
int ParseCURVE (char *str);
int ParsePOLYGON (char *str);
int ParseFILLPOLYGON (char *str);
int ParseARC (char *str);
int ParseLINEWIDTH (char *str);
int ParseLINETYPE (char *str);
int ParseLINEHEAD (char *str);
int ParseLINEEND (char *str);
int ParseCOLOR (char *str);
int ParseCOLORRGB (char *str);
int ParseCOLORCMYK (char *str);
int ParseSETEEFONT (char *str);
int ParseSETCCFONT (char *str);
int ParseFONTSIZE (char *str);
int ParseFONTHEIGHT (char *str);
int ParseFONTWIDTH (char *str);
int ParseCHARROTATE (char *str);
int ParseCHARSLANT (char *str);
int ParseCHARCOLOR (char *str);
int ParseCHARHOLLOW (char *str);
int ParseCHAR3D (char *str);
int ParseSHOWCHAR (char *str);
int CheckCommand (char cmd[]);
int ParseString (char *str);
void CloseMetaFile (HFILENO hMetaFile);
void CloseAllMetaFile (void);
HFILENO InitMetaFile (char *filename);
void MetaFileProc (HFILENO hMetaFile);
void AllMetaFileProc (void);
/* filec.c 03/06/97 13.33.36 */
void SetDefaultExternBlock (void);
int WriteExternBlock (FILE *fp);
int ReadExternBlock (FILE *fp);
int FileSave (char *FileName);
int FileLoad (char *FileName, HPAGE *FirstPage, HBOX *FirstBox);
int FileExportText (char *FileName);
int FileExportStory (char *FileName, HTEXTBOX HBox);
/* fixedc.c 26/10/96 00.43.08 */
long LSin (int a);
void FixedAdd (LPFIXED lpf1, LPFIXED lpf2, LPFIXED lpf);
void FixedSub (LPFIXED lpf1, LPFIXED lpf2, LPFIXED lpf);
void FixedMul (LPFIXED lpf1, LPFIXED lpf2, LPFIXED lpf);
void MAT2Mul (LPMAT2 lpmat1, LPMAT2 lpmat2, LPMAT2 lpmat3);
void GetSkewMatrix2 (MAT2 *Matrix, int SkewAngle);
void GetRotateMatrix2 (MAT2 *Matrix, int RotateAngle);
void GetZoomMatrix2 (MAT2 *Matrix, float ZoomX, float ZoomY);
void Matrix2ConvertPoint (MAT2 *Matrix, int *X, int *Y);
/* form.c 02/06/97 17.55.08 */
int FBInitCells (HFormBoxs hFormBox, int numLines, int numCols);
int FBInsACol (HFormBoxs hFormBox,
         int iNewCol,
         int CellWidth,
         BOOL bText);
int FBDelACol (HFormBoxs hFormBox, int iDelCol, BOOL bText);
int FBInsALine (HFormBoxs hFormBox,
         int iNewLine,
         int CellHeight,
         BOOL bText);
int FBDelALine (HFormBoxs hFormBox, int iDelLine, BOOL bText);
int FBMergeCellTexts (HFormBoxs hFormBox,
         int iMgLine,
         int iMgCol,
         int numMgLines,
         int numMgCols,
         int maxCols);
int FBMergeCells (HFormBoxs hFormBox,
         int iMgLine,
         int iMgCol,
         int numMgLines,
         int numMgCols);
int FBDdisMergeBlock (HFormBoxs hFormBox, int iCell);
int FBDisMergeCells (HFormBoxs hFormBox,
         int iMgLine,
         int iMgCol,
         int numMgLines,
         int numMgCols,
         HANDLE hUndoCellTable);
int TableColOfX (HFormBoxs hFormBox, int x);
int TableLineOfY (HFormBoxs hFormBox, int y);
int FBXYToNewXY (HFormBoxs hFormBox,
         int x,
         int y,
         int *xNew,
         int *yNew);
int FBCellofLineCol (HFormBoxs hFormBox, int iLine, int iCol);
int IsFBCellofLineCol (HFormBoxs hFormBox, int iLine, int iCol);
int FBCellofXY (HFormBoxs hFormBox, int x, int y);
int FBGetCellRect (HFormBoxs hFormBox,
         int iCell,
         int *px0,
         int *py0,
         int *px1,
         int *py1);
int FBPGetCellRect (PFormBoxs pFormBox,
         int iCell,
         int *px0,
         int *py0,
         int *px1,
         int *py1);
int FBDrawBorder (HFormBoxs hFormBox);
int FBFindNextCell (HFormBoxs hFormBox,
         int iCell,
         enum CELLDIRECTION direction);
int FBPlusHoriLine (HFormBoxs hFormBox, int iHoriLine, int yPlus);
int FBPlusVertLine (HFormBoxs hFormBox, int iVertLine, int xPlus);
int FBChangeCellLeftLine (HFormBoxs hFormBox, int iCell, int xNew);
int FBChangeCellTopLine (HFormBoxs hFormBox, int iCell, int yNew);
int FBChangeCellRightLine (HFormBoxs hFormBox, int iCell, int xNew);
int FBChangeCellBottomLine (HFormBoxs hFormBox, int iCell, int yNew);
int FBFindBlockCell (HFormBoxs hFormBox,
         int iCellBegin,
         int iCellEnd,
         int iPosInBlock);
int FBMergeBlock (HFormBoxs hFormBox, int iCellBegin, int iCellEnd);
int FBCellToLineCol (HFormBoxs hFormBox,
         int iCell,
         int *piCellLine,
         int *piCellCol);
int AdjustTableBoxRightVertline (PFormBoxs pFormBox);
int AdjustTableBoxBottomHoriline (PFormBoxs pFormBox);
int TableChangeHortLine (HFormBoxs hFormBox, int iHortLine, int yNew);
int TableChangeVertLine (HFormBoxs hFormBox, int iVertLine, int xNew);
void ReFormatTableText (HFormBoxs hFormBox, BOOL bEraseBk);
int FBChangeHLineType (HFormBoxs hFormBox, int iNewLine, int Type);
int FBChangeVLineType (HFormBoxs hFormBox, int iNewCol, int Type);
int FBSlipCell (HFormBoxs hFormBox, int iCell, int type);
int TableBoxGetMinVertline (PFormBoxs pFormBox);
int TableBoxGetMinHortline (PFormBoxs pFormBox);
int TableBoxGetMinWidth (PFormBoxs pFormBox);
int TableBoxGetMinHeight (PFormBoxs pFormBox);
int TableBoxGetMaxWidth (PFormBoxs pFormBox);
int TableBoxGetMaxHeight (PFormBoxs pFormBox);
int TableBoxGetLineFromCell (HFormBoxs hFormBox, int iCell);
int TableBoxGetColFromCell (HFormBoxs hFormBox, int iCell);
void TableBoxCalculate (HFormBoxs hFormBox, int iCellBegin, int iCellEnd);
int IsCellWidthValid (HBOX HBox, int width);
void TableBoxGetTextStr (HFormBoxs hFormBox,
         int iCell,
         char *str,
         int max);
/* groupc.c 26/10/96 00.43.08 */
int Group (HPAGE HPage,
         ORDINATETYPE GroupLeft,
         ORDINATETYPE GroupTop,
         ORDINATETYPE GroupWidth,
         ORDINATETYPE GroupHeight);
int GroupAll (HPAGE HPage);
void GroupDelete (void);
void GroupMove (ORDINATETYPE MoveX, ORDINATETYPE MoveY);
void GroupLock (void);
void GroupUnlock (void);
void GroupRotate (short RotateAngle);
void GroupRotateInit (ORDINATETYPE RotateAxisX, ORDINATETYPE RotateAxisY);
void GroupSetBack (void);
void GroupSetFront (void);
int MousePointInGroup (int MouseX, int MouseY);
void GroupDrawAllBorder (char DrawType);
/* handlec.c 12/05/97 14.52.24 */
HANDLE HandleAlloc (long Size, char AllocType);
void HandleFree (HANDLE Handle);
void *HandleLock (HANDLE Handle);
HANDLE HandleRealloc (HANDLE Handle, long Size);
void HandleInitial (void);
void HandleFinish (void);
void TestHandle (void);
/* itemsc.c 26/10/96 00.43.08 */
int ItemConstruct (void);
int ItemDestruct (HITEM ItemNumber);
HITEM ItemGetLastChild (HITEM ItemNumber);
HITEM ItemInsert (Items *InsertItem, HITEM FatherItem, HITEM PrevItem);
HITEM ItemAppend (Items *AppendItem, HITEM FatherItem);
void ItemDelete (HITEM ItemNumber);
HITEM ItemSearchbyHandle (HANDLE Handle);
HITEM ItemSetFront (HITEM ItemNumber, HITEM PrevItem);
int ItemInitial (void);
void ItemFinish (void);
/* lines.c 18/04/97 22.54.04 */
void LineToPath (LPDC lpdc, POINT point1, POINT point2, UINT LineWidth);
void WithWidthLine (LPDC lpdc,
         int x1,
         int y1,
         int x2,
         int y2,
         UINT LineWidth,
         UINT LineType,
         unsigned int ArrowType,
         UCHAR FillMode);
int makejoint (LPDC lpdc,
         int x1,
         int y1,
         int x2,
         int y2,
         int x3,
         int y3,
         UINT LineWidth);
int makearrow (LPDC lpdc,
         int x1,
         int y1,
         int x2,
         int y2,
         UINT LineWidth,
         UCHAR ArrowType);
int DrawUpTriangle (LPDC lpdc,
         int x1,
         int y1,
         int x2,
         int y2,
         UCHAR FillMode,
         UINT LineWidth,
         UCHAR LineType);
int DrawDownTriangle (LPDC lpdc,
         int x1,
         int y1,
         int x2,
         int y2,
         UCHAR FillMode,
         UINT LineWidth,
         UCHAR LineType);
int DrawRectangle (LPDC lpdc,
         int x1,
         int y1,
         int x2,
         int y2,
         UCHAR FillMode,
         UINT LineWidth,
         UCHAR LineType,
         UCHAR LineArrow);
int WithArcTypeArc (LPDC lpdc,
         POINT MiddlePoint,
         float Xlen,
         float Ylen,
         float StartAngle,
         float FinishAngle,
         UCHAR FillMode,
         UINT ArcWidth,
         UINT ArcType);
/* meta.c 03/06/97 14.25.58 */
int RGBtoEGA (int r, int g, int b);
int CMYKtoEGA (int c, int m, int y, int k);
void MetaSetRGBColor (int r, int g, int b);
void MetaSetCMYKColor (int c, int m, int y, int k);
void MetaColor (int color);
void MetaSetLineWidth (int linewidth);
void MetaSetLineType (int type);
void MetaSetLineHead (int var);
void MetaSetLineEnd (int var);
void MetaLine (int x1, int y1, int x2, int y2);
void MetaMakeLineJoint (int x1,
         int y1,
         int x2,
         int y2,
         int x3,
         int y3);
void MetaTriangle (int x1, int y1, int x2, int y2, int x3, int y3);
void MetaLines (LPPOINT lppoint, int dotn);
void MetaPolygon (LPPOINT lppoint, int dotn);
void MetaRectangle (int x1, int y1, int x2, int y2);
void MetaFillPolygon (LPPOINT lppnt, int dotn);
void MetaFillRectangle (int x1, int y1, int x2, int y2);
void MetaFillTriangle (int x1, int y1, int x2, int y2, int x3, int y3);
void MetaEllipse (int xc, int yc, int rx, int ry);
void MetaCircle (int xc, int yc, int r);
void MetaArc (int xc, int yc, int xs, int ys, double angle);
void MetaRoundRectangle (int x1, int y1, int x2, int y2);
void MetaCircleRectangle (int x1, int y1, int x2, int y2);
void MetaFillEllipse (int xc, int yc, int rx, int ry);
void MetaFillCircle (int xc, int yc, int r);
void MetaFillRoundRectangle (int x1, int y1, int x2, int y2);
void MetaFillCircleRectangle (int x1, int y1, int x2, int y2);
void MetaCurve (int x0,
         int y0,
         int x1,
         int y1,
         int x2,
         int y2,
         int x3,
         int y3);
void MetaSetEEFont (int fnt);
void MetaSetCCFont (int fnt);
void MetaSetCharSize (int size);
void MetaSetCharW (int w);
void MetaSetCharH (int h);
void MetaSetCharSlant (int ang);
void MetaSetCharRotate (int ang);
void MetaSetCharHollow (int v);
void MetaSetChar3D (int v);
void MetaSetCharColor (int color);
void MetaShowChar (unsigned short code, int x, int y);
void MetaShowLaceChar (unsigned short code, int x, int y);
void MetaLace (int x1, int y1, int x2, int y2, int lacen);
void RunMetaFile (void);
/* pagec.c 03/06/97 13.26.28 */
HPAGE PageNumberToHandle (int PageNumber);
int PageHandleToNumber (HPAGE PageHandle);
HPAGE PageGotoNumber (int PageNumber);
HPAGE PageGotoHandle (HPAGE PageHandle);
HPAGE PageNew (Pages *NewPage, int PageNumber);
void PageDelete (int PageNumber);
void PageDeleteAll (void);
int MovePage (int thispagen, int destpagen, int flag);
void PageInitial (void);
void PageFinish (void);
ORDINATETYPE WindowXToUserX (int X);
int UserXToWindowX (ORDINATETYPE X);
ORDINATETYPE WindowYToUserY (int Y);
int UserYToWindowY (ORDINATETYPE Y);
ORDINATETYPE myWindowXToUserX (int X);
int myUserXToWindowX (ORDINATETYPE X);
ORDINATETYPE myWindowYToUserY (int Y);
int myUserYToWindowY (ORDINATETYPE Y);
void PageChangeCurrent (HPAGE NewCurrentPage);
void PageDraw (HPAGE PageHandle,
         HWND Window,
         int DrawLeft,
         int DrawTop,
         int DrawRight,
         int DrawBottom);
int BoxAutoAppdenPage (HBOX HBox);
void ChangePageSetup (Pages *OldPage);
void DrawPageFootHead (HPAGE PageHandle, BOOL fDrawFoot, BOOL fDrawHead);
/* polyfunc.c 07/05/97 21.01.18 */
void ImageSlantRotateFillLine16 (int x1, int x2, int y, LPDC lpdc);
void ImageDrawFillLineBW (int x1, int x2, int y, LPDC lpdc);
void ImageDrawFillLine16 (int x1, int x2, int y, LPDC lpdc);
void ImageDrawFillLine256 (int x1, int x2, int y, LPDC lpdc);
void ImageDrawFillLineTrueColor (int x1, int x2, int y, LPDC lpdc);
void SetImageNoClipLineFill (int x1, int x2, int y, LPDC lpdc);
void SetImageClipLineFill (int x1, int x2, int y, LPDC lpdc);
/* printc.c 03/06/97 13.27.22 */
void SetDeviceColor (unsigned long Color, int CharSign);
int SetPrinter (int type);
int PPage (HPAGE HPage);
int PrintToDevice (int PageStart, int PageEnd);
int NewPPage (HPAGE HPage, int Num);
int NewPPageMain (int PageNum);
/* rotatec.c 24/12/96 03.33.26 */
void RotatePoint (int *RotateX,
         int *RotateY,
         int OldX,
         int OldY,
         int RotateAxisX,
         int RotateAxisY,
         int RotateAngle);
void Rotate (ORDINATETYPE *RotateX,
         ORDINATETYPE *RotateY,
         ORDINATETYPE OldX,
         ORDINATETYPE OldY,
         ORDINATETYPE RotateAxisX,
         ORDINATETYPE RotateAxisY,
         int RotateAngle);
int TriPointToAngle (ORDINATETYPE X1,
         ORDINATETYPE Y1,
         ORDINATETYPE X2,
         ORDINATETYPE Y2,
         ORDINATETYPE X3,
         ORDINATETYPE Y3);
int ConvertRotateAngle (int OldAngle,
         ORDINATETYPE OldAxisX,
         ORDINATETYPE OldAxisY,
         ORDINATETYPE NewAxisX,
         ORDINATETYPE NewAxisY,
         ORDINATETYPE *Left,
         ORDINATETYPE *Top);
void ArctoLine (int AxisX,
         int AxisY,
         int Rx,
         int Ry,
         int StartAngle,
         int EndAngle,
         int *TotalPoints,
         int *RPoints,
         int AngleInc);
/* tabdatac.c 26/10/96 00.43.08 */
/* undo.c 26/10/96 00.43.08 */
int UndoInitial (void);
void UndoFinish (void);
void UndoClear (void);
int UndoInsertCursorGoto (int OldPosition);
int UndoInsertCursorDefineBlock (int OldBlockStart, int OldBlockEnd);
int UndoInsertCursorUndefineBlock (int OldStart, int OldEnd);
int UndoInsertStringInsert (int Position, int StringLength);
int UndoInsertStringDelete (int Position,
         int StringLength,
         Wchar *String);
int UndoInsertStringExchange (int Position,
         int InsertStringLength,
         int DeleteStringLength,
         Wchar *DeleteString);
int UndoInsertImageInsert (char *OldImageName);
int UndoInsertImageDelete (char *OldImageName);
int UndoInsertImageMove (int OldMoveX, int OldMoveY);
int UndoInsertImageZoom (int OldZoomX, int OldZoomY);
int UndoInsertImageRotate (int OldRotate);
int UndoInsertImageSkew (int OldAngle);
int UndoInsertImageColor (int OldColor);
int UndoInsertTableSlip (int iCell, int type);
int UndoInsertTableCellMerge (int iMgLine,
         int iMgCol,
         int numMgLines,
         int numMgCols,
         HANDLE hUndoCellTable);
int UndoInsertTableDismerge (int iLineBegin,
         int iColBegin,
         int iLineNum,
         int iColNum);
int UndoInsertTableInsertLine (int nLineIndex);
int UndoInsertTableDeleteLine (int nLineIndex, int OldPosY);
int UndoInsertTableInsertCol (int nColIndex);
int UndoInsertTableDeleteCol (int nColIndex, int OldPosX);
int UndoInsertTableChangeHline (int nLineIndex, int OldType);
int UndoInsertTableChangeVline (int nColIndex, int OldType);
int UndoInsertTableChangeSlant (int iCell, int type);
int UndoInsertTableLineMove (int nLineIndex, int OldPosY);
int UndoInsertTableColMove (int nColIndex, int OldPosX);
int UndoInsertBoxMove (int OldPosX, int OldPosY);
int UndoInsertBoxResize (int OldPosX, int OldPosY);
int UndoInsertBoxRotate (int OldLeft,
         int OldTop,
         int OldRotateAngle,
         int OldRotateAxisX,
         int OldRotateAxisY);
int UndoInsertBoxSelect (HBOX OldHBox, int Position, int Cell);
int UndoInsertBoxCreat (HBOX HBox, HBOX OldBox);
int UndoInsertBoxDelete (HBOX HBox);
int UndoInsertCompose (int Number);
void Undo (void);
