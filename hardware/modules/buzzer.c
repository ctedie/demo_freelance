#include "buzzer.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

// On définit l'horloge d'entrée du PWM après division
// 120 MHz / 64 = 1 875 000 Hz
static uint32_t g_ui32PWMClock = 1875000;

void Buzzer_Init(void) {
    // 1. Activation des horloges
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0)) {}

    // === LA CORRECTION EST ICI ===
    // On applique un diviseur par 64 à l'horloge du PWM.
    // Cela permet aux basses fréquences (comme le DO à 262 Hz) d'entrer dans le registre 16-bit.
    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_64);

    // 2. Configuration de la broche PF1
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);

    // 3. Configuration du Générateur 0
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // 4. Activation du générateur
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);

    Buzzer_Stop();
}

void Buzzer_PlayNote(uint32_t frequency) {
    if (frequency == 0) {
        Buzzer_Stop();
        return;
    }

    // Le calcul utilise maintenant l'horloge divisée (1 875 000 Hz)
    // Exemple pour DO (262 Hz) : 1875000 / 262 = 7156 -> Entraînera une valeur < 65535 (Parfait !)
    uint32_t ui32Period = g_ui32PWMClock / frequency;

    // Ajustement de la période
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Period);

    // Rapport cyclique à 50%
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ui32Period / 2);

    // Activation de la sortie
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
}

void Buzzer_Stop(void) {
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
}
