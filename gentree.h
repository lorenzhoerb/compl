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

typedef struct s_node {
    int op;
    struct s_node *kids[2];
    STATEPTR_TYPE state;
    char *regname;
    char *id;
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
treenode *newRegisterNode(char* regname);
treenode *newNumNode(long num);
treenode *newIdNode(char *id);

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
    OP_HASH = 10

};

#endif