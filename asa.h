#ifndef ASA_H
#define ASA_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ts.h"
#include "parser.h"

typedef enum {typeNb, typeOp, typeOpUn, typeOpComp, typeInst,
              typeOpLog, typeAff, typeID} typeNoeud;
enum {diff, supegal, infegal};
enum {non, et, ou};
typedef enum {afficher} inst_t;

typedef struct {
  int val;
} feuilleNb;

typedef struct {
  int ope;
  struct asa * noeud[2];
} noeudOp;

typedef struct {
  inst_t instr;
  struct asa * noeud;
} noeudInst;

typedef struct {
  struct asa * noeud[2];
} noeudAff;

typedef struct {
  ts * ts_id;
} feuilleID;

typedef struct asa {
  typeNoeud type;
  int ninst;
  union {
    feuilleNb nb;
    noeudOp op;
    noeudInst inst;
    noeudAff aff;
    feuilleID id;
  };
} asa;


extern int tete ;
extern int ligne_ram ;


// fonction d'erreur utilisée également par Bison
void yyerror(const char * s);

/*
  Les fonctions creer_<type> construise un noeud de l'arbre
  abstrait du type correspondant et renvoie un pointeur celui-ci
 */
asa * creer_feuilleNb(int value);
asa * creer_noeudOp(int ope, asa * p1, asa * p2);
asa * creer_noeudOpComp(int ope, asa * p1, asa * p2);
asa * creer_noeudOpUn(int ope, asa * p1);

asa * creer_noeudInst(inst_t inst, asa * p1);
asa * creer_noeudOpLog(int ope, asa * p1, asa * p2);
void free_asa(asa *p);
asa * creer_noeudAff(asa * p1, asa * p2);
asa * creer_feuilleID(char * iden);

// produit du code pour la machine RAM à partir de l'arbre abstrait
// ET de la table de symbole
void codegen(asa *p);

extern ts * tsymb;
extern FILE * out_file;

#endif
