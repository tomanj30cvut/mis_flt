#include <xc.h>
#include <stdbool.h>
#include "platformMIS32mk.h"
#include "filter.h"
#include "memory.h"

filterTypeBool_t filterS1;
filterTypeBool_t filterS2;

memoryTypeBool_t memoryS1;
memoryTypeBool_t memoryS2;

filterTypeBool_t filterS9A;
filterTypeBool_t filterS9B;

void configApplication(void) {
    initFilterTypeBool(&filterS1, false);
    initFilterTypeBool(&filterS2, false);
    
    initFilterTypeBool(&filterS9A, false);
    initFilterTypeBool(&filterS9B, false);
    
    initMemoryTypeBool(&memoryS1, false);
    initMemoryTypeBool(&memoryS2, false);
}

void runApplication(void) {

    bool inputS1 = getButtonS1();
    bool inputS2 = getButtonS2();
    bool inputS9A = getCoderChannelA();
    bool inputS9B = getCoderChannelB();
    
    runFilterTypeBool(&filterS1, inputS1);
    runFilterTypeBool(&filterS2, inputS2);
    
    runFilterTypeBool(&filterS9A, inputS9A);
    runFilterTypeBool(&filterS9B, inputS9B);
    
    if(filterS1.outputValid) {
        bool memOutS1 = runMemoryTypeBool(&memoryS1, filterS1.outputValue);
        setLedV1(memOutS1);
    }
    
    if(filterS2.outputValid) {
        bool memOutS2 = runMemoryTypeBool(&memoryS2, filterS2.outputValue);
        setLedV2(memOutS2);
    }
    
    if(filterS9A.outputValid) {
        setCoderLedA(filterS9A.outputValue);
    }
    
    if(filterS9B.outputValid) {
        setCoderLedB(filterS9B.outputValue);
    }
}