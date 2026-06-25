#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#define BUFFER_SIZE 32

// ==========================================
// PROTOTYPES DES FONCTIONS
// ==========================================
void ConfigureSystem(void);
void ConfigureUART(void);
void ConfigureSoftwarePWM(void);
void SetRGBIntensity(uint8_t r, uint8_t g, uint8_t b);
void ProcessCommand(char* cmd);
void UART0IntHandler(void);
void Timer0AIntHandler(void);

// ==========================================
// VARIABLES GLOBALES
// ==========================================
char rxBuffer[BUFFER_SIZE];
volatile uint16_t rxIndex = 0;
volatile bool commandReceived = false;

// Variables pour le PWM Logiciel (plage 0 à 255)
volatile uint8_t pwmCounter = 0;
volatile uint8_t intensityR = 0;
volatile uint8_t intensityG = 0;
volatile uint8_t intensityB = 0;

// ==========================================
// FONCTION PRINCIPALE
// ==========================================
int main(void) {
    ConfigureSystem();
    ConfigureSoftwarePWM();
    ConfigureUART(); // Initialisé en dernier

    // Petit test visuel : lueur blanche au démarrage
    SetRGBIntensity(10, 10, 10);

    while (1) {
        if (commandReceived) {
            ProcessCommand(rxBuffer);
            
            rxIndex = 0;
            commandReceived = false;
        }
    }
}

// ==========================================
// IMPLÉMENTATION DES FONCTIONS
// ==========================================

void ConfigureSystem(void) {
    // Horloge système à 120 MHz
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // Activation des périphériques
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // UART0 pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // RED (PF2), GREEN (PF3)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG); // BLUE (PG0)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Timer pour le PWM logiciel
}

void ConfigureUART(void) {
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, 120000000, 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    IntRegister(INT_UART0, UART0IntHandler);
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

void ConfigureSoftwarePWM(void) {
    // 1. Configurer les pins des LED en sorties GPIO normales
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);

    // Éteindre les pins au départ
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0);
    GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0);

    // 2. Configurer le Timer0A pour générer des interruptions périodiques
    // Fréquence voulue : 256 étapes * 100 Hz (fréquence PWM globale) = ~25600 Hz
    // Période du Timer = 120 000 000 / 25600 = 4687 cycles
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, 4687);

    // 3. Enregistrer et activer l'interruption du Timer
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    
    // Activation globale des interruptions
    IntMasterEnable();

    // Démarrer le Timer
    TimerEnable(TIMER0_BASE, TIMER_A);
}

void SetRGBIntensity(uint8_t r, uint8_t g, uint8_t b) {
    // Mise à jour atomique des variables lues par l'interruption
    intensityR = r;
    intensityG = g;
    intensityB = b;
}

void ProcessCommand(char* cmd) {
    int r = 0, g = 0, b = 0;
    
    // Décodage de la chaîne "R,G,B"
    if (sscanf(cmd, "%d,%d,%d", &r, &g, &b) == 3) {
        if (r < 0) r = 0; if (r > 255) r = 255;
        if (g < 0) g = 0; if (g > 255) g = 255;
        if (b < 0) b = 0; if (b > 255) b = 255;

        SetRGBIntensity((uint8_t)r, (uint8_t)g, (uint8_t)b);
    }
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

    // Incrémentation du compteur de cycle (0 à 255 automatique car uint8_t)
    pwmCounter++;
}

void UART0IntHandler(void) {
    uint32_t ui32Status;

    ui32Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Status);

    while(UARTCharsAvail(UART0_BASE)) {
        char c = UARTCharGetNonBlocking(UART0_BASE);

        // Écho
        UARTCharPutNonBlocking(UART0_BASE, c);

        if (c == '\n' || c == '\r') {
            if (rxIndex > 0) {
                rxBuffer[rxIndex] = '\0';
                commandReceived = true;
            }
        } 
        else if (rxIndex < (BUFFER_SIZE - 1)) {
            rxBuffer[rxIndex++] = c;
        }
    }
}
