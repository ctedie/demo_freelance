#include "uart_pc.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"

// Inclusion des autres modules pour l'actionnement
#include "led_rgb.h"
#include "buzzer.h"
#include "music.h"

// Taille étendue à 128 octets pour recevoir confortablement les trames de partitions
#define UART_BUFFER_SIZE 1024

static char g_cRxBuffer[UART_BUFFER_SIZE];
static uint8_t g_ui8BufferIndex = 0;

/**
 * @brief Initialisation de l'UART0 (115200 bauds, 8-N-1) avec interruptions
 */
void UART_PC_Init(void) {
    // 1. Activation des périphériques UART0 et GPIO Port A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

    // 2. Configuration des broches PA0 (RX) et PA1 (TX)
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // 3. Configuration du protocole (115200, 8-N-1) sur l'horloge système à 120 MHz
    UARTConfigSetExpClk(UART0_BASE, 120000000, 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    // 4. Configuration et activation des interruptions (Rx et Rx Timeout)
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    IntEnable(INT_UART0);
}

/**
 * @brief Routine d'interruption (ISR) de l'UART0 - Parseur de commandes non-bloquant
 */
void UART0IntHandler(void) {
    uint32_t ui32Status;

    // Récupération et acquittement du statut de l'interruption
    ui32Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Status);

    // Lecture de tous les caractères disponibles dans le FIFO matériel
    while(UARTCharsAvail(UART0_BASE)) {
        char c = (char)UARTCharGetNonBlocking(UART0_BASE);

        // Si on détecte une fin de trame
        if (c == '\n' || c == '\r') {
            if (g_ui8BufferIndex > 0) {
                // Ajout du caractère de fin de chaîne de caractères C
                g_cRxBuffer[g_ui8BufferIndex] = '\0';

                // ============================================================
                // SCÉNARIO 1 : RECEPTION D'UNE PARTITION ("M:BPM,Count,f1,d1...")
                // ============================================================
                if (g_cRxBuffer[0] == 'M' && g_cRxBuffer[1] == ':') {
                    char *ptr = &g_cRxBuffer[2];
                    
                    // Extraction du BPM
                    uint16_t ui16Bpm = 0;
                    while(*ptr >= '0' && *ptr <= '9') { ui16Bpm = ui16Bpm * 10 + (*ptr - '0'); ptr++; }
                    if(*ptr == ',') ptr++;

                    // Extraction du nombre de notes
                    uint8_t ui8NoteCount = 0;
                    while(*ptr >= '0' && *ptr <= '9') { ui8NoteCount = ui8NoteCount * 10 + (*ptr - '0'); ptr++; }
                    if(*ptr == ',') ptr++;

                    // Tableau statique local pour stocker la partition (Max 24 notes)
                    static Note_t s_IncomingMelody[128];
                    if(ui8NoteCount > 128) ui8NoteCount = 128;

                    // Parsing itératif des couples (Fréquence, Durée)
                    uint8_t index;
                    for(index = 0; index < ui8NoteCount; index++)
                    {
                        // Extraction Fréquence
                        uint32_t freq = 0;
                        while(*ptr >= '0' && *ptr <= '9') { freq = freq * 10 + (*ptr - '0'); ptr++; }
                        if(*ptr == ',') ptr++;

                        // Extraction Durée
                        uint8_t dur = 0;
                        while(*ptr >= '0' && *ptr <= '9') { dur = dur * 10 + (*ptr - '0'); ptr++; }
                        if(*ptr == ',') ptr++;

                        s_IncomingMelody[index].frequency = freq;
                        s_IncomingMelody[index].duration = dur;
                    }

                    // Envoi immédiat au séquenceur matériel autonome
                    Music_PlayMelody(s_IncomingMelody, ui8NoteCount, ui16Bpm);
                }
                
                // ============================================================
                // SCÉNARIO 2 : COMMANDE DU BUZZER EN DIRECT ("B:Frequence")
                // ============================================================
                else if (g_cRxBuffer[0] == 'B' && g_cRxBuffer[1] == ':') {
                    uint32_t ui32Frequency = 0;
                    char *ptr = &g_cRxBuffer[2];
                    while(*ptr >= '0' && *ptr <= '9') { ui32Frequency = ui32Frequency * 10 + (*ptr - '0'); ptr++; }
                    
                    if (ui32Frequency == 0) {
                        Buzzer_Stop();
                    } else {
                        // Si le séquenceur jouait une musique, on la coupe pour donner la priorité au live
                        Music_Stop(); 
                        Buzzer_PlayNote(ui32Frequency);
                    }
                } 
                
                // ============================================================
                // SCÉNARIO 3 : COMMANDE DE LA LED RGB ("R,G,B")
                // ============================================================
                else {
                    int r = 0, g = 0, b = 0;
                    char *ptr = g_cRxBuffer;
                    
                    while(*ptr >= '0' && *ptr <= '9') { r = r * 10 + (*ptr - '0'); ptr++; } if(*ptr == ',') ptr++;
                    while(*ptr >= '0' && *ptr <= '9') { g = g * 10 + (*ptr - '0'); ptr++; } if(*ptr == ',') ptr++;
                    while(*ptr >= '0' && *ptr <= '9') { b = b * 10 + (*ptr - '0'); ptr++; }
                    
                    LED_RGB_SetIntensity(r, g, b);
                }

                // Réinitialisation de l'index pour la prochaine trame
                g_ui8BufferIndex = 0;
            }
        } 
        // Accumulation dans le tampon tant qu'il reste de la place
        else if (g_ui8BufferIndex < (UART_BUFFER_SIZE - 1)) {
            g_cRxBuffer[g_ui8BufferIndex++] = c;
        }
    }
}

/**
 * @brief Envoie une chaîne de caractères (string) au PC via UART0
 * @param str Pointeur vers la chaîne de caractères à envoyer
 */
void UART_PC_SendString(const char *str) {
    // Tant qu'on n'est pas arrivé au caractère de fin de chaîne '\0'
    while(*str != '\0') {
        // Envoi du caractère actuel (bloquant pour s'assurer que le FIFO ne déborde pas)
        UARTCharPut(UART0_BASE, *str);
        str++; // On passe au caractère suivant
    }
}
