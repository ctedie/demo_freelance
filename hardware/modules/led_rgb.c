#include "led_rgb.h"
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

// Variables privées au module pour le PWM Logiciel
static volatile uint8_t pwmCounter = 0;
static volatile uint8_t intensityR = 0;
static volatile uint8_t intensityG = 0;
static volatile uint8_t intensityB = 0;

// Prototype privé du Handler d'interruption
void Timer0AIntHandler(void);

void LED_RGB_Init(void) {
    // 1. Activation des périphériques GPIO et du Timer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // RED (PF2), GREEN (PF3)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG); // BLUE (PG0)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Timer pour le PWM logiciel

    // Attente que les périphériques soient prêts
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}

    // 2. Configurer les pins des LED en sorties GPIO normales
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);

    // Éteindre les pins au départ
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0);
    GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0);

    // 3. Configurer le Timer0A pour générer des interruptions périodiques
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, 4687);

    // 4. Enregistrer et activer l'interruption du Timer
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    
    // Activation globale des interruptions
    IntMasterEnable();

    // Démarrer le Timer
    TimerEnable(TIMER0_BASE, TIMER_A);
}

void LED_RGB_SetIntensity(uint8_t r, uint8_t g, uint8_t b) {
    intensityR = r;
    intensityG = g;
    intensityB = b;
}

// Routine du PWM Logiciel (Exécutée à ~25.6 kHz)
void Timer0AIntHandler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Gestion du Rouge (PF2)
    if (pwmCounter < intensityR) {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    } else {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    }

    // Gestion du Vert (PF3)
    if (pwmCounter < intensityG) {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
    } else {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
    }

    // Gestion du Bleu (PG0)
    if (pwmCounter < intensityB) {
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_PIN_0);
    } else {
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0);
    }

    // Incrémentation du compteur de cycle
    pwmCounter++;
}
