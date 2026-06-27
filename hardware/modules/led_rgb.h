#ifndef MODULES_LED_RGB_H_
#define MODULES_LED_RGB_H_

#include <stdint.h>

/**
 * @brief Active les périphériques GPIO et configure le Timer0A pour le PWM Logiciel
 */
void LED_RGB_Init(void);

/**
 * @brief Met à jour les intensités cibles lues par l'interruption
 * @param r Intensité Rouge (0 à 255)
 * @param g Intensité Verte (0 à 255)
 * @param b Intensité Bleue (0 à 255)
 */
void LED_RGB_SetIntensity(uint8_t r, uint8_t g, uint8_t b);

#endif /* MODULES_LED_RGB_H_ */
