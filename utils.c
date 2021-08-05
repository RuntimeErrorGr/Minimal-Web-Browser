//Lăbău Cristea Andrei Liviu 314 CB
#include "utils.h"

Resource* get_page_resources(const char *URL_Name, int *n) {
    int i, j, name_len, url_name_len = strlen(URL_Name);
    unsigned int resource_nr = 0;
    uint32_t size = 0;
    if(URL_Name == NULL) {
        return NULL;
    }
    for(i = 0; i < url_name_len; i++) {
        resource_nr += URL_Name[i];
    }
    resource_nr %= 13;
    Resource *result = (Resource *)calloc(resource_nr, sizeof(Resource));
    *n = resource_nr;
    for(i = 0; i < resource_nr; i++) {
        sprintf(result[i].name, "%s-(%d)", URL_Name, i);
        size = 0;
	name_len = strlen(result[i].name);
        for(j = 0; j < name_len; j++) {
            size += result[i].name[j];
        }
        /* Some randomness */
        size ^= size << 3;
        size += size >> 5;
        size ^= size << 4;
        size += size >> 17;
        size ^= size << 25;
        size += size >> 6;
        /* 100MB Maximum size */
        result[i].dimension = size % 104857601;
        result[i].currently_downloaded = 0;
    }
    return result;
}

/* Seteaza bandwith */
void setBand(TBrowser* brws, unsigned long bandwidth) {
    brws -> bandwidth = bandwidth;
}

/* Initializeaza o structura pagina web */
TWebPage* AlocPage(char* url) {
    int n = 0;
    Resource* resources = get_page_resources(url, &n);
    if(!resources) {
        printf("ERROR: Can't get page resources.\n");
        return NULL;
    }
    TWebPage* page = calloc(1,sizeof(TWebPage));
    if(!page) {
        return NULL;
    }
    strcpy(page -> url, url);
    page -> num_res = n;
    page -> resources = calloc(n, sizeof(Resource));
    if(!page -> resources) {
        return NULL;
    }
    memcpy(page -> resources, resources, n * sizeof(Resource));
    return page;
}

/* Initializeaza o structura tab */
Tab* AlocTab() {
    Tab* tab = calloc(1,sizeof(Tab));
    if(!tab) { 
        return NULL;
    }
    tab -> current_page = calloc(1, sizeof(TWebPage));
    tab -> back_stack = InitS(sizeof(TWebPage));
    tab -> forward_stack = InitS(sizeof(TWebPage));
    return tab; 
}

/* Initializeaza structura browser */
TBrowser* OpenBrowser() {
    TBrowser* browser = calloc(1, sizeof(TBrowser));
    if(!browser) {
        return NULL;
    }
    Tab* tab = AlocTab();               
    TLista* L = &(browser -> tabs);
    InsertSf(L, tab);
    browser -> current_tab = *L;
    browser -> bandwidth = 1024;
    browser -> time = 0;
    browser -> historyQ = InitQ(21 * sizeof(char));
    browser -> priorDownQ = InitQ(sizeof(Resource));
    browser -> finishDownQ = InitQ(sizeof(Resource));
    return browser;
}

/* Deschidere tab */
int OpenTab(TBrowser* brws) {
    Tab* tab = AlocTab();                   // se aloca un tab
    TLista L = brws -> tabs;                
    InsertSf(&L, tab);                      // insereaza in lista de taburi
    if(!L) {
        return 0;
    }
    TLista p = L;
    for(; p -> urm != NULL; p = p -> urm);
    brws -> current_tab = p;                // seteaza tabul curent
return 1;
}

/* Inchidere tab */
void DelTab(TBrowser* brws) {
    TLista L = brws -> tabs;                                   
    TLista p = L, ant = NULL;
    for(; p -> urm != NULL; ant = p, p = p -> urm);
    if(p == brws -> current_tab) {
        brws -> current_tab = ant;
    }
    ant -> urm = NULL;
    free(p);
}

/* Schimba tabul curent */
void ChangeTab(TBrowser* brws, int index) {
    TLista L = brws -> tabs;
    TLista p = L;
    int k = 0;
    while (k < index) {
        p = p -> urm;
        k++;
    }
    brws -> current_tab = p;
}

/* Afiseaza url-ul paginii curente de pe un tab */
void DispTab(void* x, FILE* f) {
    TWebPage* page = calloc(1, sizeof(TWebPage));
    if(!page) {
        exit(1);
    }
    memcpy(page, ((Tab*)x) -> current_page, sizeof(TWebPage));
    if(!strcmp(page->url, "")) { 
        fprintf(f, "empty)\n");
    }
    else {
        fprintf(f, "%s)\n", page -> url);
    }
}

/* Afiseaza taburi deschise
 * L = lista taburi
*/
void PrintOpenTabs(TLista L, void (*DispEl)(void* x, FILE* f), FILE* f) {
    int nrTab = 0;
    TLista p = L;
    for(; p != NULL; p = p -> urm) {
        fprintf(f, "(%d: ", nrTab);
        DispEl(p -> info, f);
        nrTab++;
    }
}

/* Accesare adresa */
void Goto(TBrowser* brws, char* url) {
    IntrQ(brws -> historyQ, url);                       // istoric global                    
    TWebPage* page = AlocPage(url);
    TLista tabCell = brws -> current_tab;
    Tab* tab = (Tab*)tabCell -> info;
    if(tab -> current_page) {
        Push(tab -> back_stack, tab -> current_page);   // 
    }
    ResetS(tab -> forward_stack);                       // reset stiva
    memcpy(tab -> current_page, page, sizeof(TWebPage));
}

/* Accesarea paginii anterioare */ 
int Back(TBrowser* brws, FILE* f) {
    Tab* tab = (Tab*)brws -> current_tab -> info;
    TWebPage* aux = calloc(1, sizeof(TWebPage));
    if(!aux) {
        return 0;
    }
    if(!VIDAST(tab -> back_stack)) {
        Push(tab -> forward_stack, tab -> current_page);
        Pop(tab -> back_stack, aux);
        memcpy(tab -> current_page, aux, sizeof(TWebPage));
    }
    else {
        fprintf(f, "can't go back, no pages in stack\n");
    }
    return 1;
}

/* Accesarea paginii urmatoare */
int Forward(TBrowser* brws, FILE* f) {
    Tab* tab = (Tab*)brws -> current_tab -> info;
    TWebPage* aux = calloc(1, sizeof(TWebPage));
    if(!aux) {
        return 0;
    }
    if(!VIDAST(tab -> forward_stack)) {
        Push(tab -> back_stack, tab -> current_page);
        Pop(tab -> forward_stack, aux);
        memcpy(tab -> current_page, aux, sizeof(TWebPage));
    }
    else {
        fprintf(f, "can't go forward, no pages in stack\n");
    }
    return 1;
}

/* Afiseaza un element de tip char */
void afisChar(void* el, FILE* f) {
    fprintf(f, "%s\n", (char*)el);
}

/* Afisez coada istoricului global */
void AfisQ(TQueue* q, void (*afisEl)(void* x, FILE* f), FILE* f ) {
    TQueue* aux = InitQ(21 * sizeof(char));
    while(!VIDAQ(q)) {                          // daca am elemente in coada
        void* elem = calloc(21, sizeof(char));
        if(!elem) {
            exit(1);
        }
        ExtrQ(q, elem);                         // extragere primul element
        afisChar(elem, f);                      // afisare
        IntrQ(aux, elem);                       // introducere in coada aux.
    }
    while(!VIDAQ(aux)) {                        // refacere coada initiala
        void* elem = calloc(21, sizeof(char));
        if(!elem) {
            exit(1);
        }
        ExtrQ(aux, elem);                       
        IntrQ(q, elem);
    }
    DistrQ(aux);                                // distruge lista initiala
}

/* Sterge primele nrEntries intrari din istoricul global */
void DelHistory(TQueue* q, int nrEntries) {
    if(nrEntries == 0) {
        ResetQ(q);
    }
    else {
        int k = 0;
        while(k < nrEntries) {
            void* aux = calloc(21, sizeof(char));
             if(!aux) {
                exit(1);
            }
            ExtrQ(q, aux);
            free(aux);
            k++;
        }
    }
}

/* Afiseaza lista de resurse descarcarcabile ale unei pagini */
void AfisDownloads(TWebPage* page, FILE* f) {
    Resource* res = page -> resources;
    if(res) {
        int nrRes = page -> num_res;
        int i;
        for(i = 0; i < nrRes; i++) {
            fprintf(f, "[%d - ", i);
            fprintf(f, "\"%s\" : ", res[i].name);
            fprintf(f, "%ld]\n", res[i].dimension);
        }
    }
}

/* Descarca resursa cu indexul specificat */
int DownloadRes(TBrowser* brws, int index, FILE* f) {
    TLista tabCell = brws -> current_tab;
    Tab* tab = (Tab*)tabCell -> info;
    TWebPage* page = tab -> current_page;
    Resource* resource = calloc(1, sizeof(Resource));
    if(!resource) {
        return 0;
    }
    if(!page -> resources) {
        return 0;
    }
    memcpy(resource, &page -> resources[index], sizeof(Resource));
    unsigned long priority;
    priority = resource -> dimension - resource -> currently_downloaded;
    AQ auxQ = InitQ(sizeof(Resource));
    if(VIDAQ(brws -> priorDownQ)) {                 // daca coada e goala
        IntrQ(brws -> priorDownQ, resource);        // introduc in varf
    }
    else {
        int in = 0;
        int ok = 1;                                 
        while(!VIDAQ(brws -> priorDownQ)) {         // daca coada nu e goala
            Resource* aux = calloc(1, sizeof(Resource));
            if(!aux) {
                return 0;
            }
            ExtrQ(brws -> priorDownQ, aux);         // verific prioritatea
            if(aux -> dimension - aux -> currently_downloaded < priority) {
                IntrQ(auxQ, aux);                   
            }
            else {
                in = 1;
                if(ok == 1) {               // resursa inca nu a fost adaugata
                    IntrQ(auxQ, resource);  // adaug resursa
                    ok = 0;
                }
                IntrQ(auxQ, aux);
            }
            if(VIDAQ(brws -> priorDownQ) && in == 0) { 
                IntrQ(auxQ, resource);      // resursa inca nu a fost adaugata
            }
        }
        while(!VIDAQ(auxQ)) {               // refac coada initiala
            Resource* aux = calloc(1, sizeof(Resource));
            if(!aux) {
                return 0;
            }
            ExtrQ(auxQ, aux);
            IntrQ(brws -> priorDownQ, aux);
        }
        DistrQ(auxQ);                       // eliberez coada auxiliara
    }
    return 1;
}

/* Simuleaza trecerea unei secunde la comanda goto */
void ModifyDown(AQ q, unsigned long bandwidth) {
    AQ auxQ = InitQ(sizeof(Resource));
    Resource* aux = calloc(1, sizeof(Resource));
    if(!aux) {
        exit(1);
    }
    ExtrQ(q, aux);
    aux -> currently_downloaded += bandwidth;   // actualizez cantitatea
    IntrQ(auxQ, aux);                           
    while(!VIDAQ(q)) {                          // introduc toate celelalte 
        Resource* aux2 = calloc(1, sizeof(Resource));
        if(!aux) {
            exit(1);
        }
        ExtrQ(q, aux2);                          
        IntrQ(auxQ, aux2);                     
    }
    while(!VIDAQ(auxQ)) {                       // refac coada
        Resource* aux2 = calloc(1, sizeof(Resource));
        ExtrQ(auxQ, aux2);
        IntrQ(q, aux2);
    }
    DistrQ(auxQ);
}

/* Afiseaza istoricul descarcarilor nefinalizate*/
void DispDownHistory(AQ q, FILE* f) {
    TQueue* aux = InitQ(sizeof(Resource));
    while(!VIDAQ(q)) {
        Resource* elem = calloc(1, sizeof(Resource));
        if(!elem) {
            exit(1);
        }
        ExtrQ(q, elem);             // extrag cate un element pentru afisare
        unsigned long priority;
        priority = elem -> dimension - elem -> currently_downloaded;
        fprintf(f, "[\"%s\" : ", elem -> name);
        fprintf(f, "%ld/%ld]\n", priority, elem -> dimension);
        IntrQ(aux, elem);          // intorduc in coada aux
    }
    while(!VIDAQ(aux)) {           // refac coada initiala
        Resource* elem = calloc(1, sizeof(Resource));
        if(!elem) {
            exit(1);
        }
        ExtrQ(aux, elem);
        IntrQ(q, elem);
    }
    DistrQ(aux);                   // distrug coada auxiliara
}

/* Afiseaza istoricul descarcarilor finalizate */
void DispCompletedDown(AQ q, FILE* f) {
    AQ aux = InitQ(sizeof(Resource));
    while(!VIDAQ(q)) {
        Resource* elem = calloc(1, sizeof(Resource));
        if(!elem) {
            exit(1);
        }
        ExtrQ(q, elem);             // extrag cate un element pentru afisare
        fprintf(f, "[\"%s\" : completed]\n", elem -> name);
        IntrQ(aux, elem);           // intorduc in coada aux
    }
    while(!VIDAQ(aux)) {            // refac coada initiala
        Resource* elem = calloc(1, sizeof(Resource));
        if(!elem) {
            exit(1);
        }
        ExtrQ(aux, elem);
        IntrQ(q, elem);
    }
}

void Wait(AQ downQ, AQ finishQ, unsigned long load) {
    AQ auxQ1 = InitQ(sizeof(Resource));
    while(!VIDAQ(downQ)) {                            // am elemente in coada
        Resource* elem = calloc(1, sizeof(Resource));
        if(!elem) {
            exit(1);
        }
        ExtrQ(downQ, elem);                           // extrag cate un element
        unsigned long remaining = elem -> dimension - elem -> currently_downloaded;
        if(remaining > load) {                        // caz 1
            elem -> currently_downloaded += load;
            load = 0;                                 // actualizez load
            IntrQ(auxQ1, elem);                      
        }
        else if(remaining == load) {                  // caz 2
            elem -> currently_downloaded += load;     // actualizez load
            load = 0;
            IntrQ(finishQ, elem); 
        }
        else if(remaining < load) {                   // caz 3
            elem -> currently_downloaded += remaining;
            load -= remaining;                        // actualizez load
            IntrQ(finishQ, elem);                    
        }
    }
    while(!VIDAQ(auxQ1)) {                          // refac coada initiala
        Resource* elem = calloc(1, sizeof(Resource));
        if(!elem) {
            exit(1);
        }
        ExtrQ(auxQ1, elem);
        IntrQ(downQ, elem);
    }
    DistrQ(auxQ1);
}