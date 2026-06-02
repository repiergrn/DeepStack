/**
 * @file    board.c
 * @brief   Implémentation d'initialisation de la carte.
 * @details Implémente l'initialisation du matériel pour le microcontrôleur
 * STM32G431 y compris les périphériques GPIO, UART et affichage.
 */

#include "board.h"

#include "stm32g4_gpio.h"
#include "stm32g4_sys.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_xpt2046.h"

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
void board_init(void) {
  HAL_Init();

  BSP_UART_init(UART2_ID, 115200);

  BSP_SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

  BSP_GPIO_enable();
  BSP_GPIO_pin_config(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,
                      GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_NO_AF);

  XPT2046_init();

  ILI9341_Init();
  ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
  ILI9341_DisplayOn();
}
