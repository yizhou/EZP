/* boxeditc.c 11/12/96 05.02.08 */
int EditBufferDeleteString (Wchar *Source,
         TEXTTYPE Position,
         TEXTTYPE Length,
         TEXTTYPE *SourceLength);
int EditBufferSearchNextAttribute (Wchar *Source,
         TEXTTYPE Position,
         TEXTTYPE RightBound,
         unsigned char SearchAttribute,
         TEXTTYPE *AttributePosition);
int EditBufferSearchNextExtraChar (Wchar *Source,
         TEXTTYPE Position,
         TEXTTYPE RightBound,
         Wchar CharCode);
int TextBoxInsertString (HBOX HTextBox,
         TEXTTYPE Position,
         Wchar *String,
         TEXTTYPE DestLength);
int TextBoxDeleteString (HBOX HTextBox,
         TEXTTYPE Position,
         TEXTTYPE DestLength);
int TextBoxExchangeString (HBOX HTextBox,
         TEXTTYPE Position,
         Wchar *String,
         TEXTTYPE ExchangeLength,
         TEXTTYPE DestLength);
int TextSearchAttribute (HBOX HTextBox,
         TEXTTYPE Position,
         char SearchAttribute,
         TEXTTYPE *AttributePosition);
int NewTextSearchAttribute (HBOX HTextBox, TEXTTYPE Position);
int TextSearchCFont (HBOX HTextBox,
         TEXTTYPE Position,
         TEXTTYPE *AttributePosition);
int TextSearchEFont (HBOX HTextBox,
         TEXTTYPE Position,
         TEXTTYPE *AttributePosition);
int TextChangeAttribute (HBOX HTextBox,
         TEXTTYPE Position,
         int ChangeLength,
         int ChangeItem,
         int Attribute,
         TEXTTYPE *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int TextBoxSeekTextPosition (HBOX HTextBox,
         TEXTTYPE Position,
         int Length,
         TEXTTYPE *NewPosition);
Wchar TextPositionChar (HBOX HTextBox, TEXTTYPE Position);
int TableCellGetTextHead (HBOX HTableBox, int CellNumber);
int TrueCellIsFirstCell (HBOX HTableBox, int TextPosition);
int TableTextGetCellNumber (HBOX HTableBox, int TextPosition);
int TableCellGetTextLength (HBOX HBox, int TableCell);
void DisplayCellBlock (HBOX HTableBox, int StartCell, int EndCell);
void CancelCellBlock (HBOX HTableBox);
int BoxGetCursor (HBOX HBox,
         int WindowX,
         int WindowY,
         TEXTTYPE *Position,
         int *BlockStart,
         int *BlockEnd);
int BoxGetSelectBlock (HBOX HBox,
         int WindowX,
         int WindowY,
         TEXTTYPE *Position);
int TextBoxSeekNextWord (HBOX HTextBox,
         TEXTTYPE Position,
         TEXTTYPE *NewPosition);
int TextBoxSeekPrevWord (HBOX HTextBox,
         TEXTTYPE Position,
         TEXTTYPE *NewPosition);
/* cursorc.c 12/05/97 13.27.52 */
void DispFontWindow (int tt);
void TextCursorDisplay (void);
void CreatFontWindow (int x1, int x2);
void CreatStatusWindow (void);
int TextCreatCursor (HWND Window,
         int Height,
         int RotateAxisX,
         int RotateAxisY,
         int RotateAngle);
void TextCursorOff (void);
void TextCursorMoveTo (int MovetoX, int MovetoY);
void TextCursorSetHeight (int Height);
void TextDestroyCursor (int *Cursor);
void TextCursorSetRotate (int RotateAxisX,
         int RotateAxisY,
         int RotateAngle);
void CloseTextCursor (void);
void OpenTextCursor (void);
/* findc.c 26/10/96 00.43.18 */
int find_str (Wchar *str, Wchar *sub_str, long len_str, int Sign);
int FindText (HBOX *iHBox,
         FindStructs *aFS,
         int *Position,
         int *BlockStart,
         int *BlockEnd);
void FindorReplaceNext (HBOX *HBox,
         FindStructs *FindStruct,
         int *Position,
         int *BlockStart,
         int *BlockEnd);
/* keyedit.c 07/05/97 11.10.06 */
int BoxXToWindowX (ORDINATETYPE X, TextBoxs *TextBox);
int BoxIsModule (char *s, HBOX HBox);
int BoxYToWindowY (ORDINATETYPE Y, TextBoxs *TextBox);
int CursorLocate2 (HBOX HBox,
         HBOX *pNewBox,
         int Position,
         int *X,
         int *Y);
int CursorLocate (HBOX HBox,
         HBOX *pNewBox,
         int Position,
         int *X,
         int *Y);
int CancelBlock (HBOX HBox, int *BlockStart, int *BlockEnd);
int CursorCtrlLeft (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorCtrlRight (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorLeft (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int CursorRight (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int CursorUp (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int CursorDown (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int CursorGotoLine (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int NewLine,
         int *BlockStart,
         int *BlockEnd);
int CursorHome (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int TableCursorEnd (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorEnd (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorPgUp (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorPgDn (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorStoryHome (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorStoryEnd (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorShiftLeft (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int CursorShiftRight (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int CursorShiftUp (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int CursorShiftDown (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int CursorShiftHome (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorShiftEnd (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorShiftStoryHome (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int CursorShiftStoryEnd (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int TextBoxDel (HBOX HBox,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int TextBoxDelKey (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int TextBoxBackSpace (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd);
int TextBoxEnterKey (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd,
         Wchar *KeyString);
int TextBoxKey (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd,
         Wchar *KeyString);
int TextBoxOverwriteKey (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int Length,
         int *BlockStart,
         int *BlockEnd,
         Wchar *KeyString);
int TextBoxInsertBox (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd,
         HBOX InsertHBox);
int TextBoxTABKey (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
int TextBoxShiftTabKey (HBOX HBox,
         HBOX *NewHBox,
         int Position,
         int *NewPosition,
         int *BlockStart,
         int *BlockEnd);
long TextBoxGetRemainSize(HBOX HBox);
int TableBoxInsertText (HBOX HBox, int iCell, char *buff, BOOL fErase);
int calculate (HBOX HBox, int Position, int *NewPosition, int fPerson);
void AdjustCtrl_KB_pos (HBOX HBox, int Position, int *NewPosition);
void AdjustCtrl_KK_pos (HBOX HBox, int Position, int *NewPosition);
