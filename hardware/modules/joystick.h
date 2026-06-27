#ifndef MODULES_JOYSTICK_H_
#define MODULES_JOYSTICK_H_

#include <stdint.h>
#include <stdbool.h>

// Structure propre pour encapsuler toutes les données du Joystick et des boutons
typedef struct {
    uint32_t x;       // Valeur brute Axe X (0 à 4095)
    uint32_t y;       // Valeur brute Axe Y (0 à 4095)
    bool btnSelect;   // True si le stick est pressé
    bool btnS1;       // True si le bouton S1 est pressé
    bool btnS2;       // True si le bouton S2 est pressé
} Joystick_State_t;

/**
 * @brief Initialise les pins GPIO (boutons) et l'ADC0 (Séquenceur 1) pour le Joystick
 */
void Joystick_Init(void);

/**
 * @brief Lit l'état actuel des axes analogiques et des boutons numériques
 * @param state Pointeur vers la structure qui recevra les mesures
 */
void Joystick_ReadState(Joystick_State_t* state);

#endif /* MODULES_JOYSTICK_H_ */
