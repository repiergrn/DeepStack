/**
 * @file    main.c
 * @brief   Point d'entrée principal de l'application Stack.
 * @details Gère la machine à états principale qui contrôle le flux du jeu :
 *          initialisation, affichage du menu, exécution du jeu et affichage du
 * score.
 */

#include "board.h"
#include "game.h"
#include "menu.h"
#include "rectangle.h"

#include "stm32g4_gpio.h"

#include <stdbool.h>

/**
 * @brief   Point d'entrée principal de l'application Stack.
 * @details Initialise la carte et le jeu, puis entre dans la boucle de la
 * machine à états qui gère les transitions entre l'initialisation, le menu, le
 * jeu et l'affichage du score.
 * @return  int - Cette fonction ne revient jamais (boucle infinie).
 * @note    Le cycle de l'application :
 *          - STATE_INIT : Initialiser la carte et le jeu
 *          - STATE_MENU : Afficher le menu de sélection de difficulté
 *          - STATE_GAME : Exécuter la logique du jeu
 *          - STATE_SCORE : Afficher le score final et les meilleurs scores
 *          Le cycle revient ensuite à STATE_MENU pour un nouveau jeu.
 */
int main(void) {
  GameState state = STATE_INIT;
  Game game;
  GameDifficulty difficulty = DIFFICULTY_MEDIUM;

  for (;;) {
    switch (state) {
    case STATE_INIT:
      board_init();
      if (!game_init(&game)) {
        while (1)
          ;
      }
      state = STATE_MENU;
      break;
    case STATE_MENU:
      difficulty = show_game_menu();
      state = STATE_GAME;
      break;
    case STATE_GAME:
      game_play(&game, difficulty);
      state = STATE_SCORE;
      break;
    case STATE_SCORE:
      show_score_screen(&game, difficulty);
      game_reset(&game);
      state = STATE_MENU;
      break;
    }
  }
}
