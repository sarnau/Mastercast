/*** *	Selektions-Fenster ***/#pragma once#include "MCLDEF.h"#include "mc_db.h"typedef struct {	ModellStruct	*ms;	long			pic;} LDEFBrowserStruct;class MCBrowser : public MCLDEF {protected:	virtual void	OpenObject(Cell theCell);	virtual short	Compare(Ptr a, Ptr b);	virtual void	LDEFCallback(short lMessage, Boolean lSelect, Rect *lRect, Cell lCell, short lDataOffset, short lDataLen, ListHandle lHandle);	virtual void	OpenList(Boolean infoFlag);	virtual 		~MCBrowser();	// Men�-Commandos	virtual Boolean	DoCommand(long cmd);	virtual void	GetCommandStatus(long cmd, Boolean &enabled, short &markChar, Str255 &name);	virtual OSErr	SSendData(FlavorType theFlavor, ItemReference theItem, DragReference theDrag);	virtual OSErr	SStartDrag(EventRecord *theEvent);public:	static void		Create(void);};pascal short	LDEFBrowserSearchFunk(Ptr cellDataPtr, Ptr searchDataPtr, short cellDataLen, short searchDataLen);