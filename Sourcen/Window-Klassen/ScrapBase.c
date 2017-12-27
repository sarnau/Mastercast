/*** *	Basisklasse f�r den internen Bilder-Scrap ***/#include "ScrapBase.h"ScrapBase	theScrap;/*** *	Scrap erstellen ***/			ScrapBase::ScrapBase(){	ScrapData = (ScrapStruct**)NewHandle(0L);	ThrowIfMemError_();}/*** *	Scrap l�schen ***/void		ScrapBase::ClrScrap(void){	SetHandleSize((Handle)ScrapData, 0L);	ThrowIfMemError_();}/*** *	Bild in den Scrap einf�gen ***/void		ScrapBase::AddScrap(ModellStruct *ms, short pic){long		l = GetHandleSize((Handle)ScrapData);ScrapStruct	*p;	SetHandleSize((Handle)ScrapData, l + sizeof(ScrapStruct));	ThrowIfMemError_();	p = (ScrapStruct*)(*(Handle)ScrapData + l);	p->ms = ms;			// Daten an den Buffer anh�ngen	p->pic = pic;}/*** *	Handle auf den Scrap zur�ckgeben ***/ScrapStruct	**ScrapBase::GetScrap(void){	return ScrapData;}/*** *	Wieviel Elemente liegen im Scrap? ***/short		ScrapBase::GetScrapSize(void){	return GetHandleSize((Handle)ScrapData) / sizeof(ScrapStruct);}