#include "symt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void itr_print(symtab *symtab);
void print_complex_type(struct complex_type *ct) {
  if (ct == NULL)
    return;
  if (ct->pars != NULL) {
    int i;
    for (i = 0; i < ct->pars->size; i++) {
      char *strType;
      if (ct->pars->list[i] == INT_T) {
        strType = "int";
      } else if (ct->pars->list[i] == OBJECT_T) {
        strType = "object";
      } else {
        strType = "undefined";
      }
      printf("%s X ", strType);
    }
  }

  printf("-> %d\n", ct->return_type);
}

types *types_add_first(types *t, enum basic_type basic) {
  types *newTypes = types_add(t, basic);
  size_t size = newTypes->size;
  enum basic_type tmp = newTypes->list[0];
  newTypes->list[0] = newTypes->list[size - 1];
  newTypes->list[size - 1] = tmp;
  return newTypes;
}

void is_return_valid(enum basic_type returnType, enum basic_type expr_type) {
    if(returnType != expr_type) {
        fprintf(stderr, "Error: Return type of expression does not match return type of method\n");
        exit(3);
    }
}

void check_binop_types(enum basic_type t1, enum basic_type t2) {
    if(t1 != INT_T || t2 != INT_T) {
        fprintf(stderr, "Error: Invalid types for operator. Operators require int type\n");
        exit(3);
    }
}

enum basic_type symtab_lookup_return_type(symtab *symtab, char *name) {
  sym_entry *entry = symtab_lookup(symtab, name);
  if (entry == NULL) {
    fprintf(stderr, "Error: Identifier '%s' is not declared\n", name);
    exit(3);
  }

  if(entry->kind == CLASS_NAME) {
    fprintf(stderr, "Error: variable '%s' is not declared\n", name);
    exit(3);
  }


  return entry->type->return_type;
}

void itr_print(symtab *symtab) {
  symtab_itr *itr = symtab_iter(symtab);
  sym_entry *entry;
  for (entry = symtab_next(itr); entry != NULL; entry = symtab_next(itr)) {
    printf("%s:", entry->name);
    // print_complex_type(entry->type);
    // printf(",");
  }
  printf("\n");
}

symtab *symtab_init() {
  symtab *symtab = malloc(sizeof(struct symbol_table));
  symtab->first = NULL;
  symtab->last = NULL;
  symtab->parent = NULL;
  symtab->localVarCount = 0;
  return symtab;
}

struct complex_type *complex_type_init(enum basic_type return_type,
                                       types *pars) {
  struct complex_type *ct = malloc(sizeof(struct complex_type));
  ct->return_type = return_type;
  ct->pars = pars;
  return ct;
}

types *types_init(void) {
  types *types = malloc(sizeof(types));
  types->size = 0;
  types->list = NULL;
  return types;
}

types *types_add(types *types, enum basic_type basic) {
  if (types->size == 0) {
    types->list = malloc(sizeof(enum basic_type));
    types->list[0] = basic;
    types->size = 1;
  } else {
    types->size++;
    types->list = realloc(types->list, sizeof(enum basic_type) * types->size);
    types->list[types->size - 1] = basic;
  }
  return types;
}

void print_types(types *types) {
  if (DEBUG) {
    int i;
    for (i = 0; i < types->size; i++) {
      char *strType;
      if (types->list[i] == INT_T) {
        strType = "int";
      } else if (types->list[i] == OBJECT_T) {
        strType = "object";
      } else {
        strType = "undefined";
      }
      printf("%s, ", strType);
    }
    printf("\n");
  }
}

symtab *symtab_namespace(symtab *parent) {
  symtab *namespace = symtab_init();
  namespace->parent = parent;
  namespace->localVarCount = parent->localVarCount;
  return namespace;
}

sym_entry *sym_entry_init(char *name, enum sym_kind kind,
                          struct complex_type *type, unsigned lineNr) {
  sym_entry *entry = (sym_entry *)malloc(sizeof(struct symbol_entry));
  entry->name = name;
  entry->kind = kind;
  entry->type = type;
  entry->lineNr = lineNr;
  entry->next = NULL;
  entry->varOffset = 0;
  return entry;
}

symtab *symtab_insert(symtab *symtab, char *name, enum sym_kind kind,
                      struct complex_type *type, unsigned lineNr) {

  sym_entry *duplicate = symtab_lookup(symtab, name);

  if (duplicate != NULL) {
    fprintf(stderr,
            "Duplicate symbol error: "
            "Symbol '%s' at line '%d' is already "
            "declared at line '%d'. "
            "Check the scope or rename the symbol.\n",
            name, lineNr, duplicate->lineNr);
    exit(3);
  }

  sym_entry *entry = sym_entry_init(name, kind, type, lineNr);

  if (symtab->first == NULL) {
    // table is empty
    symtab->first = entry;
    symtab->last = entry;
  } else {
    symtab->last->next = entry;
    symtab->last = entry;
  }

  symtab_print(symtab);
  // itr_print(symtab);
  return symtab;
}

symtab *symtab_insert_param(symtab *symtab, char *name, enum basic_type bt, int parOffset ,unsigned lineNr) {
  struct complex_type *ct = complex_type_init(bt, NULL);
  symtab = symtab_insert(symtab, name, PARAMETER, ct, lineNr);
  sym_entry *entry = symtab_lookup(symtab, name);
  entry->varOffset = parOffset;
  return symtab; 
}

symtab *symtab_insert_local_var(symtab *symtab, char *name, enum basic_type bt, unsigned lineNr) {
  struct complex_type *ct = complex_type_init(bt, NULL);
  symtab = symtab_insert(symtab, name, VAR, ct, lineNr);
  sym_entry *entry = symtab_lookup(symtab, name);
  entry->varOffset = symtab->localVarCount;
  symtab->localVarCount++;
  return symtab;
}

symtab *symtab_insert_obj_var(symtab *symtab, char *name, enum basic_type bt, unsigned offset, unsigned lineNr) {
  struct complex_type *ct = complex_type_init(bt, NULL);
  symtab = symtab_insert(symtab, name, OBJ_VAR, ct, lineNr);
  sym_entry *entry = symtab_lookup(symtab, name);
  entry->varOffset = offset;
  return symtab;
}

bool symtab_contains(symtab *symtab, char *name) {
  symtab_itr *itr = symtab_iter(symtab);
  sym_entry *entry;
  for (entry = symtab_next(itr); entry != NULL; entry = symtab_next(itr)) {
    if (strcmp(entry->name, name) == 0) {
      return true;
    }
  }
  return false;
}

sym_entry *symtab_lookup(symtab *symtab, char *name) {
  symtab_itr *itr = symtab_iter(symtab);
  sym_entry *entry;
  for (entry = symtab_next(itr); entry != NULL; entry = symtab_next(itr)) {
    if (strcmp(entry->name, name) == 0) {
      return entry;
    }
  }
  return NULL;
}

symtab *symtab_merge(symtab *symtab1, symtab *symtab2) {
  if (symtab1->first != NULL && symtab2->first != NULL) {
    symtab1->last->next = symtab2->first;
    symtab1->last = symtab2->last;
    return symtab1;
  } else if (symtab1->first != NULL && symtab2->first == NULL) {
    return symtab1;
  } else if (symtab1->first == NULL && symtab2->first != NULL) {
    return symtab2;
  } else {
    return symtab1; // empty table
  }
}

bool complex_type_equals(struct complex_type *t1, struct complex_type *t2) {
  if (t1 == NULL && t2 == NULL)
    return true;
  if (t1 == NULL && t2 != NULL)
    return false;
  if (t1 != NULL && t2 == NULL)
    return false;

  if (t1->return_type != t2->return_type)
    return false;

  if (t1->pars == NULL && t2->pars == NULL)
    return true;
  if (t1->pars != NULL && t2->pars == NULL)
    return false;
  if (t1->pars == NULL && t2->pars != NULL)
    return false;

  if (t1->pars->size != t2->pars->size)
    return false;

  int i;
  for (i = 0; i < t1->pars->size; i++) {
    if (t1->pars->list[i] != t2->pars->list[i])
      return false;
  }
  return true;
}

void symtab_check_method_impl(symtab *symtab, char *name,
                              struct complex_type *type, unsigned lineNr) {
  sym_entry *entry = symtab_lookup(symtab, name);
  if (entry == NULL || entry->kind != METHOD) {
    fprintf(stderr,
            "Error: Method selector is missing "
            "for method '%s' at line '%d'\n",
            name, lineNr);
    exit(3);
  }

  if (!complex_type_equals(entry->type, type)) {
    fprintf(stderr,
            "Error: Method signature of '%s' at line "
            "'%d' differs from method selectors\n",
            name, lineNr);
    exit(3);
  }
}

void symtab_check_assign(symtab *symtab, char *name, enum basic_type bt,unsigned lineNr) {
  sym_entry *entry = symtab_lookup(symtab, name);
  if (entry == NULL || entry->kind == METHOD) {
    fprintf(stderr,
            "Error: No decleration of '%s' "
            "for use at line '%d'\n",
            name, lineNr);
    exit(3);
  }

    if(entry->type->return_type != bt) {
        fprintf(stderr,
                "Error: Type of expression doesn't match declerations '%s' type at line '%d'\n",
                name, lineNr);
        exit(3);
    }

}

void symtab_check_new(symtab *symtab, char *name, unsigned lineNr) {
  sym_entry *entry = symtab_lookup(symtab, name);
  if (entry == NULL || entry->kind != CLASS_NAME) {
    fprintf(stderr,
            "Error: Object '%s' has not been declared for use at line '%d'\n",
            name, lineNr);
    exit(3);
  }
}

// symbol table iterator

symtab_itr *symtab_iter(symtab *symtab) {
  symtab_itr *itr = malloc(sizeof(symtab_itr));
  itr->symtab = symtab;
  itr->current = symtab->first;
  return itr;
}

sym_entry *symtab_next(symtab_itr *itr) {
  sym_entry *current = itr->current;
  if (current == NULL) {
    if (itr->symtab->parent == NULL) {
      return NULL;
    } else {
      itr->symtab = itr->symtab->parent;
      itr->current = itr->symtab->first;
      return symtab_next(itr);
    }
  } else {
    itr->current = itr->current->next;
    return current;
  }
}

void symtab_print(symtab *symtab) {
  if (DEBUG) {
    printf("[");
    sym_entry *tmp = symtab->first;
    for (; tmp != NULL; tmp = tmp->next) {
      printf("%s", tmp->name);
      if(tmp->kind == PARAMETER || tmp->kind == VAR) {
        printf(" offset:%d", tmp->varOffset);
      }
      if (tmp->next != NULL) {
        printf(",");
      }
    }

    if (symtab->parent != NULL) {
      symtab_print(symtab->parent);
    }

    printf("]\n");
  }
}

struct selector_list *get_selectors(symtab *symtab) {
  symtab_itr *itr = symtab_iter(symtab);
  sym_entry *entry;

  struct selector_list *sl = (struct selector_list*) malloc(sizeof(struct selector_list));
  sl->size = 0;
  sl->selectors = (char**) malloc(sizeof(char*));

  for (entry = symtab_next(itr); entry != NULL; entry = symtab_next(itr)) {
    if(entry->kind == METHOD) {
      if(sl->size == 0) {
        sl->selectors[sl->size++] = entry->name;
      } else {
        sl->selectors = (char**) realloc(sl->selectors, sizeof(char*) + sl->size + 1);
        sl->selectors[sl->size++] = entry->name;
      }
    }
  }
  return sl;
}


void printLocalVars(symtab *symtab) {
  if(DEBUG) {
    printf("local vars:\n");
    symtab_itr *itr = symtab_iter(symtab);
    sym_entry *entry;
    for (entry = symtab_next(itr); entry != NULL; entry = symtab_next(itr)) {
      if (entry->kind == VAR) {
        printf("Var: %s, Offset: %d\n", entry->name, entry->varOffset);
      }
    }
  }
}

int symtab_lookup_var_offset(struct symbol_table *symtab, char *name) {
  sym_entry *entry = symtab_lookup(symtab, name);
  return entry->varOffset;
}

enum sym_kind symtab_lookup_kind(symtab *symtab, char *name) {
  sym_entry *entry = symtab_lookup(symtab, name);
  return entry->kind;
}

bool symtab_is_kind(symtab *symtab, char *name, enum sym_kind kind) {
  sym_entry *entry = symtab_lookup(symtab, name);
  if(entry == NULL) return true;
  return entry->kind == kind;
}