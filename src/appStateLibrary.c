#include "appStateLibrary.h"
#include <stdio.h>

//--- PLC stavové řetězce -------------------------------------------------------

static const app_state_plc_state_string_t plc_state_string = {
    .PROG  = "Prog",
    .STOP  = "Stop",
    .RESET = "Reset",
    .SET   = "Set",
    .TEST  = "Test",
    .RUN   = "Run",
};

//--- Globální stav aplikace ---------------------------------------------------

app_state_t appState = {
    // Tlačítka - inicializaci na klid
    .button_s1.filter.input = false,
    .button_s1.filter.output = false,
    .button_s1.filter.outputValid = false,
    .button_s1.filter.stateCtr = 0,
    .button_s1.outputMemory = false,
    
    .button_s2.filter.input = false,
    .button_s2.filter.output = false,
    .button_s2.filter.outputValid = false,
    .button_s2.filter.stateCtr = 0,
    .button_s2.outputMemory = false,
    
    .button_s3.filter.input = false,
    .button_s3.filter.output = false,
    .button_s3.filter.outputValid = false,
    .button_s3.filter.stateCtr = 0,
    .button_s3.outputMemory = false,
    
    .button_s4.filter.input = false,
    .button_s4.filter.output = false,
    .button_s4.filter.outputValid = false,
    .button_s4.filter.stateCtr = 0,
    .button_s4.outputMemory = false,
    
    .button_s5.filter.input = false,
    .button_s5.filter.output = false,
    .button_s5.filter.outputValid = false,
    .button_s5.filter.stateCtr = 0,
    .button_s5.outputMemory = false,
    
    .button_s6.filter.input = false,
    .button_s6.filter.output = false,
    .button_s6.filter.outputValid = false,
    .button_s6.filter.stateCtr = 0,
    .button_s6.outputMemory = false,
    
    .button_s7.filter.input = false,
    .button_s7.filter.output = false,
    .button_s7.filter.outputValid = false,
    .button_s7.filter.stateCtr = 0,
    .button_s7.outputMemory = false,
    
    .button_s8.filter.input = false,
    .button_s8.filter.output = false,
    .button_s8.filter.outputValid = false,
    .button_s8.filter.stateCtr = 0,
    .button_s8.outputMemory = false,
    
    // Enkodér
    .encoder_0.outAFiltered = false,
    .encoder_0.outBFiltered = false,
    .encoder_0.outputInBaseRange = 0,
    .encoder_0.isMin = false,
    .encoder_0.isMax = false,
    
    // ADC/Potenciometr
    .adc_0.inputUnformatted = 0,
    .adc_0.outputInBaseRange = 0,
    .adc_0.isMin = false,
    .adc_0.isMax = false,
    
    // PLC stav
    .plcState = PROG,
    
    // RTM stav
    .rtm.rtmCommandActive = false,
    .rtm.lastCmd = 0,
};

//--- Implementace veřejných funkcí -------------------------------------------

app_state_t* get_app_state_address(void) {
    return &appState;
}

void init_app_state(void) {
    // Inicializuj všechna tlačítka
    appState.button_s1.filter.input = false;
    appState.button_s1.filter.output = false;
    appState.button_s1.filter.outputValid = false;
    appState.button_s1.outputMemory = false;
    
    appState.button_s2.filter.input = false;
    appState.button_s2.filter.output = false;
    appState.button_s2.filter.outputValid = false;
    appState.button_s2.outputMemory = false;
    
    appState.button_s3.filter.input = false;
    appState.button_s3.filter.output = false;
    appState.button_s3.filter.outputValid = false;
    appState.button_s3.outputMemory = false;
    
    appState.button_s4.filter.input = false;
    appState.button_s4.filter.output = false;
    appState.button_s4.filter.outputValid = false;
    appState.button_s4.outputMemory = false;
    
    appState.button_s5.filter.input = false;
    appState.button_s5.filter.output = false;
    appState.button_s5.filter.outputValid = false;
    appState.button_s5.outputMemory = false;
    
    appState.button_s6.filter.input = false;
    appState.button_s6.filter.output = false;
    appState.button_s6.filter.outputValid = false;
    appState.button_s6.outputMemory = false;
    
    appState.button_s7.filter.input = false;
    appState.button_s7.filter.output = false;
    appState.button_s7.filter.outputValid = false;
    appState.button_s7.outputMemory = false;
    
    appState.button_s8.filter.input = false;
    appState.button_s8.filter.output = false;
    appState.button_s8.filter.outputValid = false;
    appState.button_s8.outputMemory = false;
    
    // Inicializuj enkodér
    appState.encoder_0.outAFiltered = false;
    appState.encoder_0.outBFiltered = false;
    appState.encoder_0.outputInBaseRange = 0;
    appState.encoder_0.isMin = false;
    appState.encoder_0.isMax = false;
    
    // Inicializuj ADC/Potenciometr
    appState.adc_0.inputUnformatted = 0;
    appState.adc_0.outputInBaseRange = 0;
    appState.adc_0.isMin = false;
    appState.adc_0.isMax = false;
    
    // Inicializuj PLC stav
    appState.plcState = PROG;
    
    // Inicializuj RTM stav
    appState.rtm.rtmCommandActive = false;
    appState.rtm.lastCmd = 0;
}

const app_state_plc_state_string_t* get_app_state_plc_state_string_address(void) {
    return &plc_state_string;
}

const char* get_app_state_plc_state_string(app_state_plc_state_enum_t state) {
    switch (state) {
        case PROG:
            return plc_state_string.PROG;
        case STOP:
            return plc_state_string.STOP;
        case RESET:
            return plc_state_string.RESET;
        case SET:
            return plc_state_string.SET;
        case TEST:
            return plc_state_string.TEST;
        case RUN:
            return plc_state_string.RUN;
        default:
            return "Neznámý";
    }
}

void view_app_state(void) {
    // Bod přerušení pro inspekci stavu v debuggeru
    app_state_t* pState = get_app_state_address();
    (void)pState; // Vyhnout se varování o nepoužitých proměnných
}

//--- Konec appStateLibrary.c -----------------------------------------------