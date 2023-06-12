#include "assembly.h"
#include <string.h>

void defineClassSection(char *className, symtab *symtab, clist_t *usedMethods) {
    printf(".data\n");
    printf("# define class %s\n", className);
    printf("%s:\n", className);

    symtab_itr *itr = symtab_iter(symtab);
    sym_entry *entry;

    for (entry = symtab_next(itr); entry != NULL; entry = symtab_next(itr)) {
        if(entry->kind == METHOD) {
            if(clist_contains(usedMethods, entry->name)) {
                printf("\t .quad %s_%s\n", className, entry->name);
            } else {
                printf("\t .space 8\n");
            }
        }
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

void writeDecleration(int destOffset, int type, int value, regm_t *regm) {
    if(type == VAR) {
        printf("movq $%d, -%d(%%rbp)\n", value, offsetToAddrOffset(destOffset));
    } else if(type == OBJ) {
        printf("movq $%d, %d(%%rdi)\n", value, offsetToAddrOffset(destOffset));
    } else if(type == PAR) {
        printf("movq $%d, %%%s\n", value, regm->regs[destOffset]);
    }
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

void writeDeclerationReg(int offset, int type, char *regname, regm_t *regm) {
    if(type == VAR) {
        printf("movq %%%s, -%d(%%rbp)\n", regname, offsetToAddrOffset(offset));
    } else if(type == OBJ) {
        printf("movq %%%s, %d(%%rdi)\n", regname, offsetToAddrOffset(offset));
    } else if(type == PAR) {
        printf("movq %%%s, %%%s\n", regname, regm->regs[offset]);
    }
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

void writeLoadFromObjVar(unsigned offset, char *reg) {
    printf("movq %d(%%rdi), %%%s\n", offsetToAddrOffset(offset), reg);
}

void writeNegDest(char *srcReg, char *destReg) {
    if(strcmp(srcReg, destReg) != 0) {
        printf("movq %%%s, %%%s\n", srcReg, destReg);
    }
    printf("neg %%%s\n", destReg);
}

void writeNotDest(char *srcReg, char *destReg) {
    if(strcmp(srcReg, destReg) != 0) {
        printf("movq %%%s, %%%s\n", srcReg, destReg);
    }
    printf("not %%%s\n", destReg);
}

void writeNewObject(char *className, unsigned objVarCount, char *destReg) {
    printf("lea %s, %%%s\n", className, destReg); // init className
    printf("movq %%%s, (%%r15)\n", destReg); // init className
    printf("movq %%r15, %%%s\n", destReg);

    // initialize object variables with 0 (null)
    int i;
    for(i = 0; i < objVarCount; i++) {
        printf("movq $0, %d(%%r15)\n", offsetToAddrOffset(i));
    }
    // move heap pointer
    printf("addq $%d, %%r15\n", (objVarCount + 1)*8);
}

void writeLabel(unsigned labelId) {
    printf("L%d:\n", labelId);
}

void writeJumpLabel(unsigned labelId) {
    printf("jmp L%d\n", labelId);
}

void writeIfJump(char *reg, unsigned val) {
    printf("cmp $0, %%%s\n", reg);
    printf("je L%d\n", val);
}

void writePrepareParForCall(unsigned parIndex, char *srcReg) {
    char *oldReg = getReg(parIndex);
    printf("pushq %%%s\n", oldReg); // save register
    printf("movq %%%s, %%%s\n", srcReg, oldReg);
}

void writePrepareParForCallNum(unsigned parIndex, int val) {
    char *oldReg = getReg(parIndex);
    printf("pushq %%%s\n", oldReg); // save register
    printf("movq $%d, %%%s\n", val, oldReg);
}

void writeRestoreParametersFromStack(unsigned parCount) {
    int i;
    for(i = parCount - 1; i >= 0; i--) {
        char *parDest = getReg(i);
        printf("pop %%%s\n", parDest);
    }
}

void writeCall(char *objAdrReg, char *tmpReg, unsigned selectorOffset) {
    printf("movq (%%rdi), %%%s\n", tmpReg); //move class address from obj table to tmp
    printf("movq %d(%%%s), %%%s\n", selectorOffset * 8, tmpReg, tmpReg);
    printf("call *%%%s\n", tmpReg);
}