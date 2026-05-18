#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include <stdint.h>
#include "game.h"

#define FLASH_EEPROM_ADDR  0x0801F800
#define FLASH_PAGE_63      63

void flash_write_best_score(GameDifficulty difficulty, uint32_t score);
uint32_t flash_read_best_score(GameDifficulty difficulty);

#endif
