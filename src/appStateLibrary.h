#ifndef APP_STATE_LIB
#define APP_STATE_LIB

#include <stdbool.h>

//--- Filtrovací a paměťové struktury ------------------------------------------

typedef struct {
    bool input;          ///< Vstupní hodnota
    bool output;         ///< Filtrovaná výstupní hodnota
    bool outputValid;    ///< Příznak platného výstupu
    unsigned stateCtr:4; ///< Čítač stavu
} app_state_filter_bool_t;

typedef struct {
    bool input;          ///< Vstupní hodnota
    bool output;         ///< Výstupní paměť
    unsigned stateCtr:4; ///< Čítač stavu
} app_state_memory_bool_t;

//--- Struktura tlačítka -------------------------------------------------------

typedef struct {
    app_state_filter_bool_t filter;   ///< Filtrační stupeň
    bool outputMemory;                ///< Výstup paměti (SR flipflop)
} app_state_button_t;

//--- Struktura enkodéru -------------------------------------------------------

typedef struct {
    bool outAFiltered;              ///< Filtrovaný kanál A
    bool outBFiltered;              ///< Filtrovaný kanál B
    unsigned char outputInBaseRange; ///< Dekódovaná hodnota [0-255]
    bool isMin;                     ///< Dosaženo minima
    bool isMax;                     ///< Dosaženo maxima
} app_state_encoder_t;

//--- Struktura ADC/Potenciometru -----------------------------------------------

typedef struct {
    signed short inputUnformatted;  ///< Neformátovaná hodnota ADC
    signed short outputInBaseRange; ///< Zpracovaná hodnota [-2047, 2047]
    bool isMin;                     ///< Dosaženo minima
    bool isMax;                     ///< Dosaženo maxima
} app_state_adc_t;

//--- PLC stavový automat -------------------------------------------------------

typedef enum {
    PROG,   ///< Programovací stav
    STOP,   ///< Zastavený stav
    RESET,  ///< Reset stav
    SET,    ///< Nastavit stav
    TEST,   ///< Testovací stav
    RUN,    ///< Běžící stav
} app_state_plc_state_enum_t;

typedef struct {
    const char* PROG;   ///< "Prog" řetězec
    const char* STOP;   ///< "Stop" řetězec
    const char* RESET;  ///< "Reset" řetězec
    const char* SET;    ///< "Set" řetězec
    const char* TEST;   ///< "Test" řetězec
    const char* RUN;    ///< "Run" řetězec
} app_state_plc_state_string_t;

//--- RTM struktura ------------------------------------------------------------

typedef struct {
    bool rtmCommandActive;  ///< RTM příkaz aktivní
    unsigned char lastCmd;  ///< Poslední přijatý příkaz
} app_state_rtm_t;

//--- Hlavní struktura stavů aplikace ------------------------------------------

typedef struct {
    // Tlačítka
    app_state_button_t button_s1;   ///< Tlačítko S1
    app_state_button_t button_s2;   ///< Tlačítko S2
    app_state_button_t button_s3;   ///< Tlačítko S3
    app_state_button_t button_s4;   ///< Tlačítko S4
    app_state_button_t button_s5;   ///< Tlačítko S5
    app_state_button_t button_s6;   ///< Tlačítko S6
    app_state_button_t button_s7;   ///< Tlačítko S7
    app_state_button_t button_s8;   ///< Tlačítko S8
    
    // Enkodér
    app_state_encoder_t encoder_0;  ///< Enkodér S9
    
    // ADC / Potenciometr
    app_state_adc_t adc_0;          ///< ADC/Potenciometr
    
    // PLC
    app_state_plc_state_enum_t plcState; ///< Aktuální PLC stav
    
    // RTM
    app_state_rtm_t rtm;            ///< RTM stav
} app_state_t;

//--- Veřejné funkce -----------------------------------------------------------

app_state_t* get_app_state_address(void);

void init_app_state(void);

const char* get_app_state_plc_state_string(app_state_plc_state_enum_t state);

const app_state_plc_state_string_t* get_app_state_plc_state_string_address(void);

void view_app_state(void);

#endif // APP_STATE_LIB

//--- Konec appStateLibrary.h -----------------------------------------------