#include "uart_pc.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>    
#include <string.h>   
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "led_rgb.h"

// Inclusion du module LED pour pouvoir appliquer les changements d'intensité
#include "led_rgb.h" 

// --- VARIABLES LOCALES DE STOCKAGE POUR LE BUFFER SÉRIE ---
#define UART_BUFFER_SIZE 32
static char g_cRxBuffer[UART_BUFFER_SIZE];
static uint8_t g_ui8BufferIndex = 0;

/**
 * @brief Initialisation du module UART0 pour la communication avec le PC
 * @note La fonction d'interruption UART0IntHandler doit être déclarée statiquement 
 * dans le fichier tm4c1294ncpdt_startup_ccs.c
 */
void UART_PC_Init(void) {
    // 1. Activation des horloges pour l'UART0 et le Port A (PA0/PA1)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Attente de stabilisation des périphériques
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

    // Petit délai de sécurité pour stabiliser le bus à 120 MHz
    SysCtlDelay(10);

    // 2. Configuration des broches PA0 (RX) et PA1 (TX) en mode UART
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // 3. Configuration de la liaison (115200 bauds, 8 bits de données, 1 bit de stop, pas de parité)
    // IMPORTANT : On utilise la fréquence exacte de 120 MHz configurée dans le main
    UARTConfigSetExpClk(UART0_BASE, 120000000, 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    // 4. Configuration des interruptions UART
    // On active les interruptions sur la réception (RX) et sur le timeout de réception (RT)
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    
    // On active l'interruption au niveau du contrôleur d'interruptions (NVIC)
    // NOTE : Pas de "IntRegister" ici ! La liaison est gérée par le fichier startup.
    IntEnable(INT_UART0);

    // Réinitialisation du buffer de réception par sécurité
    memset(g_cRxBuffer, 0, UART_BUFFER_SIZE);
    g_ui8BufferIndex = 0;
}

/**
 * @brief Envoie d'une chaîne de caractères via l'UART0 (Tiva C -> PC)
 */
void UART_PC_SendString(const char *str) {
    while(*str) {
        // Envoi du caractère (bloquant si le buffer d'envoi matériel FIFO est plein)
        UARTCharPut(UART0_BASE, *str++);
    }
}

/**
 * @brief Routine d'interruption (ISR) de l'UART0
 * @note Cette fonction est publique (pas de static) pour être visible par le fichier de startup.
 */
void UART0IntHandler(void) {
    uint32_t ui32Status;

    // Récupération et acquittement des drapeaux d'interruption
    ui32Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Status);

    // Lecture de tous les caractères disponibles dans la FIFO matérielle de réception
    while(UARTCharsAvail(UART0_BASE)) {
        char c = (char)UARTCharGetNonBlocking(UART0_BASE);

        // Détection de la fin de ligne (\n ou \r)
        if (c == '\n' || c == '\r') {
            if (g_ui8BufferIndex > 0) {
                // On ferme proprement la chaîne de caractères (String C)
                g_cRxBuffer[g_ui8BufferIndex] = '\0';

                // Variables de stockage temporaires pour l'extraction
                int r = 0, g = 0, b = 0;

                // Décodage du format "R,G,B" envoyé par Node.js (ex: "255,0,0")
                if (sscanf(g_cRxBuffer, "%d,%d,%d", &r, &g, &b) == 3) {
                    // === SÉCURISATION ET CONNEXION DE L'ACTIONNEUR ===
                    // On applique directement les intensités reçues à la LED RGB !
                    LED_RGB_SetIntensity(r, g, b);
                }

                // Réinitialisation de l'index du buffer pour la prochaine trame
                g_ui8BufferIndex = 0;
            }
        } 
        // Si ce n'est pas une fin de ligne, on accumule dans le buffer s'il y a de la place
        else if (g_ui8BufferIndex < (UART_BUFFER_SIZE - 1)) {
            g_cRxBuffer[g_ui8BufferIndex++] = c;
        }
    }
}
