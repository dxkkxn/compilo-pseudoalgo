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
%type <noeud> EXP
//%type <noeud> INSTS

%right AFFECT
%left OU
%left ET
%left '<' '>' '=' SUPEGAL INFEGAL DIFF
%left '+' '-'
%left '*' '/' '%'
%right NON
%precedence MOINS_UN
%start PROG

%%

PROG : ALGO ID SAUT_LIGNES
       ENTREE LIST_VAR SAUT_LIGNES
       DEBUT 
       INSTS 
       FIN SAUT_LIGNES
        { /*codegen($5);*/printf("OK\n");} ;

LIST_VAR    : LIST_VAR ID
            | ID
            ;

SAUT_LIGNES : %empty
            | SAUT_LIGNES SAUT_LIGNE
            ;

INSTS : INSTS INST 
      | INST  
      ;

EXP : NB      { /*$$ = creer_feuilleNb(yylval.nb);*/ printf("NB"); }
    | ID {printf("id");}
    | '(' EXP ')' { printf("( EXP )");}
    | EXP OU EXP {printf("OU");}
    | EXP ET EXP {printf("ET");}
    | NON EXP {printf("NON");}
    | EXP '<' EXP {printf("exp < exp");}
    | EXP '>' EXP {printf("exp > exp");}
    | EXP SUPEGAL EXP {printf("exp supegal exp");}
    | EXP INFEGAL EXP {printf("exp supegal exp");}
    | EXP '=' EXP {printf("exp = exp");}
    | EXP DIFF EXP {printf("DIFF");}
    | EXP '*' EXP {printf("*");}
    | EXP '/' EXP {printf("/");}
    | EXP '%' EXP {printf("%%");}
    | EXP '-' EXP {printf("-");}
    | '-' EXP %prec MOINS_UN {printf("moins un");}
    | EXP '+' EXP {printf("+");} //$$ = creer_noeudOp('+', $1, $3); }
    ;
INST : SAUT_LIGNE {printf("saut_ligne");}
     | VAR ID {printf("var id saut_ligne");}
     | ID AFFECT EXP {printf("id affect nb");}
     | SI EXP ALORS INSTS SINON INSTS FSI {printf("si exp alors sinon insts");}
     | SI EXP ALORS INSTS FSI {printf("Si exp alors insts");}
     | TQ EXP FAIRE INSTS FTQ {printf("tq exp faire sinon insts ftq");}
     | AFFICHER EXP {printf("afficher exp");}
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

