/**
 * @file    board.h
 * @brief   Interface d'initialisation de la carte.
 * @details Fournit l'interface pour initialiser la carte microcontrôleur,
 *          y compris les périphériques GPIO, UART et affichage.
 */

#ifndef BOARD_H_
#define BOARD_H_

/**
 * @brief   Initialise le matériel de la carte.
 * @details Effectue toutes les étapes d'initialisation nécessaires :
 *          - Initialisation HAL
 *          - Configuration UART2 (115200 bauds)
 *          - Configuration GPIO pour LED
 *          - Initialisation de l'afficheur TFT ILI9341
 *          - Initialisation du contrôleur tactile XPT2046
 * @return  void
 * @note    Doit être appelée une seule fois au démarrage du système avant toute
 * autre opération.
 */
void board_init(void);

#endif /* BOARD_H_ */
