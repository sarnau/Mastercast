#include "GlobalDefines.h"#include "AgenturFax.h"#include "mc_db.h"#include "FAXstf.h"#include "Print.h"#include "GeosErrors.h"#include "MenuLib.h"#include "Utilities.h"#include <string.h>#include <ctype.h>FAgenturFax		*AFax;extern Boolean	PrintStructValid;extern TPrint	PrintStruct;/*** *	Constructor, Variablen initialisieren ***/				FAgenturFax::FAgenturFax(){	FaxPrintActive = false;			// Fax nicht aktiv	AFPrintStructValid = false;		// Drucker-Setup ung�ltig	AgenturId = 0L;					// ung�ltige Agentur-ID	ModellId = 0L;					// ung�ltige Modell-ID	strcpy(Ortsvorwahl, "040");		// eigene Ortvorwahl	strcpy(Amtsvorwahl, "0,");		// um das Amt zu erreichen}/*** *	Fax initialisieren ***/void			FAgenturFax::Init(void){	FaxStart();	FaxPrintActive = true;	// Drucker-Setup switchen	SavePrintStructValid = PrintStructValid; SavePrintStruct = PrintStruct;	PrintStructValid = AFPrintStructValid; PrintStruct = AFPrintStruct;}/*** *	Fax abmelden ***/void			FAgenturFax::Exit(void){	FaxStop();	FaxPrintActive = false;	// Drucker-Setup switchen	AFPrintStructValid = PrintStructValid; AFPrintStruct = PrintStruct;	PrintStructValid = SavePrintStructValid; PrintStruct = SavePrintStruct;}/*** *	Fax aktiv? ***/Boolean			FAgenturFax::Active(void){	return FaxPrintActive;}/*** *	Fax-Men�punkt enablen, wenn m�glich! ***/void			FAgenturFax::EnableMenu(Boolean both){	UNUSED(both);#if 0static char	FaxDisable[] = { -iFaxPreferences, -iFaxPageSetup, -iFaxPrint, 0 };MenuHandle		theMenu = GetMHandle(gFileId);AgenturStruct	*as;	MenuAble(gFileId, FaxDisable);			// erst die Men�s disablen	if(!FaxExist()) return;					// Kein FAXstf installiert? => disabled lassen!	EnableItem(theMenu, iFaxPreferences);	// Faxen-Setup enablen	EnableItem(theMenu, iFaxPageSetup);		// Faxen-Setup enablen	if(!both) return;						// Drucken nicht enablen => raus	as = DB_AGet(AgenturId);				// Agentur des Modells suchen	if(!as || !as->Fax[0]) return;			// Keine Fax-Nummer! => raus	EnableItem(theMenu, iFaxPrint);			// Faxen enablen#endif}/*** *	Fax an die Agentur vorbereiten (Page Setup) ***/Boolean			FAgenturFax::Setup(long aid, long mid){#if 1	UNUSED(aid); UNUSED(mid);	return false;#elseAgenturStruct	*as;Str255			FaxNum, AgenturName;	if(!FaxExist()) return false;		// Kein FAXstf installiert? (nur zur Sicherheit!)	ModellId = mid;	AgenturId = aid;	as = DB_AGet(aid);					// Agentur des Modells suchen	if(!as->Fax[0]) return false;		// Keine Fax-Nummer! =>	strcpy((char*)FaxNum, Amtsvorwahl);	if(!strncmp(as->Fax, Ortsvorwahl, strlen(Ortsvorwahl))) {	// Ortvorwahl identisch?		char	*s = as->Fax + strlen(Ortsvorwahl);		while(!isdigit(*s)) s++;								// Leerzeichen, etc. skippen		strcpy((char*)FaxNum, s);								// die dann einfach weglassen	} else {		strcpy((char*)FaxNum, as->Fax);	// Ansonsten: komplett w�hlen	}	CtoPstr((char*)FaxNum);	strcpy((char*)AgenturName, as->Name); CtoPstr((char*)AgenturName);	FaxDestination(FaxNum, "\p", AgenturName, AgenturName, "\p", "\p", true);	// restliche Infos sind nicht n�tig!	return true;#endif}/*** *	Fax an die Agentur abschicken (die Druck-Routine) ***/#define PRINT_DEBUG		1void			FAgenturFax::Send(TPPrPort port, THPrint p){	UNUSED(p); UNUSED(port);#if 0Rect			iRect;ModellStruct	*ms = DB_MGet(ModellId);			// Modell suchenAgenturStruct	*ap = DB_AGet(AgenturId);#if PRINT_DEBUGGrafPtr			savePort;WindowPtr		aWindow;Rect			wRect;#endif	if(gPrintPage < 0)		// Init _vor_ der ersten Seite?		return;				// nix zu tun =>	iRect = ((GrafPtr)port)->portRect;#if PRINT_DEBUG	GetPort(&savePort);	wRect = ((GrafPtr)port)->portRect;	OffsetRect(&wRect, 50, 50);	aWindow = NewCWindow(nil, &wRect, "\p", true, plainDBox, (WindowPtr)-1L, true, 0);	SetPort(aWindow);	iRect = ((GrafPtr)aWindow)->portRect;	InsetRect(&iRect, 2, 2);	// etwas verkleinern	FrameRect(&iRect);#endif	gPrintPage = 0;				// Fertig! Drucken beenden#if PRINT_DEBUG	while(!CheckKey(0x3B)) {}	// auf "Control" warten	DisposeWindow(aWindow);	SetPort(savePort);#endif#endif}/*** *	einige Programminterne Einstellungen zum Faxen ***/////	eigener Dialogfilter f�r L�ngen- und Ziffernbegrenzung von TextEdit-Feldern//static pascal Boolean	OwnNumFilter(DialogPtr d,EventRecord *event,short *item){TEHandle			t = ((DialogPeek)d)->textH;		// Handle auf aktuelles TextEdit-Feldshort				key;static char		AllowedKeys[] = " #()*,-0123456789MPT";	// Im W�hlstring erlaubte Tasten#define MaxNumLen	10int					i;	UNUSED(item);	while (0==0) {		if ((event->what != keyDown)&&(event->what != autoKey)) break;		if ((event->modifiers & cmdKey)&&			((event->message & charCodeMask) == 'v')) {	// Paste?			if (((*t)->teLength + TEGetScrapLength()				- ((*t)->selEnd - (*t)->selStart)) <= MaxNumLen) break;			SysBeep(1);			event->what = nullEvent;			return(false);						// Tastencode ignorieren		}		if ((event->modifiers & cmdKey)) break;	// Command gedr�ckt?		key = event->message & charCodeMask;		if(key >= 'a' && key <= 'z') {			key -= 0x20;						// in Gro�buchstaben wandeln			event->message &= 0xFFFFFFFF - charCodeMask;			event->message |= key;				//in Gro�buchstaben gewandelte Taste einsetzen		}		if(key >= 32) {			for(i=0;AllowedKeys[i] && AllowedKeys[i] != key;i++) {}			if(!AllowedKeys[i])				goto beep;		}		if ((*t)->teLength < MaxNumLen) break;	// Text evtl. zu lang?		if ((*t)->selStart != (*t)->selEnd) break;	// Bereich selektiert?		if (key < 32) break;					// Cursortaste o.�.?	beep:		SysBeep(1);		event->what = nullEvent;		return(false);						// Tastencode ignorieren	}//	return(OwnDialogFilter(d,event,item));	// allgemeinen Dialoghandler aufrufen	return true;}void			FAgenturFax::Preferences(void){#if 1	DebugStr("\pNoch nicht umgestellt!");#elseDialogPtr			d;short				item;ModalFilterUPP		myUProc = NewModalFilterProc(OwnNumFilter);	d = GetCenteredDialog(132,nil);	if(!d) return;				// Dialog vorhanden?	SetDialogText(d,5,(StringPtr)Amtsvorwahl);	SetDialogText(d,6,(StringPtr)Ortsvorwahl);	ShowWindow((WindowPtr)d);	while(1) {		OutlineDialogItem(d,1);		ModalDialog(myUProc, &item);		if(item == 1 || item == 3)			break;	}	if(item == 1) {				// Ok-Button		GetDialogText(d,5,(StringPtr)Amtsvorwahl);		GetDialogText(d,6,(StringPtr)Ortsvorwahl);	}	DisposDialog(d);	DisposeRoutineDescriptor(myUProc);#endif}