#ifndef GENTREE
#define GENTREE
#ifdef USE_IBURG
#ifndef BURM
typedef struct burm_state *STATEPTR_TYPE;
#endif
#else
#define STATEPTR_TYPE int
#endif

#include <stdlib.h>
#include <stdio.h>
#include "assembly.h"

enum VAR_TYPE {
    LOC,
    OBJ,
    PAR
};

typedef struct s_node {
    int op;
    struct s_node *kids[2];
    STATEPTR_TYPE state;
    char *regname;
    char *id;
    enum VAR_TYPE type;
    int val;
} treenode;

typedef treenode *treenodep;

#define NODEPTR_TYPE	treenodep
#define OP_LABEL(p)	((p)->op)
#define LEFT_CHILD(p)	((p)->kids[0])
#define RIGHT_CHILD(p)	((p)->kids[1])
#define STATE_LABEL(p)	((p)->state)
#define PANIC		printf

treenode *newOperatorNode(int op, treenode *left, treenode *right);
treenode *newNumNode(long num);
treenode *newIdNode(char *id, int offset, enum VAR_TYPE type);
treenode *newObjectNode(char *className, int objVars);

enum {
    OP_REG = 1,
    OP_NUM = 2,
    OP_ID = 3,
    OP_ADD = 4,
    OP_MULT = 5,
    OP_NEG = 6,
    OP_UNARY = 7,
    OP_OR = 8,
    OP_GREATER = 9,
    OP_HASH = 10,
    OP_ASSIGN = 11,
    OP_RETURN = 12,
    OP_NOT = 13,
    OP_NEW = 14
};

#endif