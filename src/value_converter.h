#ifndef VALUE_CONVERTER_H
#define VALUE_CONVERTER_H

#include <stdbool.h>

//--- Struktura konvertoru

typedef struct {
    signed short potValue;      // Potenciometr [-2047, 2047]
    unsigned char encoderValue; // Enkodér [0-255]
    unsigned char outputValue;  // Výstup [0-255]
    bool isMaxReached;          // Dosa?eno maxima (255)?
    bool isMinReached;          // Dosa?eno minima (0)?
    bool selectEncoder;         // P?epína?: true=enkodér, false=potenciometr
} value_converter_t;

//--- Funkce

/** Inicializuj konvertor */
void initValueConverter(value_converter_t *converter);

/** P?evod potenciometru [-2047, 2047] -> [0, 255] */
unsigned char convertPotentiometerValue(signed short potValue);

/** Zpracuj hodnoty, vrátí výstup dle p?epína?e */
unsigned char processValues(value_converter_t *converter,
                           signed short potValue,
                           unsigned char encoderValue,
                           bool selectEncoder);

/** Vrátí aktuální výstupní hodnotu */
unsigned char getOutputValue(value_converter_t *converter);

/** Vrátí, jestli je maximum (255) */
bool isMaximumReached(value_converter_t *converter);

/** Vrátí, jestli je minimum (0) */
bool isMinimumReached(value_converter_t *converter);

#endif // VALUE_CONVERTER_H
