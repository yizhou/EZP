/* buildstr.c 03/06/97 13.56.26 */
int DetectDot16Font (int fontn);
int DetectDot13Font (int fontn);
int DetectDot24Font (int fontn);
int DetectDot24GBSymbol (void);
int detectEFont (int CFont);
int DetectVecFont (int fontn);
int DetectTrueTypeFont (int fontn);
int hh (void);
void BuildChineseChar (int PosX,
         int PosY,
         Wchar Code,
         int CFont,
         int Width,
         int Height,
         int Slant,
         int RotateAngle,
         int Color);
void BuildEnglishChar (int PosX,
         int PosY,
         Wchar Code,
         int EFont,
         int Width,
         int Height,
         int Slant,
         int RotateAngle,
         int Color);
/* cache.c 26/05/97 09.29.14 */
void InitCache (void);
void DestoryCache (void);
void CloseCache (void);
int GetCacheData (Wchar code,
         int font,
         int size,
         char *pBuffer,
         int Length);
void PutCacheData (Wchar code,
         int font,
         int size,
         char *pBuffer,
         int Length);
/* font.c 26/05/97 10.03.48 */
int FontInitial (void);
int FontEnd (void);
void set_buf_param (int w, int h, char *ptr);
void buf_sline (LPDC lpdc, int x1, int y1, int x2, int y2);
void buf_scanline (int x1, int x2, int y1, LPDC lpdc);
int BuildChar (int x,
         int y,
         Wchar code,
         SHORT fnt,
         int ww,
         int hh,
         SHORT Slant,
         SHORT rota,
         LONG Color,
         SHORT lib,
         USHORT chflag,
         USHORT pattern);
/* format.c 11/05/97 20.56.06 */
void mprintf (char *s);
void AssertFailed (char *FILE, int LINE, char *p);
Wchar Wtoupper (Wchar s);
size_t Wstrlen (Wchar *s);
void MakeWchar (unsigned char *s, Wchar *p);
Wchar *Wstrcat (Wchar *dest, Wchar *src);
Wchar *Wstrcpy (Wchar *dest, Wchar *src);
int CNoBegin(Wchar val);
int CNoEnd(Wchar val);
void GetTtfWidth (Wchar code, int font, USHORT *aw, SHORT *lsb);
int GetLineBottom (Pmark_rec eptr);
Pmark_rec LocateMarkbyPos (PTextBoxs hw, int start_pos);
int BoxCursorPosDown (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int linedown);
int PosToBoxCursorXY (HBOX HBox,
         HBOX *phBoxnew,
         int Pos,
         int *X,
         int *Y);
Pmark_rec BoxXYToPos (HBOX HBox,
         HBOX *NewHBox,
         int X,
         int Y,
         int *Pos);
int PosToBoxLineColumn (HBOX HBox,
         HBOX *NewHBox,
         int Pos,
         int *Line,
         int *Column);
int BoxLineColumnToPos (HBOX HBox,
         HBOX *NewHBox,
         int Line,
         int Column,
         int *Pos);
int FormatAll (HBOX HBox);
void TextBoxInitialLineTable (PTextBoxs TextBox);
void TextBoxDeleteLineTable (PTextBoxs TextBox);
void FormatChangeText (HBOX HBox,
         int StartPos,
         int ChangedLength,
         int OrigLength,
         int *StartChangeLine,
         int *ChangeLines,
         BOOL bEraseBk);
void FormatInsertText (HBOX HBox,
         int StartPos,
         int Length,
         int *StartChangeLine,
         int *ChangeLines,
         BOOL bEraseBk);
void FormatDeleteText (HBOX HBox,
         int StartPos,
         int Length,
         int *StartChangeLine,
         int *ChangeLines,
         BOOL bErase);
void EraseMark (Pmark_rec pmarkErase, BOOL bEraseBk);
void TextBoxRedrawPart (HBOX HBox,
         int StartLine,
         int LineSum,
         int StartPos,
         BOOL bEraseBk);
void TextBoxRedraw (HBOX HBox,
         int StartLine,
         int LineSum,
         BOOL bEraseBk);
void DisplayBlock (HBOX HBox, int posBStart, int posBStop);
int PosToBoxRowEnd (HBOX HBox,
         int Pos,
         int *NewPosition,
         int *Y,
         int *X);
int GetLineText (HBOX HBox, int Pos, char *str);
void AdjustTableCells (HBOX HBox);
void AddRowTableCells (HBOX HBox, int dir);
void AddColTableCells (HBOX HBox, int dir);
/* marklist.c 26/10/96 00.43.48 */
Pmark_rec AddEle (Pmark_rec *elistp, Pmark_rec current, Pmark_rec eptr);
void FreeMarkList (Pmark_rec list);
void FreeMarks (Pmark_rec *listhead,
         Pmark_rec list,
         Pmark_rec listend,
         BOOL bEraseBk);
/* polyfill.c 04/12/96 21.43.14 */
void DefaultEdgeFillLine (LPDC lpdc, int x1, int y1, int x2, int y2);
void DefaultLineFillLine (int x1, int x2, int y, LPDC lpdc);
VOID SortY (LPINT ybuf, LPINT indbuf, int cnt);
BOOL InsertEdge (LPFILLP pfill,
         int x0,
         int y0,
         int x1,
         int y1,
         int flag);
BOOL PolyFillPolygon (LPDC lpdc,
         LPPOINT lppoint,
         LPINT lpdotn,
         int polyn);
BOOL FillPolygon (LPDC lpdc, LPPOINT lppoint, int dotn);
/* region.c 11/05/97 20.56.56 */
int MaxRL (HBOX hBox, int *w, int *h);
int Y0ofRg (REGIONITEM *pRgItem);
void GetTXX0X1 (int xyDots[], int yCut, int *x0, int *x1);
int BoxGetLine (PTextBoxs pBox,
         int iRg,
         int y,
         int RequiredHeight,
         int *x0Line,
         int *wLine,
         int *yNew);
void GetRgXY (REGIONITEM *pRgItem, int xyDots[]);
void InsertTXRg (REGIONITEM rgList[],
         int *numRg,
         int posIns,
         int x0Top,
         int x1Top,
         int y0Top,
         int x0Bot,
         int x1Bot,
         int y1Bot);
void FreeRL (PTextBoxs pBox);
void InitRL (PTextBoxs pBox);
/* true.c 11/05/97 08.34.20 */
BOOL TTOutline (LPDC lpdc,
         Wchar code,
         int font,
         int ww,
         int hh,
         LPPOINT lppoint,
         LPINT lpdotn,
         LPINT lppolyn,
         LPMAT2 lpmatrix);
void TTWidth (LPDC lpdc,
         Wchar code,
         int font,
         USHORT *aw,
         SHORT *lsb);
