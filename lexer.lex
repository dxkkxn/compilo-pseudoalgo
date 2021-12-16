%{
#include <string.h>
#include "parser.h" 
  
%}
  
%option nounput
%option noinput
%option yylineno

CHIFFRE  [0-9]
OP_ARITH [-+*/%\(\)]
PAREN [\(\)]
ID [A-Za-z_][A-Za-z0-9_]*
NUM ([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-6])
FILTER [ \t]
COMMEN (#.*\n)


%%
{FILTER}  {}
{COMMEN}  {}
"\n"      {return SAUT_LIGNE;}
"ALGO"    {return ALGO;}
"ENTREE"  {return ENTREE;}
"DEBUT"   {return DEBUT;}
"FIN"     {return FIN;}
"SI"      {return SI;}
"ALORS"   {return ALORS;}
"SINON"   {return SINON;}
"FSI"     {return FSI;}
"VAR"     {return VAR;}
"TQ"      {return TQ;}
"FAIRE"   {return FAIRE;}
"FTQ"     {return FTQ;}
"!="      {return DIFF;}
">="      {return SUPEGAL;}
"<="      {return INFEGAL;}
"="       {return yytext[0];}
"<"       {return yytext[0];}
">"       {return yytext[0];}
"<-"      {return AFFECT;}
"OU"      {return OU;}
"ET"      {return ET;}
"NON"     {return NON;}
"AFFICHER" {return AFFICHER;}
{ID}      {return ID;}
{OP_ARITH}  {return yytext[0];}

{NUM} {yylval.nb = atoi(yytext); return NB;}
.    {fprintf(stderr, "[err lexer] caractere inconnu %c %d\n",yytext[0],yytext[0]);
      return 1;}

%%
