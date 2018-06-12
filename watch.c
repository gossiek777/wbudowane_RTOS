#include "watch.h"

void Watch_Init(){
}

struct WatchEvent sWatch_Read(void){
}



/*
struct Watch {

	unsigned char ucMinutes, ucSecconds;
	unsigned char fSeccondsValueChanged, fMinutesValueChanged;
};

struct Watch sWatch;

void WatchUpdate(){
	
	sWatch.ucSecconds++;
	sWatch.fSeccondsValueChanged=1;
	if(sWatch.ucSecconds ==3) {
		sWatch.ucSecconds=0;
		sWatch.ucMinutes++;
		sWatch.fMinutesValueChanged=1;
	}
	if(sWatch.ucMinutes == 60){
		sWatch.ucMinutes=0;
	}
}
*/