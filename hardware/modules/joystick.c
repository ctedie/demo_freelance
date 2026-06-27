#include "joystick.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_adc.h"       // <--- TRÈS IMPORTANT : contient les définitions des canaux et registres bas niveau
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"    // <--- Contient les prototypes comme ADCSequenceConfigure

void Joystick_Init(void) {
    // 1. Activation des périphériques (Ports E, C, L et l'ADC0)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Pour l'ADC (PE4, PE3)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); // Pour Btn Joystick (PC6)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL); // Pour Btn 1 (PL1) et Btn 2 (PL2)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)) {}

    // 2. Configuration des Boutons en entrées numériques avec résistance de Pull-Up (WPU)
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_1 | GPIO_PIN_2);
    
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTL_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // 3. Configuration des broches ADC en entrées analogiques (PE4 = AIN9, PE3 = AIN0)
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_4);
    
    // Configuration du Séquenceur 1 de l'ADC0
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    
    // Étape 0 : Échantillonner l'axe X (PE4 -> AIN9)
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH9);
    // Étape 1 : Échantillonner l'axe Y (PE3 -> AIN0) + Interruption + Fin de séquence
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    
    // Activation du séquenceur
    ADCSequenceEnable(ADC0_BASE, 1);
    ADCIntClear(ADC0_BASE, 1);
}

void Joystick_ReadState(Joystick_State_t* state) {
    uint32_t adcValues[2];

    // 1. Déclenchement de la conversion
    ADCProcessorTrigger(ADC0_BASE, 1);
    
    // Attente de la fin de la conversion
    while(!ADCIntStatus(ADC0_BASE, 1, false)) {}
    ADCIntClear(ADC0_BASE, 1);
    
    // Récupération des valeurs (X = [0] correspondant à l'étape 0, Y = [1] pour l'étape 1)
    ADCSequenceDataGet(ADC0_BASE, 1, adcValues);
    state->x = adcValues[0];
    state->y = adcValues[1];
    
    // 2. Lecture de l'état des boutons (Inversion car Pull-Up : 0 = Pressé)
    state->btnSelect = (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6) == 0);
    state->btnS1     = (GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1) == 0);
    state->btnS2     = (GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_2) == 0);
}
