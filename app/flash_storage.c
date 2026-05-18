#include "flash_storage.h"
#include "stm32g4xx_hal.h"

#define MAGIC 0xDEADBEEF

// Offset par difficulté (en bytes depuis FLASH_EEPROM_ADDR)
// On stocke par paires de double word (8 bytes chacune)
// 0x00 : magic (4 bytes) + padding (4 bytes)
// 0x08 : score EASY (4 bytes) + padding (4 bytes)
// 0x10 : score MEDIUM (4 bytes) + padding (4 bytes)
// 0x18 : score HARD (4 bytes) + padding (4 bytes)

static uint32_t get_offset(GameDifficulty difficulty) {
    if (difficulty == DIFFICULTY_EASY)        return 8;
    if (difficulty == DIFFICULTY_MEDIUM)      return 16;
    return 24;
}

uint32_t flash_read_best_score(GameDifficulty difficulty) {
    uint32_t magic = *((volatile uint32_t*)(FLASH_EEPROM_ADDR));

    if (magic != MAGIC) {
        return 0;
    }

    uint32_t score = *((volatile uint32_t*)(FLASH_EEPROM_ADDR + get_offset(difficulty)));

    // Valeur impossible = Flash corrompue
    if (score > 9999) {
        return 0;
    }

    return score;
}

void flash_write_best_score(GameDifficulty difficulty, uint32_t score) {
    // Lire les scores existants
    uint32_t easy   = flash_read_best_score(DIFFICULTY_EASY);
    uint32_t medium = flash_read_best_score(DIFFICULTY_MEDIUM);
    uint32_t hard   = flash_read_best_score(DIFFICULTY_HARD);

    // Mettre à jour
    if (difficulty == DIFFICULTY_EASY)        easy   = score;
    else if (difficulty == DIFFICULTY_MEDIUM) medium = score;
    else                                       hard   = score;

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase;
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks     = FLASH_BANK_1;
    erase.Page      = FLASH_PAGE_63;
    erase.NbPages   = 1;

    uint32_t error = 0;
    HAL_FLASHEx_Erase(&erase, &error);

    // magic
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_EEPROM_ADDR,      (uint64_t)MAGIC);
    // easy
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_EEPROM_ADDR + 8,  (uint64_t)easy);
    // medium
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_EEPROM_ADDR + 16, (uint64_t)medium);
    // hard
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_EEPROM_ADDR + 24, (uint64_t)hard);

    HAL_FLASH_Lock();
}
