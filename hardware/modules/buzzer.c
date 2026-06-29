#include "buzzer.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

// Fréquence système à 120 MHz
static uint32_t g_ui32SysClock = 120000000;

void Buzzer_Init(void) {
    // 1. Activation des horloges pour le GPIO F et le module PWM0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0)) {}

    // 2. Configuration de la broche PF1 pour le multiplexage du PWM0
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);

    // 3. Configuration du Générateur 0 du PWM (Le Générateur 0 gère M0PWM0 et M0PWM1)
    // Mode "Count-Down" sans synchronisation externe
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // 4. Activation du générateur de signal
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);

    // On s'assure que le buzzer est complètement silencieux au démarrage
    Buzzer_Stop();
}

void Buzzer_PlayNote(uint32_t frequency) {
    if (frequency == 0) {
        Buzzer_Stop();
        return;
    }

    // Calcul de la période en cycles d'horloge pour la fréquence demandée
    uint32_t ui32Period = g_ui32SysClock / frequency;

    // Ajustement de la période du Générateur 0
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Period);

    // Fixation du rapport cyclique à 50% sur la sortie PWM1 (Broche PF1)
    // Le son est optimal avec un signal parfaitement carré
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ui32Period / 2);

    // Activation de la sortie physique du PWM1
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
}

void Buzzer_Stop(void) {
    // Désactivation immédiate de la sortie physique (0V constant sur PF1)
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
}
