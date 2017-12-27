/*** *	Dialogbox-Klasse * *	�1994 Dekotec Info-Sys, Markus Fritze ***/#include "DialogItems.h"#include "DialogBase.h"#include "DoEvent.h"/*** *	Beschreibung eines "Nothing"-Item ***/// Diese Klasse kommt nicht ins Headerfile, sie ist nutzlosclass DialogNothingItem : public DialogItem {};DialogNothingItem	DNothingItem;	// das Objekt brauchen wir nur einmal/*** *	Beschreibung eines Progress-Bar-Items ***/#include "GlobalLib.h"ProgressBarItem		PBarItem;	// das Objekt brauchen wir nur einmalvoid			ProgressBarItem::Init(void){DialogBase	*d = itemObject->dBasePtr;	itemObject->data = 0L;	d->RedrawItem(itemObject->item, false);}void			ProgressBarItem::SetValue(short val){DialogBase	*d = itemObject->dBasePtr;	itemObject->data = val;	d->RedrawItem(itemObject->item, false);}short			ProgressBarItem::GetValue(void){	return itemObject->data;}void			ProgressBarItem::SetMaxValue(short val){DialogBase	*d = itemObject->dBasePtr;	itemObject->progress.maxval = val;	d->RedrawItem(itemObject->item, false);}short			ProgressBarItem::GetMaxValue(void){	return itemObject->progress.maxval;}void			ProgressBarItem::Draw(void){DialogBase	*d = itemObject->dBasePtr;	// Gr��e und Position des Progress-Bars ermitteln	Rect		mRect;	d->GetItemRect(itemObject->item, &mRect);	short		hsize = mRect.right - mRect.left - 1;	// Breite des Bars	d->SetGraphicPort();	PenNormal();	// Anzeige umrahmen	FrameRect(&mRect);	InsetRect(&mRect, 1, 1);	short		pDone;	if (itemObject->progress.maxval == 0)	pDone = 0;	else			pDone = (itemObject->data * hsize / itemObject->progress.maxval);	if (pDone > hsize) pDone = hsize - 1;	if(gQDVersion > 1) {	// Color Quickdraw Port?		RGBColor	foreColor, backColor;		GetForeColor(&foreColor);		GetBackColor(&backColor);		short	saver = mRect.right;		mRect.right = mRect.left + pDone;		RGBForeColor(&cDarkGray);		RGBBackColor(&cWhite);		PaintRect(&mRect);		mRect.left = mRect.right + 1;		mRect.right = saver;		RGBForeColor(&cSteelBlue);		RGBBackColor(&cBlack);		PaintRect(&mRect);		RGBForeColor(&foreColor);		RGBBackColor(&backColor);	} else {		FillRect(&mRect,&qd.black);		mRect.right = mRect.left + pDone;		EraseRect(&mRect);	}}/*** *	Beschreibung eines List-Items ***/DialogListItem		DListItem;	// das Objekt brauchen wir nur einmal/*** *	Liste gem. String-Resource f�llen. ***/void			DialogListItem::StrToList(short strListID, unsigned long bitmask){Str255		theString;short		index;	for(index=1;;index++) {		theString[0] = 0;		GetIndString(theString, strListID, index);	// String lesen		if(!theString[0])					// Ende der Liste			break;		if(bitmask & (1L << (index-1)))		// Bit gesetzt?			AddList(theString);				// an die Liste anh�ngen	}}/*** *	Selektierungs-Bitfeld gem. einer Liste erzeugen ***/unsigned long	DialogListItem::GetSelList(void){Cell			c;short			i;unsigned long	bitm = 0L;ListHandle	l = (ListHandle)itemObject->data;	for(i=0;i<(*l)->dataBounds.bottom;i++) {	// alle Zellen durchsuchen		c.h = 0; c.v = i;		if (LGetSelect(false,&c,l))				// selektierte Zelle gefunden?			bitm |= 1L<<i;						// Bit in der Maske setzen	}	return bitm;}/*** *	ListHandle des Objektes zur�ckgeben ***/ListHandle		DialogListItem::GetList(void){	return (ListHandle)itemObject->data;}/*** *	Selektierungs-Bitfeld gem. einer Liste erzeugen ***/void			DialogListItem::SetSelList(unsigned long bitm){Cell			theCell;int				i;ListHandle		theList = (ListHandle)itemObject->data;	theCell.h = 0; theCell.v = 0;	for(i=31; i>=0; i--) {						// alle Zellen durchsuchen		LSetSelect((bitm & (1L << i)), theCell, theList);		if(!LNextCell(true, true, &theCell, theList))			break;								// keine weiteren Zellen mehr�	}	LScroll(-16383, -16383, theList);			// zur 1. Zelle scrollen (falls gar keine mehr selektiert ist)	LAutoScroll(theList);						// und nun zur 1. selektierten Zelle}/*** *	Item in die Liste einsortieren ***/short			DialogListItem::SortToList(StringPtr s){Cell		c = {0,0};Str255		st;short		stLen;ListHandle	l = (ListHandle)itemObject->data;	LSetDrawingMode(false, l);						// st�ndiges Update ausschalten	c.v = LAddRow(1, 9999, l);				// erstmal eine Zeile anh�ngen	while(c.v > 0) {						// Tabellenanfang erreicht? => raus		c.v--;		stLen = 255;		LGetCell(st+1, &stLen, c, l);		// Vorg�nger-Zelle holen		st[0] = stLen;						// Pascal-String zusammensetzen		c.v++;		if(IUCompString(s,st)>=0) break;	// Position gefunden?		LSetCell(st+1, stLen, c, l);		// Item hochschieben		c.v--;	}	LSetCell(s + 1, s[0], c, l);			// Item setzen	LSetDrawingMode(true,l);						// st�ndiges Update wieder einschalten	LUpdate((*l)->port->visRgn,l);	return c.v;								// Zellen-Zeile zur�ckgeben}/*** *	Item an die Liste anh�ngen ***/short			DialogListItem::AddList(StringPtr s){Cell		c = {0,0};short		i = 0;	i = LAddRow(1, 9999, (ListHandle)itemObject->data);	// eine Zeile anh�ngen	c.v = i;									// eine Zeile anh�ngen	LSetCell(s + 1, s[0], c, (ListHandle)itemObject->data);	// Item �ndern	return i;									// Zellen-Zeile zur�ckgeben}/*** *	LDEF-Liste initialisieren ***/void			DialogListItem::Init(void){FontInfo	f;int			i;Rect		r;ListHandle	l;static Rect	dr = { 0,0,1,1 };Cell		cs = { 0,0 };DialogBase	*d = itemObject->dBasePtr;	d->SetGraphicPort();	d->GetItemRect(itemObject->item, &r);	GetFontInfo(&f);	i = f.ascent + f.descent + f.leading;				// Fonth�he	i = (r.bottom - r.top) - (r.bottom - r.top) % i;	// H�he des Items	r.bottom = r.top + i;								// untere Kante neu setzen	d->SetItemRect(itemObject->item, &r);	if (itemObject->list.flags & 0x4000) r.right -= (ScrollBar_Size - 1);	// Platz f�r den V-Slider	if (itemObject->list.flags & 0x2000) r.bottom -= (ScrollBar_Size - 1);// Platz f�r den H-Slider	l = LNew(&r, &dr, cs, itemObject->list.procId, d->GetDialogPtr(), false, false,		(itemObject->list.flags & 0x2000) == 0x2000, (itemObject->list.flags & 0x4000) == 0x4000);	ThrowIfMemFail_(l);	(*l)->selFlags = itemObject->list.selflags;	LDelRow(1,0,l);										// erste Zelle gleich wieder wegwerfen	itemObject->data = (long)l;	if(itemObject->list.resid) {						// Strings automatisch hinzuf�gen?		Str255		theString;		for(i=1 ;; i++) {			theString[0] = 0;			GetIndString(theString, itemObject->list.resid, i);	// String lesen			if(!theString[0])							// Ende der Liste				break;			if(itemObject->list.mask & (1L << (i-1)))	// Bit gesetzt?				AddList(theString);						// an die Liste anh�ngen		}	}}/*** *	LDEF Liste zeichnen ***/void			DialogListItem::Draw(void){Rect		frame;GrafPtr		aktPort;DialogBase	*d = itemObject->dBasePtr;	// Rechteck der Liste ermitteln	d->GetItemRect(itemObject->item, &frame);	// Liste nun sicher ausgeben	LSetDrawingMode(true, (ListHandle)itemObject->data);	// Liste updaten	LUpdate((*(ListHandle)(itemObject->data))->port->visRgn, (ListHandle)itemObject->data);	// Rahmen um die Liste zeichnen	GetPort(&aktPort);	if(aktPort->portBits.rowBytes & 0xC000) {		// Color Quickdraw Grafport?			RGBColor		saveColor;			GetForeColor(&saveColor);			InsetRect(&frame, -2, -2);			RGBForeColor(&cMediumGray);			MoveTo(frame.right - 1, frame.top);			LineTo(frame.left, frame.top);			LineTo(frame.left, frame.bottom - 1);			RGBForeColor(&cWhite);				MoveTo(frame.left +1, frame.bottom -1);			LineTo(frame.right -1, frame.bottom -1);			LineTo(frame.right -1, frame.top + 1);			RGBForeColor(&cBlack);				InsetRect(&frame, 1, 1);			FrameRect(&frame);			InsetRect(&frame, -1, -1);			RGBForeColor(&saveColor);	} else {		InsetRect(&frame, -1, -1);		FrameRect(&frame);	}}/*** *	Liste aktivieren bzw. deaktivieren ***/void			DialogListItem::Activate(Boolean flag){	LActivate(flag, (ListHandle)itemObject->data);}/*** *	Klick in die Liste abfangen ***/void			DialogListItem::Klick(Point p, short clicks){	UNUSED(clicks);	if((itemObject->list.selflags & 1) == 1)		return;	LClick(p, gTheEvent.modifiers, (ListHandle)itemObject->data);}/*** *	Liste freigeben ***/void			DialogListItem::Exit(void){	LDispose((ListHandle)itemObject->data);}/*** *	Beschreibung des Dialogbox-Zeichensatz-Items (zum Einstellen eines beliebigen Zeichensatzes) ***/DialogFontItem		DFontItem;	// das Objekt brauchen wir nur einmal/*** *	Dieses Item wird nur initialisiert und setzt dabei den Zeichensatz der Dialogbox ***/void			DialogFontItem::Init(void){DialogBase	*d = itemObject->dBasePtr;TEHandle	te;FontInfo	fInfo;DialogPtr	dp = d->GetDialogPtr();	d->SetGraphicPort();	SetFont(itemObject->dialogfont.fondId);	te = ((DialogPeek)dp)->textH;	(*te)->txSize = dp->txSize;	(*te)->txFont = dp->txFont;	GetFontInfo(&fInfo);	(*te)->lineHeight = fInfo.ascent + fInfo.descent + fInfo.leading;	(*te)->fontAscent = fInfo.ascent;}/*** *	Array aller vorhandenen Item-Klassen. Der Index auf das Array steht in der DObj-Resource ***/DialogItem		*DItems[] = { &DNothingItem, &DNothingItem, &PBarItem, &DListItem,							  &DNothingItem, &DFontItem };