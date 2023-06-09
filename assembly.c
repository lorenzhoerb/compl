#include "assembly.h"

void defineClassSection(char *className, char **selectors, unsigned size) {
    printf(".data\n");
    printf("# define class %s\n", className);
    printf("%s:\n", className);
    int i;
    for (i = 0; i < size; i++) {
        printf("\t .quad %s_%s\n", className, selectors[i]);
    }
    printf("\n\n.text\n");
    printf(".globl %s\n\n", className);
    printf("# method implementation of class %s\n", className);
}

void defineObject(char *objName, char *className) {
    printf("%s:\n", objName);
    printf(".quad %s\n", className);
    // toDo add quads for obj vars;
}


void implementMethod(char *className, char *methodName, unsigned varCount) {
    printf("\n.type %s_%s, @function\n", className, methodName);
    printf("%s_%s:\n", className, methodName);
    printf("pushq %%rbp\n");
    printf("movq %%rsp, %%rbp\n");
    setupMethodStack(varCount);
    printf("\n");
}

void setupMethodStack(unsigned varCount) {
    if(varCount > 0) {
        printf("subq $%d, %%rsp\n", 8*varCount);
    }
}

void writeDecleration( int destOffset, int value) {
    printf("movq $%d, -%d(%%rbp)\n", value, offsetToAddrOffset(destOffset));
}

void genReturn() {
    printf("leave\nret\n\n");
}

void writeLoadId(int offset, char *destReg) {
    printf("movq -%d(%%rbp), %%%s\n", offsetToAddrOffset(offset), destReg);
}

int offsetToAddrOffset(int offset) {
    return (1+offset)*8;
}

void writeAddNumToReg(char *regname, int val) {
    fprintf(stdout, "addq $%d, %%%s\n", val, regname);
}

void writeOpNumReg(char *op, char *regname, int val) {

    printf("%s $%d, %%%s\n", op, val, regname);
}

void writeOpRegReg(char *op, char *srcReg, char *destReg) {
    printf("%s %%%s, %%%s\n", op, destReg, srcReg);
}

void writeReturnNum(int val) {
    printf("movq $%d, %%rax\n", val);
}

void writeReturnReg(char *regname) {
    printf("movq %%%s, %%rax\n", regname);
}

void writeDeclerationReg(int offset, char *regname) {
    printf("movq %%%s, -%d(%%rbp)\n", regname, offsetToAddrOffset(offset));
}

void writeGreaterThan(char *reg1, char *reg2) {
    printf("cmp %%%s, %%%s\n", reg2, reg1);
    printf("setg %%al\n");
    printf("and $1, %%rax\n");
    writeNeg(-1, "rax");
}

void writeNeg(int factor, char *regname) {
    writeOpNumReg("imulq", regname, factor);
}

void writeLoadFromStack(int offset, char *destReg) {
    printf("movq -%d(%%rbp), %%%s\n", offsetToAddrOffset(offset), destReg);
}

void writeOpRegRegDest(char *op, char *reg1, char *reg2, char *dest) {
    printf("movq %%%s, %%%s\n", reg2, dest);
    printf("%s %%%s, %%%s\n", op, reg1, dest);
}

void writeOPNumRegDest(char *op, int val, char *reg, char *dest) {
    printf("movq $%d, %%%s\n", val, dest);
    printf("%s %%%s, %%%s\n", op, reg, dest);
}