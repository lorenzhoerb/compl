#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <stdio.h>

void defineClassSection(char *className, char **selectors, unsigned size);
void defineObject(char *objName, char *className);
void implementMethod(char *className, char *methodName, unsigned varCount);
void setupMethodStack(unsigned varCount);
void genReturn(void);
void writeDecleration(int destOffset, int value);
void writeLoadId(int offset, char *destReg);
int offsetToAddrOffset(int offset);
void writeAddNumToReg(char *regname, int val);
void writeOpNumReg(char *op, char *regname, int val);
void writeOpRegReg(char *op, char *srcReg, char *dest);
void writeReturnNum(int val);
void writeReturnReg(char *regname);
void writeDeclerationReg(int offset, char *regname);
void writeGreaterThan(char *reg1, char *reg2, char *dest);
void writeNeg(int factor, char *regname);
void writeLoadFromStack(int offset, char *destReg);
void writeOpRegRegDest(char *op, char *reg1, char *reg2, char *dest);
void writeOPNumRegDest(char *op, int val, char *reg, char *dest);
char *regToLowerBitReg(char *reg);
void writeGreaterThanNumReg(int val, char *reg, char *dest);
void writeGreaterThanRegNum(char *reg, int val, char *dest);

#endif