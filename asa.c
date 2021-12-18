#include "asa.h"

asa * creer_feuilleNb(int val) {
  asa *p;
  printf("nb %d\n", val);
  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");
  p->ninst = 1;
  p->type = typeNb;
  p->nb.val = val;
  return p;
}

asa * creer_noeudOpUn(int ope, asa * p1) {
  asa * p;

  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");

  p->type = typeOpUn;
  p->op.ope = ope;
  p->op.noeud[0]=p1;
  p->op.noeud[1]=NULL;
  p->ninst = p1->ninst;
  return p;
}

asa * creer_noeudOpComp(int ope, asa * p1, asa * p2) {
  asa * p;
  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");
  p->type = typeOpComp;
  p->op.ope = ope;
  p->op.noeud[0] = p1;
  p->op.noeud[1] = p2;
  p->ninst = p1->ninst + p2->ninst+2;
  return p;
}

asa * creer_noeudOp(int ope, asa * p1, asa * p2) {
  asa * p;
  printf (">>>>>>>>>%c\n", ope);

  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");

  p->type = typeOp;
  p->op.ope = ope;
  p->op.noeud[0]=p1;
  p->op.noeud[1]=p2;
  p->ninst = p1->ninst+p2->ninst+2;
  
  return p;
}

void free_asa(asa *p) {
 
  if (!p) return;
  switch (p->type) {
    case typeOp:
      free_asa(p->op.noeud[0]);
      free_asa(p->op.noeud[1]);
      break;
    default: break;
  }
  free(p);
}

void codegen(asa *p) {
    printf("CODEGEN\n");
    if (!p) {
      printf ("!p error\n"); return;
    }
    printf("%d\n", p->type);
    switch(p->type) {
      case typeNb:
        fprintf(out_file, "LOAD #%d\n", p->nb.val);
        ligne_ram++;
        break;
      case typeOpUn:
        switch(p->op.ope) {
          case '+':
            codegen(p->op.noeud[0]);
            fprintf(out_file, "STORE %d\n", tete++);
            ligne_ram++;
            break;
          case '-':
            codegen(p->op.noeud[0]);
            fprintf(out_file, "MUL #-1\n");
            ligne_ram+=1;
            break;
        }
        break;
      case typeOp:
        switch(p->op.ope) {
          case '+':
            codegen(p->op.noeud[0]);
            fprintf(out_file, "STORE %d\n", tete++);
            ligne_ram++;
            codegen(p->op.noeud[1]);
            fprintf(out_file, "ADD %d\n", --tete);
            ligne_ram++;
            break;
          case '-':
            codegen(p->op.noeud[1]);
            fprintf(out_file, "STORE %d\n", tete++);
            ligne_ram++;
            codegen(p->op.noeud[0]);
            fprintf(out_file, "SUB %d\n", --tete);
            ligne_ram++;
            break;
          case '*':
            codegen(p->op.noeud[0]);
            fprintf(out_file, "STORE %d\n", tete++);
            ligne_ram++;
            codegen(p->op.noeud[1]);
            fprintf(out_file, "MUL %d\n", --tete);
            ligne_ram++;
            break;
          case '/':
            codegen(p->op.noeud[1]);
            fprintf(out_file, "STORE %d\n", tete++);
            ligne_ram++;
            codegen(p->op.noeud[0]);
            fprintf(out_file, "DIV %d\n", --tete);
            ligne_ram++;
            break;
          case '%':
            codegen(p->op.noeud[1]);
            fprintf(out_file, "STORE %d\n", tete++);
            ligne_ram++;
            codegen(p->op.noeud[0]);
            fprintf(out_file, "MOD %d\n", --tete);
            ligne_ram++;
            break;
        }
      default:
        break;
    }
}



void yyerror(const char * s) {
    fprintf(stderr, "%s\n", s);
    exit(0);
}
