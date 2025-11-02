#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>

typedef struct {
    bool lastState;
    bool outputValue;
} memoryTypeBool_t;

void initMemoryTypeBool(memoryTypeBool_t *mem, bool initValue);
bool runMemoryTypeBool(memoryTypeBool_t *mem, bool value);

#endif