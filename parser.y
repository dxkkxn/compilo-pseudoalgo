%{
  #include <stdio.h>
  #include <ctype.h>
  #include <unistd.h>
  
  #include "asa.h"
  #include "ts.h"

  extern int yylex();
  
%}

%union{
  int nb;
  struct asa * noeud;
 };

%define parse.error verbose

%token SAUT_LIGNE "\n"
%token ALGO "ALGO"
%token ENTREE "ENTREE"
%token DEBUT "DEBUT"
%token FIN "FIN"
%token SI "SI"
%token SINON "SINON"
%token ALORS "ALORS"
%token FSI "FSI"
%token VAR "VAR"
%token TQ "TQ"
%token FAIRE "FAIRE"
%token FTQ "FTQ"
%token AFFECT "<-"
%token SUPEGAL ">="
%token INFEGAL "<="
%token DIFF "!="
%token OU "OU"
%token ET "ET"
%token NON "NON"

%token ID
%token <nb> NB




%type <noeud> EXP
%type <noeud> INSTS

%right AFFECT
%left OU
%left ET
%left "<" ">" "=" SUPEGAL INFEGAL DIFF
%left '+' '-'
%left '*' '/' '%'
%right NON
%start PROG

%%

//       ENTREE LIST_VAR SAUT_LIGNE
PROG : ALGO ID SAUT_LIGNE DEBUT INSTS FIN{ /*codegen($5);*/printf("OK\n");} ;

INSTS : INSTS INST {}
      | INST  {}
      ;

EXP : NB      { $$ = creer_feuilleNb(yylval.nb); }
    | '(' EXP ')' { printf("( EXP )");}
    | EXP OU EXP {printf("OU");}
    | EXP ET EXP {printf("ET");}
    | NON EXP {printf("NON");}
    | EXP '*' EXP {printf("*");}
    | EXP '/' EXP {printf("/");}
    | EXP '%' EXP {printf("%%");}
    | EXP '-' EXP {printf("-");}
    | EXP '+' EXP {printf("-");} //$$ = creer_noeudOp('+', $1, $3); }
    ;
INST : SAUT_LIGNE {printf("saut_ligne");}
     | VAR ID {printf("var id saut_ligne");}
     | SI EXP ALORS INSTS {printf("Si exp alors insts");}
     | SI EXP ALORS INSTS SINON INSTS {printf("si exp alors sinon insts");}
     | TQ EXP FAIRE INSTS FTQ {printf("tq exp faire sinon insts ftq");}
     ;
%%
int main( int argc, char * argv[] ) {

  extern FILE *yyin;
  if (argc == 1){
    fprintf(stderr, "aucun fichier fournie\n");
    return 1;
  }
  yyin = fopen(argv[1],"r");
  yyparse();
  return 0;
}

