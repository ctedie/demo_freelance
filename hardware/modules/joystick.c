#include "joystick.h"
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

void Joystick_Init(void) {
    // 1. Activation des horloges pour les boutons (Port C et Port L)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);

    // 2. Attente de stabilisation des périphériques
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)) {}

    // Petit délai de sécurité pour le bus à 120MHz
    SysCtlDelay(10);

    // 3. Configuration des Boutons (PC6, PL1, PL2) en entrées avec Pull-Up
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_1 | GPIO_PIN_2);
    
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTL_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

void Joystick_ReadState(Joystick_State_t* state) {
    // Valeurs par défaut pour l'instant (milieu de course pour l'ADC)
    state->x = 2048;
    state->y = 2048;
    
    // Lecture des boutons physiques (0 = Pressé à cause du Pull-Up)
    state->btnSelect = (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6) == 0);
    state->btnS1     = (GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1) == 0);
    state->btnS2     = (GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_2) == 0);
}
