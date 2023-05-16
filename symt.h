#ifndef SYMT
#define SYMT
#include <stdbool.h>
#include <stdlib.h>
#define DEBUG 1

enum sym_kind { VAR, METHOD, PARAMETER, OBJ_VAR, CLASS_NAME };

enum basic_type { INT_T, OBJECT_T };

typedef struct symbol_entry {
  char *name;
  enum sym_kind kind;
  struct complex_type *type;
  unsigned lineNr;
  struct symbol_entry *next;
} sym_entry;

typedef struct symbol_table {
  struct symbol_table *parent;
  sym_entry *first;
  sym_entry *last;
} symtab;

struct selector_list {
    char **selectors;
    size_t size;
};

typedef struct symbol_iterator {
  struct symbol_table *symtab;
  struct symbol_entry *current;
} symtab_itr;

typedef struct type_list {
  size_t size;
  enum basic_type *list;
} types;

struct complex_type {
  enum basic_type return_type;
  types *pars;
};

/**
 * Inits a symbol table
 */
symtab *symtab_init();

struct selector_list *get_selectors(symtab *symtab);

void is_return_valid(enum basic_type returnType, enum basic_type expr_type);

/**
* Checks if the both types are type int else error
*/
void check_binop_types(enum basic_type t1, enum basic_type t2);

/**
 * Inits a symbol entry
 */
sym_entry *symentry_init(char *name, enum sym_kind kind,
                         struct complex_type *type, unsigned lineNr);

types *types_add_first(types *types, enum basic_type);

/**
 * Inits a type list and sets the size to 0
 */
types *types_init(void);

struct complex_type *complex_type_init(enum basic_type return_type,
                                       types *pars);

enum basic_type symtab_lookup_return_type(symtab *symtab, char *name);

/**
 * Adds a basic type to the typle list and increases the size by one
 */
types *types_add(types *types, enum basic_type basic);

void print_types(types *types);

bool complex_type_equals(struct complex_type *t1, struct complex_type *t2);

/**
 * Merges two tables and returns the merged one
 */
symtab *symtab_merge(symtab *symtab1, symtab *symtab2);

/**
 * Inserts a symbol entry to a symbol table if it is not already existing.
 * Otherwise throw error.
 * @return symtab with added entry
 */
symtab *symtab_insert(symtab *symtab, char *name, enum sym_kind kind,
                      struct complex_type *type, unsigned lineNr);

/**
 * Creates a new namespace. Returns a new symbol tabel refering to it's parente.
 */
symtab *symtab_namespace(symtab *parent);

/**
 * Checks if a the given symbol table contains the given name
 * @return 1 if contains else 0
 */
bool symtab_contains(symtab *symtab, char *name);

/**
 * Prints the symbol table
 */
void symtab_print(symtab *symtab);

/**
 * Looks up a symbol table entry with the given name.
 * @return symbol table entry if entry with given name exists else NULL
 */
sym_entry *symtab_lookup(symtab *symtab, char *name);

/**
 * Checks if the implemented method in a class has a matching method selector.
 * If there is no selector throw error and exit.
 */
void symtab_check_method_impl(symtab *symtab, char *name,
                              struct complex_type *type, unsigned lineNr);

/**
 * Checks if the variable is declared.
 * A Variable can be a parameter, local variable or a object variable
 * Else throw error and exit
 */
void symtab_check_assign(symtab *symtab, char *name, enum basic_type bt,unsigned lineNr);

/**
 * Checks if object with class is existing.
 * Else throw error and exit
 */
void symtab_check_new(symtab *symtab, char *name, unsigned lineNr);

/**
 * Creates an iterator for the given symbol table.
 */
symtab_itr *symtab_iter(symtab *symtab);

/**
 * Gets the next element of the symbol table iterator
 * @return Next element if exists else NULL
 */
sym_entry *symtab_next(symtab_itr *itr);

#endif
