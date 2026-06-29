#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "modules/led_rgb.h"
#include "modules/uart_pc.h"
#include "modules/joystick.h"

int main(void) {
    // Configuration de l'horloge système à 120 MHz
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // Initialisations successives
    LED_RGB_Init();   // Doit contenir ton code stable (sans appel dynamique à TimerIntRegister)
    UART_PC_Init();   // Doit contenir ton code stable (sans appel dynamique à IntRegister)
    Joystick_Init();  // Notre nouveau module sécurisé

    // Force une couleur de test pour valider le démarrage du microcontrôleur
    LED_RGB_SetIntensity(0, 255, 0); // Vert pour dire "OK"

    while (1) {
        // La boucle reste vide pour le moment, on valide juste le boot
    }
}
