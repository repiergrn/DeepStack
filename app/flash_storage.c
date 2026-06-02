/**
 * @file    flash_storage.c
 * @brief   Implémentation du stockage en mémoire flash.
 * @details Implémente le stockage persistant des meilleurs scores dans la
 * mémoire flash du microcontrôleur en utilisant la technique d'émulation
 * EEPROM.
 */

#include "flash_storage.h"
#include "stm32g4xx_hal.h"

/** @brief Nombre magique pour identifier les données flash valides. */
#define MAGIC 0xDEADBEEF

/**
 * @brief   Obtient le décalage de la mémoire flash pour un niveau de difficulté
 * donné.
 * @param   difficulty - Le niveau de difficulté.
 * @return  uint32_t - Décalage en octets à partir de FLASH_EEPROM_ADDR.
 * @details Les niveaux de difficulté sont stockés à :
 *          - EASY (Facile) : décalage 8
 *          - MEDIUM (Moyen) : décalage 16
 *          - HARD (Difficile) : décalage 24
 * @note    Fonction d'assistance interne.
 */
static uint32_t get_offset(GameDifficulty difficulty) {
  if (difficulty == DIFFICULTY_EASY)
    return 8;
  if (difficulty == DIFFICULTY_MEDIUM)
    return 16;
  return 24;
}

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
uint32_t flash_read_best_score(GameDifficulty difficulty) {
  uint32_t magic = *((volatile uint32_t *)(FLASH_EEPROM_ADDR));

  if (magic != MAGIC) {
    return 0;
  }

  uint32_t score =
      *((volatile uint32_t *)(FLASH_EEPROM_ADDR + get_offset(difficulty)));

  if (score > 9999) {
    return 0;
  }

  return score;
}

/**
 * @brief   Écrit le meilleur score d'un niveau de difficulté dans la mémoire
 * flash.
 * @param   difficulty - Le niveau de difficulté à mettre à jour.
 * @param   score - Le score à stocker.
 * @return  void
 * @details Met à jour le meilleur score pour la difficulté spécifiée et
 * préserve les scores des autres niveaux de difficulté. Efface et reprogramme
 * la page entière de flash pour maintenir la cohérence des données.
 * @note    Cette fonction désactive temporairement la protection flash, modifie
 * la page entière et réactive ensuite la protection.
 */
void flash_write_best_score(GameDifficulty difficulty, uint32_t score) {
  uint32_t easy = flash_read_best_score(DIFFICULTY_EASY);
  uint32_t medium = flash_read_best_score(DIFFICULTY_MEDIUM);
  uint32_t hard = flash_read_best_score(DIFFICULTY_HARD);

  if (difficulty == DIFFICULTY_EASY)
    easy = score;
  else if (difficulty == DIFFICULTY_MEDIUM)
    medium = score;
  else
    hard = score;

  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef erase;
  erase.TypeErase = FLASH_TYPEERASE_PAGES;
  erase.Banks = FLASH_BANK_1;
  erase.Page = FLASH_PAGE_63;
  erase.NbPages = 1;

  uint32_t error = 0;
  HAL_FLASHEx_Erase(&erase, &error);

  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_EEPROM_ADDR,
                    (uint64_t)MAGIC);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_EEPROM_ADDR + 8,
                    (uint64_t)easy);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_EEPROM_ADDR + 16,
                    (uint64_t)medium);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_EEPROM_ADDR + 24,
                    (uint64_t)hard);

  HAL_FLASH_Lock();
}
