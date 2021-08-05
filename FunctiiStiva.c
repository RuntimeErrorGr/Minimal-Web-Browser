//Lăbău Cristea Andrei Liviu 314 CB
#include "utils.h"

/* Initializare stiva generica
 * d = dimens. elementelor din stiva
*/
void* InitS(size_t d) {
	ASt s;
	s = (ASt)malloc(sizeof(TStack));
	if(!s) {
		return NULL;
	} 
	s -> dim = d;
	s -> vf = NULL;
	return (void*)s;
}

/* Inserare in stiva
 * a = stiva in care se efectueaza inserarea
 * ae = elementul de inserat 
*/
int Push(void* a, void* ae) {
	TLista aux = (TLista)malloc(sizeof(TCell));
	if(!aux) {
		return 0;
	}
	aux -> info = malloc(DIMST(a));
	if(!aux -> info) {
		return 0;
	}
	memcpy(aux -> info, ae, DIMST(a));
	aux -> urm = VFST(a);
	VFST(a) = aux;
	return 1;
}

/* Extragere din stiva
 * a = stiva din care se efectueaza extragerea
 * ae = elementul extras
*/
int Pop(void* a, void* ae) {
	if(VIDAST(a)) {
		return 0;
	}
	memcpy(ae, VFST(a)->info, DIMST(a));
	TLista aux = VFST(a);
	VFST(a) = aux -> urm;
	free(aux);
	return 1;
}

/* Copiaza elementul din varful stivei la adresa ae
 * a = stiva
 * ae = adresa la care se copiaza elementul din varf
*/
int Top(void* a, void* ae) {
	if(VIDAST(a)) {
		return 0;
	}
	memcpy(ae, VFST(a)->info, DIMST(a));
	return 1;
}

/* Reseteaza stiva
 * a = stiva de resetat
*/
void ResetS(void* a) {
	TLista L = VFST(a);
	DistrugeL(&L);
	VFST(a) = NULL;
}

/* Distruge stiva
 * a = stiva de distrus
*/
void DistrS(void* a) {
	TLista L = VFST(a);
	DistrugeL(&L);
	VFST(a) = NULL;
	free(a);
}
