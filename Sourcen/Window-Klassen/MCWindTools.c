#include "MCWindTools.h"#include "mc_db.h"#include "Utilities.h"#include <Movies.h>#include <string.h>#include <LFileStream.h>/*** *	Pfad und Filename f�r ein Bild zusammenbasteln und aufl�sen. Damit dann einen FSSpec f�llen * *	Bei Movies mu� die Qualit�t "highq" stets true sein! ***/static void			BuildFilename(FSSpec *file, long mid, short pic, Boolean highq, Boolean movie);static void			BuildFilename(FSSpec *file, long mid, short pic, Boolean highq, Boolean movie){Str63			fileName;Str63			stemp;ModellStruct	*ms = DB_MGet(mid);				// Modell suchenOSErr			iErr;	fileName[0] = 0;	if(ms) {		if(pic == 1 && !highq) {				// Die Bilder in hoher Qualit�t liegen in einem anderen Ordner			GetIndString(fileName, 4000, 2);	// Headsheet-Ordner		} else {			short	FolderId = 3;			if(highq) FolderId = 8;			if(movie) FolderId = 9;			GetIndString(fileName, 4000, FolderId);// Ordner			NumToString(ms->Agentur, stemp);	// Agentur-ID			ConcatPString(fileName, stemp);			ConcatPString(fileName, "\p:");			NumToString(mid, stemp);			// Modell-ID			ConcatPString(fileName, stemp);			ConcatPString(fileName, "\p:");		}		// Filename zusammenbasteln		NumToString(mid, stemp);				// Modell-ID		while(stemp[0] < 5) {			BlockMoveData(stemp+1, stemp+2, 5);			stemp[1] = '0';			stemp[0]++;		}		ConcatPString(fileName, stemp);		NumToString(pic, stemp);				// Bildname		while(stemp[0] < 3) {			BlockMoveData(stemp+1, stemp+2, 3);			stemp[1] = '0';			stemp[0]++;		}		ConcatPString(fileName, stemp);		if(movie)			ConcatPString(fileName, "\p.MOV");		else			ConcatPString(fileName, "\p.JPG");	}	iErr = FSMakeFSSpec(file->vRefNum, file->parID, fileName, file);	if(iErr == fnfErr) {		ConcatPString(fileName, "\p;1");		// Extension (";1" kommt vom ISO-Treiber)		iErr = FSMakeFSSpec(file->vRefNum, file->parID, fileName, file);	}}/*** *	Existiert ein Film zum entsprechenden Modell? ***/Boolean			MovieExist(long mid){FSSpec			file;	// Filename auf der CD zusammenbasteln	file.vRefNum = 0; 			// auf dem aktuellen Ordner	file.parID = 0; 			// im aktuellen Verzeichnis	BuildFilename(&file, mid, 1, true,true);	// erst den gew�nschten Film suchen	FInfo		fndrInfo;	OSErr		iErr = FSpGetFInfo(&file, &fndrInfo);	if(iErr)		return false;			// Datei nicht gefunden!	return true;}/*** *	Bild gem�� des Filenamens zeichnen, inkl. einer proportionalen Skalierung * *	Falls pos = false, wird p als linke obere Ecke angenommen *		  pos = true, p ist die rechte untere Ecke ***/void			MoviePlay(long mid, Boolean pos, Point p){FSSpec			file;short			movieResFile;OSErr			err;short			movieRTesID;Str255			movieName;Boolean			wasChanged;Movie			aMovie = 0L;WindowPtr		aWindow = nil, whichWindow;GrafPtr			oldPort = nil;Rect			movieBounds,wRect;MovieController	aController;Boolean			done;EventRecord		theEvent;ModellStruct	*ms;	if(!MovieExist(mid)) return;			// Movie nicht zu �ffnen => raus	// Filename auf der CD zusammenbasteln	file.vRefNum = 0; 			// auf dem aktuellen Ordner	file.parID = 0; 			// im aktuellen Verzeichnis	BuildFilename(&file, mid, 1, true, true);	// erst den gew�nschten Film suchen	err = OpenMovieFile(&file, &movieResFile, fsRdPerm);	if(err) goto raus;			// Film nicht zu laden => raus	movieRTesID = 0;			// Wir wollen den ersten Film	err = NewMovieFromFile(&aMovie, movieResFile, &movieRTesID, movieName, newMovieActive, &wasChanged);	CloseMovieFile(movieResFile);	if(err || !aMovie) goto raus;	// Film nicht zu laden => raus	GetPort(&oldPort);	// Fenster �ffnen	GetMovieBox(aMovie, &movieBounds);	OffsetRect(&movieBounds, -movieBounds.left, -movieBounds.top);	SetMovieBox(aMovie, &movieBounds);	wRect = movieBounds;	OffsetRect(&wRect, p.h, p.v);	if(pos)		OffsetRect(&wRect, -(wRect.right - wRect.left), -(wRect.bottom - wRect.top));	ms = DB_MGet(mid);							// Modell suchen	strcpy((char*)movieName, ms->Modellname);	C2PStr((char*)movieName);	aWindow = NewCWindow(nil, &wRect, movieName, false, plainDBox, (WindowPtr)-1L, true, 0);	SetMovieGWorld(aMovie, (CGrafPtr)aWindow, 0);	// Movie-Controller allozieren	aController = NewMovieController(aMovie, &movieBounds, mcTopLeftMovie);	MCGetControllerBoundsRect(aController, &movieBounds);	SizeWindow(aWindow, movieBounds.right - movieBounds.left, movieBounds.bottom - movieBounds.top, true);	ShowWindow(aWindow);									// Fenster sichtbar machen	MCDoAction(aController, mcActionSetKeysEnabled, (Ptr)true);	// Movie sofort starten//	MCDoAction(aController, mcActionPlay, (Ptr)Long2Fix(1L));	done = false;	while(!done) {		WaitNextEvent(everyEvent, &theEvent, 0, nil);		if(!MCIsPlayerEvent(aController, &theEvent)) {			switch(theEvent.what) {			case updateEvt:					whichWindow = (WindowPtr)theEvent.message;					BeginUpdate(whichWindow);					EndUpdate(whichWindow);					break;			case mouseDown:					FindWindow(theEvent.where, &whichWindow);					if(whichWindow != aWindow)						done = true;		// nicht der Movie => raus					break;			}		}	}	DisposeMovieController(aController);	DisposeMovie(aMovie);raus:	if(oldPort != nil)		SetPort(oldPort);	if(aWindow != nil)		DisposeWindow(aWindow);}/*** *	Bilddatei komplett in eine Handle einlesen. * *	mid = ID des Photomodells *	pic = Bildnummer (1��) *	highq = true, wenn m�glichst die hohe Qualit�t *	highq = false, wenn m�glichst das kleine Bild ***/#define PicCacheSize		500typedef struct {	Handle		Data;		// purgeable Handle auf die Bilddaten	long		MId;		// Modell-ID	short		Pic;		// Bild-ID	Boolean		Qual;		// Qualit�t (true = hoch, false = gering)	long		Count;		// Z�hler, wie h�ufig das Bild benutzt wurde} PicFileCacheStruct;PicFileCacheStruct	*PicFileCache;Handle			GetPictData(long mid, short pic, Boolean highq){FSSpec			file;Handle			theData;PicFileCacheStruct	*pcp, *pcl;long			mincount;Boolean			setupCache;	// Cache allozieren, wenn noch nicht geschehen	if(!PicFileCache)		PicFileCache = (PicFileCacheStruct*)NewPtrClear(sizeof(PicFileCacheStruct) * PicCacheSize);	// alle Eintr�ge auf purgeable setzen! (falls nicht schon geschehen)	for(int j = 0; j<PicCacheSize; j++)	{		if(PicFileCache[j].Data == nil || *PicFileCache[j].Data == nil)			continue;		HPurge(PicFileCache[j].Data);	}	mincount = 0x7FFFFFFFL;						// maximaler Wert, damit mindestens ein Bild gefunden wird	for(int i = 0; i<PicCacheSize; i++) {		pcp = &PicFileCache[i];		if(pcp->Count < mincount) {				// am seltenstes benutzes Bild merken			mincount = pcp->Count;			pcl = pcp;							// Ptr auf den Cache-Eintrag		}		if(pcp->MId != mid || pcp->Pic != pic || pcp->Qual != highq)	// Bild im Cache gefunden?			continue;							// Nein => weitersuchen		setupCache = false;						// Cache-Eintrag nur updaten		if(!pcp->Data)							// Bild war mal im Cache?		{//			TRACE("Gone    %5ld %2d %d\n", pcp->MId, pcp->Pic, pcp->Qual);			goto loadpic;						// Bild neu laden =>		}		if(!*pcp->Data) {						// Bild war mal im Cache?//			TRACE("Flushed %5ld %2d %d\n", pcp->MId, pcp->Pic, pcp->Qual);			DisposeHandle(pcp->Data);			// alte Resthandle freigeben			pcp->Data = nil;			goto loadpic;						// Bild neu laden =>		}//		TRACE("Found   %5ld %2d %d\n", pcp->MId, pcp->Pic, pcp->Qual);		pcp->Count++;							// Nutzung mitz�hlen		HNoPurge(pcp->Data);					// nicht aus Versehen freigeben�		return pcp->Data;						// Bild im Cache => Handle auf die Daten zur�ckgeben	}	setupCache = true;	pcp = pcl;									// seltensten Cache-Eintrag belegen	if(pcp->Data) {		DisposeHandle(pcp->Data);				// evtl. Daten in diesem Eintrag freigeben		pcp->Data = nil;	}loadpic:	// Zuerst suchen wir das Bild einmal	memset(&file, 0, sizeof(file));				// auf dem aktuellen Ordner, im aktuellen Verzeichnis, Filename l�schen	// 1. Versuch: in der gew�nschten Aufl�sung suchen	BuildFilename(&file, mid, pic, highq, false);		// erst das gew�nschte Bild suchen	FInfo		fndrInfo;	OSErr		iErr = FSpGetFInfo(&file, &fndrInfo);	if(iErr) {		// 2. Versuch: in der jeweils anderen Aufl�sung suchen		BuildFilename(&file, mid, pic, !highq, false);	// zur Not die andere Qualit�t nehmen		iErr = FSpGetFInfo(&file, &fndrInfo);	}	if(iErr)		return nil;	// nun versuchen wir die Bilddaten zu laden	try {//		TRACE("Load    %5ld %2d %d\n", mid, pic, highq);		LFileStream		theFile(file);		theFile.OpenDataFork(fsCurPerm);		theData = theFile.ReadDataFork();		HNoPurge(theData);						// nicht aus Versehen freigeben�		// geladenes Bild im Cache vermerken		pcp->Data = theData;		if(setupCache) {//			TRACE("Setup Cacheentry\n");			pcp->MId = mid;			pcp->Pic = pic;			pcp->Qual = highq;			pcp->Count = 1;		}	}	catch(PP_PowerPlant::ExceptionCode inErr) {		return nil;	}	return theData;}/*** *	FSSpec-Record eines Bildes ermitteln ***/OSErr			GetPictFSSpec(long mid, short pic, Boolean highq, FSSpecPtr fsp){OSErr		iErr;FInfo		fndrInfo;	if(!GetPictData(mid, pic, highq))		// Bild suchen		return fnfErr;						// => nicht gefunden	BuildFilename(fsp, mid, pic, highq, false);			// FSSpec des Bildes ermitteln	iErr = FSpGetFInfo(fsp, &fndrInfo);	if(iErr) {		BuildFilename(fsp, mid, pic, !highq, false);	// FSSpec des alternativen Bildes ermitteln		iErr = FSpGetFInfo(fsp, &fndrInfo);	}	return iErr;}/*** *	FSSpec-Record eines Bildes ermitteln ***/OSErr			GetPictHandle(long mid, short pic, Boolean highq, PicHandle *ph){Handle				d;extern PicHandle	JPEG2PicHandle(Handle data);	d = GetPictData(mid, pic, highq);		// Bild suchen	if(!d)		return fnfErr;						// => nicht gefunden	*ph = JPEG2PicHandle(d);				// Bild in eine PicHandle wandeln	HPurge(d);								// Freigeben ist m�glich	if(!*ph)		return fnfErr;	return noErr;}/*** *	Bildgr��e ermitteln, inkl. einer proportionalen Skalierung ***/OSErr			GetPictRect(long mid, short pic, Rect &iRect, Boolean highq, Boolean scale){Boolean			JFIFPictRect(Handle data, Rect &iRect);Handle			dHand;long			wide, high, a;Rect			r;	dHand = GetPictData(mid, pic, highq);	if(!dHand) return fnfErr;	JFIFPictRect(dHand, r);	HPurge(dHand);					// Freigeben ist m�glich	if(!scale) {					// nicht skalieren?		iRect = r;					// dann das Bild-Rechteck direkt zur�ckgeben		return noErr;	}	wide = r.right - r.left;		// calculate width and height	high = r.bottom - r.top;	a = high * (iRect.right - iRect.left) / wide + iRect.top;	if(a <= iRect.bottom)		iRect.bottom = a;	else		iRect.right = wide * (iRect.bottom - iRect.top) / high + iRect.left;	return noErr;}/*** *	Bild gem�� des Filenamens zeichnen, inkl. einer proportionalen Skalierung * *	R�ckgabecode: PicHandle, wenn Bild gezeichnet wurde; 0L, wenn nicht (wg. Clipping) ***/void			DrawPic(long mid, short pic, Rect &iRect, Boolean draw, Boolean highq){PicHandle		JFIF2Picture(Handle data);WindowPtr		ww;RgnHandle		theRgn;Rect			theRect;Rect			*r;long			wide, high;long			a;Handle			dHand;	PenNormal();	GetPort((GrafPtr*)&ww);	if(draw & 0x7F) {					// Bild darstellen oder nur skalieren? Beim Skalieren stets berechnen		theRgn = NewRgn();		InsetRect(&iRect, -1, -1);		RectRgn(theRgn, &iRect);		SectRgn(ww->visRgn, theRgn, theRgn);		InsetRect(&iRect, 1, 1);		if(EmptyRgn(theRgn)) {			// Bild �berhaupt sichtbar?			DisposeRgn(theRgn);			return;						// n� => raus		}		DisposeRgn(theRgn);	}	dHand = GetPictData(mid, pic, highq);	if(!dHand)		goto picnfound;					// dann ein Rechteck anstatt des Bildes zeichnen		if(GetPictRect(mid, pic, theRect, highq, false) != noErr) {	// Bild nicht zu laden? (nicht gefunden, o.�.)picnfound:		if(draw & 0x7F) {			EraseRect(&iRect);			MoveTo(iRect.left, iRect.top);			LineTo(iRect.right, iRect.bottom);			MoveTo(iRect.right, iRect.top);			LineTo(iRect.left, iRect.bottom);			InsetRect(&iRect, -1, -1);			FrameRect(&iRect);				// Rechteck mit Kreuz drin anstatt des Bildes zeichnen			InsetRect(&iRect, 1, 1);		}		if(draw & 0x80)						// Bildgr��e errechnen?			SetRect(&iRect, 0,0, 100, 100);	// willk�rliche Gr��e nehmen		return;								// und raus�	}	r = &theRect;	if((draw & 0x80) == 0x00) {		wide = r->right - r->left;		// calculate width and height		high = r->bottom - r->top;		a = high * (iRect.right - iRect.left) / wide + iRect.top;		if(a <= iRect.bottom)			iRect.bottom = a;		else			iRect.right = wide * (iRect.bottom - iRect.top) / high + iRect.left;	} else {		iRect = *r;	}	InsetRect(&iRect, -1, -1);	if(draw & 0x7F)		FrameRect(&iRect);				// Bild umrahmen	InsetRect(&iRect, 1, 1);	if(draw & 0x7F) {		extern void			JPEG2GrafPort(Handle dHand, Rect &iRect);		extern PicHandle	JPEG2PicHandle(Handle data);		if(highq) {		// hohe Qualit�t (Ausdruck)			PicHandle	thePicture = JPEG2PicHandle(dHand);			if(thePicture) {			// hohe Qualit�t �ber DrawPicture() ausgeben und nicht				DrawPicture(thePicture, &iRect);	// direkt blitten!				KillPicture(thePicture);// Warum? Weil die Daten dann als JPEGs an den			}							// Druckertreiber gehen (PostScript Level 2 unterst�tzt JPEG!)		} else {			JPEG2GrafPort(dHand, iRect);		}		HPurge(dHand);					// Freigeben ist m�glich	}}