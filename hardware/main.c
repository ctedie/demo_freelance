#include <stdint.h>
#include <stdbool.h>
#include <stdio.h> // Requis pour sprintf
#include "driverlib/sysctl.h"
#include "modules/led_rgb.h"
#include "modules/uart_pc.h"
#include "modules/joystick.h"
#include "modules/buzzer.h"

int main(void) {
    Joystick_State_t currentJoystickState;
    char uartBuffer[64];

    // Horloge système à 120 MHz
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // Initialisations
    LED_RGB_Init();   
    UART_PC_Init();   
    Joystick_Init();  
    Buzzer_Init();
    
    while (1) {
        // 1. Lecture de l'état actuel (ADC + Boutons)
        Joystick_ReadState(&currentJoystickState);

        // 2. Formatage de la trame attendue par Node.js : "J:X,Y,Select,S1,S2\n"
        // Les booléens sont convertis en 1 (pressé) ou 0 (relâché)
        sprintf(uartBuffer, "J:%d,%d,%d,%d,%d\n",
                currentJoystickState.x,
                currentJoystickState.y,
                currentJoystickState.btnSelect ? 1 : 0,
                currentJoystickState.btnS1 ? 1 : 0,
                currentJoystickState.btnS2 ? 1 : 0);

        // 3. Envoi de la trame au PC via l'UART
        UART_PC_SendString(uartBuffer);

        // 4. Temporisation d'environ 50ms pour ne pas saturer le port série (20Hz)
        // 120 MHz / 3 = 40 000 000 de cycles par seconde. Pour 50ms -> 2 000 000
        SysCtlDelay(2000000);
    }
}
