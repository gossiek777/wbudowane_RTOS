#include "string.h"
#include "command_decoder.h"

struct Keyword asKeywordList[MAX_KEYWORD_NR]={
		{CALLIB, "callib"},
		{GOTO, "goto"},
		{ID, "id"},
		{BUTTON, "button"},
		{STATE, "state"},
		{WAIT, "wait"},
		{SPEED, "speed"},
	};
struct Token asToken[MAX_TOKEN_NR];
unsigned char ucTokenNr;
	
unsigned char ucFindTokensInString(char *pcString){
	
	enum MsgState {TOKEN, DELIMITER};
	enum MsgState eState = DELIMITER;
	
	unsigned char ucTokenCounter = 0;
	unsigned char ucSignCounter = 0;
	char cCurrentSign;
	
	for(ucSignCounter=0; pcString[ucSignCounter] != NULL; ucSignCounter++){
		cCurrentSign = pcString[ucSignCounter];
		switch(eState){
			case DELIMITER: 	
				if(cCurrentSign == DELIMITER_CHAR){
					eState = DELIMITER;
				}
				else if(ucTokenCounter < MAX_TOKEN_NR){
					eState = TOKEN;
					asToken[ucTokenCounter].uValue.pcString = pcString + ucSignCounter; 
					ucTokenCounter++;
				}
				else{
					return(ucTokenCounter);
				}
				break;
			case TOKEN: 
				if(cCurrentSign!= DELIMITER_CHAR){
					eState = TOKEN;
				}
				else{
					eState = DELIMITER;
				}
				break;
		}	
	}
	return(ucTokenCounter);
}

enum Result eStringToKeyword(char pcStr[], enum KeywordCode *peKeywordCode){
	
	unsigned char ucKeywordIndex;
	
	for(ucKeywordIndex=0; ucKeywordIndex < MAX_KEYWORD_NR; ucKeywordIndex++){
		if(eCompareString(pcStr, asKeywordList[ucKeywordIndex].cString) == EQUAL){
			*peKeywordCode = asKeywordList[ucKeywordIndex].eCode;
			return(OK);
		}
	}
	return(ERROR);
}

void DecodeTokens(void){
	
	unsigned int uiNumber;
	unsigned char ucTokenIndex;
	enum KeywordCode eKeyword;
	
	for(ucTokenIndex=0; ucTokenIndex < ucTokenNr; ucTokenIndex++){
		if((eStringToKeyword (asToken[ucTokenIndex].uValue.pcString, &eKeyword))== OK){
			asToken[ucTokenIndex].eType = KEYWORD;
			asToken[ucTokenIndex].uValue.eKeyword=eKeyword;
		}
		else if((eHexStringToUInt(asToken[ucTokenIndex].uValue.pcString,&uiNumber))== OK){
			asToken[ucTokenIndex].eType = NUMBER;
			asToken[ucTokenIndex].uValue.uiNumber = uiNumber;		
		}
		else{
			asToken[ucTokenIndex].eType = STRING;
		}
	}
}

void DecodeMsg(char *pcString){

	ucTokenNr = ucFindTokensInString (pcString);
	ReplaceCharactersInString(pcString,' ',NULL);
	DecodeTokens();
}
