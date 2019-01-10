/* 
* Lexer.h
*
* Created: 12/14/2018 8:49:13 PM
* Author: z
*/

#include "main.h"
#include "Value.h"

#define NUM_VARS 10
#define MAX_VARNAME_LEN 8

#ifndef __LEXER_H__
#define __LEXER_H__


class Lexer
{
//variables
public:
    bool repl_mode;
	char vars[NUM_VARS][MAX_VARNAME_LEN];  // array to hold variable names
	Value vals[NUM_VARS]; // array to hold variable values
	int var_ptr;  // pointer to next available variable location
	const char* text;
	int pos;
	char current_char;
	void error();
	void advance();
	char peek();
	void skip_whitespace();
	Token parse_number();
	Token parse_hex();
	Token parse_string();
	Token _id();
	Token get_next_token();
	void eat(TokenType tokType);
    void execute_statement(const char* line); // used for REPL mode
	void program();
	void statement_list();
	void statement();
	void assignment_statement();
	Value function_call();
	void for_statement();
	void while_statement();
	void if_statement();
	void gosub_statement();
	Value expr();
	Value term();
	Value factor();
	Token current_token;
	Value lookup_var(const char* name);
	bool store_var(const char* name, Value v);
protected:
private:

//functions
public:
    Lexer();
	Lexer(const char* txt);
	~Lexer();
protected:
private:
	Lexer( const Lexer &c );
	Lexer& operator=( const Lexer &c );

}; //Lexer

int nocase_cmp(char* str, const char* str2);

#endif //__LEXER_H__
