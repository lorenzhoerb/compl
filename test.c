typedef struct burm_state *STATEPTR_TYPE;

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "gentree.h"
#ifndef ALLOC
#define ALLOC(n) malloc(n)
#endif

#ifndef burm_assert
#define burm_assert(x,y) if (!(x)) { extern void abort(void); y; abort(); }
#endif

int burm_max_nt = 0;

struct burm_state {
	int op;
	STATEPTR_TYPE left, right;
	short cost[1];
	struct {
	} rule;
};


short *burm_nts[] = {
};

char burm_arity[] = {
};

int burm_rule(STATEPTR_TYPE state, int goalnt) {
	burm_assert(goalnt >= 1 && goalnt <= 0, PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
	if (!state)
		return 0;
	switch (goalnt) {
	default:
		burm_assert(0, PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
	}
	return 0;
}


#ifdef STATE_LABEL
NODEPTR_TYPE *burm_kids(NODEPTR_TYPE p, int eruleno, NODEPTR_TYPE kids[]) {
	burm_assert(p, PANIC("NULL tree in burm_kids\n"));
	burm_assert(kids, PANIC("NULL kids in burm_kids\n"));
	switch (eruleno) {
	default:
		burm_assert(0, PANIC("Bad external rule number %d in burm_kids\n", eruleno));
	}
	return kids;
}

int burm_op_label(NODEPTR_TYPE p) {
	burm_assert(p, PANIC("NULL tree in burm_op_label\n"));
	return OP_LABEL(p);
}

STATEPTR_TYPE burm_state_label(NODEPTR_TYPE p) {
	burm_assert(p, PANIC("NULL tree in burm_state_label\n"));
	return STATE_LABEL(p);
}

NODEPTR_TYPE burm_child(NODEPTR_TYPE p, int index) {
	burm_assert(p, PANIC("NULL tree in burm_child\n"));
	switch (index) {
	case 0:	return LEFT_CHILD(p);
	case 1:	return RIGHT_CHILD(p);
	}
	burm_assert(0, PANIC("Bad index %d in burm_child\n", index));
	return 0;
}

#endif

extern treenode *root;
extern int yyparse(void);
void burm_reduce(NODEPTR_TYPE bnode, int goalnt);

void invoke_burm(NODEPTR_TYPE root) {
    burm_label(root);
    burm_reduce(root, 1);
}
void burm_reduce(NODEPTR_TYPE bnode, int goalnt)
{
  int ruleNo = burm_rule (STATE_LABEL(bnode), goalnt);
  short *nts = burm_nts[ruleNo];
  NODEPTR_TYPE kids[100];
  int i;

  if (ruleNo==0) {
    fprintf(stderr, "tree cannot be derived from start symbol");
    exit(1);
  }
  burm_kids (bnode, ruleNo, kids);
  for (i = 0; nts[i]; i++)
    burm_reduce (kids[i], nts[i]);    /* reduce kids */

#if DEBUG
  printf ("%s", burm_string[ruleNo]);  /* display rule */
#endif

  switch (ruleNo) {
  default:    assert (0);
  }
}
