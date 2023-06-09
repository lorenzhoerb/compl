#include "reg.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* regs[REG_SIZE] = {
    "rdi",
    "rsi",
    "rdx",
    "rcx",
    "r8",
    "r9",
    "r10",
    "rax"
};

regm_t *initRegM(unsigned parCount) {
    regm_t *regm = malloc(sizeof(regm_t));
    regm->parCount = parCount;
    regm->free = REG_SIZE - parCount;

    int i;
    for(i = 0; i < REG_SIZE; i++) {
        regm->regs[i] = regs[i];
        regm->use[i] = i < parCount ? true : false;
    }
    return regm;
}

void destroyRegM(regm_t *regm) {
    free(regm);
}

void freeReg(regm_t *regm, char *reg) {
    int i = regm->parCount;
    for(; i < REG_SIZE; i++) {
        if(strcmp(reg, regm->regs[i]) == 0) {
            // make register free for use
            regm->use[i] = false;
            regm->free++; 
            break;
        }
    }
}

char *newReg(regm_t *regm) {
    if(regm->free == 0) {
        fprintf(stderr, "Error: No available register\n");
        exit(3);
    }

    int i = regm->parCount;
    for(; i < REG_SIZE; i++) {
        if(regm->use[i] == false) {
            regm->use[i] = true;
            regm->free--;
            return regm->regs[i];
        }
    }
    return NULL;
}