/* clplinec.c 26/10/96 00.43.26 */
int getmaxx (void);
int getmaxy (void);
int getmaxcolor (void);
void Init_dc (void);
int Init_template (void);
void FinishTemplate (void);
void SetDeviceBoundary (int Right, int Bottom);
void SetViewportPolygon (int BoxDots, int *BoxXY);
void SetClipportPolygon (int BoxDots, int *BoxXY);
void HLine (int X1, int Y, int X2);
int FindRowFillData (int Row, short *buff);
/* floyd.c 26/10/96 00.43.26 */
/* imagec.c 07/05/97 21.12.44 */
void PointSkew (int *NewX,
	 int *NewY,
	 int OldX,
	 int OldY,
	 int SkewAxisX,
	 int SkewAxisY,
	 int SkewAngle);
int ImageGetNewHandle (ImageDescribes *TiffPresent);
int ZoomChangeBuff (unsigned char *p0,
	 int len,
	 short start,
	 short end,
	 int left,
	 char *buff);
int PictureBoxImportTiff (char *PictureFileName, HBOX HPictureBox);
int PictureBoxDisplayPicture (HBOX HPictureBox);
int PictureBoxClearPicture (HBOX HPictureBox);
int PictureBoxMoveClearPicture (HBOX HPictureBox, int MoveX, int MoveY);
int PictureBoxMovePicture (HBOX HPictureBox, int MoveX, int MoveY);
int PictureBoxRotatePicture (HBOX HPictureBox, int NewRotateAngle);
int PictureBoxSkewPicture (HBOX HPictureBox, int NewSkewAngle);
int PictureBoxZoomPicture (HBOX HPictureBox, float ZoomX, float ZoomY);
int PictureBoxSetPictureColor (HBOX HPictureBox, int NewColor);
int PictureBoxGetPictureColor (HBOX HPictureBox, int *Color);
int ImageChangeNewParameter (HBOX HPictureBox);
int PictureBoxClearImage (HBOX HPictureBox);
/* tifffill.c 11/05/97 20.53.28 */
int IsScreenPic (int x, int y);
int GetTiffCompreeLine (FILE *fp, char *buff, UINT myBytesPerRow);
int TiffToPresent (char *FileName,
	 ImageDescribes *TiffPresent,
	 char PutSign,
	 int DeviceDPI,
	 int DeviceColorBits,
	 HANDLE *ImageH24Data);
int BmpToPresent (char *FileName,
	 ImageDescribes *TiffPresent,
	 char PutSign,
	 int DeviceDPI,
	 int DeviceColorBits,
	 HANDLE *ImageH24Data);
int PictureToPresent (char *FileName,
	 ImageDescribes *TiffPresent,
	 char PutSign,
	 int DeviceDPI,
	 int DeviceColorBits,
	 HANDLE *ImageH24Data);
/* unlzw.c 26/10/96 00.43.26 */
int unlzw (char *sbuff, int slen, char *tbuff, int tlen);
