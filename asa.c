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

asa * creer_feuilleID(char * iden) {
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");
  printf("ok\n");
  printf(">>>> %s\n", iden);
  printf("ok\n");
  p->ninst = 1;
  p->type = typeID;
  p->id.ts_id = ts_retrouver_id(iden);
  if (p->id.ts_id == NULL) {
    yyerror("[ERROR] Variable non declare");
 }
  return p;
}

asa * creer_noeudAff(asa * p1, asa * p2) {
  asa * p;

  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");

  p->type = typeAff;
  p->aff.noeud[0]= p1;
  p->aff.noeud[1]= p2;
  p->ninst = p1->ninst+p2->ninst+1;
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
  if (ope == '-') {
    p->ninst += 1;
  }
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

  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");

  p->type = typeOp;
  p->op.ope = ope;
  p->op.noeud[0]=p1;
  p->op.noeud[1]=p2;
  p->ninst = p1->ninst+p2->ninst;
  switch (ope) {
    case '>': case'<': case '=': case diff:
      p->ninst += 6;
      break;
    case infegal: case supegal:
      p->ninst += 7;
      break;
  }

  return p;
}

asa * creer_noeudOpLog(int ope, asa * p1, asa * p2) {
  asa * p;

  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");

  p->type = typeOpLog;
  p->op.ope = ope;
  p->op.noeud[0]=p1;
  p->ninst = p1->ninst;
  switch(ope) {
    case non:
      p->ninst += 3;
      break;
    case ou:
      p->op.noeud[1]=p2;
      p->ninst += p2->ninst;
      p->ninst += 5;
      break;
    case et:
      p->op.noeud[1]=p2;
      p->ninst += p2->ninst;
      p->ninst += 3;
      break;
  }

  return p;
}

int count_ninst(node_list * head) {
  int res = 0;
  node_list * aux = head;
  while(aux->next != NULL) {
    res += aux->tree->ninst;
    aux = aux->next;
  }
  return res;
}
asa * creer_noeudInst(inst_t inst, asa * p1, node_list * p2, node_list * p3) {
    asa * p;

    if ((p = malloc(sizeof(asa))) == NULL)
      yyerror("echec allocation mémoire");

    p->type = typeInst;
    p->inst.instr = inst;
    p->inst.noeud_exp = p1;
    p->ninst = p1->ninst+1;
    switch (inst) {
      case Si : case Tq:
        break;
      case SiSinon:
        p->inst.node_insts[0]= *p2;
        p->inst.node_insts[1]= *p3;
        break;
      default:
        break;
    }

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
    printf("codegen\n");
    if (!p) {
      printf ("!p error\n"); return;
    }
    printf("no error should write %d\n", p->type);
    switch(p->type) {
      case typeNb:
          fprintf(out_file, "LOAD #%d\n", p->nb.val);
          ligne_ram++;
          break;
      case typeID:
          fprintf(out_file, "LOAD %d\n", p->id.ts_id->adr);
          ligne_ram++;
          break;
      case typeOpUn:
          switch(p->op.ope) {
            case '+':
              codegen(p->op.noeud[0]);
              break;
            case '-':
              codegen(p->op.noeud[0]);
              fprintf(out_file, "MUL #-1\n"); ligne_ram++;
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
          break;
      case typeOpComp:
          switch(p->op.ope) {
              case '=':
                codegen(p->op.noeud[0]);
                fprintf(out_file, "STORE %d\n", tete++); ligne_ram++;
                codegen(p->op.noeud[1]);
                fprintf(out_file, "SUB %d\n", --tete); ligne_ram++;
                fprintf(out_file, "JUMZ %d\n", ligne_ram+3); ligne_ram++;
                fprintf(out_file, "LOAD #0\n"); ligne_ram++;
                fprintf(out_file, "JUMP %d\n", ligne_ram + 2); ligne_ram++;
                fprintf(out_file, "LOAD #1\n"); ligne_ram++;
                break;
              case '<':
                codegen(p->op.noeud[1]);
                fprintf(out_file, "STORE %d\n", tete++); ligne_ram++;
                codegen(p->op.noeud[0]);
                fprintf(out_file, "SUB %d\n", --tete); ligne_ram++;
                fprintf(out_file, "JUML %d\n", ligne_ram+3); ligne_ram++;
                fprintf(out_file, "LOAD #0\n"); ligne_ram++;
                fprintf(out_file, "JUMP %d\n", ligne_ram + 2); ligne_ram++;
                fprintf(out_file, "LOAD #1\n"); ligne_ram++;
                break;
              case '>':
                codegen(p->op.noeud[1]);
                fprintf(out_file, "STORE %d\n", tete++); ligne_ram++;
                codegen(p->op.noeud[0]);
                fprintf(out_file, "SUB %d\n", --tete); ligne_ram++;
                fprintf(out_file, "JUMG %d\n", ligne_ram+3); ligne_ram++;
                fprintf(out_file, "LOAD #0\n"); ligne_ram++;
                fprintf(out_file, "JUMP %d\n", ligne_ram + 2); ligne_ram++;
                fprintf(out_file, "LOAD #1\n"); ligne_ram++;
                break;
            case diff:
                codegen(p->op.noeud[0]);
                fprintf(out_file, "STORE %d\n", tete++); ligne_ram++;
                codegen(p->op.noeud[1]);
                fprintf(out_file, "SUB %d\n", --tete); ligne_ram++;
                fprintf(out_file, "JUMZ %d\n", ligne_ram+3); ligne_ram++;
                fprintf(out_file, "LOAD #1\n"); ligne_ram++;
                fprintf(out_file, "JUMP %d\n", ligne_ram + 2); ligne_ram++;
                fprintf(out_file, "LOAD #0\n"); ligne_ram++;
                break;
            case supegal:
                codegen(p->op.noeud[1]);
                fprintf(out_file, "STORE %d\n", tete++); ligne_ram++;
                codegen(p->op.noeud[0]);
                fprintf(out_file, "SUB %d\n", --tete); ligne_ram++;
                fprintf(out_file, "JUMG %d\n", ligne_ram+4); ligne_ram++;
                fprintf(out_file, "JUMZ %d\n", ligne_ram+3); ligne_ram++;
                fprintf(out_file, "LOAD #0\n"); ligne_ram++;
                fprintf(out_file, "JUMP %d\n", ligne_ram + 2); ligne_ram++;
                fprintf(out_file, "LOAD #1\n"); ligne_ram++;
                break;
            case infegal:
              // a <= b -> a-b
                codegen(p->op.noeud[1]);
                fprintf(out_file, "STORE %d\n", tete++); ligne_ram++;
                codegen(p->op.noeud[0]);
                fprintf(out_file, "SUB %d\n", --tete); ligne_ram++;
                fprintf(out_file, "JUML %d\n", ligne_ram+4); ligne_ram++;
                fprintf(out_file, "JUMZ %d\n", ligne_ram+3); ligne_ram++;
                fprintf(out_file, "LOAD #0\n"); ligne_ram++;
                fprintf(out_file, "JUMP %d\n", ligne_ram + 2); ligne_ram++;
                fprintf(out_file, "LOAD #1\n"); ligne_ram++;
                break;
          }
          break;
      case typeOpLog:
        switch(p->op.ope) {
          case non:
            codegen(p->op.noeud[0]);
            fprintf(out_file, "JUMZ %d\n", ligne_ram+3); ligne_ram++;
            fprintf(out_file, "LOAD #0\n"); ligne_ram++;
            fprintf(out_file, "JUMP %d\n", ligne_ram+2); ligne_ram++;
            fprintf(out_file, "LOAD #1\n"); ligne_ram++;
            break;
          case et:
            codegen(p->op.noeud[0]);
            fprintf(out_file, "JUMZ %d\n", ligne_ram+p->op.noeud[1]->ninst+3);
            ligne_ram++;
            codegen(p->op.noeud[1]);
            fprintf(out_file, "JUMZ %d\n", ligne_ram+2); ligne_ram++;
            fprintf(out_file, "LOAD #1\n"); ligne_ram++;
            break;
          case ou:
            codegen(p->op.noeud[0]);
            fprintf(out_file, "JUMZ %d\n", ligne_ram+3); ligne_ram++;
            fprintf(out_file, "LOAD #1\n"); ligne_ram++;
            fprintf(out_file, "JUMP %d\n", ligne_ram+p->op.noeud[1]->ninst+3);
            ligne_ram++;
            codegen(p->op.noeud[1]);
            fprintf(out_file, "JUMZ %d\n", ligne_ram+2); ligne_ram++;
            fprintf(out_file, "LOAD #1\n"); ligne_ram++;
            break;
        }
        break;
      case typeInst:
          switch(p->inst.instr) {
              case Afficher:
                codegen(p->inst.noeud_exp);
                fprintf(out_file, "WRITE \n"); ligne_ram++;
                break;
              default:
                break;

          }
          break;
      case typeAff:
          codegen(p->aff.noeud[1]);
          fprintf(out_file, "STORE %d\n", p->aff.noeud[0]->id.ts_id->adr);
          ligne_ram++;
          break;
      default:
        break;
    }
}

void codegen_list_insts(node_list * head) {
  node_list * aux = head;
  while(aux->next != NULL) {
    codegen(head->tree);
    aux = aux->next;
  }
  printf("SUCCESS\n");
}

node_list * extend(node_list * l1, node_list * l2) {
  if (l1 == NULL) {
    return l2;
  }
  if (l2 == NULL) {
    return l1;
  }
  node_list * aux = l1;
  while(aux->next != NULL) {
    aux = aux->next;
  }
  aux->next = l2;
  return l1;
}

node_list * creer_node_list(asa * tree) {
  if (tree) {
    node_list * res;
    if ((res = malloc(sizeof(node_list))) == NULL)
      yyerror("echec allocation mémoire");

    res->next = NULL;
    res->tree = tree;
    return res;
  }
  return NULL;
}

void yyerror(const char * s) {
    fprintf(stderr, "%s\n", s);
    exit(0);
}
