#define MAX_TOKEN_NR 3 
#define MAX_KEYWORD_STRING_LTH 20 
#define MAX_KEYWORD_NR 5

enum TokenType {KEYWORD, NUMBER, STRING};
enum KeywordCode {ID, CALLIB, GOTO, BUTTON, STATE};


union TokenValue {
	enum KeywordCode eKeyword;
	unsigned int uiNumber;
	char *pcString;
};

struct Token {
	enum TokenType eType; 
	union TokenValue uValue;
};


struct Keyword{
	enum KeywordCode eCode;
	char cString[MAX_KEYWORD_STRING_LTH + 1]; 
};



extern struct Keyword asKeywordList[MAX_KEYWORD_NR];
extern struct Token asToken[MAX_TOKEN_NR];
extern unsigned char ucTokenNr;


void DecodeMsg(char *pcString);
