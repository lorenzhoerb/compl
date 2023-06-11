#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <stdio.h>
#include "clist.h"
#include "symt.h"
#include "gentree.h"
#include "reg.h"

void defineClassSection(char *className, symtab *symtab, clist_t *usedMethods);
void defineObject(char *objName, char *className);
void implementMethod(char *className, char *methodName, unsigned varCount);
void setupMethodStack(unsigned varCount);
void genReturn(void);
void writeDecleration(int destOffset, int type, int value, regm_t *regm);
int offsetToAddrOffset(int offset);
void writeOpNumReg(char *op, char *regname, int val);
void writeOpRegReg(char *op, char *srcReg, char *dest);
void writeReturnNum(int val);
void writeReturnReg(char *regname);
void writeDeclerationReg(int offset, int type,char *regname, regm_t *regm);
void writeGreaterThan(char *reg1, char *reg2, char *dest);
void writeNeg(int factor, char *regname);
void writeLoadFromStack(int offset, char *destReg);
void writeOpRegRegDest(char *op, char *reg1, char *reg2, char *dest);
void writeOPNumRegDest(char *op, int val, char *reg, char *dest);
char *regToLowerBitReg(char *reg);
void writeGreaterThanNumReg(int val, char *reg, char *dest);
void writeGreaterThanRegNum(char *reg, int val, char *dest);
void writeNotEqualsRegReg(char *reg1, char *reg2, char *dest);
void writeNotEqualsRegNum(char *reg, int val, char *dest);
void writeLoadFromObjVar(unsigned offset, char *reg);
void writeNegDest(char *srcReg, char *destReg);
void writeNotDest(char *srcReg, char *destReg);
void writeLabel(unsigned labelId);
void writeJumpLabel(unsigned labelId);
void writeIfJump(char *reg, unsigned val);
/**
* Creates a new object in the heap. Reserves memory for the object variables
* and saves the address of the object to destReg.
*/
void writeNewObject(char *calssName, unsigned objVarCount, char *destReg);

#endif