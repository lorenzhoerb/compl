#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <stdio.h>

void defineClassSection(char *className, char **selectors, unsigned size);
void defineObject(char *objName, char *className);
void implementMethod(char *className, char *methodName, unsigned varCount);
void setupMethodStack(unsigned varCount);
void genReturn(void);
void writeDecleration(int value, int destOffset);

#endif