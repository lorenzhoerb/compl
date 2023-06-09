#ifndef REG
#define REG

#define REG_SIZE 8
#include <stdbool.h>

// Register Manager
typedef struct regm {
    unsigned parCount;
    unsigned free;
    char *regs[REG_SIZE];
    bool use[REG_SIZE];
} regm_t;

/**
* Initialezes reg manager.
* Setups up the registers and claims the first registers for the parameters.
*/
regm_t *initRegM(unsigned parCount);

/**
* Frees the allocated register manager.
*/
void destroyRegM(regm_t *regm);

/**
* Frees the given register and makes it available for the next use.
*/
void freeReg(regm_t *regm, char *reg);

/**
* Returns an available register. If there is no register free anymore error and exit.
* 
*/
char *newReg(regm_t *regm);

#endif