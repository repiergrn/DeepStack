#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include <stdbool.h>

#include "rectangle.h"

typedef enum {
	STATE_INIT,
    STATE_MENU,
    STATE_GAME,
    STATE_SCORE
} GameState;

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} GameDifficulty;

typedef struct {
    uint16_t rect_y;
    uint16_t rect_x;
    uint16_t rect_length;

    uint16_t height;

    uint16_t score;

    bool toright;

    Rectangles rects;
} Game;

bool game_init(Game* game);
void game_free(Game* game);
void game_reset(Game* game);
void game_play(Game* game, GameDifficulty difficulty);

#endif /* GAME_H_ */
