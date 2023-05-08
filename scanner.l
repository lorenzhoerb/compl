%{
#include <stdio.h>
#include "oxout.tab.h"
int transformNum(char *num, unsigned int base);
void lexicalError(void);
%}

%option yylineno

WHITESPACE      [\n\t ]
COMMENT         ^\*.*\*$
ID              [a-zA-Z][a-zA-Z0-9_]*
DIGIT           [0-9]
DIGITS          {DIGIT}({DIGIT}|_)*
HEX_DIGIT       [a-fA-F0-9]
HEX             0x({HEX_DIGIT}|_)*{HEX_DIGIT}({HEX_DIGIT}|_)*

%%
","         return (',');
";"         return (';');
"("         {return LEFT_PAREN;}
")"         {return RIGHT_PAREN;}
"<-"        {return ASSIGN;}
"->"        {return ARROW;}
"-"         {return MINUS;}
"+"         {return PLUS;}
"*"         {return MULTIPLY;}
">"         {return GREATER_THAN;}
"#"         {return HASH;}

object      {return OBJECT;}
int         {return INT;}
class       {return CLASS;}
end         {return END;}
return      {return RETURN;}
cond        {return COND;}
continue    {return CONTINUE;}
break       {return BREAK;}
not         {return NOT;}
null        {return NULLKEY;}
new         {return NEW;}
or          {return OR;}


{HEX}       return (NUM); @{ @NUM.n@ = transformNum(yytext, 16); @}
{DIGITS}    return (NUM); @{ @NUM.n@ = transformNum(yytext, 10); @}
{ID}        return (ID); @{ @ID.id@ = strdup(yytext); @ID.lineNr@ = yylineno; @}
{COMMENT}       ;
{WHITESPACE}    ;
.		    lexicalError();
%%

int yywrap(){}

int transformNum(char *num, unsigned int base) {
    int i, j;
    for(i = 0, j = 0; i < strlen(num); i++) {
        if(num[i] != '_') {
            num[j++] = num[i];
        }
    }
    num[j] = '\0';

    char *endptr;
    int inum = strtol(num, &endptr, base);

    return inum;
}

void lexicalError(void) {
    fprintf(stderr, "Syntax error on line: %d\nUnexpected character(s): %s\n"
    , yylineno, yytext);
    exit(1);
}