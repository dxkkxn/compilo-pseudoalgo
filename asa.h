#ifndef ASA_H
#define ASA_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ts.h"
#include "parser.h"

typedef enum {typeNb, typeOp, typeOpUn, typeOpComp, typeInst,
              typeOpLog, typeAff, typeID, typeIDTab, typeAffTab} typeNoeud;
enum {diff, supegal, infegal};
enum {non, et, ou};
typedef enum {afficher, si_sinon, si, tq} inst_t;

typedef struct {
  int val;
} feuilleNb;

typedef struct {
  int ope;
  struct asa * noeud[2];
} noeudOp;

typedef struct {
  inst_t instr;
  struct asa * noeud_exp;
  struct node_list * node_insts[2];
} noeudInst;

typedef struct {
  struct asa * noeud[2];
} noeudAff;

typedef struct {
  ts * ts_id;
  struct asa * index;
  struct asa * exp;
} noeudAffTab;

typedef struct {
  ts * ts_id;
} feuilleID;

typedef struct {
  ts * ts_id;
  struct asa * index;
} feuilleIDTab;

typedef struct asa {
  typeNoeud type;
  int ninst;
  union {
    feuilleNb nb;
    noeudOp op;
    noeudInst inst;
    noeudAff aff;
    feuilleID id;
    feuilleIDTab id_tab;
    noeudAffTab aff_tab;
  };
} asa;

typedef struct node_list {
   struct asa * tree;
   struct node_list * next;
} node_list;

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

asa * creer_noeudInst(inst_t inst, asa * p1, node_list * p2, node_list * p3);
asa * creer_noeudOpLog(int ope, asa * p1, asa * p2);
asa * creer_noeudAff(asa * p1, asa * p2);
asa * creer_noeudAffTab(char * iden, asa * index,  asa * exp) ;
asa * creer_feuilleID(char * iden);
asa * creer_feuilleIDTab(char * iden, asa * i);
void free_asa(asa *p);

// produit du code pour la machine RAM à partir de l'arbre abstrait
// ET de la table de symbole
void codegen(asa *p);
void main_codegen(node_list * head) ;
void codegen_list_insts(node_list * head);
void codegen_entree(char * id);

extern ts * tsymb;
extern FILE * out_file;

/*
** foncions de detection pour les optimisations
*/
bool detect_const(asa * arbre_op) ;
bool detect_inc(asa * arbre_aff) ;
bool detect_dec(asa * arbre_aff) ;
bool detect_comp_iden(asa * arbre_comp) ;

// fontions de node_list
node_list * creer_node_list(asa * tree);
// Concatene l1 a l2
// (l1 -> l2)
node_list * extend(node_list * l1, node_list * l2);
int count_ninst(node_list * head);
void free_list_insts(node_list * head) ;

void print_inst_list(node_list * head) ;

void codegen_entree(char * id) ;
void main_codegen(node_list * head) ;

#endif
