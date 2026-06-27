#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "driverlib/sysctl.h"
#include "modules/led_rgb.h"
#include "modules/uart_pc.h"

#define MAIN_BUFFER_SIZE 32
char mainRxBuffer[MAIN_BUFFER_SIZE];

void ProcessCommand(char* cmd) {
    int r = 0, g = 0, b = 0;
    
    // Décodage de la chaîne "R,G,B"
    if (sscanf(cmd, "%d,%d,%d", &r, &g, &b) == 3) {
        if (r < 0) r = 0; if (r > 255) r = 255;
        if (g < 0) g = 0; if (g > 255) g = 255;
        if (b < 0) b = 0; if (b > 255) b = 255;

        LED_RGB_SetIntensity((uint8_t)r, (uint8_t)g, (uint8_t)b);
    }
}

int main(void) {
    // 1. Horloge système à 120 MHz (Unique configuration globale)
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // 2. Initialisation de tes modules découplés
    LED_RGB_Init();
    UART_PC_Init(); // Initialisé en dernier comme dans ton architecture

    // Petit test visuel : lueur blanche au démarrage
    LED_RGB_SetIntensity(10, 10, 10);
    UART_PC_SendString("Systeme Pret. En attente de commandes RGB...\n");

    while (1) {
        // Interrogation de l'interface publique de l'UART
        if (UART_PC_CommandAvailable()) {
            UART_PC_GetCommand(mainRxBuffer);
            ProcessCommand(mainRxBuffer);
        }
    }
}
