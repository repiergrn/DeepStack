/**
 * @file    menu.h
 * @brief   Interface du menu et de l'interface utilisateur.
 * @details Fournit les fonctions pour afficher le menu du jeu, la sélection de
 * difficulté et l'écran d'affichage du score.
 */

#ifndef MENU_H_
#define MENU_H_

#include "game.h"

/**
 * @brief   Affiche le score final et les meilleurs scores sur l'écran.
 * @param   game - Pointeur sur la structure Game contenant le score actuel.
 * @param   difficulty - Le niveau de difficulté qui vient d'être complété.
 * @return  void
 * @details Affiche l'écran "GAME OVER" avec le score actuel, le meilleur score
 * pour la difficulté sélectionnée et sauvegarde les nouveaux meilleurs scores
 * dans la mémoire flash. Attend l'entrée tactile de l'utilisateur avant de
 * revenir.
 * @note    Compare également et met à jour le meilleur score dans la mémoire
 * flash si applicable.
 */
void show_score_screen(Game *game, GameDifficulty difficulty);

/**
 * @brief   Affiche le menu de sélection de difficulté.
 * @return  GameDifficulty - Le niveau de difficulté sélectionné.
 * @details Affiche le menu interactif avec trois boutons de difficulté (Facile,
 * Moyen, Difficile) et un bouton PLAY. L'utilisateur sélectionne la difficulté
 * en appuyant sur les boutons et confirme en appuyant sur PLAY.
 * @note    Le menu affiche des blocs de différentes tailles pour représenter
 * visuellement les niveaux de difficulté et la progression du jeu.
 */
GameDifficulty show_game_menu(void);

#endif /* MENU_H_ */
