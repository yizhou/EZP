#ifndef  GRAPHICS_H

  #define  GRAPHICS_H
  #include <graph.h>

struct viewporttype {
        int left, top, right, bottom;
        int clip;
};

enum EGA_COLORS {
    EGA_BLACK            =  0,      /* dark colors */
    EGA_BLUE             =  1,
    EGA_GREEN            =  2,
    EGA_CYAN             =  3,
    EGA_RED              =  4,
    EGA_MAGENTA          =  5,
    EGA_BROWN            =  6,
    EGA_LIGHTGRAY        =  8,       //By zjh swap
    EGA_DARKGRAY         =  7,     /* light colors */
    EGA_LIGHTBLUE        =  9,
    EGA_LIGHTGREEN       =  10,
    EGA_LIGHTCYAN        =  11,
    EGA_LIGHTRED         =  12,
    EGA_LIGHTMAGENTA     =  13,
    EGA_YELLOW           =  14,
    EGA_WHITE            =  15
};

#define BLACK                   EGA_BLACK
#define LIGHTBLUE               EGA_LIGHTBLUE
#define LIGHTGREEN              EGA_LIGHTGREEN
#define LIGHTCYAN               EGA_LIGHTCYAN
#define LIGHTRED                EGA_LIGHTRED
#define LIGHTMAGENTA            EGA_LIGHTMAGENTA
#define YELLOW                  EGA_YELLOW
#define WHITE                   EGA_WHITE

 /*--------- define some graphics functions ----------*/
#define setmode(MODE)           _setvideomode(MODE)
//#define initgraph()             setmode(_VRES16COLOR)
#define closegraph()            setmode(_TEXTC80)
#define setcolor(COLOR)         _setcolor(COLOR)
#define getcolor()              _getcolor()
#define setbkcolor(COLOR)       _setbkcolor(COLOR)
#define getbkcolor()            _getbkcolor()
#define moveto(X,Y)             _moveto(X,Y)
#define lineto(X,Y)             _lineto(X,Y)
#define line(X1,Y1,X2,Y2)       { moveto(X1,Y1); _lineto(X2,Y2); }
#define putpixel(X,Y,COLOR)     _setpixel(X,Y)
#define rectangle(X1,Y1,X2,Y2)  _rectangle(_GBORDER,X1,Y1,X2,Y2)
#define setfillstyle(W,COLOR)   setcolor(COLOR)
#define getlinestyle()          _getlinestyle()
#define setlinestyle(STYLE)     _setlinestyle(STYLE)
#define bar(X1,Y1,X2,Y2)        _rectangle(_GFILLINTERIOR,X1,Y1,X2,Y2)

#define fillpoly(N,ARRAY)       _polygon(_GFILLINTERIOR,N,(struct xycoord *)ARRAY)
// #define fillpoly(N,ARRAY)     { _setplotaction(_GPSET);_polygon(_GFILLINTERIOR,N,ARRAY); }

#define drawpoly(N,ARRAY)       _polygon(_GBORDER,N,(struct xycoord *)ARRAY)
// #define outtext(STRING)         _outtext(STRING)
#define outtextxy(X,Y,STRING)   { _moveto(X,Y); _outgtext(STRING); }

#define XOR_PUT                 _GXOR
#define OR_PUT                  _GOR
#define AND_PUT                 _GAND
#define COPY_PUT                _GPSET
#define setwritemode(MODE)      _setplotaction(MODE)
#define getwritemode()          _getplotaction()                //By zjh
#define getviewsettings(pInfo)  { short X1,Y1,X2,Y2;  \
                                  _getcliprgn(&X1,&Y1,&X2,&Y2);  \
                                  (pInfo)->left=X1;  (pInfo)->top=Y1; \
                                  (pInfo)->right=X2; (pInfo)->bottom=Y2; \
                                  /* (pInfo)->clip=1;*/ }
#define setviewport(X1,Y1,X2,Y2,CLIP)   \
                                { /* if(CLIP) _setcliprgn(X1,Y1,X2,Y2);*/ \
                                  _setviewport(X1,Y1,X2,Y2); }

#define getimage(X1,Y1,X2,Y2,P) _getimage(X1,Y1,X2,Y2,P)
#define putimage(X1,Y1,P,MODE)  _putimage(X1,Y1,P,MODE)
#define imagesize(X1,Y1,X2,Y2)  _imagesize(X1,Y1,X2,Y2)

//#define getmaxx()               639           // see ..\image\clplinec.c
//#define getmaxy()               479
//#define getmaxcolor()           15

#define circle(X,Y,R)           _ellipse(_GBORDER,X-R,Y-R,X+R,Y+R)
//#define arc(X,Y,ANG1,ANG2,R)
//#define pieslice(X,Y,ANG1,ANG2,R)

#define int86                   int386
#define int86x                  int386x

/*----------
#define CLI                     _disable();
#define STI                     _enable();
#define disable()               _disable();
#define enable()                _enable();

#define
--------*/


#endif
