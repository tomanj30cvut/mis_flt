#include <xc.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "platformMIS32mk.h"
#include "filter.h"
#include "memory.h"
#include "appStateLibrary.h"
#include "messengerMIS.h"
#include "encoder_decoder.h"
#include "value_converter.h"
#include "pwm_module.h"

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

static encoder_decoder_t encoderDecoder;
static value_converter_t valueConverter;

//--- PWM globální prom?nné ---------------------------------------------------
// Konstanty pro PWM modulaci
#define PWM_PERIOD_MS           20      // Perioda 20 ms
#define PWM_MIN_PULSE_MS        1       // Minimální puls 1 ms
#define PWM_MAX_PULSE_MS        2       // Maximální puls 2 ms
#define TIMER2_PRESCALER        8       // Prescaler Timer 2
#define CPU_FREQUENCY           120000000  // 120 MHz

// Vypo?ítané hodnoty pro Timer 2 a Output Compare
#define TIMER2_FREQUENCY        (CPU_FREQUENCY / TIMER2_PRESCALER)  // 15 MHz
#define PWM_PERIOD_TICKS        (TIMER2_FREQUENCY / 50)  // 50 Hz = 20 ms
#define PWM_RANGE_TICKS         (PWM_MAX_PULSE_TICKS - PWM_MIN_PULSE_TICKS)

static unsigned short pwmInput = 0;      // Vstupní hodnota PWM [0-255]
static unsigned short pwmDutyCycleTicks = PWM_MIN_PULSE_TICKS;  // Vypo?ítaný duty cycle

//--- RTM komunika?ní prom?nné ------------------------------------------------
static unsigned char rtmCommandMode = 0;  // Aktuální RTM p?íkaz (0-4)
static unsigned short rtmTimingCounter = 0;  // ?íta? pro RTM timing (50ms)
#define RTM_TIMING_MAX  50  // Odeslání zprávy ka?dých 50 ms


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
    
    initEncoderDecoder(&encoderDecoder, 128);
    initValueConverter(&valueConverter);
    
    // Inicializuj RTM komunikaci
    initMessengerRTM();
        
    // Inicializuj PWM modul
    configPWM();
}

//--- Inicializace PWM modulu (Timer 2 a Output Compare 16) -------------------
void configPWM(void) {
    // Zakáz preruseni Timer 2 b?hem konfigurace
    IEC0bits.T2IE = 0;
    
    // Nakonfiguruj Timer 2
    T2CONbits.ON = 0;           // Vypni Timer 2
    T2CONbits.TCKPS = 0b011;    // Prescaler = 8
    PR2 = PWM_PERIOD_TICKS;     // Nastavit periodu
    TMR2 = 0;                   // Vynuluj ?íta?
    
    // Nakonfiguruj Output Compare 16 (PWM mód)
    OC16CONbits.ON = 0;         // Vypni Output Compare 16
    OC16CONbits.OCTSEL = 0;     // Vyber Timer 2 jako zdroj
    OC16CONbits.OCM = 0b110;    // PWM mód: Continuous Pulse Mode
    
    // Nastav po?áte?ní duty cycle (1 ms = minimální puls)
    OC16R = PWM_MIN_PULSE_TICKS;    // Duty cycle registr
    OC16RS = PWM_MIN_PULSE_TICKS;   // Secondary duty cycle registr
    
    // Nastav p?eru?ení Timer 2
    IPC2bits.T2IP = 1;          // Priorita = 1
    IPC2bits.T2IS = 3;          // Subprioritas = 3
    IFS0bits.T2IF = 0;          // Vynuluj p?íznak p?eru?ení
    IEC0bits.T2IE = 1;          // Povol p?eru?ení Timer 2
    
    // Spus? Timer 2 a Output Compare 16
    T2CONbits.ON = 1;
    OC16CONbits.ON = 1;
}

//--- Funkce pro výpo?et duty cycle z vstupní hodnoty [0-255] ----------------
unsigned short calculateDutyCycle(unsigned char value) {
    // Ome? vstup na rozsah [0-255]
    if (value > 255) value = 255;
    
    // P?epo?et: 0 ? MIN_PULSE, 255 ? MAX_PULSE
    // Lineární interpolace: output = MIN + (value/255) * RANGE
    unsigned long dutyCycleLong = PWM_MIN_PULSE_TICKS;
    dutyCycleLong += ((unsigned long)value * PWM_RANGE_TICKS) / 255;
    
    return (unsigned short)dutyCycleLong;
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
    // Dekóduj enkodér
    unsigned char encoderCount = runEncoderDecoder(&encoderDecoder,
                                                    filterS9A.outputValue,
                                                    filterS9B.outputValue);
    
    // Vyber zdroj podle V2 a zpracuj
    bool selectEncoder = appState->button_s2.outputMemory;
    unsigned char outputValue = processValues(&valueConverter,
                                             potValue,
                                             encoderCount,
                                             selectEncoder);
    
    // Ulo? finální výstup
    appState->adc_0.outputInBaseRange = outputValue;
    
    // ?ídí MIN/MAX LED (V9, V12)
    setCoderLedLL(isMinimumReached(&valueConverter));
    setCoderLedHL(isMaximumReached(&valueConverter));
    
    // Displej (LED V13-V24)
    setFpgaVxValue((unsigned short)outputValue);
    
    runMessengerRTM();
}
