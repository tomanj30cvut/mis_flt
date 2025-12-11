/*
 * File:   rtm_pwm_extension.c
 * Purpose: Rozšíření RTM komunikace pro PWM úlohu
 * 
 * Popis: Tato implementace rozšiřuje messengerMIS.c o nové RTM příkazy
 *        (CMD 0-4) pro PWM úlohu
 */

#include "messengerMIS.h"
#include "platformMIS32mk.h"
#include "appStateLibrary.h"
#include <stdio.h>
#include <string.h>
