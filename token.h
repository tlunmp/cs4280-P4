#ifndef TOKEN_H
#define TOKEN_H


typedef enum {
	IDENTIFIER,
	KEYWORD,
	NUMBER,	
	EQUAL,
	GREATER_THAN,
	LESS_THAN,
	COLON,
	PLUS,
	MINUS,
	STAR,
	SLASH,
	PERCENT,
	DOT,
	LEFT_PARENT,
	RIGHT_PARENT,
	COMMA,
	LEFT_BRACE,
	RIGHT_BRACE,
	SEMICOLON,
	LEFT_BRACKETS,
	RIGHT_BRACKETS,
	
	//keywords
	START_KEYWORD,
	STOP_KEYWORD,
	LOOP_KEYWORD,
	VOID_KEYWORD,
	VAR_KEYWORD,
	RETURN_KEYWORD,
	SCAN_KEYWORD,
	OUT_KEYWORD,
	PROGRAM_KEYWORD,
	IF_KEYWORD,
	THEN_KEYWORD,
	LET_KEYWORD,
	EOT // end of token
} TokenType;

typedef struct {
	TokenType tokenType;
	char instance[100];
	int lineNum;
} Token;

typedef struct {
	TokenType tokenType;
	char instance[100];
	int lineNum;
} Tk;


Token* tokens;
#endif
