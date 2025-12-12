/*
 * File:   pwm_module.h
 * Purpose: PWM modul pro Ãºlohu PWM - deklarace funkcÃ­
 */

#ifndef PWM_MODULE_H
#define PWM_MODULE_H

#include <stdbool.h>
#include <xc.h>

//--- Deklarace funkcí --------------------------------------------------------

/**
 * Inicializuje PWM modul (Timer 2 a Output Compare 16)
 * - Nastaví periodu na 20 ms
 * - Duty cycle od 1 ms do 2 ms
 * - Výstup na portu E, bit 1 (pin 53)
 */
void configPWM(void);

/**
 * Vypo?ítá duty cycle pro Output Compare registr
 * @param value Vstupní hodnota [0-255]
 * @return Duty cycle v tickech pro OC16RS registr
 */
unsigned short calculateDutyCycle(unsigned char value);

//--- PWM Konstanty (pro výpo?ty) ---

#define PWM_PERIOD_MS           20      // Perioda 20 ms
#define PWM_MIN_PULSE_MS        1       // Minimální puls 1 ms (hodnota 0)
#define PWM_MAX_PULSE_MS        2       // Maximální puls 2 ms (hodnota 255)
#define TIMER2_PRESCALER        8       // Prescaler Timer 2
#define CPU_FREQUENCY           120000000  // 120 MHz

// Vypo?ítané hodnoty
#define TIMER2_FREQUENCY        (CPU_FREQUENCY / TIMER2_PRESCALER)
#define PWM_PERIOD_TICKS        (TIMER2_FREQUENCY / 50)
#define PWM_MIN_PULSE_TICKS     ((TIMER2_FREQUENCY * PWM_MIN_PULSE_MS) / 1000)
#define PWM_MAX_PULSE_TICKS     ((TIMER2_FREQUENCY * PWM_MAX_PULSE_MS) / 1000)
#define PWM_RANGE_TICKS         (PWM_MAX_PULSE_TICKS - PWM_MIN_PULSE_TICKS)

#endif  // PWM_MODULE_H
