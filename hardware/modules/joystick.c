#include "joystick.h"
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"  // On réintègre la bibliothèque ADC de TivaWare

void Joystick_Init(void) {
    // 1. Activation des horloges de TOUS les périphériques nécessaires
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Port E pour l'ADC (PE4 = X, PE3 = Y)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); // Port C pour le bouton Joystick (PC6)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL); // Port L pour Btn 1 et Btn 2 (PL1, PL2)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);  // Le périphérique ADC 0

    // 2. Attente stricte de la mise à disposition de TOUS les périphériques
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)) 
    {
        // On attend que les horloges soient verrouillées
    }

    // LE SAUVEGARDE : Délai matériel pour stabiliser les bus périphériques à 120 MHz
    SysCtlDelay(30);

    // 3. Configuration des Boutons Numériques en entrées avec Pull-Up
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_1 | GPIO_PIN_2);
    
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTL_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // 4. Configuration des Pins de l'Axe X (PE4) et Axe Y (PE3) en mode Analogique (ADC)
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_4);
    
    // Configuration du Séquenceur 1 de l'ADC0 (Déclenchement logiciel par le processeur)
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    
    // Étape 0 du séquenceur : Échantillonner l'axe X (PE4 relié au canal matériel AIN9)
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH9);
    
    // Étape 1 du séquenceur : Échantillonner l'axe Y (PE3 relié au canal matériel AIN0)
    // On lève le drapeau de fin de séquence (ADC_CTL_END) et de fin de conversion (ADC_CTL_IE)
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    
    // Activation effective du séquenceur 1
    ADCSequenceEnable(ADC0_BASE, 1);
    
    // Nettoyage initial du drapeau d'interruption (juste par sécurité)
    ADCIntClear(ADC0_BASE, 1);
}

void Joystick_ReadState(Joystick_State_t* state) {
    uint32_t adcValues[2];

    // 1. Déclenchement manuel de la conversion du séquenceur 1
    ADCProcessorTrigger(ADC0_BASE, 1);
    
    // 2. Attente de la fin de conversion par scrutation (Pas d'interruption CPU !)
    while(!ADCIntStatus(ADC0_BASE, 1, false)) {
        // On attend que les deux étapes soient terminées
    }
    
    // Effacement du drapeau pour la prochaine lecture
    ADCIntClear(ADC0_BASE, 1);
    
    // 3. Récupération des données brutes (X est l'étape 0 -> index [0], Y est l'étape 1 -> index [1])
    ADCSequenceDataGet(ADC0_BASE, 1, adcValues);
    state->x = adcValues[0];
    state->y = adcValues[1];
    
    // 4. Lecture des boutons (0 = Pressé à cause de la configuration en Pull-Up)
    state->btnSelect = (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6) == 0);
    state->btnS1     = (GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1) == 0);
    state->btnS2     = (GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_2) == 0);
}
