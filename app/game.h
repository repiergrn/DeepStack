/**
 * @file    game.h
 * @brief   Interface de logique de jeu.
 * @details Définit les structures et les fonctions pour gérer l'état du jeu
 * Stack, y compris l'initialisation du jeu, la réinitialisation et la boucle de
 * jeu principale.
 */

#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>
#include <stdint.h>

#include "rectangle.h"

/**
 * @enum    GameState
 * @brief   États de la machine à états de l'application.
 */
typedef enum {
  /** @brief État d'initialisation. */
  STATE_INIT,
  /** @brief État d'affichage du menu et de sélection de difficulté. */
  STATE_MENU,
  /** @brief État de jeu actif. */
  STATE_GAME,
  /** @brief État d'affichage du score. */
  STATE_SCORE
} GameState;

/**
 * @enum    GameDifficulty
 * @brief   Niveaux de difficulté du jeu affectant la vitesse de jeu.
 */
typedef enum {
  /** @brief Difficulté facile - jeu plus lent. */
  DIFFICULTY_EASY,
  /** @brief Difficulté moyenne - vitesse modérée. */
  DIFFICULTY_MEDIUM,
  /** @brief Difficulté difficile - jeu plus rapide. */
  DIFFICULTY_HARD
} GameDifficulty;

/**
 * @struct  Game
 * @brief   Représente l'état complet du jeu.
 * @details Contient toutes les informations nécessaires pour exécuter le jeu,
 *          y compris la position du rectangle en mouvement, l'orientation,
 *          la pile des rectangles capturés et le score actuel.
 */
typedef struct {
  /** @brief Coordonnée Y du rectangle en mouvement. */
  uint16_t rect_y;
  /** @brief Coordonnée X du rectangle en mouvement. */
  uint16_t rect_x;
  /** @brief Largeur (longueur) du rectangle en mouvement. */
  uint16_t rect_length;

  /** @brief Hauteur des rectangles empilés. */
  uint16_t height;

  /** @brief Score actuel du jeu (nombre d'empilements réussis). */
  uint16_t score;

  /** @brief Drapeau de direction du mouvement (true = se déplace à droite/bas).
   */
  bool toright;

  /** @brief Pile des rectangles capturés. */
  Rectangles rects;
} Game;

/**
 * @brief   Initialise l'état du jeu.
 * @param   game - Pointeur sur la structure Game à initialiser.
 * @return  bool - Vrai si l'initialisation a réussi, faux sinon.
 * @note    Doit être appelée avant toute autre fonction de jeu.
 *          Définit les valeurs initiales pour la position, la taille et crée la
 * pile de rectangles.
 */
bool game_init(Game *game);

/**
 * @brief   Libère toutes les ressources allouées au jeu.
 * @param   game - Pointeur sur la structure Game à libérer.
 * @return  void
 * @note    Après l'appel de cette fonction, la structure de jeu n'est plus
 * valide tant qu'elle n'est pas réinitialisée.
 */
void game_free(Game *game);

/**
 * @brief   Réinitialise le jeu à son état initial.
 * @param   game - Pointeur sur la structure Game à réinitialiser.
 * @return  void
 * @note    Efface le score, réinitialise la position et la taille du rectangle
 * en mouvement, et réinitialise la pile de rectangles. N'utilise pas la mémoire
 * allouée ; utilisez game_free() pour cela.
 */
void game_reset(Game *game);

/**
 * @brief   Exécute la boucle de jeu principale.
 * @param   game - Pointeur sur la structure Game.
 * @param   difficulty - Le niveau de difficulté sélectionné.
 * @return  void
 * @note    Cette fonction exécute le jeu jusqu'à ce que le joueur échoue à
 * capturer le rectangle. Gère l'entrée tactile, le mouvement du rectangle, le
 * rendu et la détection de collision. La difficulté affecte la vitesse du
 * mouvement du rectangle.
 */
void game_play(Game *game, GameDifficulty difficulty);

#endif /* GAME_H_ */
