#include "asa.h"
#include "ts.h"


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

asa * creer_feuilleIDTab(char * iden, asa * i) {
  ts * ts_id = ts_retrouver_id(iden);
  if (ts_id->est_tab == false)
    yyerror("[ERROR] Variable n'est pas un tableau");
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");
  p->ninst = 3+i->ninst ;
  p->type = typeIDTab;
  p->id_tab.ts_id = ts_id;
  p->id_tab.index = i;
  if (p->id.ts_id == NULL) {
    yyerror("[ERROR] Variable non declare");
 }
  return p;
}

asa * creer_noeudAffTab(char * iden, asa * index,  asa * exp) {
  asa * p;
  ts * ts_id = ts_retrouver_id(iden);
  if (ts_id->est_tab == false)
    yyerror("[ERROR] Variable n'est pas un tableau");

  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");

  p->type = typeAffTab;
  p->ninst = exp->ninst+index->ninst+3;
  p->aff_tab.ts_id = ts_id;
  p->aff_tab.index = index;
  p->aff_tab.exp = exp;
  return p;
}

asa * creer_feuilleID(char * iden) {
  asa *p;
  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");
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
  if (detect_inc(p) || detect_dec(p)){
    p->ninst = 1;
  } else {
    p->ninst = p2->ninst+1;
  }
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

asa * creer_noeudOp(int ope, asa * p1, asa * p2) {
  asa * p;
  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");
  p->type = typeOp;
  p->op.ope = ope;
  p->op.noeud[0] = p1;
  p->op.noeud[1] = p2;
  if (detect_const(p)) {
    if (p1->type == typeNb) {
      p->ninst = p2->ninst+1;
    } else {
      p->ninst = p1->ninst+1;
    }
  } else {
    p->ninst = p1->ninst + p2->ninst+2;
  }
  return p;
}

asa * creer_noeudOpComp(int ope, asa * p1, asa * p2) {
  asa * p;

  if ((p = malloc(sizeof(asa))) == NULL)
    yyerror("echec allocation mémoire");

  p->type = typeOpComp;
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
  if (detect_comp_iden(p)) {
    p->ninst = 6;
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
      p->ninst += 4;
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
  while(aux != NULL) {
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
    p->ninst = p1->ninst;
    switch (inst) {
      case si :
        p->ninst += count_ninst(p2) +1;
        p->inst.node_insts[0]= p2;
        break;
      case tq:
        p->ninst += count_ninst(p2)+2;
        p->inst.node_insts[0]= p2;
        break;
      case afficher:
        p->ninst += 1;
        break;
      case si_sinon:
        p->inst.node_insts[0]= p2;
        p->inst.node_insts[1]= p3;
        p->ninst += count_ninst(p2)+count_ninst(p3)+2;
        break;
      default:
        break;
    }
    return p;
}

bool detect_const(asa * arbre_op) {
  assert(arbre_op->type == typeOp);
  bool detected = false;
  if (arbre_op->op.noeud[0]->type == typeNb
      || arbre_op->op.noeud[1]->type == typeNb) {
      detected = true;
    }
  return detected;
}

bool detect_inc(asa * arbre_aff) {
  assert(arbre_aff->type == typeAff);
  asa * iden = arbre_aff->aff.noeud[0];
  assert(iden->type == typeID);
  bool detected = false;
  if (arbre_aff->aff.noeud[1]->type == typeOp) {
    asa * arbre_op = arbre_aff->aff.noeud[1];
    if (arbre_op->op.ope == '+'
        && (arbre_op->op.noeud[0]->type == typeID
           || arbre_op->op.noeud[1]->type == typeID)
        && (arbre_op->op.noeud[0]->type == typeNb
            || arbre_op->op.noeud[1]->type == typeNb))
      {

        asa * iden_op ;
        asa * nb;
        if (arbre_op->op.noeud[0]->type == typeID)  {
          iden_op = arbre_op->op.noeud[0];
          nb = arbre_op->op.noeud[1];
        }
        if (arbre_op->op.noeud[1]->type == typeID)  {
          iden_op = arbre_op->op.noeud[1];
          nb = arbre_op->op.noeud[0];
        }
        if (iden_op->id.ts_id == iden->id.ts_id && nb->nb.val == 1)
          detected = true;
      }
  }
  return detected;
}

bool detect_dec(asa * arbre_aff) {
  assert(arbre_aff->type == typeAff);
  asa * iden = arbre_aff->aff.noeud[0];
  assert(iden->type == typeID);
  bool detected = false;
  if (arbre_aff->aff.noeud[1]->type == typeOp) {
    asa * arbre_op = arbre_aff->aff.noeud[1];
    if (arbre_op->op.ope == '-'
        && (arbre_op->op.noeud[0]->type == typeID
           || arbre_op->op.noeud[1]->type == typeID)
        && (arbre_op->op.noeud[0]->type == typeNb
            || arbre_op->op.noeud[1]->type == typeNb))
      {

        asa * iden_op ;
        asa * nb;
        if (arbre_op->op.noeud[0]->type == typeID)  {
          iden_op = arbre_op->op.noeud[0];
          nb = arbre_op->op.noeud[1];
        }
        if (arbre_op->op.noeud[1]->type == typeID)  {
          iden_op = arbre_op->op.noeud[1];
          nb = arbre_op->op.noeud[0];
        }
        if (iden_op->id.ts_id == iden->id.ts_id && nb->nb.val == 1)
          detected = true;
      }
  }
  return detected;
}

bool detect_comp_iden(asa * arbre_comp) {
  bool detect = false;
  if (arbre_comp->op.ope == '<' && arbre_comp->op.noeud[0]->type == typeID
      && arbre_comp->op.noeud[1]->type == typeID) {
    detect = true;
  }
  return detect;
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
    int temp;
    int prec_gen_code_exp ;
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
      case typeIDTab:
          codegen(p->id_tab.index);
          fprintf(out_file, "ADD #%d\n",
                  p->id_tab.ts_id->adr); ligne_ram++;
          fprintf(out_file, "STORE %d\n", tete++); ligne_ram++;
          fprintf(out_file, "LOAD @%d\n", --tete); ligne_ram++;
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
        if (detect_const(p)) {
          asa * exp ;
          asa * cte;
          if (p->op.noeud[0]->type == typeNb)  {
            cte = p->op.noeud[0];
            exp = p->op.noeud[1];
          } else {
            assert(p->op.noeud[1]->type == typeNb);
            cte = p->op.noeud[1];
            exp = p->op.noeud[0];
          }
          codegen(exp);
          switch(p->op.ope) {
            case '+':
              fprintf(out_file, "ADD #%d\n", cte->nb.val); ligne_ram++;
              break;
            case '-':
              fprintf(out_file, "SUB #%d\n", cte->nb.val); ligne_ram++;
              break;
            case '*':
              fprintf(out_file, "MUL #%d\n", cte->nb.val); ligne_ram++;
              break;
            case '/':
              fprintf(out_file, "DIV #%d\n", cte->nb.val); ligne_ram++;
              break;
            case '%':
              fprintf(out_file, "MOD #%d\n", cte->nb.val); ligne_ram++;
              break;
          }
        } else {
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
        }
        break;
      case typeOpComp:
        if (detect_comp_iden(p)) {
          fprintf(out_file, "LOAD %d\n", p->op.noeud[0]->id.ts_id->adr);
          ligne_ram++;
          fprintf(out_file, "SUB %d\n",  p->op.noeud[1]->id.ts_id->adr);
          ligne_ram++;
          fprintf(out_file, "JUML %d\n", ligne_ram+3); ligne_ram++;
          fprintf(out_file, "LOAD #0\n"); ligne_ram++;
          fprintf(out_file, "JUMP %d\n", ligne_ram+2); ligne_ram++;
          fprintf(out_file, "LOAD #1\n"); ligne_ram++;
          break;
        } else {
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
              case afficher:
                codegen(p->inst.noeud_exp);
                fprintf(out_file, "WRITE \n"); ligne_ram++;
                break;
              case si:
                codegen(p->inst.noeud_exp);
                temp = count_ninst(p->inst.node_insts[0]);
                fprintf(out_file, "JUMZ %d\n", ligne_ram+temp+1);ligne_ram++;
                codegen_list_insts(p->inst.node_insts[0]);
                break;
              case si_sinon:
                codegen(p->inst.noeud_exp);
                temp = count_ninst(p->inst.node_insts[0]);
                fprintf(out_file, "JUMZ %d\n", ligne_ram+temp+2);ligne_ram++;
                codegen_list_insts(p->inst.node_insts[0]);
                temp = count_ninst(p->inst.node_insts[1]);
                fprintf(out_file, "JUMP %d\n", ligne_ram+temp+1);ligne_ram++;
                codegen_list_insts(p->inst.node_insts[1]);
                break;
              case tq:
                prec_gen_code_exp = ligne_ram;
                codegen(p->inst.noeud_exp);
                temp = count_ninst(p->inst.node_insts[0]);
                fprintf(out_file, "JUMZ %d\n", ligne_ram+temp+2);ligne_ram++;
                codegen_list_insts(p->inst.node_insts[0]);
                fprintf(out_file, "JUMP %d\n", prec_gen_code_exp);ligne_ram++;
                break;
              
              default:
                break;

          }
          break;
      case typeAff:
        if (detect_dec(p)) {
          fprintf(out_file, "DEC %d\n", p->aff.noeud[0]->id.ts_id->adr);
          ligne_ram++;
        } else if (detect_inc(p)) {
          fprintf(out_file, "INC %d\n", p->aff.noeud[0]->id.ts_id->adr);
          ligne_ram++;
        } else {
          codegen(p->aff.noeud[1]);
          fprintf(out_file, "STORE %d\n", p->aff.noeud[0]->id.ts_id->adr);
          ligne_ram++;
        }
        break;
      case typeAffTab:
        // calcul du bon adresse
        codegen(p->aff_tab.index);
        fprintf(out_file, "ADD #%d\n",
                p->aff_tab.ts_id->adr); ligne_ram++;

        fprintf(out_file, "STORE %d\n", tete++); ligne_ram++;

        codegen(p->aff_tab.exp);
        fprintf(out_file, "STORE @%d\n", --tete); ligne_ram++;
        break;
      default:
        break;
    }
}

void codegen_list_insts(node_list * head) {
  node_list * aux = head;
  while(aux != NULL) {
    codegen(aux->tree);
    aux = aux->next;
  }
  printf("SUCCESS\n");
}

void print_inst_list(node_list * head) {
  node_list * aux = head;
  while(aux != NULL) {
    fprintf(out_file, "%p --> ", aux);
    aux = aux->next;
  }
  fprintf(out_file, "NULL\n");
}

node_list * extend(node_list * l1, node_list * l2) {
  if (l1 == NULL) {
    return l2;
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

void codegen_entree(char * id) {
  ts * ts_id = ts_retrouver_id(id);
  int size = ts_id->size;
  for(int i = 0; i < size; i++) {
    fprintf(out_file, "READ\n"); ligne_ram++;
    fprintf(out_file, "STORE %d\n", ts_id->adr+i); ligne_ram++;
  }
}

void yyerror(const char * s) {
    fprintf(stderr, "%s\n", s);
    exit(0);
}
