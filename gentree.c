#include "gentree.h"
#include <string.h>

treenode *newOperatorNode(int op, treenode *left, treenode *right) {
    treenode *newNode = (treenode*) malloc(sizeof(treenode));

    if(newNode == NULL) {
        fprintf(stderr, "Out of memory.\n");
        exit(4);
    }

    newNode->op = op;
    newNode->kids[0] = left;
    newNode->kids[1] = right;
    newNode->regname = NULL;
    newNode->id = NULL;
    newNode->val = 0;
    return newNode;
}

treenode *newRegisterNode(char* regname) {
    treenode *newNode = newOperatorNode(OP_REG, NULL, NULL);
    newNode->regname = regname;
    return newNode;
}


treenode *newNumNode(long num) {
    treenode *newNode = newOperatorNode(OP_NUM, NULL, NULL);
    newNode->val = num;
    return newNode;
}

treenode *newIdNode(char *id, int offset, const char *regname) {
    treenode *newNode = newOperatorNode(OP_ID, NULL, NULL);
    newNode->id = id;
    newNode->val = offset;
    if(regname != NULL) {
        newNode->regname = regname;
    }
    return newNode;
}