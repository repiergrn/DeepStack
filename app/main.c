#include "board.h"
#include "game.h"
#include "menu.h"
#include "rectangle.h"

#include "stm32g4_gpio.h"

#include <stdbool.h>

int main(void) {
	GameState state = STATE_INIT;
	Game game;
	GameDifficulty difficulty = DIFFICULTY_MEDIUM;

	for(;;) {
		switch(state) {
		    case STATE_INIT:
		        board_init();
		        if (!game_init(&game)) {
		            while (1);
		        }
		        state = STATE_MENU;
		        break;
		    case STATE_MENU:
		        difficulty = show_game_menu();
		        state = STATE_GAME;
		        break;
		    case STATE_GAME:
		        game_play(&game, difficulty);
		        state = STATE_SCORE;
		        break;
		    case STATE_SCORE:
		        show_score_screen(&game, difficulty);
		        game_reset(&game);
		        state = STATE_MENU;
		        break;
		}
	}
}
