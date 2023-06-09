#include "assembly.h"
#include <string.h>

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

int offsetToAddrOffset(int offset) {
    return (1+offset)*8;
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
    if(strcmp(regname, "rax") != 0) {
        printf("movq %%%s, %%rax\n", regname);
    }
}

void writeDeclerationReg(int offset, char *regname) {
    printf("movq %%%s, -%d(%%rbp)\n", regname, offsetToAddrOffset(offset));
}

void writeGreaterThan(char *reg1, char *reg2, char *dest) {
    char *bitRegDest = regToLowerBitReg(dest);
    printf("cmp %%%s, %%%s\n", reg2, reg1);
    printf("setg %%%s\n", bitRegDest);
    printf("and $1, %%%s\n", dest);
    writeNeg(-1, dest);
}

void writeNeg(int factor, char *regname) {
    writeOpNumReg("imulq", regname, factor);
}

void writeLoadFromStack(int offset, char *destReg) {
    printf("movq -%d(%%rbp), %%%s\n", offsetToAddrOffset(offset), destReg);
}

void writeOpRegRegDest(char *op, char *reg1, char *reg2, char *dest) {
    char *toDest;
    if(strcmp(reg1, dest) == 0) {
        toDest = reg2;
    } else if(strcmp(reg2, dest) == 0) {
        toDest = reg1;
    } else {
        printf("movq %%%s, %%%s\n", reg2, dest);
        toDest = reg1;
    }

    if(strcmp(reg1, dest) != 0 && strcmp(reg2, dest) != 0) {
    }
    printf("%s %%%s, %%%s\n", op, toDest, dest);
}

void writeOPNumRegDest(char *op, int val, char *reg, char *dest) {
    if(strcmp(reg, dest) == 0) {
        printf("%s $%d, %%%s\n", op, val, dest);
    } else {
        printf("movq $%d, %%%s\n", val, dest);
        printf("%s %%%s, %%%s\n", op, reg, dest);
    }
}

char *regToLowerBitReg(char *reg) {
    if (strcmp(reg, "rdi") == 0) {
        return "dil";
    } else if (strcmp(reg, "rsi") == 0) {
        return "sil";
    } else if (strcmp(reg, "rdx") == 0) {
        return "dl";
    } else if (strcmp(reg, "rcx") == 0) {
        return "cl";
    } else if (strcmp(reg, "r8") == 0) {
        return "r8b";
    } else if (strcmp(reg, "r9") == 0) {
        return "r9b";
    } else if (strcmp(reg, "r10") == 0) {
        return "r10b";
    } else if (strcmp(reg, "rax") == 0) {
        return "al";
    }

    // If no match, return the original string
    return reg;
}

void writeGreaterThanNumReg(int val, char *reg, char *dest) {
    char *bitRegDest = regToLowerBitReg(dest);
    printf("cmp $%d, %%%s\n", val, reg);
    printf("setl %%%s\n", bitRegDest); // invert greater to less because operands are switched
    printf("and $1, %%%s\n", dest);
    writeNeg(-1, dest);
}

void writeGreaterThanRegNum(char *reg, int val, char *dest) {
    char *bitRegDest = regToLowerBitReg(dest);
    printf("cmp $%d, %%%s\n", val, reg);
    printf("setg %%%s\n", bitRegDest);
    printf("and $1, %%%s\n", dest);
    writeNeg(-1, dest);
}

void writeNotEqualsRegReg(char *reg1, char *reg2, char *dest) {
    char *bitRegDest = regToLowerBitReg(dest);
    printf("cmp %%%s, %%%s\n", reg2, reg1);
    printf("setne %%%s\n", bitRegDest);
    printf("and $1, %%%s\n", dest);
    writeNeg(-1, dest);
}

void writeNotEqualsRegNum(char *reg, int val, char *dest) {
    char *bitRegDest = regToLowerBitReg(dest);
    printf("cmp $%d, %%%s\n", val, reg);
    printf("setne %%%s\n", bitRegDest);
    printf("and $1, %%%s\n", dest);
    writeNeg(-1, dest);
}