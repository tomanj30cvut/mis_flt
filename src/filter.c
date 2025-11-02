#include "filter.h"

enum {S0, S1, S2, S3, S4};

void initFilterTypeBool(filterTypeBool_t *filter, bool initValue) {
    filter->stateCtr = S0;
    filter->inputValue = initValue;
    filter->outputValid = false;
    filter->outputValue = false;
}


void runFilterTypeBool(filterTypeBool_t *filter, bool value) {
    filter->inputValue = value;
    
    switch(filter->stateCtr) {
        case S0:
            filter->outputValid = false;
            if(value)
                filter->stateCtr = S1;
            else
                filter->stateCtr = S3;
            break;
            
        case S1:
            filter->outputValid = false;
            if(value)
                filter->stateCtr = S2; 
            else
                filter->stateCtr = S3;
            break;
            
        case S2:
            if(value)
            {
                filter->stateCtr = S2; 
                filter->outputValue = true;
                filter->outputValid = true;
            }
            else
            {
                filter->stateCtr = S3;
                filter->outputValid = false;
            }
            break;
            
        case S3:
            filter->outputValid = false;
            if(value)
                filter->stateCtr = S1;
            else
                filter->stateCtr = S4;
            break;
            
        case S4:
            if(value)
            {
                filter->stateCtr = S1; 
                filter->outputValid = false;
            }
            else
            {
                filter->stateCtr = S4;  
                filter->outputValue = false;  
                filter->outputValid = true;
            }
            break;
            
        default:
            filter->stateCtr = S0;
            filter->outputValid = false;
            break;
    }
}