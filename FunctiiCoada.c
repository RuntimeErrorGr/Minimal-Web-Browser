//Lăbău Cristea Andrei Liviu 314 CB
#include "utils.h"

/* Initializare coada
 * d = dimens. elementelor din coada 
*/
void* InitQ(size_t d) {
	AQ q;
	q = (AQ)malloc(sizeof(TQueue));
	if(!q) {
		return NULL;
	} 
	q -> dim = d;
	q -> ic = NULL;
	q -> sc = NULL;
	return (void*)q;
}

/* Inserare in coada
 * a = coada in care se efectueaza inserarea
 * ae = elementul de inserat
*/
int IntrQ(void* a, void* ae) {
	TLista aux = (TLista)malloc(sizeof(TCell));
	if(!aux) {
		return 0;
	}
	aux -> info = (void*)malloc(DIMQ(a));
	if(!aux->info) {
		return 0;
	}
	memcpy(aux -> info, ae, DIMQ(a));
	if (VIDAQ(a)) {
		aux -> urm = NULL;
		ICQ(a) = aux;
		SCQ(a) = ICQ(a);
	}
	else {
		aux -> urm = NULL;
		SCQ(a) -> urm = aux;
		SCQ(a) = aux;
	}
	return 1;
}

/* Extragere din coada
 * a = coada din care se efectueaza extragerea
 * ae = elementul extras
*/
int ExtrQ(void* a, void* ae) {
	if(VIDAQ(a)) {
		return 0;
	}
	TLista aux = ICQ(a);
	ICQ(a) = aux -> urm;
	memcpy(ae, aux -> info, DIMQ(a));
	free(aux -> info);
	free(aux);
	if(ICQ(a) == NULL) {
		SCQ(a) = NULL;
	}
	return 1;
}

/* Copiaza elementul din capul cozii
 * a = coada
 * ae = adresa la care se copiaza elementul din cap
*/
int PrimQ(void* a, void* ae) {
	if(VIDAQ(a)) {
		return 0;
	}
	memcpy(ae, ICQ(a)->info, DIMST(a));
	return 1;
}

/* Reseteaza coada
 * a = coada de resetat
*/
void ResetQ(void* a) {
	TLista p = ICQ(a);
	ICQ(a) = NULL;
	SCQ(a) = NULL;
	DistrugeL(&p);
}

/* Distruge coada
 * a = coada de distrus
*/
void DistrQ(void* a) {
	ResetQ(a);
	free(a);
}
