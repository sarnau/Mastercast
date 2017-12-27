/*** *	Datenbank-Schnittstelle f�r Festplatte und im RAM von mastercast. * *	�1994 Dekotec Abt. Info-Sys, Markus Fritze ***/#include "mc_db.h"#include <string.h>#include <stdlib.h>// Die Liste der Agenturen gibt es nur einmalAgenturList		*AList;/*** *	Agenturen-Verwaltung ***/void	AgenturList::Add(Agentur* agentur){	InsertLast(agentur);}void	AgenturList::Remove(Agentur* agentur){	Remove(agentur);}Agentur	*AgenturList::FindId(long id){TListIterator	t(this);Agentur			*a;	if((a = (Agentur*)t.First()) != nil) {			// erste Agentur ermitteln? Kein da? => raus!		do {			if(a->Id == id)							// ID gefunden?				return a;							// Ptr auf Agentur zur�ckgeben		} while((a = (Agentur*)t.Next()) != nil);	// zur n�chsten Agentur	}	return nil;}#if 0/*** *	Modell-Array sortieren ***/extern "C" static int	compfuncA(const AgenturStruct *m1, const AgenturStruct *m2);static int		compfuncA(const AgenturStruct *m1, const AgenturStruct *m2){	return strcmp(m1->Name, m2->Name);}void			DB_ASort(void){	qsort(AgenturList, AgenturAnz, sizeof(AgenturStruct*),#if THINK_C || __SC__	(__cmp_func)#else	(_Cmpfun*)#endif	compfuncA);}#endif/***** *    Modellverwaltung *****/#if !DEMOModellStruct	**ModellList;#elseModellStruct	*ModellList[MAXMODELLANZ + 1];#endifshort			ModellAnz;static long			GetNum(Ptr data, short illval);static long			GetNum(Ptr data, short illval){long	num = 0L;	for(; *data; data++) {		if(*data < '0' || *data > '9')			return illval;					// falls Key fehlerhaft: maximal 10 Modelle (Pseudo-Demo)		num *= 10L;		num += *data & 0x0F;	}	return num;}/*** *	Alle Datens�tze freigeben ***/void			DB_FreeAll(void){#if !DEMO	if(ModellList) {		for(int i=0;ModellList[i];i++)			DisposePtr((Ptr)ModellList[i]);		DisposePtr((Ptr)ModellList);		ModellList = 0L;		ModellAnz = 0;	}#else	memset(ModellList, 0, sizeof(ModellList));	ModellAnz = 0;#endif	if(AList) {		delete AList;		AList = nil;	}}/*** *	Modell-Array einrichten ***/short			maxMod = 10;void			DB_MInit(short count){#if !DEMO	ModellList = (ModellStruct**)NewPtrClear((count + 1) * sizeof(void*));#endif}/*** *	Modell gem�� der Id in die Datenbank einsortieren ***/void			DB_MInsert(ModellStruct *ms){#if DEMO	if(ModellAnz >= MAXMODELLANZ) return;#endif	ModellList[ModellAnz++] = ms;}/*** *	Modell-Array sortieren ***/static int		compfuncM(const void*p1, const void*p2){	const ModellStruct **m1 = (const ModellStruct **)p1;	const ModellStruct **m2 = (const ModellStruct **)p2;	return strcmp((*m1)->Modellname, (*m2)->Modellname);//	return (*m1)->Id - (*m2)->Id;}void			DB_MSort(void){	qsort(ModellList, ModellAnz, sizeof(ModellStruct*), compfuncM);}/*** *	Modell gem�� der Id in der Datenbank suchen. * *	R�ckgabe: Ptr auf die ModellStruct oder NULL, wenn Modell nicht gefunden ***/ModellStruct	*DB_MGet(long id){	for(int i=0; ModellList[i]; i++) {		if(ModellList[i]->Id != id)		// Modell schon in der Liste?			continue;		return ModellList[i];	}	return 0L;				// Modell nicht gefunden!}/*** *	Modell ab einer best. Position in der DB suchen ***/static char		*DB_GetDez(char *s, long *value){long	val = 0L;	while(*s >= '0' && *s <= '9') {		// alle Ziffern holen		val *= 10L;		val += *s - '0';		s++;	}	*value = val;	return s;}ModellStruct	**DB_MFound(char *s, ModellStruct **MList){char			*t;long			l;long			v;char			found;char			bef,cond;int				i;ModellStruct	*ml;	for(i=0;MList[i];i++) {		ml = MList[i];		t = s;		do {			if(*t == '\0')			// Stringende erreicht?				return MList + i;	// => Modell gefunden!			bef = t[0];				// Befehl (Struktur-ID)			cond = t[1];			// Bedingung merken			t = DB_GetDez(t + 2, &v);	// Vergleichswert holen			found = false;			switch(bef) {			// Struktur-ID			case 'a': l = ml->Agentur; goto agenturid;			case 'b': l = ml->Geschlecht; goto value;			case 'c': l = ml->Base; goto value;			case 'd': l = ml->GebJahr; goto number;			case 'e': l = ml->Groesse; goto number;			case 'f': l = ml->Brust; goto number;			case 'g': l = ml->Taille; goto number;			case 'h': l = ml->Huefte; goto number;			case 'i': l = ml->Konfektion; goto number;			case 'j': l = ml->Kragen; goto number;//			case 'k': l = ml->Bh; goto number;			case 'l': l = ml->Schuhe; goto number;			case 'm': l = ml->Hut; goto number;			case 'n': l = ml->Ring; goto number;			case 'o': l = ml->Haarfarbe; goto value;			case 'p': l = ml->Haartyp; goto value;			case 'q': l = ml->Haare; goto value;			case 'r': l = ml->Augenfarbe; goto value;			case 's': l = ml->Look; goto bitfield;			case 't': l = ml->Eignung; goto bitfield;			case 'u': l = ml->Talente; goto bitfield;			case 'v': l = ml->Fuehrerschein; goto bitfield;			case 'w': l = ml->Sportarten; goto bitfield;			case 'x': l = ml->Besonders; goto bitfield;			case 'y': l = ml->Sprachen; goto bitfield;			}			continue;agenturid:	do {				found = l == v;				if(found) break;				if(*t != ',') break;				t = DB_GetDez(t + 1, &v);	// weiteren Vergleichswert holen			} while(1);			while((*t >= '0' && *t <= '9') || *t == ',')				t++;					// Rest der Condition skippen			continue;// Bitfeld vergleichenbitfield:			if(!l)						// kein Wert definiert?				continue;				// dann trifft die Bedingung eh nicht zu�			if(cond == '&')				// AND				found = (v & l) == v;	// totale �bereinstimmung			else if(cond == '|')		// OR				found = (v & l) != 0L;	// min. eine �bereinstimmung			else DebugStr("\pWrong Bitfield-Cond");			continue;// eindeutiger Zahlenwert (z.B. die Haarfarbe, das Geschlecht)value:			found = (1L << (l-1)) & v;	// in der Auswahl enthalten?			continue;// Zahlen mit Wertebereich (z.B. die Gr��e)number:			if(!l)						// kein Wert angegeben?				continue;				// dann trifft die Bedingung NICHT zu!			if(cond == '<')				// Eintrag <= gesuchter Wert				found = l <= v;			// totale �bereinstimmung			else if(cond == '>')		// Eintrag >= gesuchter Wert				found = l >= v;			// min. eine �bereinstimmung			else DebugStr("\pWrong Number-Cond");			continue;		} while(found);	}	return 0L;					// kein passendes Modell gefunden}/*** *	erstes Modell gem. der Suchmaske in der DB suchen ***/ModellStruct	**DB_MFoundFirst(char *s){	return DB_MFound(s, ModellList);}/*** *	n�chstes Modell das auf die gleiche Suchmaske pa�t in der DB suchen ***/ModellStruct	**DB_MFoundNext(char *s, ModellStruct **ml){	if(!ml || !*ml) return 0L;	return DB_MFound(s, ml + 1);		// ab dem n�chsten Modell weitersuchen}