#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

// Définition de la période pour une fréquence PWM de 10 kHz
// Horloge PWM = Horloge système (120 MHz) / 1 = 120 000 000 Hz
// Période = 120 000 000 / 10 000 = 12 000 cycles
#define PWM_PERIOD 12000

void ConfigureSystem(void) {
    // Configuration de l'horloge système à 120 MHz
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // Activation des périphériques indispensables
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Pour l'UART0 (PA0, PA1)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Pour RED (PF2) et GREEN (PF3)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG); // Pour BLUE (PG0)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);  // Module PWM0 matériel
}

void ConfigureUART(void) {
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, 120000000, 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

void ConfigureHardwarePWM(void) {
    // 1. Configurer les fonctions alternatives PWM sur les pins validées
    GPIOPinConfigure(GPIO_PF2_M0PWM2); // ROUGE
    GPIOPinConfigure(GPIO_PF3_M0PWM3); // VERT
    GPIOPinConfigure(GPIO_PG0_M0PWM4); // BLEU (Corrigé en M0PWM4)

    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_0);

    // 2. Horloge du module PWM (120 MHz direct, pas de diviseur)
    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_1);

    // 3. Configurer les Générateurs du Module 0
    // PF2/PF3 utilisent le Générateur 1. PG0 utilise le Générateur 2 (lié à M0PWM4).
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // 4. Définir la période (Fréquence de 10 kHz)
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, PWM_PERIOD);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, PWM_PERIOD);

    // 5. Initialiser le rapport cyclique à 0% au démarrage (LED éteintes)
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 0); // Rouge (M0PWM2)
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, 0); // Vert  (M0PWM3)
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, 0); // Bleu  (M0PWM4 - Corrigé)

    // 6. Activer les générateurs 1 et 2
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);
    PWMGenEnable(PWM0_BASE, PWM_GEN_2);

    // 7. Activer les sorties physiques PWM vers les pins
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT | PWM_OUT_3_BIT | PWM_OUT_4_BIT, true);
}

// Fonction pour appliquer l'intensité (0 à 255) sur chaque canal
void SetRGBIntensity(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t widthR = (r * PWM_PERIOD) / 255;
    uint32_t widthG = (g * PWM_PERIOD) / 255;
    uint32_t widthB = (b * PWM_PERIOD) / 255;

    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, widthR);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, widthG);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, widthB); // Corrigé en PWM_OUT_4
}

int main(void) {
    ConfigureSystem();
    ConfigureUART();
    ConfigureHardwarePWM();

    // Petit test visuel : blanc faible intensité au démarrage
    SetRGBIntensity(30, 30, 30);

    while (1) {
        // En attente de la trame UART
    }
}
