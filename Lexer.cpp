/* 
* Lexer.cpp
*
* Created: 12/14/2018 8:49:13 PM
* Author: z
*/


#include "Lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

// default constructor
Lexer::Lexer()
{
    repl_mode = true;
    var_ptr = 0;
}
Lexer::Lexer(const char* txt)
{
    repl_mode = false;
	text = txt;
	pos = 0;
	current_char = text[pos];
	current_token = get_next_token();
	var_ptr = 0;
} //Lexer

// default destructor
Lexer::~Lexer()
{
} //~Lexer

void Lexer::error()
{
	printf("ERROR!!! Invalid char\n");
	printf("%s\n", current_token.value.ToString());
	if (!repl_mode) while(1){}
}

void Lexer::advance()
{
	pos += 1;
	if (pos > strlen(text) - 1)
	{
		current_char = '\0';
	}
	else
	{
		current_char = text[pos];
	}
}

char Lexer::peek()
{
	if (pos + 1 < strlen(text))
	{
		char c = text[pos + 1];
		return text[pos + 1];
	}
	else
	{
		return '\0';
	}
}

void Lexer::skip_whitespace()
{
	while (current_char != '\0' && isspace(current_char))
	{
		advance();
	}
}

Token Lexer::parse_number()
{
	char result[20];
	int i = 0;
	while (current_char != '\0' && isdigit(current_char))
	{
		result[i++] = current_char;
		advance();
	}
    if (current_char == '.')
    {
        result[i++] = current_char;
        advance();
        while (current_char != '\0' && isdigit(current_char))
        {
            result[i++] = current_char;
            advance();
        }
        
        result[i++] = '\0';
        Token t;
        t.type = FLOATNUM;
        t.value = Value(atof(result));
        return t;
    }
    
	result[i++] = '\0';
	Token t;
	t.type = INT;
	t.value = Value(atoi(result));

	return t;
}

Token Lexer::parse_hex()
{
	char result[20];
	int i = 0;
	while (current_char != '\0' && (isdigit(current_char) ||
			current_char == 'A' || current_char == 'B' ||
			current_char == 'C' || current_char == 'D' ||
			current_char == 'E' || current_char  == 'F'))
	{
		result[i++] = current_char;
		advance();
	}
	result[i++] = '\0';
	Token t;
	t.type = INT;
	t.value = Value((int)strtol(result, NULL, 16));
	return t;

}

Token Lexer::parse_string()
{
	char result[MAXSTRLENGTH];
	int i = 0;
	while (current_char != '\0')
	{
		if (current_char == '"' && peek() != '"') break;  // end of string
		else if (current_char == '"' && peek() == '"') advance();  // deal with double-quote literals
		result[i++] = current_char;
		advance();
	}

	advance();
	result[i++] = '\0';
	Token t;
	t.type = STR;
	t.value = Value(result);
	return t;
}


int nocase_cmp(char* str, const char* str2)
{
	int i = 0;
	while(str2[i] != '\0') {
		if (toupper(str[i]) != toupper(str2[i])) return 1;
		i++;
	}
	
	return 0;
}

Token Lexer::_id()
{
	char name[MAXSTRLENGTH];
	int i = 0;
	Token t;
	while (current_char != '\0' && isalnum(current_char))
	{
		if (isalpha(current_char)) current_char = toupper(current_char);
		name[i++] = current_char;
		advance();
	}
	
	// detect labels and return
	if (current_char == ':') { 
		advance();
		name[i++] = '\0';
		t.type = LABEL;
		t.value = Value(name);
		return t;
	}

	name[i++] = '\0';

	if (nocase_cmp(name, "PRINT") == 0) { t.type = FUNC_CALL; t.value = Value(name); }
	else if (nocase_cmp(name, "DELAY") == 0) { t.type = FUNC_CALL;	t.value = Value(name); }
	else if (nocase_cmp(name, "PORTC") == 0) { t.type = FUNC_CALL;	t.value = Value(name); }
	else if (nocase_cmp(name, "TRUE") == 0) { t.type = BOOL; t.value = Value(true); }
	else if (nocase_cmp(name, "FALSE") == 0) { t.type = BOOL; t.value = Value(false); }
	else if (nocase_cmp(name, "DIM") == 0) { t.type = DIM; t.value = Value("DIM"); }
	else if (nocase_cmp(name, "LET") == 0) { t.type = LET; t.value = Value("LET"); }
	else if (nocase_cmp(name, "NOT") == 0) { t.type = NOT; t.value = Value("NOT"); }
	else if (nocase_cmp(name, "AND") == 0) { t.type = AND; t.value = Value("AND"); }
	else if (nocase_cmp(name, "OR") == 0) { t.type = OR; t.value = Value("OR"); }
	else if (nocase_cmp(name, "XOR") == 0) { t.type = NOT; t.value = Value("XOR"); }
	else if (nocase_cmp(name, "IF") == 0) { t.type = IF; t.value = Value("IF"); }
	else if (nocase_cmp(name, "THEN") == 0) { t.type = THEN; t.value = Value("THEN"); }
	else if (nocase_cmp(name, "WHILE") == 0) { t.type = WHILE; t.value = Value("WHILE"); }
	else if (nocase_cmp(name, "WEND") == 0) { t.type = WEND; t.value = Value("WEND"); }
	else if (nocase_cmp(name, "RETURN") == 0) { t.type = RETURN; t.value = Value("RETURN"); }
	else if (nocase_cmp(name, "GOSUB") == 0) { t.type = GOSUB; t.value = Value("GOSUB"); }
	else if (nocase_cmp(name, "ELSE") == 0) { t.type = ELSE; t.value = Value("ELSE"); }
	else if (nocase_cmp(name, "FOR") == 0) { t.type = FOR; t.value = Value("FOR"); }
    else if (nocase_cmp(name, "AS") == 0) { t.type = AS; t.value = Value("AS"); }
    else if (nocase_cmp(name, "TO") == 0) { t.type = TO; t.value = Value("TO"); }
	else if (nocase_cmp(name, "STEP") == 0) { t.type = STEP; t.value = Value("STEP"); }
	else if (nocase_cmp(name, "NEXT") == 0) { t.type = NEXT; t.value = Value("NEXT"); }
	else if (nocase_cmp(name, "END") == 0) {t.type = END; t.value = Value("END"); }
	else
	{
		// return as a variable name token (value is the name of the var)
		t.type = ID;
		t.value = Value(name);
	}

	return t;

}

Token Lexer::get_next_token()
{
	Token t;

	while (current_char != '\0')
	{
		if (isspace(current_char))
		{
			// check for Unix and Windows line endings
			if (current_char == '\r' || current_char == '\n')
			{
				if (current_char == '\r' && peek() == '\n')
				{
					advance();
				}

				advance();
				// Token t;
				t.type = NEWLINE;
				t.value = Value('\n');
				return t;
			}

			skip_whitespace();
			continue;
		}

		if (isdigit(current_char))
		{
			return parse_number();
		}

		if (current_char == '"')
		{
			advance();
			return parse_string();
		}

		if (isalnum(current_char))
		{
			return _id();
		}

		if (current_char == '&' && peek() == 'H')
		{
			advance();
			advance();
			return parse_hex();
		}

		if (current_char == '&')
		{
			advance();
			// Token t;
			t.type = STR_CONCAT;
			t.value = Value('&');
			return t;
		}

		if (current_char == '=')
		{
			advance();
			// Token t;
			t.type = EQ;
			t.value = Value('=');
			return t;
		}

		if (current_char == ',')
		{
			advance();
			// Token t;
			t.type = COMMA;
			t.value = Value(',');
			return t;
		}

		if (current_char == '%')
		{
			advance();
			// Token t;
			t.type = MOD;
			t.value = Value('%');
			return t;
		}

		if (current_char == ':')
		{
			advance();
			// Token t;
			t.type = COLON;
			t.value = Value(':');
			return t;
		}

		if (current_char == '<')
		{
			if (peek() == '>')
			{
				advance();
				advance();
				// Token t;
				t.type = NEQ;
				t.value = Value("<>");
				return t;
			}
			else if (peek() == '=')
			{
				advance();
				advance();
				// Token t;
				t.type = LTE;
				t.value = Value("<=");
				return t;
			}
			else if (peek() == '<')
			{
				advance();
				advance();
				// Token t;
				t.type = LSHIFT;
				t.value = Value("<<");
				return t;
			}

			advance();
			// Token t;
			t.type = LT;
			t.value = Value('<');
			return t;
		}

		if (current_char == '>')
		{
			if (peek() == '=')
			{
				advance();
				advance();
				// Token t;
				t.type = GTE;
				t.value = Value(">=");
				return t;
			}
			else if (peek() == '>')
			{
				advance();
				advance();
				// Token t;
				t.type = RSHIFT;
				t.value = Value(">>");
				return t;
			}

			advance();
			// Token t;
			t.type = GT;
			t.value = Value('<');
			return t;
		}

		if (current_char == '+')
		{
			advance();
			if (current_char == '=')
			{
				advance();
				// Token t;
				t.type = INCR;
				t.value = Value("+=");
				return t;
			}
			// Token t;
			t.type = PLUS;
			t.value = Value("+");
			return t;
		}

		if (current_char == '-')
		{
			advance();
			if (current_char == '=')
			{
				advance();
				// Token t;
				t.type = DECR;
				t.value = Value("-=");
				return t;
			}
			// Token t;
			t.type = MINUS;
			t.value = Value('-');
			return t;
		}

		if (current_char == '^')
		{
			advance();
			// Token t;
			t.type = POW;
			t.value = Value('^');
			return t;
		}

		if (current_char == '*')
		{
			advance();
			// Token t;
			t.type = MUL;
			t.value = Value("*");
			return t;
		}

		if (current_char == '/')
		{
			advance();
			// Token t;
			t.type = DIV;
			t.value = Value('/');
			return t;
			
		}

		if (current_char == '\\')
		{
			advance();
			// Token t;
			t.type = INTDIV;
			t.value = Value('\\');
			return t;
		}

		if (current_char == '(')
		{
			advance();
			// Token t;
			t.type = LPAREN;
			t.value = Value('(');
			return t;
		}

		if (current_char == ')')
		{
			advance();
			// Token t;
			t.type = RPAREN;
			t.value = Value(')');
			return t;
		}

		error();
	}

	t.type = END;
	t.value = Value('\0');
	return t;

}

void Lexer::eat(TokenType tokType)
{
	if (current_token.type == tokType)
	{
		current_token = get_next_token();
	}
	else { error(); }
}

void Lexer::execute_statement(const char* line)
{
    text = line;
	pos = 0;
	current_char = text[pos];
	current_token = get_next_token();
    statement();
}

// program: statement_list
void Lexer::program()
{
	statement_list();
}

 // statement: function_call | if_statement | assignment_statement | empty
void Lexer::statement_list()
{
	statement();
	while (current_token.type != END)
	{
		if (current_token.type == NEWLINE)
		{
			eat(NEWLINE);
		}
		statement();
	}
}

// statement: function_call | assignment_statement | empty
void Lexer::statement()
{
	if (current_token.type == ID || current_token.type == DIM)
		assignment_statement();
	else if (current_token.type == FUNC_CALL)
		function_call();
	else if (current_token.type == WHILE)
		while_statement();
	else if (current_token.type == FOR)
		for_statement();
	else if (current_token.type == IF)
		if_statement();
	else if (current_token.type == GOSUB)
		gosub_statement();
    
}

// statement: function_call | assignment_statement | empty
void Lexer::assignment_statement()
{
    char varname[20];
	bool newArrayOp = false;
	if (current_token.type == DIM) {
		newArrayOp = true;
		eat(DIM);
	}
	strcpy(varname, current_token.value.ToString());
	eat(ID);
    if (!newArrayOp) {
        if (current_token.type == LPAREN) {
            // array element assignment
            eat(LPAREN);
            int index = expr().number;
            eat(RPAREN);
            eat(EQ);
            Value parentValue = lookup_var(varname);
            Value result = expr();
            if (result.type == INTEGER) parentValue.update_array(index, result.number);
            else if (result.type == FLOAT) parentValue.update_array(index, result.floatNumber);
        }
        else {
            // regular old variable assignment (non-array)
            if (current_token.type == EQ) {
                eat(EQ);
                Value right(expr());
                store_var(varname, right);	
            }
            else {
                // just eval the ID
                pos = 0;
                current_char = text[pos];
                current_token = get_next_token();
                Value right(expr());
                if (repl_mode) printf("%s\n", right.ToString()); 
            }
        }
    }
    else {
        // DIM a new array of given size
        eat(LPAREN);
        int size = expr().number;
        eat(RPAREN);
        eat(AS);
        char type[10];
        strcpy(type, current_token.value.ToString());
        if (nocase_cmp(type, "INTEGER") == 0) { Value right(INTEGER, size); store_var(varname, right); }
        else {
            Value right(FLOAT, size); 
            store_var(varname, right);
        }
        eat(ID);
    }
}

// gosub_statement: GOSUB label
void Lexer::gosub_statement()
{
	eat(GOSUB);
	char subname[20];
	strcpy(subname, current_token.value.ToString());
	int tempPos = pos;
	eat(ID);	
	bool foundLabel = false;
	while (!foundLabel) {
		current_token = get_next_token();
		if (current_token.type == LABEL && strcmp(subname, current_token.value.ToString()) == 0) {
			// found the sub
			eat(LABEL);
			foundLabel = true;
			while (current_token.type != RETURN) {
				statement();
				eat(NEWLINE);
			}
			eat(RETURN);
			
			// return to the caller..
			pos = tempPos;
			current_char = text[pos];
			current_token = get_next_token();
		}	
	}
	//eat(NEWLINE);
}

// if_statement: IF expr THEN statement
void Lexer::if_statement()
{
	eat(IF);
	if (expr().ToBoolean()) {
		printf("in here\n");
		eat(THEN);
		statement();
		eat(NEWLINE);
		if (current_token.type == ELSE) {
			// if there was an else clause, eat it up...
			eat(ELSE);
			while (current_token.type != NEWLINE) current_token = get_next_token();
			eat(NEWLINE);
		}
	}
	else {
		eat(THEN);
		while (current_token.type != NEWLINE) current_token = get_next_token();
		eat(NEWLINE);
		if (current_token.type == ELSE) {
			eat(ELSE);
			statement();
			eat(NEWLINE);
		}
		else {
			// no else was given...
			while (current_token.type != NEWLINE) current_token = get_next_token();
			eat(NEWLINE);
		}
	}
}

// statement: whiile_statement LPAREN expr RPAREN NEWLINE statement_list() NEWLINE WEND
void Lexer::while_statement()
{
	eat(WHILE);
	int tempPos = pos-1;
	eat(LPAREN);
	while (expr().ToBoolean()) {
		eat(RPAREN); eat(NEWLINE);
		while (current_token.type != WEND) {
			statement();
			eat(NEWLINE);
		}
		// return back to the expression
		pos = tempPos;
		current_char = text[pos];
		current_token = get_next_token();
		eat(LPAREN);
	}
	if (current_token.type == RPAREN) {
		eat(RPAREN);
		int while_count = 1;
		while (while_count > 0)
		{
			// go thru block until we hit our WEND..
			//  skip other WHILE constructs if needed
			current_token = get_next_token();
			if (current_token.type == WHILE) while_count++;
			if (current_token.type == WEND) while_count--;
		}
		eat(WEND);
	}

}

// statement: for_statement assignment_statement TO expr STEP expr NEWLINE statement_list() NEWLINE NEXT ID
void Lexer::for_statement()
{
	int tempPos = pos+3;
	eat(FOR);
	char varname[20];
	strcpy(varname, current_token.value.ToString());
	assignment_statement();
	eat(TO);
	while (lookup_var(varname).number <= expr().number) {
		int incrVal = 1;
		if (current_token.type == STEP) {
			eat(STEP); 
			Value v(expr());
			incrVal = v.number;
		}
		while (current_token.type != NEXT) {
			statement();
			eat(NEWLINE);
		}
		
		store_var(varname, lookup_var(varname).number + incrVal);
		// return back to the expression
		pos = tempPos;
		current_char = text[pos];
		do {
			current_token = get_next_token();
		}
		while (current_token.type != TO);
		eat(TO);
	}
	int for_count = 1;
	while (for_count > 0)
	{
		// go thru block until we hit our NEXT..
		//  skip other FOR constructs if needed
		current_token = get_next_token();
		if (current_token.type == FOR) for_count++;
		if (current_token.type == NEXT) for_count--;
	}
	eat(NEXT);
	if (current_token.type == ID) eat(ID);
}

// statement: function_call | assignment_statement | empty
Value Lexer::function_call()
{
	char funcname[20];
	strcpy(funcname, current_token.value.ToString());
	eat(FUNC_CALL);
	eat(LPAREN);
	if (strcmp(funcname, "PRINT") == 0) {
		// print to CONSOLE if on PC
		Value right(expr());
        eat(RPAREN);
#ifdef PC_ONLY
		printf("%s\n", right.ToString());
#else
		lcd_printf(right.ToString());
#endif
		return right;
	}
	else if (strcmp(funcname, "DELAY") == 0) {
		// delay in mS
		Value right(expr());
		eat(RPAREN);
#ifdef PC_ONLY
		usleep(right.number*1000);
#else
		_delay_ms(right.number);
#endif
		return right;
	}
	else if (strcmp(funcname, "PORTC") == 0) {
		// delay in mS
		Value right(expr());
		eat(RPAREN);
		printf("%i\n", right.number);
		return right;
	}
	
	Value v(0);
	return v;
}

/*
expr: term ((PLUS | MINUS | STR_CONCAT | LSHIFT | RSHIFT) term)*
*/
Value Lexer::expr()
{
	Value result = term();

	while (current_token.type == PLUS ||
			current_token.type == MINUS ||
			current_token.type == STR_CONCAT ||
			current_token.type == LSHIFT ||
			current_token.type == RSHIFT)
	{
		Token token = current_token;
		if (token.type == PLUS)
		{
			eat(PLUS);
			result = (result + term());
		}
		else if (token.type == MINUS)
		{
			eat(MINUS);
			result = (result - term());
		}
		else if (token.type == STR_CONCAT)
		{
			eat(STR_CONCAT);
			result = (result & term());
		}
		else if (token.type == LSHIFT)
		{
			eat(LSHIFT);
			result = (result << term());
		}
		else if (token.type == RSHIFT)
		{
			eat(RSHIFT);
			result = (result >> term());
		}
	}

	return result;
}

// term : factor ((POW | MUL | DIV | INTDIV |
//              MOD | EQ | NEQ | 
//              LT | LTE | GT | GTE |
//              AND | OR | XOR) factor)*
Value Lexer::term()
{
	Value result = factor();

	while (
			current_token.type == MUL ||
			current_token.type == DIV ||			
			current_token.type == AND ||
			current_token.type == OR ||
			current_token.type == XOR ||
			current_token.type == GT ||
			current_token.type == GTE ||
			current_token.type == LT ||
			current_token.type == LTE ||
			current_token.type == MOD ||
			current_token.type == NEQ ||
			current_token.type == EQ
		  )
	{
		Token token = current_token;
		// if (token.type == POW)
		// {
			// eat(POW);
			// result = result.pow(factor());
		// }
		if (token.type == MUL)
		{
			eat(MUL);
			result = result * factor();
		}
		else if (token.type == DIV)
		{
			eat(DIV);
			result = result / factor();
		}
		// else if (token.type == INTDIV)
		// {
			// eat(INTDIV);
			// result = result.intDiv(factor());
		// }
		else if (token.type == MOD)
		{
			eat(MOD);
			result = result % factor();
		}
		else if (token.type == EQ)
		{
			eat(EQ);
			result = (result == factor());
		}
		else if (token.type == NEQ)
		{
			eat(NEQ);
			result = (result != factor());
		}
		else if (token.type == LT)
		{
			eat(LT);
			result = (result < factor());
		}
		else if (token.type == LTE)
		{
			eat(LTE);
			result = (result <= factor());
		}
		else if (token.type == GT)
		{
			eat(GT);
			result = (result > factor());
		}
		else if (token.type == GTE)
		{
			eat(GTE);
			result = (result >= factor());
		}
		else if (token.type == AND)
		{
			eat(AND);
			result = (result & factor());
		}
		else if (token.type == OR)
		{
			eat(OR);
			result = (result | factor());
		}
		else if (token.type == XOR)
		{
			eat(XOR);
			result = (result ^ factor());
		}
	}

	return result;
}

// factor : (PLUS | MINUS) factor | INTEGER | LPAREN expr RPAREN | (NOT) BOOLEAN | STRING | ID
Value Lexer::factor()
{
	Token token = current_token;
	if (token.type == PLUS)
	{
		eat(PLUS);
		return +factor();
	}
	else if (token.type == MINUS)
	{
		eat(MINUS);
		return -factor();
	}
	else if (token.type == NOT)
	{
		eat(NOT);
		return !factor();

	}
	else if (token.type == BOOL)
	{
		eat(BOOL);
		return token.value;
	}
	else if (token.type == STR)
	{
		eat(STR);
		return token.value;
	}
	else if (token.type == ID)
	{
		eat(ID);
        if (current_token.type == LPAREN) {
            eat(LPAREN);
            // we have an array element here, look ip up
            int index = expr().number;
            eat(RPAREN);
            Value parentVal = lookup_var(token.value.ToString());
            Value v = parentVal.index_array(index);
            return v;
        }
        else {
            Value v = lookup_var(token.value.ToString());
            return v;
        }
	}
	else if (token.type == FUNC_CALL)
	{
		return function_call();
	}
	else if (token.type == INT)
	{
		eat(INT);
		return token.value;
	}
	else if (token.type == FLOATNUM)
	{
		eat(FLOATNUM);
		return token.value;
	}
	else if (token.type == LPAREN)
	{
		eat(LPAREN);
		Value result = expr();
		eat(RPAREN);
		return result;
	}

	error();

	Value v(0);
	return v;
}

Value Lexer::lookup_var(const char* name)
{
	int i = 0;
	for (i=0;i<NUM_VARS;i++)
	{
		if (strcmp(vars[i], name) == 0) break;
	}
	
	if (i >= NUM_VARS) {
		printf("Var not found! %s\n", name); 
        if (!repl_mode) while(1);
	}
	
	return vals[i];
}

bool Lexer::store_var(const char* name, Value v)
{
	int i = 0;
	for (i=0;i<NUM_VARS;i++)
	{
		// var name found, update the existing value
		if (strcmp(vars[i], name)== 0) {
			vals[i] = v;
			return true;
		}
	}
	
	// didn't find var name, so add it to end of array
	if (i >= NUM_VARS) {
		strcpy(vars[var_ptr], name);
		vals[var_ptr++] = v;
		return true;
	}
	
	if (var_ptr >= NUM_VARS) {
		printf("number of vars exceeded\n");
		if (!repl_mode) while(1) {}
	}
		
	return false;
}