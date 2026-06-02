/**
 * @file    flash_storage.h
 * @brief   Interface de stockage en mémoire flash pour les données de jeu
 * persistantes.
 * @details Fournit les fonctions pour lire et écrire les meilleurs scores pour
 * chaque niveau de difficulté dans la mémoire flash du microcontrôleur.
 */

#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include "game.h"
#include <stdint.h>

/** @brief Adresse de mémoire flash pour l'émulation EEPROM (page 63). */
#define FLASH_EEPROM_ADDR 0x0801F800

/** @brief Numéro de page flash contenant les données EEPROM. */
#define FLASH_PAGE_63 63

/**
 * @brief   Écrit le meilleur score d'un niveau de difficulté dans la mémoire
 * flash.
 * @param   difficulty - Le niveau de difficulté à mettre à jour.
 * @param   score - Le score à stocker.
 * @return  void
 * @details Met à jour le meilleur score pour la difficulté spécifiée et
 * préserve les scores des autres niveaux de difficulté. Efface et reprogramme
 * la page entière de flash pour maintenir la cohérence des données.
 * @note    Disposition de la page flash 63 :
 *          - Décalage 0x00 : Nombre magique (0xDEADBEEF, 4 octets)
 *          - Décalage 0x08 : Meilleur score mode Facile (4 octets)
 *          - Décalage 0x10 : Meilleur score mode Moyen (4 octets)
 *          - Décalage 0x18 : Meilleur score mode Difficile (4 octets)
 */
void flash_write_best_score(GameDifficulty difficulty, uint32_t score);

/**
 * @brief   Lit le meilleur score d'un niveau de difficulté depuis la mémoire
 * flash.
 * @param   difficulty - Le niveau de difficulté à lire.
 * @return  uint32_t - Le meilleur score, ou 0 si la flash n'est pas initialisée
 * ou corrompue.
 * @details Valide le nombre magique et la plage de scores avant de retourner la
 * valeur. Si la mémoire flash n'a pas été initialisée, retourne 0.
 * @note    Les scores supérieurs à 9999 sont traités comme une corruption et
 * retournent 0.
 */
uint32_t flash_read_best_score(GameDifficulty difficulty);

#endif
