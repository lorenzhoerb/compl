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
    setupMethodStack(varCount);
    printf("\n");
}

void setupMethodStack(unsigned varCount) {
    printf("enter $%d, $0\n", 8*varCount);
}

void writeDecleration( int destOffset, int value) {
    printf("moveq $%d, %d(%%sbp)\n", value, offsetToAddrOffset(destOffset));
}

void genReturn() {
    printf("ret\n\n");
}

void writeLoadId(int offset, char *destReg) {
    printf("movq %d(%%sbp) %s\n", offsetToAddrOffset(offset), destReg);
}

int offsetToAddrOffset(int offset) {
    return (1+offset)*8;
}