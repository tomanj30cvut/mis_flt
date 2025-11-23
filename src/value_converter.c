#include "value_converter.h"

void initValueConverter(value_converter_t *converter) {
    converter->potValue = 0;
    converter->encoderValue = 0;
    converter->outputValue = 128;        // St?ed
    converter->isMaxReached = false;
    converter->isMinReached = false;
    converter->selectEncoder = false;
}

// Konverze: [-2047, 2047] -> [0, 255]
unsigned char convertPotentiometerValue(signed short potValue) {
    // Omez na rozsah
    if(potValue > 2047) potValue = 2047;
    if(potValue < -2047) potValue = -2047;

    // Posun: [-2047, 2047] -> [0, 4094]
    // Normalizace: [0, 4094] -> [0, 255]
    signed long temp = (signed long)potValue + 2047;
    temp = (temp * 255) / 4094;

    return (unsigned char)temp;
}

unsigned char processValues(value_converter_t *converter,
                           signed short potValue,
                           unsigned char encoderValue,
                           bool selectEncoder) {
    // Ulo? hodnoty
    converter->potValue = potValue;
    converter->encoderValue = encoderValue;
    converter->selectEncoder = selectEncoder;

    // Vyber výstup podle p?epína?e (V2)
    if(selectEncoder) {
        converter->outputValue = encoderValue;  // Enkodér ON
    } else {
        converter->outputValue = convertPotentiometerValue(potValue);  // Potenciometr ON
    }

    // Aktualizuj MIN/MAX
    converter->isMaxReached = (converter->outputValue == 255);
    converter->isMinReached = (converter->outputValue == 0);

    return converter->outputValue;
}

unsigned char getOutputValue(value_converter_t *converter) {
    return converter->outputValue;
}

bool isMaximumReached(value_converter_t *converter) {
    return converter->isMaxReached;
}

bool isMinimumReached(value_converter_t *converter) {
    return converter->isMinReached;
}
