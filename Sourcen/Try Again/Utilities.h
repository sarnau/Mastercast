/*** *	Kleinkram-Routinen ***/#pragma once#include "GlobalDefines.h"extern const RGBColor	cWhite, cBlack, cSteelBlue, cMediumGray, cDarkGray;Boolean		CheckKey(short keyCode);typedef struct {	short		txFont;		// Font-ID	short		txMode;		// Transfer-Modus	short		txSize;		// Text-Gr��e	Style		txFace;		// Text-Stil	RGBColor	fGround;	// Vordergrundfarbe	RGBColor	bGround;	// Hintergrundfarbe	long		fgColor;	// Farben ohne Color Quickdraw	long		bkColor;} FontStruct;extern FontStruct		fDefault;void		SaveText(FontStruct *f);void		RestoreText(const FontStruct *f);short		SetFont(short resid);void		ConcatPString(Str255 d,Str255 s);void		CopyPString(Str255 d,Str255 s);Boolean		ComparePString(Str255 s1,Str255 s2);/*** *	Handy Macros/inlines ***//* define our own abs() so we don't need StdLib */#define ABS(val) (((val) < 0) ? (-(val)) : (val))/* Pascal string copy */#define PStrCopy(dest, src)	(BlockMove (src, dest, (*(char *)(src))+1))#define TopLeft(r)		(* (Point *) &(r).top)#define BotRight(r)		(* (Point *) &(r).bottom)#define HiWrd(aLong)	(((short *) &(aLong))[0])#define LoWrd(aLong)	(((short *) &(aLong))[1])#define MIN(a, b) ((a) < (b) ? (a) : (b) )#define MAX(a, b) ((a) > (b) ? (a) : (b) )#define SETPT(pt, x, y)	(*(pt)).h = (x); (*(pt)).v = (y)#define SETRECT(r, left, top, right, bottom)	\						SETPT(&TopLeft(*(r)), (left), (top)); \						SETPT(&BotRight(*(r)), (right), (bottom))/* *	Useful macros for testing gestalt attribute responses * *	BTstBool returns a true boolean value (0 or 1), but is slower than: *	BTstQ which simply returns a non-zero value if the bit is set which *	means the result could get lost if assigned to a short, for example. * *	arg is any integer value, bitnbr is the number of the bit to be tested. *	bitnbr = 0 is the least significant bit. */#define BTstBool(arg, bitnbr)	(((arg) >> (bitnbr)) & 1)#define BTstQ(arg, bitnbr)		((arg) & (1L << (bitnbr)))#define chBackspace				'\b'		/* ASCII code for Backspace character */#define chClear					'\033'		/* ASCII code for Clear key (aka ESC) */#define chDown					'\037'		/* ASCII code for down arrow */#define chEnd					'\004'		/* ASCII code for the End key */#define chEnter					'\003'		/* ASCII code for Enter character */#define chEscape				'\033'		/* ASCII code for Escape (aka Clear) key */#define chFunction				'\020'		/* ASCII code for any function key */#define chFwdDelete				'\177'		/* ASCII code for forward delete */#define chHelp					'\005'		/* ASCII code for Help key */#define chHome					'\001'		/* ASCII code for the Home key */#define chLeft					'\034'		/* ASCII code for left arrow */#define chPageDown				'\f'		/* ASCII code for Page Down key */#define chPageUp				'\013'		/* ASCII code for Page Up key */#define chReturn				'\r'		/* ASCII code for Return character */#define chRight					'\035'		/* ASCII code for right arrow */#define chSpace					' '			/* ASCII code for Space character */#define chTab					'\t'		/* ASCII code for Tab character */#define chUp					'\036'		/* ASCII code for up arrow */