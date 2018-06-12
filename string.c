#include "string.h"

void CopyString(char pcSource[], char pcDestination[]){
	
	unsigned char ucCharacterCounter;
	
	for (ucCharacterCounter=0; (pcSource[ucCharacterCounter]!=NULL); ucCharacterCounter++){
		pcDestination[ucCharacterCounter]=pcSource[ucCharacterCounter];
	}
	pcDestination[ucCharacterCounter]=pcSource[ucCharacterCounter];
}

enum CompResult eCompareString(char pcStr1[], char pcStr2[]){
	
	unsigned char ucCharacterCounter;
	
	for(ucCharacterCounter=0;(pcStr1[ucCharacterCounter]!=NULL)&&(pcStr2[ucCharacterCounter]!=NULL);ucCharacterCounter++){
		if(pcStr1[ucCharacterCounter]!=pcStr2[ucCharacterCounter]){
			return NOTEQUAL;
		}	
	}
	if(pcStr1[ucCharacterCounter]!=pcStr2[ucCharacterCounter]){
		return NOTEQUAL;
	}	
	return EQUAL;
}	

void AppendString (char pcSourceStr[], char pcDestinationStr[]){
	
	unsigned char ucCharacterCounter;
	
	for (ucCharacterCounter=0;pcDestinationStr[ucCharacterCounter]!=NULL;ucCharacterCounter++){}
	CopyString(pcSourceStr,pcDestinationStr+ucCharacterCounter);
}

void ReplaceCharactersInString(char pcString[], char cOldChar,char cNewChar){
	
	unsigned char ucCharacterCounter;
	
	for (ucCharacterCounter=0;pcString[ucCharacterCounter]!=NULL;ucCharacterCounter ++){
		if(pcString[ucCharacterCounter]==cOldChar){
			pcString[ucCharacterCounter]=cNewChar;
		}
	}	
}

void UIntToHexStr (unsigned int uiValue, char pcStr[]){
       
	unsigned char ucCurrentTetrad;
	unsigned char ucTetradCounter;
 
	pcStr[0] = '0';
	pcStr[1] = 'x';
	pcStr[6] = NULL;
 
	for(ucTetradCounter = 0; ucTetradCounter < 4; ucTetradCounter++){
		ucCurrentTetrad = (uiValue >> (ucTetradCounter*4)) & 0xF;
		if(ucCurrentTetrad < 10){
			pcStr[5 - ucTetradCounter] = '0' + ucCurrentTetrad;
		}
		else{
			pcStr[5 - ucTetradCounter] = 'A' + ucCurrentTetrad - 10;
		}
	}
}

enum Result eHexStringToUInt(char pcStr[],unsigned int *puiValue){
	
	char cCurrentSign;
	unsigned char ucSignsCounter;
	*puiValue=0;
	
	if((pcStr[0] != '0')||(pcStr[1] != 'x')||(pcStr[2] == NULL)){
		return (ERROR);
	}
	for(ucSignsCounter=2; pcStr[ucSignsCounter]!=NULL; ucSignsCounter++){
		cCurrentSign = pcStr[ucSignsCounter];
		if (ucSignsCounter>=6){
			return (ERROR);
		}
		*puiValue= *puiValue<<4;
		if((cCurrentSign>='0')&&(cCurrentSign<='9')){
			*puiValue= *puiValue + (cCurrentSign-'0');
		}
		else if((cCurrentSign>='A')&&(cCurrentSign<='F')){
			*puiValue= *puiValue + (cCurrentSign-'A'+10);		
		}
		else{
			return (ERROR);
		}
	}
	return (OK);
}

void AppendUIntToString (unsigned int uiValue, char pcDestinationStr[]){

	unsigned char ucSignsCounter;

	for (ucSignsCounter=0; pcDestinationStr[ucSignsCounter]!=NULL; ucSignsCounter++){}
	UIntToHexStr(uiValue, pcDestinationStr+ucSignsCounter);
}
