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

#define burm_num_NT 1
int burm_max_nt = 1;

struct burm_state {
	int op;
	STATEPTR_TYPE left, right;
	short cost[2];
	struct {
		unsigned burm_num:4;
	} rule;
};

static short burm_nts_0[] = { burm_num_NT, burm_num_NT, 0 };
static short burm_nts_1[] = { burm_num_NT, 0 };
static short burm_nts_2[] = { 0 };

short *burm_nts[] = {
	0,	/* 0 */
	burm_nts_0,	/* 1 */
	burm_nts_1,	/* 2 */
	burm_nts_0,	/* 3 */
	burm_nts_0,	/* 4 */
	burm_nts_0,	/* 5 */
	burm_nts_0,	/* 6 */
	burm_nts_0,	/* 7 */
	burm_nts_2,	/* 8 */
};

char burm_arity[] = {
	0,	/* 0 */
	0,	/* 1=OP_REG */
	0,	/* 2=OP_NUM */
	0,	/* 3=OP_ID */
	2,	/* 4=OP_ADD */
	2,	/* 5=OP_MULT */
	1,	/* 6=OP_NEG */
	2,	/* 7=OP_UNARY */
	2,	/* 8=OP_OR */
	2,	/* 9=OP_GREATER */
	2,	/* 10=OP_HASH */
};

static short burm_decode_num[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
};

int burm_rule(STATEPTR_TYPE state, int goalnt) {
	burm_assert(goalnt >= 1 && goalnt <= 1, PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
	if (!state)
		return 0;
	switch (goalnt) {
	case burm_num_NT:
		return burm_decode_num[state->rule.burm_num];
	default:
		burm_assert(0, PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
	}
	return 0;
}


STATEPTR_TYPE burm_state(int op, STATEPTR_TYPE left, STATEPTR_TYPE right) {
	int c;
	STATEPTR_TYPE p, l = left, r = right;

	if (burm_arity[op] > 0) {
		p = (STATEPTR_TYPE)ALLOC(sizeof *p);
		burm_assert(p, PANIC("ALLOC returned NULL in burm_state\n"));
		p->op = op;
		p->left = l;
		p->right = r;
		p->rule.burm_num = 0;
		p->cost[1] =
			32767;
	}
	switch (op) {
	case 1: /* OP_REG */
		{
			static struct burm_state z = { 1, 0, 0,
				{	0,
					32767,
				},{
					0,
				}
			};
			return &z;
		}
	case 2: /* OP_NUM */
		{
			static struct burm_state z = { 2, 0, 0,
				{	0,
					1,	/* num: OP_NUM */
				},{
					8,	/* num: OP_NUM */
				}
			};
			return &z;
		}
	case 3: /* OP_ID */
		{
			static struct burm_state z = { 3, 0, 0,
				{	0,
					32767,
				},{
					0,
				}
			};
			return &z;
		}
	case 4: /* OP_ADD */
		assert(l && r);
		{	/* num: OP_ADD(num,num) */
			c = l->cost[burm_num_NT] + r->cost[burm_num_NT] + 1;
			if (c + 0 < p->cost[burm_num_NT]) {
				p->cost[burm_num_NT] = c + 0;
				p->rule.burm_num = 4;
			}
		}
		break;
	case 5: /* OP_MULT */
		assert(l && r);
		{	/* num: OP_MULT(num,num) */
			c = l->cost[burm_num_NT] + r->cost[burm_num_NT] + 1;
			if (c + 0 < p->cost[burm_num_NT]) {
				p->cost[burm_num_NT] = c + 0;
				p->rule.burm_num = 3;
			}
		}
		break;
	case 6: /* OP_NEG */
		assert(l);
		{	/* num: OP_NEG(num) */
			c = l->cost[burm_num_NT] + 1;
			if (c + 0 < p->cost[burm_num_NT]) {
				p->cost[burm_num_NT] = c + 0;
				p->rule.burm_num = 2;
			}
		}
		break;
	case 7: /* OP_UNARY */
		assert(l && r);
		{	/* num: OP_UNARY(num,num) */
			c = l->cost[burm_num_NT] + r->cost[burm_num_NT] + 1;
			if (c + 0 < p->cost[burm_num_NT]) {
				p->cost[burm_num_NT] = c + 0;
				p->rule.burm_num = 1;
			}
		}
		break;
	case 8: /* OP_OR */
		assert(l && r);
		{	/* num: OP_OR(num,num) */
			c = l->cost[burm_num_NT] + r->cost[burm_num_NT] + 1;
			if (c + 0 < p->cost[burm_num_NT]) {
				p->cost[burm_num_NT] = c + 0;
				p->rule.burm_num = 5;
			}
		}
		break;
	case 9: /* OP_GREATER */
		assert(l && r);
		{	/* num: OP_GREATER(num,num) */
			c = l->cost[burm_num_NT] + r->cost[burm_num_NT] + 1;
			if (c + 0 < p->cost[burm_num_NT]) {
				p->cost[burm_num_NT] = c + 0;
				p->rule.burm_num = 6;
			}
		}
		break;
	case 10: /* OP_HASH */
		assert(l && r);
		{	/* num: OP_HASH(num,num) */
			c = l->cost[burm_num_NT] + r->cost[burm_num_NT] + 1;
			if (c + 0 < p->cost[burm_num_NT]) {
				p->cost[burm_num_NT] = c + 0;
				p->rule.burm_num = 7;
			}
		}
		break;
	default:
		burm_assert(0, PANIC("Bad operator %d in burm_state\n", op));
	}
	return p;
}

#ifdef STATE_LABEL
static void burm_label1(NODEPTR_TYPE p) {
	burm_assert(p, PANIC("NULL tree in burm_label\n"));
	switch (burm_arity[OP_LABEL(p)]) {
	case 0:
		STATE_LABEL(p) = burm_state(OP_LABEL(p), 0, 0);
		break;
	case 1:
		burm_label1(LEFT_CHILD(p));
		STATE_LABEL(p) = burm_state(OP_LABEL(p),
			STATE_LABEL(LEFT_CHILD(p)), 0);
		break;
	case 2:
		burm_label1(LEFT_CHILD(p));
		burm_label1(RIGHT_CHILD(p));
		STATE_LABEL(p) = burm_state(OP_LABEL(p),
			STATE_LABEL(LEFT_CHILD(p)),
			STATE_LABEL(RIGHT_CHILD(p)));
		break;
	}
}

STATEPTR_TYPE burm_label(NODEPTR_TYPE p) {
	burm_label1(p);
	return STATE_LABEL(p)->rule.burm_num ? STATE_LABEL(p) : 0;
}

NODEPTR_TYPE *burm_kids(NODEPTR_TYPE p, int eruleno, NODEPTR_TYPE kids[]) {
	burm_assert(p, PANIC("NULL tree in burm_kids\n"));
	burm_assert(kids, PANIC("NULL kids in burm_kids\n"));
	switch (eruleno) {
	case 7: /* num: OP_HASH(num,num) */
	case 6: /* num: OP_GREATER(num,num) */
	case 5: /* num: OP_OR(num,num) */
	case 4: /* num: OP_ADD(num,num) */
	case 3: /* num: OP_MULT(num,num) */
	case 1: /* num: OP_UNARY(num,num) */
		kids[0] = LEFT_CHILD(p);
		kids[1] = RIGHT_CHILD(p);
		break;
	case 2: /* num: OP_NEG(num) */
		kids[0] = LEFT_CHILD(p);
		break;
	case 8: /* num: OP_NUM */
		break;
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
  case 1:
 printf("Calc: %d\n", kids[0]->val * kids[1]->val); bnode->val = kids[0]->val * kids[1]->val;
    break;
  case 2:
 bnode->val = -1*kids[0]->val;
    break;
  case 3:
 printf("Calculate: %d * %d = %d\n", kids[0]->val, kids[1]->val, kids[0]->val * kids[1] ->val); bnode->val=kids[0]->val * kids[1]->val;
    break;
  case 4:
 printf("Calculate: %d + %d = %d\n", kids[0]->val, kids[1]->val, kids[0]->val + kids[1] ->val); bnode->val=kids[0]->val + kids[1]->val;
    break;
  case 5:
 printf("Calculated bit or: %d\n", kids[0]->val | kids[1]->val); bnode->val = kids[0]->val | kids[1]->val;
    break;
  case 6:
 printf("%d > %d = %d\n", kids[0]->val, kids[1]->val, kids[0]->val > kids[1]->val ? -1 : 0); bnode->val = kids[0]->val > kids[1]->val ? -1 : 0;
    break;
  case 7:
 printf("%d HASH  %d = %d\n", kids[0]->val, kids[1]->val, kids[0]->val != kids[1]->val ? -1 : 0); bnode->val = kids[0]->val != kids[1]->val ? -1 : 0;
    break;
  case 8:
 printf("i am a register\n");
    break;
  default:    assert (0);
  }
}
