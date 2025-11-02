#include "memory.h"

void initMemoryTypeBool(memoryTypeBool_t *mem, bool initValue) {
    mem->lastState = false;
    mem->outputValue = initValue;
}

bool runMemoryTypeBool(memoryTypeBool_t *mem, bool value) {
    if(value == true && mem->lastState == false) {
        mem->outputValue = !mem->outputValue;
    }
    
    mem->lastState = value;
    return mem->outputValue;
}