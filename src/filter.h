#ifndef FILTER_H
#define FILTER_H

#include <stdbool.h>

typedef struct {
    unsigned stateCtr : 4;     
    bool inputValue : 1;
    bool outputValid : 1;
    bool outputValue : 1;
} filterTypeBool_t;

void initFilterTypeBool(filterTypeBool_t *filter, bool initValue);
void runFilterTypeBool(filterTypeBool_t *filter, bool value);

#endif