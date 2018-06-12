#include <lpc21xx.h>
#include "led.h"

void Delay(unsigned int uiMiliSec) {
	unsigned int uiLoopCtr, uiDelayLoopCount;
	uiDelayLoopCount = uiMiliSec*12000;
	for(uiLoopCtr = 0; uiLoopCtr < uiDelayLoopCount; uiLoopCtr++) {}
}

//Led_0=1Hz Led_1=4Hz
int main( void ){
	LedInit();
	while(1){
		LedToggle(0);
		LedToggle(1);
		Delay(500);
		LedToggle(0);
		Delay(500);
		LedToggle(0);
		Delay(500);
		LedToggle(0);
		Delay(500);
	}
}

//Led_0=3Hz Led_1=4Hz
/*int main( void ){
	LedInit();
	while(1){
		LedToggle(0);
		LedToggle(1);
		Delay(500);
	}
}
*/

/*int main( void ){
	LedInit();
	while(1){
		LedToggle(0);
		Delay(500);
	}
}
*/
