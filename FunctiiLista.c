//Lăbău Cristea Andrei Liviu 314 CB
#include "utils.h"

/* Aloca celula lista */
TLista AlocCelula (void* x) {
    TLista aux;
    aux = (TLista)malloc(sizeof(TCell));
    if(!aux) {
        return NULL;
    }
    aux -> urm = NULL;
    aux -> info = x;
    return aux;
}

/* Inserare la final intr-o lista generica
 * L = lista 
 * x = elementul de inserat
*/
int InsertSf(TLista* L, void* x) {
    TLista aux, ultim, p;
    for(p = *L, ultim = NULL; p != NULL; ultim = p, p = p -> urm);
    aux = AlocCelula(x);
    if(!aux) {
        return 0;
    }
    if(ultim) {
        ultim -> urm = aux;
    }
    else {
        *L = aux;
    }
    return 1;
}

/* Distruge lista */
void DistrugeL(TLista* l) {
    TLista p = *l, aux;
    while(p != NULL) {
        aux = p;
        p = p -> urm;
        free(aux -> info);
        free(aux);
    }
    *l = NULL;
}