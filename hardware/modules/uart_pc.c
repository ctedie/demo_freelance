#include "uart_pc.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>   
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"

// Inclusion des modules pour piloter les actionneurs
#include "led_rgb.h" 
#include "buzzer.h"

// --- CONFIGURATION DU BUFFER SÉRIE SÉCURISÉ ---
#define UART_BUFFER_SIZE 32

// Alignement sur 4 octets (32 bits) pour garantir un accès mémoire optimal par le CPU
#pragma DATA_ALIGN(g_cRxBuffer, 4)
static char g_cRxBuffer[UART_BUFFER_SIZE];
static uint8_t g_ui8BufferIndex = 0;

/**
 * @brief Initialisation de l'UART0 pour la communication avec le PC (115200 bauds)
 */
void UART_PC_Init(void) {
    // 1. Activation des horloges pour l'UART0 et le Port A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

    SysCtlDelay(10); // Petit délai de stabilisation mécanique du bus

    // 2. Configuration des broches PA0 (RX) et PA1 (TX)
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // 3. Liaison série à 115200 bauds basée sur l'horloge système de 120 MHz
    UARTConfigSetExpClk(UART0_BASE, 120000000, 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    // 4. Configuration et activation des interruptions matérielles
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    IntEnable(INT_UART0);

    // Initialisation propre de la mémoire du tampon
    memset(g_cRxBuffer, 0, UART_BUFFER_SIZE);
    g_ui8BufferIndex = 0;
}

/**
 * @brief Envoi d'une chaîne de caractères textuelle vers le PC
 */
void UART_PC_SendString(const char *str) {
    while(*str) {
        UARTCharPut(UART0_BASE, *str++);
    }
}

/**
 * @brief Routine d'interruption (ISR) de l'UART0 - Gestionnaire de paquets
 */
void UART0IntHandler(void) {
    uint32_t ui32Status;

    // Récupération et acquittement des drapeaux d'interruption UART
    ui32Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Status);

    // Vidage de la FIFO matérielle de réception
    while(UARTCharsAvail(UART0_BASE)) {
        char c = (char)UARTCharGetNonBlocking(UART0_BASE);

        // Détection de fin de ligne (Validation de la trame reçue)
        if (c == '\n' || c == '\r') {
            if (g_ui8BufferIndex > 0) {
                g_cRxBuffer[g_ui8BufferIndex] = '\0'; // Fermeture de la string C

                // ============================================================
                // SCÉNARIO 1 : COMMANDE DU BUZZER PIANO - Format "B:Frequence"
                // ============================================================
                if (g_cRxBuffer[0] == 'B' && g_cRxBuffer[1] == ':') {
                    uint32_t ui32Frequency = 0;
                    char *ptr = &g_cRxBuffer[2];

                    // Extraction itérative rapide de l'entier (Fréquence en Hz)
                    while(*ptr >= '0' && *ptr <= '9') {
                        ui32Frequency = ui32Frequency * 10 + (*ptr - '0');
                        ptr++;
                    }

                    // Application immédiate de l'état audio
                    if (ui32Frequency == 0) {
                        Buzzer_Stop();
                    } else {
                        Buzzer_PlayNote(ui32Frequency);
                    }
                } 
                // ============================================================
                // SCÉNARIO 2 : COMMANDE DE LA LED RGB - Format standard "R,G,B"
                // ============================================================
                else {
                    int r = 0, g = 0, b = 0;
                    char *ptr = g_cRxBuffer;

                    // Extraction de la composante Rouge
                    while(*ptr >= '0' && *ptr <= '9') { r = r * 10 + (*ptr - '0'); ptr++; }
                    if(*ptr == ',') ptr++; // Saut de la virgule séparatrice
                    
                    // Extraction de la composante Verte
                    while(*ptr >= '0' && *ptr <= '9') { g = g * 10 + (*ptr - '0'); ptr++; }
                    if(*ptr == ',') ptr++; // Saut de la virgule séparatrice
                    
                    // Extraction de la composante Bleue
                    while(*ptr >= '0' && *ptr <= '9') { b = b * 10 + (*ptr - '0'); ptr++; }

                    // Application des cycles de travail (PWM) pour la LED RGB
                    LED_RGB_SetIntensity(r, g, b);
                }

                // Remise à zéro de l'index pour charger la trame suivante
                g_ui8BufferIndex = 0;
            }
        } 
        // Accumulation des caractères entrants tant que le buffer n'est pas plein
        else if (g_ui8BufferIndex < (UART_BUFFER_SIZE - 1)) {
            g_cRxBuffer[g_ui8BufferIndex++] = c;
        }
    }
}
