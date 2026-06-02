/**
 * @file    rectangle.c
 * @brief   Implémentation de la gestion des rectangles et de la pile.
 * @details Implémente la gestion dynamique de la pile pour les rectangles en
 * utilisant une sémantique de tampon circulaire, la détection de collision et
 * le rendu.
 */

#include "rectangle.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief   Convertit un index logique en index physique dans le tampon
 * circulaire.
 * @param   rects - Pointeur sur la structure Rectangles.
 * @param   logical_index - L'index logique (0 = le plus ancien).
 * @return  uint16_t - L'index physique dans le tableau de données.
 * @note    Fonction d'assistance interne pour la gestion du tampon circulaire.
 */
static uint16_t rectangles_index(const Rectangles *rects,
                                 uint16_t logical_index) {
  return (uint16_t)((rects->begin + logical_index) % rects->capacity);
}

/**
 * @brief   Initialise la pile de rectangles.
 * @param   rects - Pointeur sur la structure Rectangles à initialiser.
 * @return  bool - Vrai si l'initialisation a réussi, faux sinon.
 * @details Alloue de la mémoire pour la pile de rectangles et crée l'ensemble
 * par défaut des rectangles initiaux.
 * @note    Doit être appelée avant toute autre fonction de rectangles.
 */
bool rectangles_init(Rectangles *rects) {
  if (rects == NULL) {
    return false;
  }

  Rectangle *data = malloc(RECTANGLES_DEFAULT_CAPACITY * sizeof(*data));
  if (!data) {
    return false;
  }

  *rects = (Rectangles){.capacity = RECTANGLES_DEFAULT_CAPACITY,
                        .size = 0,
                        .begin = 0,
                        .data = data};

  for (uint16_t i = 0; i < RECTANGLES_DEFAULT_COUNT; i++) {
    rectangles_push(rects,
                    (Rectangle){RECTANGLE_DEFAULT_Y0,
                                RECTANGLE_DEFAULT_Y0 + RECTANGLE_DEFAULT_SIZE});
  }

  return true;
}

/**
 * @brief   Libère toutes les ressources allouées à la pile de rectangles.
 * @param   rects - Pointeur sur la structure Rectangles à libérer.
 * @return  void
 * @note    Après l'appel de cette fonction, la structure de rectangles n'est
 * plus valide jusqu'à la réinitialisation.
 */
void rectangles_free(Rectangles *rects) {
  if (rects == NULL) {
    return;
  }

  free(rects->data);

  rects->data = NULL;
  rects->capacity = 0;
  rects->size = 0;
  rects->begin = 0;
}

/**
 * @brief   Réinitialise la pile de rectangles à son état initial.
 * @param   rects - Pointeur sur la structure Rectangles à réinitialiser.
 * @return  void
 * @details Efface la pile et recrée l'ensemble par défaut des rectangles
 * initiaux. N'utilise pas la mémoire allouée.
 */
void rectangles_reset(Rectangles *rects) {
  if (rects == NULL) {
    return;
  }

  rects->size = 0;
  rects->begin = 0;

  for (uint16_t i = 0; i < RECTANGLES_DEFAULT_COUNT; i++) {
    rectangles_push(rects,
                    (Rectangle){RECTANGLE_DEFAULT_Y0,
                                RECTANGLE_DEFAULT_Y0 + RECTANGLE_DEFAULT_SIZE});
  }
}

/**
 * @brief   Ajoute un rectangle à la pile.
 * @param   rects - Pointeur sur la structure Rectangles.
 * @param   rect - Le rectangle à ajouter.
 * @return  bool - Vrai si le rectangle a été ajouté, faux en cas d'erreur.
 * @details Gère la sémantique du tampon circulaire. Quand la pile est pleine,
 *          les anciens rectangles sont écrasés.
 */
bool rectangles_push(Rectangles *rects, Rectangle rect) {
  if (rects == NULL || rects->data == NULL || rects->capacity == 0) {
    return false;
  }

  if (rects->size < rects->capacity) {
    const uint16_t idx = rectangles_index(rects, rects->size);

    rects->data[idx] = rect;
    rects->size++;

    return true;
  }

  rects->data[rects->begin] = rect;
  rects->begin = (uint16_t)((rects->begin + 1) % rects->capacity);

  return true;
}

/**
 * @brief   Obtient le rectangle supérieur (le plus récemment ajouté).
 * @param   rects - Pointeur sur la structure Rectangles.
 * @return  const Rectangle* - Pointeur sur le rectangle supérieur, ou NULL si
 * la pile est vide.
 * @note    Le pointeur retourné pointe vers des données internes et ne doit pas
 * être modifié.
 */
const Rectangle *rectangles_top(const Rectangles *rects) {
  if (rects == NULL || rects->data == NULL || rects->size == 0) {
    return NULL;
  }

  const uint16_t idx = rectangles_index(rects, (uint16_t)(rects->size - 1));

  return &rects->data[idx];
}

/**
 * @brief   Calcule l'intersection de deux rectangles.
 * @param   a - Premier rectangle.
 * @param   b - Deuxième rectangle.
 * @param   out - Rectangle de sortie contenant l'intersection.
 * @return  bool - Vrai si les rectangles se croisent, faux sinon.
 * @details Le rectangle de sortie représente la région qui se chevauche.
 *          Une intersection valide nécessite y_0 < y_1.
 */
bool rectangle_intersection(const Rectangle *a, const Rectangle *b,
                            Rectangle *out) {
  if (a == NULL || b == NULL || out == NULL) {
    return false;
  }

  out->y_0 = (a->y_0 > b->y_0) ? a->y_0 : b->y_0;
  out->y_1 = (a->y_1 < b->y_1) ? a->y_1 : b->y_1;

  return out->y_0 < out->y_1;
}

/**
 * @brief   Calcule l'intersection du rectangle supérieur avec un autre
 * rectangle.
 * @param   rects - Pointeur sur la structure Rectangles.
 * @param   other - L'autre rectangle à croiser.
 * @param   out - Rectangle de sortie contenant l'intersection.
 * @return  bool - Vrai si l'intersection existe, faux sinon.
 * @note    Un wrapper pratique autour de rectangle_intersection().
 */
bool rectangles_top_intersection(const Rectangles *rects,
                                 const Rectangle *other, Rectangle *out) {
  const Rectangle *top = rectangles_top(rects);

  if (top == NULL) {
    return false;
  }

  return rectangle_intersection(top, other, out);
}

/**
 * @brief   Ajoute un rectangle à la pile s'il croise avec le rectangle
 * supérieur.
 * @param   rects - Pointeur sur la structure Rectangles.
 * @param   rect - Le rectangle à tester et potentiellement ajouter.
 * @return  RectAddResult - Indique le succès, pas d'intersection ou erreur.
 * @details Si le rectangle croise avec le sommet de la pile, l'intersection
 * est ajoutée comme un nouveau rectangle et peut être capturée à l'itération
 * suivante.
 * @note    C'est la fonction principale de logique de jeu pour déterminer les
 * captures réussies.
 */
RectAddResult rectangles_push_if_intersect(Rectangles *rects,
                                           const Rectangle *rect) {
  if (rects == NULL || rect == NULL) {
    return RECT_ADD_ERROR;
  }

  Rectangle inter;

  if (!rectangles_top_intersection(rects, rect, &inter)) {
    return RECT_ADD_NONE;
  }

  if (!rectangles_push(rects, inter)) {
    return RECT_ADD_ERROR;
  }

  return RECT_ADD_SUCCESS;
}

/**
 * @brief   Dessine tous les rectangles de la pile sur l'affichage.
 * @param   rects - Pointeur sur la structure Rectangles.
 * @return  void
 * @details Affiche chaque rectangle à sa position horizontale correspondante,
 *          créant la pile visuelle des pièces capturées.
 */
void rectangles_draw_all(const Rectangles *rects) {
  if (rects == NULL || rects->data == NULL || RECTANGLE_DEFAULT_HEIGHT == 0) {
    return;
  }

  for (uint16_t i = 0; i < rects->size; i++) {
    uint16_t idx = rectangles_index(rects, i);
    const Rectangle *rect = &rects->data[idx];

    uint16_t x0 = i * RECTANGLE_DEFAULT_HEIGHT;
    uint16_t x1 = x0 + RECTANGLE_DEFAULT_HEIGHT - 1;
    ILI9341_DrawFilledRectangle(x0, rect->y_0, x1, rect->y_1,
                                ILI9341_COLOR_CYAN);
  }
}
