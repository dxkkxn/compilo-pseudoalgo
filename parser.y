%{
    #include <stdio.h>
    #include <ctype.h>
    #include <unistd.h>

    #include "asa.h"
    #include "ts.h"

    extern int yylex();
    extern int yylex_destroy();
    FILE * out_file;
    int tete ;
    int ligne_ram ;
    asa * tmp;
    node_list * list = NULL;
    char * temp;

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
%token <iden> ID



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
       ENTREE VAR_ENTREE SAUT_LIGNES
       DEBUT
       INSTS
       FIN SAUT_LIGNES {free($2); main_codegen($8);};

VAR_ENTREE  : VAR_ENTREE ID {ts_ajouter_id(yylval.iden, 1, false);
                             codegen_entree(yylval.iden); free($2);}
            | VAR_ENTREE ID '['NB']' {ts_ajouter_id($2, $4, true);
                                      codegen_entree($2); free($2);}
            | ID {ts_ajouter_id($1, 1, false);
                  codegen_entree($1); free($1);}
            | ID '['NB']' {ts_ajouter_id($1, $3, true); codegen_entree($1);
                           free($1);}
            ;

LIST_ID : LIST_ID ID  { ts_ajouter_id($2, 1, false); free($2);}
        | LIST_ID ID '['NB']' {ts_ajouter_id($2, $4, true); free($2);}
        | ID { ts_ajouter_id($1, 1, false); free($1);}
        | ID '['NB']' {ts_ajouter_id($1, $3, true); free($1);}
        ;


SAUT_LIGNES : %empty
            | SAUT_LIGNES SAUT_LIGNE
            ;

INSTS : INSTS INST   { $$ = extend($1, creer_node_list($2));}
      | INST {$$ = creer_node_list($1);}
      ;


EXP : NB      { $$ = creer_feuilleNb(yylval.nb); }
    | ID      {$$ = creer_feuilleID(yylval.iden); free(yylval.iden); }
    | ID '['EXP']' { $$ = creer_feuilleIDTab($1, $3); free($1);}
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
     | VAR ID AFFECT EXP SAUT_LIGNE {
        ts_ajouter_id($2, 1, false);
        $$ = creer_noeudAff(creer_feuilleID($2), $4); free($2);}
     | VAR LIST_ID SAUT_LIGNE {$$=NULL;};
| ID AFFECT EXP SAUT_LIGNE { $$ = creer_noeudAff(creer_feuilleID($1), $3);
                            free($1);}
     | ID '['EXP']' AFFECT EXP SAUT_LIGNE
     { $$ = creer_noeudAffTab($1, $3, $6); free($1);}
     | SI EXP ALORS INSTS SINON INSTS FSI {$$ = creer_noeudInst(si_sinon,$2, $4, $6);}
     | SI EXP ALORS INSTS FSI {$$ = creer_noeudInst(si, $2, $4, NULL);}
     | TQ EXP FAIRE INSTS FTQ {$$ = creer_noeudInst(tq, $2, $4, NULL);}
     | AFFICHER EXP SAUT_LIGNE {$$ = creer_noeudInst(afficher, $2, NULL, NULL);}
     | EXP SAUT_LIGNE { $$ = $1;}
     ;
%%
int main( int argc, char * argv[] ) {

  extern FILE *yyin;
  char * filename = "out.ram";
  int opt;

  char * target = argv[1];
  while ((opt = getopt(argc, argv, "o:")) != -1) {
      switch(opt) {
          case 'o':
              filename = optarg;
              break;
          default:
              break;
      }
  }
  out_file = fopen(filename, "w");
  ligne_ram = 0;
  tete = 1;
  yyin = fopen(target,"r");
  yyparse();
  fclose(out_file);
  fclose(yyin);
  yylex_destroy();
  return 0;
}

