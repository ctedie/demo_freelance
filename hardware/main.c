#include <stdbool.h>
#include <stdint.h>
#include <stdio.h> // Requis pour sprintf

#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "modules/buzzer.h"
#include "modules/joystick.h"
#include "modules/led_rgb.h"
#include "modules/music.h"
#include "modules/uart_pc.h"
// Cette fonction est appelée automatiquement toutes les millisecondes
void SysTickIntHandler(void) {
  Music_Tick1ms(); // Fait avancer le séquenceur musical
}

int main(void) {
  Joystick_State_t currentJoystickState;
  char uartBuffer[64];

  // Horloge système à 120 MHz
  SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                      SYSCTL_CFG_VCO_480),
                     120000000);

  // Initialisations
  LED_RGB_Init();
  UART_PC_Init();
  Joystick_Init();
  Buzzer_Init();
  Music_Init();

  // --- CONFIGURATION DU SYSTICK (1 ms) ---
  // À 120 MHz, 1 ms = 120 000 cycles d'horloge
  SysTickPeriodSet(120000);
  SysTickIntEnable();
  SysTickEnable();

  // --- TEST : CRÉATION D'UNE PARTITION (Ex: Frère Jacques) ---
  const Note_t frere_jacques[] = {
      {NOTE_C4, DUREE_NOIRE}, {NOTE_D4, DUREE_NOIRE},   {NOTE_E4, DUREE_NOIRE},
      {NOTE_C4, DUREE_NOIRE - DUREE_TRIPLE_CROCHE}, {SILENCE, DUREE_TRIPLE_CROCHE}, {NOTE_C4, DUREE_NOIRE},   {NOTE_D4, DUREE_NOIRE},
      {NOTE_E4, DUREE_NOIRE}, {NOTE_C4, DUREE_NOIRE},   {NOTE_E4, DUREE_NOIRE},
      {NOTE_F4, DUREE_NOIRE}, {NOTE_G4, DUREE_BLANCHE}, {NOTE_E4, DUREE_NOIRE},
      {NOTE_F4, DUREE_NOIRE}, {NOTE_G4, DUREE_BLANCHE}};

  // On calcule automatiquement le nombre de notes dans le tableau
  uint16_t taille_morceau = sizeof(frere_jacques) / sizeof(Note_t);

  // On lance le morceau à 120 BPM (Le code n'est pas bloqué, la suite s'exécute
  // !)
  Music_PlayMelody(frere_jacques, taille_morceau, 120);
  while (1) {
    // 1. Lecture de l'état actuel (ADC + Boutons)
    Joystick_ReadState(&currentJoystickState);

    // 2. Formatage de la trame attendue par Node.js : "J:X,Y,Select,S1,S2\n"
    // Les booléens sont convertis en 1 (pressé) ou 0 (relâché)
    sprintf(uartBuffer, "J:%d,%d,%d,%d,%d\n", currentJoystickState.x,
            currentJoystickState.y, currentJoystickState.btnSelect ? 1 : 0,
            currentJoystickState.btnS1 ? 1 : 0,
            currentJoystickState.btnS2 ? 1 : 0);

    // 3. Envoi de la trame au PC via l'UART
    UART_PC_SendString(uartBuffer);

    // 4. Temporisation d'environ 50ms pour ne pas saturer le port série (20Hz)
    // 120 MHz / 3 = 40 000 000 de cycles par seconde. Pour 50ms -> 2 000 000
    SysCtlDelay(2000000);
  }
}
