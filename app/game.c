/**
 * @file    game.c
 * @brief   Implémentation de la logique de jeu.
 * @details Implémente les mécaniques principales du jeu, y compris
 * l'initialisation, la gestion d'état, la gestion de l'entrée tactile, le
 * mouvement du rectangle et la détection de collision.
 */

#include "game.h"

#include "stm32g4_gpio.h"

#include "rectangle.h"
#include "tft_ili9341/stm32g4_xpt2046.h"

/**
 * @brief   Initialise l'état du jeu.
 * @param   game - Pointeur sur la structure Game à initialiser.
 * @return  bool - Vrai si l'initialisation a réussi, faux sinon.
 * @note    Configure la position initiale du rectangle, la taille et crée la
 * pile de rectangles.
 */
bool game_init(Game *game) {
  if (game == NULL) {
    return false;
  }

  game->score = 0;
  game->toright = true;

  game->rect_y = 0;
  game->rect_length = RECTANGLE_DEFAULT_SIZE;

  game->rect_x = RECTANGLE_DEFAULT_HEIGHT * RECTANGLES_DEFAULT_COUNT;
  game->height = RECTANGLE_DEFAULT_HEIGHT;

  if (!rectangles_init(&game->rects)) {
    return false;
  }

  return true;
}

/**
 * @brief   Libère toutes les ressources allouées au jeu.
 * @param   game - Pointeur sur la structure Game à libérer.
 * @return  void
 * @note    Après l'appel de cette fonction, la structure de jeu n'est plus
 * valide jusqu'à la réinitialisation.
 */
void game_free(Game *game) {
  if (game == NULL) {
    return;
  }

  rectangles_free(&game->rects);

  game->score = 0;
  game->toright = false;

  game->rect_y = 0;
  game->rect_x = 0;
  game->rect_length = 0;
  game->height = 0;
}

/**
 * @brief   Réinitialise le jeu à son état initial.
 * @param   game - Pointeur sur la structure Game à réinitialiser.
 * @return  void
 * @note    Efface le score, réinitialise la position et la taille du rectangle
 * en mouvement, et réinitialise la pile de rectangles.
 */
void game_reset(Game *game) {
  if (game == NULL) {
    return;
  }

  game->score = 0;
  game->toright = true;

  game->rect_y = 0;
  game->rect_length = RECTANGLE_DEFAULT_SIZE;

  game->rect_x = RECTANGLE_DEFAULT_HEIGHT * RECTANGLES_DEFAULT_COUNT;
  game->height = RECTANGLE_DEFAULT_HEIGHT;

  rectangles_reset(&game->rects);
}

/**
 * @brief   Gère l'entrée tactile et la détection de collision.
 * @param   game - Pointeur sur la structure Game.
 * @param   touched - État tactile actuel.
 * @param   touched_before - État tactile précédent (mis à jour par cette
 * fonction).
 * @param   last_touch_tick - Horodatage du dernier événement tactile (mis à
 * jour par cette fonction).
 * @return  bool - Vrai si le jeu continue, faux si le joueur a échoué.
 * @details Détecte le toucher sur l'écran, applique un filtre de rebond de
 * 300ms, calcule l'intersection du rectangle en mouvement avec la pile et met à
 * jour l'état du jeu en cas de capture réussie.
 * @note    La fonction empêche le rebond tactile en suivant l'état tactile
 * précédent et en imposant un délai minimum de 300ms entre les touchers
 * consécutifs.
 */
static bool handle_touch(Game *game, bool touched, bool *touched_before,
                         uint32_t *last_touch_tick) {
  if (!game) {
    return false;
  }

  if (touched && !(*touched_before)) {
    uint32_t now = HAL_GetTick();

    if (now - *last_touch_tick < 300) {
      *touched_before = touched;
      return true;
    }

    *last_touch_tick = now;

    Rectangle moving = {game->rect_y,
                        (uint16_t)(game->rect_y + game->rect_length)};

    RectAddResult res = rectangles_push_if_intersect(&game->rects, &moving);

    if (res != RECT_ADD_SUCCESS) {
      return false;
    }

    const Rectangle *top = rectangles_top(&game->rects);
    if (top != NULL) {
      game->rect_y = top->y_0;
      game->rect_length = (uint16_t)(top->y_1 - top->y_0);
    }

    if (game->rect_x < RECTANGLES_TOTAL_HEIGHT) {
      game->rect_x += game->height;
    }

    game->score++;

    ILI9341_Fill(0x0010);
    rectangles_draw_all(&game->rects);
  }

  *touched_before = touched;
  return true;
}

/**
 * @brief   Met à jour la position du rectangle en mouvement.
 * @param   game - Pointeur sur la structure Game.
 * @return  void
 * @details Déplace le rectangle verticalement, rebondissant aux limites de
 * l'écran. La direction du mouvement est contrôlée par le drapeau toright.
 * @note    Implémente une oscillation lisse entre les bords de l'écran.
 */
static void update_movement(Game *game) {
  if (game->toright) {
    if (game->rect_y + game->rect_length >= 240) {
      game->toright = false;
      game->rect_y--;
    } else {
      game->rect_y++;
    }
  } else {
    if (game->rect_y == 0) {
      game->toright = true;
      game->rect_y++;
    } else {
      game->rect_y--;
    }
  }
}

/**
 * @brief   Efface le rectangle en mouvement de l'affichage.
 * @param   game - Pointeur sur la structure Game.
 * @return  void
 * @details Efface la zone du rectangle actuel avec la couleur de fond.
 *          Agrandit légèrement la zone d'effacement pour éviter les artefacts.
 */
static void hide_rect(Game *game) {
  ILI9341_DrawFilledRectangle(game->rect_x,
                              game->rect_y > 0 ? game->rect_y - 1 : 0,
                              game->rect_x + game->height,
                              game->rect_y + game->rect_length + 1, 0x0010);
}

/**
 * @brief   Dessine le rectangle en mouvement sur l'affichage.
 * @param   game - Pointeur sur la structure Game.
 * @return  void
 * @details Affiche le rectangle en mouvement actuel à sa position.
 */
static void render_rect(Game *game) {
  ILI9341_DrawFilledRectangle(
      game->rect_x, game->rect_y, game->rect_x + game->height,
      game->rect_y + game->rect_length, ILI9341_COLOR_CYAN);
}

/**
 * @brief   Exécute la boucle de jeu principale.
 * @param   game - Pointeur sur la structure Game.
 * @param   difficulty - Le niveau de difficulté sélectionné.
 * @return  void
 * @details Contrôle la boucle de jeu incluant :
 *          - Affichage des rectangles empilés
 *          - Mise à jour du mouvement du rectangle
 *          - Traitement de l'entrée tactile
 *          - Fréquence d'images adaptive selon la difficulté et le score
 * @note    La fréquence d'images diminue à mesure que le score augmente,
 * rendant le jeu plus difficile. La boucle continue jusqu'à ce que le joueur
 * échoue à capturer le rectangle.
 */
void game_play(Game *game, GameDifficulty difficulty) {
  if (game == NULL) {
    return;
  }

  ILI9341_Fill(0x0010);
  rectangles_draw_all(&game->rects);
  bool touched_before = false;
  uint32_t last_touch_tick = 0;

  uint32_t base_delay;
  if (difficulty == DIFFICULTY_EASY)
    base_delay = 12;
  else if (difficulty == DIFFICULTY_MEDIUM)
    base_delay = 7;
  else
    base_delay = 3;

  uint32_t min_delay;
  if (difficulty == DIFFICULTY_EASY)
    min_delay = 4;
  else if (difficulty == DIFFICULTY_MEDIUM)
    min_delay = 1;
  else
    min_delay = 0;

  for (;;) {
    int16_t tx, ty;
    bool touched = XPT2046_getMedianCoordinates(
        &tx, &ty, XPT2046_COORDINATE_SCREEN_RELATIVE);

    if (!handle_touch(game, touched, &touched_before, &last_touch_tick)) {
      return;
    }

    hide_rect(game);
    update_movement(game);
    render_rect(game);

    uint32_t reduction = game->score / 2;
    uint32_t delay = (base_delay > reduction + min_delay)
                         ? base_delay - reduction
                         : min_delay;

    if (delay > 0)
      HAL_Delay(delay);
  }
}
