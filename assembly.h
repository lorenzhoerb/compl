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

#endif