#include <xc.h>
#include <stdbool.h>
#include "platformMIS32mk.h"
#include "filter.h"
#include "memory.h"
//--- Globální instance filtr? a pam?ti ----------------------------------------

// Filtry tla?ítek
static filterTypeBool_t filterS1;
static filterTypeBool_t filterS2;
static filterTypeBool_t filterS3;
static filterTypeBool_t filterS4;
static filterTypeBool_t filterS5;
static filterTypeBool_t filterS6;
static filterTypeBool_t filterS7;
static filterTypeBool_t filterS8;

// Filtry enkodéru
static filterTypeBool_t filterS9A;
static filterTypeBool_t filterS9B;

// Pam?? tla?ítek (SR flipflop)
static memoryTypeBool_t memoryS1;
static memoryTypeBool_t memoryS2;
static memoryTypeBool_t memoryS3;
static memoryTypeBool_t memoryS4;
static memoryTypeBool_t memoryS5;
static memoryTypeBool_t memoryS6;
static memoryTypeBool_t memoryS7;
static memoryTypeBool_t memoryS8;

//--- Konfigura?ní funkce -----------------------------------------------------

void configApplication(void) {
    
    // Inicializuj stav aplikace
    init_app_state();
    
    // Inicializuj filtr a pam?? tla?ítka S1
    initFilterTypeBool(&filterS1, false);
    initMemoryTypeBool(&memoryS1, false);
    
    // Inicializuj filtr a pam?? tla?ítka S2
    initFilterTypeBool(&filterS2, false);
    initMemoryTypeBool(&memoryS2, false);
    
    // Inicializuj filtr a pam?? tla?ítka S3
    initFilterTypeBool(&filterS3, false);
    initMemoryTypeBool(&memoryS3, false);
    
    // Inicializuj filtr a pam?? tla?ítka S4
    initFilterTypeBool(&filterS4, false);
    initMemoryTypeBool(&memoryS4, false);
    
    // Inicializuj filtr a pam?? tla?ítka S5
    initFilterTypeBool(&filterS5, false);
    initMemoryTypeBool(&memoryS5, false);
    
    // Inicializuj filtr a pam?? tla?ítka S6
    initFilterTypeBool(&filterS6, false);
    initMemoryTypeBool(&memoryS6, false);
    
    // Inicializuj filtr a pam?? tla?ítka S7
    initFilterTypeBool(&filterS7, false);
    initMemoryTypeBool(&memoryS7, false);
    
    // Inicializuj filtr a pam?? tla?ítka S8
    initFilterTypeBool(&filterS8, false);
    initMemoryTypeBool(&memoryS8, false);
    
    // Inicializuj filtry enkodéru
    initFilterTypeBool(&filterS9A, false);
    initFilterTypeBool(&filterS9B, false);
    
    // Inicializuj RTM komunikaci
    initMessengerRTM();
}

//--- Funkce b?hu aplikace ---------------------------------------------------

void runApplication(void) {
    
    app_state_t *appState = get_app_state_address();
    
    //--- Na?ti vstupní signály -----------------------------------------------
    
    bool inputS1 = getButtonS1();
    bool inputS2 = getButtonS2();
    bool inputS3 = getButtonS3();
    bool inputS4 = getButtonS4();
    bool inputS5 = getButtonS5();
    bool inputS6 = getButtonS6();
    bool inputS7 = getButtonS7();
    bool inputS8 = getButtonS8();
    
    bool inputS9A = getCoderChannelA();
    bool inputS9B = getCoderChannelB();
    
    signed short potValue = getPotentiometerValue();
    
    // OmeZ potenciometr na rozsah [-2047, 2047]
    if (potValue > 2047) potValue = 2047;
    if (potValue < -2047) potValue = -2047;
    
    //--- Procházej tla?ítka filtrem -------------------------------------------
    
    runFilterTypeBool(&filterS1, inputS1);
    runFilterTypeBool(&filterS2, inputS2);
    runFilterTypeBool(&filterS3, inputS3);
    runFilterTypeBool(&filterS4, inputS4);
    runFilterTypeBool(&filterS5, inputS5);
    runFilterTypeBool(&filterS6, inputS6);
    runFilterTypeBool(&filterS7, inputS7);
    runFilterTypeBool(&filterS8, inputS8);
    
    // Procházej enkodér filtrem
    runFilterTypeBool(&filterS9A, inputS9A);
    runFilterTypeBool(&filterS9B, inputS9B);
    
    //--- Aktualizuj stav aplikace filtrovanými hodnotami --------------------
    
    appState->button_s1.filter.input = filterS1.inputValue;
    appState->button_s1.filter.output = filterS1.outputValue;
    appState->button_s1.filter.outputValid = filterS1.outputValid;
    
    appState->button_s2.filter.input = filterS2.inputValue;
    appState->button_s2.filter.output = filterS2.outputValue;
    appState->button_s2.filter.outputValid = filterS2.outputValid;
    
    appState->button_s3.filter.input = filterS3.inputValue;
    appState->button_s3.filter.output = filterS3.outputValue;
    appState->button_s3.filter.outputValid = filterS3.outputValid;
    
    appState->button_s4.filter.input = filterS4.inputValue;
    appState->button_s4.filter.output = filterS4.outputValue;
    appState->button_s4.filter.outputValid = filterS4.outputValid;
    
    appState->button_s5.filter.input = filterS5.inputValue;
    appState->button_s5.filter.output = filterS5.outputValue;
    appState->button_s5.filter.outputValid = filterS5.outputValid;
    
    appState->button_s6.filter.input = filterS6.inputValue;
    appState->button_s6.filter.output = filterS6.outputValue;
    appState->button_s6.filter.outputValid = filterS6.outputValid;
    
    appState->button_s7.filter.input = filterS7.inputValue;
    appState->button_s7.filter.output = filterS7.outputValue;
    appState->button_s7.filter.outputValid = filterS7.outputValid;
    
    appState->button_s8.filter.input = filterS8.inputValue;
    appState->button_s8.filter.output = filterS8.outputValue;
    appState->button_s8.filter.outputValid = filterS8.outputValid;
    
    // Aktualizuje enkodér
    appState->encoder_0.outAFiltered = filterS9A.outputValue;
    appState->encoder_0.outBFiltered = filterS9B.outputValue;
    
    // Aktualizuje potenciometr
    appState->adc_0.inputUnformatted = potValue;
    appState->adc_0.outputInBaseRange = potValue;
    
    //--- Procházej pam?? pro tla?ítka s platným výstupem -------------------
    
    if (filterS1.outputValid) {
        bool memOutS1 = runMemoryTypeBool(&memoryS1, filterS1.outputValue);
        appState->button_s1.outputMemory = memOutS1;
        setLedV1(memOutS1);
    }
    
    if (filterS2.outputValid) {
        bool memOutS2 = runMemoryTypeBool(&memoryS2, filterS2.outputValue);
        appState->button_s2.outputMemory = memOutS2;
        setLedV2(memOutS2);
    }
    
    if (filterS3.outputValid) {
        bool memOutS3 = runMemoryTypeBool(&memoryS3, filterS3.outputValue);
        appState->button_s3.outputMemory = memOutS3;
    }
    
    if (filterS4.outputValid) {
        bool memOutS4 = runMemoryTypeBool(&memoryS4, filterS4.outputValue);
        appState->button_s4.outputMemory = memOutS4;
    }
    
    if (filterS5.outputValid) {
        bool memOutS5 = runMemoryTypeBool(&memoryS5, filterS5.outputValue);
        appState->button_s5.outputMemory = memOutS5;
    }
    
    if (filterS6.outputValid) {
        bool memOutS6 = runMemoryTypeBool(&memoryS6, filterS6.outputValue);
        appState->button_s6.outputMemory = memOutS6;
    }
    
    if (filterS7.outputValid) {
        bool memOutS7 = runMemoryTypeBool(&memoryS7, filterS7.outputValue);
        appState->button_s7.outputMemory = memOutS7;
    }
    
    if (filterS8.outputValid) {
        bool memOutS8 = runMemoryTypeBool(&memoryS8, filterS8.outputValue);
        appState->button_s8.outputMemory = memOutS8;
    }
    
    //--- ?ídí LED výstupy enkodéru -------------------------------------------
    
    if (filterS9A.outputValid) {
        setCoderLedA(filterS9A.outputValue);
    }
    
    if (filterS9B.outputValid) {
        setCoderLedB(filterS9B.outputValue);
    }
    
    runMessengerRTM();
}
