%{
  #include <stdio.h>
  #include <ctype.h>
  #include <unistd.h>
  
  #include "asa.h"
  #include "ts.h"

  extern int yylex();
  FILE * out_file;
  int tete ;
  int ligne_ram ;

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
%token AFFICHER "AFFICHER"
%token AFFECT "<-"
%token SUPEGAL ">="
%token INFEGAL "<="
%token DIFF "!="
%token OU "OU"
%token ET "ET"
%token NON "NON"
%token ID



%token <nb> NB
%type <noeud> EXP INST INSTS
//%type <noeud> INSTS

%right AFFECT
%left OU
%left ET
%left '<' '>' '=' SUPEGAL INFEGAL DIFF
%left '+' '-'
%left '*' '/' '%'
%right NON
%precedence OP_UN // operateur unaire
%start PROG

%%

PROG : ALGO ID SAUT_LIGNES
       ENTREE LIST_VAR SAUT_LIGNES
       DEBUT
       INSTS
       FIN SAUT_LIGNES ;

LIST_VAR    : LIST_VAR ID
            | ID
            ;

SAUT_LIGNES : %empty
            | SAUT_LIGNES SAUT_LIGNE
            ;

INSTS : INSTS INST  {codegen($1); codegen($2);}
      | INST  {codegen($1);}
      ;

EXP : NB      { $$ = creer_feuilleNb(yylval.nb); }
    | ID {printf("id");}
    | '(' EXP ')' { $$ = $2;}
    | EXP OU EXP {printf("OU");}
    | EXP ET EXP {printf("ET");}
    | NON EXP {printf("NON");}
    | EXP '<' EXP { $$ = creer_noeudOpComp('<', $1, $3); }
    | EXP '>' EXP { $$ = creer_noeudOpComp('>', $1, $3); }
    | EXP '=' EXP { $$ = creer_noeudOpComp('=', $1, $3); }
    | EXP SUPEGAL EXP {printf("exp supegal exp");}
    | EXP INFEGAL EXP {printf("exp supegal exp");}
    | EXP DIFF EXP {printf("DIFF");}
    | EXP '*' EXP { $$ = creer_noeudOp('*', $1, $3); }
    | EXP '/' EXP { $$ = creer_noeudOp('/', $1, $3); }
    | EXP '%' EXP { $$ = creer_noeudOp('%', $1, $3); }
    | EXP '-' EXP { $$ = creer_noeudOp('-', $1, $3); }
    | EXP '+' EXP { $$ = creer_noeudOp('+', $1, $3); }
    | '-' EXP %prec OP_UN { $$ = creer_noeudOpUn('-', $2);}
    | '+' EXP %prec OP_UN { $$ = creer_noeudOpUn('+', $2);}
    ;

INST : SAUT_LIGNE {$$= NULL;}
     | VAR ID {printf("var id saut_ligne");}
     | ID AFFECT EXP {printf("id affect nb");}
     | SI EXP ALORS INSTS SINON INSTS FSI {printf("si exp alors sinon insts");}
     | SI EXP ALORS INSTS FSI {printf("Si exp alors insts");}
     | TQ EXP FAIRE INSTS FTQ {printf("tq exp faire sinon insts ftq");}
     | AFFICHER EXP SAUT_LIGNE { printf("afficher"); $$ = $2;}
     | EXP SAUT_LIGNE{ $$ = $1;}
     ;
%%
int main( int argc, char * argv[] ) {

  extern FILE *yyin;
  if (argc != 3){
    fprintf(stderr, "usage : ./arc file.algo file.ram\n");
    return 1;
  }
  ligne_ram = 0;
  tete = 1;
  out_file = fopen(argv[2], "w");
  yyin = fopen(argv[1],"r");
  yyparse();
  fprintf(out_file, "WRITE\nSTOP\n");
  return 0;
}

