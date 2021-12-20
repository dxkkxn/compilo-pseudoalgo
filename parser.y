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
  asa * tmp;
  asa * tab[1024];
  int index_tab = 0;

%}

%union {
  int nb;
  char * iden;
  struct asa * noeud;
  struct node_list * el_list;
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
%type <noeud> EXP INST
%type <el_list> INSTS

%right AFFECT
%left OU
%left ET
%right NON
%left '<' '>' '=' SUPEGAL INFEGAL DIFF
%left '+' '-'
%left '*' '/' '%'
%precedence OP_UN // operateur unaire
%start PROG

%%

PROG : ALGO ID SAUT_LIGNES
       ENTREE LIST_VAR SAUT_LIGNES
       DEBUT
       INSTS
       FIN SAUT_LIGNES {codegen_list_insts($8); fprintf(out_file, "STOP\n");}

LIST_VAR    : LIST_VAR ID
            | ID
            ;

SAUT_LIGNES : %empty
            | SAUT_LIGNES SAUT_LIGNE
            ;

INSTS : INST INSTS { $$ = extend(creer_node_list($1), $2);}
      | INST   { $$ = creer_node_list($1);}
      ;

EXP : NB      { $$ = creer_feuilleNb(yylval.nb); }
    | ID {$$ = creer_feuilleID(yylval.iden);}
    | '(' EXP ')' { $$ = $2;}
    | EXP OU EXP { $$ = creer_noeudOpLog(ou, $1, $3);}
    | EXP ET EXP { $$ = creer_noeudOpLog(et, $1, $3);}
    | NON EXP { $$ = creer_noeudOpLog(non, $2, NULL);}
    | EXP '<' EXP { $$ = creer_noeudOpComp('<', $1, $3); }
    | EXP '>' EXP { $$ = creer_noeudOpComp('>', $1, $3); }
    | EXP '=' EXP { $$ = creer_noeudOpComp('=', $1, $3); }
    | EXP SUPEGAL EXP { $$ = creer_noeudOpComp(supegal, $1, $3);}
    | EXP INFEGAL EXP  { $$ = creer_noeudOpComp(infegal, $1, $3);}
    | EXP DIFF EXP  { $$ = creer_noeudOpComp(diff, $1, $3);}
    | EXP '*' EXP { $$ = creer_noeudOp('*', $1, $3); }
    | EXP '/' EXP { $$ = creer_noeudOp('/', $1, $3); }
    | EXP '%' EXP { $$ = creer_noeudOp('%', $1, $3); }
    | EXP '-' EXP { $$ = creer_noeudOp('-', $1, $3); }
    | EXP '+' EXP { $$ = creer_noeudOp('+', $1, $3); }
    | '-' EXP %prec OP_UN { $$ = creer_noeudOpUn('-', $2);}
    | '+' EXP %prec OP_UN { $$ = creer_noeudOpUn('+', $2);}
    ;

INST : SAUT_LIGNE {$$ = NULL;}
     | VAR ID SAUT_LIGNE {ts_ajouter_id(yylval.iden, 1); $$=NULL;}
     | ID {tmp=creer_feuilleID(yylval.iden);} AFFECT EXP SAUT_LIGNE
     { $$ = creer_noeudAff(tmp, $4);}
     | SI EXP ALORS INSTS SINON INSTS FSI {$$ = creer_noeudInst(SiSinon,$2, $4, $6);}
     | SI EXP ALORS INSTS FSI {$$ = creer_noeudInst(Si, $2, $4, NULL);}
     | TQ EXP FAIRE INSTS FTQ {$$ = creer_noeudInst(Tq, $2, $4, NULL);}
     | AFFICHER EXP SAUT_LIGNE { $$ = creer_noeudInst(Afficher, $2, NULL, NULL);}
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
  return 0;
}

