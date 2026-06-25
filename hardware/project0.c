#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void) {
    // 1. Configuration de l'horloge système à 120 MHz
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // 2. Activation du périphérique GPIO Port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Attendre que le périphérique soit prêt
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
        // Attendre que le périphérique soit prêt
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG)) {}


    // 3. Configuration des pins PF1 et PF2 en sorties digitales standards
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);

    // 4. ÉTAT DU TEST :
    // On met PF1 à 1 (GPIO_PIN_1) et PF2 à 0
   // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_PIN_0);

    while (1) {
        // Boucle infinie, observe la couleur sur ton BoosterPack
    }
}
