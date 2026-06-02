/**
 * @file    rectangle.h
 * @brief   Interface de gestion des rectangles et de la pile.
 * @details Fournit les structures de données et les fonctions pour gérer une
 * pile dynamique de rectangles utilisés dans le jeu Stack, y compris la
 * détection de collision et le rendu.
 */

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <stdbool.h>

#include "tft_ili9341/stm32g4_ili9341.h"

/** @brief Capacité par défaut de la pile de rectangles. */
#define RECTANGLES_DEFAULT_CAPACITY 18

/** @brief Coordonnée Y initiale pour le rectangle par défaut. */
#define RECTANGLE_DEFAULT_Y0 70

/** @brief Taille (largeur) par défaut des rectangles. */
#define RECTANGLE_DEFAULT_SIZE 100

/** @brief Couleur par défaut pour les rectangles. */
#define RECTANGLE_DEFAULT_COLOR ILI9341_COLOR_BLACK

/** @brief Hauteur (épaisseur) par défaut des rectangles. */
#define RECTANGLE_DEFAULT_HEIGHT 10

/** @brief Nombre de rectangles par défaut créés à l'initialisation. */
#define RECTANGLES_DEFAULT_COUNT 4

/** @brief Hauteur totale de tous les rectangles empilés possibles. */
#define RECTANGLES_TOTAL_HEIGHT                                                \
  (RECTANGLE_DEFAULT_HEIGHT * RECTANGLES_DEFAULT_CAPACITY)

/**
 * @struct  Rectangle
 * @brief   Représente un seul rectangle avec une étendue verticale.
 * @details Stocke les coordonnées Y (haut et bas) d'un rectangle.
 */
typedef struct {
  /** @brief Coordonnée Y supérieure du rectangle. */
  uint16_t y_0;
  /** @brief Coordonnée Y inférieure du rectangle. */
  uint16_t y_1;
} Rectangle;

/**
 * @struct  Rectangles
 * @brief   File circulaire de rectangles (pile de pièces capturées).
 * @details Gère une pile dynamique de rectangles avec une sémantique de tampon
 * circulaire pour une gestion efficace de la mémoire.
 */
typedef struct {
  /** @brief Capacité totale allouée. */
  uint16_t capacity;
  /** @brief Nombre actuel de rectangles dans la pile. */
  uint16_t size;
  /** @brief Index du rectangle le plus ancien (premier de la file circulaire).
   */
  uint16_t begin;
  /** @brief Pointeur vers les données de rectangle allouées dynamiquement. */
  Rectangle *data;
} Rectangles;

/**
 * @enum    RectAddResult
 * @brief   Codes de résultat pour les opérations d'ajout de rectangles.
 */
typedef enum {
  /** @brief Pas d'intersection, rectangle non ajouté. */
  RECT_ADD_NONE,
  /** @brief Rectangle ajouté avec succès. */
  RECT_ADD_SUCCESS,
  /** @brief Erreur lors de l'ajout du rectangle. */
  RECT_ADD_ERROR
} RectAddResult;

/**
 * @brief   Initialise la pile de rectangles.
 * @param   rects - Pointeur sur la structure Rectangles à initialiser.
 * @return  bool - Vrai si l'initialisation a réussi, faux sinon.
 * @details Alloue de la mémoire pour la pile de rectangles et crée l'ensemble
 * par défaut des rectangles initiaux.
 * @note    Doit être appelée avant toute autre fonction de rectangles.
 */
bool rectangles_init(Rectangles *rects);

/**
 * @brief   Libère toutes les ressources allouées à la pile de rectangles.
 * @param   rects - Pointeur sur la structure Rectangles à libérer.
 * @return  void
 * @note    Après l'appel de cette fonction, la structure de rectangles n'est
 * plus valide jusqu'à la réinitialisation.
 */
void rectangles_free(Rectangles *rects);

/**
 * @brief   Réinitialise la pile de rectangles à son état initial.
 * @param   rects - Pointeur sur la structure Rectangles à réinitialiser.
 * @return  void
 * @details Efface la pile et recrée l'ensemble par défaut des rectangles
 * initiaux. N'utilise pas la mémoire allouée.
 */
void rectangles_reset(Rectangles *rects);

/**
 * @brief   Ajoute un rectangle à la pile.
 * @param   rects - Pointeur sur la structure Rectangles.
 * @param   rect - Le rectangle à ajouter.
 * @return  bool - Vrai si le rectangle a été ajouté, faux en cas d'erreur.
 * @details Gère la sémantique du tampon circulaire. Quand la pile est pleine,
 *          les anciens rectangles sont écrasés.
 */
bool rectangles_push(Rectangles *rects, Rectangle rect);

/**
 * @brief   Obtient le rectangle supérieur (le plus récemment ajouté).
 * @param   rects - Pointeur sur la structure Rectangles.
 * @return  const Rectangle* - Pointeur sur le rectangle supérieur, ou NULL si
 * la pile est vide.
 * @note    Le pointeur retourné pointe vers des données internes et ne doit pas
 * être modifié.
 */
const Rectangle *rectangles_top(const Rectangles *rects);

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
                            Rectangle *out);

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
                                 const Rectangle *other, Rectangle *out);

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
                                           const Rectangle *rect);

/**
 * @brief   Dessine tous les rectangles de la pile sur l'affichage.
 * @param   rects - Pointeur sur la structure Rectangles.
 * @return  void
 * @details Affiche chaque rectangle à sa position horizontale correspondante,
 *          créant la pile visuelle des pièces capturées.
 */
void rectangles_draw_all(const Rectangles *rects);

#endif
