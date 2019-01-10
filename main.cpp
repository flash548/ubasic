/*
 * cc.cpp
 *
 * Created: 12/14/2018 3:03:59 PM
 * Author : z
 */ 
using namespace std;

 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Value.h"
#include "Lexer.h"

bool repl = false;

int main(void)
{
#ifdef PC_ONLY
	char buffer[1000];
	FILE* fp;
	int lSize;
	fp = fopen ("testpc.bas" , "rb");
	fseek(fp , 0L , SEEK_END);
	lSize = ftell(fp);
	rewind(fp);
	fread(buffer , lSize, 1 , fp);
	fclose(fp);
#else
	const char* buffer = "a=0\nDIM ab(5)\nfor i=0 to 4\nprint(i)\nnext i\nEND";
#endif
    char cmd[256];
    if (repl) {
        Lexer l;
        while (true) {
            printf(">> ");
            gets(cmd);
            l.execute_statement(cmd);
        }
    }
    else {
        Lexer l(buffer);    
        l.program();
    }
	
	return 0;
}

