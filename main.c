//Lăbău Cristea Andrei Liviu 314 CB
#define MAX_LEN_CMD 100
#define DEFAULT_BAND 1048576
#define MAX_LEN_LINE 256
#define MAX_FILE_NAME 12
#define MAX_COMS 1000

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "utils.h"

int main(int argc, char *argv[]) {
	TBrowser* brws = OpenBrowser();						// initializare browser
	char InFileName[MAX_FILE_NAME];
	strcpy(InFileName, argv[1]);
	FILE* in_file = fopen(InFileName, "rt");			// deschidere fisier in
	if(in_file == NULL) {
		fprintf(stderr, "ERROR: Can't open file %s\n", InFileName);
		return -1;
	}
	char OutFileName[MAX_FILE_NAME];
	strcpy(OutFileName, argv[2]);
	FILE* out_file = fopen(OutFileName, "wt");			// deschidere fisier out
	if(out_file == NULL) {
		fprintf(stderr, "ERROR: Can't open file %s\n", OutFileName);
		return -1;
	}
	char line[MAX_LEN_CMD];
	char* buffer = malloc(MAX_LEN_CMD * MAX_COMS * sizeof(char));
	if(!buffer) {
		printf("ERROR\n");
		return -1;
	}
	while(fgets(line, sizeof(line), in_file) != NULL) { 	// citeste info
		strcat(buffer, line);
	}
	fclose(in_file);
	char* p = strtok(buffer, " \n");
	while(p) {
		if(strncmp(p, "goto", 4) == 0) {
			p = strtok(NULL, " \n");
			char* url = p;
			Goto(brws, url);
			if(!VIDAQ(brws -> priorDownQ)) {				// trece 1 sec. 
				ModifyDown(brws -> priorDownQ, brws -> bandwidth);
			}
			if(ICQ(brws -> priorDownQ)) {					
				Resource* aux = calloc(1, sizeof(Resource));
				if(!aux) {
					exit(1);
				}
				PrimQ(brws -> priorDownQ, aux);
				if(COMPLETED(aux)) {						
					Resource* aux2 = calloc(1, sizeof(Resource));
					if(!aux2) {
						exit(1);
					}
					ExtrQ(brws -> priorDownQ, aux2);	
					IntrQ(brws -> finishDownQ, aux2);
				}
			}
		}
		if(strncmp(p, "set_band", 8) == 0) {
			char* band = p + 9;
			unsigned long bandwidth = atol(band);
			setBand(brws, bandwidth);
		}
		if(strcmp(p, "newtab") == 0) {
			OpenTab(brws);
		}
		if(strcmp(p, "deltab") == 0) {
			DelTab(brws);
		}
		if(strncmp(p, "change_tab", 10) == 0) {
			p = strtok(NULL, " \n");
			char* indexS = p;
			int index = atoi(indexS);
			ChangeTab(brws, index);
		}
		if(strcmp(p, "print_open_tabs") == 0) {
			TLista tabs = brws -> tabs;
			PrintOpenTabs(tabs, DispTab, out_file);
		}
		if(strcmp(p, "back") == 0) {
			Back(brws, out_file);
		}
		if(strcmp(p, "forward") == 0) {
			Forward(brws, out_file);
		}
		if(strcmp(p, "history") == 0) {
			AfisQ(brws->historyQ, afisChar, out_file);
		}
		if(strncmp(p, "del_history", 11) == 0) {
			p = strtok(NULL, " \n");
			char* entriesS = p;
			int entries = atoi(entriesS);
			DelHistory(brws -> historyQ, entries);
		}
		if(strcmp(p, "list_dl") == 0) {
			TLista tabCell = brws -> current_tab;
			Tab* tab = (Tab*)tabCell -> info;
			TWebPage* page = tab -> current_page;
			AfisDownloads(page, out_file);
		}
		if(strcmp(p, "downloads") == 0) {
			DispDownHistory(brws -> priorDownQ, out_file);
			if(ICQ(brws -> finishDownQ)) {
				DispCompletedDown(brws -> finishDownQ, out_file);
			}
		}
		if(strncmp(p, "download", 8) == 0 && strcmp(p, "downloads") != 0) {
			char* indexS = p + 9;
			int index = atoi(indexS);
			DownloadRes(brws, index, out_file);
		}
		if(strncmp(p, "wait", 4) == 0) {
			char* secondsS = p + 5;
			brws -> time = atol(secondsS);	
			unsigned long load = brws -> time * brws -> bandwidth;
			Wait(brws -> priorDownQ, brws -> finishDownQ, load);
		}
		p = strtok(NULL, " \n");
	}
	fclose(out_file);
	return 0;
}