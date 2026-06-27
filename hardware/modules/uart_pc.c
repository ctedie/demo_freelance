#include "uart_pc.h"
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"

#define BUFFER_SIZE 32

// Variables privées au module pour la réception UART
static char rxBuffer[BUFFER_SIZE];
static volatile uint16_t rxIndex = 0;
static volatile bool commandReceived = false;

// Prototype privé de la routine d'interruption
static void UART0IntHandler(void);

void UART_PC_Init(void) {
    // 1. Activation des périphériques indispensables
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

    // 2. Configuration des broches PA0 (RX) et PA1 (TX)
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // 3. Configuration du baudrate (Fixé à 120 MHz comme défini dans le système)
    UARTConfigSetExpClk(UART0_BASE, 120000000, 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // 4. Enregistrement et activation des interruptions
    IntRegister(INT_UART0, UART0IntHandler);
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

bool UART_PC_CommandAvailable(void) {
    return commandReceived;
}

void UART_PC_GetCommand(char* outBuffer) {
    // Copie sécurisée du buffer interne vers l'extérieur
    int i = 0;
    while (rxBuffer[i] != '\0' && i < (BUFFER_SIZE - 1)) {
        outBuffer[i] = rxBuffer[i];
        i++;
    }
    outBuffer[i] = '\0';

    // Reset des drapeaux pour la prochaine commande
    rxIndex = 0;
    commandReceived = false;
}

void UART_PC_SendString(const char *str) {
    while(*str) {
        UARTCharPut(UART0_BASE, *str++);
    }
}

// Routine d'interruption de l'UART0
static void UART0IntHandler(void) {
    uint32_t ui32Status;

    ui32Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Status);

    while(UARTCharsAvail(UART0_BASE)) {
        char c = UARTCharGetNonBlocking(UART0_BASE);

        // Écho vers le PC
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
