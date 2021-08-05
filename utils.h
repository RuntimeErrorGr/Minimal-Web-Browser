//Lăbău Cristea Andrei Liviu 314 CB
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifndef _DOWNLOAD_RESOURCE_
#define _DOWNLOAD_RESOURCE_

// Lista generica
typedef struct Cell {
	struct Cell* urm; 	// adres. urmatoarei celule
	void* info;			// adres. informatiei
} TCell, *TLista;

// Stiva generica
typedef struct Stack {
	size_t dim;			// dim. element
	TLista vf;			// adresa celulei din varf
} TStack, *ASt;

// Coada generica 
typedef struct Queue {
	size_t dim;			// dim. element
	TLista ic, sc;		// adresa inceput si sfarsit
} TQueue, *AQ;

// Resursa
typedef struct {
    char name[100];
    unsigned long dimension;
    unsigned long currently_downloaded;
} Resource;

// Pagina web
typedef struct {
	char url[21];
	unsigned int num_res;
	Resource* resources;
} TWebPage;

// Tab
typedef struct {
	TWebPage* current_page;			// adresa pagina curenta
	ASt back_stack;					// back stack
	ASt forward_stack; 				// forward stack
} Tab;

// Browser
typedef struct {
	TLista tabs;					// lista taburi
	TLista current_tab;				// adresa tab curent
	AQ historyQ;					// coada istoric global
	AQ priorDownQ;					// coada descarcari in derulare
	AQ finishDownQ;					// coada descarcari finalizate
	unsigned long bandwidth;		// lungime de banda
	unsigned long time;				// timp pentru wait
} TBrowser;

/* macrodefinitii stiva - acces campuri */
#define VFST(a) (((ASt)(a)) -> vf)
#define DIMST(a) (((ASt)(a)) -> dim)
#define VIDAST(a) (VFST(a) == NULL)

/* macrodefinitii coada - acces campuri */
#define DIMQ(a) (((AQ)(a)) -> dim)
#define ICQ(a) (((AQ)(a)) -> ic)
#define SCQ(a) (((AQ)(a)) -> sc)
#define VIDAQ(a) (ICQ(a) == NULL && SCQ(a) == NULL)
#define COMPLETED(a) (a -> currently_downloaded == a -> dimension)

// headere functii utils
Tab* AlocTab();
TBrowser* OpenBrowser();
int OpenTab(TBrowser* brws);
void DelTab(TBrowser* brws);
TWebPage* AlocPage(char* url);
void DispTab(void* x, FILE* f);
void afisChar(void* el, FILE* f);
int Back(TBrowser* brws, FILE* f);
void DispDownHistory(AQ q, FILE* f);
void Goto(TBrowser* brws, char* url);
int Forward(TBrowser* brws, FILE* f);
void DispCompletedDown(AQ q, FILE* f);
int PrintHistory(AQ history, FILE* f);
void ChangeTab(TBrowser* brws, int index);
void DelHistory(TQueue* q, int nrEntries);
void AfisDownloads(TWebPage* page, FILE* f);
void ModifyDown(AQ q, unsigned long bandwidth);
int DownloadRes(TBrowser* brws, int index, FILE* f);
void Wait(AQ downQ, AQ finishQ, unsigned long load);
void setBand(TBrowser* brws, unsigned long bandwidth);
Resource* get_page_resources(const char *URL_Name, int *n);
void AfisQ(TQueue* q, void (*afisEl)(void* x, FILE* f), FILE* f );
void PrintOpenTabs(TLista L, void (*DispEl)(void*, FILE* f), FILE* f);

// headere functii lista
void DistrugeL(TLista* l);
TLista AlocCelula (void* x);
int InsertSf(TLista* L, void* x);

// headere functii Stiva
void ResetS(void* a);
void DistrS(void* a);
void* InitS(size_t d);
int Pop(void* a, void* ae);
int Top(void* a, void* ae);
int Push(void* a, void* ae);

// headere functii coada
void ResetQ(void* a);
void DistrQ(void* a);
void* InitQ(size_t d);
int IntrQ(void* a, void* ae);
int ExtrQ(void* a, void* ae);
int PrimQ(void* a, void* ae);
#endif /* _DOWNLOAD_RESOURCE_ */
