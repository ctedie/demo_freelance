#ifndef MODULES_UART_PC_H_
#define MODULES_UART_PC_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialise les broches PA0/PA1 et le périphérique UART0
 */
void UART_PC_Init(void);

/**
 * @brief Vérifie si une commande complète (terminée par \n ou \r) a été reçue
 * @return true si une commande est prête à être lue, false sinon
 */
bool UART_PC_CommandAvailable(void);

/**
 * @brief Récupère la commande reçue et réinitialise le buffer de réception
 * @param outBuffer Pointeur vers le tableau de char qui va recevoir la chaîne
 */
void UART_PC_GetCommand(char* outBuffer);

/**
 * @brief Envoie une chaîne de caractères brute au PC
 * @param str Chaîne de caractères se terminant par '\0'
 */
void UART_PC_SendString(const char *str);

#endif /* MODULES_UART_PC_H_ */
